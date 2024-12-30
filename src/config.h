#define CFG
#include "./main.c"

static const char *colors[] = {
    [COLOR_BG]      = "#1a1a1a",
    [COLOR_HL]      = "#2e2e2e",
    [COLOR_BORDER]  = "#2e2e2e",
    [COLOR_STRINGS] = "#adadad",
    [COLOR_QUERY]   = "#e3e3e3",
};

static const char *font_name = "JetBrainsMono Nerd Font:size=20";
// static const char *font_name = "Roboto:size=20";
static const char *truncation_symbol = "...";

static int position_x        = 0,
           position_y        = 0,
           padding_x         = 15,
           padding_y         = 15,
           cursor_width      = 2,
           cursorbar_width   = 2,
           text_spacing      = 10,
           border_width      = 5,
           scrollbar_width   = 5,
           scrollbar_height  = 100;

static float width_ratio = 0.17f;

static bool wrapping         = true,
            case_sensitive   = false,
            scroll_next_page = false;
