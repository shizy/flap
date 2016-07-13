# flap

A window size/position tool to consistently place the same window in different sized monitors; or to calculate the size/position of a window prior to creation.

## Installation

Install dependences through pacman:

```bash
$ pacman -S libxcb
```

Then clone, build, and install:

```bash
$ git clone https://github.com/shizy/flap.git
$ cd flap
$ make
$ sudo make install
```

## Parameters

flap [-s SEARCHTYPE:SEARCHTERM] [-w WIDTH[%]] [-h HEIGHT[%]] [-x XANCHOR[+|-OFFSET[%]]] [-y YANCHOR[+|-OFFSET[%]]] [-v|-i|-t] [-m MONITORNAME] [-f "FORMATSTRING"]<br/><br/>

-s SEARCHTYPE:SEARCHTERM<br/>
    SEARCHTYPE              - 'i', 'c' or 'n' to search for a window by instance, class, or name respectively.<br/>
    SEARCHTERM              - the window instance, name, or class to search by.<br/>
 -w WIDTH[%%]<br/>
     WIDTH                  - the desired width of the window in pixels. If the optional '%' sign is specified,<br/>
                              the width is calulated relative to the size of target monitor.<br/>
 -h HEIGHT[%%]<br/>
     HEIGHT                 - the desired height of the window in pixels. If the optional '%' sign is specified,<br/>
                              the height is calulated relative to the size of target monitor.<br/>
 -x XANCHOR[+|-OFFSET[%%]]<br/>
     XANCHOR                - 'x', 'l', 'c', 'r' for current x position, left, center, and right respectively.<br/>
                              The x anchor represents the alignment of the chosen edge of the window to that same<br/>
                              chosen edge of the monitor.<br/>
     XOFFSET                - the optional offset distance or a chosen anchor. This will distance the window's<br/>
                              anchor edge from the target monitor's anchor edge. If the optional '%' sign is<br/>
                              specified, the distance is calculated relative to the width of the target monitor.<br/>
 -y YANCHOR[+|-OFFSET[%%]]<br/>
     YANCHOR                - 'y', 't', 'c', 'b' for current y position, top, center, and bottom respectively.<br/>
                              The y anchor represents the alignment of the chosen edge of the window to that same<br/>
                              chosen edge of the monitor.<br/>
     YOFFSET                - the optional offset distance or a chosen anchor. This will distance the window's<br/>
                              anchor edge from the target monitor's anchor edge. If the optional '%' sign is<br/>
                              specified, the distance is calculated relative to the height of the target monitor.<br/>
 -v                         - show the target window.<br/>
 -i                         - hide the target window.<br/>
 -t                         - toggle the visibility the target window.<br/>
 -m                         - specify a target monitor name to always perform relative calulcations against.<br/>
 -f "FORMATSTRING"          - If specified, does not change the size or position of the window. Instead it outputs<br/>
                              a format string with the calculated dimentions of the window:<br/>
                 %%         - output a % symbol<br/>
                 %d         - output the hexidecimal target window id<br/>
                 %D         - output the decimal target window id<br/>
                 %r         - output the hexidecimal root window id<br/>
                 %R         - output the decimal root window id<br/>
                 %x         - output the x position of the target window<br/>
                 %X         - output the x position of the target monitor<br/>
                 %y         - output the y position of the target window<br/>
                 %Y         - output the y position of the target monitor<br/>
                 %w         - output the width of the target window<br/>
                 %W         - output the width of the target monitor<br/>
                 %h         - output the height of the target window<br/>
                 %H         - output the height of the target monitor<br/>
                 %a         - output the x-center position of the target window<br/>
                 %A         - output the x-center position of the target monitor<br/>
                 %b         - output the y-center position of the target window<br/>
                 %B         - output the y-center position of the target monitor<br/>

## Examples

Position the bottom of the target window to be 10 pixels from the top of the target monitor, and the right of the target window to be 25% of the width of the monitor from the right edge of the monitor:
```bash
$ flap -s n:examplewindow -x r-25% -y t+10
```

Position the center of the window to be 10 pixels left and above the center of the target monitor:
```bash
$ flap -s n:examplewindow -x c-10 -y c-10
```

Resize the window to be 5 pixels below it's current location:
```bash
$ flap -s n:examplewindow -y y+10
```

Resize a window to be 200 pixels high and 90% of the width of the target monitor
```bash
$ flap -s n:examplewindow -h 200 -w 90%
```
