#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>

#define CHIP8_VIDEO_WIDTH 64
#define CHIP8_VIDEO_HEIGHT 32
#define CHIP8_MEMORY_SIZE 4096
#define CHIP8_REGISTER_COUNT 16
#define CHIP8_STACK_SIZE 16
#define CHIP8_KEY_COUNT 16

typedef struct {
    uint8_t memory[CHIP8_MEMORY_SIZE];
    uint8_t V[CHIP8_REGISTER_COUNT];
    uint16_t I;
    uint16_t pc;
    uint16_t stack[CHIP8_STACK_SIZE];
    uint8_t sp;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t keypad[CHIP8_KEY_COUNT];
    uint32_t video[CHIP8_VIDEO_WIDTH * CHIP8_VIDEO_HEIGHT];
    int16_t wait_key_reg;
} Chip8;

// Initialize the emulator state (registers, memory, fontset)
void chip8_init(Chip8 *chip8);

// Load ROM binary into emulator memory starting at 0x200
bool chip8_load_rom(Chip8 *chip8, const char *filename);

// Execute one Fetch-Decode-Execute instruction cycle
void chip8_cycle(Chip8 *chip8);

#endif // CHIP8_H
