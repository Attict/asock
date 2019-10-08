#include "core.h"
#include "socket.h"
#include <string.h>

/**
 * asock_socket_write
 *
 */
int asock_socket_write(int ssl, asock_socket_t *s,
    const char *data, int length, int msg_more)
{
  if (asock_socket_is_closed(ssl, s) || asock_socket_is_shutdown(ssl, s))
  {
    return 0;
  }

  int written = asock_core_send(asock_poll_fd(&s->p), data, length, msg_more);
  if (written != length)
  {
    s->context->loop->data.last_write_failed = 1;
    asock_poll_change(&s->p, s->context->loop,
        ASOCK_SOCKET_READABLE | ASOCK_SOCKET_WRITABLE);
  }

  return written < 0 ? 0 : written;
}


/**
 * asock_socket_close
 *
 */
asock_socket_t *asock_socket_close(int ssl, asock_socket_t *s)
{
  if (!asock_socket_is_closed(0, s))
  {
    asock_context_unlink(s->context, s);
    asock_poll_stop((asock_poll_t *) s, s->context->loop);
    asock_core_close_socket(asock_poll_fd((asock_poll_t *) s));

    // Link this socket to the close-list and let it be deleted after
    // this iteration.
    s->next = s->context->loop->data.closed_head;
    s->context->loop->data.closed_head = s;

    // Any socket with prev = context is marked as closed
    s->prev = (asock_socket_t *) s->context;

    return s->context->on_close(s);
  }

  return s;
}

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
 * asock_socket_timeout
 *
 */
void asock_socket_timeout(int ssl, asock_socket_t *s, unsigned int seconds)
{
  if (seconds)
  {
    unsigned short timeout_sweeps = 0.5f
        + ((float) seconds) / ((float) ASOCK_TIMEOUT_GRANULARITY);
    s->timeout = timeout_sweeps ? timeout_sweeps : 1;
  }
  else
  {
    s->timeout = 0;
  }
}

/**
 * asock_socket_flush
 *
 */
void asock_socket_flush(int ssl, asock_socket_t *s)
{
  if (!asock_socket_is_shutdown(0, s))
  {
    asock_core_socket_flush(asock_poll_fd((asock_poll_t *) s));
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
 * asock_socket_ext
 *
 */
void *asock_socket_ext(int ssl, asock_socket_t *s)
{
  return s + 1;
}
