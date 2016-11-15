#include "oyjl_tree.h"
#include <stdarg.h>


int main( int argc, char** argv )
{
  FILE * fp = fopen("test3.json","rb");
  char * text = 0;
  oyjl_val value = 0;
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
    oyjl_val root = 0;
    const char * xpath = 0;
    char error_buffer[128];

    root = oyjl_tree_parse( text, error_buffer, 128 );

    if(argc > 1 && strcmp(argv[1],"-v") != 0)
      xpath = argv[1];

    if(!xpath)
    {
      char * json = 0;
      oyjl_tree_to_json( root, &level, &json );
      fwrite( json, sizeof(char), strlen(json), stdout );
    }

    if(xpath)
    {
      value = oyjl_tree_get_value( root, 0, xpath );
      printf("%s xpath \"%s\"\n", value?"found":"found not", xpath);
    }
    if(value)
    {
      char * json = 0;
      oyjl_tree_to_json( value, &level, &json );
      fwrite( json, sizeof(char), strlen(json), stdout );
    }

    oyjl_tree_free( root );
    free(text);
    fclose(fp);
  }
  return error;
}


