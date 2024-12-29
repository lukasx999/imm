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
static int position_x        = 0;
static int position_y        = 0; // 32
static int padding_x         = 15;
static int padding_y         = 15;
static int text_spacing      = 10;
static int border_width      = 1;
static float ratio           = 0.17f;
