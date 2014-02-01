/**
 *  Oyranos is an open source Color Management System 
 * 
 *  @par Copyright:
 *            2009-2013 (C) Kai-Uwe Behrmann
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

#include "oyranos_config_internal.h" /* I18N */
#include "oyranos_i18n.h"

#include <oyranos.h>
#include <oyranos_cmm.h>   /* for hacking into module API */

#include <oyConversion_s.h>

#define DEBUG 1

extern "C" {
#include "GLee.h"
}

#include "../../oyranos_logo.h"
#include "oyranos_display_helper_classes_fltk.cpp" // observer callback
#include "Oy_Fl_Group.h"
#include "Oy_Fl_GL_Box.h"
#include "Oy_Fl_Shader_Box.h"
#include "Oy_Fl_Image_Box.h"

#include <FL/Fl_Tile.H>
#include <FL/Fl_Menu_Button.H>

oyConversion_s * idcc = 0;
Oy_Fl_Image_Widget * oy_widget = 0;

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

void jobResultRepeatCb(void*)
{
  oyJobResult();
  Fl::repeat_timeout(0.02, (void(*)(void*))jobResultRepeatCb);
}

/* keep at least one node around to handle options */
static oyFilterNode_s * icc = 0;

int
main(int argc, char** argv)
{
  // activate FLTK thread support
  Fl::lock();

  /* some Oyranos types */

  int file_pos = 1;
  const char * file_name = NULL;

  int gl_box = 0x01;
  int logo = 0x02;
  const char * module_name = 0;
  const char * clut_name = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyI18NInit_();

  /* handle arguments */
  for(int i = 1; i < argc; ++i)
  {
    if(argc > 1 && strcmp(argv[i], "-v") == 0)
    {
      oy_display_verbose = 1;
      ++file_pos;
    }
    if(argc > 1 && strcmp(argv[i], "--no-logo") == 0)
    {
      logo = 0;
      ++file_pos;
    }
    if(argc > 1 && strcmp(argv[i], "--use-no-gl") == 0)
    {
      gl_box = 0;
      ++file_pos;
    }
    if(argc > 2 && strcmp(argv[i], "--module") == 0)
    {
      module_name = argv[i+1];
      ++file_pos;
      ++file_pos;
    }
    if(argc > 2 && strcmp(argv[i], "--shader") == 0)
    {
      gl_box = 0x04;
      clut_name = argv[i+1];
      ++file_pos;
      ++file_pos;
    }
    if(argc > 1 && (strcmp(argv[i], "--help") == 0 ||
       strcmp(argv[i], "-h") == 0 ) )
    {
      printf("Usage: image_display [options] <image_file>\n"
             "\t--use-no-gl\tuse normal pixel copy\n"
             "\t--no-logo\tskip Oyranos logo\n"
             "\t--module <name>\tselect a Oyranos wrapped CMM\n"
             "\t--shader <file>\tset a CLUT from PPM image for color transform\n"
             "\t-v\t\tprint verbosely\n"
             "\t--help\t\tprint this help text\n"
            );
      exit(0);
    }
  }
  if(argc > file_pos && argv[file_pos])
    file_name = argv[file_pos];
  else
    file_name = "../../oyranos_logo.ppm";


  /* setup the drawing box */
  Oy_Fl_Shader_Box * oy_shader_box = 0;
  Oy_Fl_GL_Box * oy_gl_box = 0;
  Oy_Fl_Image_Box * oy_box = 0;
  Oy_Fl_Double_Window * win = createWindow( &oy_widget, gl_box | logo );
  int error = 0;
  if(oy_widget)
  {
    if(gl_box)
    {
      if(gl_box & 0x04)
      {
        oy_shader_box = dynamic_cast<Oy_Fl_Shader_Box*> (oy_widget);
        error = oy_shader_box->setImage( file_name, module_name, NULL, clut_name );
        if(!error)
          fprintf(stderr, "setImage fine\n");
        else
          fprintf(stderr, "setImage failed: %s\n", clut_name);
      } else
      {
        oy_gl_box = dynamic_cast<Oy_Fl_GL_Box*> (oy_widget);
        icc = oy_gl_box->setImage( file_name, module_name, NULL );
      }
    } else
    {
      oy_box = dynamic_cast<Oy_Fl_Image_Box*> (oy_widget);
      icc = oy_box->setImage( file_name, module_name, NULL );
    }
  }
  if(icc || (gl_box & 0x04 && !error))
  {
    setWindowMenue( win, oy_widget, icc  );
    /* observe the node */
    if(icc)
      oy_widget->observeICC( icc, conversionObserve );
    win->label( file_name );
  }

  win->show();

  Fl::add_timeout(0.02, (void(*)(void*))jobResultRepeatCb);
  Fl::run();

  return 0;
}

#include <FL/Fl_Image.H>
static Fl_RGB_Image image_oyranos_logo(oyranos_logo, 64, 64, 4, 0);

struct box_n_opts {
  oyFilterNode_s * node;
  Oy_Fl_Image_Widget * box;
};

