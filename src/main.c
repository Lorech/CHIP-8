#include <stdint.h>
#include <stdio.h>

#include "cpu.h"
#include "display.h"
#include "raylib.h"

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("No ROM path provided!");
        return 1;
    }

    InitWindow(
        SCREEN_WIDTH * SCALING_FACTOR,
        SCREEN_HEIGHT * SCALING_FACTOR,
        "CHIP-8");
    SetTargetFPS(TARGET_FRAMERATE);

    startup(argv[1]);
    const int instructionsPerFrame = INSTRUCTIONS_PER_SECOND / TARGET_FRAMERATE;

    while (!WindowShouldClose()) {
        for (uint8_t i = 0; i < instructionsPerFrame; i++) {
            struct cpu_status status = run_cycle();
            if (status.code) {
                printf(
                    "WARNING: CPU error %d while executing instruction %04X.\n",
                    status.code,
                    status.instruction);
                break;
            }
        }
    }

    CloseWindow();

    return 0;
}
