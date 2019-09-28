#ifndef ASOCK_ASYNC_H
#define ASOCK_ASYNC_H

#include "asock.h"
#include "loop.h"
#include "poll.h"
#include <stdalign.h>

/**
 * asock_async_t
 *
 * @brief todo
 */
typedef struct asock_async_t asock_async_t;

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

/**
 * asock_async_close
 *
 * @brief todo
 *
 * @param a
 */
void asock_async_close(asock_async_t *a);

/**
 * asock_async_set
 *
 * @brief todo
 *
 * @param a Async Struct to be set
 * @param cb Callback
 */
void asock_async_set(asock_async_t *a, void (*cb)(asock_async_t *));

/**
 * asock_async_wakeup
 *
 * @brief todo
 *
 * @param a
 */
void asock_async_wakeup(asock_async_t *a);

#endif // ASOCK_ASYNC_H
