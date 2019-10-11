#ifndef ASOCK_SSL_H
#define ASOCK_SSL_H

#include "../asock.h"
#include "socket.h"
#include <openssl/ssl.h>
#include <openssl/bio.h>

/**
 * ASOCK_MAX_HANDSHAKES_PLI
 *
 * @breif Max handshakes per loop iteration.
 */
static const int ASOCK_MAX_HANDSHAKES_PLI = 5;

/**
 * asock_ssl_data_t
 *
 * @brief
 */
typedef struct asock_ssl_data_t
{
  char *ssl_read_input, *ssl_read_output;
  unsigned int ssl_read_input_length;
  unsigned int ssl_read_input_offset;
  asock_socket_t *ssl_socket;

  int last_write_was_msg_more;
  int msg_more;

  // These are used to throttle SSL handshakes per loop iteration
  long long last_iteration_nr;
  int handshake_budge;

  BIO *shared_rbio;
  BIO *shared_wbio;
  BIO_METHOD *shared_biom;
}
asock_ssl_data_t;

/**
 * asock_ssl_socket_t
 *
 * @brief
 */
typedef struct asock_ssl_socket_t
{
  asock_socket_t s;
  SSL *ssl;
  int ssl_write_wants_read;
}
asock_ssl_socket_t;

typedef struct asock_ssl_context_t
{
  asock_context_t sc;

  SSL_CTX *ssl_context;
  int is_parent;

  asock_ssl_socket_t *(*on_open)(asock_ssl_socket_t *,
      int is_client, char *ip, int ip_length);
  asock_ssl_socket_t *(*on_data)(asock_ssl_socket_t *,
      char *data, int length);
  asock_ssl_socket_t *(*on_close)(asock_ssl_socket_t *);
}
asock_ssl_context_t;



#endif // ASOCK_SSL_H
