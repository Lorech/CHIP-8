#include "cpu.h"

#include <stdint.h>
#include <stdio.h>

#include "memory.h"

// Bit masks for extracting instructions.
#define N1 0xF000  // First nibble - instruction category
#define N2 0x0F00  // Second nibble - usually a variable register
#define N3 0x00F0  // Third nibble - usually a variable register
#define N4 0x000F  // Fourth nibble - a 4-bit number
#define B2 0x00FF  // Second byte - an 8-bit number
#define MA 0x0FFF  // Three nibbles - a 12-bit memory address

static uint16_t PC = 0;  // Program counter
static uint16_t I = 0;   // Index register
static uint8_t V[16];    // Variable registers

void run_cycle()
{
    uint16_t instruction = read_instruction();
    run_instruction(instruction);
}

static void run_instruction(uint16_t instruction)
{
    // TODO: Implement the missing instructions.
    switch (instruction & N1) {
        case 0x0000:  // System
            switch (instruction & MA) {
                case 0x00E0:
                    // TODO: Implement clear screen.
                    break;
                default:
                    printf(
                        "WARNING: Tried running system instruction 0x%04X, "
                        "which is not implemented.\n",
                        instruction);
            }
            break;
        case 0x1000:  // Jump
            PC = instruction & MA;
            break;
        case 0x6000:  // Set variable register
            V[instruction & N2] = instruction & B2;
            break;
        case 0x7000:  // Add to variable register
            V[instruction & N2] += instruction & B2;
            break;
        case 0xA000:  // Set index register
            I = instruction & MA;
            break;
        case 0xD000:  // Draw
            // TODO: Implement draw.
            break;
        default:
            printf("WARNING: Instruction 0x%04X has no implementation.\n",
                   instruction);
    }
}

static uint16_t read_instruction()
{
    uint16_t instruction = (read_memory(PC) << 8) | read_memory(PC + 1);
    PC += 2;
    return instruction;
}
