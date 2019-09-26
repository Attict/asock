#include "../src/asock.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * main
 *
 * @brief: TODO
 */
int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    printf("Usage: example_echo <port>\n");
    return 1;
  }

  asock_socket_context_t *my_context = asock_create_socket_context();

  return 0;
}
