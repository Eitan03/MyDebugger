#include <stdio.h>

#define TB_IMPL

#include "MyPtrace.h"
#include "frontend/frontend.h"
#include "getFileTextSection.h"
#include "utils.h"

struct my_windowLayoutVerticalParams codeWindowLayoutParams;
struct my_windowLayoutVerticalParams upRightWindowLayoutParams;
struct my_windowLayoutGridParams windowGridParams;
struct Window codeWindow = {};
struct Window upRightWindow = {};
struct Window bottomRightWindow = {};

char *upRightWindowText[] = {"Welcome To My Debugger!"};

char **registersText; /* allocated */

pid_t traceePid;

struct user_regs_struct *fpregs; /* allocated */

pid_t initTracee(char *argv[])
{
    pid_t traceePid = fork();
    if (traceePid == -1)
    {
        validateErrno(traceePid, "fork");
    }

    if (traceePid == 0)
    {
        mpt_traceMe(argv[1], argv[1]);
    }

    return traceePid;
}

void initWindows(int instructionCount, const char *instructionsText[], uint64_t instructionLineStart)
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
        psignal(WSTOPSIG(status), "a");
        printf("Child has stopped due to signal %d\n", WSTOPSIG(status));
        // Child has stopped due to signal WSTOPSIG(status)
    }
    if (WIFSIGNALED(status))
    {
        psignal(WTERMSIG(status), "b");
        printf("Child received signal %d\n", WTERMSIG(status));
        // Child received signal WTERMSIG(status)
    }

    if (WIFEXITED(status))
    {
        psignal(WEXITSTATUS(status), "c");
        printf("Child exited with code %d\n", WEXITSTATUS(status));
    }
    // child exited with code WEXITSTATUS(status)

    // drawFrontend();
}

void drawFrontend()
{
    fe_drawWindow(&codeWindow);
    fe_drawWindow(&upRightWindow);
    fe_drawWindow(&bottomRightWindow);

    fe_present();
    fe_execute_events();
}

void initAllocatedVariables()
{
    registersText = my_realloc(registersText, sizeof(char *) * REGISTERS_NUMBER);
    validateErrno(registersText == NULL, "realloc");
    for (int i = 0; i < REGISTERS_NUMBER; i++)
    {
        registersText[i] = my_malloc(sizeof(char) * REGISTERS_TEXT_SIZE);
        validateErrno(registersText[i] == NULL, "malloc");
    }

    fpregs = my_malloc(sizeof(struct user_regs_struct));
    validateErrno(fpregs == NULL, "malloc");
}

int main(int argc, char *argv[])
{
    printf("running on process %ld\n", (long)getpid());
    getc(stdin);
    initAllocatedVariables();

    if (argc < 2)
    {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    traceePid = initTracee(argv);

    mpt_listenToChild(traceePid, childSignalHandler);
    FileTextSection textSection = getTextSectionFromMaps(traceePid);
    // textSection.start -= 1; // Align to page size
    char *instrucitonsBinary = mpt_getDataFromProcess(
        traceePid,
        textSection.start,
        textSection.end - textSection.start);

    mpt_getRegisters(traceePid, fpregs);
    mpt_regStructToText(fpregs, registersText);

    int instructionCount = 0;

    Instruction *instructions = getInstructions(
        instrucitonsBinary,
        textSection.end -
            textSection.start - 1,
        textSection.start,
        &instructionCount);

    my_free(instrucitonsBinary);

    const char *instructionsText[instructionCount];
    for (int i = 0; i < instructionCount; i++)
    {
        instructionsText[i] = instructions[i].text;
    }

    fe_init();

    initWindows(instructionCount, instructionsText, textSection.start);

    drawFrontend();

    fe_exit();

    return 0;
}
