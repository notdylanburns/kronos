#include "route.h"

struct RouteTable *new_routetable() {
    struct RouteTable *rt = malloc(sizeof(struct RouteTable));
    if (rt == NULL) return NULL;
    rt->entries = NULL;
    rt->count = 0;
    return rt;
}

struct RouteTableEntry *new_routetableentry(enum HTTPMethod method, char *route, RouteHandler handler) {
    struct RouteTableEntry *rte = malloc(sizeof(struct RouteTableEntry));
    if (rte == NULL) return NULL;

    rte->method = method;
    rte->route = route;
    rte->handler = handler;

    return rte;
}

void add_route(struct RouteTable *rt, struct RouteTableEntry *rte) {
    rt->count++;
    rt->entries = realloc(rt->entries, rt->count * sizeof(struct RouteTable *));

    rt->entries[rt->count - 1] = rte;
}

void destroy_routetable(struct RouteTable *rt) {
    for (int i = 0; i < rt->count; i++) {
        free(rt->entries[i]->route);
        free(rt->entries[i]);
    }
    free(rt->entries);
    free(rt);
}