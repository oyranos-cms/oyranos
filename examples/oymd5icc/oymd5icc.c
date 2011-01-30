/* cc -g -Wall `pkg-config oyranos --libs --cflags` oymd5icc.c -o oymd5icc */
#include <oyranos_alpha.h>
#include <stdio.h>
#include <stdlib.h>



int main( int argc, char ** argv )
{
  oyProfile_s * p = 0;
  uint32_t * i;
  uint32_t md5[4];

  if(argc == 1)
  {
    printf("Usage: %s icc_profile.icc\n", argv[0]);
    exit(0);
  }

  p = oyProfile_FromFile( argv[1], 0,0 );
  oyProfile_GetMD5(p, md5);
  i = (uint32_t*)md5;
  printf( "profile hash[based on md5]: %x%x%x%x\n", i[0], i[1], i[2], i[3] );

  return 0;
}
