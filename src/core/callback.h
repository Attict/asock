#ifndef ASOCK_CALLBACK_H
#define ASOCK_CALLBACK_H

#include "../asock.h"
#include "loop.h"
#include "poll.h"
#include <stdalign.h>

/**
 * asock_callback_t
 *
 * @brief todo
 */
typedef struct asock_callback_t
{
  alignas(ASOCK_EXT_ALIGN) asock_poll_t p;
  asock_loop_t *loop;
  int cb_expects_the_loop;
  void (*cb)(struct asock_callback_t *cb);
}
asock_callback_t;




#endif // ASOCK_CALLBACK_H
