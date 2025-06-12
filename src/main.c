#include <stdio.h>

#define TB_IMPL

#include "./utils/utils.h"
#include "MyPtrace/MyPtrace.h"
#include "frontend/frontend.h"
#include "getFileTextSection.h"

struct my_windowLayoutVerticalParams codeWindowLayoutParams;
struct my_windowLayoutVerticalParams upRightWindowLayoutParams;
struct my_windowLayoutGridParams windowGridParams;
struct Window codeWindow = {};
struct Window upRightWindow = {};
struct Window bottomRightWindow = {};

char *upRightWindowText[] = {"Welcome To My Debugger!"};

mpt_context *traceeContext;

FileTextSection textSection = {};

int instructionCount;
const Instruction *instructions = NULL; /* allocated */
const char **instructionsText = NULL;   /* allocated */

char **registersText = NULL; /* allocated */

struct user_regs_struct *fpregs = NULL; /* allocated */

void initWindows(int instructionCount, const char **instructionsText, uint64_t instructionLineStart)
{

    codeWindowLayoutParams = (struct my_windowLayoutVerticalParams){
        .isLinesNumbered = true,
        .isNumberedHex = true,
        .numberedLineStartIndex = instructionLineStart};
    upRightWindowLayoutParams = (struct my_windowLayoutVerticalParams){
        .isLinesNumbered = false,
        .isNumberedHex = false,
        .numberedLineStartIndex = 0};
    windowGridParams = (struct my_windowLayoutGridParams){.horizontal_lines = 3};

    codeWindow.posX = 0;
    codeWindow.posY = 0;
    codeWindow.width = fe_width() / 2;
    codeWindow.height = fe_height();
    codeWindow.title = "My Window";
    codeWindow.textsNum = instructionCount;
    codeWindow.texts = instructionsText;
    codeWindow.layout_type = MY_WINDOW_LAYOUT_TYPE_VERTICAL;
    codeWindow.layoutParams = &codeWindowLayoutParams;

    upRightWindow.posX = codeWindow.width;
    upRightWindow.posY = 0;
    upRightWindow.width = fe_width() / 2;
    upRightWindow.height = fe_height() / 2;
    upRightWindow.title = "Up Right Window";
    upRightWindow.textsNum = 1;
    upRightWindow.texts = (const char **)upRightWindowText;
    upRightWindow.layout_type = MY_WINDOW_LAYOUT_TYPE_VERTICAL;
    upRightWindow.layoutParams = &upRightWindowLayoutParams;

    bottomRightWindow.posX = codeWindow.width;
    bottomRightWindow.posY = upRightWindow.height;
    bottomRightWindow.width = fe_width() / 2;
    bottomRightWindow.height = fe_height() / 2;
    bottomRightWindow.title = "Registers";
    bottomRightWindow.textsNum = REGISTERS_NUMBER;
    bottomRightWindow.texts = (const char **)registersText;
    bottomRightWindow.layout_type = MY_WINDOW_LAYOUT_TYPE_GRID;
    bottomRightWindow.layoutParams = &windowGridParams;
}

void childSignalHandler(int status)
{
    if (WIFSTOPPED(status))
    {
        if (SIGTRAP == WSTOPSIG(status))
        {
            initRegisters();
            initInstructions();
            initWindows(instructionCount, instructionsText, textSection.start);
        }
        psignal(WSTOPSIG(status), "a");
        // printf("Child has stopped due to signal %d\n", WSTOPSIG(status));
        // Child has stopped due to signal WSTOPSIG(status)
    }
    if (WIFSIGNALED(status))
    {
        psignal(WTERMSIG(status), "b");
        // printf("Child received signal %d\n", WTERMSIG(status));
        // Child received signal WTERMSIG(status)
    }

    if (WIFEXITED(status))
    {
        // printf("Child exited with code %d\n", WEXITSTATUS(status));
    }

    drawFrontend();
}

void drawFrontend()
{
    fe_drawWindow(&codeWindow);
    fe_drawWindow(&upRightWindow);
    fe_drawWindow(&bottomRightWindow);

    fe_present();
    fe_execute_events();
}

void initRegisters()
{

    if (registersText == NULL)
    {
        registersText = my_realloc(registersText, sizeof(char *) * REGISTERS_NUMBER);
        validateErrno(registersText == NULL, "realloc");
        for (int i = 0; i < REGISTERS_NUMBER; i++)
        {
            registersText[i] = my_malloc(sizeof(char) * REGISTERS_TEXT_SIZE);
            validateErrno(registersText[i] == NULL, "malloc");
        }
    }

    if (fpregs == NULL)
    {
        fpregs = my_malloc(sizeof(struct user_regs_struct));
        validateErrno(fpregs == NULL, "malloc");
    }

    mpt_getRegisters(traceeContext, fpregs);
    mpt_regStructToText(fpregs, registersText);
}

void initInstructions()
{
    textSection = getTextSectionFromMaps(mpt_getTraceePid(traceeContext));
    char *instrucitonsBinary = mpt_getDataFromProcess(
        traceeContext,
        textSection.start,
        textSection.end - textSection.start);

    if (instructions != NULL)
    {
        freeInstructions((Instruction *)instructions, instructionCount);
    }

    instructionCount = 0;
    instructions = getInstructions(
        instrucitonsBinary,
        textSection.end -
            textSection.start - 1,
        textSection.start,
        &instructionCount);

    my_free(instrucitonsBinary);

    if (instructionsText != NULL)
    {
        my_free(instructionsText);
    }
    instructionsText = my_malloc(sizeof(char *) * instructionCount);

    validateErrno(instructionsText == NULL, "malloc");
    for (int i = 0; i < instructionCount; i++)
    {
        instructionsText[i] = instructions[i].text;
    }
}

int main(int argc, char *argv[])
{
    printf("running on process %ld\n", (long)getpid());
    getc(stdin);

    if (argc < 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    traceeContext = mpt_initTrace(argv[1], (const char *)&(argv[2]));

    fe_init();

    mpt_listenToChild(traceeContext, childSignalHandler);

    fe_exit();

    return 0;
}
