#include "async.h"
#include "poll.h"
#include <stdint.h>
#include <sys/event.h>
#include <unistd.h>

/**
 * asock_async_close
 *
 * @note
 */
void asock_async_close(asock_async_t *a)
{

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
