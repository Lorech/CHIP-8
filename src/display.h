#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdbool.h>
#include <stdint.h>

// Standard CHIP-8 screen parameters.
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define TARGET_FRAMERATE 60

#define SCALING_FACTOR 10  // Scale of the screen to make it visible.

/**
 * Fully clears the display, resetting it to the base color.
 *
 * Corresponds to the CPU instruction 0x00E0.
 */
void clear_display();

/**
 * Draws the provided sprite at the provided x and y coordinates.
 *
 * Corresponds to the CPU instruction 0xDXYN.
 *
 * @param x The horizontal offset at which to start drawing the sprite.
 * @param y The vertical offset at which to start drawing the sprite.
 * @param h The height at which to draw the sprite.
 * @param sprite The bytes of the sprite, where active bits should be inverted.
 * @return The new status of the VF register, based on a pixel collision.
 */
bool draw_sprite(uint8_t x, uint8_t y, uint8_t h, uint8_t *sprite);

/**
 * An abstraction for CPU drawing functions onto Raylib.
 *
 * Handles drawing a local 2D array of pixels onto the Raylib window, allowing
 * the non-static functions to directly correspond to CPU instructions, without
 * introducing additional complexity for dealing with Raylib.
 */
static void draw();

#ifdef UNIT_TEST
/**
 * Retrieves the display array for testing purposes.
 *
 * @return The display array.
 */
bool (*get_display())[SCREEN_WIDTH];
#endif  // !UNIT_TEST

#endif  // !DISPLAY_H_
