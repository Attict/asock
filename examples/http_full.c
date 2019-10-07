#include "../src/asock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AHTTP_MAX_HEADERS 50

/**
 * ahttp_socket_t
 *
 * @brief
 */
typedef struct ahttp_socket_t
{
  int offset;
}
ahttp_socket_t;

/**
 * ahttp_context_t
 *
 * @brief
 */
typedef struct ahttp_context_t
{
  char *response;
  int length;
}
ahttp_context_t;

/**
 * ahttp_header_t
 *
 * @brief
 */
typedef struct ahttp_header_t
{
  char* name;
  char* value;
}
ahttp_header_t;

/**
 * ahttp_request_t
 *
 * @brief
 */
typedef struct ahttp_request_t
{
  ahttp_header_t headers[AHTTP_MAX_HEADERS];
  char *version;
  char *method;
  char *uri;
  char *body;
}
ahttp_request_t;

/**
 * ahttp_response_t
 *
 * @brief
 */
typedef struct ahttp_response_t
{
  ahttp_header_t headers[AHTTP_MAX_HEADERS];
}
ahttp_response_t;

/**
 * AHTTP_STATUS
 *
 * @brief
 */
enum
{
  AHTTP_STATUS_OK = 200,
  AHTTP_STATUS_CREATED = 201,
  AHTTP_STATUS_ACCEPTED = 202,
  AHTTP_STATUS_NO_CONTENT = 204,
  AHTTP_STATUS_MULTIPLE_CHOICES = 300,
  AHTTP_STATUS_MOVED_PERMANENTLY = 301,
  AHTTP_STATUS_MOVED_TEMPORARILY = 302,
  AHTTP_STATUS_NOT_MODIFIED = 304,
  AHTTP_STATUS_BAD_REQUEST = 400,
  AHTTP_STATUS_UNAUTHORIZED = 401,
  AHTTP_STATUS_FORBIDDEN = 403,
  AHTTP_STATUS_NOT_FOUND = 404,
  AHTTP_STATUS_INTERNAL_SERVER_ERROR = 500,
  AHTTP_STATUS_NOT_IMPLEMENTED = 501,
  AHTTP_STATUS_BAD_GATEWAY = 502,
  AHTTP_STATUS_SERVICE_UNAVAILABLE = 503
};


ahttp_request_t *parseRequest(char *data)
{
  //ahttp_request_t *request = malloc(sizeof(ahttp_request_t));
  //char *line = strtok(data, "\r\n");
  //while (line != NULL)
  //{
  //  printf("Line: %s\n", line);
  //  line = strtok(NULL, "\r\n");
  //}

  // Parse first line as METHOD, URI, VERSION
  // Parse next lines as HEADER -> KEY, VALUE
  // Parse body after \r\n\r\n

  const char *body = strstr(data, "\r\n\r\n");
  printf("data: %s\n", data);
  printf("Test: %s\n", body);

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
  parseRequest(data);

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

int main(int argc, char *argv[])
{
  return ahttp_run(4000);
}
