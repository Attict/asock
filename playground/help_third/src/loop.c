#include "async.h"
#include "callback.h"
#include "loop.h"
#include "timer.h"
#include <stdlib.h>
#include <unistd.h>

/**
 * asock_loop_create
 *
 */
asock_loop_t *asock_loop_create(void *hint,
    void (*wakeup_cb)(asock_loop_t *loop), void (*pre_cb)(asock_loop_t *loop),
    void (*post_cb)(asock_loop_t *loop), unsigned int ext_size)
{
  asock_loop_t *loop = (asock_loop_t *) malloc(sizeof(asock_loop_t) + ext_size);
  loop->num_polls = 0;

  loop->fd = kqueue();

  asock_loop_data_init(loop, wakeup_cb, pre_cb, post_cb);
  return loop;
}

/**
 * asock_loop_data_init
 *
 */
void asock_loop_data_init(asock_loop_t *loop, void (*wakeup_cb)(asock_loop_t *),
    void (*pre_cb)(asock_loop_t *), void (*post_cb)(asock_loop_t *loop))
{
  loop->data.sweep_timer = asock_timer_create(loop, 1, 0);
  loop->data.recv_buf = malloc(
      ASOCK_RECV_BUFFER_LENGTH + ASOCK_RECV_BUFFER_PADDING * 2);
  loop->data.ssl_data = 0;
  loop->data.head = 0;
  loop->data.iterator = 0;
  loop->data.closed_head = 0;

  loop->data.pre_cb = pre_cb;
  loop->data.post_cb = post_cb;
  loop->data.iteration_nr = 0;
  loop->data.wakeup_async = asock_async_create(loop, 1, 0);

  asock_async_set(
      loop->data.wakeup_async, (void (*)(asock_async_t *)) wakeup_cb);
}

/**
 * asock_loop_free
 *
 * @note
 */
void asock_loop_free(asock_loop_t *loop)
{
  asock_loop_data_free(loop);
  close(loop->fd);
  free(loop);
}

/**
 * asock_loop_data_free
 *
 * @note
 */
void asock_loop_data_free(asock_loop_t *loop)
{
  free(loop->data.recv_buf);

  asock_timer_close((asock_timer_t *) loop->data.sweep_timer);
  asock_async_close(loop->data.wakeup_async);
}

/**
 * asock_loop_update_pending
 *
 */
void asock_loop_update_pending(asock_loop_t *loop, asock_poll_t *old_poll,
    asock_poll_t *new_poll, int old_events, int new_events)
{
  // Possibly remaining entries...
  // Ready polls may contain same poll twice under kqueue
  // as one poll may hold two filters.
  int remaining_entries = 2;

  //
  // @todo For kqueue, if we track things in asock_poll_change,
  //       it is possible to have a fast path with no seeking in cases
  //       of: current poll being us AND we only for one thing.
  //

  for (int i = loop->current_ready_poll; i < loop->num_ready_polls
      && remaining_entries; i++)
  {
    if (GET_READY_POLL(loop, i) == old_poll)
    {
      // If new events do not contain the ready events of this poll
      // then remove (no we filter that out later on)
      SET_READY_POLL(loop, i, new_poll);

      remaining_entries--;
    }
  }
}

/**
 * asock_loop_wakeup
 *
 */
void asock_loop_wakeup(asock_loop_t *loop)
{
  asock_loop_t *casted_loop = (asock_loop_t *) loop;
  asock_async_wakeup((asock_async_t *) casted_loop->data.wakeup_async);
}

/**
 * asock_loop_integrate
 *
 */
void asock_loop_integrate(asock_loop_t *loop)
{
  asock_timer_set(loop->data.sweep_timer,
      (void (*)(asock_timer_t *)) asock_loop_sweep_timer_cb,
      ASOCK_TIMEOUT_GRANULARITY * 1000,
      ASOCK_TIMEOUT_GRANULARITY * 1000);
}

/**
 * asock_loop_sweep_timer_cb
 *
 */
void asock_loop_sweep_timer_cb(asock_callback_t *cb)
{
  asock_timer_sweep(cb->loop);
}

/**
 * asock_loop_run
 *
 */
void asock_loop_run(asock_loop_t *loop)
{
  asock_loop_integrate(loop);

  // While we ahve non-fallthrough polls we shouldn't fall through
  while (loop->num_polls)
  {

  }
}
