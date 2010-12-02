/* gcc -Wall -g `pkg-config oyranos libxml-2.0 --libs --cflags` oy_filter_node.c -o oy_filter_node */
#include <oyranos_alpha.h>
#include <oyranos_helper.h>
#include <oyranos_i18n.h>
#include "oyranos_string.h"
#include <oyranos_texts.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <stdio.h>
#include <string.h>

#include "oyranos_widgets_fltk.h"
#include "oyranos_forms.h"
#include "../fl_i18n/fl_i18n.H"
#include "config.h"

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Help_View.H>

using namespace oyranos;
using namespace oyranos::forms;

extern oyUiHandler_s * oy_ui_fltk_handlers[];


void usage(int argc, char ** argv)
{
  printf("\n");
  printf("oyranos-xforms v%d.%d.%d %s\n",
                          OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C,
                                _("is a Oyranos module options tool"));
  printf("%s\n",                 _("Usage"));
  printf("  %s\n",               _("Show options [include policy]"));
  printf("      %s -n \"module_name\" [-f]\n", argv[0]);
  printf("\n");
  printf("  %s\n",               _("Write Results:"));
  printf("      %s -n \"module_name\" -o \"xml_file\"\n", argv[0]);
  printf("\n");
  printf("  %s\n",               _("Get XFORMS:"));
  printf("      %s -n \"module_name\" -x \"xhtml_file\"\n", argv[0]);
  printf("\n");
  printf("  %s\n",               _("General options:"));
  printf("      -v  %s\n",       _("verbose"));
  printf("      -i \"xhtml_file\"  %s\n",_("read XFORMS"));
  printf("      -f  %s\n",       _("show policy options"));
  printf("\n");
  puts(_("For more informations read the man page:"));
  printf("      man oyranos-xforms_not_yet\n");
}

void callback_done( Fl_Widget * w, void * )
{
  w->window()->hide();
}

void callback_help_view( oyPointer * ptr, const char * help_text )
{
  int error = 0;

#if HELP_VIEW_DISPLAY
  Fl_Text_Buffer * buffer = 0;
  buffer = (Fl_Text_Buffer*)ptr;
  if(buffer)
    buffer->text( help_text?help_text:"" );
#else
  Fl_Help_View * help_view = (Fl_Help_View*)ptr;
  if(help_view)
  {
    /* Format plain text to some HTML codes */
    if(help_text)
    {
      static size_t len = 1024;
      static char * text = (char*) malloc( len );
      int i = 0, ti = 0;
      char c;

      if(len < strlen(help_text)*2 + 24)
      {
        len = strlen(help_text)*2 + 24;
        free( text );
        text = (char*) malloc( len );
      }

      while( help_text[i] )
      {
        c = help_text[i];
        if(c == '\n')  /* line break */
        {
          sprintf( &text[ti], "<br>" );
          ti += 4;
        }
        else if(c == ' ') /* empty space */
        {
          sprintf( &text[ti], "&nbsp;" );
          ti += 6;
        } else
        {
          text[ti] = c;
          ++ti;
        }
        ++i;
      }
      text[ti] = 0;

      help_view->value(text);
    } else
    {
      /* Erase only if the widget needs no scrollbar. */
      help_view->topline(10000);
      help_view->leftline(10000);
      if(help_view->topline() == 0 && help_view->leftline() == 0)
        help_view->value("");

      /* bring text back after the tests */
      help_view->topline( 0 );
      help_view->leftline( 0 );
    }
  }
#endif
  else
    error = 1;
}


