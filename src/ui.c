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



Menu menu_new(
    const char **strings,
    size_t strings_len,
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
    XftColorAllocName(dpy, vis, cmap, color_query,   &xft_color_query);
    XftColorAllocName(dpy, vis, cmap, color_hl,      &xft_color_hl);
    /* -------- */

    Matches matches = matches_init(strings, strings_len);

    Menu menu = {
        .matches          = matches,
        .cursor           = 0,
        .query            = { 0 },
        .quit             = false,
        .window_height    = height,
        .window_width     = width,
        .dpy              = dpy,
        .root             = root,
        .scr_num          = scr_num,
        .scr              = scr,
        .cmap             = cmap,
        .gc               = gc,
        .win              = win,
        .vis              = vis,
        .text_spacing     = text_spacing,
        .font             = font,
        .xft_drawctx      = xft_draw_ctx,
        .color_strings    = xft_color_strings,
        .color_query      = xft_color_query,
        .color_hl         = xft_color_hl,
    };
    return menu;

}



void menu_destroy(Menu *menu) {
    XftDrawDestroy(menu->xft_drawctx);
    XftColorFree(menu->dpy, menu->vis, menu->cmap, &menu->color_strings);
    XftColorFree(menu->dpy, menu->vis, menu->cmap, &menu->color_hl);
    XftColorFree(menu->dpy, menu->vis, menu->cmap, &menu->color_query);
    XftFontClose(menu->dpy, menu->font);
    XFreeColormap(menu->dpy, menu->cmap);
    XFreeGC(menu->dpy, menu->gc);
    XDestroyWindow(menu->dpy, menu->win);
    XCloseDisplay(menu->dpy);
}



static void render_string(const Menu *menu, int x, int y, const char *str, const XftColor *color) {
    XftDrawStringUtf8(
        menu->xft_drawctx, color, menu->font,
        x, y,
        (FcChar8 *) str, strlen(str)
    );
}






static unsigned short get_font_height(const Menu *menu) {
    XGlyphInfo extents = { 0 };
    // using 'X' because its the tallest character
    XftTextExtents8(menu->dpy, menu->font, (FcChar8 *) "X", strlen("X"), &extents);
    return extents.height;
}

static unsigned short get_font_width(const Menu *menu) {
    XGlyphInfo extents = { 0 };
    XftTextExtents8(menu->dpy, menu->font, (FcChar8 *) "X", strlen("X"), &extents);
    return extents.width;
}





static void render_ui(const Menu *menu) {

    int offset_x = 15;

    /* Cursor */
    XftDrawRect(
        menu->xft_drawctx,
        &menu->color_query,
        offset_x + strlen(menu->query) * get_font_width(menu),
        0,
        2,
        get_font_height(menu)*2
    );
    /* -------- */

    /* Query */
    render_string(
        menu,
        offset_x,
        get_font_height(menu)*1.5,
        menu->query,
        &menu->color_query
    );
    /* ----- */

    // TODO: add vertical bar before cursorline like fzf
    // TODO: show matches count at the right in the textbox

    int string_height = get_font_height(menu)*2 + menu->text_spacing;

    if (menu->matches.sorted_len != 0) {
        /* Cursorline */
        XftDrawRect(
            menu->xft_drawctx,
            &menu->color_hl,
            offset_x,
            string_height + string_height * menu->cursor,
            menu->window_width,
            get_font_height(menu)*2
        );
        /* -------- */
    }

    /* Strings */
    for (size_t i=0; i < menu->matches.sorted_len; ++i) {
        render_string(
            menu,
            offset_x,
            string_height + string_height * i + get_font_height(menu) * 1.5,
            menu->matches.sorted[i],
            &menu->color_strings
        );
    }
    /* ------- */

}


static void handle_keypress(Menu *menu, XKeyEvent *key_event) {

    KeySym sym     = XLookupKeysym(key_event, 1);
    uint32_t state = key_event->state;

    if (state & ControlMask) {

        switch (sym) {
            case XK_N: {
                menu->cursor++;
                if ((size_t) menu->cursor > menu->matches.sorted_len-1)
                    menu->cursor = 0;
            } break;
            case XK_P: {
                menu->cursor--;
                if (menu->cursor < 0)
                    menu->cursor = menu->matches.sorted_len-1;
            } break;
            case XK_U: {
                strncpy(menu->query, "", QUERY_MAXLEN);
            } break;
            case XK_C: {
                menu->quit = true;
            } break;
        }

    } else {

        switch (sym) {
            case XK_Return: {
                puts(menu->matches.sorted[menu->cursor]);
                menu->quit = true;
            } break;
            case XK_Escape: {
                menu->quit = true;
            } break;
            case XK_BackSpace: {
                menu->query[strcspn(menu->query, "\0")-1] = '\0';
            } break;
            default: {
                if (strlen(menu->query) + 1 == QUERY_MAXLEN)
                    break;

                char buf[64] = { 0 };
                XLookupString(key_event, buf, sizeof buf, NULL, NULL);
                strcat(menu->query, buf);

            } break;
        }

    }

}


static void handle_event(Menu *menu, XEvent *event) {

    switch (event->type) {
        case KeyPress: {
            XKeyEvent key_event = event->xkey;
            handle_keypress(menu, &key_event);
        } break;

        default: {} break;
    }

}


void menu_run(Menu *menu) {

    XEvent event = { 0 };
    while (!menu->quit) {
        XNextEvent(menu->dpy, &event);
        XClearWindow(menu->dpy, menu->win);
        matches_sort(&menu->matches, menu->query);

        // prevent cursor from clipping out of bounds when the amount of matches is reduced
        if ((size_t) menu->cursor >= menu->matches.sorted_len)
            menu->cursor = menu->matches.sorted_len-1;

        XGrabKeyboard(
            menu->dpy,
            menu->root,
            true,
            GrabModeAsync,
            GrabModeAsync,
            CurrentTime
        );

        render_ui(menu);
        handle_event(menu, &event);

    }
}


