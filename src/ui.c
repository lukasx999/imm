#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

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
    int position_x,
    int position_y,
    int padding_x,
    int padding_y,
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
    int width  = ratio * XWidthOfScreen(scr) - border_width*2;
    int height = XHeightOfScreen(scr) - border_width*2;

    int valuemask = CWOverrideRedirect | CWBackPixel | CWBorderPixel | CWEventMask;
    XSetWindowAttributes winattr = {
        .override_redirect = true,
        .background_pixel  = get_color(dpy, cmap, color_bg),
        .border_pixel      = get_color(dpy, cmap, color_border),
        .event_mask        = KeyPressMask | PointerMotionMask | ExposureMask,
    };

    XClassHint class_hint = { "xmenu", "xmenu" };

    Window win = XCreateWindow(
        dpy, root,
        position_x, position_y,
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
        .x.dpy            = dpy,
        .x.root           = root,
        .x.scr_num        = scr_num,
        .x.scr            = scr,
        .x.cmap           = cmap,
        .x.gc             = gc,
        .x.win            = win,
        .x.vis            = vis,
        .matches          = matches,
        .cursor           = 0,
        .padding_x        = padding_x,
        .padding_y        = padding_y,
        .query            = { 0 },
        .quit             = false,
        .window_height    = height,
        .window_width     = width,
        .text_spacing     = text_spacing,
        .font             = font,
        .xft_drawctx      = xft_draw_ctx,
        .color_strings    = xft_color_strings,
        .color_query      = xft_color_query,
        .color_hl         = xft_color_hl,
        .anim_x           = 0.0f,
    };
    return menu;

}

void menu_destroy(Menu *menu) {
    XCloseDisplay(menu->x.dpy);
}



static void draw_string(const Menu *menu, int x, int y, const char *str, const XftColor *color) {
    XftDrawStringUtf8(
        menu->xft_drawctx, color, menu->font,
        x, y,
        (FcChar8 *) str, strlen(str)
    );
}

static unsigned short get_font_height(const Menu *menu) {
    XGlyphInfo extents = { 0 };
    // using 'X' because its the tallest character
    XftTextExtents8(menu->x.dpy, menu->font, (FcChar8 *) "X", strlen("X"), &extents);
    return extents.height;
}

static unsigned short get_font_width(const Menu *menu) {
    XGlyphInfo extents = { 0 };
    XftTextExtents8(menu->x.dpy, menu->font, (FcChar8 *) "X", strlen("X"), &extents);
    return extents.width;
}

static void render_ui(const Menu *menu) {

    /* Cursor */
    XftDrawRect(
        menu->xft_drawctx,
        &menu->color_query,
        menu->padding_x + strlen(menu->query) * get_font_width(menu),
        menu->padding_y,
        2,
        get_font_height(menu)*2
    );
    /* -------- */

    /* Query */
    draw_string(
        menu,
        menu->padding_x,
        menu->padding_y + get_font_height(menu)*1.5,
        menu->query,
        &menu->color_query
    );
    /* ----- */

    // TODO: add vertical bar before cursorline like fzf
    // TODO: show matches count at the right in the textbox (5/10)

    int string_height = get_font_height(menu)*2 + menu->text_spacing;

    if (menu->matches.sorted_len != 0) {
        /* Cursorline */
        XftDrawRect(
            menu->xft_drawctx,
            &menu->color_hl,
            menu->padding_x,
            menu->padding_y + string_height + string_height * menu->cursor,
            menu->window_width,
            get_font_height(menu)*2
        );
        /* -------- */
    }



    /* Strings */
    for (size_t i=0; i < menu->matches.sorted_len; ++i) {
        draw_string(
            menu,
            menu->padding_x,
            menu->padding_y + string_height + string_height * i + get_font_height(menu) * 1.5,
            menu->matches.sorted[i],
            &menu->color_strings
        );
    }
    /* ------- */

}

