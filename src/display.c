#include "display.h"

#include <stdint.h>

#ifndef UNIT_TEST
#include "raylib.h"
#endif  // !UNIT_TEST

static bool display[SCREEN_HEIGHT][SCREEN_WIDTH];

void clear_display()
{
    for (uint8_t y = 0; y < SCREEN_HEIGHT; y++) {
        for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {
            display[y][x] = 0;
        }
    }

    draw();
}

bool draw_sprite(uint8_t x, uint8_t y, uint8_t h, uint8_t *sprite_data)
{
    // Wrap the starting coordinates if they are out of bounds.
    x = x % SCREEN_WIDTH;
    y = y % SCREEN_HEIGHT;

    bool vf = false;

    for (uint8_t row = 0; row < h; row++) {
        uint8_t sprite = sprite_data[row];
        for (uint8_t col = 0; col < 8; col++) {
            // Clip the sprite if it would overflow the screen.
            if (y + row >= SCREEN_HEIGHT || x + col >= SCREEN_WIDTH) {
                continue;
            }

            if (sprite & (0x80 >> col)) {
                if (display[y + row][x + col]) {
                    vf = true;
                }
                display[y + row][x + col] ^= 1;
            }
        }
    }

    draw();
    return vf;
}

static void draw()
{
#ifndef UNIT_TEST
    BeginDrawing();
    ClearBackground(BLACK);
    for (uint8_t y = 0; y < SCREEN_HEIGHT; y++) {
        for (uint8_t x = 0; x < SCREEN_WIDTH; x++) {
            if (display[y][x]) {
                DrawRectangle(
                    x * SCALING_FACTOR,
                    y * SCALING_FACTOR,
                    SCALING_FACTOR,
                    SCALING_FACTOR,
                    RAYWHITE);
            }
        }
    }
    EndDrawing();
#endif  // !UNIT_TEST
}

#ifdef UNIT_TEST
bool (*get_display())[SCREEN_WIDTH]
{
    return display;
}
#endif  // !UNIT_TEST
