#include "callback.h"
#include "timer.h"

/**
 * asock_create_timer
 *
 */
asock_timer_t *asock_timer_create(
    asock_loop_t *loop, int fallthrough, unsigned int ext_size)
{

}

/**
 * asock_timer_close
 *
 */
void asock_timer_close(asock_timer_t *timer)
{
  asock_callback_t *cb = (asock_callback_t *) timer;

  struct kevent event;
  EV_SET(&event, (uintptr_t) cb, EVFILT_TIMER, EV_DELETE, 0, 0, cb);

  // (Regular) Sockets are the only polls which are not freed immediately.
  asock_poll_free((asock_poll_t *) timer, cb->loop);
}

/**
 * asock_timer_set
 *
 */
void asock_timer_set(
    asock_timer_t *timer, void (*cb)(asock_timer_t *t), int ms, int repeat_ms)
{

}

/**
 * asock_timer_ext
 *
 */
void *asock_timer_ext(asock_timer_t *timer)
{
  return ((asock_callback_t *) timer) + 1;
}

/**
 * asock_timer_loop
 *
 */
asock_loop_t *asock_timer_loop(asock_timer_t *t)
{
  asock_callback_t *cb = (asock_callback_t *) t;
  return cb->loop;
}
