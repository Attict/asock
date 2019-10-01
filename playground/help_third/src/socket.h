#ifndef ASOCK_SOCKET_H
#define ASOCK_SOCKET_H

#include "asock.h"
#include "context.h"
#include "loop.h"

/**
 * asock_socket_t
 *
 * @brief todo
 */
typedef struct asock_socket_t
{
  alignas(ASOCK_EXT_ALIGN) asock_poll_t p;
  asock_context_t *context;
  asock_socket_t *prev, *next;
  unsigned short timeout;
}
asock_socket_t;

/**
 * asock_socket_write
 *
 * @brief Write data up to length, returning actual bytes written.
 *        Will call the on_writable callback of active socket context.
 *        Set hint msg_more if you have more immediate date to write.
 *
 * @param ssl
 * @param s Socket
 * @param data
 * @param length
 * @param msg_more
 * @return True (1) | False (0)
 */
int asock_socket_write(int ssl, asock_socket_t *s,
    const char *data, int length, int msg_more);

/**
 * asock_socket_close
 *
 * @brief todo
 *
 * @param ssl
 * @param s Socket
 * @return Socket
 */
asock_socket_t *asock_socket_close(int ssl, asock_socket_t *s);

/**
 * asock_socket_shutdown
 *
 * @brief todo
 *
 * @param ssl True (1) | False (0)
 * @param s Socket
 */
void asock_socket_shutdown(int ssl, asock_socket_t *s);

/**
 * asock_socket_free_closed
 *
 * @brief Properly takes the linked list and timeout sweep into account.
 *
 * @param loop
 */
void asock_socket_free_closed(asock_loop_t *loop);

/**
 * asock_socket_timeout
 *
 * @brief todo
 *
 * @param ssl
 * @param s Socket
 * @param seconds How many seconds to timeout after
 */
void asock_socket_timeout(int ssl, asock_socket_t *s, unsigned int seconds);

/**
 * asock_socket_flush
 *
 * @brief todo
 *
 * @param ssl
 * @param s Socket
 */
void asock_socket_flush(int ssl, asock_socket_t *s);

/**
 * asock_socket_is_closed
 *
 * @brief Returns whether this socket has been closed.
 *        Only valid if memory has not yet been released.
 *
 * @param
 * @param
 * @return True (1) | False (0)
 */
int asock_socket_is_closed(int ssl, asock_socket_t *s);

/**
 * asock_socket_is_shutdown
 *
 * @brief todo
 *
 * @param ssl
 * @param s
 * @return True (1) | False (0)
 */
int asock_socket_is_shutdown(int ssl, asock_socket_t *s);

/**
 * asock_socket_remote_addr
 *
 * @brief todo
 *
 * @param ssl
 * @param s
 * @param buf
 * @param len
 */
void asock_socket_remote_addr(int ssl, asock_socket_t *s, char *buf, int *len);

/**
 * asock_socket_ext
 *
 * @brief todo
 *
 * @param ssl
 * @param s
 * @return Next pointer?
 */
void *asock_socket_ext(int ssl, asock_socket_t *s);

/**
 * asock_socket_ext
 *
 * @brief todo
 *
 * @param ssl
 * @param s
 * @return User data
 */
void *asock_socket_ext(int ssl, asock_socket_t *s);

#endif