#include "../../liboyranos_core/oyranos_threads.c"
int changeIccOptionsUpdate ( oyJob_s * job )
{
  struct box_n_opts * arg = (box_n_opts*) job->context;

#if 0
  ((Fl_Widget*)arg->box)->damage(FL_DAMAGE_ALL,arg->box->x(),arg->box->y(),arg->box->w(),arg->box->h());
  arg->box->damage_resize(arg->box->x(),arg->box->y(),arg->box->w(),arg->box->h());
#else
  arg->box->deactivate();
  arg->box->activate();
#endif
  return 0;
}

extern "C" {
int changeIccOptions ( oyJob_s * job )
{
  struct box_n_opts * arg = (box_n_opts*) job->context;
  oyStruct_s * object = (oyStruct_s*) arg->node;

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

    error = oyFilterNode_GetUi( node, &ui_text, &namespaces, malloc );

    opts = oyFilterNode_GetOptions( node, OY_SELECT_FILTER |
                                          oyOPTIONATTRIBUTE_ADVANCED );
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
  return 0;
}
void jobCallback                     ( double              progress_zero_till_one,
                                       char              * status_text,
                                       int                 thread_id_,
                                       int                 job_id )
{ printf( "%s():%d %02f %s %d/%d\n",__func__,__LINE__,progress_zero_till_one,
          status_text?status_text:"",thread_id_,job_id); }
}

void callback ( Fl_Widget* w, void* daten )
{
  struct box_n_opts * arg = (box_n_opts*) daten;
  oyStruct_s * object = (oyStruct_s*) arg->node;

  if(!w->parent())
    printf("Could not find parents.\n");
  else
  if(!object)
    printf("Oyranos argument missed.\n");
  else
  if(object && object->type_ == oyOBJECT_FILTER_NODE_S)
  {
    oyJob_s * job = (oyJob_s*) calloc(sizeof(oyJob_s),1);
    job->work = changeIccOptions;
    job->finish = changeIccOptionsUpdate;
    job->context = (oyStruct_s*)daten;
    job->cb_progress = jobCallback;
    oyJob_Add(job, 0);
    job = NULL;
  } else
    printf("could not find a suitable program structure\n");
}

void view_cb ( Fl_Widget* w, void* daten )
{
  struct box_n_opts * arg = (box_n_opts*) daten;
  oyStruct_s * object = (oyStruct_s*) arg->node;

  if(!w->parent())
    printf("Could not find parents.\n");
  else
  if(!object)
    printf("Oyranos argument missed.\n");
  else
  if(object && object->type_ == oyOBJECT_FILTER_NODE_S)
  {
    int error = 0;
    oyConversion_s * cc = arg->box->conversion();
    oyFilterNode_s * in = oyConversion_GetNode( cc, OY_INPUT);
    oyOptions_s * opts =  oyFilterNode_GetOptions( in, 0 );
    const char * fn =     oyOptions_FindString( opts, "//" OY_TYPE_STD "/file_read/filename", 0 );
    char * command = new char [1024];

    if(!fn)
    {
      oyImage_s * image = oyConversion_GetImage( cc, OY_INPUT );
      opts = oyImage_GetTags(image);
      fn =   oyOptions_FindString( opts, "//" OY_TYPE_STD "/file_read/filename", 0 );
      oyImage_Release( &image );
    }

    /* export the options values */
    sprintf( command, "iccexamin -g \"%s\" &", fn );

    error = system(command);
    if(error)
      fprintf(stderr, "error %d for \"%s\"", error, command );

    oyFilterNode_Release( &in );
    oyOptions_Release( &opts );
    if(command) delete [] command;
  }
  else
    printf("could not find a suitable program structure\n");
}
void dbg_cb ( Fl_Widget* w, void* daten )
{
  struct box_n_opts * arg = (box_n_opts*) daten;
  oyStruct_s * object = (oyStruct_s*) arg->node;

  if(!w->parent())
    printf("Could not find parents.\n");
  else
  if(!object)
    printf("Oyranos argument missed.\n");
  else
  if(object && object->type_ == oyOBJECT_FILTER_NODE_S)
  {
    if(idcc)
      oyConversion_Release( &idcc );
    idcc = arg->box->conversion();
    int oy_debug_old = oy_debug;
    oy_debug = 1;
    oyShowConversion_(idcc, 0);
    oy_debug = oy_debug_old;
    oyImage_s * image = oyConversion_GetImage( idcc, OY_INPUT );
    oyImage_WritePPM( image, "debug_image.ppm", "image_display input image");
    oyImage_Release( &image );
  }
}

void exit_cb ( Fl_Widget* w, void* daten ) {exit(0);}

