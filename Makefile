CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -Iinclude
LDFLAGS = -lbcrypt
SOURCES = src/main.c src/input.c src/game.c src/render.c src/admin.c
HEADERS = include/snake.h include/admin.h
TARGET = snake.exe

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)
clean:
	del /Q $(TARGET) 2>NUL

.PHONY: all clean
