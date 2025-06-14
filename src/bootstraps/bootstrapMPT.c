#include "./bootstrapMPT.h"

mpt_context *traceeContext;

FileTextSection textSection = {};

int instructionCount;
const Instruction *instructions = NULL; /* allocated */
const char **instructionsText = NULL;   /* allocated */

struct user_regs_struct *fpregs = NULL; /* allocated */

void loadNewExec()
{
    initRegisters();
    initInstructions();
    initWindows(instructionCount, instructionsText, textSection.start);
}

void childSignalHandler(int status)
{
    if (WIFSTOPPED(status))
    {
        if (SIGTRAP == WSTOPSIG(status))
        {
            loadNewExec();
        }
        // printf("Child has stopped due to signal %d\n", WSTOPSIG(status));
    }
    if (WIFSIGNALED(status))
    {
        psignal(WTERMSIG(status), "b");
        // printf("Child received signal %d\n", WTERMSIG(status));
    }

    if (WIFEXITED(status))
    {
        // printf("Child exited with code %d\n", WEXITSTATUS(status));
    }

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
    instructionsText = (const char **)my_malloc(sizeof(char *) * instructionCount);

    validateErrno(instructionsText == NULL, "malloc");
    for (int i = 0; i < instructionCount; i++)
    {
        instructionsText[i] = instructions[i].text;
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