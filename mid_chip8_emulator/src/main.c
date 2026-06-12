#include <stdio.h>
#include <stdbool.h>
#include "chip8.h"

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

// Application States
typedef enum {
    STATE_MENU,
    STATE_EMULATOR
} AppState;

// Mapping QWERTY keyboard to CHIP-8 Hex Keypad:
// 1 2 3 4  ->  1 2 3 C
// Q W E R  ->  4 5 6 D
// A S D F  ->  7 8 9 E
// Z X C V  ->  A 0 B F
void handle_key_event(Chip8 *chip8, SDL_Event *event, uint8_t state) {
    switch (event->key.keysym.sym) {
        case SDLK_1: chip8->keypad[1] = state; break;
        case SDLK_2: chip8->keypad[2] = state; break;
        case SDLK_3: chip8->keypad[3] = state; break;
        case SDLK_4: chip8->keypad[0xC] = state; break;
        
        case SDLK_q: chip8->keypad[4] = state; break;
        case SDLK_w: chip8->keypad[5] = state; break;
        case SDLK_e: chip8->keypad[6] = state; break;
        case SDLK_r: chip8->keypad[0xD] = state; break;
        
        case SDLK_a: chip8->keypad[7] = state; break;
        case SDLK_s: chip8->keypad[8] = state; break;
        case SDLK_d: chip8->keypad[9] = state; break;
        case SDLK_f: chip8->keypad[0xE] = state; break;
        
        case SDLK_z: chip8->keypad[0xA] = state; break;
        case SDLK_x: chip8->keypad[0]   = state; break;
        case SDLK_c: chip8->keypad[0xB] = state; break;
        case SDLK_v: chip8->keypad[0xF] = state; break;
    }
}

// Retro Pixel Line Font drawing helpers
void draw_line(SDL_Renderer *r, int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(r, x1, y1, x2, y2);
}

