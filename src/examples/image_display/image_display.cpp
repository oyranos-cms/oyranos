/**
 *  Oyranos is an open source Color Management System 
 * 
 *  @par Copyright:
 *            2009-2016 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
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
#include "oyranos_sentinel.h" /* oyInit_() */

#include <oyranos.h>
#include <oyranos_cmm.h>   /* for hacking into module API */
#include <oyranos_io.h>    /* oyFindApplication() */
#include "oyranos_string.h"
#include "oyranos_threads.h"

#include <oyConversion_s.h>
#include <oyProfiles_s.h>

#ifdef DEBUG
#undef DEBUG
#endif
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
Oy_Fl_Double_Window * win = 0;
Oy_Fl_Group * lg = 0;

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
const char * clut_name = 0;

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
  const char * icc_color_context = 0;
  int i;
  oyOptions_s * module_options = NULL;
  uint32_t icc_profile_flags = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyExportStart_(EXPORT_CHECK_NO);


/* workaround to initialise OpenMP in order to get code compiled under osX 10.6.8 */
#pragma omp parallel for private(i)
  for(i = 0; i < 1; ++i)
    file_pos = 1;

  /* handle arguments */
  for(i = 1; i < argc; ++i)
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
    if(argc > 1 && strcmp(argv[i], "--use-pixel") == 0)
    {
      gl_box = 0;
      ++file_pos;
    }
    if(argc > 2 && strcmp(argv[i], "--icc-color-context") == 0)
    {
      icc_color_context = argv[i+1];
      oyOptions_SetFromString( &module_options, OY_DEFAULT_CMM_CONTEXT,
                             icc_color_context, OY_CREATE_NEW );
      icc_profile_flags = oyICCProfileSelectionFlagsFromRegistration( icc_color_context );
      ++file_pos;
      ++file_pos;
    }
    if(argc > 2 && strcmp(argv[i], "--effect") == 0)
    {
      oyOptions_SetFromString( &module_options, OY_DEFAULT_EFFECT,
                             "1", OY_CREATE_NEW );
      oyProfiles_s * effects = oyProfiles_New(0);
      oyProfile_s * p = oyProfile_FromName( argv[i+1], icc_profile_flags, 0 );
                        oyProfiles_MoveIn( effects, &p, -1 );
      oyOptions_MoveInStruct( &module_options,
                                      OY_PROFILES_EFFECT,
                                       (oyStruct_s**) &effects,
                                       OY_CREATE_NEW );
      /*oyOptions_SetFromString( &module_options, OY_DEFAULT_EFFECT_PROFILE,
                             argv[i+1], OY_CREATE_NEW );*/
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
             "\t--use-pixel\tuse normal pixel copy\n"
             "\t--no-logo\tskip Oyranos logo\n"
             "\t--icc-color-context <name>\tselect a Oyranos wrapped context CMM\n"
             "\t--effect <name>\tselect a effect profile\n"
             "\t--shader <file>\tset a CLUT from PPM image for color transform\n"
             "\t-v\t\tprint verbosely\n"
             "\t--help\t\tprint this help text\n"
            );
      exit(0);
    }
  }

  if(oy_debug)
  for(i = 0; i < argc; ++i)
    fprintf(stderr, "argv[%d] = %s\n", i, argv[i] );

  if(argc > file_pos && argv[file_pos])
    file_name = argv[file_pos];
  else
    file_name = "../../oyranos_logo.ppm";


  /* setup the drawing box */
  Oy_Fl_Shader_Box * oy_shader_box = 0;
  Oy_Fl_GL_Box * oy_gl_box = 0;
  Oy_Fl_Image_Box * oy_box = 0;
  win = createWindow( &oy_widget, gl_box | logo );
  int error = 0;
  if(oy_widget)
  {
    if(gl_box)
    {
      if(gl_box & 0x04)
      {
        oy_shader_box = dynamic_cast<Oy_Fl_Shader_Box*> (oy_widget);
        error = oy_shader_box->setImage( file_name, module_options, clut_name );
        if(!error)
          fprintf(stderr, "setImage fine\n");
        else
          fprintf(stderr, "setImage failed: %s\n", clut_name);
      } else
      {
        oy_gl_box = dynamic_cast<Oy_Fl_GL_Box*> (oy_widget);
        icc = oy_gl_box->setImage( file_name, module_options );
      }
    } else
    {
      oy_box = dynamic_cast<Oy_Fl_Image_Box*> (oy_widget);
      icc = oy_box->setImage( file_name, module_options );
    }
  }
  setWindowMenue( win, oy_widget, icc  );
  if(icc || (gl_box & 0x04 && !error))
  {
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

int changeIccOptionsUpdate ( oyJob_s * job )
{
  oyBlob_s * context = (oyBlob_s *) job->context;
  struct box_n_opts * arg = (box_n_opts*) oyBlob_GetPointer( context );

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
  oyBlob_s * context = (oyBlob_s *) job->context;
  struct box_n_opts * arg = (box_n_opts*) oyBlob_GetPointer( context );
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

    error = oyFilterNode_GetUi( node, 0, &ui_text, &namespaces, malloc );

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
    {
      const char * uiRenderer = getenv("OY_OFORMS_RENDERER");
      if(uiRenderer && uiRenderer[0])
        sprintf(command, "%s ", uiRenderer );
      else
      if((uiRenderer = getenv("OY_XFORMS_RENDERER")) != NULL && uiRenderer[0])
        sprintf(command, "%s ", uiRenderer );
      else
        sprintf(command, "oyranos-xforms-fltk " );
    }
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
                                       int                 job_id,
                                       oyStruct_s        * cb_progress_context OY_UNUSED )
{ printf( "%s():%d %02f %s %d/%d\n",__func__,__LINE__,progress_zero_till_one,
          status_text?status_text:"",thread_id_,job_id); }
}

