#include <stdio.h>

#include "cpu.h"
#include "display.h"
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

// MARK: Startup

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

// MARK: Instructions

// 0x0NNN
void test_machine_language_routine_is_error()
{
    struct cpu_status status = debug_run_instruction(0x0000);
    TEST_ASSERT_EQUAL_UINT8(INVALID_INSTRUCTION, status.code);
}

// 0x6XNN
void test_set_updates_variable_registers()
{
    struct cpu_status status;

    status = debug_run_instruction(0x6001);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT8(0x01, get_variable_registers()[0]);

    status = debug_run_instruction(0x6002);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT8(0x02, get_variable_registers()[0]);

    status = debug_run_instruction(0x6303);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT8(0x03, get_variable_registers()[3]);
}

// 0x7XNN
void test_add_updates_variable_registers()
{
    struct cpu_status status;

    status = debug_run_instruction(0x7001);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT8(0x01, get_variable_registers()[0]);

    status = debug_run_instruction(0x7002);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT8(0x03, get_variable_registers()[0]);

    status = debug_run_instruction(0x7303);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT8(0x03, get_variable_registers()[3]);
}

// 0x1NNN
void test_jump_updates_program_counter()
{
    struct cpu_status status;

    status = debug_run_instruction(0x1000);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(0x000, get_program_counter());

    status = debug_run_instruction(0x1FFF);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(0xFFF, get_program_counter());
}

// 0x3XNN
void test_skip_if_variable_equal_constant()
{
    struct cpu_status status;

    // False
    status = debug_run_instruction(0x3001);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(0x200, get_program_counter());

    // True
    status = debug_run_instruction(0x3000);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(0x202, get_program_counter());
}

// 0x4XNN
void test_skip_if_variable_not_equal_constant()
{
    struct cpu_status status;

    // False
    status = debug_run_instruction(0x4000);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(0x200, get_program_counter());

    // True
    status = debug_run_instruction(0x4001);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(0x202, get_program_counter());
}

// 0x5XY0
void test_skip_if_variable_equal_variable()
{
    struct cpu_status status;

    // False
    debug_run_instruction(0x6101);           // Set V1 to 1.
    status = debug_run_instruction(0x5010);  // Compare V0 (0) to V1 (1).
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(0x200, get_program_counter());

    // True
    debug_run_instruction(0x6100);           // Set V1 to 0.
    status = debug_run_instruction(0x5010);  // Compare V0 (0) to V1 (0).
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(0x202, get_program_counter());
}

// 0x9XY0
void test_skip_if_variable_not_equal_variable()
{
    struct cpu_status status;

    // False
    status = debug_run_instruction(0x9010);  // Compare V0 (0) to V1 (0).
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(0x200, get_program_counter());

    // True
    debug_run_instruction(0x6101);           // Set V1 to 1.
    status = debug_run_instruction(0x9010);  // Compare V0 (0) to V1 (1).
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(0x202, get_program_counter());
}

// 0xANNN
void test_set_index_updates_index_register()
{
    struct cpu_status status;

    status = debug_run_instruction(0xA300);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(0x300, get_index_register());
}

// 0xDXYN
void test_draw_renders_sprite()
{
    struct cpu_status status;

    // Move the index register and insert the sprite there.
    status = debug_run_instruction(0xA300);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    uint8_t sprite[6] = {0x6C, 0xFE, 0xFE, 0x7C, 0x38, 0x10};
    for (uint8_t i = 0; i < 6; i++) {
        write_memory(get_index_register() + i, sprite[i]);
    }

    // Move the variable registers and draw the sprite at them.
    status = debug_run_instruction(0x601D);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    status = debug_run_instruction(0x610D);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    status = debug_run_instruction(0xD016);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);

    // Validate that the sprite was drawn.
    bool expected[6][8] = {
        {0, 1, 1, 0, 1, 1, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0},
    };
    bool(*display)[SCREEN_WIDTH] = get_display();
    for (uint8_t y = 13; y < 6; y++) {
        for (uint8_t x = 29; x < 8; x++) {
            TEST_ASSERT_EQUAL(expected[y][x], display[y][x]);
        }
    }
}

// 0x00E0
void test_clear_display_clears_display()
{
    struct cpu_status status;

    // Draw the sprite the same as the last test.
    status = debug_run_instruction(0xA300);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    uint8_t sprite[6] = {0x6C, 0xFE, 0xFE, 0x7C, 0x38, 0x10};
    for (uint8_t i = 0; i < 6; i++) {
        write_memory(get_index_register() + i, sprite[i]);
    }
    status = debug_run_instruction(0x601D);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    status = debug_run_instruction(0x610D);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    status = debug_run_instruction(0xD016);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);

    // Clear the display and validate that it was cleared.
    bool(*display)[SCREEN_WIDTH] = get_display();
    for (uint8_t y = 13; y < 6; y++) {
        for (uint8_t x = 29; x < 8; x++) {
            TEST_ASSERT_FALSE(display[y][x]);
        }
    }
}

// MARK: CPU cycling

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
    RUN_TEST(test_machine_language_routine_is_error);
    RUN_TEST(test_set_updates_variable_registers);
    RUN_TEST(test_add_updates_variable_registers);
    RUN_TEST(test_jump_updates_program_counter);
    RUN_TEST(test_skip_if_variable_equal_constant);
    RUN_TEST(test_skip_if_variable_not_equal_constant);
    RUN_TEST(test_skip_if_variable_equal_variable);
    RUN_TEST(test_skip_if_variable_not_equal_variable);
    RUN_TEST(test_set_index_updates_index_register);
    RUN_TEST(test_draw_renders_sprite);
    RUN_TEST(test_clear_display_clears_display);
    RUN_TEST(test_read_instruction_reads_and_moves_pc);
    RUN_TEST(test_run_cycle_reads_and_executes_instruction);
    return UNITY_END();
}
