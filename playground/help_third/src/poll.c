#include "poll.h"
#include <stdlib.h>

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
