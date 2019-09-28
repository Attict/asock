#ifndef ASOCK_POLL_H
#define ASOCK_POLL_H

#include "asock.h"
#include "loop.h"

/**
 * GET_READY_POLL
 *
 * @brief todo
 */
#define GET_READY_POLL(loop, index) \
    (struct us_poll_t *) loop->ready_polls[index].udata

/**
 * SET_READY_POLL
 *
 * @brief todo
 */
#define SET_READY_POLL(loop, index, poll) \
    loop->ready_polls[index].udata = poll

/**
 * asock_poll_t
 *
 * @brief todo
 */
typedef struct asock_poll_t
{
    alignas(ASOCK_EXT_ALIGN) struct
    {
        int fd : 28;
        unsigned int poll_type : 4;
    }
    state;
}
asock_poll_t;

/**
 * asock_poll_fd
 *
 * @brief todo
 *
 * @param poll
 * @return The poll's state File Descriptor
 */
int asock_poll_fd(asock_poll_t *poll);

/**
 * asock_poll_free
 *
 * @brief todo
 *
 * @param p
 * @param loop
 */
void asock_poll_free(asock_poll_t *p, asock_loop_t *loop);

#endif // ASOCK_POLL_H
