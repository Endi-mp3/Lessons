# Final executable name
TARGET = uart_test

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# Source and header files
SRCS = main.c private.c stud_uart.c
HDRS = private.h

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link object files into final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
test: $(TARGET)
	./$(TARGET)
# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Mark targets that aren't actual files
.PHONY: all clean

