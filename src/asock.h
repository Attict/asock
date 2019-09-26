#ifndef ASOCK_H
#define ASOCK_H

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>



/**
 * asock_socket_t
 *
 */
struct asock_socket_t
{
  struct asock_socket_t *next, *prev;
  struct asock_socket_context_t *context;
};

struct asock_socket_context_t
{

};

/**
 * asock_create_socket
 *
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

#endif // ASOCK_H
