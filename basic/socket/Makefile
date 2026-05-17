MSGFILE := msg.txt
BUILD_DIR := .build
TARGETBINNAME := mysock
TARGETBIN := $(BUILD_DIR)/$(TARGETBINNAME)
CC      := gcc
CFLAGS  := -Wall -Wextra -std=c99 -g $(shell pkg-config --cflags ncurses)

all: prepare
	$(CC) $(CFLAGS) my_socket.c -o $(TARGETBIN) -I./

run_srv: all
	$(TARGETBIN) server 256

run_srv_small: all
	$(TARGETBIN) server 32

run_srv_big: all
	$(TARGETBIN) server 65535


run_clnt: all
	$(TARGETBIN) clnt "$(shell cat $(MSGFILE) )"

lldb_clnt: all
	lldb $(TARGETBIN) clnt "$(shell cat $(MSGFILE) )"

gdb_clnt: all
	gdb --args $(TARGETBIN) clnt "$(shell cat $(MSGFILE) )"

prepare:
	mkdir -p $(BUILD_DIR)

stop:
	killall $(TARGETBINNAME)

clean:
	rm -rf .build
