#include "kronos-error.h"

int get_errno() {
    return errno;
}

void throw_err(int err, char *type) {
    print_err(err, type);
    exit(err);
}

void print_err(int err, char *type) {
    fprintf(stderr, "%s: %s", type, strerror(err));
}