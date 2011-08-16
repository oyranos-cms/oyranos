#include <yajl/yajl_version.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
  printf("libyajl %d.%d.%d\n", YAJL_MAJOR, YAJL_MINOR, YAJL_MICRO);
  return YAJL_VERSION;
}
