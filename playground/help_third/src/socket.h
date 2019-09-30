#ifndef ASOCK_SOCKET_H
#define ASOCK_SOCKET_H

#include "asock.h"
#include "context.h"
#include "loop.h"

static inline int asock_core_create_listen(const char *host, int port, int options)
{
  return 0;
}

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
 * asock_socket_free_closed
 *
 * @brief Properly takes the linked list and timeout sweep into account.
 *
 * @param loop
 */
void asock_socket_free_closed(asock_loop_t *loop);

#endif
