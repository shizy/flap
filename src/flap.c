/*
 * Words! book-words! what are you?
 * Words no more, for hearken and see,
 * My song is there in the open air - and I must sing,
 * With the banner and pennant a-flapping.
 *
 * "The Song of the Banner at Daybreak" (st. II) - Walt Whitman
 */

#include <xcb/xcb_icccm.h>
#include <xcb/xcb_ewmh.h>
#include <xcb/randr.h>
#include <xcb/xcb.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

xcb_connection_t *c;
xcb_ewmh_connection_t *ewmh;
xcb_window_t root, active_window, target_window;
char term[64], *format = 0, *monitor = 0,
     type, wp, hp, xp, yp, xa, ya, vis;

typedef struct {
    int w, h, x, y, cx, cy;
} Geometry;

int
find_target_window () {

    xcb_query_tree_cookie_t qcookie = xcb_query_tree(c, root);
    xcb_query_tree_reply_t *qreply;

    if ((qreply = xcb_query_tree_reply(c, qcookie, NULL))) {

        xcb_window_t *children = xcb_query_tree_children(qreply);

        for (int i = 0; i < qreply->children_len; i++) {

            xcb_ewmh_get_utf8_strings_reply_t name;
            xcb_icccm_get_wm_class_reply_t class;
            xcb_icccm_get_text_property_reply_t instance;

            switch (type) {
                case 'i':
                    if (xcb_icccm_get_wm_name_reply(c, xcb_icccm_get_wm_name(c, children[i]), &instance, NULL) == 1 &&
                        !strcmp(term, instance.name)) {
                        target_window = children[i];
                        return 1;
                    }
                    break;
                case 'n':
                    if (xcb_ewmh_get_wm_name_reply(ewmh, xcb_ewmh_get_wm_name(ewmh, children[i]), &name, NULL) == 1 &&
                        !strcmp(term, name.strings)) {
                        target_window = children[i];
                        return 1;
                    }
                    break;
                case 'c':
                    if (xcb_icccm_get_wm_class_reply(c, xcb_icccm_get_wm_class(c, children[i]), &class, NULL) == 1 &&
                        !strcmp(term, class.class_name)) {
                        target_window = children[i];
                        return 1;
                    }
                    break;
            }
        }
        free(qreply);
    }

    return 0;
}

uint8_t
get_window_visibility (xcb_window_t *w) {

    xcb_get_window_attributes_reply_t *reply = xcb_get_window_attributes_reply(c, xcb_get_window_attributes(c, *w), NULL);
    return reply->map_state;
}

int
get_window_geometry (xcb_window_t *w, Geometry *g) {

    xcb_get_geometry_reply_t *geo;

    if ((geo = xcb_get_geometry_reply(c, xcb_get_geometry(c, *w), NULL)) == NULL) {
        printf("Error getting window geometry\n");
        return 0;
    }
    g->x = geo->x;
    g->y = geo->y;
    g->w = geo->width;
    g->h = geo->height;
    g->cx = geo->x + (geo->width / 2);
    g->cy = geo->y + (geo->height / 2);

    free(geo);
    return 1;
}

int
get_monitor_by_point (int cx, int cy, Geometry *mon) {

    xcb_randr_get_screen_resources_current_reply_t *res;
    if ((res = xcb_randr_get_screen_resources_current_reply(c, xcb_randr_get_screen_resources_current(c, root), NULL)) == NULL) {
        printf("Error randr unable to query screen resources\n");
        return 0;
    }

    int num = xcb_randr_get_screen_resources_current_outputs_length(res);
    xcb_randr_output_t *outputs = xcb_randr_get_screen_resources_current_outputs(res);
    xcb_randr_get_output_info_cookie_t cookies[num];

    for (int i = 0; i < num; i++) {
        cookies[i] = xcb_randr_get_output_info(c, outputs[i], XCB_CURRENT_TIME);
    }

    xcb_randr_get_output_info_reply_t *reply;

    for (int i = 0; i < num; i++) {

        if ((reply = xcb_randr_get_output_info_reply(c, cookies[i], NULL)) != NULL && reply->crtc != XCB_NONE) {

            if (monitor) {
                char *oname = (char *)xcb_randr_get_output_info_name(reply);
                oname[reply->name_len] = 0; //fix odd characters being appended to the end of the name
                if (strcmp(oname, monitor)) continue;
            }

            xcb_randr_get_crtc_info_reply_t *crtc;
            if ((crtc = xcb_randr_get_crtc_info_reply(c, xcb_randr_get_crtc_info(c, reply->crtc, XCB_CURRENT_TIME), NULL)) != NULL) {

                if ((cx >= crtc->x && cx < (crtc->x + crtc->width)) &&
                    (cy >= crtc->y && cy < (crtc->y + crtc->height))) {
                    mon->x = crtc->x;
                    mon->y = crtc->y;
                    mon->w = crtc->width;
                    mon->h = crtc->height;
                    mon->cx = crtc->x + (crtc->width / 2);
                    mon->cy = crtc->y + (crtc->height / 2);
                    return 1;
                }
            }
            free(crtc);
        }
    }

    free(reply);
    return 0;
}

