cmake_minimum_required(VERSION 3.10)
project(chip8_emulator C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/chip8.c
    src/chip8.h
    src/main.c
)

# Search for SDL2
find_package(SDL2)

if(SDL2_FOUND)
    message(STATUS "Found SDL2 via CMake find_package: ${SDL2_LIBRARIES}")
    add_executable(chip8_emu ${SOURCES})
    target_link_libraries(chip8_emu PRIVATE SDL2::SDL2)
else()
    message(WARNING "SDL2 package not found by find_package. Attempting fallback headers/libs.")
    # Fallback search configurations or manually adding directories if needed
    add_executable(chip8_emu ${SOURCES})
    
    # Try generic compiler flags
    target_link_libraries(chip8_emu PRIVATE SDL2)
endif()
