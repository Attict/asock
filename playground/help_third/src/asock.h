///
/// asock.h
/// ~~~~~~~
///

#ifndef ASOCK_H
#define ASOCK_H

#define ASOCK_EXT_ALIGN 16

/* 512kb shared receive buffer */
#define LIBUS_RECV_BUFFER_LENGTH 524288
/* A timeout granularity of 4 seconds means give or take 4 seconds from set timeout */
#define LIBUS_TIMEOUT_GRANULARITY 4
/* 32 byte padding of receive buffer ends */
#define LIBUS_RECV_BUFFER_PADDING 32
/* Guaranteed alignment of extension memory */
#define LIBUS_EXT_ALIGNMENT 16

#include <stdalign.h>
#define LIBUS_SOCKET_DESCRIPTOR int
#define WIN32_EXPORT

#ifdef __cplusplus
extern "C" {
#endif

enum {
    /* No meaning, default listen option */
    LIBUS_LISTEN_DEFAULT,
    /* We exclusively own this port, do not share it */
    LIBUS_LISTEN_EXCLUSIVE_PORT
};

/* Library types publicly available */
struct us_socket_t;
struct us_timer_t;
struct us_socket_context_t;
struct us_loop_t;
struct us_poll_t;

/* Public interfaces for timers */

/* Create a new high precision, low performance timer. May fail and return null */
struct us_timer_t *us_create_timer(struct us_loop_t *loop, int fallthrough, unsigned int ext_size);

/* Returns user data extension for this timer */
void *us_timer_ext(struct us_timer_t *timer);

/* */
void us_timer_close(struct us_timer_t *timer);

/* Arm a timer with a delay from now and eventually a repeat delay.
 * Specify 0 as repeat delay to disable repeating. Specify both 0 to disarm. */
void us_timer_set(struct us_timer_t *timer, void (*cb)(struct us_timer_t *t), int ms, int repeat_ms);

/* Returns the loop for this timer */
struct us_loop_t *us_timer_loop(struct us_timer_t *t);

/* Public interfaces for contexts */

struct us_socket_context_options_t {
    const char *key_file_name;
    const char *cert_file_name;
    const char *passphrase;
    const char *dh_params_file_name;
    int ssl_prefer_low_memory_usage;
};

/* A socket context holds shared callbacks and user data extension for associated sockets */
struct us_socket_context_t *us_create_socket_context(int ssl, struct us_loop_t *loop,
    int ext_size, struct us_socket_context_options_t options);

/* Delete resources allocated at creation time. */
void us_socket_context_free(int ssl, struct us_socket_context_t *context);

/* Setters of various async callbacks */
void us_socket_context_on_open(int ssl, struct us_socket_context_t *context,
    struct us_socket_t *(*on_open)(struct us_socket_t *s, int is_client, char *ip, int ip_length));
void us_socket_context_on_close(int ssl, struct us_socket_context_t *context,
    struct us_socket_t *(*on_close)(struct us_socket_t *s));
void us_socket_context_on_data(int ssl, struct us_socket_context_t *context,
    struct us_socket_t *(*on_data)(struct us_socket_t *s, char *data, int length));
void us_socket_context_on_writable(int ssl, struct us_socket_context_t *context,
    struct us_socket_t *(*on_writable)(struct us_socket_t *s));
void us_socket_context_on_timeout(int ssl, struct us_socket_context_t *context,
    struct us_socket_t *(*on_timeout)(struct us_socket_t *s));

/* Emitted when a socket has been half-closed */
void us_socket_context_on_end(int ssl, struct us_socket_context_t *context, struct us_socket_t *(*on_end)(struct us_socket_t *s));

/* Returns user data extension for this socket context */
void *us_socket_context_ext(int ssl, struct us_socket_context_t *context);

/* Listen for connections. Acts as the main driving cog in a server. Will call set async callbacks. */
struct us_listen_socket_t *us_socket_context_listen(int ssl, struct us_socket_context_t *context,
    const char *host, int port, int options, int socket_ext_size);

/* listen_socket.c/.h */
void us_listen_socket_close(int ssl, struct us_listen_socket_t *ls);

/* Land in on_open or on_close or return null or return socket */
struct us_socket_t *us_socket_context_connect(int ssl, struct us_socket_context_t *context,
    const char *host, int port, int options, int socket_ext_size);

/* Returns the loop for this socket context. */
struct us_loop_t *us_socket_context_loop(int ssl, struct us_socket_context_t *context);

/* Invalidates passed socket, returning a new resized socket which belongs to a different socket context.
 * Used mainly for "socket upgrades" such as when transitioning from HTTP to WebSocket. */
struct us_socket_t *us_socket_context_adopt_socket(int ssl, struct us_socket_context_t *context, struct us_socket_t *s, int ext_size);

/* Create a child socket context which acts much like its own socket context with its own callbacks yet still relies on the
 * parent socket context for some shared resources. Child socket contexts should be used together with socket adoptions and nothing else. */
struct us_socket_context_t *us_create_child_socket_context(int ssl, struct us_socket_context_t *context, int context_ext_size);

/* Public interfaces for loops */

/* Returns a new event loop with user data extension */
struct us_loop_t *us_create_loop(void *hint, void (*wakeup_cb)(struct us_loop_t *loop),
    void (*pre_cb)(struct us_loop_t *loop), void (*post_cb)(struct us_loop_t *loop), unsigned int ext_size);

/* Frees the loop immediately */
void us_loop_free(struct us_loop_t *loop);

/* Returns the loop user data extension */
void *us_loop_ext(struct us_loop_t *loop);

/* Blocks the calling thread and drives the event loop until no more non-fallthrough polls are scheduled */
void us_loop_run(struct us_loop_t *loop);

/* Signals the loop from any thread to wake up and execute its wakeup handler from the loop's own running thread.
 * This is the only fully thread-safe function and serves as the basis for thread safety */
void us_wakeup_loop(struct us_loop_t *loop);

/* Hook up timers in existing loop */
void us_loop_integrate(struct us_loop_t *loop);

/* Returns the loop iteration number */
long long us_loop_iteration_number(struct us_loop_t *loop);

/* Public interfaces for polls */

/* A fallthrough poll does not keep the loop running, it falls through */
struct us_poll_t *us_create_poll(struct us_loop_t *loop, int fallthrough, unsigned int ext_size);


/* Associate this poll with a socket descriptor and poll type */
void us_poll_init(struct us_poll_t *p, LIBUS_SOCKET_DESCRIPTOR fd, int poll_type);

/* Start, change and stop polling for events */
void us_poll_start(struct us_poll_t *p, struct us_loop_t *loop, int events);
void us_poll_change(struct us_poll_t *p, struct us_loop_t *loop, int events);
void us_poll_stop(struct us_poll_t *p, struct us_loop_t *loop);

/* Return what events we are polling for */
int us_poll_events(struct us_poll_t *p);


/* Get associated socket descriptor from a poll */
int us_poll_fd(struct us_poll_t *p);

/* Resize an active poll */
struct us_poll_t *us_poll_resize(struct us_poll_t *p, struct us_loop_t *loop, unsigned int ext_size);

/* Public interfaces for sockets */

/* Write up to length bytes of data. Returns actual bytes written.
 * Will call the on_writable callback of active socket context on failure to write everything off in one go.
 * Set hint msg_more if you have more immediate data to write. */
int us_socket_write(int ssl, struct us_socket_t *s, const char *data, int length, int msg_more);

/* Set a low precision, high performance timer on a socket. A socket can only have one single active timer
 * at any given point in time. Will remove any such pre set timer */
void us_socket_timeout(int ssl, struct us_socket_t *s, unsigned int seconds);

/* Return the user data extension of this socket */
void *us_socket_ext(int ssl, struct us_socket_t *s);

/* Return the socket context of this socket */
struct us_socket_context_t *us_socket_context(int ssl, struct us_socket_t *s);

/* Withdraw any msg_more status and flush any pending data */
void us_socket_flush(int ssl, struct us_socket_t *s);

/* Shuts down the connection by sending FIN and/or close_notify */
void us_socket_shutdown(int ssl, struct us_socket_t *s);

/* Returns whether the socket has been shut down or not */
int us_socket_is_shut_down(int ssl, struct us_socket_t *s);

/* Returns whether this socket has been closed. Only valid if memory has not yet been released. */
int us_socket_is_closed(int ssl, struct us_socket_t *s);

/* Immediately closes the socket */
struct us_socket_t *us_socket_close(int ssl, struct us_socket_t *s);

/* Copy remote (IP) address of socket, or fail with zero length. */
void us_socket_remote_address(int ssl, struct us_socket_t *s, char *buf, int *length);

#ifdef __cplusplus
}
#endif

#define LIBUS_USE_KQUEUE

#endif // ASOCK_H
