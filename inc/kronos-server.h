#ifndef _KRONOS_SERVER_H_GUARD_
#define _KRONOS_SERVER_H_GUARD_

#define SERVERNAME "Kronos"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "kronos-error.h"
#include "kronos-socket-utils.h"
#include "kronos-http.h"
#include "kronos-route.h"

enum KronosFlag {
    KRONOS_HTTP_VERBOSE    = 1 << 0,
    KRONOS_HTTP_PRINT_HEAD = 1 << 1,
};

struct HTTPServer {
    struct RouteTable *rt;
    uint8_t running;
    uint8_t error;
    uint8_t flags;
};

typedef struct HTTPServer HTTPServer;

extern HTTPServer *new_httpserver(uint8_t flags);
extern void route(HTTPServer *server, enum HTTPMethod method, char *route, RouteHandler handler);
extern void run_server(HTTPServer *server, char *host, uint16_t port);
extern void *handle_request(void *vargp); // Threading function
extern void destroy_server(HTTPServer *server);

#endif