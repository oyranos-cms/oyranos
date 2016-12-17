/*  @file oyjl.c
 *
 *  oyjl - Yajl tree extension
 *
 *  @par Copyright:
 *            2016 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl command line
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2016/12/17
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "oyjl_tree_internal.h"
#include "oyjl_macros.h"


void printfHelp(int argc, char ** argv)
{
  fprintf( stderr, "\n");
  fprintf( stderr, "%s %s\n",   argv[0],
                                _("is a JSON parse tool"));
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "  %s\n",               _("Print JSON:"));
  fprintf( stderr, "      %s -j [-v] [-i FILE_NAME]\n",        argv[0]);
  fprintf( stderr, "        -i FILE_NAME  %s\n", _("JSON file"));
  fprintf( stderr, "        -x PATH | --xpath PATH\n");
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print a help text:"));
  fprintf( stderr, "      %s -h\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("General options:"));
  fprintf( stderr, "        -v              %s\n", _("verbose"));
  fprintf( stderr, "\n");
  fprintf( stderr, "\n");
}

typedef enum {
  JSON,
  COUNT,
  TYPE,
  KEY
} SHOW;

int main(int argc, char ** argv)
{
  SHOW show = JSON;
  int verbose = 0;
  int error = 0;
  size_t size = 0;
  char * text = NULL;
  const char * input_file_name = NULL,
             * output_file_name = NULL,
             * xpath = 0;
  oyjl_val root = NULL,
           value = NULL;
  int index = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif

  if(argc >= 2)
  {
    int pos = 1, i;
    char *wrong_arg = 0;
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            for(i = 1; pos < argc && i < strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case 'i': OY_PARSE_STRING_ARG(input_file_name); break;
              case 'o': OY_PARSE_STRING_ARG(output_file_name); break;
              case 'x': OY_PARSE_STRING_ARG(xpath); break;
              case 'c': show = COUNT; break;
              case 'j': show = JSON; break;
              case 'k': show = KEY; break;
              case 't': show = TYPE; break;
              case 'v': ++verbose; break;
              case 'h':
              case '-':
                        if(i == 1)
                        {
                             if(OY_IS_ARG("verbose"))
                        { ++verbose; i=100; break; }
                        }
              default:
                        printfHelp(argc, argv);
                        exit (0);
                        break;
            }
            break;
        default:
                        printfHelp(argc, argv);
                        exit (0);
                        break;
      }
      if( wrong_arg )
      {
       fprintf( stderr, "%s %s", "wrong argument to option:", wrong_arg);
       printfHelp(argc, argv);
       exit(1);
      }
      ++pos;
    }
  } else
  {
                        printfHelp(argc, argv);
                        exit (0);
  }

  if(input_file_name)
  {
    FILE * fp = fopen(input_file_name,"rb");

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
  }

  if(text)
  {
    char error_buffer[128] = {0};
    root = oyjl_tree_parse( text, error_buffer, 128 );

    if(xpath)
    {
      char * t = strdup(xpath), *x;
      if(show == KEY)
      {
        x = strrchr( t, '/' );
        if(x)
        {
          if(oyjl_tree_paths_get_index( x+1, &index ))
            index = 0;
          *x = 0;
        }
      }
      value = oyjl_tree_get_value( root, 0, t );
      if(verbose)
        fprintf(stderr, "%s xpath \"%s\"\n", value?"found":"found not", xpath);
    }
    else
      value = root;
  }

  switch(show)
  {
    case JSON:
      {
        char * json = NULL;
        int level = 0;
        oyjl_tree_to_json( value, &level, &json );
        if(json)
          fwrite( json, sizeof(char), strlen(json), stdout );
      }
      break;
    case TYPE:
      switch(value->type)
      {
        case oyjl_t_string: puts( "string" ); break;
        case oyjl_t_number: puts( "number" ); break;
        case oyjl_t_object: puts( "object" ); break;
        case oyjl_t_array: puts( "array" ); break;
        case oyjl_t_true: puts( "true" ); break;
        case oyjl_t_false: puts( "false" ); break;
        case oyjl_t_null: puts( "null" ); break;
        case oyjl_t_any: puts( "any" ); break;
      }
      break;
    case COUNT:
      {
        char n[128] = {0};
        sprintf(n, "%d", oyjl_value_count(value));
        puts( n );
      }
      break;
    case KEY:
      if(value->type == oyjl_t_object && oyjl_value_count(value) > index)
        puts( value->u.object.keys[index] );
      break;
  }

  if(root) oyjl_tree_free( root );
  if(text) free(text);

  return error;
}