void draw_pixel_char(SDL_Renderer *r, char c, int x, int y) {
    switch (c) {
        case 'A':
            draw_line(r, x, y+8, x, y);
            draw_line(r, x, y, x+6, y);
            draw_line(r, x+6, y, x+6, y+8);
            draw_line(r, x, y+4, x+6, y+4);
            break;
        case 'B':
            draw_line(r, x, y, x, y+8);
            draw_line(r, x, y, x+5, y);
            draw_line(r, x+5, y, x+5, y+4);
            draw_line(r, x+5, y+4, x, y+4);
            draw_line(r, x+5, y+4, x+5, y+8);
            draw_line(r, x+5, y+8, x, y+8);
            break;
        case 'C':
            draw_line(r, x, y, x+6, y);
            draw_line(r, x, y, x, y+8);
            draw_line(r, x, y+8, x+6, y+8);
            break;
        case 'D':
            draw_line(r, x, y, x, y+8);
            draw_line(r, x, y, x+4, y);
            draw_line(r, x+4, y, x+6, y+2);
            draw_line(r, x+6, y+2, x+6, y+6);
            draw_line(r, x+6, y+6, x+4, y+8);
            draw_line(r, x+4, y+8, x, y+8);
            break;
        case 'E':
            draw_line(r, x, y, x, y+8);
            draw_line(r, x, y, x+6, y);
            draw_line(r, x, y+4, x+4, y+4);
            draw_line(r, x, y+8, x+6, y+8);
            break;
        case 'F':
            draw_line(r, x, y, x, y+8);
            draw_line(r, x, y, x+6, y);
            draw_line(r, x, y+4, x+4, y+4);
            break;
        case 'G':
            draw_line(r, x, y, x+6, y);
            draw_line(r, x, y, x, y+8);
            draw_line(r, x, y+8, x+6, y+8);
            draw_line(r, x+6, y+8, x+6, y+4);
            draw_line(r, x+4, y+4, x+6, y+4);
            break;
        case 'H':
            draw_line(r, x, y, x, y+8);
            draw_line(r, x+6, y, x+6, y+8);
            draw_line(r, x, y+4, x+6, y+4);
            break;
        case 'I':
            draw_line(r, x, y, x+4, y);
            draw_line(r, x+2, y, x+2, y+8);
            draw_line(r, x, y+8, x+4, y+8);
            break;
        case 'L':
            draw_line(r, x, y, x, y+8);
            draw_line(r, x, y+8, x+6, y+8);
            break;
        case 'M':
            draw_line(r, x, y, x, y+8);
            draw_line(r, x, y, x+3, y+3);
            draw_line(r, x+6, y, x+3, y+3);
            draw_line(r, x+6, y, x+6, y+8);
            break;
        case 'N':
            draw_line(r, x, y, x, y+8);
            draw_line(r, x, y, x+6, y+8);
            draw_line(r, x+6, y, x+6, y+8);
            break;
        case 'O':
            draw_line(r, x, y, x+6, y);
            draw_line(r, x, y, x, y+8);
            draw_line(r, x+6, y, x+6, y+8);
            draw_line(r, x, y+8, x+6, y+8);
            break;
        case 'P':
            draw_line(r, x, y, x, y+8);
            draw_line(r, x, y, x+6, y);
            draw_line(r, x+6, y, x+6, y+4);
            draw_line(r, x+6, y+4, x, y+4);
            break;
        case 'R':
            draw_line(r, x, y, x, y+8);
            draw_line(r, x, y, x+6, y);
            draw_line(r, x+6, y, x+6, y+4);
            draw_line(r, x+6, y+4, x, y+4);
            draw_line(r, x+2, y+4, x+6, y+8);
            break;
        case 'S':
            draw_line(r, x+6, y, x, y);
            draw_line(r, x, y, x, y+4);
            draw_line(r, x, y+4, x+6, y+4);
            draw_line(r, x+6, y+4, x+6, y+8);
            draw_line(r, x+6, y+8, x, y+8);
            break;
        case 'T':
            draw_line(r, x, y, x+6, y);
            draw_line(r, x+3, y, x+3, y+8);
            break;
        case 'U':
            draw_line(r, x, y, x, y+8);
            draw_line(r, x+6, y, x+6, y+8);
            draw_line(r, x+8, y+8, x+6, y+8); // wait, fixed bottom
            draw_line(r, x, y+8, x+6, y+8);
            break;
        case 'X':
            draw_line(r, x, y, x+6, y+8);
            draw_line(r, x+6, y, x, y+8);
            break;
        case 'Y':
            draw_line(r, x, y, x+3, y+4);
            draw_line(r, x+6, y, x+3, y+4);
            draw_line(r, x+3, y+4, x+3, y+8);
            break;
        case '-':
            draw_line(r, x+1, y+4, x+5, y+4);
            break;
        case '8':
            draw_line(r, x, y, x, y+8);
            draw_line(r, x+6, y, x+6, y+8);
            draw_line(r, x, y, x+6, y);
            draw_line(r, x, y+4, x+6, y+4);
            draw_line(r, x, y+8, x+6, y+8);
            break;
        case ' ':
            // space
            break;
    }
}

void draw_pixel_string(SDL_Renderer *r, const char *text, int x, int y) {
    int cur_x = x;
    while (*text) {
        draw_pixel_char(r, *text, cur_x, y);
        cur_x += 10;
        text++;
    }
}

