# Final executable name
TARGET = termit_app

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g $(shell pkg-config --cflags ncurses)
LDFLAGS = $(shell pkg-config --libs ncurses)
INCLUDE = -I/opt/homebrew/opt/ncurses/include


# Source and header files
SRCS = mgvmgmt.c MyMenu.c main.c
HDRS = MyMenu.h MyMenu_Types.h mgvmgmt.h

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link object files into final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@  $(LDFLAGS)

# Run the program
test: $(TARGET)
	./$(TARGET)
# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Mark targets that aren't actual files
.PHONY: all clean

