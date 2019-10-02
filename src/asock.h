///
/// asock.h
/// ~~~~~~~
///

#ifndef ASOCK_H
#define ASOCK_H

#include <stdalign.h>

#define ASOCK_EXT_ALIGN 16
#define ASOCK_RECV_BUFFER_LENGTH 524288
#define ASOCK_RECV_BUFFER_PADDING 32
#define ASOCK_TIMEOUT_GRANULARITY 4

#ifdef __cplusplus
extern "C" {
#endif

typedef struct asock_core_listen_t asock_core_listen_t;
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


// Loop
void asock_loop_run(asock_loop_t *loop);
asock_loop_t *asock_loop_create(void *hint,
    void (*wakeup_cb)(asock_loop_t *loop), void (*pre_cb)(asock_loop_t *loop),
    void (*post_cb)(asock_loop_t *loop), unsigned int ext_size);


// Context
asock_context_t *asock_context_create(int ssl, asock_loop_t *loop,
    int ext_size, asock_options_t options);
asock_core_listen_t *asock_context_listen(int ssl, asock_context_t *context,
    const char *host, int port, int options, int ext_size);
void asock_context_on_open(int ssl, asock_context_t *context,
    asock_socket_t *(*on_open)(asock_socket_t *s,
        int is_client, char *ip, int ip_length));
void asock_context_on_close(int ssl, asock_context_t *context,
    asock_socket_t *(*on_close)(asock_socket_t *s));
void asock_context_on_data(int ssl, asock_context_t *context,
    asock_socket_t *(*on_data)(asock_socket_t *s, char *data, int length));
void asock_context_on_writable(int ssl, asock_context_t *context,
    asock_socket_t *(*on_writable)(asock_socket_t *s));
void asock_context_on_timeout(int ssl, asock_context_t *context,
    asock_socket_t *(*on_timeout)(asock_socket_t *));
void asock_context_on_end(int ssl, asock_context_t *context,
    asock_socket_t *(*on_end)(asock_socket_t *));

// Socket
asock_socket_t *asock_socket_close(int ssl, asock_socket_t *s);
void asock_socket_shutdown(int ssl, asock_socket_t *s);
void *asock_socket_ext(int ssl, asock_socket_t *s);
int asock_socket_write(int ssl, asock_socket_t *s,
    const char *data, int length, int msg_more);
void asock_socket_timeout(int ssl, asock_socket_t *s, unsigned int seconds);


#ifdef __cplusplus
}
#endif

#endif // ASOCK_H
