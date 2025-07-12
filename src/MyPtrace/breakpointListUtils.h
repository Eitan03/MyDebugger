#ifndef BRKEAPOINTS_UTILS_H
#define BRKEAPOINTS_UTILS_H

#include <stdint.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/user.h>

#include "../utils/utils.h"

#include "./MyPtrace.h"

#include "../LinkedList/LinkedList.h"

#define BREAKPOINT_OPCODE 0xCC

typedef struct mpt_breakpoint
{
    long original_opcode;
    uint64_t address;
} mpt_breakpoint;

typedef int (*ComparisonFunction)(const void *key1, const void *key2, void *additionalArgs);
typedef void (*FreeValueFunction)(Value value, void *AdditionalArgs);

int compareBreakpoints(const void *break1, const void *break2, void *additionalArgs)
{
    return ((const mpt_breakpoint *)break1)->address == ((const mpt_breakpoint *)break2)->address;
}

void freeBreakpoint(void *breakpoint, void *additionalArgs)
{
    my_free((mpt_breakpoint *)breakpoint);
}

LinkedList *createBreakpointsLinkedList()
{
    return datatypes_linkedList_create(
        compareBreakpoints,
        freeBreakpoint,
        NULL,
        NULL);
}

/**
 * returns 0 on success and 1 if breakpoint already there
 * uses PTRACE_PEEKDATA to get the original value before setting a breakpoint with 0xCC
 */
int addBreakpoint(__mpt_context *ctx, uint64_t address)
{

    mpt_breakpoint *breakpoint = (mpt_breakpoint *)malloc(sizeof(mpt_breakpoint));
    breakpoint->address = address;

    if (datatypes_linkedList_exists(ctx->breakpoints, (Value)breakpoint))
    {
        return 1;
    }

    breakpoint->original_opcode = (long)ptrace(PTRACE_PEEKTEXT, ctx->childPid, address, NULL);

    long breakpoint_instruction = (breakpoint->original_opcode & ~0xFF) ^ BREAKPOINT_OPCODE;

    ptrace(PTRACE_POKETEXT, ctx->childPid, address, (long)breakpoint_instruction);

    datatypes_linkedList_append(ctx->breakpoints, breakpoint);

    return 0;
}

int isBreakpointHit(__mpt_context *ctx)
{
    struct user_regs_struct regs;
    mpt_getRegisters(ctx, &regs);

    mpt_breakpoint breakpoint;
    breakpoint.address = regs.rip - 1;

    return datatypes_linkedList_exists(ctx->breakpoints, (Value)&breakpoint);
}

void handleBreakpoint(__mpt_context *ctx)
{
    /* code duplication, maybe externally get rip and pass to isBreakpointHit and handleBreakpoint */
    struct user_regs_struct regs;
    mpt_getRegisters(ctx, &regs);

    mpt_breakpoint hollowSearchBreakpoint;
    hollowSearchBreakpoint.address = regs.rip - 1;

    // assumes breakpoint exsits (and hit) so no need to check for NULL
    mpt_breakpoint *breakpoint = (mpt_breakpoint *)datatypes_linkedList_find(ctx->breakpoints, (Value)&hollowSearchBreakpoint)->value;

    ptrace(PTRACE_POKETEXT, ctx->childPid, breakpoint->address, breakpoint->original_opcode);

    regs.rip = breakpoint->address;
    mpt_setRegisters(ctx, &regs);

    if (datatypes_linkedList_remove(ctx->breakpoints, (Value)breakpoint) == 0)
    {
        printf("error! trying to remove non existing breakpoint!");
        exit(EXIT_FAILURE);
    }
    return;
}

#endif