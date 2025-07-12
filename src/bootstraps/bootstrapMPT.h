#ifndef BOOTSTRAP_MPT_H
#define BOOTSTRAP_MPT_H

#include "./bootstrapFrontend.h"
#include <signal.h>

#include "./utils/utils.h"
#include "MyPtrace/MyPtrace.h"
#include "getFileTextSection.h"

extern mpt_context *traceeContext;

extern FileTextSection textSection;

extern int instructionCount;
extern const Instruction *instructions; /* allocated */
extern char **instructionsText;         /* allocated */

extern struct user_regs_struct *fpregs; /* allocated */

void loadNewExec();
void childExecHandler();
void childSignalHandler(int status);
void childExitHandler(unsigned char status);
void freeMPT();

void initRegisters();
void initInstructions();

#endif // BOOTSTRAP_MPT_H