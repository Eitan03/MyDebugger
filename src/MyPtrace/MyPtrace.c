#include "MyPtrace.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../LinkedList/LinkedList.h"
#include "../utils/utils.h"
#include <signal.h>

typedef struct mpt_context
{
    pid_t childPid;
    LinkedList *breakpoints;
} mpt_context;

void mpt_traceMe(char *programName, const char *args)
{
    printf("executing %s\n", programName);
    int err = ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    validateErrno(err, "ptrace");

    // this is useful to set the PTRACE_O_TRACEEXEC before the child executes
    // to allow catching nested exec calls
    raise(SIGSTOP); // Give tracer time to attach and set PTRACE_O_TRACEEXEC

    execl(programName, args, (char *)NULL);
}

mpt_context *mpt_initTrace(char *programName, const char *args)
{
    pid_t traceePid = fork();
    if (traceePid == -1)
    {
        validateErrno(traceePid, "fork");
    }

    if (traceePid == 0)
    {
        mpt_traceMe(programName, args);
    }

    mpt_context *traceeContext = (mpt_context *)my_malloc(sizeof(mpt_context));
    if (traceeContext == NULL)
    {
        validateErrno(-1, "malloc");
    }

    traceeContext->childPid = traceePid;
    traceeContext->breakpoints = datatypes_linkedList_create(
        NULL,
        NULL,
        NULL,
        NULL);

    return traceeContext;
}

void mpt_freeContext(mpt_context *ctx)
{
    datatypes_linkedList_destroy(ctx->breakpoints);
    my_free(ctx);
}

void mpt_listenToChild(mpt_context *ctx, ChildExecHandler childExecHandler, ChildSignalHandler childSignalHandler)
{
    int status;

    waitpid(ctx->childPid, &status, 0);
    ptrace(PTRACE_SETOPTIONS, ctx->childPid, NULL, PTRACE_O_TRACEEXEC);
    ptrace(PTRACE_CONT, ctx->childPid, NULL, NULL);

    do
    {
        waitpid(ctx->childPid, &status, 0);
        if (status >> 8 == (SIGTRAP | (PTRACE_EVENT_EXEC << 8)))
        {
            childExecHandler();
        }
        else
        {
            childSignalHandler(status);
        }
        ptrace(PTRACE_CONT, ctx->childPid, NULL, NULL);
    } while (!WIFEXITED(status));
    // child exited with code WEXITSTATUS(status)
}

void mpt_getRegisters(mpt_context *ctx, struct user_regs_struct *regs)
{
    int err = ptrace(PTRACE_GETREGS, ctx->childPid, NULL, regs);
    validateErrno(err, "ptrace");
}

void mpt_regStructToText(
    struct user_regs_struct *regs,
    char **registersText)
{
    sprintf(registersText[0], "r15: 0x%08llx", regs->r15);
    sprintf(registersText[1], "r14: 0x%08llx", regs->r14);
    sprintf(registersText[2], "r13: 0x%08llx", regs->r13);
    sprintf(registersText[3], "r12: 0x%08llx", regs->r12);
    sprintf(registersText[4], "r11: 0x%08llx", regs->r11);
    sprintf(registersText[5], "r10: 0x%08llx", regs->r10);
    sprintf(registersText[6], "r9: 0x%08llx", regs->r9);
    sprintf(registersText[7], "r8: 0x%08llx", regs->r8);
    sprintf(registersText[8], "rax: 0x%08llx", regs->rax);
    sprintf(registersText[9], "rbx: 0x%08llx", regs->rbx);
    sprintf(registersText[11], "rcx: 0x%08llx", regs->rcx);
    sprintf(registersText[12], "rdx: 0x%08llx", regs->rdx);
    sprintf(registersText[13], "rdi: 0x%08llx", regs->rdi);
    sprintf(registersText[14], "rsi: 0x%08llx", regs->rsi);
    sprintf(registersText[15], "rsp: 0x%08llx", regs->rsp);
    sprintf(registersText[16], "rip: 0x%08llx", regs->rip);
    sprintf(registersText[17], "eflags: 0x%08llx", regs->eflags);
}

char *mpt_getDataFromProcess(mpt_context *ctx, uint64_t address, size_t length)
{
    char *data = my_malloc(length);
    struct iovec local[1] = {{.iov_base = data, .iov_len = length}};
    struct iovec remote[1] = {{.iov_base = (void *)address, .iov_len = length}};

    ssize_t nread = process_vm_readv(ctx->childPid, local, 2, remote, 1, 0);
    if (nread == -1)
    {
        perror("process_vm_readv");
        exit(EXIT_FAILURE);
    }

    if (nread != length)
    {
        fprintf(stderr, "Failed to read memory from process %d: %zd bytes read, expected %zu bytes\n", ctx->childPid, nread, length);
        exit(EXIT_FAILURE);
    }

    return data;
}

pid_t mpt_getTraceePid(mpt_context *ctx)
{
    return ctx->childPid;
}