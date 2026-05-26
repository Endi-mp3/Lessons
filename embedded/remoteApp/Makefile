# ==== Project settings ====
TARGET   := remoteApp
BUILDDIR ?= .build
OBJDIR   := $(BUILDDIR)/obj
TGTDIR   := $(BUILDDIR)

# ==== Compiler and flags ====
CC      := gcc
CFLAGS  := -Wall -Wextra -std=c99 -g $(shell pkg-config --cflags ncurses)
LDFLAGS := $(shell pkg-config --libs ncurses)
# -I/opt/homebrew/opt/ncurses/include
INCLUDE := -ImyLib -I../esp32/esp_projects/main

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

run_srv: $(TGTDIR)/$(TARGET)
	./$< server 256

run_srv_small: $(TGTDIR)/$(TARGET)
	./$< server 32

run_srv_big: $(TGTDIR)/$(TARGET)
	./$< server 65535

run: $(TGTDIR)/$(TARGET)
	./$< $(ARGS)

run_clnt: $(TGTDIR)/$(TARGET)
	./$< clnt "$(shell cat $(MSGFILE) )"

lldb_clnt: $(TGTDIR)/$(TARGET)
	lldb $(TGTDIR)/$(TARGET) clnt "$(shell cat $(MSGFILE) )"

gdb_clnt: $(TGTDIR)/$(TARGET)
	gdb --args $(TGTDIR)/$(TARGET) clnt "$(shell cat $(MSGFILE) )"

stop:
	killall $(TARGET)
# ==== Clean build artifacts ====
clean:
	rm -rf $(BUILDDIR)

.PHONY: all clean test

