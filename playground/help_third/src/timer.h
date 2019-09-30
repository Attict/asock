#ifndef ASOCK_TIMER_H
#define ASOCK_TIMER_H

#include "loop.h"

/**
 * asock_timer_t
 *
 * @brief todo
 */
typedef struct asock_timer_t
{

}
asock_timer_t;

/**
 * asock_create_timer
 *
 * @brief Creates a new high precision, low performance timer.
 *        May fail and return null.
 *
 * @param loop
 * @param fallthrough
 * @param ext_size
 * @return Newly created timer, or if fails, then NULL.
 */
asock_timer_t *asock_timer_create(
    asock_loop_t *loop, int fallthrough, unsigned int ext_size);

/**
 * asock_timer_close
 *
 * @brief todo
 */
void asock_timer_close(asock_timer_t *timer);

/**
 * asock_timer_set
 *
 * @brief Arms a timer with a delay from now, and eventually a repeat delay.
 *        Specify 0 as repeat delay to disable repeating.
 *        Specify both 0 to disarm.
 *
 * @param t Timer
 * @param cb Callback
 * @param ms Milliseconds
 * @param repeat_ms Repeat Milliseconds
 */
void asock_timer_set(
    asock_timer_t *t, void (*cb)(asock_timer_t *t), int ms, int repeat_ms);

/**
 * asock_timer_ext
 *
 * @brief todo
 *
 * @param timer
 */
void *asock_timer_ext(asock_timer_t *timer);

/**
 * asock_timer_loop
 *
 * @brief todo
 *
 * @param timer
 * @return
 */
asock_loop_t *asock_timer_loop(asock_timer_t *timer);




#endif // ASOCK_TIMER_H
