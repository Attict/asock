/**
 * bsd.h
 * ~~~~~
 *
 * Authored by: Eric Wagner <eric@attict.net>
 *
 */

#ifndef BSD_H
#define BSD_H

// top-most wrapper of bsd-like syscalls

// holds everything you need from the bsd/winsock interfaces, only included by internal libusockets.h
// here everything about the syscalls are inline-wrapped and included

#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#define SETSOCKOPT_PTR_TYPE int *
#define LIBUS_SOCKET_ERROR -1


static inline int bsd_create_socket(int domain, int type, int protocol)
{
  // returns INVALID_SOCKET on error
  int flags = 0;
  int created_fd = socket(domain, type | flags, protocol);

  //
  // apple_no_sigpipe
  //
  if (created_fd != -1)
  {
    int no_sigpipe = 1;
    setsockopt(created_fd, SOL_SOCKET, SO_NOSIGPIPE, &no_sigpipe, sizeof(int));
  }

  //
  // bsd_set_nonblocking
  //
  fcntl(created_fd, F_SETFL, fcntl(created_fd, F_GETFL, 0) | O_NONBLOCK);

  return created_fd;
}

/**
 * bsd_addr_t
 *
 * @brief: todo
 *
 */
struct bsd_addr_t
{
  struct sockaddr_storage mem;
  socklen_t len;
  char *ip;
  int ip_length;
};

/**
 * bsd_socket_addr
 *
 * @brief: todo
 *
 */
static inline int bsd_socket_addr(int fd, struct bsd_addr_t *addr)
{
  addr->len = sizeof(addr->mem);
  if (getpeername(fd, (struct sockaddr *) &addr->mem, &addr->len))
  {
    return -1;
  }

  //
  // internal_finalize_bsd_addr
  //
  // parse, so to speak, the address
  //
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
 * bsd_addr_get_ip
 *
 * @brief: todo
 *
 */
static inline char *bsd_addr_get_ip(struct bsd_addr_t *addr)
{
  return addr->ip;
}

/**
 * bsd_addr_get_ip_length
 *
 * @brief: todo
 *
 */
static inline int bsd_addr_get_ip_length(struct bsd_addr_t *addr)
{
  return addr->ip_length;
}

/**
 * bsd_accept_socket
 *
 * @brief: todo
 *
 * @notes: removed calls for `apple_no_sigpipe` and `bsd_set_nonblocking`
 *
 */
static inline int bsd_accept_socket(int fd, struct bsd_addr_t *addr)
{
  int accepted_fd;
  addr->len = sizeof(addr->mem);
  accepted_fd = accept(fd, (struct sockaddr *) addr, &addr->len);

  //
  // internal_finalize_bsd_addr
  //
  // parse, so to speak, the address
  //
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

  //
  // apple_no_sigpipe
  //
  if (accepted_fd != -1)
  {
    int no_sigpipe = 1;
    setsockopt(accepted_fd, SOL_SOCKET, SO_NOSIGPIPE, &no_sigpipe, sizeof(int));
  }

  //
  // bsd_set_nonblocking
  //
  fcntl(accepted_fd, F_SETFL, fcntl(accepted_fd, F_GETFL, 0) | O_NONBLOCK);

  // Return after we've set `apple_no_sigpipe` and `bsd_set_nonblocking`
  return accepted_fd;
}

/**
 * bsd_recv
 *
 * @brief: Receives incoming message from the connected client.
 */
static inline int bsd_recv(int fd, void *buffer, int length, int flags)
{
  return recv(fd, buffer, length, flags);
}


static inline int bsd_send(int fd, const char *buf, int length, int msg_more) {

  // MSG_MORE (Linux), MSG_PARTIAL (Windows), TCP_NOPUSH (BSD)

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#ifdef MSG_MORE

  // for Linux we do not want signals
  return send(fd, buf, length, (msg_more * MSG_MORE) | MSG_NOSIGNAL);

#else

  // use TCP_NOPUSH

  return send(fd, buf, length, MSG_NOSIGNAL);

#endif
}

static inline int bsd_would_block()
{
  return errno == EWOULDBLOCK;// || errno == EAGAIN;
}

// return LIBUS_SOCKET_ERROR or the fd that represents listen socket
// listen both on ipv6 and ipv4
static inline int bsd_create_listen_socket(const char *host, int port, int options)
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

  int listenFd = -1;
  struct addrinfo *listenAddr;
  for (struct addrinfo *a = result; a && listenFd == -1; a = a->ai_next)
  {
    if (a->ai_family == AF_INET6) {
      listenFd = bsd_create_socket(a->ai_family, a->ai_socktype, a->ai_protocol);
      listenAddr = a;
    }
  }

  for (struct addrinfo *a = result; a && listenFd == -1; a = a->ai_next) {
    if (a->ai_family == AF_INET) {
      listenFd = bsd_create_socket(a->ai_family, a->ai_socktype, a->ai_protocol);
      listenAddr = a;
    }
  }

  if (listenFd == -1) {
    freeaddrinfo(result);
    return LIBUS_SOCKET_ERROR;
  }

  /* Always enable SO_REUSEPORT and SO_REUSEADDR _unless_ options specify otherwise */
#if defined(__linux) && defined(SO_REUSEPORT)
  if (!(options & LIBUS_LISTEN_EXCLUSIVE_PORT)) {
    int optval = 1;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
  }
#endif

  int enabled = 1;
  setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, (SETSOCKOPT_PTR_TYPE) &enabled, sizeof(enabled));

#ifdef IPV6_V6ONLY
  int disabled = 0;
  setsockopt(listenFd, IPPROTO_IPV6, IPV6_V6ONLY, (SETSOCKOPT_PTR_TYPE) &disabled, sizeof(disabled));
#endif

  if (bind(listenFd, listenAddr->ai_addr, listenAddr->ai_addrlen) || listen(listenFd, 512)) {
    close(listenFd);
    freeaddrinfo(result);
    return -1;
  }

  freeaddrinfo(result);
  return listenFd;
}

/**
 *
 */
static inline int bsd_create_connect_socket(const char *host, int port, int options)
{
  struct addrinfo hints, *result;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  char port_string[16];
  sprintf(port_string, "%d", port);

  if (getaddrinfo(host, port_string, &hints, &result) != 0) {
    return LIBUS_SOCKET_ERROR;
  }

  int fd = bsd_create_socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if (fd == -1) {
    freeaddrinfo(result);
    return -1;
  }

  connect(fd, result->ai_addr, result->ai_addrlen);
  freeaddrinfo(result);

  return fd;
}

#endif // BSD_H
