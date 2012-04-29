/**
 *  Oyranos is an open source Colour Management System 
 * 
 *  @par Copyright:
 *            2009-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/02/19
 *
 *  The given example displays a 8/16-bit PPM or float PFM files on screen.
 */

#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <oyranos.h>
#include <oyranos_alpha.h>
#include <oyranos_cmm.h>   /* for hacking into module API */

#define DEBUG 1

#include "../../oyranos_logo.h"
#include "oyranos_display_helper_classes_fltk.cpp" // observer callback
#include "Oy_Fl_Group.h"
#include "Oy_Fl_Shader_Box.h"
#include "Oy_Fl_Image_Box.h"

#include <FL/Fl_Tile.H>
#include <FL/Fl_Menu_Button.H>

#ifdef USE_GETTEXT
#include "config.h" /* I18N */
#include "fl_i18n/fl_i18n.H"
#else
#define _(text) text
#endif


extern "C" {
/* forward declaration of oyranos_alpha.c */
int                oyWriteMemToFile_ ( const char*, const void*, size_t );
char* oyReadFileToMem_  (const char* fullFileName, size_t *size,
                         oyAlloc_f     allocate_func);
}


Oy_Fl_Double_Window * createWindow (Oy_Fl_Image_Widget ** oy_box, uint32_t flags);
void setWindowMenue                  ( Oy_Fl_Double_Window*win,
                                       Oy_Fl_Image_Widget* oy_box,
                                       oyFilterNode_s    * node );

int
main(int argc, char** argv)
{
  // activate FLTK thread support
  Fl::lock();

  /* some Oyranos types */

  oyFilterNode_s * icc;
  int file_pos = 1;
  const char * file_name = NULL;


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

  int gl_box = 1;

  /* handle arguments */
  for(int i = 1; i < argc; ++i)
  {
    if(argc > 1 && strcmp(argv[i], "-v") == 0)
    {
      oy_display_verbose = 1;
      ++file_pos;
    }
    if(argc > 1 && strcmp(argv[i], "--use-no-gl") == 0)
    {
      gl_box = 0;
      ++file_pos;
    }
    if(argc > 1 && (strcmp(argv[i], "--help") == 0 ||
       strcmp(argv[i], "-h") == 0 ) )
    {
      printf("Usage: image_display --use-no-gl   use normal pixel copy\n" );
      exit(0);
    }
  }
  if(argc > file_pos && argv[file_pos])
    file_name = argv[file_pos];
  else
    file_name = "../../oyranos_logo.ppm";


  /* setup the drawing box */
  Oy_Fl_Shader_Box * oy_gl_box = 0;
  Oy_Fl_Image_Box * oy_box = 0;
  Oy_Fl_Image_Widget * oy_widget = 0;
  Oy_Fl_Double_Window * win = createWindow( &oy_widget, gl_box );
  if(oy_widget)
  {
    if(gl_box)
    {
      oy_gl_box = dynamic_cast<Oy_Fl_Shader_Box*> (oy_widget);
      icc = oy_gl_box->setImage( file_name );
    } else
    {
      oy_box = dynamic_cast<Oy_Fl_Image_Box*> (oy_widget);
      icc = oy_box->setImage( file_name );
    }
  }
  if(icc)
  {
    setWindowMenue( win, oy_widget, icc  );
    /* observe the node */
    oy_widget->observeICC( icc, conversionObserve );
    win->label( file_name );
  }


  win->show();

  Fl::run();

  return 0;
}

#include <FL/Fl_Image.H>
static Fl_RGB_Image image_oyranos_logo(oyranos_logo, 64, 64, 4, 0);

struct box_n_opts {
  oyFilterNode_s * node;
  Oy_Fl_Image_Widget * box;
};