Oy_Fl_Double_Window * createWindow (Oy_Fl_Image_Widget ** oy_box, uint32_t flags)
{
  int w = 640,
      h = 480,
      lh = 0;

  if(flags & 0x02)
    lh = 100;


  Fl::get_system_colors();
  Oy_Fl_Double_Window *win = new Oy_Fl_Double_Window( w, h+lh, "Oyranos" );
  { Fl_Tile* t = new Fl_Tile(0,0, w, h+lh);
      if(flags & 0x04)
        *oy_box = new Oy_Fl_Shader_Box(0,0,w,h);
      else if(flags & 0x01)
        *oy_box = new Oy_Fl_GL_Box(0,0,w,h);
      else
        *oy_box = new Oy_Fl_Image_Box(0,0,w,h);
      (*oy_box)->box(FL_FLAT_BOX);
      t->resizable(*oy_box);

      {
        oyProfile_s * e = NULL; /* default: sRGB */
        Oy_Fl_Group * og = new Oy_Fl_Group(0, h, w, lh, e);
        int gh = h;
        oyProfile_Release( &e );
        if(flags & 0x02)
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
        /* place the color managed logo */
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

oyOptions_s * findOpts( const char * filter_name )
{
  oyFilterGraph_s * g = oyFilterGraph_FromNode( icc, 0 );
  oyFilterNode_s * n = oyFilterGraph_GetNode( g, -1, filter_name, NULL );
  oyOptions_s * opts = oyFilterNode_GetOptions( n, 0 );
  oyFilterGraph_Release( &g );
  oyFilterNode_Release( &n );

  return opts;
}

int
event_handler(int e)
{
  int found = 0;
  oyOptions_s * opts;
  static double scale_changer = 2.0;

  switch (e)
  {
  case FL_SHORTCUT:
      if(Fl::event_key() == FL_Escape)
      {
        exit(0);
        found = 1;
      } else
      if(Fl::event_key() == 'q'
       && Fl::event_state() == FL_CTRL)
     {
        exit(0);
        found = 1;
      }
    case FL_KEYBOARD:
    {
      int k = ((char*)Fl::event_text())[0];
      double scale = 1.0;

      switch (k)
      {
      case '-':
        found = 1;
        fprintf(stderr, "event_handler -\n" );
        opts = findOpts( "//" OY_TYPE_STD "/scale" );
        oyOptions_FindDouble( opts,
                                   "scale",
                                   0, &scale );
        scale /= scale_changer;
        oyOptions_SetFromDouble( &opts,
                                   "//" OY_TYPE_STD "/scale/scale",
                                   scale, 0, OY_CREATE_NEW );
        oyOptions_Release( &opts );
        break;
      case '+': // 43
        found = 1;
        fprintf(stderr, "event_handler +\n" );
        opts = findOpts( "//" OY_TYPE_STD "/scale" );
        oyOptions_FindDouble( opts,
                                   "scale",
                                   0, &scale );
        scale *= scale_changer;
        oyOptions_SetFromDouble( &opts,
                                   "//" OY_TYPE_STD "/scale/scale",
                                   scale, 0, OY_CREATE_NEW );
        oyOptions_Release( &opts );
        break;
      case '*':
        found = 1;
        scale_changer += (scale_changer-1.0)*.2;
        break;
      case '/':
        found = 1;
        scale_changer -= (scale_changer-1.0)*.2;
        break;
      case '0':
        {
          oyConversion_s * cc = oy_widget->conversion();
          oyImage_s * image = oyConversion_GetImage( cc, OY_INPUT );
          double widget_width = oy_widget->w(),
                 widget_height = oy_widget->h(),
                 image_height = oyImage_GetHeight( image ),
                 image_width = oyImage_GetWidth( image );
          if(widget_width/image_width < widget_height/image_height)
            scale = widget_width/image_width;
          else
            scale = widget_height/image_height;
          /* reset position to zero */
          oy_widget->px = 0;
          oy_widget->py = 0;
        }
        found = 1;
        fprintf(stderr, "event_handler +\n" );
        opts = findOpts( "//" OY_TYPE_STD "/scale" );
        oyOptions_SetFromDouble( &opts,
                                   "//" OY_TYPE_STD "/scale/scale",
                                   scale, 0, OY_CREATE_NEW );
        oyOptions_Release( &opts );
        break;
      case '1':
        scale = 1.0;
        found = 1;
        fprintf(stderr, "event_handler +\n" );
        opts = findOpts( "//" OY_TYPE_STD "/scale" );
        oyOptions_FindDouble( opts,
                                   "scale",
                                   0, &scale );
        scale = 1.0;
        oyOptions_SetFromDouble( &opts,
                                   "//" OY_TYPE_STD "/scale/scale",
                                   scale, 0, OY_CREATE_NEW );
        oyOptions_Release( &opts );
        break;
      default:
        break;
      }
    }
    break;
  default: 
    break;
  }
  
  return found;
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
      if(node)
      {
        menue_->add( _("Edit Options ..."),
                   FL_CTRL + 'e', callback, (void*)arg, 0 );
        menue_->add( _("Examine ICC Profile ..."),
                   FL_CTRL + 'i', view_cb, (void*)arg, 0 );
        menue_->add( _("Debug"),
                   FL_CTRL + 'd', dbg_cb, (void*)arg, 0 );
      }
      menue_->add( _("Quit"),
                   FL_CTRL + 'q', exit_cb, (void*)arg, 0 );
      menue_button_->copy(menue_->menu());

      Fl::add_handler(event_handler);
  win->end();
}

