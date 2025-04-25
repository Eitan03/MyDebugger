#define TB_IMPL
#include "termbox2.h"
#include <stdio.h>
#include "frontend/frontend.h"

#ifndef my_malloc
#define my_malloc malloc
#define my_realloc realloc
#define my_free free
#endif

int main()
{
    // printf("running on process %ld\n", (long)getpid());
    // getc(stdin);

    tb_init();

    const char *lines[] = {
        "line1", "line2",  "line3",  "line4",  "line5",  "line6",  "line7",  "line8",
        "line9", "line10", "line11", "line12", "line13", "line14", "line15",
    };

    struct my_windowLayoutVerticalParams windowLayoutParams = {.isLinesNumbered = true};
    struct my_windowLayoutGridParams windowGridParams = {.horizontal_lines = 3};

    struct Window code_window = {.posX = 0,
                                 .posY = 0,
                                 .width = tb_width() / 2,
                                 .height = tb_height(),
                                 .title = "My Window",
                                 .textsNum = 15,
                                 .texts = lines,
                                 .layout_type = MY_WINDOW_LAYOUT_TYPE_VERTICAL,
                                 .layoutParams = &windowLayoutParams};

    struct Window up_right_window = {.posX = code_window.width,
                                     .posY = 0,
                                     .width = tb_width() / 2,
                                     .height = tb_height() / 2,
                                     .title = "Up Right Window",
                                     .textsNum = 15,
                                     .texts = lines,
                                     .layout_type = MY_WINDOW_LAYOUT_TYPE_VERTICAL,
                                     .layoutParams = &windowLayoutParams};

    struct Window bottom_right_window = {.posX = code_window.width,
                                         .posY = up_right_window.height,
                                         .width = tb_width() / 2,
                                         .height = tb_height() / 2,
                                         .title = "Bottom Right Window",
                                         .textsNum = 15,
                                         .texts = lines,
                                         .layout_type = MY_WINDOW_LAYOUT_TYPE_GRID,
                                         .layoutParams = &windowGridParams};

    struct tb_event ev;
    int y = 1;

    // tb_printf(0, y++, TB_GREEN, 0, "hello from termbox");
    // tb_printf(0, y++, 0, 0, "width=%d height=%d", tb_width(), tb_height());
    drawWindow(&code_window);
    drawWindow(&up_right_window);
    drawWindow(&bottom_right_window);
    tb_present();

    tb_poll_event(&ev);
    tb_shutdown();

    return 0;

    return 0;
}
