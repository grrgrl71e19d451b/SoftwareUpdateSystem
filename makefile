# Nome del compilatore
CC = gcc

# Flag del compilatore
CFLAGS = -Wall -g -Iinclude

# Directory
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Obiettivi
all: $(BIN_DIR) $(OBJ_DIR) $(BIN_DIR)/server $(BIN_DIR)/client_upload $(BIN_DIR)/client_download $(BIN_DIR)/clear_packages $(BIN_DIR)/clear_logs

# Creazione delle directory necessarie
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR)/server: $(OBJ_DIR)/server.o $(OBJ_DIR)/common.o $(OBJ_DIR)/logger.o
	$(CC) $(CFLAGS) -lpthread -o $@ $^

$(BIN_DIR)/client_upload: $(OBJ_DIR)/client_upload.o $(OBJ_DIR)/common.o $(OBJ_DIR)/logger.o
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/client_download: $(OBJ_DIR)/client_download.o $(OBJ_DIR)/common.o $(OBJ_DIR)/logger.o
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/clear_packages: $(OBJ_DIR)/clear_packages.o
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/clear_logs: $(OBJ_DIR)/clear_logs.o
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/server.o: $(SRC_DIR)/server.c include/common.h include/logger.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/client_upload.o: $(SRC_DIR)/client_upload.c include/common.h include/logger.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/client_download.o: $(SRC_DIR)/client_download.c include/common.h include/logger.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/clear_packages.o: $(SRC_DIR)/clear_packages.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/clear_logs.o: $(SRC_DIR)/clear_logs.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/common.o: $(SRC_DIR)/common.c include/common.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/logger.o: $(SRC_DIR)/logger.c include/logger.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/server $(BIN_DIR)/client_upload $(BIN_DIR)/client_download $(BIN_DIR)/clear_packages $(BIN_DIR)/clear_logs

