/**
 *  Oyranos is an open source Colour Management System 
 * 
 *  @par Copyright:
 *            2009-2010 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/02/19
 *
 *  The given example displays a 8/16-bit PPM file or float on screen.
 */

#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <oyranos.h>
#include <oyranos_alpha.h>
#include <oyranos_cmm.h>   /* for hacking into module API */

#include "../../oyranos_logo.h"
#include "oyranos_graph_display_helpers.c"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/fl_draw.H>

#ifdef HAVE_X11
#include <FL/x.H>
#endif


#include <cmath>

#ifdef USE_GETTEXT
#include "config.h" /* I18N */
#include "fl_i18n/fl_i18n.H"
#else
#define _(text) text
#endif

#define USE_RESOLVE


extern "C" {
/* forward declaration of oyranos_alpha.c */
int                oyWriteMemToFile_ ( const char*, const void*, size_t );
char* oyReadFileToMem_  (const char* fullFileName, size_t *size,
                         oyAlloc_f     allocate_func);
}

int verbose = 0;


class Oy_Fl_Double_Window : public Fl_Double_Window
{
  public:
  Oy_Fl_Double_Window(int X, int Y, int W, int H, const char *title = 0)
   : Fl_Double_Window(X,Y,W,H,title) {};
  Oy_Fl_Double_Window(int W, int H, const char* title = 0) 
   : Fl_Double_Window(W,H,title) {};
  ~Oy_Fl_Double_Window() {};

  int pos_x;
  int pos_y;

  int  handle (int e)
  { 
    int lx_,ly_,lw_,lh_,wx,wy;
    lx_=ly_=lw_=lh_=wx=wy=0;
#if ((FL_MAJOR_VERSION * 100 + FL_MINOR_VERSION) > 106)
    Fl::screen_xywh(lx_,ly_,lw_,lh_);
#endif
    Fl_Window * win = 0;
    win = dynamic_cast<Fl_Window*> (this);
    wx = win->x();
    wy = Fl_Window::y();

#if defined(__APPLE__)
    /* osX virtualises window placement, but we need exact positioning */
    if(pos_x != x() || pos_y != y())
    {
      redraw();
    }
#endif

    pos_x = x();
    pos_y = y();

    if(e == FL_FOCUS)
      redraw();

    //printf("%d,%d event: %d\n", pos_x, pos_y, e );

    return Fl_Double_Window::handle(e);
  };
};


class Fl_Oy_Box : public Fl_Box
{
  oyConversion_s * context;
  oyPixelAccess_s * ticket;
  oyRectangle_s * old_display_rectangle;
  oyRectangle_s * old_roi_rectangle;
  int dirty;

