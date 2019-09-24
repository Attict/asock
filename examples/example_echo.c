#include "../src/asock.h"
#include "stdio.h"

const int SSL = 1;

/**
 * Extension of socket
 *
 * @brief: TODO
 */
struct echo_socket
{
  char *backpressure;
  int length;
};

/**
 * Extension of socket context
 *
 * @brief: TODO
 */
struct echo_context
{

};

/**
 * Loop wakeup handler
 *
 * @brief: TODO
 */
void on_wakeup(asock_loop_t* loop)
{
}

/**
 * Loop pre-iteration handler
 *
 * @brief: TODO
 */
void on_pre(asock_loop_t* loop)
{

}

/**
 * Loop post-iteration handler
 *
 * @brief: TODO
 */
void on_post(asock_loop_t* loop)
{

}

/**
 * Socket writable handler
 *
 * @brief: TODO
 */
asock_socket_t* on_writable(asock_socket_t* socket)
{

  return socket;
}

/**
 * Socket closed handler
 *
 * @brief: TODO
 */
asock_socket_t* on_close(asock_socket_t* socket)
{

  return socket;
}

/**
 * Socket end handler
 *
 * @brief: TODO
 */
asock_socket_t* on_end(asock_socket_t* socket)
{

  return socket;
}

/**
 * Socket data recieved handler
 *
 * @brief: TODO
 */
asock_socket_t* on_data(asock_socket_t* socket)
{

  return socket;
}

/**
 * Socket open handler
 *
 * @brief: TODO
 */
asock_socket_t* on_open(asock_socket_t* socket,
    int is_client, char* ip, int ip_length)
{

  return socket;
}

/**
 * Socket timeout handler
 *
 * @brief: TODO
 */
asock_socket_t* on_timeout(asock_socket_t* socket)
{

  return socket;
}

/**
 * main
 *
 * @brief: TODO
 */
int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    printf("Usage: example_echo <port>\n");
    return 1;
  }

  return 0;
}
