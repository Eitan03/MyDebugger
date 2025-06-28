#ifndef FRONTEND_H
#define FRONTEND_H

#include "termbox2.h"
#include <stdbool.h>

#include "LinkedList/LinkedList.h"

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
    union {
        const char **array;
        LinkedList *list;
    } texts;
    bool isTextList; /* true if texts is LinkedList<char*> */
    unsigned int textsNum;
    unsigned int layout_type;
    void *layoutParams;
};

struct my_windowLayoutVerticalParams
{
    bool isLinesNumbered;
    bool isNumberedHex;
    int numberedLineStartIndex;
};

struct my_windowLayoutGridParams
{
    unsigned int horizontal_lines;
};

int fe_width(void);
int fe_height(void);

void fe_init(void);
void fe_clear(void);
void fe_present(void);
void fe_execute_events(void);
void fe_exit(void);

void fe_drawWindow(struct Window *window);

#endif
