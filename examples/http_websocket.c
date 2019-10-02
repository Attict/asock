#include "../src/asock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

typedef struct http_socket_t
{
  // How far we have streamed our response
  int offset;
}
http_socket_t;

typedef struct http_context_t
{
  // The shared response
  char *response;
  int length;
}
http_context_t;


void on_wakeup(asock_loop_t *loop) {}
void on_pre(asock_loop_t *loop) {}
void on_post(asock_loop_t *loop) {}

asock_socket_t *on_open(asock_socket_t *s,
    int is_client, char *ip, int ip_length)
{
  http_socket_t *socket = (http_socket_t *) asock_socket_ext(0, s);
  // Reset offset
  socket->offset = 0;
  // Timeout idle http connections
  asock_socket_timeout(0, s, 30);

  printf("Client connected\n");

  return s;
}

asock_socket_t *on_close(asock_socket_t *s)
{
  printf("Client disconnected\n");
  return s;
}

asock_socket_t *on_data(asock_socket_t *s, char *data, int length)
{
  http_socket_t *socket = (http_socket_t *) asock_socket_ext(0, s);
  http_context_t *context =
      (http_context_t *) asock_context_ext(0, asock_context(0, s));
  socket->offset =
      asock_socket_write(0, s, context->response, context->length, 0);
  asock_socket_timeout(0, s, 30);
  printf("Recieved data: %s\n", data);


  //
  // Regex to get the Sec-WebSocket-Key
  //
  regex_t regex;
  int reti;
  char msgbuf[100];

  reti = regcomp(&regex, "", 0);


  return s;
}

asock_socket_t *on_writable(asock_socket_t *s)
{
  http_socket_t *socket = (http_socket_t *) asock_socket_ext(0, s);
  http_context_t *context =
      (http_context_t *) asock_context_ext(0, asock_context(0, s));
  socket->offset += asock_socket_write(0, s,
      context->response + socket->offset, context->length - socket->offset, 0);
  return s;
}


asock_socket_t *on_end(asock_socket_t *s)
{
  asock_socket_shutdown(0, s);
  return asock_socket_close(0, s);
}

asock_socket_t *on_timeout(asock_socket_t *s)
{
  return asock_socket_close(0, s);
}

int main()
{
  asock_loop_t *loop = asock_loop_create(0, on_wakeup, on_pre, on_post, 0);
  asock_options_t options = {};
  asock_context_t *context =
      asock_context_create(0, loop, sizeof(http_context_t), options);

  // Generate the shared response
  const char body[] = "<html><body><h1>Hello, world!</h1></body></html>";

  http_context_t *http_context =
      (http_context_t *) asock_context_ext(0, context);
  http_context->response = (char *) malloc(128 + sizeof(body) - 1);
  http_context->length = snprintf(
      http_context->response, 128 + sizeof(body) - 1,
      "HTTP/1.1 101 Switching Protocols\r\n"
      "Upgrade: websocket\r\n"
      "Connection: Upgrade\r\n"
      "Sec-WebSocket-Accept: \r\n"
      "Content-Length: %ld\r\n\r\n%s",
      sizeof(body) - 1, body);

  // Set up event handlers
  asock_context_on_open(0, context, on_open);
  asock_context_on_data(0, context, on_data);
  asock_context_on_writable(0, context, on_writable);
  asock_context_on_close(0, context, on_close);
  asock_context_on_timeout(0, context, on_timeout);
  asock_context_on_end(0, context, on_end);

  asock_core_listen_t *ls = asock_context_listen(0, context, 0, 3000, 0, sizeof(http_socket_t));

  if (ls)
  {
    printf("Listening on port 3000...\n");
    asock_loop_run(loop);
  }
  else
  {
    printf("Failed to listen!\n");
  }

  return 0;
}
