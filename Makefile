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

install: /usr/local/include/kronos
	cp include/* /usr/local/include/kronos/
	cp $(TARGET) /usr/local/lib/
	@echo "Run 'ln -s /usr/local/lib/$(TARGET) /usr/lib/$(TARGET)' as root to remove the need to edit \$$LD_LIBRARY_PATH"

uninstall:
	rm -rf /usr/local/include/kronos
	rm -f /usr/local/lib/$(TARGET)
	@echo "Run 'unlink /usr/lib/$(TARGET)' as root to finish uninstalling"

/usr/local/include/kronos:
	mkdir /usr/local/include/kronos

full: all uninstall install


.PHONY: all
