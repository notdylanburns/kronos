CC := gcc
CFLAGS := -Wall -Werror -fpic
LDFLAGS := -lpthread -lm

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c, obj/%.o, $(SRCS)) 
TARGET := libkronos.so

all: build remove
build: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) -shared -o $(TARGET) $^

objects: $(OBJS)
obj/%.o: src/%.c obj
	$(CC) $(CFLAGS) -c $< -o $@ 

obj:
	mkdir $@

remove:
	rm -rf obj

clean: remove
	rm -f libkronos.so

.PHONY: all
