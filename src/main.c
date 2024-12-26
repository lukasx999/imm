#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <X11/Xlib.h>
#include <X11/X.h>


int main(void) {

    Display *dpy = XOpenDisplay(NULL);

    Window root = DefaultRootWindow(dpy);
    int x = 0, y = 0;
    int width = 500, height = 500;
    int border_width = 5;

    int scr = DefaultScreen(dpy);
    Window win = XCreateSimpleWindow(
        dpy, root,
        x, y,
        width, height,
        border_width, 0,
        BlackPixel(dpy, scr)
    );


    XMapWindow(dpy, win);

    XSelectInput(dpy, win, KeyPressMask);

    XEvent ev = { 0 };
    while (1) {

        XNextEvent(dpy, &ev);

        switch (ev.type) {

            case KeyPress: {
            } break;

            default: {} break;
        }

    }



    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);

    return EXIT_SUCCESS;

}
