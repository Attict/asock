#include "tcp.h"
#include <stdio.h>

void MyTCPServerHandler(TCPServerInfo *info)
{
  const char *response = TCPServerReceive(info);
  TCPServerSend(info, response);
}

int main()
{
  TCPServerConfig config = {};
  config.max_connections = 10000;
  config.host = "localhost";
  config.port = 7000;
  config.welcome = "ECHO Daemon v1.0\n";
  config.keep_alive = TRUE;
  tcpServerHandler = &MyTCPServerHandler;
  return TCPServerRun(config);
}
