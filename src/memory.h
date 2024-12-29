#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdbool.h>
#include <stdint.h>

#define FONT_START 0x50         // General convention around CHIP-8
#define PROGRAM_START 0x200     // General convention around CHIP-8
#define MEMORY_SIZE (4 * 1024)  // 4KB
#define FONT_SIZE (16 * 5)      // 16 characters of 5 bytes

/**
 * Initializes the memory array to zero.
 *
 * This function must be called before any read or write operations to ensure
 * that the memory is properly initialized in a consistent state.
 *
 * @return void
 */
void init_memory();

/**
 * Loads the provided program into memory.
 *
 * @param program An array describing the program.
 */
void load_program(uint8_t *program);

/**
 * Reads a value from memory at the specified address.
 *
 * The function assumes that the provided address fits within the 12-bit memory
 * space, and out-of-bounds access should be validated by the CPU.
 *
 * @param address The 12-bit memory address to read from.
 * @return The 8-bit value stored at the specified address.
 */
uint8_t read_memory(uint16_t address);

/**
 * Gets a pointer to memory at the current address.
 *
 * Allows for passing around several bytes of data stored in memory to other
 * modules without having to manually index across results.
 *
 * @param address The 12-bit memory address to read from.
 * @return A pointer to the requested memory address.
 */
uint8_t *get_memory_pointer(uint16_t address);

/**
 * Writes a value to memory at the specified address.
 *
 * The function assumes that the provided address fits within the 12-bit memory
 * space, and out-of-bounds access should be validated by the CPU.
 *
 * @param address The 12-bit memory address to write to.
 * @param value The 8-bit value to store at the specified address.
 * @return void
 */
void write_memory(uint16_t address, uint8_t value);

#endif  // !MEMORY_H_
