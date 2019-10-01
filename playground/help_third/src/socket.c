#include "core.h"
#include "socket.h"
#include <string.h>

/**
 * asock_socket_shutdown
 *
 */
void asock_socket_shutdown(int ssl, asock_socket_t *s)
{
  // Todo: should we emit on_close if calling shutdown on an already
  // half-closed socket?  We need more states in that case, we need to track
  // RECEIVED_FIN so far, the app has to track this and call close as needed
  if (!asock_socket_is_closed(ssl, s) && !asock_socket_is_shutdown(ssl, s))
  {
    asock_poll_set_type(&s->p, ASOCK_POLL_TYPE_SHUTDOWN);
    asock_poll_change(&s->p, s->context->loop,
        asock_poll_events(&s->p) & ASOCK_SOCKET_READABLE);
    asock_core_shutdown_socket(asock_poll_fd((asock_poll_t *) s));
  }
}

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

/**
 * asock_socket_is_shutdown
 *
 */
int asock_socket_is_shutdown(int ssl, asock_socket_t *s)
{
  return asock_poll_type(&s->p) == ASOCK_POLL_TYPE_SHUTDOWN;
}

/**
 * asock_socket_remote_addr
 *
 */
void asock_socket_remote_addr(int ssl, asock_socket_t *s, char *buf, int *len)
{
  asock_core_addr_t addr;
  if (asock_core_socket_addr(asock_poll_fd(&s->p), &addr)
      || *len < asock_core_addr_ip_len(&addr))
  {
    *len = 0;
  }
  else
  {
    *len = asock_core_addr_ip_len(&addr);
    memcpy(buf, asock_core_get_ip(&addr), *len);
  }
}

