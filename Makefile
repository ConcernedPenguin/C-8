CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Werror -Iinclude $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)

SRC = src/main.c src/chip8.c src/cpu.c src/display.c src/renderer.c

OBJ = $(SRC:.c=.o)

TARGET = chip8

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

