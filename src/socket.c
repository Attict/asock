#include "asock.h"

/**
 * asock_apple_no_sigpipe
 *
 * @brief: TODO
 */
static inline ASOCK_SOCKET_DESCRIPTOR asock_apple_no_sigpipe(
    ASOCK_SOCKET_DESCRIPTOR fd)
{
#ifdef __APPLE__
  if (fd != ASOCK_SOCKET_ERROR) {
    int no_sigpipe = 1;
    setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &no_sigpipe, sizeof(int));
  }
#endif
  return fd;
}

/**
 * asock_set_nonblocking
 *
 * @brief: TODO
 */
static inline ASOCK_SOCKET_DESCRIPTOR asock_set_nonblocking(
    ASOCK_SOCKET_DESCRIPTOR fd)
{
#ifdef _WIN32
  // LibUV will set windows sockets as non-blocking
#else
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
#endif
  return fd;
}

/**
 * asock_create_socket
 *
 * @brief: TODO
 */
static inline ASOCK_SOCKET_DESCRIPTOR asock_create_socket(int domain,
    int type, int protocol)
{
  int flags = 0;

#if defined(SOCK_CLOEXEC) && defined(SOCK_NONBLOCK)
  flags = SOCK_CLOEXEC | SOCK_NONBLOCK;
#endif

  ASOCK_SOCKET_DESCRIPTOR created_fd = socket(domain, type | flags, protocol);

  return asock_set_nonblocking(asock_apple_no_sigpipe(created_fd));
}

/**
 * asock_create_connect_socket
 *
 * @brief: TODO
 */
static inline ASOCK_SOCKET_DESCRIPTOR asock_create_connect_socket(
    const char* host, int port, int options)
{
  struct addrinfo hints;
  struct addrinfo* result;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  char port_string[16];
  sprintf(port_string, "%d", port);

  if (getaddrinfo(host, port_string, &hints, &result) != 0)
  {
    return ASOCK_SOCKET_ERROR;
  }

  ASOCK_SOCKET_DESCRIPTOR fd = asock_create_socket(result->ai_family,
      result->ai_socktype, result->ai_protocol);

  if (fd == ASOCK_SOCKET_ERROR)
  {
    freeaddrinfo(result);
    return ASOCK_SOCKET_ERROR;
  }

  connect(fd, result->ai_addr, result->ai_addrlen);
  freeaddrinfo(result);

  return fd;
}
