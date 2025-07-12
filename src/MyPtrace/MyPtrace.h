#ifndef MY_PTRACE_H
#define MY_PTRACE_H

#include <inttypes.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>

#include "../LinkedList/LinkedList.h"

// i only take the intresting registers, ignore some
#define REGISTERS_NUMBER 18
#define REGISTERS_TEXT_SIZE 50

typedef void (*ChildSignalHandler)(int);
typedef void (*ChildExecHandler)(void);
typedef void (*ChildExitHandler)(unsigned char);

typedef struct __mpt_context
{
    pid_t childPid;
    LinkedList *breakpoints;
} __mpt_context;

// #define mpt_context __mpt_context;
typedef struct __mpt_context mpt_context;

__mpt_context *mpt_initTrace(char *programName, const char *args);
void mpt_freeContext(__mpt_context *ctx);
void mpt_listenToChild(__mpt_context *ctx, ChildExecHandler childExecHandler, ChildSignalHandler childSignalHandler, ChildExitHandler childExitHandler);
void mpt_getRegisters(__mpt_context *ctx, struct user_regs_struct *regs);
void mpt_setRegisters(__mpt_context *ctx, struct user_regs_struct *regs);

void mpt_regStructToText(struct user_regs_struct *regs, char **registersText);

/* gets data using process_vm_readv introduced in linux 3.2
to avoid looping with every single byte using PTRACE_PEEK_DATA */
char *mpt_getDataFromProcess(__mpt_context *ctx, uint64_t address, size_t length);

int mpt_setBreakpoint(__mpt_context *ctx, uint64_t address);

pid_t mpt_getTraceePid(__mpt_context *ctx);
#endif