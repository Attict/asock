#include "core.h"
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * asock_core_create_socket
 *
 * @note
 */
int asock_core_create_socket(int domain, int type, int protocol)
{
  int flags = 0;
  int created_fd = socket(domain, type | flags, protocol);

  // No sigpipe for apple
#ifdef __APPLE__
  if (created_fd != -1)
  {
    int no_sigpipe = 1;
    setsockopt(created_fd, SOL_SOCKET, SO_NOSIGPIPE, &no_sigpipe, sizeof(int));
  }
#endif

  // Set non-blocking
  fcntl(created_fd, F_SETFL, fcntl(created_fd, F_GETFL, 0) | O_NONBLOCK);

  return created_fd;
}

/**
 * asock_core_connect_socket
 *
 * @note
 */
int asock_core_connect_socket(const char *host, int port, int options)
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

  int fd = asock_core_create_socket(
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
 * asock_core_listen_socket
 *
 * @note
 */
int asock_core_listen_socket(const char *host, int port, int options)
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

  // IPV6
  for (struct addrinfo *a = result; a && listen_fd == -1; a = a->ai_next)
  {
    if (a->ai_family == AF_INET6)
    {
      listen_fd = asock_core_create_socket(
          a->ai_family, a->ai_socktype, a->ai_protocol);
      listen_addr = a;
    }
  }

  // IPV4
  for (struct addrinfo *a = result; a && listen_fd == -1; a = a->ai_next)
  {
    if (a->ai_family == AF_INET)
    {
      listen_fd = asock_core_create_socket(
          a->ai_family, a->ai_socktype, a->ai_protocol);
      listen_addr = a;
    }
  }

  if (listen_fd == -1)
  {
    freeaddrinfo(result);
    return -1;
  }

  int enabled = 1;
  setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR,
      (int *) &enabled, sizeof(enabled));

  // IPV6
#ifdef IPV6_V6ONLY
  int disabled = 0;
  setsockopt(listen_fd, IPPROTO_IPV6, IPV6_V6ONLY,
      (int *) &disabled, sizeof(disabled));
#endif

  if (bind(listen_fd, listen_addr->ai_addr, listen_addr->ai_addrlen)
      || listen(listen_fd, 512))
  {
    close(listen_fd);
    freeaddrinfo(result);
    return -1;
  }

  freeaddrinfo(result);
  return listen_fd;
}

/**
 * asock_core_listen_close
 *
 */
void asock_core_listen_close(int ssl, asock_core_listen_t *ls)
{
  // asock_listen_socket_t extends asock_socket_t so we close in similar ways

  if (!asock_socket_is_closed(0, &ls->s))
  {
    asock_context_unlink(ls->s.context, &ls->s);
    asock_poll_stop((asock_poll_t *) &ls->s, ls->s.context->loop);
    asock_core_close_socket(asock_poll_fd((asock_poll_t *) &ls->s));

    // Link this socket to the close-list and
    // let it be deleted after this iteration
    ls->s.next = ls->s.context->loop->data.closed_head;
    ls->s.context->loop->data.closed_head = &ls->s;

    // Any socket with prev = context is marked as closed
    ls->s.prev = (asock_socket_t *) ls->s.context;
  }

  // We cannot immediately free a listen socket
  // as we can be inside an accept loop
}

/**
 * asock_core_shutdown_socket
 *
 * @note
 */
void asock_core_shutdown_socket(int fd)
{
  shutdown(fd, SHUT_WR);
}

/**
 * asock_core_close_socket
 *
 * @note
 */
void asock_core_close_socket(int fd)
{
  close(fd);
}

/**
 * asock_core_socket_flush
 *
 * @note
 */
void asock_core_socket_flush(int fd)
{
#ifdef TCP_CORK
  int enabled = 0;
  setsockopt(fd, IPPROTO_TCP, TCP_CORK, &enabled, sizeof(int));
#endif
}

/**
 * asock_core_socket_nodelay
 *
 * @note
 */
void asock_core_socket_nodelay(int fd, int enabled)
{
  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *) &enabled, sizeof(enabled));
}

/**
 * asock_core_socket_addr
 *
 * @note
 */
int asock_core_socket_addr(int fd, asock_core_addr_t *addr)
{
  addr->len = sizeof(addr->mem);
  if (getpeername(fd, (struct sockaddr *) &addr->mem, &addr->len))
  {
    return -1;
  }

  // Parse the address
  if (addr->mem.ss_family == AF_INET6)
  {
    addr->ip = (char *) &((struct sockaddr_in6 *) addr)->sin6_addr;
    addr->ip_length = sizeof(struct in6_addr);
  }
  else if (addr->mem.ss_family == AF_INET)
  {
    addr->ip = (char *) &((struct sockaddr_in *) addr)->sin_addr;
    addr->ip_length = sizeof(struct in_addr);
  }
  else
  {
    addr->ip_length = 0;
  }

  return 0;
}

/**
 * asock_core_accept_socket
 *
 * @note
 */
int asock_core_accept_socket(int fd, asock_core_addr_t *addr)
{
  int accepted_fd;
  addr->len = sizeof(addr->mem);
  accepted_fd = accept(fd, (struct sockaddr *) addr, &addr->len);

  // Parse the address
  if (addr->mem.ss_family == AF_INET6)
  {
    addr->ip = (char *) &((struct sockaddr_in6 *) addr)->sin6_addr;
    addr->ip_length = sizeof(struct in6_addr);
  }
  else if (addr->mem.ss_family == AF_INET)
  {
    addr->ip = (char *) &((struct sockaddr_in *) addr)->sin_addr;
    addr->ip_length = sizeof(struct in_addr);
  }
  else
  {
    addr->ip_length = 0;
  }

  // No SIGPIPE on Apple
#ifdef __APPLE__
  if (accepted_fd != -1)
  {
    int no_sigpipe = 1;
    setsockopt(accepted_fd, SOL_SOCKET, SO_NOSIGPIPE, &no_sigpipe, sizeof(int));
  }
#endif

  // Set non-blocking
  fcntl(accepted_fd, F_SETFL, fcntl(accepted_fd, F_GETFL, 0) | O_NONBLOCK);

  return accepted_fd;
}

/**
 * asock_core_recv
 *
 * @note
 */
int asock_core_recv(int fd, void *buffer, int length, int flags)
{
  return recv(fd, buffer, length, flags);
}

/**
 * asock_core_send
 *
 * @note
 */
int asock_core_send(int fd, const char *buffer, int length, int msg_more)
{
  return send(fd, buffer, length, 0);
}

/**
 * asock_core_would_block
 *
 * @note
 */
int asock_core_would_block()
{
  return errno == EWOULDBLOCK;
}

/**
 * asock_core_addr_ip_len
 *
 * @note
 */
int asock_core_addr_ip_len(asock_core_addr_t *addr)
{
  return addr->ip_length;
}

/**
 * asock_core_get_ip
 *
 * @note
 */
char *asock_core_get_ip(asock_core_addr_t *addr)
{
  return addr->ip;
}
