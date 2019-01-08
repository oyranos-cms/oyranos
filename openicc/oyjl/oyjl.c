/*  @file oyjl.c
 *
 *  oyjl - Yajl tree extension
 *
 *  @par Copyright:
 *            2016-2019 (C) Kai-Uwe Behrmann
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


void printfHelp(int argc OYJL_UNUSED, char ** argv)
{
  if(strstr(argv[0],"jsontoyaml"))
  {
  fprintf( stderr, "\n");
  fprintf( stderr, "%s %s\n",   argv[0],
                                _("is a JSON to YAML converter"));
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "      %s FILE_NAME\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "\n");
    return;
  }

  if(strstr(argv[0],"jsontoxml"))
  {
  fprintf( stderr, "\n");
  fprintf( stderr, "%s %s\n",   argv[0],
                                _("is a JSON to XML converter"));
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "      %s FILE_NAME\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "\n");
    return;
  }


  fprintf( stderr, "\n");
  fprintf( stderr, "%s %s\n",   argv[0],
                                _("is a JSON parse tool"));
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "  %s\n",               _("Print:"));
  fprintf( stderr, "      %s [-j|-c|-k|-p|-s STRING] [-v] [-i FILE_NAME] [-x PATH]\n",        argv[0]);
  fprintf( stderr, "        -j\tprint JSON - default mode\n");
  fprintf( stderr, "        -y\tprint YAML - better human readable\n");
  fprintf( stderr, "        -m\tprint XML  - for XML parsers\n");
  fprintf( stderr, "        -c\tprint node count\n");
  fprintf( stderr, "        -k\tprint key name\n");
  fprintf( stderr, "        -p\tprint all matching paths\n");
  fprintf( stderr, "        -v\tset a key to a value\n");
  fprintf( stderr, "        -i FILE_NAME  %s\n", _("JSON file"));
  fprintf( stderr, "        -x PATH\tpath expression, indexing is in edged brackets \"[NUMBER]\"\n");
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print a help text:"));
  fprintf( stderr, "      %s -h\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("General options:"));
  fprintf( stderr, "        -v              %s\n", _("verbose"));
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Example"));
  fprintf( stderr, "  %s\n",               _("Print JSON:"));
  fprintf( stderr, "      %s -j -i example.json -x [0]\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print count:"));
  fprintf( stderr, "      %s -c -i example.json -x [0]/[0]\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print key name:"));
  fprintf( stderr, "      %s -k -i example.json -x [0]/[0]\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print path:"));
  fprintf( stderr, "      %s -p -i example.json -x /\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Set key/value:"));
  fprintf( stderr, "      %s -i example.json -x [0]/[0]/key -s \"value\"\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "\n");
}


typedef enum {
  JSON,
  YAML,
  XML,
  COUNT,
  TYPE,
  KEY,
  PATHS
} SHOW;

int main(int argc, char ** argv)
{
  SHOW show = JSON;
  int verbose = 0;
  int error = 0;
  int size = 0;
  char * text = NULL;
  const char * value_string = NULL;
  const char * input_file_name = NULL,
             * xpath = NULL;
  oyjl_val root = NULL,
           value = NULL;
  int index = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif

  if(strstr(argv[0],"jsontoyaml"))
  {
    if(argc > 1)
      input_file_name = argv[1];
    else
    {
      printfHelp(argc, argv);
      exit (0);
    }
    show = YAML;
  }
  else
  if(strstr(argv[0],"jsontoxml"))
  {
    if(argc > 1)
      input_file_name = argv[1];
    else
    {
      printfHelp(argc, argv);
      exit (0);
    }
    show = XML;
  }
  else
  if(argc >= 2)
  {
    int pos = 1, i;
    char *wrong_arg = 0;
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            for(i = 1; pos < argc && i < (int)strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case 'i': OY_PARSE_STRING_ARG(input_file_name); break;
              case 'x': OY_PARSE_STRING_ARG(xpath); break;
              case 'c': show = COUNT; break;
              case 'j': show = JSON; break;
              case 'k': show = KEY; break;
              case 'm': show = XML; break;
              case 'p': show = PATHS; break;
              case 's': OY_PARSE_STRING_ARG(value_string); break;
              case 't': show = TYPE; break;
              case 'v': ++verbose; break;
              case 'y': show = YAML; break;
              case 'h':
              case '-':
                        if(i == 1)
                        {
                             if(OY_IS_ARG("verbose"))
                        { ++verbose; i=100; break; }
                        } OYJL_FALLTHROUGH
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
    FILE * fp;
   
    if(strcmp(input_file_name,"-") == 0)
      fp = stdin;
    else
      fp = fopen(input_file_name,"rb");

    if(fp)
    {
      text = oyjlReadFileStreamToMem( fp, &size ); 
      if(fp != stdin) fclose( fp );
    }
  }

  if(text)
  {
    char error_buffer[256] = {0};
    if(verbose)
      fprintf(stderr, "file read:\t\"%s\"\n", input_file_name);
    root = oyjlTreeParse( text, error_buffer, 256 );
    if(error_buffer[0] != '\000')
      fprintf(stderr, "ERROR:\t\"%s\"\n", error_buffer);
    if(verbose)
      fprintf(stderr, "file parsed:\t\"%s\"\n", input_file_name);

    if(xpath)
    {
      char ** paths = NULL;
      int count = 0, i;

      oyjlTreeToPaths( root, 1000000, xpath, 0, &paths );
      while(paths && paths[count]) ++count;

      if(show == PATHS)
        for(i = 0; i < count; ++i)
          fprintf(stdout,"%s\n", paths[i]);
      else if(show == KEY)
        fprintf(stdout,"%s\n", (count && paths[0] && strlen(strchr(paths[0],'/'))) ? strrchr(paths[0],'/') + 1 : "");

      if(paths || value_string)
        value = oyjlTreeGetValue( root,
                                     value_string ? OYJL_CREATE_NEW : 0,
                                     paths?paths[0]:xpath );
      if(verbose)
        fprintf(stderr, "%s xpath \"%s\"\n", value?"found":"found not", xpath);

      oyjlStringListRelease( &paths, count, free );

      if(value_string)
      {
        if(value)
          oyjlValueSetString( value, value_string );
        else
          oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_"obtained no leaf for xpath \"%s\" from JSON:\t\"%s\"",
                         OYJL_DBG_ARGS_, xpath, input_file_name );
      }

      if(verbose)
        fprintf(stderr, "processed:\t\"%s\"\n", input_file_name);
    }
    else if(value_string)
      oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_"set argument needs as well a xpath argument", OYJL_DBG_ARGS_ );
    else
      value = root;
  }

  if(value)
  switch(show)
  {
    case JSON:
    case YAML:
    case XML:
      {
        char * text = NULL;
        int level = 0;
        if(show == JSON)
          oyjlTreeToJson( value_string ? root : value, &level, &text );
        else if(show == YAML)
        {
          oyjlTreeToYaml( value_string ? root : value, &level, &text );
          oyjlStringAdd( &text, 0,0, "\n" );
        }
        else if(show == XML)
        {
          oyjlTreeToXml( value_string ? root : value, &level, &text );
          oyjlStringAdd( &text, 0,0, "\n" );
        }
        if(text)
        {
          fwrite( text, sizeof(char), strlen(text), stdout );
          free(text);
        }
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
        sprintf(n, "%d", oyjlValueCount(value));
        puts( n );
      }
      break;
    case KEY:
      if(!xpath && value->type == oyjl_t_object && oyjlValueCount(value) > index)
        puts( value->u.object.keys[index] );
      break;
    case PATHS:
      if(!xpath)
      {
        char ** paths = NULL;
        int count = 0, i;

        oyjlTreeToPaths( root, 1000000, NULL, 0, &paths );
        if(verbose)
          fprintf(stderr, "processed:\t\"%s\"\n", input_file_name);
        while(paths && paths[count]) ++count;

        for(i = 0; i < count; ++i)
          fprintf(stdout,"%s\n", paths[i]);

        oyjlStringListRelease( &paths, count, free );
      }
      break;
    default: break;
  }

  if(root) oyjlTreeFree( root );
  if(text) free(text);

  return error;
}

