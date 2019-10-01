#include "core.h"
#include "socket.h"
#include <string.h>

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

/**
 * asock_listen_socket_close
 *
 */
void asock_listen_socket_close(int ssl, asock_listen_socket_t *ls)
{
  // asock_listen_socket_t extends asock_socket_t so we close in similar ways

  if (!asock_socket_is_closed(0, &ls->s))
  {
    asock_context_unlink(ls->s.context, &ls->s);
    asock_poll_stop((asock_poll_t *) &ls->s, ls->s.context->loop);
    asock_core_close_socket(asock_poll_fd((asock_poll_t *) &ls->s));

    // Link this socket to the close-list and
    // let it be deleted after this iteration
    ls->s.next = ls->s.context->loop->data.closed_head;
    ls->s.context->loop->data.closed_head = &ls->s;

    // Any socket with prev = context is marked as closed
    ls->s.prev = (asock_socket_t *) ls->s.context;
  }

  // We cannot immediately free a listen socket
  // as we can be inside an accept loop
}

