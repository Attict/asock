#include "poll.h"
#include <stdlib.h>

/**
 * asock_poll_fd
 *
 * @note
 */
int asock_poll_fd(asock_poll_t *poll)
{
  return poll->state.fd;
}

/**
 * asock_poll_free
 *
 * @param p Poll
 * @param loop
 */
void asock_poll_free(asock_poll_t *p, asock_loop_t *loop)
{
  loop->num_polls--;
  free(p);
}
