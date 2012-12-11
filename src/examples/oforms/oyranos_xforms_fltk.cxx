/** @file oyranos_xforms_fltk.cxx
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2009-2011 (C) Kai-Uwe Behrmann
 *
 *  @brief    forms handling for the FLTK toolkit
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/00/00
 */

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
#include <iostream>

#include "oyranos_widgets_fltk.h"
#include "oyranos_forms.h"
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
  fprintf(stderr, "\n");
  fprintf(stderr, "oyranos-xforms-fltk v%d.%d.%d %s\n",
                          OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C,
                                _("is a Oyranos o(X)FORMS options tool"));
  fprintf(stderr, "%s\n",                 _("Usage"));
  fprintf(stderr, "  %s\n",               _("General options:"));
  fprintf(stderr, "      -v  %s\n",       _("verbose"));
  fprintf(stderr, "      -i \"xhtml_file\"  %s\n",_("read XFORMS"));
  fprintf(stderr, "\n");
  fprintf(stderr, "%s", _("For more informations read the man page:"));
  fprintf(stderr, "      man oyranos-xforms\n");
}

void callback_done( Fl_Widget * w, void * )
{
  w->window()->hide();
}

void callback_help_view( oyPointer * ptr, const char * help_text )
{
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

      char * string = oyStringCopy_( help_text, malloc );
      int n = strlen(string);
      
      while( string[i] )
      {
        c = string[i];
        if(c == '\n')  /* line break */
        {
          sprintf( &text[ti], "<br>" );
          ti += 4;
        } else
        {
          text[ti] = c;
          ++ti;
        }
        ++i;
      }
      text[ti] = 0;

      help_view->value( (const char *)text );
      if(string) free(string); string = 0;
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
}

int main (int argc, char ** argv)
{
  int output_model = 0;
  const char * input_xml_file = 0;
  const char * output_model_file = 0,
             * result_xml = 0;
  char * text = 0, * t = 0;
  oyFormsArgs_s * forms_args = oyFormsArgs_New( 0 );
  char ** other_args = 0;
  int other_args_n = 0;
  int error = 0;
  int print = 1;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyI18NInit_();

  Fl::scheme("plastic");


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
                        if(oy_debug) fprintf(stderr, #opt "=%s\n",opt)

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
              case 'o': OY_PARSE_STRING_ARG( output_model_file ); break;
              case 'O': output_model = 1; print = 0; break;
              case 'i': OY_PARSE_STRING_ARG( input_xml_file ); break;
              case 'v': oy_debug += 1; break;
              case 'h': /* only for compatibility with cmd line */ break;
              case 'l': /* only for compatibility with cmd line */ break;
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
              case '?':
              default:
                        fprintf(stderr, "%s -%c\n", _("Unknown argument"), argv[pos][i]);
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
        fprintf(stderr, "%s %s\n", _("wrong argument to option:"), wrong_arg);
        exit(1);
      }
      ++pos;
    }
    if(oy_debug) fprintf(stderr,  "%s\n", argv[1] );

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

  forms_args->print = print;

  /* get Layout file */
  if(input_xml_file)
  {
    size_t size = 0;
    text = oyReadFileToMem_(input_xml_file, &size, oyAllocateFunc_);
  }

  if(oy_debug)
    fprintf(stderr, "%s\n", text);

  Fl_Double_Window * w = new Fl_Double_Window(400,475,_("XFORMS in FLTK"));
    oyCallback_s callback = {oyOBJECT_CALLBACK_S, 0,0,0,
                                  (void(*)())callback_help_view,0};
    Fl_Group* o = new Fl_Group(0, 340, 400, 100);
      Fl_Help_View * help_view = new Fl_Help_View( 0,340,400,100 );
      help_view->box(FL_ENGRAVED_BOX);
      help_view->color(FL_BACKGROUND_COLOR);
      //help_view->align(FL_ALIGN_LEFT);
      help_view->selection_color(FL_DARK1);
      help_view->value("");
      callback.data = help_view;
    o->end(); // Fl_Group* o
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

  if(print)
  {
    w->show(1, argv);
    Fl::run();
  }


  result_xml = oyFormsArgs_ModelGet( forms_args );
  if(output_model_file)
  {
    if(result_xml)
      oyWriteMemToFile_( output_model_file, result_xml, strlen(result_xml) );
    else
      fprintf( stderr, "%s\n", "no model found" );
  }
  else if(output_model)
  {
    if(result_xml)
      printf( "%s\n", result_xml );
    else
      fprintf( stderr, "%s\n", "no model found" );
  }

  oyFormsArgs_Release( &forms_args );

  if(text) free(text); text = 0;

  return error;
}


