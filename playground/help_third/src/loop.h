#ifndef ASOCK_LOOP_H
#define ASOCK_LOOP_H

#include "asock.h"
#include <stdalign.h>
#include <sys/event.h>

/** FIXME **/
typedef struct asock_loop_data_t
{
    asock_timer_t *sweep_timer;
    struct asock_async_t *wakeup_async;
    int last_write_failed;
    asock_context_t *head;
    asock_context_t *iterator;
    char *recv_buf;
    void *ssl_data;
    void (*pre_cb)(asock_loop_t *);
    void (*post_cb)(asock_loop_t *);
    asock_socket_t *closed_head;
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
 * asock_loop_run
 *
 * @brief The main loop
 *
 * @param loop
 */
void asock_loop_run(asock_loop_t *loop);

/**
 * asock_loop_create
 *
 * @brief todo
 *
 * @param hint
 * @param wakeup_cb
 * @param pre_cb
 * @param post_cb
 * @param ext_size
 * @return
 */
asock_loop_t *asock_loop_create(void *hint,
    void (*wakeup_cb)(asock_loop_t *loop), void (*pre_cb)(asock_loop_t *loop),
    void (*post_cb)(asock_loop_t *loop), unsigned int ext_size);

/**
 * asock_loop_data_init
 *
 * @brief todo
 *
 * @param loop
 * @param wakeup_cb
 * @param pre_cb
 * @param post_cb
 * @return
 */
void asock_loop_data_init(asock_loop_t *loop, void (*wakeup_cb)(asock_loop_t *),
    void (*pre_cb)(asock_loop_t *), void (*post_cb)(asock_loop_t *loop));

/**
 * asock_loop_link
 *
 * @brief todo
 *
 * @param loop
 * @param context
 */
void asock_loop_link(asock_loop_t *loop, asock_context_t *context);

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

/**
 * asock_loop_wakeup
 *
 * @brief Signals the loop from any thread to wake up and execute its
 *        wakeup handler from the loops own running thread.
 *        This is the only fully thread-safe function, and serves as the
 *        basis for thread safety.
 *
 * @param loop Loop
 */
void asock_loop_wakeup(asock_loop_t *loop);

/**
 * asock_loop_integrate
 *
 * @brief todo
 *
 * @param loop
 */
void asock_loop_integrate(asock_loop_t *loop);

/**
 * asock_loop_sweep_timer_cb
 *
 * @brief todo
 *
 * @param cb
 */
void asock_loop_sweep_timer_cb(asock_callback_t *cb);

/**
 * asock_loop_ext
 *
 * @brief todo
 *
 * @param loop
 * @return Next loop pointer
 */
void *asock_loop_ext(asock_loop_t *loop);

/**
 * asock_loop_iteration_number
 *
 * @brief todo
 *
 * @param loop
 * @return Iteration number of loop data
 */
long long asock_loop_iteration_number(asock_loop_t *loop);

#endif // ASOCK_LOOP_H
