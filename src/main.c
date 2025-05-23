#include <stdio.h>
#include <sys/ptrace.h>

#define TB_IMPL

#include "frontend/frontend.h"
#include "getFileTextSection.h"

#ifndef my_malloc
#define my_malloc malloc
#define my_realloc realloc
#define my_free free
#endif

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    pid_t child = fork();
    if (child == 0)
    {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        printf("executing %s\n", argv[1]);
        execv(argv[1], (char *const *)argv[1]);
    }

    printf("running on process %ld\n", (long)getpid());
    // getc(stdin);

    FileTextSection textSection = getTextSectionFromMaps();

    int instructionCount = 0;

    Instruction *instructions = getInstructions((char *)textSection.start, textSection.end - textSection.start, (uint64_t)textSection.start, &instructionCount);
    const char *instructionsText[instructionCount];
    for (int i = 0; i < instructionCount; i++)
    {
        instructionsText[i] = instructions[i].text;
    }

    fe_init();

    struct my_windowLayoutVerticalParams windowLayoutParams = {.isLinesNumbered = true};
    struct my_windowLayoutGridParams windowGridParams = {.horizontal_lines = 3};

    struct Window code_window = {.posX = 0,
                                 .posY = 0,
                                 .width = fe_width() / 2,
                                 .height = fe_height(),
                                 .title = "My Window",
                                 .textsNum = instructionCount,
                                 .texts = instructionsText,
                                 .layout_type = MY_WINDOW_LAYOUT_TYPE_VERTICAL,
                                 .layoutParams = &windowLayoutParams};

    struct Window up_right_window = {.posX = code_window.width,
                                     .posY = 0,
                                     .width = fe_width() / 2,
                                     .height = fe_height() / 2,
                                     .title = "Up Right Window",
                                     .textsNum = 15,
                                     .texts = instructionsText,
                                     .layout_type = MY_WINDOW_LAYOUT_TYPE_VERTICAL,
                                     .layoutParams = &windowLayoutParams};

    struct Window bottom_right_window = {.posX = code_window.width,
                                         .posY = up_right_window.height,
                                         .width = fe_width() / 2,
                                         .height = fe_height() / 2,
                                         .title = "Bottom Right Window",
                                         .textsNum = 15,
                                         .texts = instructionsText,
                                         .layout_type = MY_WINDOW_LAYOUT_TYPE_GRID,
                                         .layoutParams = &windowGridParams};

    fe_drawWindow(&code_window);
    fe_drawWindow(&up_right_window);
    fe_drawWindow(&bottom_right_window);

    fe_present();
    fe_execute_events();

    fe_exit();

    return 0;

    return 0;
}
