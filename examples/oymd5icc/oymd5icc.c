/* cc -g -Wall `pkg-config oyranos --libs --cflags` oymd5icc.c -o oymd5icc */
#include <oyProfile_s.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main( int argc, char ** argv )
{
  oyProfile_s * p = 0;
  uint32_t * i;
  uint32_t md5[4],
           id[4];
  int profile_pos = 1,
      set_md5 = 0,
      error;

  if(argc == 1)
  {
    printf("Usage: %s [--set] icc_profile.icc\n", argv[0]);
    exit(0);
  }

  if(strcmp(argv[1],"--set") == 0)
  {
    set_md5 = 1;
    ++profile_pos;
  }

  p = oyProfile_FromFile( argv[profile_pos], 0,0 );
  error = oyProfile_GetMD5(p, OY_COMPUTE, md5);

  oyProfile_GetMD5(p, OY_FROM_PROFILE, id);

  if(set_md5)
    oyProfile_ToFile_( p, argv[profile_pos]);

  i = (uint32_t*)md5;
  if(p)
    printf( "%x%x%x%x[%x%x%x%x] %s\n",
            i[0],i[1],i[2],i[3], id[0],id[1],id[2],id[3], argv[profile_pos] );
  else
    fprintf(stderr, "not found: %s\n", argv[profile_pos] );

  return error;
}
