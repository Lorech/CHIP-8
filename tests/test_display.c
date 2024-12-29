#include <stdint.h>

#include "display.h"
#include "macros.h"
#include "unity.h"

void setUp()
{
    clear_display();
}

void tearDown()
{
    return;
}

void test_clear_display_clears_display()
{
    bool(*display)[SCREEN_WIDTH] = get_display();
    for (uint8_t y = 0; y < SCREEN_HEIGHT; y++) {
        for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {
            TEST_ASSERT_FALSE(display[y][x]);
        }
    }
}

void test_draw_sprite_draws_sprite()
{
    uint8_t sprite[6] = {0x6C, 0xFE, 0xFE, 0x7C, 0x38, 0x10};
    bool vf = draw_sprite(0, 0, len(sprite), sprite);
    bool expected[6][8] = {
        {0, 1, 1, 0, 1, 1, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0},
    };

    bool(*display)[SCREEN_WIDTH] = get_display();
    for (uint8_t y = 0; y < 6; y++) {
        for (uint8_t x = 0; x < 8; x++) {
            TEST_ASSERT_EQUAL(expected[y][x], display[y][x]);
        }
    }

    TEST_ASSERT_FALSE(vf);
}

void test_draw_sprite_draws_sprite_with_collision()
{
    uint8_t sprite[6] = {0x6C, 0xFE, 0xFE, 0x7C, 0x38, 0x10};

    // Draw the sprite twice, effectively clearing the display with a collision.
    bool vf;
    for (uint8_t y = 0; y < 2; y++) {
        vf = draw_sprite(0, 0, len(sprite), sprite);
    }

    bool(*display)[SCREEN_WIDTH] = get_display();
    for (uint8_t y = 0; y < 6; y++) {
        for (uint8_t x = 0; x < 8; x++) {
            TEST_ASSERT_FALSE(display[y][x]);
        }
    }

    TEST_ASSERT_TRUE(vf);
}

void test_draw_sprite_wraps_cursor()
{
    uint8_t sprite[6] = {0x6C, 0xFE, 0xFE, 0x7C, 0x38, 0x10};
    // Overflow the cursor back to 0,0.
    bool vf = draw_sprite(SCREEN_WIDTH, SCREEN_HEIGHT, len(sprite), sprite);
    bool expected[6][8] = {
        {0, 1, 1, 0, 1, 1, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1, 0},
        {0, 1, 1, 1, 1, 1, 0, 0},
        {0, 0, 1, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0},
    };

    bool(*display)[SCREEN_WIDTH] = get_display();
    for (uint8_t y = 0; y < 6; y++) {
        for (uint8_t x = 0; x < 8; x++) {
            TEST_ASSERT_EQUAL(expected[y][x], display[y][x]);
        }
    }

    TEST_ASSERT_FALSE(vf);
}

void test_draw_sprite_clips_sprite()
{
    uint8_t sprite[6] = {0x6C, 0xFE, 0xFE, 0x7C, 0x38, 0x10};
    // Draw the top left corner of the sprite until it overlaps the edges.
    bool vf =
        draw_sprite(SCREEN_WIDTH - 2, SCREEN_HEIGHT - 2, len(sprite), sprite);
    bool expected[6][8] = {
        {0, 1, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
    };

    bool(*display)[SCREEN_WIDTH] = get_display();
    for (uint8_t y = SCREEN_WIDTH - 2; y < 6; y++) {
        for (uint8_t x = SCREEN_HEIGHT - 2; x < 8; x++) {
            TEST_ASSERT_EQUAL(
                expected[y % SCREEN_HEIGHT][x % SCREEN_WIDTH],
                display[y % SCREEN_HEIGHT][x % SCREEN_WIDTH]);
        }
    }

    TEST_ASSERT_FALSE(vf);
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_clear_display_clears_display);
    RUN_TEST(test_draw_sprite_draws_sprite);
    RUN_TEST(test_draw_sprite_draws_sprite_with_collision);
    RUN_TEST(test_draw_sprite_wraps_cursor);
    RUN_TEST(test_draw_sprite_clips_sprite);
    return UNITY_END();
}
