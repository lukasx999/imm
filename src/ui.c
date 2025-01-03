#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <X11/Xutil.h>

#include "./ui.h"
#include "./sort.h"


#define ARRAY_SIZE(arr) (sizeof arr / sizeof *arr)



static unsigned short get_font_height(const Menu *menu) {
    /*
    The font height should be the same for all strings.
    Otherwise a string like "qux", will have a different font height
    than something like "FOO", which we dont want
    Using 'X' because its one of the tallest characters
    */
    XGlyphInfo extents = { 0 };
    const char *c = "X";
    XftTextExtents8(menu->x.dpy, menu->opts.font, (FcChar8 *) c, strlen(c), &extents);
    return extents.height;
}

static unsigned short get_font_width(const Menu *menu, const char *s) {
    XGlyphInfo extents = { 0 };
    XftTextExtentsUtf8(menu->x.dpy, menu->opts.font, (FcChar8 *) s, strlen(s), &extents);
    return extents.width;
}

static unsigned long get_color(Display *dpy, Colormap cmap, const char *color) {
    XColor xcolor = { 0 };
    XParseColor(dpy, cmap, color, &xcolor);
    XAllocColor(dpy, cmap, &xcolor);
    return xcolor.pixel;
}

static void draw_string(
    const Menu *m,
    int x,
    int y,
    const char *str,
    const XftColor *color,
    int lpad,
    int rpad
) {
    /* Truncating long strings */
    char buf[BUFSIZ] = { 0 };
    strcpy(buf, str);

    // BUG: not working correctly with non-monospace fonts
    bool is_too_long = get_font_width(m, buf) > m->window_width - (lpad + rpad);
    if (is_too_long) {
        const char *symbol = "...";
        float mean_charsize = (float) get_font_width(m, buf) / strlen(buf);
        size_t last_char = (m->window_width - rpad) / mean_charsize;
        strcpy(buf + last_char - strlen(symbol) - 1, symbol);
    }

    XftDrawStringUtf8(
        m->x.xft_drawctx, color, m->opts.font,
        x, y,
        (FcChar8 *) buf, strlen(buf)
    );
}

static inline int get_string_height(const Menu *m) {
    return get_font_height(m) * 2 + m->opts.text_spacing;
}

static void render_ui(Menu *m) {
    int string_height = get_string_height(m);
    int max_vis_entries = m->window_height / string_height - 1;

    // Adjust scroll offset when cursor leaves the screen
    int scroll_step = m->opts.scroll_next_page ? max_vis_entries : 1;

    if (m->cursor - m->scroll_offset >= max_vis_entries)
        m->scroll_offset += scroll_step;
    if (m->cursor - m->scroll_offset < 0)
        m->scroll_offset -= scroll_step;


    // prevent cursor from clipping out of bounds when the amount of matches is reduced
    if ((size_t) m->cursor >= m->matches.sorted_len)
        m->cursor = m->scroll_offset = 0;



    /* Cursor */
    if (m->is_cursor_anim_done && m->show_cursor) {
        XftDrawRect(
            m->x.xft_drawctx,
            &m->opts.color_query,
            m->opts.padding_x + get_font_width(m, m->query),
            m->opts.padding_y,
            m->opts.cursor_width,
            get_font_height(m)*2
        );
    }

    // anchor point of strings in Xlib is at the bottom left

    int query_offset_y = m->opts.padding_y + get_font_height(m) * 1.5;

    /* Match count */
    char matchcount[BUFSIZ] = { 0 };
    if (m->opts.show_matchcount) {
        snprintf(
            matchcount,
            ARRAY_SIZE(matchcount),
            "%lu/%lu",
            m->matches.sorted_len,
            m->matches.strings_len
        );

        draw_string(
            m,
            m->window_width - m->opts.padding_x - get_font_width(m, matchcount),
            query_offset_y,
            matchcount,
            &m->opts.color_query,
            0, 0
        );
    }


    /* Query */
    draw_string(
        m,
        m->opts.padding_x,
        query_offset_y,
        m->query,
        &m->opts.color_query,
        m->opts.padding_x,
        get_font_width(m, matchcount)
    );

    /* hide cursor if query is colliding with matchcounter */
    // BUG: not working correctly with non-monospace fonts
    m->show_cursor = get_font_width(m, m->query) < m->window_width - (m->opts.padding_x + get_font_width(m, matchcount));




    if (m->matches.sorted_len != 0) {
        int y = string_height * (m->cursor - m->scroll_offset);

        /* Cursorline */
        XftDrawRect(
            m->x.xft_drawctx,
            &m->opts.color_hl,
            m->opts.padding_x,
            m->opts.padding_y + string_height + y,
            m->window_width,
            get_font_height(m) * 2
        );

        /* Cursorbar */
        XftDrawRect(
            m->x.xft_drawctx,
            &m->opts.color_strings,
            m->opts.padding_x,
            m->opts.padding_y + string_height + y,
            m->opts.cursorbar_width,
            get_font_height(m) * 2
        );

    }


    /* Matches */
    for (size_t i=0; i < (size_t) max_vis_entries; ++i) {
        const char *item = matches_get(&m->matches, i + m->scroll_offset);
        if (item == NULL)
            break;

        int offset_x = m->opts.padding_x * 2;

        int y = string_height * i;
        draw_string(
            m,
            offset_x,
            query_offset_y + string_height + y,
            item,
            &m->opts.color_strings,
            offset_x,
            m->opts.padding_x
        );
    }

    /* Scrollbar */
    if (m->opts.show_scrollbar) {
        float current = (float) m->cursor / m->matches.sorted_len;
        int offsety   = m->opts.padding_y + string_height;
        int y         = current * (m->window_height - m->opts.scrollbar_height - offsety);
        XftDrawRect(
            m->x.xft_drawctx,
            &m->opts.color_strings,
            m->window_width - m->opts.scrollbar_width,
            offsety + y,
            m->opts.scrollbar_width,
            m->opts.scrollbar_height
        );
    }


}

