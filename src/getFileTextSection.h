#ifndef GET_FILE_TEXT_SECTION_H
#define GET_FILE_TEXT_SECTION_H

#include <fcntl.h>
#include <inttypes.h>

typedef struct FileTextSection
{
    uint64_t start;
    uint64_t end;
} FileTextSection;

typedef struct Instruction
{
    char *text;
    uint64_t address;
} Instruction;

Instruction *getInstructions(char *data, uint64_t length, uint64_t runtime_address, int *instructionCount);
void freeInstructions(Instruction *instructions, int instructionCount);

FileTextSection getTextSectionFromMaps(pid_t processId);

#endif
