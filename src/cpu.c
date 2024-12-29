#include "cpu.h"

#include <stdint.h>
#include <stdio.h>

#include "display.h"
#include "memory.h"
#include "stack.h"

// Bit masks for extracting instructions.
#define N1 0xF000  // First nibble - instruction category
#define N2 0x0F00  // Second nibble - usually a variable register
#define N3 0x00F0  // Third nibble - usually a variable register
#define N4 0x000F  // Fourth nibble - a 4-bit number
#define B2 0x00FF  // Second byte - an 8-bit number
#define MA 0x0FFF  // Three nibbles - a 12-bit memory address

static uint16_t PC = 0x200;  // Program counter
static uint16_t I = 0x000;   // Index register
static uint8_t V[16];        // Variable registers
static stack s;              // The stack memory

void startup(char *path)
{
    FILE *f;
    f = fopen(path, "rb");
    uint8_t program[0xFFF - 0x200];
    fread(program, 1, 0xFFF - 0x200, f);
    fclose(f);

    init_stack(&s);
    init_memory();
    load_program(program);
    clear_display();
}

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
                    clear_display();
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
            V[(instruction & N2) >> 8] = instruction & B2;
            break;
        case 0x7000:  // Add to variable register
            V[(instruction & N2) >> 8] += instruction & B2;
            break;
        case 0xA000:  // Set index register
            I = instruction & MA;
            break;
        case 0xD000:  // Draw
            V[0xF] = draw_sprite(
                V[(instruction & N2) >> 8],
                V[(instruction & N3) >> 4],
                instruction & N4,
                get_memory_pointer(I));
            break;
        default:
            printf(
                "WARNING: Instruction 0x%04X has no implementation.\n",
                instruction);
    }
}

static uint16_t read_instruction()
{
    uint16_t instruction = (read_memory(PC) << 8) | read_memory(PC + 1);
    PC += 2;
    return instruction;
}

#ifdef UNIT_TEST
uint16_t get_program_counter()
{
    return PC;
}

uint16_t get_index_register()
{
    return I;
}

uint8_t *get_variable_registers()
{
    return V;
}

stack *get_stack()
{
    return &s;
}

uint16_t debug_read_instruction()
{
    return read_instruction();
}

void debug_run_instruction(uint16_t instruction)
{
    run_instruction(instruction);
}
#endif  // !UNIT_TEST
