CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -Iinclude
SOURCES = src/main.c src/input.c src/game.c src/render.c
TARGET = snake.exe

all: $(TARGET)

$(TARGET): $(SOURCES) include/snake.h
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	del /Q $(TARGET) 2>NUL

.PHONY: all clean
