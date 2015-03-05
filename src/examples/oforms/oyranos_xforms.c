/** @file oyranos_xforms.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2012-2015 (C) Kai-Uwe Behrmann
 *
 *  @brief    module options tool
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2012/12/11
 *
 */
/* gcc -Wall -g `pkg-config oyranos libxml-2.0 --libs --cflags` oy_filter_node.c -o oy_filter_node */
#include <oyranos_helper.h>
#include <oyranos_i18n.h>
#include "oyranos_string.h"
#include <oyranos_texts.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <stdio.h>
#include <string.h>

#include "oyranos_forms.h"



void usage(int argc, char ** argv)
{
  fprintf(stderr, "\n");
  fprintf(stderr, "oyranos-xforms v%d.%d.%d %s\n",
                        OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C,
                                _("is a Oyranos module options tool"));
  fprintf(stderr, "%s\n",                 _("Usage"));
  fprintf(stderr, "  %s\n",               _("Show options"));
  fprintf(stderr, "      %s -i \"o(X)FORMS.xhtml\" [-l] [-h]\n", argv[0]);
  fprintf(stderr, "      -h  %s\n",       _("show help texts"));
  fprintf(stderr, "      -l  %s\n",       _("list possible choices"));
  fprintf(stderr, "\n");
  fprintf(stderr, "  %s\n",               _("Write Model:"));
  fprintf(stderr, "      %s -i \"o(X)FORMS.xhtml\" -o \"xml_file\"\n", argv[0]);
  fprintf(stderr, "\n");
  fprintf(stderr, "  %s\n",               _("Show Model:"));
  fprintf(stderr, "      %s -i \"o(X)FORMS.xhtml\" -O\n", argv[0]);
  fprintf(stderr, "      %s -i \"o(X)FORMS.xhtml\" --key=value\n", argv[0]);
  fprintf(stderr, "\n");
  fprintf(stderr, "  %s\n",               _("General options:"));
  fprintf(stderr, "      -v  %s\n",       _("verbose"));
  fprintf(stderr, "\n");
  fprintf(stderr, _("For more informations read the man page:"));
  fprintf(stderr, "\n");
  fprintf(stderr, "      man oyranos-xforms\n");
}


