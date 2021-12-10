#include "kronos-server.h"

HTTPServer *new_httpserver(uint8_t flags) {
    HTTPServer *server = malloc(sizeof(HTTPServer));
    if (server == NULL) return NULL;
    server->rt = new_routetable(0, NULL);
    if (server->rt == NULL) return NULL;
    server->running = 1;
    server->error = 0;
    return server;
}

static char get_flag(HTTPServer *server, enum KronosFlag f) {
    return server->flags & (uint8_t)f;
}

static RouteHandler get_handler(HTTPServer *server, enum HTTPMethod method, char *route) {
    struct RouteTableEntry *rte = server->rt->entries[server->rt->hashf(method, route) % server->rt->num_buckets];
    while (rte != NULL) {
        if (rte->method == method && strcmp(rte->route, route) == 0) 
            return rte->handler;
        rte = rte->next;
    }

    return NULL;
}

struct ThreadInfo {
    struct Socket *sock;
    HTTPServer *server;
};

void run_server(HTTPServer *server, char *host, port_t port) {
    struct Socket *listener;
    int err;

    if (IS_ERROR(create_listener(&listener, host, port)))
        throw_err(err, "SocketCreateError");

    if (IS_ERROR(bind_listener(listener)))
        throw_err(err, "SocketBindError");

    if (IS_ERROR(start_listener(listener)))
        throw_err(err, "SocketListenError");

    struct Socket *client;
    pthread_t thread_id;
    
    while (server->running) {
        if (IS_ERROR(accept_connection(listener, &client))) {
            print_err(err, "SocketConnectionError");
            continue;
        }

        printf("Connection Made");
        
        struct ThreadInfo ti = { .sock = client, .server = server };

        (void)ti;
        (void)thread_id;       
        
        err = shutdown(client->fd, SHUT_RDWR);
        if (err != 0) print_err(err, "SocketShutdownError");
    }
}

int send_response(struct Socket *s, struct HTTPResponse *res) {
    time_t val = time(NULL);
    struct tm *t = gmtime(&val);
    // day, dd mon year hh:mm:ss tmz
    char date[30];
    strftime(date, 30, "%a, %02d %b %Y %T UTC", t);
    write_header(res, "Date", date);

    char *responseBuffer = build_httpresponse(res);
    int err = socket_send(s, responseBuffer, strlen(responseBuffer));

    free(responseBuffer);
    destroy_httpresponse(res);

    return err;

}

void *handle_request(void *vargp) {
    struct ThreadInfo *ti = vargp;
    struct Socket *conn = ti->sock;
    HTTPServer *server = ti->server;

    char *req_bytes = NULL;
    int err, size;
    if (IS_ERROR(socket_read_all(conn, &req_bytes, &size))) {
        print_err(err, "SocketReadError");
        free(req_bytes);
    };

    struct HTTPRequest *req;
    if (IS_ERROR(build_httprequest(&req, req_bytes, size))) {
        print_err(err, "BuildRequestError");
        free(req_bytes);
    };

    if (get_flag(server, KRONOS_HTTP_VERBOSE)) {
        switch (req->method) {
            case GET:
                printf("GET");
                break;
            case POST:
                printf("POST");
                break;
            case PUT:
                printf("PUT");
                break;
            case DELETE:
                printf("DELETE");
                break;
            default:
                printf("UNDEFINED");
        }
        printf(" %s %s\n", req->route, req->version);

        if (get_flag(server, KRONOS_HTTP_PRINT_HEAD)) {
            for (int i = 0; i < req->header_count; i++) {
                printf("%s: %s\n", req->headers[i]->key, req->headers[i]->value);
            }

            printf("\n");
        }
    }

    RouteHandler h = get_handler(server, req->method, req->route);
    if (h == NULL) {
        struct HTTPResponse *res = new_httpresponse();
        
        set_status(res, "HTTP/1.1", STATUS_NOT_FOUND, "Not Found");

        write_header(res, "Server", SERVERNAME);

        const char *msg = "404 Not Found";
        set_content(res, "text/html; charset=UTF-8", strlen(msg), (uint8_t *)msg);

        send_response(conn, res);

    } else {
        struct HTTPResponse *res = new_httpresponse();
        
        set_status(res, "HTTP/1.1", STATUS_OK, "OK");

        write_header(res, "Server", SERVERNAME);

        // Call handler
        h(req, res);
       
        // Send 
        send_response(conn, res);
    }

    destroy_httprequest(req);
    
    if (IS_ERROR(close_socket(conn))) {
        print_err(err, "SocketCloseError");
    };

    return NULL;
}

void route(HTTPServer *server, enum HTTPMethod method, char *route, RouteHandler handler) {
    struct RouteTableEntry *rte = new_routetableentry(method, route, handler);
    add_route(server->rt, rte);
};

void destroy_server(HTTPServer *server) {
    destroy_routetable(server->rt);
    free(server);
    server = NULL;
};