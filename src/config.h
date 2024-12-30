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
// static const char *font_name = "Roboto:size=14";
static int position_x       = 0;
static int position_y       = 0; // 32
static int padding_x        = 15;
static int padding_y        = 15;
static int cursor_width     = 2;
static int text_spacing     = 10;
static int border_width     = 1;
static int scrollbar_width  = 5;
static int scrollbar_height = 100;
static float width_ratio    = 0.17f;
static bool wrapping        = true;
static bool case_sensitive  = false;