int main(int argc, char **argv) {
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    
    int scale = 12; // Scale factor for window
    int window_width = CHIP8_VIDEO_WIDTH * scale;
    int window_height = CHIP8_VIDEO_HEIGHT * scale;
    
    SDL_Window *window = SDL_CreateWindow(
        "CHIP-8 OS Dashboard",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Create texture for CHIP-8 emulation display
    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        CHIP8_VIDEO_WIDTH,
        CHIP8_VIDEO_HEIGHT
    );
    
    Chip8 chip8;
    AppState state = STATE_MENU;
    bool running = true;
    SDL_Event event;
    
    // Layout boundaries for button regions in the menu state
    // IBM Button: x=80 to x=320, y=90 to y=310
    // Calculator Button: x=448 to x=688, y=90 to y=310
    SDL_Rect ibm_rect = {80, 90, 240, 220};
    SDL_Rect calc_rect = {448, 90, 240, 220};
    
    bool hover_ibm = false;
    bool hover_calc = false;
    
    const int cycles_per_frame = 10;
    Uint32 start_time = SDL_GetTicks();
    
    printf("OS Launcher initialized. Choose program via graphical UI.\n");
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            
            if (state == STATE_MENU) {
                if (event.type == SDL_MOUSEMOTION) {
                    int mx = event.motion.x;
                    int my = event.motion.y;
                    
                    hover_ibm = (mx >= ibm_rect.x && mx <= ibm_rect.x + ibm_rect.w &&
                                 my >= ibm_rect.y && my <= ibm_rect.y + ibm_rect.h);
                    hover_calc = (mx >= calc_rect.x && mx <= calc_rect.x + calc_rect.w &&
                                  my >= calc_rect.y && my <= calc_rect.y + calc_rect.h);
                }
                else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int mx = event.button.x;
                    int my = event.button.y;
                    
                    if (mx >= ibm_rect.x && mx <= ibm_rect.x + ibm_rect.w &&
                        my >= ibm_rect.y && my <= ibm_rect.y + ibm_rect.h) {
                        printf("Launching IBM Logo Program...\n");
                        chip8_init(&chip8);
                        if (chip8_load_rom(&chip8, "roms/ibm.ch8")) {
                            state = STATE_EMULATOR;
                        }
                    }
                    else if (mx >= calc_rect.x && mx <= calc_rect.x + calc_rect.w &&
                             my >= calc_rect.y && my <= calc_rect.y + calc_rect.h) {
                        printf("Launching Calculator Program...\n");
                        chip8_init(&chip8);
                        if (chip8_load_rom(&chip8, "roms/calc.ch8")) {
                            state = STATE_EMULATOR;
                        }
                    }
                }
                else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                    // Keyboard shortcut launchers: press '1' for IBM, '2' for Calc
                    if (event.key.keysym.sym == SDLK_1) {
                        chip8_init(&chip8);
                        if (chip8_load_rom(&chip8, "roms/ibm.ch8")) state = STATE_EMULATOR;
                    }
                    if (event.key.keysym.sym == SDLK_2) {
                        chip8_init(&chip8);
                        if (chip8_load_rom(&chip8, "roms/calc.ch8")) state = STATE_EMULATOR;
                    }
                }
            } 
            else if (state == STATE_EMULATOR) {
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        printf("Returning to Main Dashboard menu...\n");
                        state = STATE_MENU;
                        hover_ibm = false;
                        hover_calc = false;
                        // Clear event states
                        memset(chip8.keypad, 0, sizeof(chip8.keypad));
                    } else {
                        handle_key_event(&chip8, &event, 1);
                    }
                } else if (event.type == SDL_KEYUP) {
                    handle_key_event(&chip8, &event, 0);
                }
            }
        }
        
        // Render step
        if (state == STATE_MENU) {
            // Draw Main Menu Screen
            // Background: Dark Gray-Blue (tailwind-like dark background)
            SDL_SetRenderDrawColor(renderer, 17, 24, 39, 255);
            SDL_RenderClear(renderer);
            
            // Draw Top bar header
            SDL_Rect top_bar = {0, 0, window_width, 50};
            SDL_SetRenderDrawColor(renderer, 79, 70, 229, 255); // Indigo
            SDL_RenderFillRect(renderer, &top_bar);
            
            // Neon underline
            SDL_Rect neon_line = {0, 50, window_width, 3};
            SDL_SetRenderDrawColor(renderer, 129, 140, 248, 255); // Light Indigo
            SDL_RenderFillRect(renderer, &neon_line);
            
            // Title Text
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            draw_pixel_string(renderer, "CHIP-8 OS LAUNCHER DASHBOARD", 240, 20);
            
            // Draw IBM Button card
            if (hover_ibm) {
                SDL_SetRenderDrawColor(renderer, 55, 65, 81, 255); // Hover Gray
                SDL_RenderFillRect(renderer, &ibm_rect);
                SDL_SetRenderDrawColor(renderer, 59, 130, 246, 255); // Glowing Blue Border
            } else {
                SDL_SetRenderDrawColor(renderer, 31, 41, 55, 255); // Normal Gray
                SDL_RenderFillRect(renderer, &ibm_rect);
                SDL_SetRenderDrawColor(renderer, 75, 85, 99, 255); // Dull Border
            }
            SDL_RenderDrawRect(renderer, &ibm_rect);
            
            // Draw IBM monitor Icon
            // Frame
            SDL_Rect ibm_monitor = {115, 110, 170, 100};
            SDL_SetRenderDrawColor(renderer, 156, 163, 175, 255); // Silver
            SDL_RenderDrawRect(renderer, &ibm_monitor);
            // Stand
            draw_line(renderer, 180, 210, 170, 230);
            draw_line(renderer, 220, 210, 230, 230);
            draw_line(renderer, 160, 230, 240, 230);
            // Screen (Black)
            SDL_Rect ibm_screen = {125, 120, 150, 80};
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &ibm_screen);
            // Draw Cyan IBM label inside monitor screen
            SDL_SetRenderDrawColor(renderer, 34, 211, 238, 255); // Cyan
            draw_pixel_char(renderer, 'I', 165, 155);
            draw_pixel_char(renderer, 'B', 190, 155);
            draw_pixel_char(renderer, 'M', 215, 155);
            
            // Draw Button labels
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            draw_pixel_string(renderer, "IBM LOGO DEMO", 125, 260);
            
            
            // Draw Calculator Button card
            if (hover_calc) {
                SDL_SetRenderDrawColor(renderer, 55, 65, 81, 255);
                SDL_RenderFillRect(renderer, &calc_rect);
                SDL_SetRenderDrawColor(renderer, 16, 185, 129, 255); // Glowing Emerald Border
            } else {
                SDL_SetRenderDrawColor(renderer, 31, 41, 55, 255);
                SDL_RenderFillRect(renderer, &calc_rect);
                SDL_SetRenderDrawColor(renderer, 75, 85, 99, 255);
            }
            SDL_RenderDrawRect(renderer, &calc_rect);
            
            // Draw Calculator Icon
            // Frame
            SDL_Rect calc_frame = {503, 110, 130, 130};
            SDL_SetRenderDrawColor(renderer, 156, 163, 175, 255);
            SDL_RenderDrawRect(renderer, &calc_frame);
            // Screen
            SDL_Rect calc_screen = {518, 125, 100, 25};
            SDL_SetRenderDrawColor(renderer, 167, 243, 208, 255); // Mint Green
            SDL_RenderFillRect(renderer, &calc_screen);
            // Mock screen characters
            SDL_SetRenderDrawColor(renderer, 6, 95, 70, 255); // Deep green
            draw_line(renderer, 525, 132, 530, 132); // mock numbers
            draw_line(renderer, 545, 132, 555, 132);
            draw_line(renderer, 560, 132, 565, 132);
            // Buttons Grid
            SDL_SetRenderDrawColor(renderer, 107, 114, 128, 255);
            for(int r_idx=0; r_idx<3; ++r_idx) {
                for(int c_idx=0; c_idx<3; ++c_idx) {
                    SDL_Rect key_btn = {525 + c_idx * 30, 165 + r_idx * 20, 16, 12};
                    SDL_RenderFillRect(renderer, &key_btn);
                }
            }
            // Draw label
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            draw_pixel_string(renderer, "CALCULATOR", 518, 260);
            
            // Footer guidance
            SDL_SetRenderDrawColor(renderer, 156, 163, 175, 255);
            draw_pixel_string(renderer, "CLICK AN ICON OR PRESS 1/2 TO START", 185, 335);
            draw_pixel_string(renderer, "ESC - EXIT SYSTEM", 295, 355);
            
            SDL_RenderPresent(renderer);
        } 
        else if (state == STATE_EMULATOR) {
            // Emulate CHIP-8 steps
            for (int i = 0; i < cycles_per_frame; ++i) {
                chip8_cycle(&chip8);
            }
            
            if (chip8.delay_timer > 0) {
                chip8.delay_timer--;
            }
            if (chip8.sound_timer > 0) {
                chip8.sound_timer--;
            }
            
            // Update texture and render CHIP-8 frame buffer
            SDL_UpdateTexture(texture, NULL, chip8.video, CHIP8_VIDEO_WIDTH * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
        
        // Tick rate frame constraint
        Uint32 elapsed = SDL_GetTicks() - start_time;
        if (elapsed < 16) {
            SDL_Delay(16 - elapsed);
        }
        start_time = SDL_GetTicks();
    }
    
    // Release resources
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    printf("CHIP-8 OS Dashboard closed.\n");
    return 0;
}
