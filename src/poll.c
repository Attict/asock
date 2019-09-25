#include "asock.h"

/**
 * asock_poll_fd
 *
 * @brief: todo
 */
ASOCK_SOCKET_DESCRIPTOR asock_poll_fd(asock_poll_t* p)
{
  return p->state.fd;
}

/**
 * asock_poll_type
 *
 * @brief: todo
 */
int asock_poll_type(asock_poll_t* p)
{
  return p->state.poll_type & 3;
}

/**
 * asock_poll_events
 *
 * @brief: todo
 */
int asock_poll_events(asock_poll_t* p)
{
  return ((p->state.poll_type & POLL_TYPE_POLLING_IN)
        ? ASOCK_SOCKET_READABLE : 0)
      | ((p->state.poll_type & POLL_TYPE_POLLING_OUT)
        ? ASOCK_SOCKET_WRITABLE : 0);
}

/**
 * asock_poll_change
 *
 * @brief: todo
 */
void asock_poll_change(asock_poll_t* p, asock_loop_t* loop, int events)
{
  int old_events = asock_poll_events(p);
  if (old_events != events)
  {
    p->state.poll_type = asock_poll_type(p)
        | ((events & ASOCK_SOCKET_READABLE) ? POLL_TYPE_POLLING_IN : 0)
        | ((events & ASOCK_SOCKET_WRITABLE) ? POLL_TYPE_POLLING_OUT : 0);

    asock_kqueue_change(loop->fd, p->state.fd, old_events, events, p);
  }
}

/**
 * asock_kqueue_change
 *
 * @brief: todo
 *
 * @todo: This is only for `kqueue`, and will not work on Linux/Windows
 *        which will require `epoll` or `libuv`, etc.
 */
int asock_kqueue_change(int kqfd, int fd, int old_events, int new_events,
    void* user_data)
{
  struct kevent change_list[2];
  int change_length = 0;

  // Do they differ in readable?
  if ((new_events & ASOCK_SOCKET_READABLE)
      != (old_events & ASOCK_SOCKET_READABLE))
  {
    EV_SET(&change_list[change_length++], fd, EVFILT_READ,
        (new_events & ASOCK_SOCKET_READABLE) ? EV_ADD : EV_DELETE,
        0, 0, user_data);
  }

  // Do they differ in writable?
  if ((new_events & ASOCK_SOCKET_WRITABLE)
      != (old_events & ASOCK_SOCKET_WRITABLE))
  {
    EV_SET(&change_list[change_length++], fd, EVFILT_WRITE,
        (new_events & ASOCK_SOCKET_WRITABLE) ? EV_ADD : EV_DELETE,
        0, 0, user_data);
  }

  int ret = kevent(kqfd, change_list, change_length, NULL, 0, NULL);

  // ret should be 0 in most cases (not guaranteed when removing async)
  return ret;
}

