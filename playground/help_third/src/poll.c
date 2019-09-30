#include "poll.h"
#include <stdlib.h>
#include <sys/event.h>

/**
 * asock_poll_create
 *
 */
asock_poll_t *asock_poll_create(asock_loop_t *loop,
    int fallthrough, unsigned int ext_size)
{
  if (!fallthrough)
  {
    loop->num_polls++;
  }
  return malloc(sizeof(asock_poll_t) + ext_size);
}

/**
 * asock_poll_init
 *
 */
void asock_poll_init(asock_poll_t *p, int fd, int poll_type)
{
  p->state.fd = fd;
  p->state.poll_type = poll_type;
}

/**
 * asock_poll_stop
 *
 */
void asock_poll_stop(asock_poll_t *p, asock_loop_t *loop)
{
  int old_events = asock_poll_events(p);
  int new_events = 0;

  if (old_events)
  {
    kqueue_change(loop->fd, p->state.fd, old_events, new_events, NULL);
  }

  // Disable any instance of us in the pending ready poll list
  asock_loop_update_pending(loop, p, 0, old_events, new_events);
}

/**
 * asock_poll_free
 *
 */
void asock_poll_free(asock_poll_t *p, asock_loop_t *loop)
{
  loop->num_polls--;
  free(p);
}

/**
 * asock_poll_fd
 *
 */
int asock_poll_fd(asock_poll_t *poll)
{
  return poll->state.fd;
}

/**
 * asock_poll_ext
 *
 */
void *asock_poll_ext(asock_poll_t *p)
{
  return p + 1;
}

/**
 * asock_poll_type
 *
 */
int asock_poll_type(asock_poll_t *p)
{
  return p->state.poll_type & 3;
}

/**
 * asock_poll_accept_event
 *
 * @todo EPOLL handling
 */
unsigned int asock_poll_accept_event(asock_poll_t *p)
{
  return 0;
}

/**
 * asock_poll_events
 *
 */
int asock_poll_events(asock_poll_t *p)
{
  return ((p->state.poll_type & ASOCK_POLL_IN) ? ASOCK_SOCKET_READABLE : 0)
      | ((p->state.poll_type & ASOCK_POLL_OUT) ? ASOCK_SOCKET_WRITABLE : 0);
}
