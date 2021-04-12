#ifndef ROUTE_H_GUARD_
#define ROUTE_H_GUARD_

#include <stdlib.h>

#include "http.h"

typedef void (*RouteHandler)(struct HTTPRequest *, struct HTTPResponse *);

struct RouteTableEntry {
    enum HTTPMethod method;
    char *route;
    RouteHandler handler;
};

struct RouteTable {
    struct RouteTableEntry **entries;
    int count;
};

extern struct RouteTable *new_routetable();
extern struct RouteTableEntry *new_routetableentry(enum HTTPMethod method, char *route, RouteHandler handler);
extern void add_route(struct RouteTable *rt, struct RouteTableEntry *rte);
extern void destroy_routetable(struct RouteTable *rt);
#endif