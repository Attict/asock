#include <stdio.h>
int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    printf("Usage: asock_watcher <watchdir> <execute>");
    return 1;
  }
  return 0;
}