void callback ( Fl_Widget* w, void* daten )
{
  struct box_n_opts * arg = (box_n_opts*) daten;
  oyStruct_s * object = (oyStruct_s*) arg->node;
  oyBlob_s * context = oyBlob_New(0);
  oyBlob_SetFromStatic( context, arg, 0, "struct box_n_opts" );

  if(!w->parent())
    printf("Could not find parents.\n");
  else
  if(!object)
    printf("Oyranos argument missed.\n");
  else
  if(object && object->type_ == oyOBJECT_FILTER_NODE_S)
  {
    oyJob_s * job = oyJob_New(0);
    job->work = changeIccOptions;
    job->finish = changeIccOptionsUpdate;
    job->context = (oyStruct_s*)context;
    job->cb_progress = jobCallback;
    oyJob_Add(&job, 0, 0);
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
    oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );
  }
}
Oy_Fl_Double_Window * make_help(const char * title, const char * text, int is_html)
{
  Oy_Fl_Double_Window * help_window=(Oy_Fl_Double_Window *)0;
  Fl_Text_Display     * text_display=(Fl_Text_Display *)0;
  Fl_Help_View        * help_browser=(Fl_Help_View *)0;
  { help_window = new Oy_Fl_Double_Window(505, 410, title?title:_("Information:"));
    help_window->box(FL_FLAT_BOX);
    help_window->color(FL_BACKGROUND_COLOR);
    help_window->selection_color(FL_BACKGROUND_COLOR);
    help_window->labeltype(FL_NO_LABEL);
    help_window->labelfont(0);
    help_window->labelsize(14);
    help_window->labelcolor(FL_FOREGROUND_COLOR);
    help_window->align(Fl_Align(FL_ALIGN_TOP));
    help_window->when(FL_WHEN_RELEASE);
    if(is_html)
    { help_browser = new Fl_Help_View(0, 0, 505, 410);
      help_browser->box(FL_THIN_UP_BOX);
      help_browser->color((Fl_Color)16);
      Fl_Group::current()->resizable(help_browser);
    } // Fl_Help_View* help_browser
    else
    { Fl_Text_Display* o = text_display = new Fl_Text_Display(0, 0, 505, 410);
      Fl_Text_Buffer * buffer = new Fl_Text_Buffer(0);
      o->buffer( buffer );
      //o->textfont( FL_COURIER );
      o->box(FL_THIN_UP_BOX);
      //o->box( FL_FLAT_BOX );
      o->color(FL_BACKGROUND_COLOR);
      Fl_Group::current()->resizable(o);
    }
    help_window->end();
  } // Oy_Fl_Double_Window* help_window
  help_window->show();
  if(text)
  {
    if(is_html)
      help_browser->value( text );
    else
      text_display->buffer()->text( text );
  }
  return help_window;
}
void help_cb ( Fl_Widget*, void* )
{
  const char * opts[] = {"add_html_header","1",
                         "add_oyranos_title","1",
                         "add_oyranos_copyright","1",
                          NULL};
  int is_html = 1;
  make_help( _("Oyranos Help"), oyDescriptionToHTML(oyGROUP_ALL, opts,0), is_html );
}
void shortcuts_cb ( Fl_Widget*, void* )
{
  char * html = NULL;
  oyStringAddPrintf( &html,0,0,
  "<html><body>"
  "<h4>%s</h4>"
  "<ul>"
  "<li>%s</li>"
  "<li>%s</li>"
  "</ul>"
  "<h4>%s</h4>"
  "<ul>"
  "<li>%s</li>"
  "<li>%s</li>"
  "<li>%s</li>"
  "<li>%s</li>"
  "<li>%s</li>"
  "<li>%s</li>"
  "</ul>"
  "<h4>%s</h4>"
  "<ul>"
  "<li>%s</li>"
  "<li>%s</li>"
  "<li>%s</li>"
  "</ul>"
  "<h4>%s</h4>"
  "<ul>"
  "<li>%s</li>"
  "<li>%s</li>"
  "<li>%s</li>"
  "<li>%s</li>"
  "</ul>"
  "<h4>%s</h4>"
  "<ul>"
  "<li>%s</li>"
  "</ul>"
  "</body></html>",
  _("Navigation:"),
  /* '>' */
  _("&gt; - next image"),
  /* '<' */
  _("&lt; - previous image"),
  _("Zoom:"),
  _("f - fit to window"),
  _("1 - map one image pixel to one screen pixel"),
  _("w - fit to window width"),
  _("h - fit to window height"),
  _("+ - zoom in"),
  _("- - zoom out"),
  _("Exposure:"),
  _("Alt + - brighter"),
  _("Alt - - darker"),
  _("Alt . - reset to normal values"),
  _("Channels:"),
  _("Alt 1 - first channel"),
  _("Alt 2 - second channel"),
  _("Alt n - n-th channel"),
  _("Alt 0 - all channels"),
  _("View:"),
  _("Alt v - set window fullscreen")
  );
  int is_html = 1;
  make_help( _("Oyranos Image Display Help"),
  html,
  is_html );
}

