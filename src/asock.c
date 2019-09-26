#include "asock.h"

/**
 * asock_socket_t
 *
 * @brief: todo
 */
struct asock_socket_t
{
  struct asock_socket_context_t *context;
  struct asock_socket_t *next, *prev;
  unsigned short timeout;
};

/**
 * asock_socket_context_t
 *
 * @brief: todo
 */
struct asock_socket_context_t
{
  asock_socket_t *head;
  asock_socket_t *iterator;
  asock_socket_context_t *prev, *next;

  asock_socket_t *(*on_open)(
      asock_socket_t *, int is_client, char *ip, int ip_length);
  asock_socket_t *(*on_data)(asock_socket_t *, char *data, int length);
  asock_socket_t *(*on_writable)(asock_socket_t *);
  asock_socket_t *(*on_close)(asock_socket_t *);
  asock_socket_t *(*on_timeout)(asock_socket_t *);
  asock_socket_t *(*on_end)(asock_socket_t *);

  int (*ignore_data)(asock_socket_t *);
};

/**
 * asock_listen_socket_t
 *
 * @brief: todo
 */
struct asock_listen_socket_t
{
  alignas(ASOCK_ALIGN) asock_socket_t s;
  unsigned int socket_ext_size;
};

// -----------------------------------------------------------------------------

/**
 * asock_create_socket
 *
 * @brief: todo
 */
int asock_create_socket(int domain, int type, int protocol)
{
  int flags = 0;
  int created_fd = socket(domain, type | flags, protocol);
  if (created_fd != -1)
  {
    // APPLE - No SIGPIPE
    int no_sigpipe = 1;
    setsockopt(created_fd, SOL_SOCKET, SO_NOSIGPIPE, &no_sigpipe, sizeof(int));
  }

  fcntl(created_fd, F_SETFL, fcntl(created_fd, F_GETFL, 0) | O_NONBLOCK);
  return created_fd;
}


/**
 * asock_create_connect_socket
 *
 * @brief: todo
 */
int asock_create_connect_socket(const char *host, int port, int options)
{
  struct addrinfo hints, *result;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  char port_string[16];
  sprintf(port_string, "%d", port);

  if (getaddrinfo(host, port_string, &hints, &result) != 0)
  {
    return -1;
  }

  int fd = asock_create_socket(
      result->ai_family, result->ai_socktype, result->ai_protocol);

  if (fd == -1)
  {
    freeaddrinfo(result);
    return -1;
  }

  connect(fd, result->ai_addr, result->ai_addrlen);
  freeaddrinfo(result);

  return fd;
}

/**
 * asock_create_listen_socket
 *
 * @brief: todo
 */
int asock_create_listen_socket(const char *host, int port, int options)
{
  struct addrinfo hints, *result;
  memset(&hints, 0, sizeof(struct addrinfo));

  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  char port_string[16];
  sprintf(port_string, "%d", port);

  if (getaddrinfo(host, port_string, &hints, &result))
  {
    return -1;
  }

  int listen_fd = -1;
  struct addrinfo *listen_addr;

  // Check for INET6
  for (struct addrinfo *a = result; a && listen_fd == -1; a = a->ai_next)
  {
    if (a->ai_family == AF_INET6)
    {
      listen_fd = asock_create_socket(
          a->ai_family, a->ai_socktype, a->ai_protocol);
      listen_addr = a;
    }
  }

  // Fallback to INET
  for (struct addrinfo *a = result; a && listen_fd == -1; a = a->ai_next)
  {
    if (a->ai_family == AF_INET)
    {
      listen_fd = asock_create_socket(
          a->ai_family, a->ai_socktype, a->ai_protocol);
      listen_addr = a;
    }
  }

  // Unable to create socket
  if (listen_fd == -1)
  {
    freeaddrinfo(result);
    return -1;
  }

  //
  // TODO:
  //
  // Always enable SO_REUSEPORT and SO_REUSEADDR.
  //

  int enabled = 1;
  setsockopt(listen_fd, IPPROTO_IPV6, IPV6_V6ONLY,
      (int *) &enabled, sizeof(enabled));

#ifdef IPV6_V6ONLY
  int disabled = 0;
  setsockopt(listen_fd, IPPROTO_IPV6, IPV6_V6ONLY,
      (int *) &disabled, sizeof(disabled));
#endif

  if (bind(listen_fd, listen_addr->ai_addr, listen_addr->ai_addrlen)
      || listen(listen_fd, 512))
  {
    asock_close_socket(listen_fd);
    freeaddrinfo(result);
    return -1;
  }

  freeaddrinfo(result);
  return listen_fd;
}

/**
 * asock_create_socket_context
 *
 * @brief: todo
 */
asock_socket_context_t *asock_create_socket_context(int context_ext_size)
{
  asock_socket_context_t *context = malloc(
      sizeof(asock_socket_context_t) + context_ext_size);
  context->head = 0;
  context->iterator = 0;
  context->next = 0;
  return context;
}

/**
 * asock_socket_context_listen
 *
 * @brief: todo
 */
asock_listen_socket_t *asock_socket_context_listen(
    asock_socket_context_t *context, const char *host,
    int port, int options, int socket_ext_size)
{
  int listen_socket_fd = asock_create_listen_socket(host, port, options);
  if (listen_socket_fd == -1)
  {
    return 0;
  }

  asock_listen_socket_t *ls = (asock_listen_socket_t *) p;


}

/**
 * asock_close_socket
 *
 * @brief: todo
 */
void asock_close_socket(int fd)
{
  shutdown(fd, SHUT_WR);
}
