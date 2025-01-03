#ifndef CPU_H_
#define CPU_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef UNIT_TEST
#include "stack.h"
#endif  // !UNIT_TEST

#define INSTRUCTIONS_PER_SECOND 700

enum cpu_status_code {
    SUCCESS,
    UNKNOWN_INSTRUCTION,
    INVALID_INSTRUCTION,
    INVALID_MEMORY_ACCESS,
    INVALID_STACK_OPERATION,
    INVALID_VARIABLE_REGISTER,
};

struct cpu_status {
    enum cpu_status_code code;  // The status code of the CPU cycle.
    uint16_t instruction;       // The instruction that was executed this cycle.
};

/**
 * Performs the startup sequence of the emulator.
 *
 * Initializes the system into a stable state and loads a ROM from the provided
 * file path.
 *
 * @param path The path to a ROM file which should be read into memory.
 */
void startup(char *path);

/**
 * Runs a single CPU cycle.
 *
 * Provides a shared abstraction for both reading and executing an instruction
 * from the system's memory.
 *
 * @return Meta information about the CPU cycle.
 */
struct cpu_status run_cycle();

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
 * @param error Meta information about the CPU cycle.
 */
static void run_instruction(uint16_t instruction, struct cpu_status *error);

#ifdef UNIT_TEST
/**
 * Retrieves the program counter for testing purposes.
 *
 * @return The program counter.
 */
uint16_t get_program_counter();

/**
 * Retrieves the index register for testing purposes.
 *
 * @return The index register.
 */
uint16_t get_index_register();

/**
 * Retrieves the variable registers for testing purposes.
 *
 * @return The variable registers.
 */
uint8_t *get_variable_registers();

/**
 * Retrieves the stack for testing purposes.
 *
 * @return The stack.
 */
stack *get_stack();

/**
 * Reads and returns the next CPU instruction.
 *
 * Publicly exposes the read_instruction function for testing purposes.
 *
 * @return The 4 bytes that describe the next instruction.
 */
uint16_t debug_read_instruction();

/**
 * Runs the provided CPU instruction.
 *
 * Publicly exposes the run_instruction function for testing purposes.
 *
 * @param instruction The instruction to execute.
 * @return Meta information about the CPU cycle.
 */
struct cpu_status debug_run_instruction(uint16_t instruction);
#endif  // !UNIT_TEST

#endif  // !CPU_H_
