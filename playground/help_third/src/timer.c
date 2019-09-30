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
