#ifndef CPU_H_
#define CPU_H_

#include <stdint.h>

#define INSTRUCTIONS_PER_SECOND 700  // The amount of instructions to perform per second.

/**
 * Performs the startup sequence of the emulator.
 *
 * Initializes the system into a stable state and loads a ROM from the provided
 * file path.
 *
 * @param path The path to a ROM file which should be read into memory.
 */
void startup(char* path);

/**
 * Runs a single CPU cycle.
 *
 * Provides a shared abstraction for both reading and executing an instruction
 * from the system's memory.
 */
void run_cycle();

/**
 * Reads and returns the next CPU instruction.
 *
 * Localizes the entire handling of the program counter and read access of the
 * memory for the current CPU cycle.
 *
 * @return The 4 bytes that describe the next instruction.
 */
static uint16_t read_instruction();

/**
 * Runs the provided CPU instruction.
 *
 * Localizes the decoding and execution of the provided instruction, delagating
 * steps to other modules of the system where appropriate.
 *
 * @param instruction The instruction to execute.
 */
static void run_instruction(uint16_t instruction);

#endif  // !CPU_H_
