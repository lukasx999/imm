#ifndef _UI_H
#define _UI_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <X11/Xlib.h>
#include <X11/X.h>
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
    } x;

    struct {
        bool wrapping;
        int text_spacing;
        int padding_x, padding_y;
        int cursor_width;
        int scrollbar_width, scrollbar_height;
        XftColor color_strings, color_query, color_hl;
    } opts;

    const int window_width, window_height;
    Matches matches;
    char query[QUERY_MAXLEN];
    int cursor;
    int scroll_offset;
    bool quit;
    XftFont *font;

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
    int text_spacing,
    int border_width,
    int scrollbar_width,
    int scrollbar_height,
    float ratio,
    bool wrapping
);
extern void menu_destroy(Menu *menu);
extern void menu_run(Menu *menu);



#endif // _UI_H
