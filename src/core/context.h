#ifndef ASOCK_CONTEXT_H
#define ASOCK_CONTEXT_H

#include "../asock.h"
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
    int ext_size, asock_options_t options);

/**
 * asock_context_free
 *
 * @brief Frees allocated memory for a context.
 *
 * @param context Context to be freed.
 */
void asock_context_free(int ssl, asock_context_t *context);

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

/**
 * asock_context_ext
 *
 * @brieft todo
 *
 * @param ssl
 * @param context
 * @Return Next context
 */
void *asock_context_ext(int ssl, asock_context_t *context);

/**
 * asock_context_on_end
 *
 * @brief todo
 *
 * @param ssl
 * @param context
 * @param on_end
 */
void asock_context_on_end(int ssl, asock_context_t *context,
    asock_socket_t *(*on_end)(asock_socket_t *));

/**
 * asock_context_on_timeout
 *
 * @brief todo
 *
 * @param ssl
 * @param context
 * @param on_timeout
 */
void asock_context_on_timeout(int ssl, asock_context_t *context,
    asock_socket_t *(*on_timeout)(asock_socket_t *));

/**
 * asock_context_on_writable
 *
 * @brief todo
 *
 * @param ssl
 * @param context
 * @param on_writable
 */
void asock_context_on_writable(int ssl, asock_context_t *context,
    asock_socket_t *(*on_writable)(asock_socket_t *s));

/**
 * asock_context_on_data
 *
 * @brief todo
 *
 * @param ssl
 * @param context
 * @param on_data
 */
void asock_context_on_data(int ssl, asock_context_t *context,
    asock_socket_t *(*on_data)(asock_socket_t *s, char *data, int length));

/**
 * asock_context_on_close
 *
 * @brief todo
 *
 * @param ssl
 * @param context
 * @param on_close
 */
void asock_context_on_close(int ssl, asock_context_t *context,
    asock_socket_t *(*on_close)(asock_socket_t *s));

/**
 * asock_context_on_open
 *
 * @brief todo
 *
 * @param ssl
 * @param context
 * @param on_open
 */
void asock_context_on_open(int ssl, asock_context_t *context,
    asock_socket_t *(*on_open)(asock_socket_t *s,
        int is_client, char *ip, int ip_length));

/**
 * asock_context
 *
 * @brief todo
 *
 * @param ssl
 * @param socket
 * @return Context
 */
asock_context_t *asock_context(int ssl, asock_socket_t *s);

/**
 * asock_context_ignore_data_handler
 *
 * @brief todo
 *
 * @param s Socket
 * @return 0
 */
int asock_context_ignore_data_handler(asock_socket_t *s);

/**
 * asock_context_listen
 *
 * @brief todo
 *
 * @param ssl
 * @param context
 * @param host
 * @param port
 * @param options
 * @param ext_size
 * @return Listen Socket
 */
asock_core_listen_t *asock_context_listen(int ssl, asock_context_t *context,
    const char *host, int port, int options, int ext_size);

/**
 * asock_context_connect
 *
 * @brief todo
 *
 * @param ssl
 * @param context
 * @param host
 * @param port
 * @param options
 * @param ext_size
 * @return Connect Socket
 */
asock_socket_t *asock_context_connect(int ssl, asock_context_t *context,
    const char *host, int port, int options, int ext_size);

/**
 * asock_context_create_child
 *
 * @brief todo
 *
 * @param ssl
 * @param context
 * @param ext_size
 * @return Child Context
 */
asock_context_t *asock_context_create_child(int ssl, asock_context_t *context,
    int ext_size);

/**
 * asock_context_adopt_socket
 *
 * @brief todo
 *
 * @param ssl
 * @param context
 * @param socket
 * @param ext_size
 * @return Socket
 */
asock_socket_t *asock_context_adopt_socket(int ssl, asock_context_t *context,
    asock_socket_t *s, int ext_size);

#endif // ASOCK_CONTEXT_H
