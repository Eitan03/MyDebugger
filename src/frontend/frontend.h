#ifndef FRONTEND_H_INCL
#define FRONTEND_H_INCL

#include <stdbool.h>

#ifndef MY_WINDOW_BORDER_COLOR
#define MY_WINDOW_BORDER_COLOR TB_GREEN
#endif
#ifndef MY_WINDOW_TITLE_FLAGS
#define MY_WINDOW_TITLE_FLAGS (TB_UNDERLINE | TB_BOLD | TB_RED)
#endif

#define MY_WINDOW_LAYOUT_TYPE_VERTICAL 1
#define MY_WINDOW_LAYOUT_TYPE_GRID 2

struct Window
{
    unsigned int posX;
    unsigned int posY;
    unsigned int width;
    unsigned int height;
    const char *title;
    const char **texts;
    unsigned int textsNum;
    const unsigned int layout_type;
    void *layoutParams;
};

struct my_windowLayoutVerticalParams
{
    bool isLinesNumbered;
};

struct my_windowLayoutGridParams
{
    unsigned int horizontal_lines;
};

void drawWindow(struct Window *window);

#endif
