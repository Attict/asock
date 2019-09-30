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
    (struct asock_poll_t *) loop->ready_polls[index].udata

/**
 * SET_READY_POLL
 *
 * @brief todo
 */
#define SET_READY_POLL(loop, index, poll) \
    loop->ready_polls[index].udata = poll

/**
 * ASOCK_SOCKET_READABLE
 * ASOCK_SOCKET_WRITABLE
 *
 * @brief todo
 */
#define ASOCK_SOCKET_READABLE 1
#define ASOCK_SOCKET_WRITABLE 2

/**
 * ASOCK_POLL ENUM
 *
 * @brief todo
 */
enum
{
  ASOCK_POLL_TYPE_SOCKET = 0,
  ASOCK_POLL_TYPE_SHUTDOWN = 1,
  ASOCK_POLL_TYPE_SEMI = 2,
  ASOCK_POLL_TYPE_CALLBACK = 3,

  ASOCK_POLL_OUT = 4,
  ASOCK_POLL_IN = 8
};

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
 * asock_poll_create
 *
 * @brief todo
 *
 * @param
 */
asock_poll_t *asock_poll_create(struct asock_loop_t *loop,
    int fallthrough, unsigned int ext_size);

/**
 * asock_poll_init
 *
 * @brief todo
 *
 * @param p Poll
 * @param fd File Descriptor
 * @param poll_type
 */
void asock_poll_init(asock_poll_t *p, int fd, int poll_type);

/**
 * asock_poll_free
 *
 * @brief todo
 *
 * @param p
 * @param loop
 */
void asock_poll_free(asock_poll_t *p, struct asock_loop_t *loop);

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
 * asock_poll_ext
 *
 * @brief todo
 *
 * @param p Poll
 * @return The next poll in memory
 */
void *asock_poll_ext(asock_poll_t *p);

/**
 * asock_poll_type
 *
 * @brief todo
 *
 * @param p Poll
 * @return
 */
int asock_poll_type(asock_poll_t *p);

/**
 * asock_poll_accept_event
 *
 * @brief todo
 *
 * @param p Poll
 */
unsigned int asock_poll_accept_event(asock_poll_t *p);

/**
 * asock_poll_events
 *
 * @brief todo
 *
 * @param
 */
int asock_poll_events(asock_poll_t *p);

#endif // ASOCK_POLL_H
