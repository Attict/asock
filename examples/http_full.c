#include "../src/asock.h"

/**
 * ahttp_socket_t
 *
 * @brief
 */
typedef struct ahttp_socket_t
{
  int offset;
}
ahttp_socket_t;

/**
 * ahttp_context_t
 *
 * @brief
 */
typedef struct ahttp_context_t
{
  char *response;
  int length;
}
ahttp_context_t;

/**
 * ahttp_on_wakeup
 *
 * @brief
 */
void ahttp_on_wakeup(asock_loop_t *loop) {}

/**
 * ahttp_on_pre
 *
 * @brief
 */
void ahttp_on_pre(asock_loop_t *loop) {}

/**
 * ahttp_onpost
 *
 * @brief
 */
void ahttp_on_post(asock_loop_t *loop) {}


/**
 * ahttp_on_open
 *
 * @brief
 */
asock_socket_t *ahttp_on_open(asock_socket_t *s, int is_client,
    char *ip, int ip_length)
{
  ahttp_socket_t *socket = (ahttp_socket_t *) asock_socket_ext(0, s);
  // Reset offset
  socket->offset = 0;
  // Timeout idle http connections
  asock_socket_timeout(0, s, 30);
  return s;
}

asock_socket_t *ahttp_on_close(asock_socket_t *s)
{
  return s;
}

