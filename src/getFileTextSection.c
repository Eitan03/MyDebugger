#include <string.h>

#include "getFileTextSection.h"

#include "Zydis.h"
#include <stdio.h>

/* Im lazy to do so optimitacilly */
int getInstructionCount(char *data, uint64_t length)
{

    ZydisDisassembledInstruction instruction;
    size_t offset = 0;
    int instrcutionCount = 0;
    while (ZYAN_SUCCESS(ZydisDisassembleIntel(
        /* machine_mode:    */ ZYDIS_MACHINE_MODE_LONG_64,
        /* runtime_address: */ 0x0,
        /* buffer:          */ data + offset,
        /* length:          */ length,
        /* instruction:     */ &instruction)))
    {
        offset += instruction.info.length;
        instrcutionCount++;
    }
    return instrcutionCount;
}

/* this can optimised via seperating the ZydisDisassembleIntel function*/
/**
 * return an HEAP array of Instruction representing each instruction
 * use freeInstructions to free the array
 * @param data the data to disassemble
 * @param length the length of the data
 * @param runtime_address the address to use for disassembly
 * @param instructionCount output - the number of instructions
 * @return an array of Instruction
 * @note the instructions are allocated on the heap and must be freed via the freeInstructions function
 */
Instruction *getInstructions(char *data, uint64_t length, uint64_t runtime_address, int *out_instructionCount)
{
    int instructionsCount = getInstructionCount(data, length);

    Instruction *instructions = (Instruction *)calloc(instructionsCount, sizeof(Instruction));
    if (instructions == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for instructions\n");
        exit(1);
    }

    ZydisDisassembledInstruction zydisInstruction;
    size_t offset = 0;
    int instructionIndex = 0;
    while (ZYAN_SUCCESS(ZydisDisassembleIntel(
        /* machine_mode:    */ ZYDIS_MACHINE_MODE_LONG_64,
        /* runtime_address: */ runtime_address + offset,
        /* buffer:          */ data + offset,
        /* length:          */ length,
        /* instruction:     */ &zydisInstruction)))
    {

        // printf("%05" PRIX64 "  %s\n", offset, instruction.text);
        Instruction *instruction = &instructions[instructionIndex];
        instruction->text = (char *)malloc(sizeof(char) * (strlen(zydisInstruction.text) + 1));
        strcpy(instruction->text, zydisInstruction.text);
        instruction->address = runtime_address + offset;

        instructionIndex++;
        offset += zydisInstruction.info.length;
    }

    if (out_instructionCount != NULL)
    {
        *out_instructionCount = instructionsCount;
    }
    return instructions;
}

void freeInstructions(Instruction *instructions, int instructionCount)
{
    for (int i = 0; i < instructionCount; i++)
    {
        free(instructions[i].text);
    }
    free(instructions);
}

FileTextSection getTextSectionFromMaps()
{
    FILE *mapsFile = fopen("/proc/self/maps", "r");
    if (!mapsFile)
    {
        perror("Failed to open /proc/self/maps");
    }

    char line[512];
    char *text_start = NULL;
    char *text_end = NULL;

    while (fgets(line, sizeof(line), mapsFile))
    {
        if (strstr(line, "r-xp") != NULL)
        {
            char *end;
            text_start = (char *)strtoul(line, &end, 16);
            text_end = (char *)strtoul(end + 1, NULL, 16);
            break;
        }
    }

    fclose(mapsFile);

    if (text_start == NULL)
    {
        fprintf(stderr, "Failed to find start of text section\n");
        exit(1);
    }

    return (FileTextSection){.start = text_start, .end = text_end};
}