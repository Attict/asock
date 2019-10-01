#include "callback.h"
#include "context.h"
#include "socket.h"
#include "timer.h"
#include <stdlib.h>
#include <sys/event.h>

/**
 * asock_create_timer
 *
 */
asock_timer_t *asock_timer_create(
    asock_loop_t *loop, int fallthrough, unsigned int ext_size)
{
  asock_callback_t *cb = malloc(sizeof(asock_callback_t) + ext_size);
  cb->loop = loop;
  cb->cb_expects_the_loop = 0;

  cb->p.state.poll_type = ASOCK_POLL_IN;
  asock_poll_set_type((asock_poll_t *) cb, ASOCK_POLL_TYPE_CALLBACK);

  if (!fallthrough)
  {
    loop->num_polls++;
  }

  return (asock_timer_t *) cb;
}

/**
 * asock_timer_close
 *
 */
void asock_timer_close(asock_timer_t *timer)
{
  asock_callback_t *cb = (asock_callback_t *) timer;

  struct kevent event;
  EV_SET(&event, (uintptr_t) cb, EVFILT_TIMER, EV_DELETE, 0, 0, cb);

  // (Regular) Sockets are the only polls which are not freed immediately.
  asock_poll_free((asock_poll_t *) timer, cb->loop);
}

/**
 * asock_timer_set
 *
 */
void asock_timer_set(
    asock_timer_t *t, void (*cb)(asock_timer_t *t), int ms, int repeat_ms)
{
  asock_callback_t *parent = (asock_callback_t *) t;
  parent->cb = (void (*)(asock_callback_t *)) cb;

  // Bug: repeat_ms must be the same as ms, or 0
  struct kevent event;
  EV_SET(&event, (uintptr_t) parent,
      EVFILT_TIMER, EV_ADD  | (repeat_ms ? 0 : EV_ONESHOT), 0, ms, parent);
  kevent(parent->loop->fd, &event, 1, NULL, 0, NULL);
}

/**
 * asock_timer_ext
 *
 */
void *asock_timer_ext(asock_timer_t *timer)
{
  return ((asock_callback_t *) timer) + 1;
}

/**
 * asock_timer_loop
 *
 */
asock_loop_t *asock_timer_loop(asock_timer_t *t)
{
  asock_callback_t *cb = (asock_callback_t *) t;
  return cb->loop;
}

/**
 * asock_timer_sweep
 *
 */
void asock_timer_sweep(asock_loop_t *loop)
{
  asock_loop_data_t *loop_data = &loop->data;
  for (loop_data->iterator = loop_data->head; loop_data->iterator;
      loop_data->iterator = loop_data->iterator->next)
  {
    asock_context_t *context = loop_data->iterator;
    for (context->iterator = context->head; context->iterator; )
    {
      asock_socket_t *s = context->iterator;
      if (s->timeout && --(s->timeout) == 0)
      {
        context->on_socket_timeout(s);

        // Check for unlink / link
        if (s == context->iterator)
        {
          context->iterator = s->next;
        }
      }
      else
      {
        context->iterator = s->next;
      }
    }
  }
}

/**
 * asock_timer_sweep_cb
 *
 */
void asock_timer_sweep_cb(asock_callback_t *cb)
{
  asock_timer_sweep(cb->loop);
}

