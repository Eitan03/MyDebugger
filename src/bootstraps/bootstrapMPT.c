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

void childSignalHandler(int status)
{
    char *msg;
    const size_t msg_buffer_len = 128;
    if (WIFSTOPPED(status))
    {
        if (SIGTRAP == WSTOPSIG(status))
        {
            loadNewExec();
        }
        // printf("Child has stopped due to signal %d\n", WSTOPSIG(status));
        msg = (char *)my_malloc(msg_buffer_len * sizeof(char));
        snprintf(msg, msg_buffer_len, "Child has stopped due to signal %d", WSTOPSIG(status));
        addMessageToMessagesWindow(msg);
    }
    if (WIFSIGNALED(status))
    {
        // psignal(WTERMSIG(status), "b");
        // printf("Child received signal %d\n", WTERMSIG(status));
    }

    if (WIFEXITED(status))
    {
        // printf("Child exited with code %d\n", WEXITSTATUS(status));
        msg = (char *)my_malloc(msg_buffer_len * sizeof(char));
        snprintf(msg, msg_buffer_len, "Child exited with code %d\n", WEXITSTATUS(status));
        addMessageToMessagesWindow(msg);
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