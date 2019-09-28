#ifndef ASOCK_CORE_H
#define ASOCK_CORE_H

/**
 * asock_core_create_socket
 *
 * @brief: todo
 *
 * @param domain
 * @param type
 * @param protocol
 * @returns The newly created file descriptor.
 */
int asock_core_create_socket(int domain, int type, int protocol);

/**
 * asock_core_connect_socket
 *
 * @brief: Creates the connect socket
 *
 * @param host
 * @param port
 * @param options
 * @returns The newly created file descriptor.
 */
int asock_core_connect_socket(const char *host, int port, int options);

/**
 * asock_core_listen_socket
 *
 * @brief: Creates the listen socket
 *
 * @param host
 * @param port
 * @param options
 * @returns The newly created file descriptor.
 */
int asock_core_listen_socket(const char *host, int port, int options);

/**
 * asock_core_socket_flush
 *
 * @brief: todo
 *
 * @param fd File Descriptor
 */
void asock_core_socket_flush(int fd);

#endif // ASOCK_CORE_H
