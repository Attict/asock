#ifndef ASOCK_H
#define ASOCK_H

#include <sys/event.h>

/**
 * Forward declarations (struct)
 *
 * @brief: todo
 */
typedef struct asock_loop_t asock_loop_t;
typedef struct asock_loop_data_t asock_loop_data_t;

/**
 * asock_loop_data_t
 *
 * @brief: todo
 */
struct asock_loop_data_t
{

};


/**
 * asock_loop_t
 *
 * @brief: todo
 */
struct asock_loop_t
{
  // Loop's own file descriptor
  int fd;

  // Number of non-fallthrough polls in the loop
  int num_polls;

  // Number of ready polls this iteration
  int num_ready_polls;

  // Current index in list of ready polls
  int current_ready_poll;

  // List of ready polls
  struct kevent ready_polls[1024];

  // Data aligned with 16-byte boundary padding
  alignas(16) asock_loop_data_t data;
};

#endif // ASOCK_H
