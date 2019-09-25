#include "../src/asock.h"
#include <stdio.h>
#include <stdlib.h>

const int SSL = 0;

/**
 * Extension of socket
 *
 * @brief: TODO
 */
typedef struct echo_socket
{
  char *backpressure;
  int length;
}
echo_socket;

/**
 * Extension of socket context
 *
 * @brief: TODO
 */
typedef struct echo_context
{

}
echo_context;

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
asock_socket_t* on_writable(asock_socket_t* s)
{
  echo_socket* es = (echo_socket*) asock_socket_ext(SSL, s);

  // Continue writing on our backpressure
  int written = asock_socket_write(SSL, s, es->backpressure, es->length, 0);
  if (written != es->length)
  {
    char* new_buffer = (char*) malloc(es->length - written);
    memcpy(new_buffer, es->backpressure, es->length - written);
    free(es->backpressure);
    es->backpressure = new_buffer;
    es->length -= written;
  }
  else
  {
    free(es->backpressure);
    es->length = 0;
  }

  asock_socket_timeout(SSL, s, 30);

  return s;
}

/**
 * Socket closed handler
 *
 * @brief: TODO
 */
asock_socket_t* on_close(asock_socket_t* s)
{
  echo_socket* es = (echo_socket*) asock_socket_ext(SSL, s);

  printf("Client disconnected\n");

  free(es->backpressure);

  return s;
}

/**
 * Socket end handler
 *
 * @brief: TODO
 */
asock_socket_t* on_end(asock_socket_t* s)
{
  asock_socket_shutdown(SSL, s);
  return asock_socket_close(SSL, s);
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
  // The event loop
  asock_loop_t* loop = asock_create_loop(0, on_wakeup, on_pre, on_post, 0);

  if (argc != 2)
  {
    printf("Usage: example_echo <port>\n");
    return 1;
  }

  return 0;
}
