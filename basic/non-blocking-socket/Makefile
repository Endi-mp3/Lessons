TARGET_DIR := build
APP_SERVER := server
APP_CLIENT := client

prep:
	mkdir -p $(TARGET_DIR)

server: prep
	gcc server.c -o $(TARGET_DIR)/$(APP_SERVER)

client: prep
	gcc client.c -o $(TARGET_DIR)/$(APP_CLIENT)

all: server client

clean:
	rm -rf $(TARGET_DIR)

run_server: server
	./$(TARGET_DIR)/$(APP_SERVER)

run_client: client
	./$(TARGET_DIR)/$(APP_CLIENT) 11111111 &
	./$(TARGET_DIR)/$(APP_CLIENT) 22222222 &
	./$(TARGET_DIR)/$(APP_CLIENT) 33333333 &
	./$(TARGET_DIR)/$(APP_CLIENT) 44444444 &
	./$(TARGET_DIR)/$(APP_CLIENT) 55555555 &
	./$(TARGET_DIR)/$(APP_CLIENT) 66666666 &

