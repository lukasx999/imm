#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>

#include "./ui.h"
#include "./fuzzy.h"




int main(void) {

    const char *query = "foobarbaz";
    const char *strings[] = { "foo", "bar", "baz" };
    size_t strcount = sizeof strings / sizeof *strings;
    /*

    fuzzy_sort(query, strings, strcount);

    for (size_t i=0; i < strcount; ++i) {
        puts(strings[i]);
    }
    */


    App app = app_new(
        strings,
        strcount,
        "#1a1a1a",
        "#2e2e2e",
        "#dba204",
        "JetBrainsMono Nerd Font:size=20",
        1,
        0.3f
    );
    app_loop(&app);
    app_destroy(&app);

    return EXIT_SUCCESS;

}
