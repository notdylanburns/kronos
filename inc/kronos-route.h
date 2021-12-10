#ifndef _KRONOS_ROUTE_H_GUARD_
#define _KRONOS_ROUTE_H_GUARD_

#include <stdlib.h>

#include "kronos-http.h"

#define KRONOS_MAX_ROUTE_BUCKETS 256

typedef void (*RouteHandler)(struct HTTPRequest *, struct HTTPResponse *);
typedef uint32_t (*RouteHashFunction)(enum HTTPMethod, char *);

struct RouteTableEntry {
    enum HTTPMethod method;
    char *route;
    RouteHandler handler;
    struct RouteTableEntry *next;
};

struct RouteTable {
    uint32_t num_buckets;
    struct RouteTableEntry **entries;
    RouteHashFunction hashf;
};

extern struct RouteTable *new_routetable(uint32_t max, RouteHashFunction f);
extern struct RouteTableEntry *new_routetableentry(enum HTTPMethod method, char *route, RouteHandler handler);
extern void add_route(struct RouteTable *rt, struct RouteTableEntry *rte);
extern void destroy_routetable(struct RouteTable *rt);
#endif