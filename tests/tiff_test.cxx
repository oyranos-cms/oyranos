#include <tiffio.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
  printf("%s\n", TIFFGetVersion());
  return 0;
}
