#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FONTSET_START_ADDRESS 0x50

static const uint8_t chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(Chip8 *chip8) {
    memset(chip8, 0, sizeof(Chip8));
    chip8->pc = 0x200; // ROM starts loading here
    chip8->wait_key_reg = -1;
    
    // Load font set into memory
    for (int i = 0; i < 80; ++i) {
        chip8->memory[FONTSET_START_ADDRESS + i] = chip8_fontset[i];
    }
    
    // Seed random number generator
    srand((unsigned int)time(NULL));
}

bool chip8_load_rom(Chip8 *chip8, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Could not open ROM file '%s'\n", filename);
        return false;
    }
    
    // Seek to end to find file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Check if it fits in memory (max 3584 bytes from 0x200 to 0xFFF)
    long max_size = CHIP8_MEMORY_SIZE - 0x200;
    if (size > max_size) {
        printf("Error: ROM file size (%ld bytes) exceeds maximum available memory (%ld bytes)\n", size, max_size);
        fclose(file);
        return false;
    }
    
    // Read directly into RAM starting at 0x200
    size_t bytes_read = fread(chip8->memory + 0x200, 1, size, file);
    fclose(file);
    
    if (bytes_read != (size_t)size) {
        printf("Error: Could not read full ROM file (read %zu of %ld bytes)\n", bytes_read, size);
        return false;
    }
    
    printf("Successfully loaded %zu bytes from '%s' into memory\n", bytes_read, filename);
    return true;
}

