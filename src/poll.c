#include "asock.h"

ASOCK_SOCKET_DESCRIPTOR asock_poll_fd(asock_poll_t* p)
{
  return p->state.fd;
}

int asock_poll_type(asock_poll_t* p)
{
  return p->state.poll_type & 3;
}


