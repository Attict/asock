#ifndef ASOCK_POLL_H
#define ASOCK_POLL_H

#include "../asock.h"

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
#ifdef ASOCK_USE_EPOLL
#define ASOCK_SOCKET_READABLE EPOLLIN
#define ASOCK_SOCKET_WRITABLE EPOLLOUT
#else
#define ASOCK_SOCKET_READABLE 1
#define ASOCK_SOCKET_WRITABLE 2
#endif

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
asock_poll_t *asock_poll_create(asock_loop_t *loop,
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
 * asock_poll_start
 *
 * @brief todo
 *
 * @param p
 * @param loop
 * @param events
 */
void asock_poll_start(asock_poll_t *p, asock_loop_t *loop, int events);

/**
 * asock_poll_stop
 *
 * @brief todo
 *
 * @param p
 * @param loop
 */
void asock_poll_stop(asock_poll_t *p, asock_loop_t *loop);

/**
 * asock_poll_free
 *
 * @brief todo
 *
 * @param p
 * @param loop
 */
void asock_poll_free(asock_poll_t *p, asock_loop_t *loop);

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

/**
 * asock_poll_set_type
 *
 * @brief todo
 *
 * @param p Poll
 * @param poll_type
 */
void asock_poll_set_type(asock_poll_t *p, int poll_type);

/**
 * asock_poll_change
 *
 * @brief todo
 *
 * @param p Poll
 * @param loop Loop
 * @param events
 */
void asock_poll_change(asock_poll_t *p, asock_loop_t *loop, int events);

/**
 * asock_kqueue_change
 *
 * @brief todo
 *
 * @param kqfd
 * @param fd
 * @param old_events
 * @param new_events
 * @param user_data
 * @return
 */
int asock_poll_kqueue_change(int kqfd, int fd, int old_events,
    int new_events, void *user_data);

/**
 * asock_poll_resize
 *
 * @brief todo
 *
 * @param p Poll
 * @param loop Loop
 * @param ext_size Extension size for returning object
 * @return Re-allocated object
 */
asock_poll_t *asock_poll_resize(asock_poll_t *p, asock_loop_t *loop,
    unsigned int ext_size);

/**
 * asock_poll_ready_dispatch
 *
 * @brief todo
 *
 * @param p Poll
 * @param error
 * @param events
 */
void asock_poll_ready_dispatch(asock_poll_t *p, int error, int events);

#endif // ASOCK_POLL_H
