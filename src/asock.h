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

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdalign.h>
#include <stdio.h>
#include <string.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/**
 * SETSOCKOPT_PTR_TYPE
 *
 * @brief: todo
 */
#define SETSOCKOPT_PTR_TYPE int*

/**
 * ASOCK_SOCKET_ERROR
 *
 * @brief: todo
 */
#define ASOCK_SOCKET_ERROR -1

/**
 * ASOCK_SOCKET_DESCRIPTOR
 *
 * @brief: Socket descriptor type, different for windows. "SOCKET" type
 */
#define ASOCK_SOCKET_DESCRIPTOR int

/**
 * ASOCK_EXT_ALIGNMENT
 *
 * @brief: Guaranteed alignment of extension memory.
 */
#define ASOCK_EXT_ALIGNMENT 16

/// ----------------------------------------------------------------------------
/// TYPEDEF DECLARATIONS
///
/// @brief: todo
///
typedef struct asock_async asock_async;
typedef struct asock_listen_socket_t asock_listen_socket_t;
typedef struct asock_loop_t asock_loop_t;
typedef struct asock_loop_data_t asock_loop_data_t;
typedef struct asock_poll_t asock_poll_t;
typedef struct asock_socket_t asock_socket_t;
typedef struct asock_socket_context_t asock_socket_context_t;
typedef struct asock_timer_t asock_timer_t;

/// ----------------------------------------------------------------------------
/// FUNCTION DECLARATIONS
///
/// @brief: todo
///
asock_loop_t* asock_create_loop(void* hint,
  void (*wakup_cb)(asock_loop_t *loop), void (*pre_cb)(asock_loop_t *loop),
  void (*post_cb)(asock_loop_t *loop), unsigned int ext_size);

/// ----------------------------------------------------------------------------
/// STRUCTS
///
/// @brief: todo
///

/**
 * asock_async
 *
 * @brief: todo
 */
struct asock_async
{

};

/**
 * asock_listen_socket_t
 *
 * @brief: TODO
 */
struct asock_listen_socket_t
{
  asock_socket_t        socket;
  unsigned int          socket_ext_size;
};


/**
 * asock_loop_t
 *
 * @brief: TODO
 */
struct asock_loop_t
{
  alignas(ASOCK_EXT_ALIGNMENT) asock_loop_data_t data;

  // Number of non-fallthrough polls in the loop
  int num_polls;

  // Number of ready polls this iteration
  int num_ready_polls;

  // Current index in list of ready polls
  int current_ready_poll;

  // Loop's own file descriptor
  int fd;

  //The list of ready polls
#ifdef ASOCK_USE_EPOLL

#else

#endif


};

/**
 * asock_loop_data_t
 *
 * @brief: TODO
 */
struct asock_loop_data_t
{
  asock_timer_t*                sweep_timer;
  asock_async*                  wakeup_async;
  int                           last_write_failed;
  asock_socket_context_t*       head;
  asock_socket_context_t*       iterator;
  char*                         recv_buffer;
  void*                         ssl_data;
  void                          (*pre_cb)(asock_loop_t*);
  void                          (*post_cb)(asock_loop_t*);
  asock_socket_t*               closed_head;
  // We do not care if this flips or not, it doesn't matter
  long long                     iteration_nr;
};

/**
 * asock_poll_t
 *
 * @brief: todo
 */
struct asock_poll_t
{
  alignas(ASOCK_EXT_ALIGNMENT) struct
  {
    int fd : 28;
    unsigned int poll_type : 4;
  }
  state;
};

/**
 * asock_socket_t
 *
 * @brief: TODO
 */
struct asock_socket_t
{
  asock_socket_context_t* context;
  asock_socket_t* prev;
  asock_socket_t* next;
  unsigned short timeout;
};

/**
 * asock_socket_context_t
 *
 * @brief: TODO
 */
struct asock_socket_context_t
{
  asock_socket_t* head;
  asock_socket_t* iterator;
  asock_socket_t* next;
  asock_socket_t* prev;
};

/**
 * asock_timer_t
 *
 * @brief: todo
 */
struct asock_timer_t
{

};

#endif // ASOCK_H