void
callback ( Fl_Widget* w, void* daten )
{
  struct box_n_opts * arg = (box_n_opts*) daten;
  oyStruct_s * object = (oyStruct_s*) arg->node;

  if(!w->parent())
    printf("Could not find parents.\n");
  else
  if(!object)
    printf("Oyranos argument missed.\n");
  else
  if(object->type_ == oyOBJECT_FILTER_NODE_S)
  {
    oyFilterNode_s * node = (oyFilterNode_s*) object;
    oyOptions_s * opts = 0,
                * new_opts = 0;
    const char * tmp_dir = getenv("TMPDIR"),
               * in_text = 0,
               * model = 0;
    char * command = new char [1024];
    char * t = 0;
    int error = 0;
    char * ui_text = 0, ** namespaces = 0;

    if(!tmp_dir)
      tmp_dir = "/tmp";

    error = oyFilterNode_UiGet( node, &ui_text, &namespaces, malloc );

    opts = oyFilterNode_OptionsGet( node, OY_SELECT_FILTER );
    model = oyOptions_GetText( opts, oyNAME_NAME );
    in_text= oyXFORMsFromModelAndUi( model, ui_text, (const char**)namespaces,0,
                                     malloc );
    /* export the options values */
    sprintf( command, "%s/image_display_in_tmp.xml", tmp_dir );
    oyWriteMemToFile_( command, in_text, strlen(in_text) );
    in_text = 0; command[0] = 0;

    /* render the options to the UI */
    sprintf(command, "oyranos-xforms-fltk " );
    sprintf(&command[strlen(command)],
            " -i %s/image_display_in_tmp.xml -o %s/image_display_tmp.xml",
            tmp_dir, tmp_dir );
    error = system(command);
    if(error)
      fprintf(stderr, "error %d for \"%s\"", error, command );

    /* reload changed options */
    t = strstr(command," -o ");
    size_t size = 0;
    char * opts_text = oyReadFileToMem_(t+4, &size, malloc);
    new_opts = oyOptions_FromText( opts_text, 0, 0 );
    free(opts_text);
    oyOptions_CopyFrom( &opts, new_opts, oyBOOLEAN_UNION, oyFILTER_REG_NONE, 0);

    /* TODO update the conversion context and enforce a redraw
     *  The context of a node can be removed to allow for updating.
     *  A redrawing flag should be obtainable from the graph.
     */
    arg->box->damage( FL_DAMAGE_USER1 );

    delete [] command;
  }
  else
    printf("could not find a suitable program structure\n");
}

Oy_Fl_Double_Window * createWindow (Oy_Fl_Image_Widget ** oy_box, uint32_t flags)
{
  int w = 640,
      h = 480;


  Fl::get_system_colors();
  Oy_Fl_Double_Window *win = new Oy_Fl_Double_Window( w, h+100, TARGET );
  { Fl_Tile* t = new Fl_Tile(0,0, w, h+100);
      if(flags & 1)
        *oy_box = new Oy_Fl_Shader_Box(0,0,w,h);
      else
        *oy_box = new Oy_Fl_Image_Box(0,0,w,h);
      (*oy_box)->box(FL_FLAT_BOX);
      t->resizable(*oy_box);

      {
        oyProfile_s * e = NULL; /* default: sRGB */
        Oy_Fl_Group * og = new Oy_Fl_Group(0, h, w, 100, e);
        int gh = h;
        oyProfile_Release( &e );
        {
          /* add some text */
          Fl_Box *box = new Fl_Box(0,0+gh,w,100, "Oyranos");
            box->labeltype(FL_ENGRAVED_LABEL);
            box->labelfont(0);
            box->labelsize(48);
            box->labelcolor(FL_FOREGROUND_COLOR);
            box->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
          box->box(FL_FLAT_BOX);
          og->resizable(box);
        }
        /* place the colour managed logo */
        {
          Fl_Box* o = new Fl_Box(16, 16+gh, 64, 64);
            o->image(image_oyranos_logo);
            o->color(FL_RED);
            o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
        }
        og->end();
      }
    t->end();
    Fl_Group::current()->resizable(t);
  }
  win->end();

  return win;
}

void setWindowMenue                  ( Oy_Fl_Double_Window * win,
                                       Oy_Fl_Image_Widget  * oy_box,
                                       oyFilterNode_s    * node )
{
  struct box_n_opts * arg = new box_n_opts;

  win->begin();
      Fl_Menu_Button  *menue_;
      Fl_Menu_Button  *menue_button_;
      menue_button_ = new Fl_Menu_Button(0,0,win->w(),win->h(),0);
      menue_button_->type(Fl_Menu_Button::POPUP3);
      menue_button_->box(FL_NO_BOX);
      menue_button_->clear();
      menue_ = new Fl_Menu_Button(0,0,win->w(),win->h(),""); menue_->hide();
      arg->node = node;
      arg->box = oy_box;
      menue_->add( _("Edit Options"),
                   FL_CTRL + 'e', callback, (void*)arg, 0 );
      menue_button_->copy(menue_->menu());
  win->end();
}

