CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -I/usr/include/SDL2 -I/mingw64/include/SDL2
LIBS = -lSDL2

# Source files
SRC = src/chip8.c src/main.c
TARGET = chip8_emu

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET) src/*.o
