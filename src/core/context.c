#include "context.h"
#include "core.h"
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

/**
 * asock_context_listen
 *
 */
asock_core_listen_t *asock_context_listen(int ssl, asock_context_t *context,
    const char *host, int port, int options, int ext_size)
{
  int ls_fd = asock_core_listen_socket(host, port, options);

  if (ls_fd == -1)
  {
    return 0;
  }

  asock_poll_t *p = asock_poll_create(
      context->loop, 0, sizeof(asock_core_listen_t));
  asock_poll_init(p, ls_fd, ASOCK_POLL_TYPE_SEMI);
  asock_poll_start(p, context->loop, ASOCK_SOCKET_READABLE);

  asock_core_listen_t *ls = (asock_core_listen_t *) p;

  ls->s.context = context;
  ls->s.timeout = 0;
  ls->s.next = 0;
  asock_context_link(context, &ls->s);

  ls->socket_ext_size = ext_size;

  return ls;
}

/**
 * asock_context_connect
 *
 * @note cs_fd = Connect Socket File Descriptor
 */
asock_socket_t *asock_context_connect(int ssl, asock_context_t *context,
    const char *host, int port, int options, int ext_size)
{
  int cs_fd = asock_core_connect_socket(host, port, options);
  if (cs_fd == -1)
  {
    return 0;
  }

  // Connect sockets are semi-sockets just like listen sockets
  asock_poll_t *p = asock_poll_create(
      context->loop, 0, sizeof(asock_socket_t) + ext_size);
  asock_poll_init(p, cs_fd, ASOCK_POLL_TYPE_SEMI);
  asock_poll_start(p, context->loop, ASOCK_SOCKET_WRITABLE);

  // Connect Socket
  asock_socket_t *cs = (asock_socket_t *) p;

  // Link it into context so that timeout fires properly
  cs->context = context;
  asock_context_link(context, cs);

  return cs;
}

/**
 * asock_context_create_child
 *
 */
asock_context_t *asock_context_create_child(int ssl, asock_context_t *context,
    int ext_size)
{
  asock_options_t options = {0};
  return asock_context_create(ssl, context->loop, ext_size, options);
}

/**
 * asock_context_adopt_socket
 *
 * @note This will set timeout to 0.
 */
asock_socket_t *asock_context_adopt_socket(int ssl, asock_context_t *context,
    asock_socket_t *s, int ext_size)
{
  // Cannot adopt closed socket
  if (asock_socket_is_closed(ssl, s))
  {
    return s;
  }

  // This properly update the iterator if in on_timeout
  asock_context_unlink(s->context, s);
  asock_socket_t *new_s = (asock_socket_t *) asock_poll_resize(
      &s->p, s->context->loop, sizeof(asock_socket_t) + ext_size);

  asock_context_link(context, new_s);
  return new_s;
}
