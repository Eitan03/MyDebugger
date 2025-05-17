#ifndef GET_FILE_TEXT_SECTION_H
#define GET_FILE_TEXT_SECTION_H

#include <inttypes.h>

typedef struct FileTextSection
{
    char *start;
    char *end;
} FileTextSection;

typedef struct Instruction
{
    char *text;
    uint64_t address;
} Instruction;

Instruction *getInstructions(char *data, uint64_t length, uint64_t runtime_address, int *instructionCount);
void freeInstructions(Instruction *instructions, int instructionCount);
FileTextSection getTextSectionFromMaps();

#endif
