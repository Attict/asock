#include "core.h"
#include "context.h"
#include "poll.h"
#include "socket.h"
#include <stdlib.h>
#include <sys/event.h>

/**
 * asock_poll_create
 *
 */
asock_poll_t *asock_poll_create(asock_loop_t *loop,
    int fallthrough, unsigned int ext_size)
{
  if (!fallthrough)
  {
    loop->num_polls++;
  }
  return malloc(sizeof(asock_poll_t) + ext_size);
}

/**
 * asock_poll_init
 *
 */
void asock_poll_init(asock_poll_t *p, int fd, int poll_type)
{
  p->state.fd = fd;
  p->state.poll_type = poll_type;
}

/**
 * asock_poll_start
 *
 */
void asock_poll_start(asock_poll_t *p, asock_loop_t *loop, int events)
{
  p->state.poll_type = asock_poll_type(p)
      | ((events & ASOCK_SOCKET_READABLE) ? ASOCK_POLL_IN : 0)
      | ((events & ASOCK_SOCKET_WRITABLE) ? ASOCK_POLL_OUT : 0);

  asock_poll_kqueue_change(loop->fd, p->state.fd, 0, events, p);
}

/**
 * asock_poll_stop
 *
 */
void asock_poll_stop(asock_poll_t *p, asock_loop_t *loop)
{
  int old_events = asock_poll_events(p);
  int new_events = 0;

  if (old_events)
  {
    asock_poll_kqueue_change(loop->fd, p->state.fd, old_events, new_events, NULL);
  }

  // Disable any instance of us in the pending ready poll list
  asock_loop_update_pending(loop, p, 0, old_events, new_events);
}

/**
 * asock_poll_free
 *
 */
void asock_poll_free(asock_poll_t *p, asock_loop_t *loop)
{
  loop->num_polls--;
  free(p);
}

/**
 * asock_poll_fd
 *
 */
int asock_poll_fd(asock_poll_t *poll)
{
  return poll->state.fd;
}

/**
 * asock_poll_ext
 *
 */
void *asock_poll_ext(asock_poll_t *p)
{
  return p + 1;
}

/**
 * asock_poll_type
 *
 */
int asock_poll_type(asock_poll_t *p)
{
  return p->state.poll_type & 3;
}

/**
 * asock_poll_accept_event
 *
 * @todo EPOLL handling
 */
unsigned int asock_poll_accept_event(asock_poll_t *p)
{
  return 0;
}

/**
 * asock_poll_events
 *
 */
int asock_poll_events(asock_poll_t *p)
{
  return ((p->state.poll_type & ASOCK_POLL_IN) ? ASOCK_SOCKET_READABLE : 0)
      | ((p->state.poll_type & ASOCK_POLL_OUT) ? ASOCK_SOCKET_WRITABLE : 0);
}

/**
 * asock_poll_set_type
 *
 * @bug Doesn't really SET, rather read and change, so needs to be
 *      initiated first!
 */
void asock_poll_set_type(asock_poll_t *p, int poll_type)
{
  p->state.poll_type = poll_type | (p->state.poll_type & 12);
}

/**
 * asock_poll_change
 *
 */
void asock_poll_change(asock_poll_t *p, asock_loop_t *loop, int events)
{
  int old_events = asock_poll_events(p);
  if (old_events != events)
  {
    p->state.poll_type = asock_poll_type(p)
      | ((events & ASOCK_SOCKET_WRITABLE) ? ASOCK_POLL_IN : 0)
      | ((events & ASOCK_SOCKET_READABLE) ? ASOCK_POLL_OUT : 0);

    asock_poll_kqueue_change(loop->fd, p->state.fd, old_events, events, p);
  }
}

/**
 * asock_kqueue_change
 *
 */
int asock_poll_kqueue_change(int kqfd, int fd, int old_events,
    int new_events, void *user_data)
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

