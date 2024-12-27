#include "memory.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#define MEMORY_SIZE (4 * 1024)   // 4KB
#define FONT_SIZE (16 * 5)       // 16 characters of 5 bytes
#define FONT_START_ADDRESS 0x50  // General convention around CHIP-8
#define PROGRAM_START 0x200      // General convention around CHIP-8

const uint8_t FONT[FONT_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
    0x20, 0x60, 0x20, 0x20, 0x70,  // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
    0xF0, 0x80, 0xF0, 0x80, 0x80,  // F
};

static uint8_t memory[MEMORY_SIZE];

void init_memory()
{
    // Clear the usable memory space.
    for (uint16_t i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }

    // Load the font into the memory.
    for (uint16_t i = 0; i < sizeof(FONT); i++) {
        memory[FONT_START_ADDRESS + i] = FONT[i];
    }
}

void load_program(uint8_t *program)
{
    for (uint16_t offset = 0; offset < MEMORY_SIZE - PROGRAM_START; offset++) {
        memory[PROGRAM_START + offset] = program[offset];
    }
}

void write_memory(uint16_t address, uint8_t value)
{
    memory[address] = value;
}

uint8_t read_memory(uint16_t address)
{
    return memory[address];
}

uint8_t *get_memory_pointer(uint16_t address)
{
    return &memory[address];
}
