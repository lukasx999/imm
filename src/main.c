#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <dirent.h>

#include "./ui.h"

#ifdef CFG
enum { COLOR_BG, COLOR_HL, COLOR_BORDER, COLOR_STRINGS, COLOR_QUERY };
#else
#include "./config.h"



// out_capacity is needed for freeing the allocated memory
static char **get_strings(size_t *out_strcount, size_t *out_capacity) {

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


typedef struct {
    bool run_desktop;
} Args;


static void parse_args(int argc, char **argv, Args *args) {
    const char *optstr = "vhd";
    int opt = 0;
    while ((opt = getopt(argc, argv, optstr)) != -1) {
        switch (opt) {
            case 'd': {
                args->run_desktop = true;
            } break;
            case 'v': {
                printf("imm 1.0\n");
                exit(0);
            } break;
            case 'h': {
                fprintf(stderr, "Usage: %s [-v] [-h]\n", argv[0]);
                exit(0);
            } break;
            default: {
                exit(1);
            } break;
        }
    }
}



typedef struct {
    char exec[BUFSIZ];
    char name[BUFSIZ];
} DesktopEntry;


static DesktopEntry get_entry_from_file(const char *filename) {
}


static void run_desktop(void) {

    const char *appdir = "/usr/share/applications/";
    DIR *dir = opendir(appdir);
    assert(dir != NULL);

    struct dirent *entry = NULL;
    while ((entry = readdir(dir)) != NULL) {
        DesktopEntry desktopentry = get_entry_from_file(entry->d_name);
    }

    closedir(dir);

}




int main(int argc, char **argv) {

    Args args = { false };
    parse_args(argc, argv, &args);

    // TODO: this
    if (args.run_desktop)
        assert(!"todo!");

    size_t strings_len      = 0;
    size_t strings_capacity = 0;
    char **strings = get_strings(&strings_len, &strings_capacity);

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
        show_matchcount
    );

    int exit_code = menu_run(&menu);
    menu_destroy(&menu);
    free_strings(strings, strings_capacity);

    return exit_code;

}

#endif // CFG
