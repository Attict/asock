///
/// asock.h
/// ~~~~~~~
///

#ifndef ASOCK_H
#define ASOCK_H

#define ASOCK_EXT_ALIGN 16
#define ASOCK_RECV_BUFFER_LENGTH 524288
#define ASOCK_RECV_BUFFER_PADDING 32
#define ASOCK_TIMEOUT_GRANULARITY 4

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
//
// New Code
//

typedef struct asock_async_t asock_async_t;
typedef struct asock_callback_t asock_callback_t;
typedef struct asock_context_t asock_context_t;
typedef struct asock_loop_t asock_loop_t;
typedef struct asock_poll_t asock_poll_t;
typedef struct asock_socket_t asock_socket_t;
typedef struct asock_timer_t asock_timer_t;

/**
 * asock_options_t
 *
 * @brief todo
 */
typedef struct asock_options_t
{
  const char *key_file_name;
  const char *cert_file_name;
  const char *passphrase;
  const char *dh_params_file_name;
  int ssl_prefer_low_memory_usage;
}
asock_options_t;


asock_loop_t *asock_loop_create(void *hint,
    void (*wakeup_cb)(asock_loop_t *loop), void (*pre_cb)(asock_loop_t *loop),
    void (*post_cb)(asock_loop_t *loop), unsigned int ext_size);
asock_context_t *asock_context_create(int ssl, asock_loop_t *loop,
    int ext_size, asock_options_t options);

///
/// NEW CODE
///


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


/* Public interfaces for contexts */

struct us_socket_context_options_t {
    const char *key_file_name;
    const char *cert_file_name;
    const char *passphrase;
    const char *dh_params_file_name;
    int ssl_prefer_low_memory_usage;
};

/* A socket context holds shared callbacks and user data extension for associated sockets */
asock_context_t *us_create_socket_context(int ssl, asock_loop_t *loop,
    int ext_size, asock_options_t options);

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


/* Returns user data extension for this socket context */
void *us_socket_context_ext(int ssl, struct us_socket_context_t *context);

/* Listen for connections. Acts as the main driving cog in a server. Will call set async callbacks. */
struct us_listen_socket_t *us_socket_context_listen(int ssl, struct us_socket_context_t *context,
    const char *host, int port, int options, int socket_ext_size);


/* Land in on_open or on_close or return null or return socket */
struct us_socket_t *us_socket_context_connect(int ssl, struct us_socket_context_t *context,
    const char *host, int port, int options, int socket_ext_size);


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

/* Hook up timers in existing loop */
void us_loop_integrate(struct us_loop_t *loop);




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

/* Immediately closes the socket */
struct us_socket_t *us_socket_close(int ssl, struct us_socket_t *s);

/* Copy remote (IP) address of socket, or fail with zero length. */
void us_socket_remote_address(int ssl, struct us_socket_t *s, char *buf, int *length);

#ifdef __cplusplus
}
#endif

#define LIBUS_USE_KQUEUE




#endif // ASOCK_H
