#include "kronos-stringstream.h"

struct StringStream *new_stringstream(char *source, uint len) {
    struct StringStream *ss = malloc(sizeof(struct StringStream));
    if (ss == NULL) return NULL;
    ss->source = source;
    ss->index = 0;
    ss->len = len;
    return ss;
}

void destroy_stringstream(struct StringStream *ss) {
    free(ss);
}