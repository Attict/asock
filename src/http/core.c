#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ahttp_request_t *ahttp_core_parse(char *data)
{
  ahttp_request_t *request = malloc(sizeof(ahttp_request_t));
  char *line = strtok(data, "\r\n");

  while (line != NULL)
  {
    printf("Line: %s\n", line);
    line = strtok(NULL, "\r\n");
  }

  // Parse first line as METHOD, URI, VERSION
  // Parse next lines as HEADER -> KEY, VALUE
  // Parse body after \r\n\r\n

  //const char *body = strstr(data, "\r\n\r\n");
  //printf("data: %s\n", data);
  //printf("Test: %s\n", body);

  return NULL;
}

// -----------------------------------------------------------------------------
// ahttp_core?
//

/**
 * ahttp_core_on_wakeup
 *
 * @brief
 */
void ahttp_core_on_wakeup(asock_loop_t *loop) {}

/**
 * ahttp_core_on_pre
 *
 * @brief
 */
void ahttp_core_on_pre(asock_loop_t *loop) {}

/**
 * ahttp_core_on_post
 *
 * @brief
 */
void ahttp_core_on_post(asock_loop_t *loop) {}

/**
 * ahttp_core_on_open
 *
 * @brief
 */
asock_socket_t *ahttp_core_on_open(asock_socket_t *s, int is_client,
    char *ip, int ip_length)
{
  ahttp_socket_t *socket = (ahttp_socket_t *) asock_socket_ext(0, s);
  // Reset offset
  socket->offset = 0;
  // Timeout idle http connections
  asock_socket_timeout(0, s, 30);
  return s;
}

asock_socket_t *ahttp_core_on_close(asock_socket_t *s)
{
  return s;
}

asock_socket_t *ahttp_core_on_data(asock_socket_t *s, char *data, int length)
{
  ahttp_socket_t *socket = (ahttp_socket_t *) asock_socket_ext(0, s);
  ahttp_context_t *context =
      (ahttp_context_t *) asock_context_ext(0, asock_context(0, s));

  // Parse data into `ahttp_request_t`
  ahttp_core_parse(data);

  const char body[] = "<html><body><h1>Something completely new!</h1></body></html>";
  context->response =
      (char *) realloc(context->response, 128 + sizeof(body) - 1);
  context->length = snprintf(
      context->response, 128 + sizeof(body) - 1,
      "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n%s",
      sizeof(body) - 1, body);

  socket->offset =
      asock_socket_write(0, s, context->response, context->length, 0);
  asock_socket_timeout(0, s, 30);
  return s;
}

/**
 * ahttp_core_on_writable
 *
 * @brief
 */
asock_socket_t *ahttp_core_on_writable(asock_socket_t *s)
{
  // asock_socket_ext places the memory in the next address after `s`
  ahttp_socket_t *socket = (ahttp_socket_t *) asock_socket_ext(0, s);
  ahttp_context_t *context =
      (ahttp_context_t *) asock_context_ext(0, asock_context(0, s));
  socket->offset += asock_socket_write(0, s,
      context->response + socket->offset, context->length - socket->offset, 0);
  return s;
}

/**
 * ahttp_core_on_end
 *
 * @brief
 */
asock_socket_t *ahttp_core_on_end(asock_socket_t *s)
{
  asock_socket_shutdown(0, s);
  return asock_socket_close(0, s);
}

/**
 * ahttp_core_on_timeout
 *
 * @brief
 */
asock_socket_t *ahttp_core_on_timeout(asock_socket_t *s)
{
  return asock_socket_close(0, s);
}

int ahttp_run(int port)
{
  asock_loop_t *loop = asock_loop_create(0, ahttp_core_on_wakeup,
      ahttp_core_on_pre, ahttp_core_on_post, 0);
  asock_options_t options = {};
  asock_context_t *context =
      asock_context_create(0, loop, sizeof(ahttp_context_t), options);

  const char body[] = "<html><body><h1>Hello, world!</h1></body></html>";

  ahttp_context_t *http_context =
      (ahttp_context_t *) asock_context_ext(0, context);
  http_context->response = (char *) malloc(128 + sizeof(body) - 1);
  http_context->length = snprintf(
      http_context->response, 128 + sizeof(body) - 1,
      "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\n\r\n%s",
      sizeof(body) - 1, body);

  // Set up event handlers
  asock_context_on_open(0, context, ahttp_core_on_open);
  asock_context_on_close(0, context, ahttp_core_on_close);
  asock_context_on_data(0, context, ahttp_core_on_data);
  asock_context_on_writable(0, context, ahttp_core_on_writable);
  asock_context_on_end(0, context, ahttp_core_on_end);
  asock_context_on_end(0, context, ahttp_core_on_timeout);

  asock_core_listen_t *ls =
      asock_context_listen(0, context, 0, port, 0, sizeof(ahttp_socket_t));

  if (ls)
  {
    printf("Listening on port %d...\n", port);
    asock_loop_run(loop);
  }
  else
  {
    printf("Failed to listen on port %d!\n", port);
  }

  return 0;
}
