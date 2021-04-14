CC := gcc
CFLAGS := -Wall -Werror -g
LDFLAGS := -lm -lpthread -lkronos

EXAMPLES := $(wildcard example*.c)
TARGETS := $(patsubst %.c, %, $(EXAMPLES))

example1:
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ "$@.c"

clean:
	rm -f $(TARGETS)