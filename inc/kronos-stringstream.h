#ifndef _KRONOS_STRINGSTREAM_H_GUARD_
#define _KRONOS_STRINGSTREAM_H_GUARD_

#include <stdlib.h>

#define peek(ss) ss->index == ss->len ? '\0' : ss->source[ss->index]
#define get(ss) ss->index == ss->len ? '\0' : ss->source[(ss->index)++]
#define next(ss) ss->index = (ss->index == ss->len) ? ss->index : ss->index++

struct StringStream {
    char *source;
    uint index; 
    uint len;
};

extern struct StringStream *new_stringstream(char *source, uint len);
extern void destroy_stringstream(struct StringStream *ss);

#endif