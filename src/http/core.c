#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Instead of returnin the request, perhaps pass in the memory allocation
// and set it.  Instead return u_int of some sort.
//
// Nginx is doing this.  It passes the request pointer, and the buffer pointer.
// Then sets the state: start, method, spaces before uri, etc.
// Then it goes over each position of the buffer?  And seems to check each
// char and determines continuing.  'G', 'E', 'T'... 'P', 'O', 'S', 'T'
// While checking the state for what to do.  Above is `state == method`
//
// State is keeping track of which char it is on.
// saces before uri -> if slash -> state = sw_after_slash_in_uri
//
// After URI state = http_H, http_HT, http_HTT, http_HTTP
// then first_major_digit, major_digit, first_minor_digit, minor_digit
//
// Then headers are turned to lowercase and read
//
// Hex -> Binary -> value
//      f -> 1111 -> 15
//      e -> 1110 -> 14
//      d -> 1101 -> 13
//      c -> 1100 -> 12
//      b -> 1011 -> 11
//      a -> 1010 -> 10
//      9 -> 1001 -> 9
//      8 -> 1000 -> 8
//      7 -> 0111 -> 7
//      6 -> 0110 -> 6
//      5 -> 0101 -> 5
//      4 -> 0100 -> 4
//      3 -> 0011 -> 3
//      2 -> 0010 -> 2
//      1 -> 0001 -> 1
//      0 -> 0000 -> 0
//
//      19 -> 0001 1111 -> 31
//      20 -> 0010 0000 -> 32
//      21 -> 0010 0001 -> 33
//
// char is actually an 8-bit integral value from 0-255, (similar to 00-FF)
//
// Use 0xFF for hex
// Use 0b00100000 for binary, but not supported for older cc
//
// FINAL:
// This method is going to iterate over each char in the data,
// and once it hits a space, it will see what the previous data was.
ahttp_request_t *ahttp_core_parse(char *data)
{
  ahttp_request_t *request = malloc(sizeof(ahttp_request_t));

  // 0 -> start
  // 1 -> method
  // 2 -> uri
  // 3 -> version
  int state = 0;

  // i for counting the data string, and where it started.
  int start = 0, i, len;
  int *method;

  for (i = 0; i < strlen(data); i++)
  {
    switch (state)
    {
      /**
       * METHOD
       */
      case 0:
        if (data[i] == ' ')
        {
          len = i - start;
          ahttp_core_parse_method(method, data, len);
          start = i;
          state++;
        }
        break;

      /**
       * URL
       */
      case 1:
        if (data[i] == ' ')
        {
          len = i - start;
          char *url = malloc(sizeof(char) * len);
          ahttp_core_parse_url(url, data, start, len);
          start = i;
          state++;
        }
        break;

      case 2:
        if (data[i]
    }
  }

  //printf("TEMP: %d\n", (1 << 14));
  //printf("Method: %d\n", request->method);

  free(request);

  return NULL;
}

/**
 * ahttp_core_parse_method
 *
 * @brief
 */
int ahttp_core_parse_method(int *method, const char *data, int len)
{
  unsigned char *m = malloc(sizeof(char) * len);
  strncpy((char *) m, data, len);

  switch (len)
  {
    case 3:
      if (ahttp_compare_str3(m, 'G', 'E', 'T'))
      {
        method = (int *) AHTTP_METHOD_GET;
      }
      else if (ahttp_compare_str3(m, 'P', 'U', 'T'))
      {
        method = (int *) AHTTP_METHOD_PUT;
      }
      break;

    case 4:
      if (m[1] == 'O')
      {
        if (ahttp_compare_str4(m, 'P', 'O', 'S', 'T'))
        {
          method = (int *) AHTTP_METHOD_POST;
        }
        else if (ahttp_compare_str4(m, 'C', 'O', 'P', 'Y'))
        {
          method = (int *) AHTTP_METHOD_COPY;
        }
        else if (ahttp_compare_str4(m, 'M', 'O', 'V', 'E'))
        {
          method = (int *) AHTTP_METHOD_MOVE;
        }
        else if (ahttp_compare_str4(m, 'L', 'O', 'C', 'K'))
        {
          method = (int *) AHTTP_METHOD_LOCK;
        }
      }
      else
      {
        if (ahttp_compare_str4(m, 'H', 'E', 'A', 'D'))
        {
          method = (int *) AHTTP_METHOD_HEAD;
        }
      }
      break;

    case 5:
      if (ahttp_compare_str5(m, 'P', 'A', 'T', 'C', 'H'))
      {
        method = (int *) AHTTP_METHOD_PATCH;
      }
      else if (ahttp_compare_str5(m, 'M', 'K', 'C', 'O', 'L'))
      {
        method = (int *) AHTTP_METHOD_MKCOL;
      }
      else if (ahttp_compare_str5(m, 'T', 'R', 'A', 'C', 'E'))
      {
        method = (int *) AHTTP_METHOD_TRACE;
      }
      break;
  }
  return 0;
}

/**
 * ahttp_core_parse_url
 *
 * @brief
 */
int ahttp_core_parse_url(char *url, const char *data, int start, int len)
{
  strncpy(url, data + start, len);
  printf("Url: %s\n", url);
  return 0;
}

/**
 * ahttp_core_parse_version
 *
 * @brief
 */
int ahttp_core_parse_version(int *major, int *minor, const char *data, int len)
{

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
