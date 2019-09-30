/*
 * Authored by Alex Hultman, 2018-2019.
 * Intellectual property of third-party.

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 *     http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "asock.h"
#include "poll.h"
#include "us_internal.h"
#include <stdlib.h>

#if defined(LIBUS_USE_EPOLL) || defined(LIBUS_USE_KQUEUE)

/* Loop */
void us_loop_free(struct us_loop_t *loop) {
    us_internal_loop_data_free(loop);
    close(loop->fd);
    free(loop);
}

/* Bug: doesn't really SET, rather read and change, so needs to be inited first! */
void us_internal_poll_set_type(struct us_poll_t *p, int poll_type) {
    p->state.poll_type = poll_type | (p->state.poll_type & 12);
}


/* Loop */
struct us_loop_t *us_create_loop(void *hint, void (*wakeup_cb)(struct us_loop_t *loop), void (*pre_cb)(struct us_loop_t *loop), void (*post_cb)(struct us_loop_t *loop), unsigned int ext_size) {
    struct us_loop_t *loop = (struct us_loop_t *) malloc(sizeof(struct us_loop_t) + ext_size);
    loop->num_polls = 0;

#ifdef LIBUS_USE_EPOLL
    loop->fd = epoll_create1(EPOLL_CLOEXEC);
#else
    loop->fd = kqueue();
#endif

    us_internal_loop_data_init(loop, wakeup_cb, pre_cb, post_cb);
    return loop;
}

void us_loop_run(struct us_loop_t *loop) {
    us_loop_integrate(loop);

    /* While we have non-fallthrough polls we shouldn't fall through */
    while (loop->num_polls) {
        /* Emit pre callback */
        us_internal_loop_pre(loop);

        /* Fetch ready polls */
#ifdef LIBUS_USE_EPOLL
        loop->num_ready_polls = epoll_wait(loop->fd, loop->ready_polls, 1024, -1);
#else
        loop->num_ready_polls = kevent(loop->fd, NULL, 0, loop->ready_polls, 1024, NULL);
#endif

        /* Iterate ready polls, dispatching them by type */
        for (loop->current_ready_poll = 0; loop->current_ready_poll < loop->num_ready_polls; loop->current_ready_poll++) {
            struct us_poll_t *poll = GET_READY_POLL(loop, loop->current_ready_poll);
            /* Any ready poll marked with nullptr will be ignored */
            if (poll) {
#ifdef LIBUS_USE_EPOLL
                int events = loop->ready_polls[loop->current_ready_poll].events;
                int error = loop->ready_polls[loop->current_ready_poll].events & (EPOLLERR | EPOLLHUP);
#else
                /* EVFILT_READ, EVFILT_TIME, EVFILT_USER are all mapped to LIBUS_SOCKET_READABLE */
                int events = LIBUS_SOCKET_READABLE;
                if (loop->ready_polls[loop->current_ready_poll].filter == EVFILT_WRITE) {
                    events = LIBUS_SOCKET_WRITABLE;
                }
                int error = loop->ready_polls[loop->current_ready_poll].flags & (EV_ERROR | EV_EOF);
#endif
                /* Always filter all polls by what they actually poll for (callback polls always poll for readable) */
                events &= asock_poll_events(poll);
                if (events || error) {
                    us_internal_dispatch_ready_poll(poll, error, events);
                }
            }
        }
        /* Emit post callback */
        us_internal_loop_post(loop);
    }
}
/* Poll */

#ifdef LIBUS_USE_KQUEUE
/* Helper function for setting or updating EVFILT_READ and EVFILT_WRITE */
int kqueue_change(int kqfd, int fd, int old_events, int new_events, void *user_data) {
    struct kevent change_list[2];
    int change_length = 0;

    /* Do they differ in readable? */
    if ((new_events & LIBUS_SOCKET_READABLE) != (old_events & LIBUS_SOCKET_READABLE)) {
        EV_SET(&change_list[change_length++], fd, EVFILT_READ, (new_events & LIBUS_SOCKET_READABLE) ? EV_ADD : EV_DELETE, 0, 0, user_data);
    }

    /* Do they differ in writable? */
    if ((new_events & LIBUS_SOCKET_WRITABLE) != (old_events & LIBUS_SOCKET_WRITABLE)) {
        EV_SET(&change_list[change_length++], fd, EVFILT_WRITE, (new_events & LIBUS_SOCKET_WRITABLE) ? EV_ADD : EV_DELETE, 0, 0, user_data);
    }

    int ret = kevent(kqfd, change_list, change_length, NULL, 0, NULL);

    // ret should be 0 in most cases (not guaranteed when removing async)

    return ret;
}
#endif

struct us_poll_t *us_poll_resize(struct us_poll_t *p, struct us_loop_t *loop, unsigned int ext_size) {
    int events = asock_poll_events(p);

