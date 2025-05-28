#include "MyPtrace.h"

#include <stdio.h>
#include <sys/ptrace.h>
#include <unistd.h>

#include "utils.h"

void mpt_traceMe(char *programName, const char *args)
{
    printf("executing %s\n", programName);
    int err = ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    validateErrno(err, "ptrace");
    err = execl(programName, args, (char *)NULL);
    validateErrno(err, "execv");
}

void mpt_getRegisters(pid_t child_pid, struct user_regs_struct *regs)
{
    int err = ptrace(PTRACE_GETREGS, child_pid, NULL, regs);
    validateErrno(err, "ptrace");
}