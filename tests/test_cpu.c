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

// 0x8XY0
void test_set_vx_to_vy()
{
    struct cpu_status status;

    debug_run_instruction(0x6101);

    status = debug_run_instruction(0x8010);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(0x01, get_variable_registers()[0x0]);
}

// 0x8XY4
void test_add_vy_to_vx()
{
    struct cpu_status status;

    debug_run_instruction(0x6001);
    debug_run_instruction(0x6102);

    status = debug_run_instruction(0x8014);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(0x03, get_variable_registers()[0x0]);
    TEST_ASSERT_FALSE(get_variable_registers()[0xF]);
}

void test_add_vy_to_vx_with_carry()
{
    struct cpu_status status;

    debug_run_instruction(0x60FF);
    debug_run_instruction(0x6102);

    status = debug_run_instruction(0x8014);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(0x01, get_variable_registers()[0x0]);
    TEST_ASSERT_TRUE(get_variable_registers()[0xF]);
}

// 0x8XY5
void test_subtract_vy_from_vx()
{
    struct cpu_status status;

    debug_run_instruction(0x6002);
    debug_run_instruction(0x6101);

    status = debug_run_instruction(0x8015);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(0xFF, get_variable_registers()[0x0]);
    TEST_ASSERT_FALSE(get_variable_registers()[0xF]);
}

void test_subtract_vy_from_vx_with_carry()
{
    struct cpu_status status;

    debug_run_instruction(0x6001);
    debug_run_instruction(0x6102);

    status = debug_run_instruction(0x8015);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(0x01, get_variable_registers()[0x0]);
    TEST_ASSERT_TRUE(get_variable_registers()[0xF]);
}

// 0x8XY7
void test_subtract_vx_from_vy()
{
    struct cpu_status status;

    debug_run_instruction(0x6001);
    debug_run_instruction(0x6102);

    status = debug_run_instruction(0x8017);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(0xFF, get_variable_registers()[0x0]);
    TEST_ASSERT_FALSE(get_variable_registers()[0xF]);
}

void test_subtract_vx_from_vy_with_carry()
{
    struct cpu_status status;

    debug_run_instruction(0x6002);
    debug_run_instruction(0x6101);

    status = debug_run_instruction(0x8017);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(0x01, get_variable_registers()[0x0]);
    TEST_ASSERT_TRUE(get_variable_registers()[0xF]);
}

// 0x8XYE
void test_shift_left()
{
    struct cpu_status status;

    debug_run_instruction(0x6003);  // Set V0 to 0b00000011

    status = debug_run_instruction(0x800E);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(0b00000110, get_variable_registers()[0x0]);
    TEST_ASSERT_FALSE(get_variable_registers()[0xF]);
}

void test_shift_left_with_carry()
{
    struct cpu_status status;

    debug_run_instruction(0x60C0);  // Set V0 to 0b11000000

    status = debug_run_instruction(0x800E);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(0b10000000, get_variable_registers()[0x0]);
    TEST_ASSERT_TRUE(get_variable_registers()[0xF]);
}

// 0x8XY6
void test_shift_right()
{
    struct cpu_status status;

    debug_run_instruction(0x60C0);  // Set V0 to 0b11000000

    status = debug_run_instruction(0x8006);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(0b01100000, get_variable_registers()[0x0]);
    TEST_ASSERT_FALSE(get_variable_registers()[0xF]);
}

void test_shift_right_with_carry()
{
    struct cpu_status status;

    debug_run_instruction(0x6003);  // Set V0 to 0b00000011

    status = debug_run_instruction(0x8006);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(0b00000001, get_variable_registers()[0x0]);
    TEST_ASSERT_TRUE(get_variable_registers()[0xF]);
}

// 0x8XY2
void test_and()
{
    struct cpu_status status;

    debug_run_instruction(0x60AC);  // Set V0 to 0b10101100
    debug_run_instruction(0x6156);  // Set V1 to 0b01010110

    status = debug_run_instruction(0x8012);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(0b00000100, get_variable_registers()[0x0]);
}

// 0x8XY1
void test_or()
{
    struct cpu_status status;

    debug_run_instruction(0x60AC);  // Set V0 to 0b10101100
    debug_run_instruction(0x6156);  // Set V1 to 0b01010110

    status = debug_run_instruction(0x8011);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(0b11111110, get_variable_registers()[0x0]);
}

