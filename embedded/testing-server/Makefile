TARGET_DIR := build
APP_SERVER := server
INCLUDES := -I../esp32/esp_projects/main -I./
TESTING_SOURCES := ../esp32/esp_projects/main/my_logic.c
DEFINES := -DTESTING=1

.PHONY: all

all: server
prep:
	mkdir -p $(TARGET_DIR)

server: prep
	gcc server.c $(TESTING_SOURCES) -o $(TARGET_DIR)/$(APP_SERVER) $(INCLUDES) $(DEFINES)


clean:
	rm -rf $(TARGET_DIR)

run_server: server
	./$(TARGET_DIR)/$(APP_SERVER)