void
calculate_target_geometry (Geometry *def, Geometry *win, Geometry *mon, Geometry *fin) {

    int xoffset, yoffset = 0;
    if (def->w > -1) fin->w  = (wp) ? (mon->w * def->w / 100) : def->w;
    else             fin->w  = win->w;
    fin->cx = fin->w / 2;
    if (def->h > -1) fin->h  = (hp) ? (mon->h * def->h / 100) : def->h;
    else             fin->h  = win->h;
    fin->cy = fin->h / 2;
    xoffset = (xp) ? (mon->w * def->x / 100) : def->x;
    yoffset = (yp) ? (mon->h * def->y / 100) : def->y;

    switch (xa) {
        case 'c': fin->x = xoffset + mon->cx - fin->cx; break;
        case 'r': fin->x = mon->x + xoffset + mon->w  - fin->w;  break;
        case 'l': fin->x = mon->x + xoffset; break;
        case 'x': fin->x = win->x + xoffset; break;
    }

    switch (ya) {
        case 'c': fin->y = yoffset + mon->cy - fin->cy; break;
        case 'b': fin->y = mon->y + yoffset + mon->h  - fin->h;  break;
        case 't': fin->y = mon->y + yoffset; break;
        case 'y': fin->y = win->y + yoffset; break;
    }
}

int
format_string(char *format, Geometry *mon, Geometry *fin) {

    int len = strlen(format), j = 0;
    char *output = (char *)malloc(strlen(format) * 64);
    memset(output, 0, strlen(format) * 64);

    for (int i = 0; i < len; i++) {

        if (format[i] == '%') {
            if (i++ == (len-1)) return 0;

            switch(format[i]) {
                case '%': sprintf(output + strlen(output), "%c", '%'); break;
                case 'd': sprintf(output + strlen(output), "0x%08x", target_window); break;
                case 'D': sprintf(output + strlen(output), "%i", target_window); break;
                case 'r': sprintf(output + strlen(output), "0x%08x", root); break;
                case 'R': sprintf(output + strlen(output), "%i", root); break;
                case 'x': sprintf(output + strlen(output), "%i", fin->x); break;
                case 'X': sprintf(output + strlen(output), "%i", mon->x); break;
                case 'y': sprintf(output + strlen(output), "%i", fin->y); break;
                case 'Y': sprintf(output + strlen(output), "%i", mon->y); break;
                case 'w': sprintf(output + strlen(output), "%i", fin->w); break;
                case 'W': sprintf(output + strlen(output), "%i", mon->w); break;
                case 'h': sprintf(output + strlen(output), "%i", fin->h); break;
                case 'H': sprintf(output + strlen(output), "%i", mon->h); break;
                case 'a': sprintf(output + strlen(output), "%i", fin->cx); break;
                case 'A': sprintf(output + strlen(output), "%i", mon->cx); break;
                case 'b': sprintf(output + strlen(output), "%i", fin->cy); break;
                case 'B': sprintf(output + strlen(output), "%i", mon->cy); break;
            }

            j = strlen(output);
        } else {
            output[j++] = format[i];
        }
    }
    printf("%s", output);
    free(output);
    return 1;
}

