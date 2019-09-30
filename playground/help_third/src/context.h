#ifndef ASOCK_CONTEXT_H
#define ASOCK_CONTEXT_H

#include "asock.h"
#include "callback.h"

typedef struct asock_context_t
{
  alignas(ASOCK_EXT_ALIGN) asock_loop_t *loop;

  asock_socket_t *head;
  asock_socket_t *iterator;
  asock_context_t *prev, *next;

  asock_socket_t *(*on_open)(asock_socket_t *,
      int is_client, char *ip, int ip_length);
  asock_socket_t *(*on_data)(asock_socket_t *, char *data, int length);
  asock_socket_t *(*on_writable)(asock_socket_t *);
  asock_socket_t *(*on_close)(asock_socket_t *);
  asock_socket_t *(*on_socket_timeout)(asock_socket_t *);
  asock_socket_t *(*on_end)(asock_socket_t *);
  int (*ignore_data)(asock_socket_t *);
}
asock_context_t;

#endif // ASOCK_CONTEXT_H