// 0x8XY3
void test_xor()
{
    struct cpu_status status;

    debug_run_instruction(0x60AC);  // Set V0 to 0b10101100
    debug_run_instruction(0x6156);  // Set V1 to 0b01010110

    status = debug_run_instruction(0x8013);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(0b11111010, get_variable_registers()[0x0]);
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

// 0xBNNN
void test_jump_with_offset_updates_program_counter()
{
    struct cpu_status status;

    status = debug_run_instruction(0xB100);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(0x100, get_program_counter());

    debug_run_instruction(0x600F);
    status = debug_run_instruction(0xB100);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(0x10F, get_program_counter());
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

// 0xFX1E
void test_add_index_updates_index_register()
{
    struct cpu_status status;

    debug_run_instruction(0x6002);
    debug_run_instruction(0xA300);

    status = debug_run_instruction(0xF01E);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(0x302, get_index_register());
    TEST_ASSERT_FALSE(get_variable_registers()[0xF]);
}

void test_add_index_updates_index_register_with_carry()
{
    struct cpu_status status;

    debug_run_instruction(0x6002);
    debug_run_instruction(0xAFFF);

    status = debug_run_instruction(0xF01E);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(0x001, get_index_register());
    TEST_ASSERT_TRUE(get_variable_registers()[0xF]);
}

// 0xFX29
void test_font_character()
{
    struct cpu_status status;

    debug_run_instruction(0x6001);
    status = debug_run_instruction(0xF029);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(FONT_START + 5, get_index_register());

    debug_run_instruction(0x6004);
    status = debug_run_instruction(0xF029);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_INT16(FONT_START + 20, get_index_register());
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

// 0xFX33
void test_decimal_conversion()
{
    struct cpu_status status;
    uint8_t *memory = get_memory_pointer(get_index_register());

    debug_run_instruction(0x6001);  // Set V0 to 1.

    status = debug_run_instruction(0xF033);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(1, memory[0]);

    debug_run_instruction(0x600F);  // Set V0 to 15.

    status = debug_run_instruction(0xF033);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(1, memory[0]);
    TEST_ASSERT_EQUAL_UINT8(5, memory[1]);

    debug_run_instruction(0x609C);  // Set V0 to 156.

    status = debug_run_instruction(0xF033);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(1, memory[0]);
    TEST_ASSERT_EQUAL_UINT8(5, memory[1]);
    TEST_ASSERT_EQUAL_UINT8(6, memory[2]);
}

// 0xFX55
void test_store_memory()
{
    struct cpu_status status;
    uint8_t *memory = get_memory_pointer(get_index_register());

    // Add data to variable registers.
    debug_run_instruction(0x6001);
    debug_run_instruction(0x6102);
    debug_run_instruction(0x6203);
    debug_run_instruction(0x6304);

    // Intentionally exclude last register to test upper bound of storage.
    status = debug_run_instruction(0xF255);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(1, memory[0]);
    TEST_ASSERT_EQUAL_UINT8(2, memory[1]);
    TEST_ASSERT_EQUAL_UINT8(3, memory[2]);
    // Default value, as V3 should be excluded.
    TEST_ASSERT_EQUAL_UINT8(0, memory[3]);
}

// 0xFX65
void test_load_memory()
{
    struct cpu_status status;
    uint8_t *memory = get_memory_pointer(get_index_register());
    uint8_t *variables = get_variable_registers();

    // Add data to memory.
    for (uint8_t i = 0; i < 4; i++) {
        memory[i] = i + 1;
    }

    // Intentionally exclude last register to test upper bound of loading.
    status = debug_run_instruction(0xF265);
    TEST_ASSERT_EQUAL_UINT8(SUCCESS, status.code);
    TEST_ASSERT_EQUAL_UINT8(1, variables[0]);
    TEST_ASSERT_EQUAL_UINT8(2, variables[1]);
    TEST_ASSERT_EQUAL_UINT8(3, variables[2]);
    // Default value, as V3 should be excluded.
    TEST_ASSERT_EQUAL_UINT8(0, variables[3]);
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
    RUN_TEST(test_set_vx_to_vy);
    RUN_TEST(test_add_vy_to_vx);
    RUN_TEST(test_add_vy_to_vx_with_carry);
    RUN_TEST(test_subtract_vy_from_vx);
    RUN_TEST(test_subtract_vy_from_vx_with_carry);
    RUN_TEST(test_subtract_vx_from_vy);
    RUN_TEST(test_subtract_vx_from_vy_with_carry);
    RUN_TEST(test_shift_left);
    RUN_TEST(test_shift_left_with_carry);
    RUN_TEST(test_shift_right);
    RUN_TEST(test_shift_right_with_carry);
    RUN_TEST(test_and);
    RUN_TEST(test_or);
    RUN_TEST(test_xor);
    RUN_TEST(test_jump_updates_program_counter);
    RUN_TEST(test_jump_with_offset_updates_program_counter);
    RUN_TEST(test_skip_if_variable_equal_constant);
    RUN_TEST(test_skip_if_variable_not_equal_constant);
    RUN_TEST(test_skip_if_variable_equal_variable);
    RUN_TEST(test_skip_if_variable_not_equal_variable);
    RUN_TEST(test_set_index_updates_index_register);
    RUN_TEST(test_add_index_updates_index_register);
    RUN_TEST(test_add_index_updates_index_register_with_carry);
    RUN_TEST(test_font_character);
    RUN_TEST(test_draw_renders_sprite);
    RUN_TEST(test_clear_display_clears_display);
    RUN_TEST(test_decimal_conversion);
    RUN_TEST(test_store_memory);
    RUN_TEST(test_load_memory);
    RUN_TEST(test_read_instruction_reads_and_moves_pc);
    RUN_TEST(test_run_cycle_reads_and_executes_instruction);
    return UNITY_END();
}
