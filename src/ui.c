#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>

#include "./ui.h"



static unsigned long get_color(Display *dpy, Colormap cmap, const char *color) {
    XColor xcolor = { 0 };
    XParseColor(dpy, cmap, color, &xcolor);
    XAllocColor(dpy, cmap, &xcolor);
    return xcolor.pixel;
}

static void set_color(App *app, const char *color) {
    unsigned long pixel = get_color(app->dpy, app->cmap, color);
    XSetForeground(app->dpy, app->gc, pixel);
}



App app_new(const char *color_bg, const char *color_border) {

    Display *dpy  = XOpenDisplay(NULL);
    Window root   = XDefaultRootWindow(dpy);
    int scr       = XDefaultScreen(dpy);
    Colormap cmap = XDefaultColormap(dpy, scr);
    GC gc         = XCreateGC(dpy, root, 0, NULL);

    int x = 0, y = 0;
    int width = 500, height = 500;
    int border_width = 5;

    unsigned long pixel_bg     = get_color(dpy, cmap, color_bg);
    unsigned long pixel_border = get_color(dpy, cmap, color_border);

    int valuemask = CWOverrideRedirect | CWBackPixel | CWBorderPixel | CWEventMask;
    XSetWindowAttributes winattr = {
        .override_redirect = true,
        .background_pixel  = pixel_bg,
        .border_pixel      = pixel_border,
        .event_mask        = KeyPressMask | ExposureMask,
    };

    XClassHint class_hint = { "xmenu", "xmenu" };

    Window win = XCreateWindow(
        dpy, root,
        x, y,
        width, height,
        border_width,
        CopyFromParent,
        CopyFromParent,
        CopyFromParent,
        valuemask,
        &winattr
    );

    XSetClassHint(dpy, win, &class_hint);
    XMapRaised(dpy, win);

    App app = {
        .dpy  = dpy,
        .root = root,
        .scr  = scr,
        .cmap = cmap,
        .gc   = gc,
        .win  = win,
    };
    return app;

}

void app_destroy(App *app) {
    XFreeColormap(app->dpy, app->cmap);
    XFreeGC(app->dpy, app->gc);
    XDestroyWindow(app->dpy, app->win);
    XCloseDisplay(app->dpy);
}





void app_loop(App *app) {

    XEvent ev = { 0 };
    while (1) {
        XNextEvent(app->dpy, &ev);
        puts("event!");

        set_color(app, "#000000");
        XDrawRectangle(app->dpy, app->win, app->gc, 50, 50, 100, 100);

        switch (ev.type) {

            case KeyPress: {
                puts("key pressed!");
            } break;

            default: {} break;
        }

    }
}
