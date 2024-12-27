#ifndef MEMORY_H_
#define MEMORY_H_

#include <stdint.h>

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
