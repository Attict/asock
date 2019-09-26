#ifndef A_TCP_SERVER_H
#define A_TCP_SERVER_H

/**
 * TODO:
 *
 * Allow LOGGER to be passed into the calls, so that logging can occur
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdint.h>

#ifdef A_TCP_SECURE
// Include ssl
#include <openssl/applink.c>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

#define TRUE 1
#define FALSE 0

#define A_TCP_SUCCESS 0
#define A_TCP_ERROR 1


///////////////////////////////////////////////////////////////////////////////
// Forward decorations
//
// TODO:
///////////////////////////////////////////////////////////////////////////////
typedef struct TCPServerInfo TCPServerInfo;
inline int _TCPServerDispose(TCPServerInfo *_tcpInfo);

///////////////////////////////////////////////////////////////////////////////
// TCP Server Info (Struct)
//
// TODO:
///////////////////////////////////////////////////////////////////////////////
typedef struct TCPServerInfo {
  int active;
  int opt;
  int master_socket;
  int addrlen, new_socket, activity, i, valread, sd;
  int max_sd;
  struct sockaddr_in address;
  fd_set readfds; // socket descriptors
  char buffer[1024];
  int client_socket[];
} TCPServerInfo;

///////////////////////////////////////////////////////////////////////////////
// TCP Server Config (Struct)
//
// TODO:
///////////////////////////////////////////////////////////////////////////////
typedef struct TCPServerConfig {
  int max_data_size;
  int max_connections;
  char *host;
  int port;
  char *welcome;
  int buffer_size;
  int keep_alive;
} TCPServerConfig;

///////////////////////////////////////////////////////////////////////////////
// TCP Server Handler (Callback)
//
// TODO:
///////////////////////////////////////////////////////////////////////////////
static void (*tcpServerHandler)(TCPServerInfo *info);

///////////////////////////////////////////////////////////////////////////////
// The info
///////////////////////////////////////////////////////////////////////////////
//static TCPServerInfo *_tcpInfo;

///////////////////////////////////////////////////////////////////////////////
// TCP Server Send (Send the message from param)
//
// TODO:
///////////////////////////////////////////////////////////////////////////////
static inline void TCPServerSend(TCPServerInfo *info, const char *msg)
{
  send(info->sd, msg, strlen(msg), 0);
}

///////////////////////////////////////////////////////////////////////////////
// TCP Server Send (Send the message from param)
//
// TODO:
///////////////////////////////////////////////////////////////////////////////
static inline char *TCPServerReceive(TCPServerInfo *info)
{
  return info->buffer;
}

///////////////////////////////////////////////////////////////////////////////
// Override Cntl + C FORCE EXIT, SIGNAL CLOSE
//
// Make sure memeory is freed and sockets are closed.
//
// TODO:
//  Don't close.
//  Use another command to force close.
//  Or at least set a variable to exit the loop.
///////////////////////////////////////////////////////////////////////////////
inline void _TCPServerSignalClose(int s)
{
  // Temp fix
  //printf("\nTerminating Server...\n");
  //_tcpInfo->active = FALSE;
  //if (_TCPServerDispose()) {
  //  printf("Server successfully closed!\n");
  //}
  //exit(0);
}

///////////////////////////////////////////////////////////////////////////////
// TCP Server Start
//
// TODO:
///////////////////////////////////////////////////////////////////////////////
inline void TCPServerInit(TCPServerConfig config)
{

}



///////////////////////////////////////////////////////////////////////////////
// TCP Server Run
//
// This is where everything is currently intiailized and what the user
// should primarily call/return in their `main` function.
///////////////////////////////////////////////////////////////////////////////
static inline int TCPServerRun(TCPServerConfig config)
{
  //struct sigaction sigIntHandler;
  //sigIntHandler.sa_handler = _TCPServerSignalClose;
  //sigemptyset(&sigIntHandler.sa_mask);
  //sigIntHandler.sa_flags = 0;
  //sigaction(SIGINT, &sigIntHandler, NULL);


  TCPServerInfo *_tcpInfo = (TCPServerInfo *)malloc(
      sizeof(TCPServerInfo) + sizeof(int) * config.max_connections);
  _tcpInfo->active = TRUE;

  int i;

  // Initialize all client_socket[] to 0 so not checked
  for (i = 0; i < config.max_connections; i++) {
    _tcpInfo->client_socket[i] = 0;
  }
  // Create a master socket
  if ((_tcpInfo->master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Set master socket to allow multiple connections,
  // this is just a good habit, it will work without this.
  if (setsockopt(_tcpInfo->master_socket, SOL_SOCKET, SO_REUSEADDR,
      (char *)&_tcpInfo->opt, sizeof(_tcpInfo->opt)) < 0) {
    perror("setsockopt failed");
    exit(EXIT_FAILURE);
  }

  // Type of socket created
  _tcpInfo->address.sin_family = AF_INET;
  _tcpInfo->address.sin_addr.s_addr = INADDR_ANY;
  _tcpInfo->address.sin_port = htons(config.port);

  // Bind the socket to localhost and port
  if (bind(_tcpInfo->master_socket,
        (struct sockaddr *)&_tcpInfo->address, sizeof(_tcpInfo->address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  printf("Server listening on %slocalhost:%d%s\n", "\033[92m",
      config.port, "\033[0m");

  // Try to specify maximum of 3 pending connections for the master socket
  if (listen(_tcpInfo->master_socket, 3) < 0) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }

  // Accept the incoming connection
  _tcpInfo->addrlen = sizeof(_tcpInfo->address);
  puts("Waiting for connections ...");

  while (_tcpInfo->active) {
    // Clear the socket set
    FD_ZERO(&_tcpInfo->readfds);

    // Add master socket to set
    FD_SET(_tcpInfo->master_socket, &_tcpInfo->readfds);
    _tcpInfo->max_sd = _tcpInfo->master_socket;

    // Add child sockets to set
    for (i = 0; i < config.max_connections; i++) {
      // Socket descriptor
      _tcpInfo->sd = _tcpInfo->client_socket[i];

      // If valid socket descriptor then add to read list
      if (_tcpInfo->sd > 0) {
        FD_SET(_tcpInfo->sd, &_tcpInfo->readfds);
      }

      // Highest file descriptor number, need it for the select function
      if (_tcpInfo->sd > _tcpInfo->max_sd) {
        _tcpInfo->max_sd = _tcpInfo->sd;
      }
    }

    // Wait for an activity on one of the sockets, timeout is NULL,
    // so wait indefinitely
    _tcpInfo->activity = select(_tcpInfo->max_sd + 1, &_tcpInfo->readfds, NULL, NULL, NULL);

    if ((_tcpInfo->activity < 0) && (errno != EINTR)) {
      printf("select failed");
    }

    // If something happened on the master socket,
    // then it's an incoming connection
    if (FD_ISSET(_tcpInfo->master_socket, &_tcpInfo->readfds)) {
      if ((_tcpInfo->new_socket = accept(_tcpInfo->master_socket,
          (struct sockaddr *)&_tcpInfo->address, (socklen_t *)&_tcpInfo->addrlen)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
      }

      // Inform user of socket number - used in send and receive commands
      printf("New connection (socketfd): %d, ip: %s, port: %d\n",
          _tcpInfo->new_socket, inet_ntoa(_tcpInfo->address.sin_addr), ntohs(_tcpInfo->address.sin_port));

      // Send new connection greeting message
      if (config.welcome != NULL) {
        if (send(_tcpInfo->new_socket, config.welcome, strlen(config.welcome), 0) != strlen(config.welcome)) {
          perror("send failed");
        }
        puts("Welcome message sent successfully!");
      }


      // Add new socket to array of sockets
      for (i = 0; i < config.max_connections; i++) {
        // If position is empty
        if (_tcpInfo->client_socket[i] == 0) {
          _tcpInfo->client_socket[i] = _tcpInfo->new_socket;
          printf("Adding to list of sockets as %d\n", i);

          break;
        }
      }
    }

    // Else it's some IO operation on some other socket
    for (i = 0; i < config.max_connections; i++) {
      _tcpInfo->sd = _tcpInfo->client_socket[i];

      if (FD_ISSET(_tcpInfo->sd, &_tcpInfo->readfds)) {
        // Check if it was for closing, and also read the incoming message
        if ((_tcpInfo->valread = read(_tcpInfo->sd, _tcpInfo->buffer, 1024)) == 0) {
          // Somebody disconnected, get his details and print
          getpeername(_tcpInfo->sd, (struct sockaddr *)&_tcpInfo->address, (socklen_t *)&_tcpInfo->addrlen);
          printf("Host disconnected... ip: %s, port: %d\n",
              inet_ntoa(_tcpInfo->address.sin_addr), ntohs(_tcpInfo->address.sin_port));

          // Close the socket and mark as 0 in list for resuse.
          close(_tcpInfo->sd);
          _tcpInfo->client_socket[i] = 0;
        } else {
          tcpServerHandler(_tcpInfo);
          memset(_tcpInfo->buffer, 0, sizeof _tcpInfo->buffer);
          if (!config.keep_alive) {
            shutdown(_tcpInfo->sd, SHUT_RDWR);
            close(_tcpInfo->sd);
            _tcpInfo->client_socket[i] = 0;
            // shutdown(client_fd, SHUT_RDWR)
            // close(client_fd)
            // clients[slot] = 0
          }
        }
      }
    }
  }

  if (_TCPServerDispose(_tcpInfo)) {
    printf("Server successfully closed!\n");
  }

  return 0;
}

extern int _TCPServerDispose(TCPServerInfo *_tcpInfo)
{
  close(_tcpInfo->master_socket);
  if (_tcpInfo) {
    free(_tcpInfo);
  }
  return TRUE;
}

#ifdef __cplusplus
}
#endif

#endif // A_TCP_SERVER_H

