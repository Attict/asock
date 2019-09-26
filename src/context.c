#include "asock.h"

/**
 * asock_socket_context_unlink
 *
 * @brief: todo
 */
void asock_socket_context_unlink(asock_socket_context_t* context,
    asock_socket_t* s)
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
 * asock_socket_context_link
 *
 * @brief: todo
 */
void asock_socket_context_link(asock_socket_context_t* context,
    asock_socket_t* s)
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
 * asock_socket_context_loop
 *
 * @brief: todo
 */
asock_loop_t* asock_socket_context_loop(int ssl,
    asock_socket_context_t* context)
{
  return context->loop;
}