  void draw()
  {
    if(context)
    {
      Oy_Fl_Double_Window * win = 0;
      win = dynamic_cast<Oy_Fl_Double_Window*> (window());
      int X = win->pos_x + x();
      int Y = win->pos_y + y();
      int W = w();
      int H = h();
      int i, height = 0, is_allocated = 0;
      oyPointer image_data = 0;
      int channels = 0;
      oyPixel_t pt;
      oyDATATYPE_e data_type;
      oyImage_s * image = 0;
      oyRectangle_s * display_rectangle = 0;
      void * display = 0,
           * window = 0;

#if defined(HAVE_X11)
      /* add X11 window and display identifiers to output image */
      display = fl_display;
      window = (void*)fl_xid(win);
#endif

      /* Inform about the images display coverage.  */
      display_rectangle = oyRectangle_NewWith( X,Y,W,H, 0 );

      /* Load the image before creating the oyPicelAccess_s object. */
      image = oyConversion_GetImage( context, OY_OUTPUT );

      if(!ticket)
      {
        oyFilterPlug_s * plug = oyFilterNode_GetPlug( context->out_, 0 );
        ticket = oyPixelAccess_Create( 0,0, plug, oyPIXEL_ACCESS_IMAGE, 0 );
      }

      if(image)
      {
        /* take care to not go over the borders */
        if(px < W - image->width) px = W - image->width;
        if(py < H - image->height) py = H - image->height;
        if(px > 0) px = 0;
        if(py > 0) py = 0;
      }

      ticket->start_xy[0] = -px;
      ticket->start_xy[1] = -py;

#if DEBUG
      printf( "%s:%d new display rectangle: %s +%d+%d +%d+%d\n", __FILE__,
        __LINE__, oyRectangle_Show(display_rectangle), x(), y(), px, py );
#endif

      dirty = oyGetScreenImage( context, ticket, display_rectangle,
                                old_display_rectangle,
                                old_roi_rectangle, "X11",
                                display, window, dirty,
                                image );

      oyRectangle_Release( &display_rectangle );

      if(verbose)
        oyShowGraph_( context->input, 0 ); verbose = 0;

      /* some error checks */
      pt = oyImage_PixelLayoutGet( image );
      data_type = oyToDataType_m( pt );
      channels = oyToChannels_m( pt );
      if(pt != 0 &&
         ((channels != 4 && channels != 3) || data_type != oyUINT8))
      {
        printf( "WARNING: wrong image data format: %s\n"
                "need 4 or 3 channels with 8-bit\n",
                image ? oyObject_GetName( image->oy_, oyNAME_NICK ) : "" );
        return;
      }

      /* get the data and draw the image */
      if(image)
      for(i = 0; i < image->height; ++i)
      {
        image_data = image->getLine( image, i, &height, -1, &is_allocated );

        /* on osX it uses sRGB without alternative */
        fl_draw_image( (const uchar*)image_data, 0, i, image->width, 1,
                       channels, W*channels);
        if(is_allocated)
          free( image_data );
      }

      oyImage_Release( &image );
    }
  }

  int e, ox, oy, px, py;
  int edit_object_id;
  int handle(int event) {
      e = event;
      switch(e) {
        case FL_PUSH:
          ox = x() - Fl::event_x();
          oy = y() - Fl::event_y();
          return (1);
        case FL_RELEASE:
          edit_object_id = -1;
          return (1);
        case FL_DRAG:
          px += ox + Fl::event_x();
          py += oy + Fl::event_y();
          ox = x() - Fl::event_x();
          oy = y() - Fl::event_y();
          redraw();
          return (1);
      }
      //printf("e: %d ox:%d px:%d\n",e, ox, px);
      int ret = Fl_Box::handle(e);
      return ret;
  }

public:
  Fl_Oy_Box(int x, int y, int w, int h) : Fl_Box(x,y,w,h)
  {
    context = 0;
    ticket = 0;
    old_display_rectangle = oyRectangle_NewWith( 0,0,0,0, 0 );
    old_roi_rectangle = oyRectangle_NewWith( 0,0,0,0, 0 );
    px=py=ox=oy=0;
    dirty = 0;
  };

  ~Fl_Oy_Box(void)
  {
    oyConversion_Release( &context );
    oyPixelAccess_Release( &ticket );
    oyRectangle_Release( &old_display_rectangle );
    oyRectangle_Release( &old_roi_rectangle );
  };

  void setConversion( oyConversion_s * c ) 
  {
    oyConversion_Release( &context );
    context = oyConversion_Copy( c, 0 );
  }

  void damage( char c )
  {
    if(c & FL_DAMAGE_USER1)
      dirty = 1;
    Fl_Box::damage( c );
  }

  oyFilterNode_s * setImage( const char * file_name );

  void observeICC( oyFilterNode_s * icc,
                     int(*observator)( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data ) )
  {
  /* observe the icc node */
  oyBlob_s * b = oyBlob_New(0);
  b->ptr = this;
  oyStruct_ObserverAdd( (oyStruct_s*)icc, (oyStruct_s*)context,
                        (oyStruct_s*)b,
                        observator );
  oyBlob_Release( &b );
  }
};

Oy_Fl_Double_Window * createWindow (Fl_Oy_Box ** oy_box);
void setWindowMenue                  ( Oy_Fl_Double_Window      * win,
                                       Fl_Oy_Box         * oy_box,
                                       oyFilterNode_s    * node );

