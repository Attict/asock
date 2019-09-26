#include "asock.h"

/**
 * asock_close_socket
 *
 * @brief: todo
 */
int asock_send(ASOCK_SOCKET_DESCRIPTOR fd, const char* buffer, int length,
    int msg_more)
{
#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#ifdef MSG_MORE
  // No signals on Linux
  return send(fd, buffer, length, (msg_more * MSG_MORE) | MSG_NOSIGNAL);
#else
  // Use TCP_NO_PUSH
  return send(fd, buffer, length, MSG_NOSIGNAL);
#endif
}

/**
 * asock_close_socket
 *
 * @brief: todo
 */
void asock_close_socket(ASOCK_SOCKET_DESCRIPTOR fd)
{
  close(fd);
}

/**
 * aosk_shutdown_socket
 *
 * @brief: todo
 */
void asock_shutdown_socket(ASOCK_SOCKET_DESCRIPTOR fd)
{
  shutdown(fd, SHUT_WR);
}
