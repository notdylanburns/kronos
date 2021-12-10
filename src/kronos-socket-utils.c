#include "kronos-socket-utils.h"

KRONOS_ERROR create_listener(struct Socket **s, char *host, port_t port) {
    char port_string[NI_MAXSERV];
    snprintf(port_string, NI_MAXSERV, "%d", port);

    struct addrinfo *ptr;
    int err = getaddrinfo(host, port_string, NULL, &ptr);
    if (err != 0) return err;

    int fd = socket(ptr->ai_family, SOCK_STREAM, 0);
    if (fd == -1) {
        freeaddrinfo(ptr);
        return get_errno();
    }

    *s = malloc(sizeof(struct Socket));
    if (*s == NULL) return KRONOS_NOMEM;

    memcpy((*s)->addr, ptr->ai_addr, ptr->ai_addrlen);
    (*s)->addrlen = ptr->ai_addrlen;
    (*s)->fd = fd;

    freeaddrinfo(ptr);

    return KRONOS_SUCCESS;
}

KRONOS_ERROR bind_listener(struct Socket *s) {
    return bind(s->fd, s->addr, s->addrlen);
}

KRONOS_ERROR start_listener(struct Socket *s) {
    return listen(s->fd, KRONOS_SOCK_BACKLOG);
}

KRONOS_ERROR accept_connection(struct Socket *listener, struct Socket **s) {
    struct sockaddr *sa = malloc(sizeof(struct sockaddr));
    if (sa == NULL) return KRONOS_NOMEM;
    return accept(listener->fd, sa, &(listener->addrlen));
}

KRONOS_ERROR socket_read_all(struct Socket *s, char **buf, int *size) {
    
    *size = 0;
    uint8_t block[KRONOS_SOCK_BLOCKSIZE];
    for (;;) {
        int bytesread = recv(s->fd, block, KRONOS_SOCK_BLOCKSIZE, 0);
        if (bytesread == -1) return get_errno();
        *buf = realloc(*buf, *size + bytesread);
        if (*buf == NULL) return KRONOS_NOMEM;
        memcpy(*buf + *size, block, bytesread);
        *size += bytesread;
        if (bytesread != KRONOS_SOCK_BLOCKSIZE) break;
    }

    return KRONOS_SUCCESS;
}

KRONOS_ERROR socket_send(struct Socket *s, char *buf, int size) {
    return send(s->fd, buf, size, 0);
}

KRONOS_ERROR close_socket(struct Socket *s) {
    return shutdown(s->fd, SHUT_RDWR);
}

void destroy_socket(struct Socket *s) {
    free(s->addr);
    free(s);
    s = NULL;
}