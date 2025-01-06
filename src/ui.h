#ifndef _UI_H
#define _UI_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include "./sort.h"



#define QUERY_MAXLEN BUFSIZ


typedef struct {

    struct {
        Display *dpy;
        Window root;
        int scr_num;
        Screen *scr;
        Colormap cmap;
        GC gc;
        Window win;
        Visual *vis;
        XftDraw *xft_drawctx;
        XIC xic;
    } x; // Xlib handles

    struct {
        bool wrapping;
        bool case_sensitive;
        bool scroll_next_page;
        bool show_scrollbar;
        bool show_matchcount;
        int text_spacing;
        int padding_x, padding_y;
        int cursor_width;
        int cursorbar_width;
        int scrollbar_width, scrollbar_height;
        XftFont *font;
        XftColor color_strings, color_query, color_hl;
    } opts; // User-defined configuration

    /* State of the UI */
    bool is_cursor_anim_done;
    bool show_cursor;
    const int window_width, window_height;
    Matches matches;
    char query[QUERY_MAXLEN]; // Input field
    int cursor;
    int scroll_offset;
    bool quit;
    int exit_code;

} Menu;

extern Menu menu_new(
    const char **strings, size_t strings_len,
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
    float ratio,
    bool wrapping,
    bool case_sensitive,
    bool scroll_next_page,
    bool show_scrollbar,
    bool show_animations,
    bool show_matchcount
);
extern void menu_destroy(Menu *menu);
extern int menu_run(Menu *menu);



#endif // _UI_H
