#include <stdio.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    printf("Usage: asock_watcher <watchdir> <execute>\n");
    return 1;
  }

  const char *dir = argv[1];
  const char *exe = argv[2];

  printf("Watching '%s' to run '%s'\n", dir, exe);



  return 0;
}
