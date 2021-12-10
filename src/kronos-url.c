#include "kronos-url.h"

static uint32_t PolynomialRollingHash(char *key) {
    const int p = 53;
    const int m = 1000000009;
    int power = 1;
    
    uint32_t res = 0;

    for (char *c = key; *c; c++) {
        res = (res + (*c * power)) % m;
        power = (p * power) % m;
    }

    return res;
}

struct URLParam *new_url_param(char *key, char *value) {
    struct URLParam *up = malloc(sizeof(struct URLParam));
    if (up == NULL) return NULL;

    up->next = NULL;
    strcpy(up->key, key);
    strcpy(up->value, value);

    return up;
}

struct URLParamMap *new_param_map(uint32_t max, ParamHashFunction hashf) {
    struct URLParamMap *um = malloc(sizeof(struct URLParamMap));
    if (um == NULL) return NULL;
    um->num_buckets = max == 0 ? KRONOS_MAX_URL_PARAM_MAP_BUCKETS : max;
    um->entries = calloc(um->num_buckets, sizeof(struct URLParamMap));
    if (um == NULL) {
        free(um);
        return NULL;
    }
    if (hashf == NULL) {
        um->hashf = &PolynomialRollingHash;
    } else um->hashf = hashf;
    return um;
}

static void destroy_url_param_ll(struct URLParam *up) {
    if (up == NULL) return;
    destroy_url_param_ll(up->next);
    free(up);
}

void destroy_param_map(struct URLParamMap *um) {
    for (int i = 0; i < um->num_buckets; i++) {
        destroy_url_param_ll(um->entries[i]);
    }
    free(um->entries);
    free(um);
}

void add_url_param(struct URLParamMap *um, struct URLParam *up) {
    uint32_t index = um->hashf(up->key) % um->num_buckets;
    struct URLParam *ll = um->entries[index];
    if (ll == NULL)
        um->entries[index] = up;
    else
        while (ll != NULL) {
            ll = ll->next;
            if (ll->next == NULL) {
                ll->next = up;
                break;
            }
        }
}

struct URLParam *get_url_param(struct URLParamMap *um, char *key) {
    uint32_t index = um->hashf(key) % um->num_buckets;
    struct URLParam *ll = um->entries[index];
    if (ll == NULL)
        return NULL;
    else 
        while (ll != NULL) {
            ll = ll->next;
            if (strcmp(ll->key, key) == 0)
                return ll;
        }
    return NULL;
}