void info_cb ( Fl_Widget* w, void* daten )
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
    oyConversion_s * cc = arg->box->conversion();
    oyFilterNode_s * in = oyConversion_GetNode( cc, OY_INPUT);
    oyOptions_s * opts =  oyFilterNode_GetOptions( in, 0 );
    const char * fn =     oyOptions_FindString( opts, "//" OY_TYPE_STD "/file_read/filename", 0 );
    char * text = NULL;
    const char * image_text = NULL;
    size_t size = 0;

    if(!fn)
    {
      oyImage_s * image = oyConversion_GetImage( cc, OY_INPUT );
      opts = oyImage_GetTags(image);
      fn =   oyOptions_FindString( opts, "//" OY_TYPE_STD "/file_read/filename", 0 );
      image_text = oyObject_GetName( image->oy_, oyNAME_NICK );
      oyImage_Release( &image );
    }

    if(oyFindApplication("tiffinfo"))
      text = oyReadCmdToMemf_( &size, "r", malloc, "tiffinfo \"%s\"", fn );
    if((!text || text[0] == 0) && oyFindApplication("file"))
      text = oyReadCmdToMemf_( &size, "r", malloc, "file \"%s\"", fn );
 
    oyStringAdd_( &text, "\n", oyAllocateFunc_, oyDeAllocateFunc_ );
    oyStringAdd_( &text, image_text, oyAllocateFunc_, oyDeAllocateFunc_ );


    oyFilterNode_Release( &in );
    oyOptions_Release( &opts );

    make_help( _("Oyranos Image Display Info"), text, 0 );
  }
  else
    printf("could not find a suitable program structure\n");
}

void exit_cb ( Fl_Widget* w OY_UNUSED, void* daten OY_UNUSED )
{
  oy_widget->conversion(NULL);
  oyFilterNode_Release( &icc );
  if(oy_debug_objects >= 0)
    oyObjectTreePrint( 0x01 | 0x02 );
  exit(0);
}

