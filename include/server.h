#ifndef SERVER_H_GUARD_
#define SERVER_H_GUARD_

#define SERVERNAME "Kronos"

#include <stdint.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "http.h"
#include "route.h"

struct HTTPServer {
    struct RouteTable *rt;
    uint8_t running;
    uint8_t error;
};

typedef struct HTTPServer HTTPServer;

extern HTTPServer *new_httpserver();
extern void route(HTTPServer *server, enum HTTPMethod method, char *route, RouteHandler handler);
extern void run_server(HTTPServer *server, uint16_t port);
extern RouteHandler get_handler(HTTPServer *server, enum HTTPMethod method, char *route);
extern void *handle_request(void *vargp); // Threading function
extern void destroy_server(HTTPServer *server);

#endif