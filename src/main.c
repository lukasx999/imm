#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>

#include "./ui.h"



int main(void) {

    App app = app_new("#1a1a1a", "#2e2e2e", 1, 0.3f);
    app_loop(&app);
    app_destroy(&app);

    return EXIT_SUCCESS;

}