Oy_Fl_Double_Window * createWindow (Oy_Fl_Image_Widget ** oy_box, uint32_t flags)
{
  int w = 640,
      h = 480,
      lh = 1;

  if(flags & 0x02)
    lh = 100;


  Fl::get_system_colors();
  Oy_Fl_Double_Window *win = new Oy_Fl_Double_Window( w, h+lh, "Oyranos" );
  Fl_Tile* t = NULL;

  if(flags & 0x02)
    t = new Fl_Tile(0,0, w, h+lh);

  if(flags & 0x04)
    *oy_box = new Oy_Fl_Shader_Box(0,0,w,h);
  else if(flags & 0x01)
    *oy_box = new Oy_Fl_GL_Box(0,0,w,h);
  else
    *oy_box = new Oy_Fl_Image_Box(0,0,w,h);
  (*oy_box)->box(FL_FLAT_BOX);

  if(flags & 0x02)
  {
    t->resizable(*oy_box);
    oyProfile_s * e = NULL; /* default: sRGB */
    Oy_Fl_Group * og = lg = new Oy_Fl_Group(0, h, w, lh, e);
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
    t->end();
    Fl_Group::current()->resizable(t);
  }
  else
    win->resizable(*oy_box);

  win->end();

  return win;
}

oyOptions_s * findOpts( const char * filter_name )
{
  oyConversion_s * cc = oy_widget->conversion();
  oyFilterGraph_s * g = oyConversion_GetGraph( cc );
  oyFilterNode_s * n = oyFilterGraph_GetNode( g, -1, filter_name, NULL );
  oyOptions_s * opts = oyFilterNode_GetOptions( n, 0 );
  oyFilterGraph_Release( &g );
  oyFilterNode_Release( &n );

  return opts;
}

oyOption_s * findOpt( const char * filter_name, const char * key_name )
{
  oyConversion_s * cc = oy_widget->conversion();
  oyFilterGraph_s * g = oyConversion_GetGraph( cc );
  oyFilterNode_s * n = oyFilterGraph_GetNode( g, -1, filter_name, NULL );
  oyOptions_s * opts = oyFilterNode_GetOptions( n, 0 );
  oyOption_s * opt = oyOptions_Find( opts, key_name, oyNAME_PATTERN );
  oyOptions_Release( &opts );
  oyFilterGraph_Release( &g );
  oyFilterNode_Release( &n );

  return opt;
}

/* Prepare a JSON complying string for the channel filter.
 * The string describes the source of the output channels.
 * e.g.: '["b", "b", "b"]'
 */
void setChannel( oyProfile_s * p, int channel_pos )
{
  oyOptions_s * opts = findOpts( "//" OY_TYPE_STD "/channel" );
  char * channels = NULL;
  icColorSpaceSignature cs = (icColorSpaceSignature) oyProfile_GetSignature( p, oySIGNATURE_COLOR_SPACE );
  int count = oyICCColorSpaceGetChannelCount( cs ), i;


  if(channel_pos == 0 ||
     channel_pos > count)
    oyStringAddPrintf( &channels, 0,0, "" );
  else
  {
    oyStringAddPrintf( &channels, 0,0, "[" );
    for(i = 0; i < count; ++i)
    {
      if(i > 0)
        oyStringAddPrintf( &channels, 0,0, ", " );
      switch(cs)
      {
      case icSigLabData:
      case icSigLuvData:
      case icSigYCbCrData:
      case icSigYxyData:
      case icSigHsvData:
      case icSigHlsData:
        if(channel_pos-1 == i)
          oyStringAddPrintf( &channels, 0,0, "\"%c\"", 'a' + channel_pos - 1 );
        else
          oyStringAddPrintf( &channels, 0,0, "0.5" );
        break;
      default:
        oyStringAddPrintf( &channels, 0,0, "\"%c\"", 'a' + channel_pos - 1 );
      }
    }
    oyStringAddPrintf( &channels, 0,0, "]" );
  }

  oyOptions_SetFromString( &opts, "//" OY_TYPE_STD "/channel/channel", channels, OY_CREATE_NEW );
  oyDeAllocateFunc_( channels );
  oyOptions_Release( &opts );
}



