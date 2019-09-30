#ifndef ASOCK_CONTEXT_H
#define ASOCK_CONTEXT_H

#include "asock.h"
#include "callback.h"

typedef struct asock_context_t
{
  alignas(ASOCK_EXT_ALIGN) asock_poll_t p;
  asock_loop_t *loop;
  int cb_expects_the_loop;
  void (*cb)(asock_callback_t *cb);
}
asock_context_t;

#endif // ASOCK_CONTEXT_H
