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
 * asock_core_socket_flush
 *
 * @brief: todo
 *
 * @param fd File Descriptor
 */
void asock_core_socket_flush(int fd);

#endif // ASOCK_CORE_H
