#include "frontend.h"
#include "termbox2.h"

void _window_drawBorder(struct Window *window)
{
    char borderWidthText[window->width + 1];
    borderWidthText[window->width] = 0;
    memset(borderWidthText, ' ', window->width);
    tb_printf(window->posX, window->posY, 0, MY_WINDOW_BORDER_COLOR, borderWidthText);
    tb_printf(window->posX, window->posY + window->height - 1, 0, MY_WINDOW_BORDER_COLOR, borderWidthText);
    for (int i = 0; i < window->height - 2; i++)
    {
        tb_set_cell(window->posX, window->posY + 1 + i, ' ', 0, MY_WINDOW_BORDER_COLOR);
        tb_set_cell(window->posX + window->width - 1, window->posY + 1 + i, ' ', 0, MY_WINDOW_BORDER_COLOR);
    }
}

int _centerTextPos(int posX, int width, const char *text)
{
    int titleLen = strlen(text);

    if (titleLen > width)
    {
        // TODO
    }
    return (posX + (width - titleLen) / 2);
}

void _window_drawTitle(struct Window *window)
{
    tb_print(_centerTextPos(window->posX, window->width, window->title), window->posY + 2, MY_WINDOW_TITLE_FLAGS, 0,
             window->title);
}

void _window_drawVerticalLayout(struct Window *window)
{
    struct my_windowLayoutVerticalParams *params = (struct my_windowLayoutVerticalParams *)window->layoutParams;

    const int topPadding = 6;    /* 1 + 2 + 3 = border + titleLine + padding */
    const int bottomPadding = 3; /* 1 + 2 = border + padding */
    const int leftPadding = 3;   /* 1 + 2 = border + padding */
    int InitialLinePos = window->posY + topPadding;

    int curLineIdx = 0;
    while (curLineIdx < window->textsNum)
    {
        if ((curLineIdx) >= (window->height - topPadding - bottomPadding)) /* overflow bottom */
        {
            tb_print(window->posX + leftPadding, (InitialLinePos + curLineIdx), 0, 0, "...");
            break;
        }
        else
        {
            if (params->isLinesNumbered)
            {
                tb_printf(window->posX + leftPadding, (InitialLinePos + curLineIdx), 0, 0, "%03d: %s", curLineIdx + 1,
                          window->texts[curLineIdx]);
            }
            else
            {
                tb_print(window->posX + leftPadding, (InitialLinePos + curLineIdx), 0, 0, window->texts[curLineIdx]);
            }
        }

        curLineIdx++;
    }
}

void _window_drawGridLayout(struct Window *window)
{
    struct my_windowLayoutGridParams *params = (struct my_windowLayoutGridParams *)window->layoutParams;

    const int topPadding = 6;    /* 1 + 2 + 3 = border + titleLine + padding */
    const int bottomPadding = 3; /* 1 + 2 = border + padding */
    const int leftPadding = 3;   /* 1 + 2 = border + padding */
    int InitialLinePos = window->posY + topPadding;

    int curLineIdx = 0;
    int curTextIdx = 0;
    while (curTextIdx < window->textsNum)
    {
        if ((curLineIdx) >= (window->height - topPadding - bottomPadding)) /* overflow bottom */
        {
            tb_print(_centerTextPos(window->posX, window->width, "..."), (InitialLinePos + curLineIdx), 0, 0, "...");
            break;
        }
        else
        {
            int lineWidth = (window->width - leftPadding) / params->horizontal_lines;

            for (int i = 0; i < params->horizontal_lines || curTextIdx + i < window->textsNum; i++)
            {

                const char *text = window->texts[curTextIdx + i];
                int textPos = _centerTextPos(window->posX + leftPadding + (i * lineWidth), lineWidth, text);
                tb_print(textPos, (InitialLinePos + curLineIdx), 0, 0, text);
            }
            curTextIdx += 3;
            curLineIdx += 2;
        }
    }
}

void drawWindow(struct Window *window)
{
    _window_drawBorder(window);
    _window_drawTitle(window);
    switch (window->layout_type)
    {
    case MY_WINDOW_LAYOUT_TYPE_VERTICAL:
        _window_drawVerticalLayout(window);
        break;
    case MY_WINDOW_LAYOUT_TYPE_GRID:
        _window_drawGridLayout(window);
        break;

    default:
        printf("Invalid window layout %d", window->layout_type);
        exit(1);
        break;
    }
}