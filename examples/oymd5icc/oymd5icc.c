/* cc -g -Wall `pkg-config oyranos --libs --cflags` oymd5icc.c -o oymd5icc */
#include <oyranos_alpha.h>
#include <stdio.h>
#include <stdlib.h>



int main( int argc, char ** argv )
{
  oyProfile_s * p = 0;
  uint32_t * i;

  if(argc == 1)
  {
    printf("Usage: %s icc_profile.icc\n", argv[0]);
    exit(0);
  }

  p = oyProfile_FromFile( argv[1], 0,0 );
  oyProfileGetMD5( p->block_, p->size_, p->oy_->hash_ptr_ );
  i = (uint32_t*)p->oy_->hash_ptr_;
  printf( "profile hash[based on md5]: %x%x%x%x\n", i[0], i[1], i[2], i[3] );

  return 0;
}
