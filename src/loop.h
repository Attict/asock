#ifndef ASOCK_LOOP_H
#define ASOCK_LOOP_H

#include "asock.h"
#include <sys/event.h>

/**
 * asock_loop_data_t
 *
 * @brief: TODO
 */
typedef struct asock_loop_data_t
{

}
asock_loop_data_t;

/**
 * asock_loop_t
 *
 * @brief: TODO
 */
typedef struct asock_loop_t
{
  alignas(ASOCK_EXT_ALIGNMENT) asock_loop_data_t data;

  // Number of non-fallthrough polls in the loop
  int num_polls;

  // Number of ready polls this iteration
  int num_ready_polls;

  // Current index in list of ready polls
  int current_ready_poll;

  // Loop's own file descriptor
  int fd;

  //The list of ready polls
#ifdef ASOCK_USE_EPOLL

#else

#endif


}
asock_loop_t;

#endif // ASOCK_LOOP_H
