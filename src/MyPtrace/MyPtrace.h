#ifndef MY_PTRACE_H
#define MY_PTRACE_H

#include <inttypes.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>

// i only take the intresting registers, ignore some
#define REGISTERS_NUMBER 18
#define REGISTERS_TEXT_SIZE 50

typedef void (*ChildSignalHandler)(int);

typedef struct mpt_context mpt_context;

mpt_context *mpt_initTrace(char *programName, const char *args);
void freeContext(mpt_context *ctx);
void mpt_listenToChild(mpt_context *ctx, ChildSignalHandler childSignalHandler);
void mpt_getRegisters(mpt_context *ctx, struct user_regs_struct *regs);

void mpt_regStructToText(
    struct user_regs_struct *regs, char **registersText);

/* gets data using process_vm_readv introduced in linux 3.2
to avoid looping with every single byte using PTRACE_PEEK_DATA */
char *mpt_getDataFromProcess(mpt_context *ctx, uint64_t address, size_t length);

pid_t mpt_getTraceePid(mpt_context *ctx);
#endif