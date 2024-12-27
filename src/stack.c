#include "stack.h"

#include <stdint.h>
#include <stdio.h>

void init_stack(stack *s)
{
    s->pointer = -1;
    for (uint8_t i = 0; i < STACK_SIZE; ++i) {
        s->addresses[i] = 0;
    }
}

bool push(stack *s, uint16_t address)
{
    if (s->pointer == STACK_SIZE - 1) {
        printf("Stack overflow!");
        return false;
    }

    s->addresses[++s->pointer] = address;
    return true;
}

bool pop(stack *s, uint16_t *address)
{
    if (s->pointer == -1) {
        printf("Stack underflow!");
        return false;
    }

    *address = s->addresses[s->pointer--];
    return true;
}
