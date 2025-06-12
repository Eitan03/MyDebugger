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

struct mpt_context;

void mpt_traceMe(char *programName, const char *args);
void mpt_listenToChild(pid_t childPid, ChildSignalHandler childSignalHandler);
void mpt_getRegisters(pid_t child_pid, struct user_regs_struct *regs);

void mpt_regStructToText(
    struct user_regs_struct *regs, char **registersText);

/* gets data using process_vm_readv introduced in linux 3.2
to avoid looping with every single byte using PTRACE_PEEK_DATA */
char *mpt_getDataFromProcess(pid_t processId, uint64_t address, size_t length);
#endif