/**
 * asock_poll_resize
 *
 */
asock_poll_t *asock_poll_resize(asock_poll_t *p, asock_loop_t *loop,
    unsigned int ext_size)
{
  int events = asock_poll_events(p);
  asock_poll_t *new_p = realloc(p, sizeof(asock_poll_t) + ext_size);

  if (p != new_p && events)
  {
    // Forcefully update poll by resetting them with new_p as user data.
    asock_poll_kqueue_change(loop->fd, new_p->state.fd, 0, events, new_p);
    asock_loop_update_pending(loop, p, new_p, events, events);
  }

  return new_p;
}

/**
 * asock_poll_ready_dispatch
 *
 */
void asock_poll_ready_dispatch(asock_poll_t *p, int error, int events)
{
  switch (asock_poll_type(p))
  {
  case ASOCK_POLL_TYPE_CALLBACK:
    asock_poll_accept_event(p);
    asock_callback_t *parent = (asock_callback_t *) p;
    parent->cb(parent->cb_expects_the_loop
        ? (asock_callback_t *) parent->loop : (asock_callback_t *) &parent->p);
    break;

  case ASOCK_POLL_TYPE_SEMI:
    // Both connect and listen sockets are semi-sockets
    // but they poll for different events
    if (asock_poll_events(p) == ASOCK_SOCKET_WRITABLE)
    {
      asock_socket_t *s = (asock_socket_t *) p;
      asock_poll_change(p, s->context->loop, ASOCK_SOCKET_WRITABLE);
      // We always use no-delay
      asock_core_socket_nodelay(asock_poll_fd(p), 1);
      // We are now a proper socket
      asock_poll_set_type(p, ASOCK_POLL_TYPE_SOCKET);

      s->context->on_open(s, 1, 0, 0);
    }
    else
    {
      asock_core_listen_t *ls = (asock_core_listen_t *) p;
      asock_core_addr_t addr;
      int client_fd = asock_core_accept_socket(asock_poll_fd(p), &addr);
      if (client_fd == -1)
      {
        // todo: start timer here
      }
      else
      {
        // todo: stop timer if any

        do
        {
          asock_poll_t *p = asock_poll_create(
              asock_context(0, &ls->s)->loop, 0, sizeof(asock_socket_t)
                  - sizeof(asock_poll_t) + ls->socket_ext_size);

          asock_poll_init(p, client_fd, ASOCK_POLL_TYPE_SOCKET);
          asock_poll_start(p, ls->s.context->loop, ASOCK_SOCKET_READABLE);

          asock_socket_t *s = (asock_socket_t *) p;
          s->context = ls->s.context;

          // We always use no-delay
          asock_core_socket_nodelay(client_fd, 1);
          asock_context_link(ls->s.context, s);
          ls->s.context->on_open(
              s, 0, asock_core_get_ip(&addr), asock_core_addr_ip_len(&addr));

          // Exit accept loop if listen socket was closed in on_open handler
          if (asock_socket_is_closed(0, &ls->s))
          {
            break;
          }
        }
        while ((client_fd
            = asock_core_accept_socket(asock_poll_fd(p), &addr)) != -1);
      }
    }
    break;

  case ASOCK_POLL_TYPE_SHUTDOWN:
  case ASOCK_POLL_TYPE_SOCKET:
    {
      // We should only use s, no p after this point.
      asock_socket_t *s = (asock_socket_t *) p;

      // Such as epollerr epollhup
      if (error)
      {
        s = asock_socket_close(0, s);
        return;
      }

      if (events & ASOCK_SOCKET_WRITABLE)
      {
        // Note: if we fail a write as a socket of one loop then adopted
        // to another loop, this will be wrong.  Absurd case though.
        s->context->loop->data.last_write_failed = 0;

        s = s->context->on_writable(s);

        if (asock_socket_is_closed(0, s))
        {
          return;
        }
      }
    }
    break;
  }
}
