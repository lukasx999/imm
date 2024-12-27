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

    Display *dpy;
    Window root;
    int scr_num;
    Screen *scr;
    Colormap cmap;
    GC gc;
    Window win;
    Visual *vis;

    XftFont *font;
    XftDraw *xft_draw_ctx;
    XftColor xft_color;
} App;

extern App app_new(const char **input, size_t input_count,
                   const char *color_bg, const char *color_border,
                   const char *color_text,
                   const char *font_name,
                   int border_width, float ratio);
extern void app_destroy(App *app);
extern void app_loop(App *app);



#endif // _UI_H
