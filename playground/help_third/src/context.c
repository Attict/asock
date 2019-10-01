#include "context.h"
#include "socket.h"
#include <stdlib.h>

/**
 * asock_context_create
 *
 */
asock_context_t *asock_context_create(int ssl, asock_loop_t *loop,
    int ext_size, asock_context_options_t options)
{
  asock_context_t *context = malloc(sizeof(asock_context_t) + ext_size);
  context->loop = loop;
  context->head = 0;
  context->iterator = 0;
  context->next = 0;
  context->ignore_data = 0;

  asock_loop_link(loop, context);
  return context;
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
