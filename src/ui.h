#ifndef _UI_H
#define _UI_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xft/Xft.h>


#define QUERY_MAXLEN BUFSIZ


typedef struct {
    const char **strings;
    size_t strings_len;

    int cursor;

    char query[QUERY_MAXLEN];

    bool quit;

    int window_width;
    int window_height;

    Display *dpy;
    Window root;
    int scr_num;
    Screen *scr;
    Colormap cmap;
    GC gc;
    Window win;
    Visual *vis;

    int text_spacing;
    XftFont *font;
    XftDraw *xft_drawctx;
    XftColor color_strings;
    XftColor color_query;
    XftColor color_hl;
} App;

extern App app_new(
    const char **strings, size_t strings_count,
    const char *color_bg,
    const char *color_hl,
    const char *color_border,
    const char *color_strings,
    const char *color_query,
    const char *font_name,
    int text_spacing,
    int border_width, float ratio
);
extern void app_destroy(App *app);
extern void app_loop(App *app);



#endif // _UI_H
