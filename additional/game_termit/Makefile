# ==== Project settings ====
TARGET   := termit_app
BUILDDIR ?= .build
OBJDIR   := $(BUILDDIR)/obj
TGTDIR   := $(BUILDDIR)

# ==== Compiler and flags ====
CC      := gcc
CFLAGS  := -Wall -Wextra -std=c99 -g $(shell pkg-config --cflags ncurses)
LDFLAGS := $(shell pkg-config --libs ncurses)
INCLUDE := -I/opt/homebrew/opt/ncurses/include -ImyLib

# ==== Sources ====
# Все .c из корня и myLib/
SRCS := $(wildcard *.c) $(wildcard myLib/*.c)

# ==== Headers (для зависимостей) ====
HDRS := $(wildcard myLib/*.h)

# ==== Object files (сохраняем структуру каталогов) ====
OBJS := $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

# ==== Default target ====
all: $(TGTDIR)/$(TARGET)

# ==== Link final binary ====
$(TGTDIR)/$(TARGET): $(OBJS) | $(TGTDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ==== Compile source files ====
$(OBJDIR)/%.o: %.c $(HDRS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# ==== Ensure build directories exist ====
$(TGTDIR):
	mkdir -p $(TGTDIR)

# ==== Run the program ====
test: $(TGTDIR)/$(TARGET)
	./$<

# ==== Clean build artifacts ====
clean:
	rm -rf $(BUILDDIR)

.PHONY: all clean test