static void cursor_inc(Menu *m) {
    m->cursor++;
    size_t index_max = m->matches.sorted_len-1;
    if ((size_t) m->cursor > index_max) {
        if (m->opts.wrapping)
            m->cursor = m->scroll_offset = 0;
        else
            m->cursor = index_max;
    }

}

static void cursor_dec(Menu *m) {
    int string_height = get_string_height(m);
    int max_vis_entries = m->window_height / string_height - 1;
    int max_offset = (m->matches.sorted_len / max_vis_entries) * max_vis_entries;

    m->cursor--;
    if (m->cursor < 0) {
        if (m->opts.wrapping) {
            m->cursor = m->matches.sorted_len-1;
            m->scroll_offset = max_offset;
        } else {
            m->cursor = 0;
        }
    }
}

static void query_clear(Menu *m) {
    m->cursor = 0;
    memset(m->query, 0, QUERY_MAXLEN);
    m->scroll_offset = 0;
}

static void delchar(Menu *m) {
    m->cursor = 0;
    m->query[strlen(m->query)-1] = '\0';
}

static void insert(Menu *m, XKeyEvent *key_event) {
    if (strlen(m->query) + 1 == QUERY_MAXLEN)
        return;

    char buf[4] = { 0 };
    Xutf8LookupString(m->x.xic, key_event, buf, ARRAY_SIZE(buf), NULL, NULL);
    strcat(m->query, buf);

}

static void select_entry(Menu *m) {
    const char *str = m->matches.sorted_len == 0
        ? m->query
        : m->matches.sorted[m->cursor];

    if (m->opts.print_index) {
        // Not using m->cursor, as it will change as the matches get sorted
        ssize_t index = matches_search(&m->matches, str);
        // if the element is not found, the enumerated value will be 0
        printf("%lu %s\n", index+1, str);
    } else
    puts(str);

    // TODO: return exit code 1 when selected entry is not in matches
    m->quit = true;
}


static void handle_keypress(Menu *m, XKeyEvent *key_event) {
    KeySym sym     = XLookupKeysym(key_event, 1);
    uint32_t state = key_event->state;

    if (state & ControlMask) {
        switch (sym) {
            case XK_J:
            case XK_N:
                cursor_inc(m);
                break;
            case XK_K:
            case XK_P:
                cursor_dec(m);
                break;
            case XK_L:
            case XK_F:
                // TODO: implement query cursor
                assert(!"todo: move cursor");
                break;
            case XK_H:
            case XK_B:
                assert(!"todo: move cursor");
                break;
            case XK_U:
                query_clear(m);
                break;
            case XK_C:
                m->quit = true;
                break;
        }
    } else {
        switch (sym) {
            case XK_Down:
                cursor_inc(m);
                break;
            case XK_Up:
                cursor_dec(m);
                break;
            case XK_Return:
                select_entry(m);
                break;
            case XK_Escape:
                m->quit = true;
                break;
            case XK_BackSpace:
                delchar(m);
                break;
            default:
                insert(m, key_event);
                break;
        }
    }

}