    struct us_poll_t *new_p = realloc(p, sizeof(struct us_poll_t) + ext_size);
    if (p != new_p && events) {
#ifdef LIBUS_USE_EPOLL
        /* Hack: forcefully update poll by stripping away already set events */
        new_p->state.poll_type = asock_poll_type(new_p);
        us_poll_change(new_p, loop, events);
#else
        /* Forcefully update poll by resetting them with new_p as user data */
        kqueue_change(loop->fd, new_p->state.fd, 0, events, new_p);
#endif

        /* This is needed for epoll also (us_change_poll doesn't update the old poll) */
        asock_loop_update_pending(loop, p, new_p, events, events);
    }

    return new_p;
}

void us_poll_start(struct us_poll_t *p, struct us_loop_t *loop, int events) {
    p->state.poll_type = asock_poll_type(p) | ((events & LIBUS_SOCKET_READABLE) ? POLL_TYPE_POLLING_IN : 0) | ((events & LIBUS_SOCKET_WRITABLE) ? POLL_TYPE_POLLING_OUT : 0);

#ifdef LIBUS_USE_EPOLL
    struct epoll_event event;
    event.events = events;
    event.data.ptr = p;
    epoll_ctl(loop->fd, EPOLL_CTL_ADD, p->state.fd, &event);
#else
    kqueue_change(loop->fd, p->state.fd, 0, events, p);
#endif
}

void us_poll_change(struct us_poll_t *p, struct us_loop_t *loop, int events) {
    int old_events = asock_poll_events(p);
    if (old_events != events) {

        p->state.poll_type = asock_poll_type(p) | ((events & LIBUS_SOCKET_READABLE) ? POLL_TYPE_POLLING_IN : 0) | ((events & LIBUS_SOCKET_WRITABLE) ? POLL_TYPE_POLLING_OUT : 0);

#ifdef LIBUS_USE_EPOLL
        struct epoll_event event;
        event.events = events;
        event.data.ptr = p;
        epoll_ctl(loop->fd, EPOLL_CTL_MOD, p->state.fd, &event);
#else
        kqueue_change(loop->fd, p->state.fd, old_events, events, p);
#endif
        /* Set all removed events to null-polls in pending ready poll list */
        //us_internal_loop_update_pending_ready_polls(loop, p, p, old_events, events);
    }
}

/* Timer */
#ifdef LIBUS_USE_EPOLL
struct us_timer_t *us_create_timer(struct us_loop_t *loop, int fallthrough, unsigned int ext_size) {
    struct us_poll_t *p = asock_poll_create(loop, fallthrough, sizeof(struct us_internal_callback_t) + ext_size);
    asock_poll_init(p, timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC), POLL_TYPE_CALLBACK);

    struct us_internal_callback_t *cb = (struct us_internal_callback_t *) p;
    cb->loop = loop;
    cb->cb_expects_the_loop = 0;

    return (struct us_timer_t *) cb;
}
#else
struct us_timer_t *us_create_timer(struct us_loop_t *loop, int fallthrough, unsigned int ext_size) {
    struct us_internal_callback_t *cb = malloc(sizeof(struct us_internal_callback_t) + ext_size);

    cb->loop = loop;
    cb->cb_expects_the_loop = 0;

    /* Bug: us_internal_poll_set_type does not SET the type, it only CHANGES it */
    cb->p.state.poll_type = POLL_TYPE_POLLING_IN;
    us_internal_poll_set_type((struct us_poll_t *) cb, POLL_TYPE_CALLBACK);

    if (!fallthrough) {
        loop->num_polls++;
    }

    return (struct us_timer_t *) cb;
}
#endif


void us_timer_set(struct us_timer_t *t, void (*cb)(struct us_timer_t *t), int ms, int repeat_ms) {
    struct us_internal_callback_t *internal_cb = (struct us_internal_callback_t *) t;

    internal_cb->cb = (void (*)(struct us_internal_callback_t *)) cb;

    /* Bug: repeat_ms must be the same as ms, or 0 */
    struct kevent event;
    EV_SET(&event, (uintptr_t) internal_cb, EVFILT_TIMER, EV_ADD | (repeat_ms ? 0 : EV_ONESHOT), 0, ms, internal_cb);
    kevent(internal_cb->loop->fd, &event, 1, NULL, 0, NULL);
}
#endif

/* Async (internal helper for loop's wakeup feature) */
struct us_internal_async *us_internal_create_async(struct us_loop_t *loop, int fallthrough, unsigned int ext_size) {
    struct us_internal_callback_t *cb = malloc(sizeof(struct us_internal_callback_t) + ext_size);

    cb->loop = loop;
    cb->cb_expects_the_loop = 1;

    /* Bug: us_internal_poll_set_type does not SET the type, it only CHANGES it */
    cb->p.state.poll_type = POLL_TYPE_POLLING_IN;
    us_internal_poll_set_type((struct us_poll_t *) cb, POLL_TYPE_CALLBACK);

    if (!fallthrough) {
        loop->num_polls++;
    }

    return (struct us_internal_async *) cb;
}