int main (int argc, char ** argv)
{
  int output_model = 0;
  const char * input_xml_file = 0;
  const char * output_model_file = 0,
             * result_xml = 0;
  char * text = 0, * t = 0;
  const char * opt_names = 0;
  oyFormsArgs_s * forms_args = oyFormsArgs_New( 0 );
  const char * data = 0, * ct = 0;
  char ** other_args = 0;
  int other_args_n = 0;
  int error = 0,
      i;
  int print = 1;
  int help = 0;
  oyOptions_s * opts = 0;
  oyOption_s * o = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyI18NInit_();


  if(argc != 1)
  {
    int pos = 1, i;
    char *wrong_arg = 0;
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            for(i = 1; i < strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case 'o': OY_PARSE_STRING_ARG( output_model_file ); break;
              case 'O': output_model = 1; print = 0; break;
              case 'i': OY_PARSE_STRING_ARG( input_xml_file ); break;
              case 'v': oy_debug += 1; break;
              case 'h': print |= 0x02; break;
              case 'l': print |= 0x04; break;
              case '-':
                        if(OY_IS_ARG("help"))
                        { help = 1; i=100; break; }
                        else if(strcmp(&argv[pos][2],"verbose") == 0)
                        { oy_debug += 1; i=100; break;
                        } else
                        {
                          STRING_ADD( t, &argv[pos][2] );
                          text = oyStrrchr_(t, '=');
                          /* get the key only */
                          if(text)
                            text[0] = 0;
                          oyStringListAddStaticString_( &other_args,&other_args_n,
                                                        t,
                                            oyAllocateFunc_,oyDeAllocateFunc_ );
                          if(text)
                            oyStringListAddStaticString_(
                                            &other_args,&other_args_n,
                                            oyStrrchr_(&argv[pos][2], '=') + 1,
                                            oyAllocateFunc_,oyDeAllocateFunc_ );
                          else {
                            if(argv[pos+1])
                            {
                              oyStringListAddStaticString_( &other_args,
                                                            &other_args_n,
                                                            argv[pos+1],
                                            oyAllocateFunc_,oyDeAllocateFunc_ );
                              ++pos;
                            } else wrong_arg = argv[pos];
                          }
                          if(t) oyDeAllocateFunc_( t );
                          t = 0;
                          i=100; break;
                        }
              case '?':
              default:
                        help = 1;
                        break;
            }
            break;
        default:
            wrong_arg = argv[pos];
      }
      if( wrong_arg )
      {
        fprintf( stderr, "%s %s\n", _("wrong argument to option:"), wrong_arg);
        exit(1);
      }
      if(help)
      {
                        usage(argc, argv);
                        exit (0);
      }
      ++pos;
    }
    if(oy_debug) fprintf( stderr, "%s\n", argv[1] );

  }

  if(!input_xml_file)
  {
    size_t text_size = 0;
    text = oyReadStdinToMem_(&text_size, oyAllocateFunc_);

    if(text_size == 0)
    {
                        usage(argc, argv);
                        exit (0);
    }
  }

  /* get Layout file */
  if(input_xml_file)
  {
    size_t size = 0;
    text = oyReadFileToMem_(input_xml_file, &size, oyAllocateFunc_);
  }

  if(oy_debug)
    fprintf( stderr, "%s\n", text);

  if(other_args)
  {
    forms_args->print = 0;

    error = oyXFORMsRenderUi( text, oy_ui_cmd_line_handlers, forms_args );
    result_xml = oyFormsArgs_ModelGet( forms_args );

    opts = oyOptions_FromText( result_xml, 0,0 );

    data = oyOptions_GetText( opts, oyNAME_NAME );
    opt_names = oyOptions_GetText( opts, oyNAME_DESCRIPTION );

      for( i = 0; i < other_args_n; i += 2 )
      {
        /* check for wrong args */
        if(opt_names && strstr( opt_names, other_args[i] ) == NULL)
        {
          fprintf(stderr, "Unknown option: %s", other_args[i]);
          usage( argc, argv );
          exit( 1 );

        } else
        {
          o = oyOptions_Find( opts, other_args[i], oyNAME_PATTERN );
          if(i + 1 < other_args_n)
          {
            ct = oyOption_GetText( o, oyNAME_NICK );
            if(oy_debug)
            fprintf( stderr, "%s => ",
                    ct?ct:"---" ); ct = 0;
            oyOption_SetFromText( o, other_args[i + 1], 0 );
            data = oyOption_GetText( o, oyNAME_NICK );

            if(oy_debug)
            fprintf( stderr, "%s\n",
                    data?oyStrchr_(data, ':') + 1:"" ); data = 0;
          }
          else
          {
            fprintf( stderr,
                    "%s: --%s  argument missed\n", _("Option"), other_args[i] );
            exit( 1 );
          }
          oyOption_Release( &o );
        }
      }
      print = 0;
  }

  forms_args->print = print;

  /*if(print) */
  error = oyXFORMsRenderUi( text, oy_ui_cmd_line_handlers, forms_args );

  /* prefere the parsed options from the command line */
  if(opts)
    result_xml = oyOptions_GetText( opts, oyNAME_NAME );
  else
    result_xml = oyFormsArgs_ModelGet( forms_args );

  if(output_model_file && result_xml)
  {
    if(result_xml)
      oyWriteMemToFile_( output_model_file, result_xml, strlen(result_xml) );
    else
      fprintf( stderr, "%s\n", "no model found" );
  }
  else if(opts || output_model)
  {
    if(result_xml)
      printf( "%s\n", result_xml );
    else
      fprintf( stderr, "%s\n", "no model found" );
  }

  /* xmlParseMemory should see the level like oyOptions_FromText. */
  opts = oyOptions_FromText( data, 0,0 );

  if(text) free(text); text = 0;

  return error;
}


