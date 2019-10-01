#include "asock.h"
#include "async.h"
#include "core.h"
#include "context.h"
#include "loop.h"
#include "poll.h"
#include "timer.h"

#include "us_internal.h"
#include <stdlib.h>

void us_internal_loop_link(asock_loop_t *loop, asock_context_t *context) {
    context->next = loop->data.head;
    context->prev = 0;
    if (loop->data.head) {
        loop->data.head->prev = context;
    }
    loop->data.head = context;
}

/* This functions should never run recursively */
void us_internal_timer_sweep(struct us_loop_t *loop) {
    asock_loop_data_t *loop_data = &loop->data;
    for (loop_data->iterator = loop_data->head; loop_data->iterator; loop_data->iterator = loop_data->iterator->next) {

        struct us_socket_context_t *context = loop_data->iterator;
        for (context->iterator = context->head; context->iterator; ) {

            struct us_socket_t *s = context->iterator;
            if (s->timeout && --(s->timeout) == 0) {

                context->on_socket_timeout(s);

                /* Check for unlink / link */
                if (s == context->iterator) {
                    context->iterator = s->next;
                }
            } else {
                context->iterator = s->next;
            }
        }
    }
}

/* Note: Properly takes the linked list and timeout sweep into account */
void us_internal_free_closed_sockets(struct us_loop_t *loop) {
    /* Free all closed sockets (maybe it is better to reverse order?) */
    if (loop->data.closed_head) {
        for (asock_socket_t *s = loop->data.closed_head; s; ) {
            asock_socket_t *next = s->next;
            asock_poll_free((asock_poll_t *) s, loop);
            s = next;
        }
        loop->data.closed_head = 0;
    }
}

void sweep_timer_cb(struct us_internal_callback_t *cb) {
    us_internal_timer_sweep(cb->loop);
}

long long us_loop_iteration_number(struct us_loop_t *loop) {
    return loop->data.iteration_nr;
}

/* These may have somewhat different meaning depending on the underlying event library */
void us_internal_loop_pre(struct us_loop_t *loop) {
    loop->data.iteration_nr++;
    loop->data.pre_cb(loop);
}

void us_internal_loop_post(struct us_loop_t *loop) {
    us_internal_free_closed_sockets(loop);
    loop->data.post_cb(loop);
}

void us_internal_dispatch_ready_poll(struct us_poll_t *p, int error, int events) {
    switch (asock_poll_type(p)) {
    case POLL_TYPE_CALLBACK: {
            asock_poll_accept_event(p);
            struct us_internal_callback_t *cb = (struct us_internal_callback_t *) p;
            cb->cb(cb->cb_expects_the_loop ? (struct us_internal_callback_t *) cb->loop : (struct us_internal_callback_t *) &cb->p);
        }
        break;
    case POLL_TYPE_SEMI_SOCKET: {
            /* Both connect and listen sockets are semi-sockets
             * but they poll for different events */
            if (asock_poll_events(p) == LIBUS_SOCKET_WRITABLE) {
                asock_socket_t *s = (asock_socket_t *) p;

                asock_poll_change(p, s->context->loop, LIBUS_SOCKET_READABLE);

                /* We always use nodelay */
                asock_core_socket_nodelay(asock_poll_fd(p), 1);

                /* We are now a proper socket */
                asock_poll_set_type(p, POLL_TYPE_SOCKET);

                s->context->on_open(s, 1, 0, 0);
            } else {
                asock_core_listen_t *listen_socket = (asock_core_listen_t *) p;

                asock_core_addr_t addr;

                int client_fd = asock_core_accept_socket(asock_poll_fd(p), &addr);

                if (client_fd == -1) {
                    /* Todo: start timer here */

                } else {

                    /* Todo: stop timer if any */

                    do {
                        struct us_poll_t *p = asock_poll_create(us_socket_context(0, &listen_socket->s)->loop, 0, sizeof(struct us_socket_t) - sizeof(struct us_poll_t) + listen_socket->socket_ext_size);
                        asock_poll_init(p, client_fd, POLL_TYPE_SOCKET);
                        asock_poll_start(p, listen_socket->s.context->loop, LIBUS_SOCKET_READABLE);

                        asock_socket_t *s = (asock_socket_t *) p;

                        s->context = listen_socket->s.context;

                        /* We always use nodelay */
                        asock_core_socket_nodelay(client_fd, 1);

                        asock_context_link(listen_socket->s.context, s);

                        listen_socket->s.context->on_open(s, 0, asock_core_get_ip(&addr), asock_core_addr_ip_len(&addr));

                        /* Exit accept loop if listen socket was closed in on_open handler */
                        if (asock_socket_is_closed(0, &listen_socket->s)) {
                            break;
                        }

                    } while ((client_fd = asock_core_accept_socket(asock_poll_fd(p), &addr)) != -1);
                }
            }
        }
        break;
    case POLL_TYPE_SOCKET_SHUT_DOWN:
    case POLL_TYPE_SOCKET: {
            /* We should only use s, no p after this point */
            struct us_socket_t *s = (struct us_socket_t *) p;

            /* Such as epollerr epollhup */
            if (error) {
                s = us_socket_close(0, s);
                return;
            }

            if (events & LIBUS_SOCKET_WRITABLE) {
                /* Note: if we failed a write as a socket of one loop then adopted
                 * to another loop, this will be wrong. Absurd case though */
                s->context->loop->data.last_write_failed = 0;

                s = s->context->on_writable(s);

                if (asock_socket_is_closed(0, s)) {
                    return;
                }

                /* If we have no failed write or if we shut down, then stop polling for more writable */
                if (!s->context->loop->data.last_write_failed || asock_socket_is_shutdown(0, s)) {
                    asock_poll_change(&s->p, us_socket_context(0, s)->loop, asock_poll_events(&s->p) & LIBUS_SOCKET_READABLE);
                }
            }

            if (events & LIBUS_SOCKET_READABLE) {
                /* Contexts may ignore data and postpone it to next iteration, for balancing purposes such as
                 * when SSL handshakes take too long to finish and we only want a few of them per iteration */
                if (s->context->ignore_data(s)) {
                    break;
                }

                int length = asock_core_recv(asock_poll_fd(&s->p), s->context->loop->data.recv_buf + LIBUS_RECV_BUFFER_PADDING, LIBUS_RECV_BUFFER_LENGTH, 0);
                if (length > 0) {
                    s = s->context->on_data(s, s->context->loop->data.recv_buf + LIBUS_RECV_BUFFER_PADDING, length);
                } else if (!length) {
                    if (asock_socket_is_shutdown(0, s)) {
                        /* We got FIN back after sending it */
                        s = us_socket_close(0, s);
                    } else {
                        /* We got FIN, so stop polling for readable */
                        asock_poll_change(&s->p, us_socket_context(0, s)->loop, asock_poll_events(&s->p) & LIBUS_SOCKET_WRITABLE);
                        s = s->context->on_end(s);
                    }
                } else if (length == -1 && !asock_core_would_block()) {
                    s = us_socket_close(0, s);
                }
            }
        }
        break;
    }
}

/* Integration only requires the timer to be set up */
void us_loop_integrate(struct us_loop_t *loop) {
    asock_timer_set(loop->data.sweep_timer, (void (*)(struct us_timer_t *)) sweep_timer_cb, LIBUS_TIMEOUT_GRANULARITY * 1000, LIBUS_TIMEOUT_GRANULARITY * 1000);
}

void *us_loop_ext(struct us_loop_t *loop) {
    return loop + 1;
}