char ** getFileList(const char * path, int * count, const char * file, int * pos )
{
  int i;
  char * fn = NULL;

  // Special case for file name with path. Then search in local "./" place.
  if(!path || path[0] == 0)
    path = ".";
  if(path && file && strlen(path) == 1 && path[0] == '.' &&
     strchr(file,OY_SLASH_C) == NULL && strchr(file,'\\') == NULL)
    oyStringAddPrintf( &fn, 0,0, "./%s", file );

  char ** files = oyGetFiles_( path, count );
  if(file && pos)
  for(i = 0; i < *count; ++i)
  {
    if(strcmp( files[i], fn?fn:file ) == 0)
    {
      *pos = i;
      break;
    }
  }

  return files;
}

const char * scale_reg = OY_INTERNAL OY_SLASH "scale/scale";
const char * scales_reg = OY_INTERNAL OY_SLASH "scale";
char * path = 0;
void               openNextImage     ( Oy_Fl_Image_Widget* oy_widget,
                                       int                 increment )
{
  int pos = 0, count = 0;
  oyConversion_s * cc = oy_widget->conversion();
  oyImage_s * image = oyConversion_GetImage( cc, OY_INPUT );
  oyOptions_s * tags = oyImage_GetTags( image );
  const char * filename = oyOptions_FindString( tags,
                                       "//" OY_TYPE_STD "/file_read/filename",
                                                        0 );
  if(!path)
    path  = oyExtractPathFromFileName_( filename );
  oyOptions_Release( &tags );

  char ** files = getFileList( path, &count, filename, &pos );

  oyImage_Release( &image );
  oyOptions_s * module_options = oyFilterNode_GetOptions( icc, 0 );

  Oy_Fl_GL_Box * oy_box = dynamic_cast<Oy_Fl_GL_Box*> (oy_widget);
  Oy_Fl_Shader_Box * oy_shader_box = dynamic_cast<Oy_Fl_Shader_Box*> (oy_widget);
  if(oy_box)
  {
    oyFilterNode_Release( &icc );
    for(int i = 0; i < count; ++i)
    {
      pos += increment;
      if(pos >= count)
        pos = 0;
      else if(pos < 0)
        pos = count - 1;

      if(strstr(files[pos],".xvpics") != NULL ||
         strstr(files[pos],".thumbnails") != NULL)
        continue;

      if(oy_display_verbose)
        fprintf( stderr, "open image %s %d/%d\n", files[pos], i, count  );
      icc = oy_box->setImage( files[pos], module_options );
      if(icc)
      {
        setWindowMenue( win, oy_widget, icc  );
        /* observe the node */
        oy_widget->observeICC( icc, conversionObserve );
        win->label( files[pos] );
        break;
      } else if(oy_display_verbose)
        fprintf( stderr, "could not open image %s; continuing with next. %d/%d\n", files[pos], i, count  );
      fflush( stderr );
      Fl::wait(0);
    }
  } else if(oy_shader_box)
  {
    for(int i = 0; i < count; ++i)
    {
      pos += increment;
      if(pos >= count)
        pos = 0;
      else if(pos < 0)
        pos = count - 1;

      if(strstr(files[pos],".xvpics") != NULL ||
         strstr(files[pos],".thumbnails") != NULL)
        continue;

      if(oy_display_verbose)
        fprintf( stderr, "open image %s %d/%d\n", files[pos], i, count  );
      int error = oy_shader_box->setImage( files[pos], module_options, clut_name );
      if(!error)
      {
        setWindowMenue( win, oy_widget, icc  );
        win->label( files[pos] );
        oy_shader_box->valid(0);
        break;
      } else if(oy_display_verbose)
        fprintf( stderr, "could not open image %s; continuing with next. %d/%d\n", files[pos], i, count  );
      fflush( stderr );
      Fl::wait(0);
    }
  }
  else
    fprintf(stderr, "need a GL box; skipping image change ...\n");

  oyOptions_Release( &module_options );
  oyStringListRelease_( &files, count, oyDeAllocateFunc_ );

  oy_widget->damage( FL_DAMAGE_USER1 );
}

void setFullscreen()
{
  static int fullscreen = 0, x,y,w,h;
  if(fullscreen)
  {
    oy_widget->window()->fullscreen_off(x,y,w,h);
    fullscreen = 0;
  } else
  {
    x = oy_widget->window()->x();
    y = oy_widget->window()->y();
    w = oy_widget->window()->w();
    h = oy_widget->window()->h();
    oy_widget->window()->fullscreen();
    fullscreen = 1;
  }
}

