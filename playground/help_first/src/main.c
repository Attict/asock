#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdalign.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <sys/event.h> // kqueue?

/**
 * Forward declarations (struct)
 *
 * @brief: todo
 */
typedef struct asock_loop_t asock_loop_t;
typedef struct asock_loop_data_t asock_loop_data_t;

/**
 * asock_loop_data_t
 *
 * @brief: todo
 */
struct asock_loop_data_t
{

};


/**
 * asock_loop_t
 *
 * @brief: todo
 */
struct asock_loop_t
{
  // Loop's own file descriptor
  int fd;

  // Number of non-fallthrough polls in the loop
  int num_polls;

  // Number of ready polls this iteration
  int num_ready_polls;

  // Current index in list of ready polls
  int current_ready_poll;

  // List of ready polls
  struct kevent ready_polls[1024];

  // Data aligned with 16-byte boundary padding
  alignas(16) asock_loop_data_t data;
};

/**
 * asock_close_socket
 *
 * @brief: todo
 */
void asock_close_socket(int fd)
{
  close(fd);
}

/**
 * asock_loop_run
 *
 * @brief:
 *
 * @see: src/eventing/epoll_kqueue.c:107
 *
 * @notes:
 *  - Looks like the polls are checked for events and errors.
 *    If neither exist, then it skips??
 *
 */
void asock_loop_run(asock_loop_t *loop)
{
  while (loop->num_polls)
  {
    // pre-callback

    loop->num_ready_polls =
        kevent(loop->fd, NULL, 0, loop->ready_polls, 1024, NULL);

    for (loop->current_ready_poll = 0;
        loop->current_ready_poll < loop->num_ready_polls;
        loop->current_ready_poll++)
    {
    }

    // post-callback
  }
}

/**
 * asock_create_socket
 *
 * @brief: todo
 */
int asock_create_socket(int domain, int type, int protocol)
{
  int flags = 0;
#if defined(SOCK_CLOEXEC) && defined(SOCK_NONBLOCK)
  flags = SOCK_CLOEXEC | SOCK_NONBLOCK;
#endif

  int created_fd = socket(domain, type | flags, protocol);

  //
  // apple_no_sigpipe
  //
  // @see: src/internal/networking/bsd.h:49
  //
  if (created_fd != -1) {
    int no_sigpipe = 1;
    setsockopt(created_fd, SOL_SOCKET, SO_NOSIGPIPE, &no_sigpipe, sizeof(int));
  }

  //
  // bsd_set_nonblocking
  //
  // @see: src/internal/networking/bsd.h:59
  //
  fcntl(created_fd, F_SETFL, fcntl(created_fd, F_GETFL, 0) | O_NONBLOCK);

  return created_fd;

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
  for (struct addrinfo *a = result; a && listen_fd == -1; a = a->ai_next)
  {
    if (a->ai_family == AF_INET6)
    {
      listen_fd =
          asock_create_socket(a->ai_family, a->ai_socktype, a->ai_protocol);
      listen_addr = a;
    }
  }

  for (struct addrinfo *a = result; a && listen_fd == -1; a = a->ai_next)
  {
    if (a->ai_family == AF_INET)
    {
      listen_fd =
          asock_create_socket(a->ai_family, a->ai_socktype, a->ai_protocol);
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

#ifdef IPV6_V6ONLY
  int disabled = 0;
  setsockopt(listen_fd, IPPROTO_IPV6, IPV6_V6ONLY,
      (int *) &disabled, sizeof(disabled));
#endif

  if (bind(listen_fd, listen_addr->ai_addr, listen_addr->ai_addrlen)
      || listen(listen_fd, 512))
  {
    asock_close_socket(listen_fd);
    return -1;
  }

  freeaddrinfo(result);
  return listen_fd;
}

/**
 * main
 *
 * @brief: todo
 */
int main()
{
  //
  // use_create_loop
  //
  // @see: src/
  //
  asock_loop_t *loop = (asock_loop_t *) malloc(sizeof(asock_loop_t) + 0);
  loop->num_polls = 0;
  loop->fd = kqueue();


  //
  // us_socket_context_listen
  //
  // @see: src/context.c:113
  //
  int listen_socket_fd = asock_create_listen_socket(0, 3000, 0);
  if (listen_socket_fd == -1) {
    printf("Failed to create listen_socket_fd!\n");
    return 1;
  }


  if (listen_socket_fd)
  {
    printf("Listening on 'localhost:3000'...\n");
    asock_loop_run(loop);
  }
  else
  {
    printf("Failed to listen!\n");
  }

  return 0;
}
