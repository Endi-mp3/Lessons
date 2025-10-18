# Final executable name
TARGET = termit_app

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g $(shell pkg-config --cflags ncursesw)
LDFLAGS = $(shell pkg-config --libs ncursesw)
INCLUDE = -I/opt/homebrew/opt/ncurses/include


# Source and header files
SRCS = main.c MyMenu.c
HDRS = MyMenu.h MyMenu_Types.h

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link object files into final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

# Run the program
test: $(TARGET)
	./$(TARGET)
# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Mark targets that aren't actual files
.PHONY: all clean