void chip8_cycle(Chip8 *chip8) {
    // Fetch opcode
    uint16_t opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
    
    // Move PC forward
    chip8->pc += 2;
    
    // Decode parameters
    uint8_t u = (opcode & 0xF000) >> 12; // first nibble (4 bits)
    uint8_t x = (opcode & 0x0F00) >> 8;  // second nibble (V register index)
    uint8_t y = (opcode & 0x00F0) >> 4;  // third nibble (V register index)
    uint8_t n = (opcode & 0x000F);       // fourth nibble (4-bit value)
    uint8_t kk = (opcode & 0x00FF);      // lower 8 bits (byte value)
    uint16_t nnn = (opcode & 0x0FFF);    // lower 12 bits (memory address)

    switch (u) {
        case 0x0:
            if (kk == 0xE0) { // 00E0 - CLS
                memset(chip8->video, 0, sizeof(chip8->video));
            } else if (kk == 0xEE) { // 00EE - RET
                if (chip8->sp > 0) {
                    chip8->sp--;
                    chip8->pc = chip8->stack[chip8->sp];
                } else {
                    printf("Error: Stack Underflow!\n");
                }
            }
            break;
            
        case 0x1: // 1NNN - JP addr
            chip8->pc = nnn;
            break;
            
        case 0x2: // 2NNN - CALL addr
            if (chip8->sp < CHIP8_STACK_SIZE) {
                chip8->stack[chip8->sp] = chip8->pc;
                chip8->sp++;
                chip8->pc = nnn;
            } else {
                printf("Error: Stack Overflow!\n");
            }
            break;
            
        case 0x3: // 3XKK - SE Vx, byte
            if (chip8->V[x] == kk) {
                chip8->pc += 2;
            }
            break;
            
        case 0x4: // 4XKK - SNE Vx, byte
            if (chip8->V[x] != kk) {
                chip8->pc += 2;
            }
            break;
            
        case 0x5: // 5XY0 - SE Vx, Vy
            if (chip8->V[x] == chip8->V[y]) {
                chip8->pc += 2;
            }
            break;
            
        case 0x6: // 6XKK - LD Vx, byte
            chip8->V[x] = kk;
            break;
            
        case 0x7: // 7XKK - ADD Vx, byte
            chip8->V[x] += kk;
            break;
            
        case 0x8:
            switch (n) {
                case 0x0: // 8XY0 - LD Vx, Vy
                    chip8->V[x] = chip8->V[y];
                    break;
                case 0x1: // 8XY1 - OR Vx, Vy
                    chip8->V[x] |= chip8->V[y];
                    break;
                case 0x2: // 8XY2 - AND Vx, Vy
                    chip8->V[x] &= chip8->V[y];
                    break;
                case 0x3: // 8XY3 - XOR Vx, Vy
                    chip8->V[x] ^= chip8->V[y];
                    break;
                case 0x4: { // 8XY4 - ADD Vx, Vy (with carry)
                    uint16_t sum = chip8->V[x] + chip8->V[y];
                    chip8->V[x] = sum & 0xFF;
                    chip8->V[0xF] = (sum > 255) ? 1 : 0;
                    break;
                }
                case 0x5: { // 8XY5 - SUB Vx, Vy
                    uint8_t carry = (chip8->V[x] >= chip8->V[y]) ? 1 : 0;
                    chip8->V[x] -= chip8->V[y];
                    chip8->V[0xF] = carry;
                    break;
                }
                case 0x6: { // 8XY6 - SHR Vx {, Vy}
                    uint8_t carry = chip8->V[x] & 0x1;
                    chip8->V[x] >>= 1;
                    chip8->V[0xF] = carry;
                    break;
                }
                case 0x7: { // 8XY7 - SUBN Vx, Vy
                    uint8_t carry = (chip8->V[y] >= chip8->V[x]) ? 1 : 0;
                    chip8->V[x] = chip8->V[y] - chip8->V[x];
                    chip8->V[0xF] = carry;
                    break;
                }
                case 0xE: { // 8XYE - SHL Vx {, Vy}
                    uint8_t carry = (chip8->V[x] & 0x80) >> 7;
                    chip8->V[x] <<= 1;
                    chip8->V[0xF] = carry;
                    break;
                }
            }
            break;
            
        case 0x9: // 9XY0 - SNE Vx, Vy
            if (chip8->V[x] != chip8->V[y]) {
                chip8->pc += 2;
            }
            break;
            
        case 0xA: // ANNN - LD I, addr
            chip8->I = nnn;
            break;
            
        case 0xB: // BNNN - JP V0, addr
            chip8->pc = nnn + chip8->V[0];
            break;
            
        case 0xC: // CXKK - RND Vx, byte
            chip8->V[x] = (rand() % 256) & kk;
            break;
            
        case 0xD: { // DXYN - DRW Vx, Vy, nibble
            uint8_t x_pos = chip8->V[x] % CHIP8_VIDEO_WIDTH;
            uint8_t y_pos = chip8->V[y] % CHIP8_VIDEO_HEIGHT;
            chip8->V[0xF] = 0;
            
            for (unsigned int row = 0; row < n; ++row) {
                if (chip8->I + row >= CHIP8_MEMORY_SIZE) break;
                uint8_t sprite_byte = chip8->memory[chip8->I + row];
                for (unsigned int col = 0; col < 8; ++col) {
                    uint8_t sprite_pixel = sprite_byte & (0x80 >> col);
                    if (sprite_pixel) {
                        uint16_t screen_x = (x_pos + col) % CHIP8_VIDEO_WIDTH;
                        uint16_t screen_y = (y_pos + row) % CHIP8_VIDEO_HEIGHT;
                        uint32_t *screen_pixel = &chip8->video[screen_y * CHIP8_VIDEO_WIDTH + screen_x];
                        
                        // Check collision
                        if (*screen_pixel == 0xFFFFFFFF) {
                            chip8->V[0xF] = 1;
                        }
                        *screen_pixel ^= 0xFFFFFFFF; // XOR color (white / black)
                    }
                }
            }
            break;
        }
        
        case 0xE:
            if (kk == 0x9E) { // EX9E - SKP Vx
                uint8_t key = chip8->V[x] & 0xF;
                if (chip8->keypad[key]) {
                    chip8->pc += 2;
                }
            } else if (kk == 0xA1) { // EXA1 - SKNP Vx
                uint8_t key = chip8->V[x] & 0xF;
                if (!chip8->keypad[key]) {
                    chip8->pc += 2;
                }
            }
            break;
            
        case 0xF:
            switch (kk) {
                case 0x07: // FX07 - LD Vx, DT
                    chip8->V[x] = chip8->delay_timer;
                    break;
                case 0x0A: { // FX0A - LD Vx, K
                    if (chip8->wait_key_reg == -1) {
                        bool key_pressed = false;
                        for (int i = 0; i < 16; ++i) {
                            if (chip8->keypad[i]) {
                                chip8->V[x] = i;
                                chip8->wait_key_reg = x;
                                key_pressed = true;
                                break;
                            }
                        }
                        if (!key_pressed) {
                            chip8->pc -= 2; // Keep repeating this instruction
                        }
                    } else {
                        uint8_t key_val = chip8->V[chip8->wait_key_reg];
                        if (chip8->keypad[key_val]) {
                            chip8->pc -= 2; // Key is still pressed, keep waiting
                        } else {
                            chip8->wait_key_reg = -1; // Released!
                        }
                    }
                    break;
                }
                case 0x15: // FX15 - LD DT, Vx
                    chip8->delay_timer = chip8->V[x];
                    break;
                case 0x18: // FX18 - LD ST, Vx
                    chip8->sound_timer = chip8->V[x];
                    break;
                case 0x1E: // FX1E - ADD I, Vx
                    chip8->I += chip8->V[x];
                    break;
                case 0x29: // FX29 - LD F, Vx
                    chip8->I = FONTSET_START_ADDRESS + (chip8->V[x] & 0xF) * 5;
                    break;
                case 0x33: { // FX33 - LD B, Vx
                    uint8_t val = chip8->V[x];
                    chip8->memory[chip8->I]     = val / 100;
                    chip8->memory[chip8->I + 1] = (val / 10) % 10;
                    chip8->memory[chip8->I + 2] = val % 10;
                    break;
                }
                case 0x55: // FX55 - LD [I], Vx
                    for (int i = 0; i <= x; ++i) {
                        chip8->memory[chip8->I + i] = chip8->V[i];
                    }
                    break;
                case 0x65: // FX65 - LD Vx, [I]
                    for (int i = 0; i <= x; ++i) {
                        chip8->V[i] = chip8->memory[chip8->I + i];
                    }
                    break;
            }
            break;
            
        default:
            printf("Error: Unknown opcode 0x%04X\n", opcode);
            break;
    }
}
