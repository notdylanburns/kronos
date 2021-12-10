#ifndef _KRONOS_URL_H_GUARD_
#define _KRONOS_URL_H_GUARD_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define KRONOS_MAX_URL_PARAM_MAP_BUCKETS 256

typedef uint32_t (*ParamHashFunction)(char *);

struct URLParam {
    char *key;
    char *value;
    struct URLParam *next;
};

struct URLParamMap {
    uint32_t num_buckets;
    struct URLParam **entries;
    ParamHashFunction hashf;
};

extern struct URLParamMap *new_param_map(uint32_t max, ParamHashFunction hashf);
extern void destroy_param_map(struct URLParamMap *um);
extern void add_url_param(struct URLParamMap *um, struct URLParam *up);
extern struct URLParam *get_url_param(struct URLParamMap *um, char *key);

#endif