int main (int argc, char ** argv)
{
  const char * node_name = 0;
  const char * output_xml_file = 0,
             * input_xml_file = 0;
  const char * output_model_file = 0,
             * result_xml = 0;
  oyFilterNode_s * node = 0;
  char * ui_text = 0,
      ** namespaces = 0,
       * text = 0, * t = 0;
  const char * opt_names = 0;
  oyFormsArgs_s * forms_args = oyFormsArgs_New( 0 );
  const char * data = 0, * ct = 0;
  char ** other_args = 0;
  int other_args_n = 0;
  int error = 0,
      i;
  oyOptions_s * opts = 0;
  oyOption_s * o = 0;
  int front = 0;  /* front end options */
  int print = 1;

#ifdef USE_GETTEXT
  const char *locale_paths[2] = {OY_SRC_LOCALEDIR,OY_LOCALEDIR};
  const char *domain = {"oyranos"};
  int is_path = -1;

  is_path = fl_search_locale_path  ( 2,
                                locale_paths,
                                "de",
                                domain);
  if(is_path < 0)
    fprintf( stderr, "Locale not found\n");
  else
  {
#if defined(_Xutf8_h) || HAVE_FLTK_UTF8
    FL_I18N_SETCODESET set_charset = FL_I18N_SETCODESET_UTF8;
#else
    FL_I18N_SETCODESET set_charset = FL_I18N_SETCODESET_SELECT;
#endif
    int err = fl_initialise_locale ( domain, locale_paths[is_path],
                                     set_charset );
    if(err) {
      fprintf( stderr,"i18n initialisation failed");
    } /*else
      fprintf( stderr, "Locale found in %s\n", locale_paths[is_path]);*/
  }
  oy_domain_codeset = fl_i18n_codeset;
#endif


/* allow "-opt val" and "-opt=val" syntax */
#define OY_PARSE_STRING_ARG( opt ) \
                        if( pos + 1 < argc && argv[pos][i+1] == 0 ) \
                        { opt = argv[pos+1]; \
                          if( opt == 0 && strcmp(argv[pos+1],"0") ) \
                            wrong_arg = "-" #opt; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+1] == '=') \
                        { opt = &argv[pos][i+2]; \
                          if( opt == 0 && strcmp(&argv[pos][i+2],"0") ) \
                            wrong_arg = "-" #opt; \
                          i = 1000; \
                        } else wrong_arg = "-" #opt; \
                        if(oy_debug) printf(#opt "=%s\n",opt)

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
              case 'o': OY_PARSE_STRING_ARG( output_model_file ); break;
              case 'f': front = 1; break;
              case 'i': OY_PARSE_STRING_ARG( input_xml_file ); break;
              case 'x': OY_PARSE_STRING_ARG( output_xml_file ); break;
              case 'v': oy_debug += 1; break;
              case '-':
                        if(strcmp(&argv[pos][2],"verbose") == 0)
                        { oy_debug += 1; i=100; break;
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
                        printf("%s -%c\n", _("Unknown argument"), argv[pos][i]);
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
        printf("%s %s\n", _("wrong argument to option:"), wrong_arg);
        exit(1);
      }
      ++pos;
    }
    if(oy_debug) printf( "%s\n", argv[1] );

  }

  if(!node_name && !input_xml_file)
  {
                        usage(argc, argv);
                        exit (0);
  }

  if(node_name)
  {
    int attributes = 0;

    node = oyFilterNode_NewWith( node_name, 0,0 );
    if(!node)
    {
      printf("No module found with name: %s", node_name);
      usage( argc, argv );
      exit( 1 );
    } else
      oyOptions_Release( &node->core->options_ );

    attributes = OY_SELECT_FILTER | OY_SELECT_COMMON |
                                    oyOPTIONATTRIBUTE_ADVANCED;
    if(front)
      attributes |= oyOPTIONATTRIBUTE_FRONT;
    opts = oyFilterNode_OptionsGet( node, attributes );

    /* ... then get the UI for this filters options. */
    error = oyFilterNode_UiGet( node, &ui_text, &namespaces, malloc );
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
          printf("Unknown option: %s", other_args[i]);
          usage( argc, argv );
          exit( 1 );

        } else
        {
          o = oyOptions_Find( opts, other_args[i] );
          if(i + 1 < other_args_n)
          {
            ct = oyOption_GetText( o, oyNAME_NICK );
            printf( "%s => ",
                    ct ); ct = 0;
            oyOption_SetFromText( o, other_args[i + 1], 0 );
            data = oyOption_GetText( o, oyNAME_NICK );

            printf( "%s\n",
                    oyStrchr_(data, ':') + 1 ); data = 0;
         }
          else
          {
            printf("%s: --%s  argument missed\n", _("Option"), other_args[i] );
            exit( 1 );
          }
          oyOption_Release( &o );
        }
      }
      print = 0;
    }

    forms_args->print = print;


    data = oyOptions_GetText( opts, oyNAME_NAME );
    text = oyXFORMsFromModelAndUi( data, ui_text, (const char**)namespaces, 0,
                                   malloc );

    if(namespaces)
    {
      i = 0;
      while(namespaces[i])
      {
        if(oy_debug)
          printf("namespaces[%d]: %s\n", i, namespaces[i]);
        free( namespaces[i++] );
      }
      free(namespaces);
    }
    if(ui_text) free(ui_text); ui_text = 0;

  }

  /* get Layout file */
  if(input_xml_file)
  {
    size_t size = 0;
    text = oyReadFileToMem_(input_xml_file, &size, oyAllocateFunc_);
  }

  if(oy_debug)
    printf("%s\n", text);

  Fl_Double_Window * w = new Fl_Double_Window(400,475,"XFORMS in FLTK");
    oyCallback_s callback = {oyOBJECT_CALLBACK_S, 0,0,0,
                                  (void(*)())callback_help_view,0};
#if HELP_VIEW_DISPLAY
    Fl_Text_Display * help_view = new Fl_Text_Display( 0,340,400,100 );
#else
    Fl_Help_View * help_view = new Fl_Help_View( 0,340,400,100 );
#endif
    help_view->box(FL_ENGRAVED_BOX);
    help_view->color(FL_BACKGROUND_COLOR);
    help_view->selection_color(FL_DARK1);
#if HELP_VIEW_DISPLAY
      Fl_Text_Buffer * buffer = new Fl_Text_Buffer(0);
      buffer->append( _("Hints") );
    help_view->buffer( buffer );
    callback.data = buffer;
#else
    help_view->value("");
    callback.data = help_view;
#endif
    oyFormsArgs_ResourceSet( forms_args, OYFORMS_FLTK_HELP_VIEW_REG,
                             (oyPointer)&callback);

    Fl_Button * done_button = new Fl_Button( 160, 445, 80, 25, _("&Done"));
    done_button->callback( callback_done, 0 );

  Fl_Scroll * scroll = new Fl_Scroll( 5,1,395,338 );
  scroll->box( FL_NO_BOX ); //FL_THIN_UP_BOX );
    OyFl_Pack_c * pack = new OyFl_Pack_c( 5,1,395,338 );
    pack->spacing(V_SPACING);
      error = oyXFORMsRenderUi( text, oy_ui_fltk_handlers, forms_args );


    pack->end();
  scroll->end();
  w->resizable( scroll );
  w->end();

  w->show();
  Fl::run();


  result_xml = oyFormsArgs_ModelGet( forms_args );
  if(output_model_file)
    oyWriteMemToFile_( output_model_file, result_xml, strlen(result_xml) );
  else
    printf( "%s\n", result_xml?result_xml:"---" );
  oyFormsArgs_Release( &forms_args );

  if(output_xml_file)
    oyWriteMemToFile_( output_xml_file, text, strlen(text) );

  /* xmlParseMemory sollte der Ebenen gewahr werden wie oyOptions_FromText. */
  opts = oyOptions_FromText( data, 0,0 );

  if(text) free(text); text = 0;

  return error;
}


