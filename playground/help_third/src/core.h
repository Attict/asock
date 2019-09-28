/**
 * Copyright 2019, Eric Wagner, All rights reserved.
 *
 * @file core.h
 * @author Eric Wagner <eric@attict.net>
 * @date 28 September 2019
 * @brief Core functionality for ASOCK
 *
 * This header defines core functionality for sockets
 * @see https://pubs.opengroup.org/onlinepubs/009695399/basedefs/sys/socket.h.html
 */

#ifndef ASOCK_CORE_H
#define ASOCK_CORE_H

#include <sys/socket.h>

/**
 * asock_core_addr_t
 *
 * @brief todo
 */
typedef struct asock_core_addr_t
{
  struct sockaddr_storage mem;
  socklen_t len;
  char *ip;
  int ip_length;

}
asock_core_addr_t;

/**
 * asock_core_create_socket
 *
 * @brief todo
 *
 * @param domain
 * @param type
 * @param protocol
 * @return The newly created file descriptor.
 */
int asock_core_create_socket(int domain, int type, int protocol);

/**
 * asock_core_connect_socket
 *
 * @brief Creates the connect socket
 *
 * @param host
 * @param port
 * @param options
 * @return The newly created file descriptor.
 */
int asock_core_connect_socket(const char *host, int port, int options);

/**
 * asock_core_listen_socket
 *
 * @brief Creates the listen socket
 *
 * @param host
 * @param port
 * @param options
 * @return The newly created file descriptor.
 */
int asock_core_listen_socket(const char *host, int port, int options);

/**
 * asock_core_shutdown_socket
 *
 * @brief todo
 *
 * @param fd File Descriptor
 */
void asock_core_shutdown_socket(int fd);

/**
 * asock_core_close_socket
 *
 * @brief todo
 *
 * @param fd File Descriptor
 */
void asock_core_close_socket(int fd);

/**
 * asock_core_socket_flush
 *
 * @brief todo
 *
 * @param fd File Descriptor
 */
void asock_core_socket_flush(int fd);

/**
 * asock_core_socket_nodelay
 *
 * @brief todo
 *
 * @param fd File Descriptor
 * @param enabled
 */
void asock_core_socket_nodelay(int fd, int enabled);

/**
 * asock_core_socket_addr
 *
 * @brief todo
 *
 * @param fd File Descriptor
 * @param addr
 * @return File Descriptor
 */
int asock_core_socket_addr(int fd, asock_core_addr_t *addr);

/**
 * asock_core_accept_socket
 *
 * @brief todo
 *
 * @param fd File Descriptor
 * @param addr
 * @return File Descriptor
 */
int asock_core_accept_socket(int fd, asock_core_addr_t *addr);

/**
 * asock_core_recv
 *
 * @brief todo
 *
 * @param fd File Descriptor
 * @param buffer Pointer to the buffer to receive
 * @param length Length of the message received
 * @param flags Type of message transmission
 * @return The length of the message written to the buffer parameter
 */
int asock_core_recv(int fd, void *buffer, int length, int flags);

/**
 * asock_core_send
 *
 * @brief todo
 *
 * @param fd File Descriptor
 * @param buffer Pointer to the buffer to send
 * @param length Length of the message to send
 * @param flags Type of message transmission
 */
int asock_core_send(int fd, const char *buffer, int length, int msg_more);

/**
 * asock_core_would_block
 *
 * @brief todo
 *
 * @return Error (1) if would block
 */
int asock_core_would_block();

/**
 * asock_core_addr_ip_len
 *
 * @brief todo
 *
 * @param addr
 * @return The ip length of the addr struct
 */
int asock_core_addr_ip_len(asock_core_addr_t *addr);

/**
 * asock_core_get_ip
 *
 * @brief todo
 *
 * @param addr
 * @return The ip address of the addr struct
 */
char *asock_core_get_ip(asock_core_addr_t *addr);


#endif // ASOCK_CORE_H
