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

-s SEARCHTYPE:SEARCHTERM
```bash
    SEARCHTYPE     - "i", "c" or "n" to search for a window by instance, class, or name respectively.
    SEARCHTERM     - the window instance, name, or class to search by.
```
 -w WIDTH[%]
 ```bash
    WIDTH          - the desired width of the window in pixels. If the optional "%" sign is specified,
                      the width is calulated relative to the size of target monitor.
```
 -h HEIGHT[%]
 ```bash
     HEIGHT        - the desired height of the window in pixels. If the optional "%" sign is specified,
                     the height is calulated relative to the size of target monitor.
```
 -x XANCHOR[+|-OFFSET[%]]
 ```bash
     XANCHOR       - "x", "l", "c", "r" for current x position, left, center, and right respectively.
                     The x anchor represents the alignment of the chosen edge of the window to that same
                     chosen edge of the monitor.
     XOFFSET       - the optional offset distance or a chosen anchor. This will distance the window's
                     anchor edge from the target monitor's anchor edge. If the optional "%" sign is
                     specified, the distance is calculated relative to the width of the target monitor.
```
 -y YANCHOR[+|-OFFSET[%]]
 ```bash
     YANCHOR       - "y", "t", "c", "b" for current y position, top, center, and bottom respectively.
                     The y anchor represents the alignment of the chosen edge of the window to that same
                     chosen edge of the monitor.
     YOFFSET       - the optional offset distance or a chosen anchor. This will distance the window's
                     anchor edge from the target monitor's anchor edge. If the optional "%" sign is
                     specified, the distance is calculated relative to the height of the target monitor.
```
 -v                - show the target window.
 -i                - hide the target window.
 -t                - toggle the visibility the target window.
 -m                - specify a target monitor name to always perform relative calulcations against.
 ```bash
 -f "FORMATSTRING" - If specified, does not change the size or position of the window. Instead it outputs
                     a format string with the calculated dimentions of the window:
         %%        - output a % symbol
         %d        - output the hexidecimal target window id
         %D        - output the decimal target window id
         %r        - output the hexidecimal root window id
         %R        - output the decimal root window id
         %x        - output the x position of the target window
         %X        - output the x position of the target monitor
         %y        - output the y position of the target window
         %Y        - output the y position of the target monitor
         %w        - output the width of the target window
         %W        - output the width of the target monitor
         %h        - output the height of the target window
         %H        - output the height of the target monitor
         %a        - output the x-center position of the target window
         %A        - output the x-center position of the target monitor
         %b        - output the y-center position of the target window
         %B        - output the y-center position of the target monitor
```

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
