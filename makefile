CC=gcc
CFLAGS=-Wall -Wextra -std=gnu11 -Iinclude

SRC=$(wildcard src/*.c)
SHELL_SRC=$(filter-out src/memory_layout.c,$(SRC))

TARGET=shellforge
MEMORY_TARGET=memory_layout

$(TARGET): $(SHELL_SRC)
	$(CC) $(CFLAGS) $(SHELL_SRC) -lreadline -o $(TARGET)

$(MEMORY_TARGET): src/memory_layout.c
	$(CC) $(CFLAGS) src/memory_layout.c -o $(MEMORY_TARGET)

clean:
	rm -f $(TARGET) $(MEMORY_TARGET)

.PHONY: clean
