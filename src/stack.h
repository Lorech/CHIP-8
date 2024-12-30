#ifndef STACK_H_
#define STACK_H_

#include <stdbool.h>
#include <stdint.h>

#define STACK_SIZE 16

typedef struct {
    uint16_t addresses[STACK_SIZE];
    int8_t pointer;
} stack;

/**
 * Initializes the stack.
 *
 * This function must be called before any push or pop operations to ensure
 * that the stack is properly initialized in a consistent state.
 *
 * @param s The stack to initialize.
 * @return void
 */
void init_stack(stack *s);

/**
 * Pushes a value onto the stack.
 *
 * For CHIP-8, the stack only stores addresses to return to from subroutines.
 *
 * @param s The stack to push onto.
 * @param address The address to push to the stack.
 * @return If the operation was successful or not.
 */
bool push(stack *s, uint16_t address);

/**
 * Pops a value from the stack.
 *
 * For CHIP-8, the stack only stores addresses to return to from subroutines.
 *
 * @param s The stack to pop from.
 * @param address The address popped from the stack.
 * @return If the operation was successful or not.
 */
bool pop(stack *s, uint16_t *address);

/**
 * Peeks at the top of the stack, setting the address to it's value without
 * mutating the stack itself.
 *
 * For CHIP-8, the stack only stores addresses to return to from subroutines.
 *
 * @param s The stack to peek from.
 * @param address The address at the top of the stack.
 * @return If the operation was successful or not.
 */
bool peek(stack *s, uint16_t *address);

#endif  // !STACK_H_
