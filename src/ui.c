#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

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




App app_new(
    const char *color_bg,
    const char *color_border,
    int border_width,
    float ratio
) {

    Display *dpy  = XOpenDisplay(NULL);
    Window root   = XDefaultRootWindow(dpy);
    int scr_num   = XDefaultScreen(dpy);
    Colormap cmap = XDefaultColormap(dpy, scr_num);
    GC gc         = XCreateGC(dpy, root, 0, NULL);
    Screen *scr   = XScreenOfDisplay(dpy, scr_num);

    XftInit(NULL);
    XftFont *font = XftFontOpenName(dpy, scr_num, "JetBrainsMono Nerd Font");
    // XftDraw *xft_draw = XftDrawCreate(dpy, root, NULL, cmap);
    // XftColor *xft_color = NULL;
    // XftColorAllocName(dpy, NULL, cmap, "red", xft_color);


    int x = 0, y = 0;
    int width  = ratio * XWidthOfScreen(scr);
    int height = XHeightOfScreen(scr);

    int valuemask = CWOverrideRedirect | CWBackPixel | CWBorderPixel | CWEventMask;
    XSetWindowAttributes winattr = {
        .override_redirect = true,
        .background_pixel  = get_color(dpy, cmap, color_bg),
        .border_pixel      = get_color(dpy, cmap, color_border),
        .event_mask        = KeyPressMask | ExposureMask,
    };

    XClassHint class_hint = { "xmenu", "xmenu" };

    Window win = XCreateWindow(
        dpy, root,
        x, y,
        width, height, border_width,
        CopyFromParent,
        CopyFromParent,
        CopyFromParent,
        valuemask,
        &winattr
    );

    XSetClassHint(dpy, win, &class_hint);
    XMapRaised(dpy, win);

    App app = {
        .dpy     = dpy,
        .root    = root,
        .font    = font,
        .scr_num = scr_num,
        .scr     = scr,
        .cmap    = cmap,
        .gc      = gc,
        .win     = win,
    };
    return app;

}

void app_destroy(App *app) {
    XftFontClose(app->dpy, app->font);
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

        set_color(app, "#ffffff");
        XDrawString(app->dpy, app->win, app->gc, 0, 150, "foo", 3);

        // XftDrawStringUtf8(

        switch (ev.type) {

            case KeyPress: {
                puts("key pressed!");
            } break;

            default: {} break;
        }

    }
}
