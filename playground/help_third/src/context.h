#ifndef ASOCK_CONTEXT_H
#define ASOCK_CONTEXT_H

#include "asock.h"
#include "callback.h"

/**
 * asock_context_t
 *
 * @brief todo
 */
typedef struct asock_context_t
{
  alignas(ASOCK_EXT_ALIGN) asock_loop_t *loop;

  asock_socket_t *head;
  asock_socket_t *iterator;
  asock_context_t *prev, *next;

  asock_socket_t *(*on_open)(asock_socket_t *,
      int is_client, char *ip, int ip_length);
  asock_socket_t *(*on_data)(asock_socket_t *, char *data, int length);
  asock_socket_t *(*on_writable)(asock_socket_t *);
  asock_socket_t *(*on_close)(asock_socket_t *);
  asock_socket_t *(*on_socket_timeout)(asock_socket_t *);
  asock_socket_t *(*on_end)(asock_socket_t *);
  int (*ignore_data)(asock_socket_t *);
}
asock_context_t;

/**
 * asock_context_options_t
 *
 * @brief todo
 */
typedef struct asock_context_options_t
{
  const char *key_file_name;
  const char *cert_file_name;
  const char *passphrase;
  const char *dh_params_file_name;
  int ssl_prefer_low_memory_usage;
}
asock_context_options_t;


/**
 * asock_context_create
 *
 * @brief todo
 *
 * @param ssl
 * @param loop
 * @param ext_size
 * @return Context
 */
asock_context_t *asock_context_create(int ssl, asock_loop_t *loop,
    int ext_size, asock_context_options_t options);

/**
 * asock_context_link
 *
 * @brief todo
 *
 * @param context
 * @param socket
 */
void asock_context_link(asock_context_t *context, asock_socket_t *s);

/**
 * asock_context_unlink
 *
 * @brief todo
 *
 * @param context
 * @param socket
 */
void asock_context_unlink(asock_context_t *context, asock_socket_t *s);

/**
 * asock_context_loop
 *
 * @brief todo
 *
 * @param ssl
 * @param context
 * @return Loop
 */
asock_loop_t *asock_context_loop(int ssl, asock_context_t *context);

#endif // ASOCK_CONTEXT_H
