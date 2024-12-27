#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>
#include <string.h>

#include "./ui.h"
#include "./sort.h"



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
    const char **strings,
    size_t strings_count,
    const char *color_bg,
    const char *color_hl,
    const char *color_border,
    const char *color_strings,
    const char *color_query,
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

    XSetClassHint(dpy, win, &class_hint);
    XMapRaised(dpy, win);
    /* ----------- */

    /* Init Xft */
    XftFont *font         = XftFontOpenName(dpy, scr_num, font_name);
    XftDraw *xft_draw_ctx = XftDrawCreate(dpy, win, vis, cmap);

    XftColor xft_color_strings = { 0 };
    XftColor xft_color_query   = { 0 };
    XftColor xft_color_hl      = { 0 };
    XftColorAllocName(dpy, vis, cmap, color_strings, &xft_color_strings);
    XftColorAllocName(dpy, vis, cmap, color_query, &xft_color_query);
    XftColorAllocName(dpy, vis, cmap, color_hl, &xft_color_hl);
    /* -------- */

    App app = {
        .cursor        = 0,
        .query         = { 0 },
        .quit          = false,
        .window_height = height,
        .window_width  = width,
        .strings       = strings,
        .strings_len   = strings_count,
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
        .xft_drawctx   = xft_draw_ctx,
        .color_strings = xft_color_strings,
        .color_query   = xft_color_query,
        .color_hl      = xft_color_hl,
    };
    return app;

}



void app_destroy(App *app) {
    XftDrawDestroy(app->xft_drawctx);
    XftColorFree(app->dpy, app->vis, app->cmap, &app->color_strings);
    XftFontClose(app->dpy, app->font);
    XFreeColormap(app->dpy, app->cmap);
    XFreeGC(app->dpy, app->gc);
    XDestroyWindow(app->dpy, app->win);
    XCloseDisplay(app->dpy);
}



static void render_string(const App *app, int x, int y, const char *str, const XftColor *color) {
    XftDrawStringUtf8(
        app->xft_drawctx, color, app->font,
        x, y,
        (FcChar8 *) str, strlen(str)
    );
}



static void handle_keypress(App *app, XKeyEvent *key_event) {

    KeySym sym     = XLookupKeysym(key_event, 1);
    uint32_t state = key_event->state;

    if (state & ControlMask) {

        switch (sym) {
            case XK_N: {
                app->cursor++;
                if (app->cursor > (int) app->strings_len-1)
                    app->cursor = 0;
            } break;
            case XK_P: {
                app->cursor--;
                if (app->cursor < 0)
                    app->cursor = app->strings_len-1;
            } break;
            case XK_U: {
                strncpy(app->query, "", QUERY_MAXLEN);
            } break;
            case XK_C: {
                app->quit = true;
            } break;
        }

    } else {

        switch (sym) {
            case XK_Return: {
                printf("%s\n", app->strings[app->cursor]);
                app->quit = true;
            } break;
            case XK_Escape: {
                app->quit = true;
            } break;
            case XK_BackSpace: {
                app->query[strcspn(app->query, "\0")-1] = '\0';
            } break;
            default: {
                if (strlen(app->query)+1 == QUERY_MAXLEN)
                    break;
                char buf[50] = { 0 };
                XLookupString(key_event, buf, 50, NULL, NULL);
                strcat(app->query, buf);
            } break;
        }

    }

}



static unsigned short get_font_height(const App *app) {
    XGlyphInfo extents = { 0 };
    // using 'X' because its the tallest character
    XftTextExtents8(app->dpy, app->font, (FcChar8 *) "X", strlen("X"), &extents);
    return extents.height;
}

static unsigned short get_font_width(const App *app) {
    XGlyphInfo extents = { 0 };
    XftTextExtents8(app->dpy, app->font, (FcChar8 *) "X", strlen("X"), &extents);
    return extents.width;
}



static void render_ui(const App *app) {

    int string_spacing = get_font_height(app) + app->text_spacing;

    /* Cursor */
    XftDrawRect(
        app->xft_drawctx,
        &app->color_query,
        strlen(app->query) * get_font_width(app),
        0,
        2,
        get_font_height(app)
    );
    /* -------- */

    /* Cursorline */
    XftDrawRect(
        app->xft_drawctx,
        &app->color_hl,
        0,
        string_spacing + app->cursor * string_spacing,
        app->window_width,
        get_font_height(app)
    );
    /* -------- */

    /* Query */
    render_string(
        app,
        0,
        get_font_height(app),
        app->query,
        &app->color_query
    );
    /* ----- */

    /* Strings */
    int strings_offset = string_spacing;

    for (size_t i=0; i < app->strings_len; ++i) {
        const char *s = app->strings[i];
        render_string(
            app,
            0,
            strings_offset + (string_spacing + string_spacing * i) - app->text_spacing, s,
            &app->color_strings
        );
    }
    /* ------- */

}



void app_loop(App *app) {

    XEvent ev = { 0 };
    while (!app->quit) {
        XNextEvent(app->dpy, &ev);

        XClearWindow(app->dpy, app->win);
        fuzzy_sort(app->query, app->strings, app->strings_len);

        XGrabKeyboard(
            app->dpy,
            app->root,
            true,
            GrabModeAsync,
            GrabModeAsync,
            CurrentTime
        );

        render_ui(app);

        switch (ev.type) {
            case KeyPress: {
                XKeyEvent key_event = ev.xkey;
                handle_keypress(app, &key_event);
            } break;

            default: {} break;
        }

    }
}
