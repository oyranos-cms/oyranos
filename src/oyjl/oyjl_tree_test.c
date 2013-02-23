#include "oyjl_tree.h"
#include <stdarg.h>

yajl_status    message_func     ( oyjl_message_e      code,
                                       const void        * context,
                                       const char        * format,
                                       ... )
{
  char * text = 0;
  va_list list;
  int i,len;
  size_t sz = 256;

  text = calloc( sizeof(char), sz );
  if(!text)
  {
    fprintf(stderr,
    "oyjl_tree_parse.c:79 oyjl_message_func() Could not allocate 256 byte of memory.\n");
    return yajl_status_error;
  }

  text[0] = 0;
  va_start( list, format);
  len = vsnprintf( text, sz-1, format, list);
  va_end  ( list );

  if (len >= (sz - 1))
  {
    text = realloc( text, (len+1)*sizeof(char) );
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list);
    va_end  ( list );
  }

  switch(code)
  {
    case oyjl_message_info:
         fprintf( stderr, "INFO ");
         break;
    case oyjl_message_client_canceled:
         fprintf( stderr, "WARNING ");
         break;
    case oyjl_message_insufficient_data:
    case oyjl_message_error:
         fprintf( stderr, "!!! ERROR ");
         break;
  }

  i = 0;
  while(text[i])
    fputc(text[i++], stderr);
  fprintf( stderr, "\n" );

  free( text );

  return yajl_status_ok;
}

int main( int argc, char** argv )
{
  FILE * fp = fopen("test3.json","rb");
  char * text = 0;
  oyjl_value_s * value = 0;
  size_t size = 0;
  int error = 0;

  if(fp)
  {
    fseek(fp,0L,SEEK_END); 
    size = ftell (fp);
    rewind(fp);
    if(size)
    {
      text = malloc(size+1);
      if(text)
        fread(text, sizeof(char), size, fp);
      text[size] = '\000';
    }
  }

  if(text)
  {
    int level = 0;
    oyjl_value_s * root = 0;
    const char * xpath = 0;

    //yajl_status status = oyjl_message_func_set( message_func );

    error = oyjl_tree_from_json( text, &root, NULL );


    if(argc > 1 && strcmp(argv[1],"-v") != 0)
      xpath = argv[1];

    if(!xpath)
    {
      if(argc > 1 && strcmp(argv[1],"-v") == 0)
        oyjl_tree_print( root, &level, stderr );
      else
      {
        char * json = 0;
        oyjl_tree_to_json( root, &level, &json );
        fwrite( json, sizeof(char), strlen(json), stdout );
      }
    }

    if(xpath)
    {
      value = oyjl_tree_get_value( root, xpath );
      printf("%s xpath \"%s\"\n", value?"found":"found not", xpath);
    }
    if(value)
      oyjl_tree_print( value, &level, stderr );

    error = oyjl_tree_free( &root );
    free(text);
    fclose(fp);
  }
  return error;
}


