#include <stdint.h>

#include "memory.h"
#include "unity.h"

void setUp()
{
    init_memory();
}

void tearDown()
{
    return;
}

void test_init_memory_loads_only_font()
{
    for (uint16_t address = 0x000; address < MEMORY_SIZE; address++) {
        uint8_t value = read_memory(address);  // Implicit test.
        if (address >= FONT_START && address < FONT_START + FONT_SIZE) {
            // The font is privately scoped, so only check for non-zero values.
            TEST_ASSERT_NOT_EQUAL_INT8(0x000, value);
        } else {
            TEST_ASSERT_EQUAL_INT8(0x000, value);
        }
    }
}

void test_load_program_loads_program()
{
    uint8_t program[4] = {0x00, 0xE0, 0x12, 0x00};
    load_program(program);
    uint8_t *memory = get_memory_pointer(PROGRAM_START);  // Implicit test.
    for (uint16_t offset = 0; offset < 4; offset++) {
        TEST_ASSERT_EQUAL_INT8(program[offset], memory[offset]);
    }
}

void run_memory_tests()
{
    RUN_TEST(test_init_memory_loads_only_font);
    RUN_TEST(test_load_program_loads_program);
}