int
event_handler(int e)
{
  int found = 0;
  oyOptions_s * opts;

  switch (e)
  {
  case FL_SHORTCUT:
    {
      int k = Fl::event_key();
      double expose = 1.0;
      if(Fl::event_key() == FL_Escape)
      {
        exit(0);
        found = 1;
      }
      if(k == FL_F + 11)
        setFullscreen();
 
      if(Fl::event_state() & FL_ALT ||
         Fl::event_state() & FL_META)
      switch (k)
      {
      case 'q':
        exit(0);
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        {
          oyConversion_s * cc = oy_widget->conversion();
          oyImage_s * image = oyConversion_GetImage( cc, OY_INPUT );
          oyProfile_s * profile = oyImage_GetProfile( image );
          setChannel( profile, k - '0' );
          oyImage_Release( &image );
          oyProfile_Release( &profile );
        }
        found = 1;
        break;
      case '-':
        found = 1;
        opts = findOpts( "//" OY_TYPE_STD "/expose" );
        oyOptions_FindDouble( opts,
                                   "expose",
                                   0, &expose );
        expose /= M_SQRT2;
        oyOptions_SetFromDouble( &opts,
                                   "//" OY_TYPE_STD "/expose/expose",
                                   expose, 0, OY_CREATE_NEW );
        oyOptions_Release( &opts );
        oy_widget->damage( FL_DAMAGE_USER1 );
        break;
      case '+': // 43
        found = 1;
        opts = findOpts( "//" OY_TYPE_STD "/expose" );
        oyOptions_FindDouble( opts,
                                   "expose",
                                   0, &expose );
        expose *= M_SQRT2;
        oyOptions_SetFromDouble( &opts,
                                   "//" OY_TYPE_STD "/expose/expose",
                                   expose, 0, OY_CREATE_NEW );
        oyOptions_Release( &opts );
        oy_widget->damage( FL_DAMAGE_USER1 );
        break;
      case '.':
        found = 1;
        opts = findOpts( "//" OY_TYPE_STD "/expose" );
        oyOptions_FindDouble( opts,
                                   "expose",
                                   0, &expose );
        expose = 1.0;
        oyOptions_SetFromDouble( &opts,
                                   "//" OY_TYPE_STD "/expose/expose",
                                   expose, 0, OY_CREATE_NEW );
        oyOptions_Release( &opts );
        oy_widget->damage( FL_DAMAGE_USER1 );
        break;
      case 'v':
        setFullscreen();
        found = 1;
        break;
      }
    } OY_FALLTHROUGH
    case FL_KEYBOARD:
    {
      int k = ((char*)Fl::event_text())[0];
      double scale = 1.0;
      oyOption_s * opt = 0;

      if(!(Fl::event_state() & FL_ALT ||
           Fl::event_state() & FL_META))
      switch (k)
      {
      case '-':
        found = 1;
        opt = findOpt( scales_reg, scale_reg );
        scale = oyOption_GetValueDouble( opt, 0 );
        scale /= oy_widget->scale_changer;
        // scale relative to the middle of the image
        oy_widget->px = int((double)(oy_widget->px - oy_widget->w()/2) / oy_widget->scale_changer) + oy_widget->w()/2;
        oy_widget->py = int((double)(oy_widget->py - oy_widget->h()/2) / oy_widget->scale_changer) + oy_widget->h()/2;
        oy_widget->resetScale();
        oyOption_SetFromDouble( opt, scale, 0,0 );
        break;
      case '+': // 43
        found = 1;
        opt = findOpt( scales_reg, scale_reg );
        scale = oyOption_GetValueDouble( opt, 0 );
        scale *= oy_widget->scale_changer;
        // scale relative to the middle of the image
        oy_widget->px = int((double)(oy_widget->px - oy_widget->w()/2) * oy_widget->scale_changer) + oy_widget->w()/2;
        oy_widget->py = int((double)(oy_widget->py - oy_widget->h()/2) * oy_widget->scale_changer) + oy_widget->h()/2;
        oy_widget->resetScale();
        oyOption_SetFromDouble( opt, scale, 0,0 );
        break;
      case '*':
        found = 1;
        oy_widget->scale_changer += (oy_widget->scale_changer-1.0)*.2;
        break;
      case '/':
        found = 1;
        oy_widget->scale_changer -= (oy_widget->scale_changer-1.0)*.2;
        break;
      case 'h':
        opt = findOpt( scales_reg, scale_reg );
        {
          oyConversion_s * cc = oy_widget->conversion();
          oyImage_s * image = oyConversion_GetImage( cc, OY_INPUT );
          double widget_height = oy_widget->h(),
                 image_height = oyImage_GetHeight( image ),
                 old_scale = scale;
          old_scale = oyOption_GetValueDouble( opt, 0 );
          scale = widget_height/image_height;
          /* reset position to zero */
          oy_widget->px = int((double)oy_widget->px / old_scale * scale);
          oy_widget->py = 0;
          oyImage_Release( &image );
        }
        found = 1;
        oy_widget->resetScale();
        oyOption_SetFromDouble( opt, scale, 0,0 );
        break;
      case 'w':
        opt = findOpt( scales_reg, scale_reg );
        {
          oyConversion_s * cc = oy_widget->conversion();
          oyImage_s * image = oyConversion_GetImage( cc, OY_INPUT );
          double widget_width = oy_widget->w(),
                 image_width = oyImage_GetWidth( image ),
                 old_scale = scale;
          old_scale = oyOption_GetValueDouble( opt, 0 );
          scale = widget_width/image_width;
          /* reset position to zero */
          oy_widget->px = int((double)oy_widget->px / old_scale * scale);
          oy_widget->py = 0;
          oyImage_Release( &image );
        }
        found = 1;
        oy_widget->resetScale();
        oyOption_SetFromDouble( opt, scale, 0,0 );
        break;
      case '0': /* image ==> window size */
      case 'f': /* fit window */
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
          oyImage_Release( &image );
        }
        found = 1;
        opt = findOpt( scales_reg, scale_reg );
        oy_widget->resetScale();
        oyOption_SetFromDouble( opt, scale, 0,0 );
        break;
      case '1': /* pixel size */
        scale = 1.0;
        found = 1;
        opt = findOpt( scales_reg, scale_reg );
        oy_widget->resetScale();
        oyOption_SetFromDouble( opt, scale, 0,0 );
        break;
      case FL_ENTER:
      case 13:
        {
          int32_t frames = 0, frame = 0;
          oyConversion_s * cc = oy_widget->conversion();
          oyImage_s * image = oyConversion_GetImage( cc, OY_INPUT );
          opts = oyImage_GetTags(image);
          oyOptions_FindInt( opts, "//" OY_TYPE_STD "/file_read/frames", 0, &frames );
          oyOptions_FindInt( opts, "//" OY_TYPE_STD "/file_read/frame", 0, &frame );
          if(frame < 0) frame = 0;
          ++frame;
          if(frame >= frames) frame = 0;
          oyOptions_SetFromInt( &opts,"//" OY_TYPE_STD "/file_read/frame",
                                frame, 0, OY_CREATE_NEW );
          oyOptions_Release( &opts );
          oyImage_Release( &image );
        }
        oy_widget->damage( FL_DAMAGE_USER1 );
        found = 1;
        break;
      case '<':
        openNextImage(oy_widget, -1);
        found = 1;
        break;
      case '>':
        openNextImage(oy_widget, +1);
        found = 1;
        break;
      default:
        break;
      }

      if(opt)
      /* store to DB */
      {
        char * d  = 0;
        int error;
        double dv = scale;
        if(k == 'f' || k == '0')
          dv = -1;
        else if(k == 'w')
          dv = -2;
        else if(k == 'h')
          dv = -3;
        oyStringAddPrintf( &d, 0,0, "%g", dv );
        error = oySetPersistentString( scale_reg, oySCOPE_USER, d, 0 );
        oyOption_Release( &opt );
        oyFree_m_( d );
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
        menue_->add( _("Show Infos"),
                   FL_CTRL + 'f', info_cb, (void*)arg, 0 );
      }
      menue_->add( _("Help Shortcuts"),
                   FL_CTRL + 'h', shortcuts_cb, (void*)arg, 0 );
      menue_->add( _("Help"),
                   FL_F + 1, help_cb, (void*)arg, 0 );
      menue_->add( _("Quit"),
                   FL_CTRL + 'q', exit_cb, (void*)arg, 0 );
      menue_button_->copy(menue_->menu());

      Fl::add_handler(event_handler);
  win->end();
}

