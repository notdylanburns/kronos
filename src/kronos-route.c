#include "kronos-route.h"

static uint32_t PolynomialRollingHash(enum HTTPMethod method, char *route) {
    const int p = 53;
    const int m = 1000000009;
    int power = 1;
    
    uint32_t res = 0;

    for (char *c = route; *c; c++) {
        res = (res + (*c * power)) % m;
        power = (p * power) % m;
    }

    return res + (int)method;
}

struct RouteTable *new_routetable(uint32_t max, RouteHashFunction f) {
    struct RouteTable *rt = malloc(sizeof(struct RouteTable));
    if (rt == NULL) return NULL;
    rt->num_buckets = max == 0 ? KRONOS_MAX_ROUTE_BUCKETS : max;
    rt->entries = calloc(rt->num_buckets, sizeof(struct RouteTableEntry));
    if (rt == NULL) {
        free(rt);
        return NULL;
    }
    if (f == NULL) {
        rt->hashf = &PolynomialRollingHash;
    } else rt->hashf = f;
    return rt;
}

struct RouteTableEntry *new_routetableentry(enum HTTPMethod method, char *route, RouteHandler handler) {
    struct RouteTableEntry *rte = malloc(sizeof(struct RouteTableEntry));
    if (rte == NULL) return NULL;

    rte->method = method;
    rte->route = route;
    rte->handler = handler;
    rte->next = NULL;

    return rte;
}

void add_route(struct RouteTable *rt, struct RouteTableEntry *rte) {
    uint32_t index = rt->hashf(rte->method, rte->route) % rt->num_buckets;
    struct RouteTableEntry *ll = rt->entries[index];
    if (ll == NULL)
        rt->entries[index] = rte;
    else
        while (ll != NULL) {
            ll = ll->next;
            if (ll->next == NULL) {
                ll->next = rte;
                break;
            }
        }
}

static void destroy_rt_ll(struct RouteTableEntry *rte) {
    void *next;
    if (rte != NULL) {
        next = rte->next;
        free(rte->route);
        free(rte);
        rte = next;
    }
}

void destroy_routetable(struct RouteTable *rt) {
    for (int i = 0; i < rt->num_buckets; i++) {
        destroy_rt_ll(rt->entries[i]);
    }
    free(rt->entries);
    free(rt);
}
