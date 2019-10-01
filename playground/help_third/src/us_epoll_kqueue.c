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


void us_loop_run(struct us_loop_t *loop) {
    asock_loop_integrate(loop);

    /* While we have non-fallthrough polls we shouldn't fall through */
    while (loop->num_polls) {
        /* Emit pre callback */
        asock_loop_pre(loop);

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
        asock_loop_post(loop);
    }
}
/* Poll */



#endif

