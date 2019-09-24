///
/// asock.h
/// ~~~~~~~
///
/// Copyright (c) 2019 Eric Wagner <eric@attict.net>
///
/// This is the public declaration.
///
#ifndef ASOCK_H
#define ASOCK_H

/**
 * Library types publicly available
 *
 * @brief: TODO
 */
typedef struct asock_loop_t asock_loop_t;

/**
 * asock_socket_context_t
 *
 * @brief TODO
 */
typedef struct asock_socket_context_t
{
  struct asock_socket_t* head;
  struct asock_socket_t* iterator;
  struct asock_socket_t* next;
  struct asock_socket_t* prev;
}
asock_socket_context_t;

/**
 * asock__socket_t
 *
 * @brief TODO
 */
typedef struct asock_socket_t
{
  struct asock_socket_context_t* context;
  struct asock_socket_t* prev;
  struct asock_socket_t* next;
  unsigned short timeout;
}
asock_socket_t;

/**
 * asock_listen_socket_t
 *
 * @brief TODO
 */
typedef struct asock_listen_socket_t
{
  struct asock_socket_t socket;
  unsigned int socket_ext_size;
}
asock_listen_socket_t;

/**
 * Forward Declaration:
 *
 * Creates a loop.
 *
 * @brief: TODO
 */
asock_loop_t* asock_create_loop(int a, int b, int c, int d, int e);

#endif // ASOCK_H
