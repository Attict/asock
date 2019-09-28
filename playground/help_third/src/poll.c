#include "poll.h"

/**
 * asock_poll_fd
 *
 * @note
 */
int asock_poll_fd(asock_poll_t *poll)
{
  return poll->state.fd;
}
