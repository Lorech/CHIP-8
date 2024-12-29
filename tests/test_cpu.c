#include <stdio.h>

#include "cpu.h"
#include "memory.h"
#include "unity.h"

#define TEST_ROM "resources/roms/Test ROM.ch8"

void setUp()
{
    startup(TEST_ROM);
}

void tearDown()
{
    return;
}

void test_startup_loads_program()
{
    // Read the file so we can compare it to the memory.
    FILE *f;
    f = fopen(TEST_ROM, "rb");
    uint8_t program[0xFFF - 0x200];
    fread(program, 1, 0xFFF - 0x200, f);
    fclose(f);

    uint8_t *memory = get_memory_pointer(PROGRAM_START);
    for (uint16_t offset = 0; offset < 16; offset++) {
        TEST_ASSERT_EQUAL_INT8(program[offset], memory[offset]);
    }
}

// TODO: Test individual instructions.

void test_read_instruction_reads_and_moves_pc()
{
    TEST_ASSERT_EQUAL_INT16(0x601D, debug_read_instruction());
    TEST_ASSERT_EQUAL_INT16(0x200 + 2, get_program_counter());
}

void test_run_cycle_reads_and_executes_instruction()
{
    run_cycle();
    TEST_ASSERT_EQUAL_INT8(0x1D, get_variable_registers()[0]);
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_startup_loads_program);
    return UNITY_END();
}
