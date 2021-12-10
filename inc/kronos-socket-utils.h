#ifndef _KRONOS_SOCKET_UTILS_H_GUARD_
#define _KRONOS_SOCKET_UTILS_H_GUARD_

#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>

#include "kronos-error.h"

#define KRONOS_SOCK_BACKLOG 5
#define KRONOS_SOCK_BLOCKSIZE 256

typedef uint16_t port_t;

struct Socket {
    int fd;
    socklen_t addrlen;
    struct sockaddr *addr;
};

extern KRONOS_ERROR create_listener(struct Socket **s, char *host, port_t port);
extern KRONOS_ERROR bind_listener(struct Socket *s);
extern KRONOS_ERROR start_listener(struct Socket *s);
extern KRONOS_ERROR accept_connection(struct Socket *listener, struct Socket **s);
extern KRONOS_ERROR socket_read_all(struct Socket *s, char **buf, int *size);
extern KRONOS_ERROR socket_send(struct Socket *s, char *buf, int size);
extern KRONOS_ERROR close_socket(struct Socket *s);

extern void destroy_socket(struct Socket *s);

#endif