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
    } x;
    Matches matches;
    char query[QUERY_MAXLEN];
    int cursor;
    bool quit;
    int window_width, window_height;
    int text_spacing;
    XftFont *font;
    XftDraw *xft_drawctx;
    XftColor color_strings;
    XftColor color_query;
    XftColor color_hl;
    float anim_x;
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
    int text_spacing,
    int border_width, float ratio
);
extern void menu_destroy(Menu *menu);
extern void menu_run(Menu *menu);



#endif // _UI_H