int
main (int argc, char *argv[]) {

    Geometry def = { -1, -1, 0, 0, 0, 0 };
    int option;

    while ((option = getopt(argc, argv, "s:w:h:x:y:m:f:vit")) != -1) {
        switch (option) {
            case 's':
                if (!sscanf(optarg, "%1[cin]:%s", &type, term)) {
                    printf("Syntax error: invalid search parameter\n");
                    return 1;
                }
                break;
            case 'w':
                if (!sscanf(optarg, "%d%1[%]", &def.w, &wp)) {
                    printf("Syntax error: invalid width parameter\n");
                    return 1;
                }
                break;
            case 'h':
                if (!sscanf(optarg, "%d%1[%]", &def.h, &hp)) {
                    printf("Syntax error: invalid height parameter\n");
                    return 1;
                }
                break;
            case 'x':
                if (!sscanf(optarg, "%[lcrx]%d%1[%]", &xa, &def.x, &xp)) {
                    printf("Syntax error: invalid x parameter\n");
                    return 1;
                }
                break;
            case 'y':
                if (!sscanf(optarg, "%[tcby]%d%1[%]", &ya, &def.y, &yp)) {
                    printf("Syntax error: invalid y parameter\n");
                    return 1;
                }
                break;
            case 'm': monitor = optarg; break;
            case 'f': format = optarg; break;
            case 'v': vis = 'v'; break;
            case 'i': vis = 'i'; break;
            case 't': vis = 't'; break;
        }
    }

    if (argc <= 1) {
        printf("Usage:\n");
        printf("flap [-s SEARCHTYPE:SEARCHTERM] [-w WIDTH[%%]] [-h HEIGHT[%%]] [-x XANCHOR[+|-OFFSET[%%]]] [-y YANCHOR[+|-OFFSET[%%]]] [-v|-i|-t] [-m MONITORNAME] [-f \"FORMATSTRING\"]\n\n");
        printf("Description:\nA window size/position tool to consistently place the same window in different sized monitors; or to calculate the size/position of a window prior to creation.\n\n");
        printf("Options:\n\n");
        printf(" -s SEARCHTYPE:SEARCHTERM\n");
        printf("\tSEARCHTYPE\t - 'i', 'c' or 'n' to search for a window by instance, class, or name respectively.\n");
        printf("\tSEARCHTERM\t - the window instance, name, or class to search by.\n\n");
        printf(" -w WIDTH[%%]\n");
        printf("\t WIDTH\t\t - the desired width of the window in pixels. If the optional '%%' sign is specified,\n\t\t\t   the width is calulated relative to the size of target monitor.\n");
        printf(" -h HEIGHT[%%]\n");
        printf("\t HEIGHT\t\t - the desired height of the window in pixels. If the optional '%%' sign is specified,\n\t\t\t   the height is calulated relative to the size of target monitor.\n");
        printf(" -x XANCHOR[+|-OFFSET[%%]]\n");
        printf("\t XANCHOR\t - 'x', 'l', 'c', 'r' for current x position, left, center, and right respectively.\n\t\t\t   The x anchor represents the alignment of the chosen edge of the window to that same\n\t\t\t   chosen edge of the monitor\n");
        printf("\t XOFFSET\t - the optional offset distance for a chosen anchor. This will distance the window's\n\t\t\t   anchor edge from the target monitor's anchor edge. If the optional '%%' sign is\n\t\t\t   specified, the distance is calculated relative to the width of the target monitor.\n");
        printf(" -y YANCHOR[+|-OFFSET[%%]]\n");
        printf("\t YANCHOR\t - 'y', 't', 'c', 'b' for current y position, top, center, and bottom respectively.\n\t\t\t   The y anchor represents the alignment of the chosen edge of the window to that same\n\t\t\t   chosen edge of the monitor\n");
        printf("\t YOFFSET\t - the optional offset distance for a chosen anchor. This will distance the window's\n\t\t\t   anchor edge from the target monitor's anchor edge. If the optional '%%' sign is\n\t\t\t   specified, the distance is calculated relative to the height of the target monitor.\n");
        printf(" -v\t\t\t - show the target window.\n");
        printf(" -i\t\t\t - hide the target window.\n");
        printf(" -t\t\t\t - toggle the visibility the target window.\n");
        printf(" -m\t\t\t - specify a target monitor name to always perform relative calulcations against.\n");
        printf(" -f \"FORMATSTRING\"\t - If specified, does not change the size or position of the window. Instead it outputs\n\t\t\t   a format string with the calculated dimentions of the window:\n");
        printf("\t %%%%\t\t - output a %% symbol\n");
        printf("\t %%d\t\t - output the hexidecimal target window id\n");
        printf("\t %%D\t\t - output the decimal target window id\n");
        printf("\t %%r\t\t - output the hexidecimal root window id\n");
        printf("\t %%R\t\t - output the decimal root window id\n");
        printf("\t %%x\t\t - output the x position of the target window\n");
        printf("\t %%X\t\t - output the x position of the target monitor\n");
        printf("\t %%y\t\t - output the y position of the target window\n");
        printf("\t %%Y\t\t - output the y position of the target monitor\n");
        printf("\t %%w\t\t - output the width of the target window\n");
        printf("\t %%W\t\t - output the width of the target monitor\n");
        printf("\t %%h\t\t - output the height of the target window\n");
        printf("\t %%H\t\t - output the height of the target monitor\n");
        printf("\t %%a\t\t - output the x-center position of the target window\n");
        printf("\t %%A\t\t - output the x-center position of the target monitor\n");
        printf("\t %%b\t\t - output the y-center position of the target window\n");
        printf("\t %%B\t\t - output the y-center position of the target monitor\n\n");
        printf("Examples:\n\n");
        printf("Position the bottom of the target window to be 10 pixels from the top of the target monitor, and the right of the target window to be 25%% of the width of the monitor from the right edge of the monitor:\n\n");
        printf("\t flap -s n:examplewindow -x r-25%% -y t+10\n\n");
        printf("Position the center of the window to be 10 pixels left and above the center of the target monitor:\n\n");
        printf("\t flap -s n:examplewindow -x c-10 -y c-10\n\n");
        printf("Resize the window to be 5 pixels below it's current location:\n\n");
        printf("\t flap -s n:examplewindow -y y+10\n\n");
        printf("Resize a window to be 200 pixels high and 90%% of the width of the target monitor\n\n");
        printf("\t flap -s n:examplewindow -h 200 -w 90%%\n\n");
        return 0;
    }

    // setup xcb stuff
    if ((c = xcb_connect(NULL, NULL)) == NULL) {
        printf("Error making xcb connection\n");
        return 1;
    }

    xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;
    root = screen->root;
    ewmh = malloc(sizeof(xcb_ewmh_connection_t));

    if (!xcb_ewmh_init_atoms_replies(ewmh, xcb_ewmh_init_atoms(c, ewmh), NULL)) {
        printf("Error initializing ewmh atoms\n");
        return 1;
    }

    // find target window
    Geometry win = { 0, 0, 0, 0, 0, 0 };
    if (type) {
        if (!find_target_window()) {
            printf("Could not find window: %s\n", term);
            return 1;
        }

        // get target window geometry
        if (!get_window_geometry(&target_window, &win)) {
            printf("Could not get window geometry\n");
            return 1;
        }
    }

    // ensure randr extension
    if(xcb_get_extension_data(c, &xcb_randr_id)->present != 1) {
        printf("Error randr xcb extension not found\n");
        return 1;
    }

    // attempt to grab the active window and it's geometry
    Geometry awin = { 0, 0, 0, 0, 0, 0 };
    xcb_ewmh_get_active_window_reply(ewmh, xcb_ewmh_get_active_window_unchecked(ewmh, 0), &active_window, NULL);
    if (active_window) {
        get_window_geometry(&active_window, &awin);
    }

    // get active monitor
    Geometry mon = { 0, 0, 0, 0, 0, 0 };
    if (!get_monitor_by_point(awin.cx, awin.cy, &mon) && !monitor) {
        printf("Error getting active monitor\n");
        return 1;
    }

    // final calculations
    Geometry fin = { 0, 0, 0, 0, 0, 0 };
    calculate_target_geometry(&def, &win, &mon, &fin);

    // adjust visibility as desired
    if (vis && !format) {
        if (vis == 'i' || (vis == 't' && get_window_visibility(&target_window) == XCB_MAP_STATE_VIEWABLE)) {
            xcb_unmap_window(c, target_window);
        } else {
            xcb_map_window(c, target_window);
        }
    }

    // make window adjustments
    if (type && !format) {
        uint32_t geo[] = { fin.x, fin.y, fin.w, fin.h };
        xcb_configure_window(c, target_window, (XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT), geo);
    }

    // print format string
    if (format) {
        if (!format_string(format, &mon, &fin)) {
            printf("Error parsing format string\n");
            return 1;
        }
    }

    xcb_flush(c);
    xcb_ewmh_connection_wipe(ewmh);
    free(ewmh);
    xcb_disconnect(c);
    return 0;
}
