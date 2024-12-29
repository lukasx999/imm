#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>

#include "./ui.h"


#ifdef CFG
enum { COLOR_BG, COLOR_HL, COLOR_BORDER, COLOR_STRINGS, COLOR_QUERY };
#else

#include "./config.h"






// out_capacity is needed for freeing the allocated memory
static char **get_strings(size_t *out_strcount, size_t *out_capacity) {

    size_t size     = 0;
    size_t capacity = 5;
    char **items    = malloc(capacity * sizeof(char*));
    for (size_t i=0; i < capacity; ++i) {
        items[i] = malloc(BUFSIZ * sizeof(char));
        memset(items[i], '\0', BUFSIZ);
    }

    char buf[BUFSIZ] = { 0 };
    while (fgets(buf, BUFSIZ, stdin) != NULL) {

        if (size == capacity) {
            capacity *= 2;
            items = realloc(items, capacity * sizeof(char*));
            for (size_t i=capacity/2; i < capacity; ++i) {
                items[i] = malloc(BUFSIZ * sizeof(char));
                memset(items[i], '\0', BUFSIZ);
            }
        }

        buf[strcspn(buf, "\n")] = '\0';
        strncpy(items[size++], buf, BUFSIZ);

    }

    *out_strcount = size;
    *out_capacity = capacity;
    return items;

}

static void free_strings(char **strings, size_t capacity) {
    for (size_t i=0; i < capacity; ++i) {
        free(strings[i]);
    }
    free(strings);
}





int main(void) {

    size_t strings_len = 0, strings_capacity = 0;
    char **strings  = get_strings(&strings_len, &strings_capacity);

    Menu menu = menu_new(
        (const char **) strings,
        strings_len,
        colors[COLOR_BG],
        colors[COLOR_HL],
        colors[COLOR_BORDER],
        colors[COLOR_STRINGS],
        colors[COLOR_QUERY],
        font_name,
        position_x,
        position_y,
        padding_x,
        padding_y,
        cursor_width,
        text_spacing,
        border_width,
        scrollbar_width,
        scrollbar_height,
        width_ratio,
        wrapping
    );

    menu_run(&menu);
    menu_destroy(&menu);
    free_strings(strings, strings_capacity);

    return EXIT_SUCCESS;

}

#endif // CFG
