#include "./bootstrapMPT.h"
#include "./bootstrapFrontend.h"

#include <stdio.h>

mpt_context *traceeContext;

FileTextSection textSection = {};

int instructionCount;
const Instruction *instructions = NULL; /* allocated */
char **instructionsText = NULL;         /* allocated */

struct user_regs_struct *fpregs = NULL; /* allocated */

void loadNewExec()
{
    initRegisters();
    initInstructions();
    initWindows(instructionCount, instructionsText);
}

void childExecHandler()
{
    // not tested with multiple exec
    loadNewExec();
    addMessageToMessagesWindow("SIGTRAP Recieved, loaded new exec");
    mpt_setBreakpoint(traceeContext, 0x401013);

    drawFrontend();
}

void childSignalHandler(int status)
{

    struct user_regs_struct regs;
    mpt_getRegisters(traceeContext, &regs);
    mpt_regStructToText(&regs, registersText);

    if (WIFSTOPPED(status))
    {
        addMessageToMessagesWindow("Child has stopped due to signal %s", strsignal(WSTOPSIG(status)));
    }
    if (WIFSIGNALED(status))
    {
        addMessageToMessagesWindow("Child received signal %s\n", strsignal(WTERMSIG(status)));
        if (WTERMSIG(status) == SIGTRAP)
        {
            struct user_regs_struct regs;
            mpt_getRegisters(traceeContext, &regs);
            addMessageToMessagesWindow("Breakpoint at line %llx hit", regs.rip);
        }
    }

    if (WIFEXITED(status))
    {
        // used on exit, should use the childExitHandler instead since
        // some data (like registers info) is missing here
    }

    drawFrontend();
}

void childExitHandler(unsigned char status)
{
    struct user_regs_struct regs;
    mpt_getRegisters(traceeContext, &regs);
    mpt_regStructToText(&regs, registersText);

    addMessageToMessagesWindow("Child exited with code %u\n", status);
    drawFrontend();
}

void initRegisters()
{

    if (registersText == NULL)
    {
        registersText = (char **)my_realloc(registersText, sizeof(char *) * REGISTERS_NUMBER);
        validateErrno(registersText == NULL, "realloc");
        for (int i = 0; i < REGISTERS_NUMBER; i++)
        {
            registersText[i] = (char *)my_malloc(sizeof(char) * REGISTERS_TEXT_SIZE);
            validateErrno(registersText[i] == NULL, "malloc");
        }
    }

    if (fpregs == NULL)
    {
        fpregs = (struct user_regs_struct *)my_malloc(sizeof(struct user_regs_struct));
        validateErrno(fpregs == NULL, "malloc");
    }

    mpt_getRegisters(traceeContext, fpregs);
    mpt_regStructToText(fpregs, registersText);
}

void initInstructions()
{
    // TODO move to freeMPT and call it here
    if (instructions != NULL)
    {
        freeInstructions((Instruction *)instructions, instructionCount);
    }
    if (instructionsText != NULL)
    {
        if (instructionCount == 0)
        {
            printf("WARNING! freeing instructionsText but instrcutionCount is 0! cant free the insrutction strings");
        }
        for (int i = 0; i < instructionCount; i++)
        {
            my_free((void *)instructionsText[i]);
        }
        my_free(instructionsText);
    }

    textSection = getTextSectionFromMaps(mpt_getTraceePid(traceeContext));
    char *instrucitonsBinary = mpt_getDataFromProcess(
        traceeContext,
        textSection.start,
        textSection.end - textSection.start);

    instructionCount = 0;
    instructions = getInstructions(
        instrucitonsBinary,
        textSection.end -
            textSection.start - 1,
        textSection.start,
        &instructionCount);

    my_free(instrucitonsBinary);

    instructionsText = (char **)my_malloc(sizeof(char *) * instructionCount);

    validateErrno(instructionsText == NULL, "malloc");

    for (int i = 0; i < instructionCount; i++)
    {
        const size_t intructionTextLength = (10 + strlen(instructions[i].text) + 1);

        instructionsText[i] = my_malloc(sizeof(char) * intructionTextLength);
        validateErrno(instructionsText[i] == NULL, "malloc");

        snprintf(instructionsText[i], intructionTextLength, "0x%06lx: %s", instructions[i].address, instructions[i].text);
    }
}

void freeMPT()
{
    if (traceeContext != NULL)
    {
        mpt_freeContext(traceeContext);
        traceeContext = NULL;
    }

    if (fpregs != NULL)
    {
        my_free(fpregs);
        fpregs = NULL;
    }

    if (instructions != NULL)
    {
        freeInstructions((Instruction *)instructions, instructionCount);
        instructions = NULL;
    }

    if (instructionsText != NULL)
    {
        my_free(instructionsText);
        instructionsText = NULL;
    }

    if (registersText != NULL)
    {
        for (int i = 0; i < REGISTERS_NUMBER; i++)
        {
            my_free(registersText[i]);
        }
        my_free(registersText);
        registersText = NULL;
    }
}