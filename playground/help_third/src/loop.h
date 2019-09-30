#ifndef ASOCK_LOOP_H
#define ASOCK_LOOP_H

#include "asock.h"
#include "poll.h"
#include <stdalign.h>
#include <sys/event.h>

/** FIXME **/
typedef struct asock_loop_data_t
{
    struct us_timer_t *sweep_timer;
    struct us_internal_async *wakeup_async;
    int last_write_failed;
    struct us_socket_context_t *head;
    struct us_socket_context_t *iterator;
    char *recv_buf;
    void *ssl_data;
    void (*pre_cb)(struct us_loop_t *);
    void (*post_cb)(struct us_loop_t *);
    struct us_socket_t *closed_head;
    /* We do not care if this flips or not, it doesn't matter */
    long long iteration_nr;
}
asock_loop_data_t;

/**
 * asock_loop_t
 *
 * @brief: todo
 *
 */
typedef struct asock_loop_t
{
  alignas(ASOCK_EXT_ALIGN) asock_loop_data_t data;

  // Number of non-fallthrough polls in the loop
  int num_polls;

  // Number of ready polls this iteration
  int num_ready_polls;

  // Current index in list of ready polls
  int current_ready_poll;

  // Loop's own file descriptor
  int fd;

  // The list of ready polls
  struct kevent ready_polls[1024];
}
asock_loop_t;

/**
 * asock_loop_free
 *
 * @brief todo
 *
 * @param loop
 */
void asock_loop_free(asock_loop_t *loop);

/**
 * asock_loop_data_free
 *
 * @brief todo
 *
 * @param loop
 */
void asock_loop_data_free(asock_loop_t *loop);

/**
 * asock_loop_update_pending
 *
 * @brief todo
 *
 * @param loop
 * @param old_poll
 * @param new_poll
 * @param old_events
 * @param new_events
 */
void asock_loop_update_pending(asock_loop_t *loop, asock_poll_t *old_poll,
    asock_poll_t *new_poll, int old_events, int new_events);

#endif // ASOCK_LOOP_H