static void cursor_inc(Menu *menu) {
    menu->cursor++;
    if ((size_t) menu->cursor > menu->matches.sorted_len-1)
        menu->cursor = 0;
}


static void cursor_dec(Menu *menu) {
    menu->cursor--;
    if (menu->cursor < 0)
        menu->cursor = menu->matches.sorted_len-1;
}

static void query_clear(Menu *menu) {
    menu->cursor = 0;
    memset(menu->query, 0, QUERY_MAXLEN);
}

static void delchar(Menu *menu) {
    menu->cursor = 0;
    menu->query[strlen(menu->query)-1] = '\0';
}

static void insert(Menu *menu, XKeyEvent *key_event) {
    if (strlen(menu->query) + 1 == QUERY_MAXLEN)
        return;

    char buf[64] = { 0 };
    XLookupString(key_event, buf, sizeof buf, NULL, NULL);
    strcat(menu->query, buf);
}

static void select_entry(Menu *menu) {
    if (menu->matches.sorted_len == 0) {
        puts(menu->query);
    } else {
        puts(menu->matches.sorted[menu->cursor]);
    }
    menu->quit = true;
}

static void handle_keypress(Menu *menu, XKeyEvent *key_event) {
    KeySym sym     = XLookupKeysym(key_event, 1);
    uint32_t state = key_event->state;

    if (state & ControlMask) {
        switch (sym) {
            case XK_N:
                cursor_inc(menu);
                break;
            case XK_P:
                cursor_dec(menu);
                break;
            case XK_U:
                query_clear(menu);
                break;
            case XK_C:
                menu->quit = true;
                break;
        }
    } else {
        switch (sym) {
            case XK_Return:
                select_entry(menu);
                break;
            case XK_Escape:
                menu->quit = true;
                break;
            case XK_BackSpace:
                delchar(menu);
                break;
            default:
                insert(menu, key_event);
                break;
        }
    }

}


static void handle_pointer_motion(Menu *menu, XMotionEvent *motion_event) {
    // TODO: this

    int x = motion_event->x;
    int y = motion_event->y;

    int offset_x = 15;
    int string_height = get_font_height(menu)*2 + menu->text_spacing;

    for (size_t i=0; i < menu->matches.sorted_len; ++i) {

        bool match_x = x >= offset_x && x <= offset_x + menu->window_width;
        int yo = string_height + string_height * i;
        bool match_y = y >= yo && y <= yo + get_font_height(menu)*2;

        if (match_x && match_y) {
            XClearWindow(menu->x.dpy, menu->x.win);
            render_ui(menu);
            XftDrawRect(
                menu->xft_drawctx,
                &menu->color_hl,
                offset_x,
                string_height + string_height * i,
                menu->window_width,
                get_font_height(menu)*2
            );
        }

    }

}

static void handle_event(Menu *menu, XEvent *event) {
    switch (event->type) {

        case KeyPress: {
            handle_keypress(menu, &event->xkey);
        } break;

        case MotionNotify: {
            // handle_pointer_motion(menu, &event->xmotion);
        } break;

        case Expose:
        case KeyRelease: break;

        default: {
            printf("event: %d\n", event->type);
            assert(!"unknown event");
        } break;
    }
}

void menu_run(Menu *menu) {

    XEvent event = { 0 };
    while (!menu->quit) {

        if (XPending(menu->x.dpy)) {
            XNextEvent(menu->x.dpy, &event);
            XClearWindow(menu->x.dpy, menu->x.win);
            matches_sort(&menu->matches, menu->query);

            // prevent cursor from clipping out of bounds when the amount of matches is reduced
            if ((size_t) menu->cursor >= menu->matches.sorted_len)
                menu->cursor = 0;

            XGrabKeyboard(
                menu->x.dpy,
                menu->x.root,
                true,
                GrabModeAsync,
                GrabModeAsync,
                CurrentTime
            );

            render_ui(menu);
            handle_event(menu, &event);

        } else {

            // TODO: this
            // sinf(menu->anim_x) * menu->window_width;


        }

    }
}


