CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -Iinclude
SOURCES = src/main.c src/input.c src/game.c src/render.c src/admin.c
TARGET = snake.exe

all: $(TARGET)

$(TARGET): $(SOURCES) include/snake.h
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)
clean:
	powershell -NoProfile -Command "Remove-Item -Force -ErrorAction SilentlyContinue '$(TARGET)'"

.PHONY: all clean
