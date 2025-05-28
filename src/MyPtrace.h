#ifndef MY_PTRACE_H
#define MY_PTRACE_H

#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>

void mpt_traceMe(char *programName, const char *args);
void mpt_getRegisters(pid_t child_pid, struct user_regs_struct *regs);
#endif