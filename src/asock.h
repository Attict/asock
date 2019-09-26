#ifndef ASOCK_H
#define ASOCK_H

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdalign.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define ASOCK_ALIGN 16

/**
 * Forward declarations (Structs)
 *
 * @brief: todo
 */
typedef struct asock_socket_t asock_socket_t;
typedef struct asock_socket_context_t asock_socket_context_t;
typedef struct asock_listen_socket_t asock_listen_socket_t;

/**
 * Forward declarations (Functions)
 *
 * @brief: todo
 */
int asock_create_socket(int domain, int type, int protocol);
int asock_create_connect_socket(const char *host, int port, int options);
int asock_create_listen_socket(const char *host, int port, int options);
asock_socket_context_t *asock_create_socket_context(int context_ext_size);
asock_listen_socket_t *asock_socket_context_listen(
    asock_socket_context_t *context, const char *host,
    int port, int options, int socket_ext_size);
void asock_close_socket(int fd);

#endif // ASOCK_H