extern "C" {
int      conversionObserve           ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data )
{
  int handled = 0;
  oyObserver_s * obs = observer;

  if(observer && observer->model &&
     observer->model->type_ == oyOBJECT_FILTER_NODE_S)
  {
    /*if(oy_debug_signals)*/
      printf("%s:%d INFO: \n\t%s %s: %s[%d]->%s[%d]\n",
                    strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__,
                    __LINE__, _("Signal"),
                    oySignalToString(signal_type),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );

    oyConversion_Correct( (oyConversion_s*)obs->observer,
                          "//" OY_TYPE_STD "/icc", 0 );

    Fl_Oy_Box * oy_box = (Fl_Oy_Box*) ((oyBlob_s*)observer->user_data)->ptr;
    oy_box->damage( FL_DAMAGE_USER1 );

  }

  return handled;
}
}

oyFilterNode_s * Fl_Oy_Box::setImage( const char * file_name )
{
  oyFilterNode_s * icc = 0;
  int error = 0;
  oyConversion_s * conversion = 0;

  error = oyGraphFromImageFileName( file_name, &conversion, &icc, 0 );

  setConversion( conversion );

   /* release unneeded objects; in C style */
  oyConversion_Release( &conversion );

  return icc;
}

int
main(int argc, char** argv)
{
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


  /* handle arguments */
  if(argc > 1 && strcmp(argv[1], "-v") == 0)
  {
    verbose = 1;
    ++file_pos;
  }
  if(argc > file_pos && argv[file_pos])
    file_name = argv[file_pos];
  else
    file_name = "../../oyranos_logo.ppm";



  /* setup the drawing box */
  Fl_Oy_Box * oy_box = 0;
  Oy_Fl_Double_Window * win = createWindow( &oy_box );
  icc = oy_box->setImage( file_name );
  if(icc)
  {
    setWindowMenue( win, oy_box, icc  );
    /* observe the node */
    oy_box->observeICC( icc, conversionObserve );
  }


  win->show();

  Fl::run();

  delete win;

  return 0;
}

#include <FL/Fl_Image.H>
static Fl_RGB_Image image_oyranos_logo(oyranos_logo, 64, 64, 4, 0);

struct box_n_opts {
  oyFilterNode_s * node;
  Fl_Oy_Box * box;
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
    int error = 0, i;
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
    i = oyWriteMemToFile_( command, in_text, strlen(in_text) );
    in_text = 0; command[0] = 0;

    /* render the options to the UI */
    sprintf(command, "oyranos-xforms-fltk " );
    sprintf(&command[strlen(command)],
            " -i %s/image_display_in_tmp.xml -o %s/image_display_tmp.xml",
            tmp_dir, tmp_dir );
    error = system(command);

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

Oy_Fl_Double_Window * createWindow (Fl_Oy_Box ** oy_box)
{
  int w = 640,
      h = 480;


  Fl::get_system_colors();
  Oy_Fl_Double_Window *win = new Oy_Fl_Double_Window( w, h+100, TARGET );
      *oy_box = new Fl_Oy_Box(0,0,w,h);
      (*oy_box)->box(FL_FLAT_BOX);
      /* add some text */
      Fl_Box *box = new Fl_Box(0,h,w,100, "Oyranos");
        box->labeltype(FL_ENGRAVED_LABEL);
        box->labelfont(0);
        box->labelsize(48);
        box->labelcolor(FL_FOREGROUND_COLOR);
        box->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
      box->box(FL_FLAT_BOX);
      /* place the not colour managed logo */
      { Fl_Box* o = new Fl_Box(16, h+16, 64, 64);
        o->image(image_oyranos_logo);
        o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
      }
  win->end();
  win->resizable(*oy_box);

  return win;
}

void setWindowMenue                  ( Oy_Fl_Double_Window * win,
                                       Fl_Oy_Box         * oy_box,
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

