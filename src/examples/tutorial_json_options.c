#include "oyOptions_s.h"
#include <stdio.h>
int main (int argc OY_UNUSED, char ** argv OY_UNUSED)
{
  int error = 0;
  oyOptions_s * result = 0;
  const char * t = NULL,
             * json;
  FILE * zout = stdout;

  json = "{\"org\":{\"free\":[{\"s1key_a\":\"val_a\",\"s1key_b\":\"val_b\"},{\"s2key_c\":\"val_c\",\"s2key_d\":\"val_d\"}],\"key_e\":\"val_e\"}}";
  oyOptions_s * options = NULL;
  error = oyOptions_FromJSON( json, options, &result, "org" );
  t = oyOptions_GetText( result, oyNAME_NICK );
  fprintf( zout, "%s\n", t?t:0 );

  json = "{\"org\":{\"free\":[{\"s1key_a\":\"val_a\",\"s1key_b\":\"val_b\"},{\"s2key_c\":\"val_c\",\"s2key_d\":\"val_d\"}],\"key_e\":\"val_e_xxx\"}}";
  error = oyOptions_FromJSON( json, options, &result, "org" );
  t = oyOptions_GetText( result, oyNAME_NICK );
  fprintf( zout, "%s\n", t?t:0 );

  json = "{\"org\":{\"free\":[{\"s1key_a\":\"val_a\",\"s1key_b\":\"val_b\"},{\"s2key_c\":\"val_c\",\"s2key_d\":\"val_d\"}],\"key_e\":\"val_e_yyy\",\"key_f\":\"val_f\"}}";
  error = oyOptions_FromJSON( json, options, &result, "org" );
  t = oyOptions_GetText( result, oyNAME_NICK );
  fprintf( zout, "%s\n", t?t:0 );

  oyOptions_SetFromText( &options, OY_STD "/key_path", 
                                   "net/host/path", OY_CREATE_NEW);
  json = "{\"org\":{\"free\":[{\"s1key_a\":\"val_a\",\"s1key_b\":\"val_b\"},{\"s2key_c\":\"val_c\",\"s2key_d\":\"val_d\"}],\"key_e\":\"val_e_yyy\",\"key_f\":\"val_f\"}}";
  error = oyOptions_FromJSON( json, options, &result, "org/free/[%d]", 1 );
  t = oyOptions_GetText( result, oyNAME_NICK );
  fprintf( zout, "%s\n", t?t:0 );

  if(error)
    fprintf( zout, "oyOptions_FromJSON() returned %d\n", error );

  oyOptions_Release( &options );

  oyOptions_Release( &result );

  return 0;
}
