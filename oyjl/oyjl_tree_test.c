#include "oyjl_tree.h"


int main( int argc, char** argv )
{
  FILE * fp = fopen("test2.json","rb");
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
    const char * xpath = "Inhaber/Vorname";

    error = oyjl_tree_from_json( text, &root, NULL );


    if(argc > 1)
      xpath = argv[1];
    else
      oyjl_tree_print( root, &level, stderr );

    value = oyjl_tree_get_value( root, xpath );
    printf("%s xpath \"%s\"\n", value?"found":"found not", xpath);
    if(value)
      oyjl_tree_print( value, &level, stderr );

    error = oyjl_tree_free( &root );
    free(text);
    fclose(fp);
  }
  return error;
}


