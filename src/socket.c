#include "asock.h"

/**
 * asock_apple_no_sigpipe
 *
 * @brief: TODO
 */
ASOCK_SOCKET_DESCRIPTOR asock_apple_no_sigpipe(ASOCK_SOCKET_DESCRIPTOR fd)
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
ASOCK_SOCKET_DESCRIPTOR asock_set_nonblocking(ASOCK_SOCKET_DESCRIPTOR fd)
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
ASOCK_SOCKET_DESCRIPTOR asock_create_socket(int domain,
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
ASOCK_SOCKET_DESCRIPTOR asock_create_connect_socket(
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

/**
 * asock_socket_ext
 *
 * @brief: todo
 * @todo: add ssl
 */
void* asock_socket_ext(int ssl, asock_socket_t* s)
{
  return s + 1;
}

/**
 * asock_socket_is_closed
 *
 * @brief: todo
 */
int asock_socket_is_closed(int ssl, asock_socket_t* s)
{
  return s->prev == (asock_socket_t*) s->context;
}

/**
 * asock_socket_is_shut_down
 *
 * @brief: todo
 */
int asock_socket_is_shut_down(int ssl, asock_socket_t* s)
{
  return asock_poll_type(&s->poll) == POLL_TYPE_SOCKET_SHUT_DOWN;
}

/**
 * asock_socket_write
 *
 * @brief: todo
 */
int asock_socket_write(int ssl, asock_socket_t* s, const char* data,
    int length, int msg_more)
{
  if (asock_socket_is_closed(ssl, s) || asock_socket_is_shut_down(ssl, s))
  {
    return 0;
  }

  int written = asock_send(asock_poll_fd(&s->poll), data, length, msg_more);
  if (written != length)
  {
    s->context->loop->data.last_write_failed = 1;
    asock_poll_change(&s->poll, s->context->loop,
        ASOCK_SOCKET_READABLE | ASOCK_SOCKET_WRITABLE);
  }

  return written < 0 ? 0 : written;
}

/**
 * asock_socket_timeout
 *
 * @brief: todo
 */
void asock_socket_timeout(int ssl, asock_socket_t* s, unsigned int seconds)
{
  if (seconds)
  {
    unsigned short timeout_sweeps = 0.5f
        + ((float) seconds) / ((float) ASOCK_TIMEOUT_GRANULARITY);
    s->timeout = timeout_sweeps ? timeout_sweeps : 1;
  }
  else
  {
    s->timeout = 0;
  }
}


/**
 * asock_socket_shutdown
 *
 * @brief: todo
 */
void asock_socket_shutdown(int ssl, asock_socket_t* s)
{
  // Todo: Should we emit on_close if calling shutdown on an already
  // half-closed socket?  We need more states in that case, we need to track
  // RECEIVED_FIN so far, the app has to track this and call close as needed.

  if (!asock_socket_is_closed(ssl, s) && !asock_socket_is_shut_down(ssl, s))
  {
    asock_poll_set_type(&s->poll, POLL_TYPE_SOCKET_SHUT_DOWN);
    asock_poll_change(&s->poll, s->context->loop,
        asock_poll_events(&s->poll) & ASOCK_SOCKET_READABLE);
    asock_shutdown_socket(asock_poll_fd((asock_poll_t*) s));
  }
}

/**
 * asock_socket_close
 *
 * @brief: todo
 */
asock_socket_t* asock_socket_close(int ssl, asock_socket_t* s)
{
  if (!asock_socket_is_closed(ssl, s))
  {
    asock_socket_context_unlink(s->context, s);
    asock_poll_stop((asock_poll_t*) s, s->context->loop);
    asock_close_socket(asock_poll_fd((asock_poll_t*) s));

    // Link this socket ot the close-list and let it be deleted after iteration
    s->next = s->context->loop->data.closed_head;
    s->context->loop->data.closed_head = s;

    // Any socket with prev = context is marked as closed
    s->prev = (asock_socket_t*) s->context;
    return s->context->on_close(s);
  }

  return s;
}
