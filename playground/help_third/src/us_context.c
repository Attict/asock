/*
 * Authored by Alex Hultman, 2018-2019.
 * Intellectual property of third-party.

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at

 *     http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "asock.h"
#include "us_internal.h"
#include "core.h"
#include <stdlib.h>

struct us_socket_context_t *us_create_socket_context(int ssl, struct us_loop_t *loop, int context_ext_size, struct us_socket_context_options_t options) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    return (struct us_socket_context_t *) us_internal_create_ssl_socket_context(loop, context_ext_size, options);
  }
#endif

  struct us_socket_context_t *context = malloc(sizeof(struct us_socket_context_t) + context_ext_size);
  context->loop = loop;
  context->head = 0;
  context->iterator = 0;
  context->next = 0;
  context->ignore_data = 0;

  us_internal_loop_link(loop, context);
  return context;
}

void us_socket_context_free(int ssl, struct us_socket_context_t *context) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    us_internal_ssl_socket_context_free((struct us_internal_ssl_socket_context_t *) context);
    return;
  }
#endif

  free(context);
}

struct us_listen_socket_t *us_socket_context_listen(int ssl, struct us_socket_context_t *context, const char *host, int port, int options, int socket_ext_size) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    return us_internal_ssl_socket_context_listen((struct us_internal_ssl_socket_context_t *) context, host, port, options, socket_ext_size);
  }
#endif

  int listen_socket_fd = asock_core_listen_socket(host, port, options);

  if (listen_socket_fd == -1) {
    return 0;
  }

  struct us_poll_t *p = asock_poll_create(context->loop, 0, sizeof(struct us_listen_socket_t));
  asock_poll_init(p, listen_socket_fd, POLL_TYPE_SEMI_SOCKET);
  asock_poll_start(p, context->loop, LIBUS_SOCKET_READABLE);

  struct us_listen_socket_t *ls = (struct us_listen_socket_t *) p;

  ls->s.context = context;
  ls->s.timeout = 0;
  ls->s.next = 0;
  asock_context_link(context, &ls->s);

  ls->socket_ext_size = socket_ext_size;

  return ls;
}

struct us_socket_t *us_socket_context_connect(int ssl, struct us_socket_context_t *context, const char *host, int port, int options, int socket_ext_size) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    return (struct us_socket_t *) us_internal_ssl_socket_context_connect((struct us_internal_ssl_socket_context_t *) context, host, port, options, socket_ext_size);
  }
#endif

  int connect_socket_fd = asock_core_connect_socket(host, port, options);
  if (connect_socket_fd == -1) {
    return 0;
  }

  /* Connect sockets are semi-sockets just like listen sockets */
  struct us_poll_t *p = asock_poll_create(context->loop, 0, sizeof(struct us_socket_t) + socket_ext_size);
  asock_poll_init(p, connect_socket_fd, POLL_TYPE_SEMI_SOCKET);
  asock_poll_start(p, context->loop, LIBUS_SOCKET_WRITABLE);

  struct us_socket_t *connect_socket = (struct us_socket_t *) p;

  /* Link it into context so that timeout fires properly */
  connect_socket->context = context;
  asock_context_link(context, connect_socket);

  return connect_socket;
}

struct us_socket_context_t *us_create_child_socket_context(int ssl, struct us_socket_context_t *context, int context_ext_size) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    return (struct us_socket_context_t *) us_internal_create_child_ssl_socket_context((struct us_internal_ssl_socket_context_t *) context, context_ext_size);
  }
#endif

  /* For TCP we simply create a new context as nothing is shared */
  struct us_socket_context_options_t options = {0};
  return us_create_socket_context(ssl, context->loop, context_ext_size, options);
}

/* Note: This will set timeout to 0 */
struct us_socket_t *us_socket_context_adopt_socket(int ssl, struct us_socket_context_t *context, struct us_socket_t *s, int ext_size) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    return (struct us_socket_t *) us_internal_ssl_socket_context_adopt_socket((struct us_internal_ssl_socket_context_t *) context, (struct us_internal_ssl_socket_t *) s, ext_size);
  }
#endif

  /* Cannot adopt a closed socket */
  if (asock_socket_is_closed(ssl, s)) {
    return s;
  }

  /* This properly updates the iterator if in on_timeout */
  asock_context_unlink(s->context, s);

  struct us_socket_t *new_s = (struct us_socket_t *) asock_poll_resize(&s->p, s->context->loop, sizeof(struct us_socket_t) + ext_size);

  asock_context_link(context, new_s);

  return new_s;
}

void us_socket_context_on_open(int ssl, struct us_socket_context_t *context, struct us_socket_t *(*on_open)(struct us_socket_t *s, int is_client, char *ip, int ip_length)) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    us_internal_ssl_socket_context_on_open((struct us_internal_ssl_socket_context_t *) context, (struct us_internal_ssl_socket_t * (*)(struct us_internal_ssl_socket_t *, int,  char *, int)) on_open);
    return;
  }
#endif

  context->on_open = on_open;
}

void us_socket_context_on_close(int ssl, struct us_socket_context_t *context, struct us_socket_t *(*on_close)(struct us_socket_t *s)) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    us_internal_ssl_socket_context_on_close((struct us_internal_ssl_socket_context_t *) context, (struct us_internal_ssl_socket_t * (*)(struct us_internal_ssl_socket_t *)) on_close);
    return;
  }
#endif

  context->on_close = on_close;
}

void us_socket_context_on_data(int ssl, struct us_socket_context_t *context, struct us_socket_t *(*on_data)(struct us_socket_t *s, char *data, int length)) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    us_internal_ssl_socket_context_on_data((struct us_internal_ssl_socket_context_t *) context, (struct us_internal_ssl_socket_t * (*)(struct us_internal_ssl_socket_t *, char *, int)) on_data);
    return;
  }
#endif

  context->on_data = on_data;
}

void us_socket_context_on_writable(int ssl, struct us_socket_context_t *context, struct us_socket_t *(*on_writable)(struct us_socket_t *s)) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    us_internal_ssl_socket_context_on_writable((struct us_internal_ssl_socket_context_t *) context, (struct us_internal_ssl_socket_t * (*)(struct us_internal_ssl_socket_t *)) on_writable);
    return;
  }
#endif

  context->on_writable = on_writable;
}

void us_socket_context_on_timeout(int ssl, struct us_socket_context_t *context, struct us_socket_t *(*on_timeout)(struct us_socket_t *)) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    us_internal_ssl_socket_context_on_timeout((struct us_internal_ssl_socket_context_t *) context, (struct us_internal_ssl_socket_t * (*)(struct us_internal_ssl_socket_t *)) on_timeout);
    return;
  }
#endif

  context->on_socket_timeout = on_timeout;
}

void us_socket_context_on_end(int ssl, struct us_socket_context_t *context, struct us_socket_t *(*on_end)(struct us_socket_t *)) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    us_internal_ssl_socket_context_on_end((struct us_internal_ssl_socket_context_t *) context, (struct us_internal_ssl_socket_t * (*)(struct us_internal_ssl_socket_t *)) on_end);
    return;
  }
#endif

  context->on_end = on_end;
}

void *us_socket_context_ext(int ssl, struct us_socket_context_t *context) {
#ifndef LIBUS_NO_SSL
  if (ssl) {
    return us_internal_ssl_socket_context_ext((struct us_internal_ssl_socket_context_t *) context);
  }
#endif

  return context + 1;
}
