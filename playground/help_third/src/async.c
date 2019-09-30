#include "async.h"
#include "callback.h"
#include "poll.h"
#include <stdlib.h>
#include <stdint.h>
#include <sys/event.h>
#include <unistd.h>

/**
 * asock_async_create
 *
 * @bug Type is only changed by `asock_poll_set_type`
 *
 * @note Ayncs (old)
 */
asock_async_t *asock_async_create(asock_loop_t *loop,
    int fallthrough, unsigned int ext_size)
{
  asock_callback_t *cb = malloc(sizeof(asock_callback_t) + ext_size);
  cb->loop = loop;
  cb->cb_expects_the_loop = 1;

  cb->p.state.poll_type = ASOCK_POLL_IN;
  asock_poll_set_type((asock_poll_t *) cb, ASOCK_POLL_TYPE_CALLBACK);

  if (!fallthrough)
  {
    loop->num_polls++;
  }

  return (asock_async_t *) cb;
}

/**
 * asock_async_close
 *
 * @note Asyncs (old)
 */
void asock_async_close(asock_async_t *a)
{
  asock_callback_t *parent = (asock_callback_t *) a;

  // Note: this will fail most of the time, as there probably is no pending
  // trigger.
  struct kevent event;
  EV_SET(&event, (uintptr_t) parent, EVFILT_USER, EV_DELETE, 0, 0, parent);
  kevent(parent->loop->fd, &event, 1, NULL, 0, NULL);

  asock_poll_free((asock_poll_t *) a, parent->loop);
}

/**
 * asock_async_set
 *
 * @note
 */
void asock_async_set(asock_async_t *a, void (*cb)(asock_async_t *))
{
  asock_callback_t *parent = (asock_callback_t *) a;
  parent->cb = (void (*)(asock_callback_t *)) cb;
}

/**
 * asock_async_wakeup
 *
 * @note
 */
void asock_async_wakeup(asock_async_t *a)
{
  asock_callback_t *cb = (asock_callback_t *) a;
  struct kevent e;
  EV_SET(&e, (uintptr_t) cb,
      EVFILT_USER, EV_ADD | EV_ONESHOT, NOTE_TRIGGER, 0, cb);
  kevent(cb->loop->fd, &e, 1, NULL, 0, NULL);
}
