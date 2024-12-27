#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

#include "./ui.h"
#include "./fuzzy.h"



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
    const char **input,
    size_t input_count,
    const char *color_bg,
    const char *color_border,
    const char *color_text,
    const char *font_name,
    int text_spacing,
    int border_width,
    float ratio
) {

    Display *dpy  = XOpenDisplay(NULL);
    assert(dpy != NULL);
    Window root   = XDefaultRootWindow(dpy);
    int scr_num   = XDefaultScreen(dpy);
    Colormap cmap = XDefaultColormap(dpy, scr_num);
    GC gc         = XCreateGC(dpy, root, 0, NULL);
    Screen *scr   = XScreenOfDisplay(dpy, scr_num);
    Visual *vis   = XDefaultVisual(dpy, scr_num);

    /* Init Window */
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

    // XIM im = XOpenIM(dpy, NULL, NULL, NULL);
    // assert(im != NULL);

    XSetClassHint(dpy, win, &class_hint);
    XMapRaised(dpy, win);
    /* ----------- */

    /* Init Xft */
    XftFont *font         = XftFontOpenName(dpy, scr_num, font_name);
    XftDraw *xft_draw_ctx = XftDrawCreate(dpy, win, vis, cmap);
    XftColor xft_color    = { 0 };
    XftColorAllocName(dpy, vis, cmap, color_text, &xft_color);
    /* -------- */

    App app = {
        .window_height = height,
        .window_width  = width,
        .input         = input,
        .input_count   = input_count,
        .dpy           = dpy,
        .root          = root,
        .scr_num       = scr_num,
        .scr           = scr,
        .cmap          = cmap,
        .gc            = gc,
        .win           = win,
        .vis           = vis,
        .text_spacing  = text_spacing,
        .font          = font,
        .xft_draw_ctx  = xft_draw_ctx,
        .xft_color     = xft_color,
    };
    return app;

}

void app_destroy(App *app) {
    XftDrawDestroy(app->xft_draw_ctx);
    XftColorFree(app->dpy, app->vis, app->cmap, &app->xft_color);
    XftFontClose(app->dpy, app->font);
    XFreeColormap(app->dpy, app->cmap);
    XFreeGC(app->dpy, app->gc);
    XDestroyWindow(app->dpy, app->win);
    XCloseDisplay(app->dpy);
}



static void render_string(App *app, int x, int y, const char *str) {
    XftDrawStringUtf8(
        app->xft_draw_ctx, &app->xft_color, app->font,
        x, y,
        (FcChar8 *) str, strlen(str)
    );
}


void app_loop(App *app) {

    XEvent ev = { 0 };
    while (1) {
        XNextEvent(app->dpy, &ev);
        puts("event!");


        // XSetInputFocus(app->dpy, app->win, RevertToParent, CurrentTime);
        /*
        int ret = XGrabKeyboard(
            app->dpy,
            app->root,
            true,
            GrabModeAsync,
            GrabModeAsync,
            CurrentTime
        );
        assert(ret == GrabSuccess);
        */




        set_color(app, "#363636");
        int padding = 25;
        XFillRectangle(
            app->dpy,
            app->win,
            app->gc,
            padding,
            padding,
            app->window_width - padding*2,
            app->window_height - padding*2
        );

        for (size_t i=0; i < app->input_count; ++i) {
            const char *s = app->input[i];

            XGlyphInfo extents = { 0 };
            // using 'X' because its the tallest character
            XftTextExtents8(app->dpy, app->font, (FcChar8 *) "X", strlen("X"), &extents);
            int spacing = extents.height + app->text_spacing;

            render_string(
                app,
                0,
                (spacing + spacing * i) - app->text_spacing, s
            );

        }

        switch (ev.type) {
            case KeyPress: {
                XKeyEvent key_event = ev.xkey;
                key_event.state;
                puts("key pressed!");
            } break;

            default: {} break;
        }

    }
}
