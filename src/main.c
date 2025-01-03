#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>

#include "./ui.h"

#ifdef CFG
enum { COLOR_BG, COLOR_HL, COLOR_BORDER, COLOR_STRINGS, COLOR_QUERY };
#else
#include "./config.h"



// out_capacity is needed for freeing the allocated memory
static char **get_strings(
    size_t *out_strcount,
    size_t *out_capacity,
    bool filter_duplicates
) {

    /* Dynarray */
    size_t size     = 0;
    size_t capacity = 5;
    char **items    = malloc(capacity * sizeof(char*));

    for (size_t i=0; i < capacity; ++i) {
        items[i] = malloc(BUFSIZ * sizeof(char));
        memset(items[i], '\0', BUFSIZ);
    }

    char buf[BUFSIZ] = { 0 };
    while (fgets(buf, BUFSIZ, stdin) != NULL) {

        buf[strcspn(buf, "\n")] = '\0';

        /* Remove duplicate entries */
        /* `-e` will show the same index for duplicate entries, */
        /* hence we are remove duplicates to prevent unexpected behaviour */
        bool is_duplicate = false;
        for (size_t i=0; i < size; ++i)
            if (!strcmp(items[i], buf))
                is_duplicate = true;

        if (is_duplicate && filter_duplicates)
            continue;

        /* Reallocating dynarray */
        if (size == capacity) {
            capacity *= 2;
            items = realloc(items, capacity * sizeof(char*));
            for (size_t i=capacity/2; i < capacity; ++i) {
                items[i] = malloc(BUFSIZ * sizeof(char));
                memset(items[i], '\0', BUFSIZ);
            }
        }

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


static inline void print_usage(char **argv) {
    fprintf(stderr, "Usage: %s [-e] [-u] [-v] [-h]\n", argv[0]);
}

typedef struct {
    bool print_index;
    bool filter_duplicates;
} Args;


static void parse_args(int argc, char **argv, Args *args) {
    const char *optstr = "evhu";
    int opt = 0;
    while ((opt = getopt(argc, argv, optstr)) != -1) {
        switch (opt) {
            case 'e': {
                args->print_index = true;
            } break;
            case 'v': {
                printf("XMenu 1.0\n");
                exit(1);
            } break;
            case 'u': {
                args->filter_duplicates = true;
            } break;
            case 'h': {
                print_usage(argv);
                exit(1);
            } break;
            default: {
                exit(1);
            } break;
        }
    }
}

/*
    TODO: find a proper name for the application
    possible names:
    - imm
    - lake
    - flow
*/




int main(int argc, char **argv) {

    Args args = { false };
    parse_args(argc, argv, &args);

    size_t strings_len      = 0;
    size_t strings_capacity = 0;
    char **strings = get_strings(&strings_len, &strings_capacity, args.filter_duplicates);

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
        cursorbar_width,
        text_spacing,
        border_width,
        scrollbar_width,
        scrollbar_height,
        width_ratio,
        wrapping,
        case_sensitive,
        scroll_next_page,
        show_scrollbar,
        show_animations,
        show_matchcount,
        args.print_index
    );

    menu_run(&menu);
    menu_destroy(&menu);
    free_strings(strings, strings_capacity);

    return EXIT_SUCCESS;

}

#endif // CFG
