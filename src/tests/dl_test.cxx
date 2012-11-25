#include <dlfcn.h>

int main(void)
{
  dlopen("/lib/libm.so", RTLD_LAZY);
  return 0;
}
