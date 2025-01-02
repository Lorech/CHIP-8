#include "cpu.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "display.h"
#include "macros.h"
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
    // Read the program from a file into memory.
    FILE *f;
    f = fopen(path, "rb");
    uint8_t program[0xFFF - 0x200];
    fread(program, 1, 0xFFF - 0x200, f);
    fclose(f);

    // Reset the CPU state.
    PC = 0x200;
    I = 0x000;
    for (uint8_t i = 0; i < 16; i++) {
        V[i] = 0x00;
    }

    // Initialize the sub-modules of the system.
    init_stack(&s);
    init_memory();
    load_program(program);
    clear_display();
}

struct cpu_status run_cycle()
{
    uint16_t instruction = read_instruction();
    struct cpu_status status = {.code = SUCCESS, .instruction = instruction};
    run_instruction(instruction, &status);
    return status;
}

static void run_instruction(uint16_t instruction, struct cpu_status *status)
{
    // TODO: Implement the missing instructions.
    switch (instruction & N1) {
        case 0x0000:  // System
            switch (instruction & MA) {
                case 0x00E0:  // Clear display
                    clear_display();
                    break;
                case 0x00EE:  // Return from subroutine
                    pop(&s, &PC);
                    break;
                default:  // Call machine code routine
                    status->code = INVALID_INSTRUCTION;
                    status->instruction = instruction;
            }
            break;
        case 0x1000:  // Jump
            PC = instruction & MA;
            break;
        case 0xB000:  // Jump with offset
            PC = (instruction & MA) + V[0x0];
            // TODO: Add configurable option to use offset of specific register.
            break;
        case 0x2000:  // Call subroutine
            push(&s, PC);
            PC = instruction & MA;
        case 0x3000:  // Skip if variable equal to constant
            if (V[(instruction & N2) >> 8] == (instruction & B2)) {
                PC += 2;
            }
            break;
        case 0x4000:  // Skip if variable not equal to constant
            if (V[(instruction & N2) >> 8] != (instruction & B2)) {
                PC += 2;
            }
            break;
        case 0x5000:  // Skip if variable equal to variable
            if (V[(instruction & N2) >> 8] == V[(instruction & N3) >> 4]) {
                PC += 2;
            }
            break;
        case 0x9000:  // Skip if variable not equal to variable
            if (V[(instruction & N2) >> 8] != V[(instruction & N3) >> 4]) {
                PC += 2;
            }
            break;
        case 0x6000:  // Set variable register
            V[(instruction & N2) >> 8] = instruction & B2;
            break;
        case 0x7000:  // Add to variable register
            V[(instruction & N2) >> 8] += instruction & B2;
            break;
        case 0x8000:  // Logic and arithmetic
        {
            uint8_t *VX = &V[(instruction & N2) >> 8];
            uint8_t *VY = &V[(instruction & N3) >> 4];
            switch (instruction & N4) {
                case 0x000:  // Set
                    V[(instruction & N2) >> 8] = V[(instruction & N3) >> 4];
                    break;
                case 0x004:  // Add
                    *VX = *VX + *VY;
                    V[0xF] = *VX <= *VY;
                    break;
                case 0x005:  // Subtract Y from X
                    V[0xF] = *VY > *VX;
                    *VX = *VY - *VX;
                    break;
                case 0x007:  // Subtract X from Y
                    V[0xF] = *VX > *VY;
                    *VX = *VX - *VY;
                    break;
                case 0x00E:  // Shift left
                    V[0xF] = (*VX & 0x80) >> 7;
                    *VX = *VX << 1;
                    // TODO: Add configurable option to move VY into VX.
                    break;
                case 0x006:  // Shift right
                    V[0xF] = *VX & 0x01;
                    *VX = *VX >> 1;
                    // TODO: Add configurable option to move VY into VX.
                    break;
                case 0x002:  // AND
                    *VX = *VX & *VY;
                    break;
                case 0x001:  // OR
                    *VX = *VX | *VY;
                    break;
                case 0x003:  // XOR
                    *VX = *VX ^ *VY;
                    break;
                default:
                    status->code = INVALID_INSTRUCTION;
                    status->instruction = instruction;
            }
        }
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
        case 0xC000:  // Random
            V[(instruction & N2) >> 8] = (uint8_t)rand() & (instruction & B2);
            break;
        case 0xF000:  // Misc.
            switch (instruction & B2) {
                case 0x001E:  // Add to index register
                    I += V[(instruction & N2) >> 8];
                    // Manually handle overflow, as the variable is a uint16,
                    // but the memory space of the system is 12 bits long.
                    if (I > 0xFFF) {
                        I = I % 0xFFF - 1;
                        V[0xF] = 1;
                    }
                    break;
                case 0x0029:  // Set index register to character
                {
                    uint8_t character = V[(instruction & N2) >> 8] & 0x0F;
                    I = FONT_START + character * 5;
                } break;
                case 0x0033:  // Convert to decimal
                {
                    uint8_t *memory = get_memory_pointer(I);
                    uint8_t num = V[(instruction & N2) >> 8];
                    // Extract the digits least significant to most.
                    uint8_t digits[3];
                    uint8_t i = 0;
                    do {
                        digits[i++] = num % 10;
                        num /= 10;
                    } while (num != 0);
                    // Insert the digits most significant to least.
                    uint8_t j = 0;
                    do {
                        memory[j++] = digits[--i];
                    } while (i != 0);
                } break;
                default:
                    status->code = INVALID_INSTRUCTION;
                    status->instruction = instruction;
            }
            break;
        default:
            status->code = UNKNOWN_INSTRUCTION;
            status->instruction = instruction;
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

struct cpu_status debug_run_instruction(uint16_t instruction)
{
    struct cpu_status status = {.code = SUCCESS, .instruction = instruction};
    run_instruction(instruction, &status);
    return status;
}
#endif  // !UNIT_TEST
