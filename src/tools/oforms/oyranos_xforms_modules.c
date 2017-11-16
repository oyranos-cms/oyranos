/** @file oyranos_xforms_modules.c
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
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2012/12/11
 *
 */
/*  gcc -Wall -g -I. -I../build_11.2 oforms/oyranos_xforms_modules.c `pkg-config oyranos libxml-2.0 --libs --cflags` -o oyranos-xforms-module
*/
#include "oyranos_config_internal.h"
#include "oyCMMapiFilters_s.h"
#include "oyCMMapiFilter_s_.h"
#include "oyCMMapi4_s_.h"
#include "oyCMMapi6_s_.h"
#include "oyCMMapi7_s_.h"
#include "oyCMMapi8_s_.h"
#include "oyCMMapi9_s_.h"
#include "oyCMMapi10_s_.h"
#include "oyranos_module_internal.h"
#include <oyranos.h>
#include <oyranos_debug.h>
#include <oyranos_helper.h>
#include <oyranos_i18n.h>
#include <oyranos_string.h>
#include <oyranos_sentinel.h>
#include <oyranos_texts.h>

#include <oyFilterNode_s.h>

#include <stdio.h>
#include <string.h>

#include "oyranos_forms.h"


void usage(int argc OY_UNUSED, char ** argv)
{
  fprintf( stderr, "\n");
  fprintf( stderr, "oyranos-xforms v%d.%d.%d %s\n",
                          OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C,
                                _("is a Oyranos module options tool"));
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "  %s\n",               _("Show options [include policy]"));
  fprintf( stderr, "      %s -n \"module_name\" [-f]\n", argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Get XFORMS:"));
  fprintf( stderr, "      %s -n \"module_name\" -x \"xhtml_file\"\n", argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Get Information List:"));
  fprintf( stderr, "      %s -n \"module_name\" -t \"\"\n", argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Get Informations:"));
  fprintf( stderr, "      %s -n \"module_name\" --short -t \"name\"\n", argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("List Modules:"));
  fprintf( stderr, "      %s -l\n", argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("General options:"));
  fprintf( stderr, "      -v  %s\n",       _("verbose"));
  fprintf( stderr, "      -f  %s\n",       _("show policy options"));
  fprintf( stderr, "\n");
  fputs( _("For more informations read the man page:"), stderr);
  fprintf( stderr, "      man oyranos-xforms-modules\n");
}

void printUi( oyCMMui_s_ * ui, const char * text_info, oyNAME_e type );


int main (int argc, char ** argv)
{
  const char * node_name = 0,
             * text_info = 0;
  oyNAME_e type = oyNAME_DESCRIPTION;
  const char * output_xml_file = 0;
  oyFilterNode_s * node = 0;
  char * ui_text = 0,
      ** namespaces = 0,
       * text = 0, * t = 0;
  const char * opt_names = 0;
  const char * data = 0, * ct = 0;
  char ** other_args = 0;
  int other_args_n = 0;
  int error = 0,
      i,
      verbose = 0,
      list = 0;
  oyOptions_s * opts = 0;
  oyOption_s * o = 0;
  int front = 0;  /* front end options */

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyExportStart_(EXPORT_CHECK_NO);

/* allow "-opt val" and "-opt=val" syntax */
  if(argc != 1)
  {
    int pos = 1, i;
    const char *wrong_arg = 0;
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            for(i = 1; i < (int)strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case 'n': OY_PARSE_STRING_ARG( node_name ); break;
              case 't': OY_PARSE_STRING_ARG( text_info ); break;
              case 'f': front = 1; break;
              case 'l': list = 1; break;
              case 'x': OY_PARSE_STRING_ARG( output_xml_file ); break;
              case 'v': if(verbose) oy_debug += 1; verbose = 1; break;
              case '-':
                        if(strcmp(&argv[pos][2],"verbose") == 0)
                        { if(verbose) oy_debug += 1; verbose = 1; i=100; break;
                        }
                        if(strcmp(&argv[pos][2],"short") == 0)
                        { if(type == oyNAME_NAME) type = oyNAME_NICK; else type = oyNAME_NAME; i=100; break;
                        }
                        STRING_ADD( t, &argv[pos][2] );
                        text = oyStrrchr_(t, '=');
                        /* get the key only */
                        if(text)
                          text[0] = 0;
                        oyStringListAddStaticString_( &other_args,&other_args_n,
                                                      t,
                                            oyAllocateFunc_,oyDeAllocateFunc_ );
                        if(text)
                        oyStringListAddStaticString_( &other_args,&other_args_n,
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
              default:
                        fprintf( stderr, "%s -%c\n", _("Unknown argument"), argv[pos][i]);
                        usage(argc, argv);
                        exit (0);
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
      ++pos;
    }
    if(oy_debug || verbose) fprintf( stderr, "%s\n", argv[1] );

  }

  if(!node_name && !list)
  {
                        usage(argc, argv);
                        exit (0);
  }

  if(node_name)
  {
    int attributes = 0;

    int j;
    if(text_info)
    {
      for(j = oyOBJECT_CMM_API4_S; j <= (int)oyOBJECT_CMM_API10_S; j++)
      {
        uint32_t * rank_list = 0;
        uint32_t apis_n = 0;
        oyCMMapiFilters_s * apis = oyCMMsGetFilterApis_( "//",
                                                         (oyOBJECT_e)j, 0,
                                                         &rank_list, &apis_n );
        int n = oyCMMapiFilters_Count( apis ), i;
        for(i = 0; i < n; ++i)
        {
          oyCMMapiFilter_s_ * f = (oyCMMapiFilter_s_*) oyCMMapiFilters_Get( apis, i );
          if(f && oyFilterRegistrationMatch(f->registration, node_name, j))
          {
            int l = 0;

            fprintf( stdout,  "[%s]:\t\"%s\"\t%s\n",
                              oyStructTypeToText(f->type_),
                              f->registration,
                              f->id_ );
            oyCMMapi4_s_ * s4 = (oyCMMapi4_s_*)f;
            oyCMMapi6_s_ * s6 = (oyCMMapi6_s_*)f;
            oyCMMapi7_s_ * s7 = (oyCMMapi7_s_*)f;
            oyCMMapi8_s_ * s8 = (oyCMMapi8_s_*)f;
            oyCMMapi9_s_ * s9 = (oyCMMapi9_s_*)f;
            oyCMMapi10_s_ * s10 = (oyCMMapi10_s_*)f;
            switch((int)f->type_)
            {
            case oyOBJECT_CMM_API4_S:
              fprintf( stdout,  "context_type: %s\n", oyNoEmptyString_m_(s4->context_type) );
              printUi( s4->ui, text_info, type );
              break;
            case oyOBJECT_CMM_API6_S:
              fprintf( stdout,  "%s -> %s\n", oyNoEmptyString_m_(s6->data_type_in), oyNoEmptyString_m_(s6->data_type_out) );
              break;
            case oyOBJECT_CMM_API7_S:
              fprintf( stdout,  "context_type: %s\n", oyNoEmptyString_m_(s7->context_type) );
              {
                while(s7->properties && s7->properties[l])
                {
                  fprintf(stdout, "\t%s\n", oyNoEmptyString_m_(s7->properties[l]) );
                  ++l;
                }
              }
              break;
            case oyOBJECT_CMM_API8_S:
              printUi( (oyCMMui_s_*) s8->ui, text_info, type );
              break;
            case oyOBJECT_CMM_API9_S:
              if(s9->getText)
              {
                if(text_info[0])
                  fprintf( stdout,  "%s\n", oyNoEmptyString_m_(s9->getText( text_info, type, (oyStruct_s*)s9)) );
                else
                {
                  fprintf( stdout,  "%s\n", _("Info Types:") );
                  while(s9->texts[l])
                  {
                    fprintf(stdout, "\t%s\n", oyNoEmptyString_m_(s9->texts[l]) );
                    ++l;
                  }
                }
              }
              break;
            case oyOBJECT_CMM_API10_S:
              if(s10->getText)
              {
                if(text_info[0])
                  fprintf( stdout,  "%s\n", oyNoEmptyString_m_(s10->getText( text_info, type, (oyStruct_s*)s10)) );
                else
                {
                  fprintf( stdout,  "%s\n", _("Info Types:") );
                  while(s10->texts[l])
                  {
                    fprintf(stdout, "\t%s\n", oyNoEmptyString_m_(s10->texts[l]) );
                    ++l;
                  }
                }
              }
              break;
            }
          }
        }
        oyCMMapiFilters_Release( &apis );
      }
      oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );

      return 0;
    }

    node = oyFilterNode_NewWith( node_name, 0,0 );
    if(!node)
    {
      fprintf( stderr, "No module found with name: %s", node_name);
      usage( argc, argv );
      exit( 1 );
    } else
    {
      opts = oyFilterNode_GetOptions( node, 0 );
      oyOptions_Clear( opts );
      oyOptions_Release( &opts );
    }

    attributes = OY_SELECT_FILTER | OY_SELECT_COMMON |
                                    oyOPTIONATTRIBUTE_ADVANCED;
    if(front)
      attributes |= oyOPTIONATTRIBUTE_FRONT;
    opts = oyFilterNode_GetOptions( node, attributes );

    /* ... then get the UI for this filters options. */
    error = oyFilterNode_GetUi( node, &ui_text, &namespaces, malloc );
    oyFilterNode_Release( &node );

    data = oyOptions_GetText( opts, oyNAME_NAME );
    opt_names = oyOptions_GetText( opts, oyNAME_DESCRIPTION );

    if(other_args)
    {
      for( i = 0; i < other_args_n; i += 2 )
      {
        /* check for wrong args */
        if(strstr( opt_names, other_args[i] ) == NULL)
        {
          fprintf( stderr, "Unknown option: %s", other_args[i]);
          usage( argc, argv );
          exit( 1 );

        } else
        {
          o = oyOptions_Find( opts, other_args[i], oyNAME_PATTERN );
          if(i + 1 < other_args_n)
          {
            ct = oyOption_GetText( o, oyNAME_NICK );
            fprintf( stderr, "%s => ",
                    ct ); ct = 0;
            oyOption_SetFromString( o, other_args[i + 1], 0 );
            data = oyOption_GetText( o, oyNAME_NICK );

            fprintf( stderr, "%s\n",
                    oyStrchr_(data, ':') + 1 ); data = 0;
         }
          else
          {
            fprintf(stderr, "%s: --%s  argument missed\n", _("Option"), other_args[i] );
            exit( 1 );
          }
          oyOption_Release( &o );
        }
      }
    }

    data = oyOptions_GetText( opts, oyNAME_NAME );
    text = oyXFORMsFromModelAndUi( data, ui_text, (const char**)namespaces, 0,
                                   malloc );

    data = 0;
    oyOptions_Release( &opts );

    if(namespaces)
    {
      i = 0;
      while(namespaces[i])
      {
        if(oy_debug || verbose)
          fprintf(stderr, "namespaces[%d]: %s\n", i, namespaces[i]);
        free( namespaces[i++] );
      }
      free(namespaces);
    }
    if(ui_text){ free(ui_text); ui_text = 0; }

  } else if(list)
  {
    int j;
    for(j = oyOBJECT_CMM_API4_S; j <= (int)oyOBJECT_CMM_API10_S; j++)
    {
      uint32_t * rank_list = 0;
      uint32_t apis_n = 0;
      oyCMMapiFilters_s * apis = oyCMMsGetFilterApis_( "//",
                                                       (oyOBJECT_e)j, 0,
                                                       &rank_list, &apis_n );
      int n = oyCMMapiFilters_Count( apis ), i;
      for(i = 0; i < n; ++i)
      {
        oyCMMapiFilter_s_ * f = (oyCMMapiFilter_s_*) oyCMMapiFilters_Get( apis, i );
        if(f)
          fprintf( stdout,  "[%s]:\t\"%s\"\t%s\n",
                            oyStructTypeToText(f->type_),
                            f->registration,
                            f->id_ );
        else
          fprintf( stdout, "      no api obtained %d",i);
        /*oyCMMapiFilter_Release( (oyCMMapiFilter_s**)&f );*/
      }
      oyCMMapiFilters_Release( &apis );
    }

    oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );

    return 0;
  }

  if(output_xml_file)
  {
    oyWriteMemToFile_( output_xml_file, text, strlen(text) );
    if(oy_debug || verbose)
      fprintf(stderr, "%s\n", text);
  }
  else
    printf("%s\n", oyNoEmptyString_m_(text));


  if(text){ free(text); text = 0; }

  oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );

  return error;
}

void printUi( oyCMMui_s_ * ui, const char * text_info, oyNAME_e type )
{
  int l = 0;
  if(ui && ui->getText)
  {
    if(text_info[0])
      fprintf( stdout,  "%s\n", oyNoEmptyString_m_(ui->getText( text_info, type, (oyStruct_s*)ui) ) );
    else
    {
      fprintf( stdout,  "%s\n", _("Info Types:") );
      while(ui->texts[l])
      {
        fprintf(stdout, "\t%s\n", oyNoEmptyString_m_(ui->texts[l]) );
        ++l;
      }
    }
  }
}