static void handle_event(Menu *m, XEvent *event) {
    switch (event->type) {

        case KeyPress: {
            m->is_cursor_anim_done = true;
            handle_keypress(m, &event->xkey);
        } break;

        case Expose:
        case KeyRelease: break;

        default: {
            printf("event: %d\n", event->type);
            assert(!"unknown event");
        } break;
    }
}

static void grab_keyboard(Menu *m) {
    struct timespec ts = { .tv_sec = 0, .tv_nsec = 1e6 };

    for (int i=0; i < 1000; ++i) {
        int ret = XGrabKeyboard(
            m->x.dpy,
            m->x.root,
            true,
            GrabModeAsync,
            GrabModeAsync,
            CurrentTime
        );
        if (ret == GrabSuccess)
            return;
        nanosleep(&ts, NULL);
    }

    fprintf(stderr, "Failed to grab keyboard\n");
    exit(1);
}

static void cursor_anim(Menu *m) {
    static float anim_x = 0.0f;

    if (m->is_cursor_anim_done)
        return;

    m->is_cursor_anim_done = anim_x >= M_PI_2;
    XClearWindow(m->x.dpy, m->x.win);
    render_ui(m);

    float i = m->window_width - m->window_width * sinf(anim_x);

    XftDrawRect(
        m->x.xft_drawctx,
        &m->opts.color_query,
        m->opts.padding_x + i,
        m->opts.padding_y,
        m->opts.cursor_width,
        get_font_height(m)*2
    );

    anim_x += 0.09f;

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
    int cursor_width,
    int cursorbar_width,
    int text_spacing,
    int border_width,
    int scrollbar_width,
    int scrollbar_height,
    float width_ratio,
    bool wrapping,
    bool case_sensitive,
    bool scroll_next_page,
    bool show_scrollbar,
    bool show_animations,
    bool show_matchcount,
    bool print_index
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
    int width  = width_ratio * XWidthOfScreen(scr) - border_width * 2;
    int height = XHeightOfScreen(scr) - border_width * 2;

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

    XIM xim = XOpenIM(dpy, NULL, NULL, NULL);
    XIC xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, NULL);

    return (Menu) {
        .x.dpy                  = dpy,
        .x.root                 = root,
        .x.scr_num              = scr_num,
        .x.scr                  = scr,
        .x.cmap                 = cmap,
        .x.gc                   = gc,
        .x.win                  = win,
        .x.vis                  = vis,
        .x.xft_drawctx          = xft_draw_ctx,
        .x.xic                  = xic,
        .opts.wrapping          = wrapping,
        .opts.case_sensitive    = case_sensitive,
        .opts.scroll_next_page  = scroll_next_page,
        .opts.show_scrollbar    = show_scrollbar,
        .opts.show_matchcount   = show_matchcount,
        .opts.print_index       = print_index,
        .opts.padding_x         = padding_x,
        .opts.padding_y         = padding_y,
        .opts.cursor_width      = cursor_width,
        .opts.text_spacing      = text_spacing,
        .opts.color_strings     = xft_color_strings,
        .opts.color_query       = xft_color_query,
        .opts.color_hl          = xft_color_hl,
        .opts.scrollbar_width   = scrollbar_width,
        .opts.scrollbar_height  = scrollbar_height,
        .opts.font              = font,
        .opts.cursorbar_width   = cursorbar_width,
        .matches                = matches,
        .cursor                 = 0,
        .scroll_offset          = 0,
        .show_cursor            = true,
        .query                  = { 0 },
        .quit                   = false,
        .window_height          = height,
        .window_width           = width,
        .is_cursor_anim_done    = !show_animations,
    };

}

void menu_run(Menu *m) {

    grab_keyboard(m);

    XEvent event = { 0 };
    while (!m->quit) {

        if (XPending(m->x.dpy)) {
            XNextEvent(m->x.dpy, &event);
            XClearWindow(m->x.dpy, m->x.win);
            matches_sort(&m->matches, m->query, m->opts.case_sensitive);

            handle_event(m, &event);
            render_ui(m);

        } else {
            cursor_anim(m);

            const int fps = 1000;
            usleep((1.0f/fps) * 10e6);
        }

    }
}

void menu_destroy(Menu *menu) {
    XCloseDisplay(menu->x.dpy);
}
