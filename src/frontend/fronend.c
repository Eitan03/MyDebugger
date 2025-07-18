#include "./frontend.h"

struct tb_event ev;

int fe_width(void)
{
    return tb_width();
}
int fe_height(void)
{
    return tb_height();
}

void fe_init(void)
{
    tb_init();
}
void fe_clear(void)
{
    tb_clear();
}
void fe_present(void)
{
    tb_present();
}
void fe_execute_events(void)
{
    tb_poll_event(&ev);
}
void fe_exit(void)
{
    tb_shutdown();
}

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
    tb_print(_centerTextPos(window->posX, window->width, window->title), window->posY + 2, MY_WINDOW_TITLE_FLAGS, 0, window->title);
}

/* returns weather overflow was reached or not*/
bool _drawTextLine(int curLineIdx, struct Window *window, const int topPadding, const int bottomPadding, const int leftPadding, int InitialLinePos, const char *text, struct my_windowLayoutVerticalParams *params)
{
    if ((curLineIdx) >= (window->height - topPadding - bottomPadding)) /* overflow bottom */
    {
        tb_print(window->posX + leftPadding, (InitialLinePos + curLineIdx), 0, 0, "...");
        return true;
    }
    else
    {
        if (params->isLinesNumbered)
        {
            tb_printf(window->posX + leftPadding, (InitialLinePos + curLineIdx), 0, 0, (params->isNumberedHex ? "0x%06x: %s" : "%06d: %s"), params->numberedLineStartIndex + curLineIdx, text);
        }
        else
        {
            tb_print(window->posX + leftPadding, (InitialLinePos + curLineIdx), 0, 0, text);
        }
    }

    return false;
}

void _window_drawVerticalLayout(struct Window *window)
{
    struct my_windowLayoutVerticalParams *params = (struct my_windowLayoutVerticalParams *)window->layoutParams;

    const int topPadding = 6;    /* 1 + 2 + 3 = border + titleLine + padding */
    const int bottomPadding = 3; /* 1 + 2 = border + padding */
    const int leftPadding = 3;   /* 1 + 2 = border + padding */
    int InitialLinePos = window->posY + topPadding;

    int curLineIdx = 0;
    LinkedNode *curNode = NULL; /* only used if texts is LinkedList */
    if (window->isTextList)
    {
        curNode = window->texts.list->first;
    }

    while (((!window->isTextList) && curLineIdx < window->textsNum) || (curLineIdx < datatypes_linkedList_length(window->texts.list)))
    {
        const char *text;
        if (window->isTextList)
        {
            text = (const char *)curNode->value;
            curNode = curNode->next;
        }
        else
        {
            text = window->texts.array[curLineIdx];
        }

        if (_drawTextLine(curLineIdx, window, topPadding, bottomPadding, leftPadding, InitialLinePos, text, params))
        {
            break; /* overflow bottom */
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
    LinkedNode *curNode = NULL; /* only used if texts is LinkedList */
    if (window->isTextList)
    {
        curNode = window->texts.list->first;
    }

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
                const char *text;
                if (window->isTextList)
                {
                    text = (const char *)curNode->value;
                    curNode = curNode->next;
                }
                else
                {
                    text = window->texts.array[curTextIdx + i];
                }
                int textPos = _centerTextPos(window->posX + leftPadding + (i * lineWidth), lineWidth, text);
                tb_print(textPos, (InitialLinePos + curLineIdx), 0, 0, text);
            }
            curTextIdx += 3;
            curLineIdx += 2;
        }
    }
}

void fe_drawWindow(struct Window *window)
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
        exit(EXIT_FAILURE);
        break;
    }
}