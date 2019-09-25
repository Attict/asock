#include "asock.h"

/**
 * asock_poll_fd
 *
 * @brief: todo
 */
ASOCK_SOCKET_DESCRIPTOR asock_poll_fd(asock_poll_t* p)
{
  return p->state.fd;
}

/**
 * asock_poll_type
 *
 * @brief: todo
 */
int asock_poll_type(asock_poll_t* p)
{
  return p->state.poll_type & 3;
}

/**
 * asock_poll_events
 *
 * @brief: todo
 */
int asock_poll_events(asock_poll_t* p)
{
  return ((p->state.poll_type & POLL_TYPE_POLLING_IN)
        ? ASOCK_SOCKET_READABLE : 0)
      | ((p->state.poll_type & POLL_TYPE_POLLING_OUT)
        ? ASOCK_SOCKET_WRITABLE : 0);
}

/**
 * asock_poll_change
 *
 * @brief: todo
 */
void asock_poll_change(asock_poll_t* p, asock_loop_t* loop, int events)
{
  int old_events = asock_poll_events(p);
  if (old_events != events)
  {
    p->state.poll_type = asock_poll_type(p)
        | ((events & ASOCK_SOCKET_READABLE) ? POLL_TYPE_POLLING_IN : 0)
        | ((events & ASOCK_SOCKET_WRITABLE) ? POLL_TYPE_POLLING_OUT : 0);

    kqueue_change(loop->fd, p->state.fd, old_events, events, p);
  }
}




