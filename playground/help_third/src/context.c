#include "context.h"
#include "socket.h"
#include <stdlib.h>

/**
 * asock_context_create
 *
 */
asock_context_t *asock_context_create(int ssl, asock_loop_t *loop,
    int ext_size, asock_options_t options)
{
  asock_context_t *context = malloc(sizeof(asock_context_t) + ext_size);
  context->loop = loop;
  context->head = 0;
  context->iterator = 0;
  context->next = 0;
  context->ignore_data = asock_context_ignore_data_handler;

  asock_loop_link(loop, context);
  return context;
}

/**
 * asock_context_free
 *
 */
void asock_context_free(int ssl, asock_context_t *context)
{
  free(context);
}

/**
 * asock_context_link
 *
 */
void asock_context_link(asock_context_t *context, asock_socket_t *s)
{
  s->context = context;
  s->timeout = 0;
  s->next = context->head;
  s->prev = 0;
  if (context->head)
  {
    context->head->prev = s;
  }
  context->head = s;
}

/**
 * asock_context_unlink
 *
 */
void asock_context_unlink(asock_context_t *context, asock_socket_t *s)
{
  // We have to properly update the iterator used to sweep sockets for timeouts
  if (s == context->iterator)
  {
    context->iterator = s->next;
  }

  if (s->prev == s->next)
  {
    context->head = 0;
  }
  else
  {
    if (s->prev)
    {
      s->prev->next = s->next;
    }
    else
    {
      context->head = s->next;
    }

    if (s->next)
    {
      s->next->prev = s->prev;
    }
  }
}

/**
 * asock_context_loop
 *
 */
asock_loop_t *asock_context_loop(int ssl, asock_context_t *context)
{
  return context->loop;
}

/**
 * asock_context_ext
 *
 */
void *asock_context_ext(int ssl, asock_context_t *context)
{
  return context + 1;
}

/**
 * asock_context_on_end
 *
 */
void asock_context_on_end(int ssl, asock_context_t *context,
    asock_socket_t *(*on_end)(asock_socket_t *))
{
  context->on_end = on_end;
}

/**
 * asock_context_on_timeout
 *
 */
void asock_context_on_timeout(int ssl, asock_context_t *context,
    asock_socket_t *(*on_timeout)(asock_socket_t *))
{
  context->on_socket_timeout = on_timeout;
}

/**
 * asock_context_on_writable
 *
 */
void asock_context_on_writable(int ssl, asock_context_t *context,
    asock_socket_t *(*on_writable)(asock_socket_t *s))
{
  context->on_writable = on_writable;
}

/**
 * asock_context_on_data
 *
 */
void asock_context_on_data(int ssl, asock_context_t *context,
    asock_socket_t *(*on_data)(asock_socket_t *s, char *data, int length))
{
  context->on_data = on_data;
}

/**
 * asock_context_on_close
 *
 */
void asock_context_on_close(int ssl, asock_context_t *context,
    asock_socket_t *(*on_close)(asock_socket_t *s))
{
  context->on_close = on_close;
}

/**
 * asock_context_on_open
 *
 */
void asock_context_on_open(int ssl, asock_context_t *context,
    asock_socket_t *(*on_open)(asock_socket_t *s,
      int is_client, char *ip, int ip_length))
{
  context->on_open = on_open;
}

/**
 * asock_context
 *
 */
asock_context_t *asock_context(int ssl, asock_socket_t *s)
{
  return s->context;
}

/**
 * asock_context_ignore_data_handler
 *
 */
int asock_context_ignore_data_handler(asock_socket_t *s)
{
  return 0;
}
