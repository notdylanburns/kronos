CC := gcc

ROOT_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
INC_DIR := $(ROOT_DIR)/inc
SRC_DIR := $(ROOT_DIR)/src
OBJ_DIR := $(ROOT_DIR)/obj

CFLAGS := -Wall -Werror -I$(INC_DIR) -fpic
LDFLAGS := -lpthread -lm

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS)) 
TARGET := libkronos.so

all: build remove
build: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) -shared -o $(TARGET) $^

objects: $(OBJS)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ 

$(OBJ_DIR):
	mkdir $@

remove:
	rm -rf $(OBJ_DIR)

clean: remove
	rm -f libkronos.so

install: /usr/local/include/kronos
	cp $(INC_DIR)/* /usr/local/include/kronos/
	cp $(TARGET) /usr/local/lib/
	ln -s /usr/local/lib/$(TARGET) /usr/lib/$(TARGET)

uninstall:
	rm -rf /usr/local/include/kronos
	rm -f /usr/local/lib/$(TARGET)
	unlink /usr/lib/$(TARGET)

/usr/local/include/kronos:
	mkdir /usr/local/include/kronos

full: all uninstall install

.PHONY: all