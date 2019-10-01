#include "socket.h"

/**
 * asock_socket_free_closed
 *
 */
void asock_socket_free_closed(asock_loop_t *loop)
{
  // Free all closed sockets (maybe it is better to reverse order?)
  if (loop->data.closed_head)
  {
    for (asock_socket_t *s = loop->data.closed_head; s; )
    {
      asock_socket_t *next = s->next;
      asock_poll_free((asock_poll_t *) s, loop);
      s = next;
    }
    loop->data.closed_head = 0;
  }
}

/**
 * asock_socket_is_closed
 *
 */
int asock_socket_is_closed(int ssl, asock_socket_t *s)
{
  return s->prev == (asock_socket_t *) s->context;
}
