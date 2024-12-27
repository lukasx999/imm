#ifndef _UI_H
#define _UI_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xft/Xft.h>



typedef struct {
    // Input
    const char **input;
    size_t input_count;

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
    XftDraw *xft_draw_ctx;
    XftColor xft_color;
} App;

extern App app_new(const char **input, size_t input_count,
                   const char *color_bg, const char *color_border,
                   const char *color_text,
                   const char *font_name,
                   int text_spacing,
                   int border_width, float ratio);
extern void app_destroy(App *app);
extern void app_loop(App *app);



#endif // _UI_H
