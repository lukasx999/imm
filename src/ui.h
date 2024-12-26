#ifndef _UI_H
#define _UI_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <X11/Xlib.h>
#include <X11/X.h>


typedef struct {
    Display *dpy;
    Window root;
    int scr;
    Colormap cmap;
    GC gc;
    Window win;
} App;


extern App app_new(const char *color_bg, const char *color_border);
extern void app_destroy(App *app);
extern void app_loop(App *app);





#endif // _UI_H
