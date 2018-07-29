/*  @file openicc_config_read.c
 *
 *  libOpenICC - OpenICC Colour Management Configuration
 *
 *  @par Copyright:
 *            2011-2017 (C) Kai-Uwe Behrmann
 *
 *  @brief    OpenICC Colour Management configuration helpers
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2011/06/27
 */

/**
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>           /* setlocale LC_NUMERIC */

#include "openicc_config.h"
#include "openicc_config_internal.h"

int main(int argc, char ** argv)
{
  openiccConfig_s * config, * db;
  const char * file_name = argc > 1 ? argv[1] : "../test.json";
  char * text = 0;
  int size = 0;
  char            ** keys = 0;
  char            ** values = 0;
  int i,j, n = 0, devices_n, flags;
  char * json, * device_class;
  const char * devices_filter[] = {OPENICC_DEVICE_CAMERA,NULL},
             * old_device_class = NULL,
             * d = NULL;

  setlocale(LC_ALL,"");
  openiccInit();


  /* read JSON input file */
  text = openiccOpenFile( file_name, &size );
  if(!text)
  {
    fprintf( stderr, "Usage: %s openicc.json\n\n", argv[0] );
    return 0;
  }
 

  /* parse JSON */
  db = openiccConfig_FromMem( text );
  openiccConfig_SetInfo ( db, file_name );
  devices_n = openiccConfig_DevicesCount(db, NULL);
  fprintf(stderr, "Found %d devices.\n", devices_n );

  
  /* print all found key/value pairs */
  for(i = 0; i < devices_n; ++i)
  {
    const char * d = openiccConfig_DeviceGet( db, NULL, i,
                                              &keys, &values, malloc,free );

    if(i)
      fprintf( stderr,"\n");

    n = 0; if(keys) while(keys[n]) ++n;
    fprintf( stderr, "[%d] device class:\"%s\" with %d keys/values pairs\n", i, d, n);
    for( j = 0; j < n; ++j )
    {
      fprintf(stderr, "%s:\"%s\"\n", keys[j], values[j]);
      free(keys[j]);
      free(values[j]);
    }
    free(keys); free(values);
  }

  /* get a single JSON device */
  i = 1; /* select the second one, we start counting from zero */
  d = openiccConfig_DeviceGetJSON ( db, NULL, i, 0,
                                    old_device_class, &json, malloc,free );
  config = openiccConfig_FromMem( json );
  device_class = openiccConfig_DeviceClassGet( config, malloc );
  openiccConfig_Release( &config );
  fprintf( stderr, "\ndevice class[%d]: \"%s\"\n", i, device_class);
  printf( "%s\n", json );
  free(json);


  /* we want a single device class DB for lets say cameras */
  devices_n = openiccConfig_DevicesCount( db, devices_filter );
  fprintf(stderr, "Found %d %s devices.\n", devices_n, devices_filter[0] );
  old_device_class = NULL;
  for(i = 0; i < devices_n; ++i)
  {
    flags = 0;
    if(i != 0) /* not the first */
      flags |= OPENICC_CONFIGS_SKIP_HEADER;
    if(i != devices_n - 1) /* not the last */
      flags |= OPENICC_CONFIGS_SKIP_FOOTER;

    d = openiccConfig_DeviceGetJSON( db, devices_filter, i, flags,
                                     old_device_class, &json, malloc,free );
    old_device_class = d;
    printf( "%s\n", json );
    free(json);
  }

  openiccConfig_Release( &db );

  return 0;
}

