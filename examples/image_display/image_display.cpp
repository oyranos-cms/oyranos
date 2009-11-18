/**
 *  Oyranos is an open source Colour Management System 
 * 
 *  @par Copyright:
 *            2009 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/02/19
 *
 *  The given example displays a 8/16-bit PPM file on screen.
 */

#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <oyranos.h>
#include <oyranos_alpha.h>
#include <oyranos_cmm.h>   /* for hacking into module API */

#include "../../oyranos_logo.h"

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
#endif

#define USE_RESOLVE

using namespace oyranos;


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


/*  Menü für rechten Maustastenklick
    node = oyFilterGraph_GetNode( graph, -1, "//" OY_TYPE_STD "/icc", 0 );
    Die icc.lcms Optionen sollten noch durch "oydi" miteinander verknüpft sein.
 */

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
      oyImage_s * image = oyConversion_GetImage( context, OY_OUTPUT );
      oyRectangle_s * display_rectangle = 0;
      oyOptions_s * image_tags = 0;
      oyDATATYPE_e data_type = oyUINT8;
      oyPixel_t pt = 0;

      if(!image)
        return;

      image_tags = oyImage_TagsGet( image );

#if defined(HAVE_X11)
      /* add X11 window and display identifiers to output image */
      oyOption_s * o = 0;
      Window  w = fl_xid(win);
      int count = oyOptions_CountType( image_tags,
                                       "//" OY_TYPE_STD "/display/window_id",
                                       oyOBJECT_BLOB_S );
      if(!count && w)
      {
        oyBlob_s * win_id = oyBlob_New(0),
                 * display_id = oyBlob_New(0);
        if(win_id)
        {
          win_id->ptr = (oyPointer)w;
          o = oyOption_New( "//" OY_TYPE_STD "/display/window_id", 0 );
          oyOption_StructMoveIn( o, (oyStruct_s**)&win_id );
          oyOptions_MoveIn( image_tags, &o, -1 );

          display_id->ptr = fl_display;
          o = oyOption_New( "//" OY_TYPE_STD "/display/display_id", 0 );
          oyOption_StructMoveIn( o, (oyStruct_s**)&display_id );
          oyOptions_MoveIn( image_tags, &o, -1 );

          oyOptions_SetFromText( &image_tags,
                                 "//" OY_TYPE_STD "/display/display_name",
                                 DisplayString(fl_display), OY_CREATE_NEW );

        } else
          printf("%s:%d WARNING: no X11 Window obtained or\n"
                 "   no oyBlob_s allocateable\n", __FILE__,__LINE__);

#if 0
        size_t size = 0;
        oyImage_s * image_in = oyConversion_GetImage( context, OY_INPUT );
        oyPointer data = oyProfile_GetMem( image_in->profile_, &size, 0,malloc);
        XcolorProfile * xprofile = (XcolorProfile*)malloc(sizeof(XcolorProfile)+                                                          size);
        oyProfileGetMD5( data, size, xprofile->md5 );
        xprofile->length = size;
        memcpy( xprofile + 1, data, size );
        XcolorProfileUpload( fl_display, xprofile );
        free( data ); size = 0; free( xprofile );

        oyImage_Release( &image_in );
#endif
      }
#endif

      /* check if the actual data can be displayed */
      pt = oyImage_PixelLayoutGet( image );
      data_type = oyToDataType_m( pt );
      channels = oyToChannels_m( pt );
      if(pt != 0 &&
         ((channels != 4 && channels != 3) || data_type != oyUINT8))
      {
        printf( "WARNING: wrong image data format: %s\n%s\n"
                "need 4 or 3 channels with 8-bit\n",
                oyOptions_FindString( image_tags, "filename", 0 ),
                image ? oyObject_GetName( image->oy_, oyNAME_NICK ) : "" );
        return;
      }

      oyOptions_Release( &image_tags );

      /* Inform about the images display coverage.  */
      image_tags = oyImage_TagsGet( image );
      display_rectangle = (oyRectangle_s*) oyOptions_GetType( image_tags, -1,
                                    "display_rectangle", oyOBJECT_RECTANGLE_S );
      oyOptions_Release( &image_tags );
      oyRectangle_SetGeo( display_rectangle, X,Y,W,H );

      if(!ticket)
      {
        oyFilterPlug_s * plug = oyFilterNode_GetPlug( context->out_, 0 );
        ticket = oyPixelAccess_Create( 0,0, plug, oyPIXEL_ACCESS_IMAGE, 0 );
        //ticket->output_image_roi = oyRectangle_NewWith( 0,0,0,0, 0 );
      }

#if 0
      oyRectangle_s window_rectangle = { oyOBJECT_RECTANGLE_S, 0,0,0 };
      oyRectangle_SetGeo( ticket->output_image_roi, px, py, W, H );
      oyRectangle_Scale( ticket->output_image_roi, 1.0/image->width );
      oyRectangle_SetGeo( &window_rectangle, x(), y(), W, H );
      oyRectangle_Scale( &window_rectangle, 1.0/image->width );
      if(ticket->output_image_roi->width > image->viewport->width)
        ticket->output_image_roi->width = image->viewport->width;
      if(ticket->output_image_roi->height > image->viewport->height)
        ticket->output_image_roi->height = image->viewport->height;
      oyRectangle_MoveInside( ticket->output_image_roi, image->viewport );
      oyRectangle_Trim( ticket->output_image_roi, &window_rectangle );
#else
      /* take care to not go over the borders */
      if(px < W - image->width) px = W - image->width;
      if(py < H - image->height) py = H - image->height;
      if(px > 0) px = 0;
      if(py > 0) py = 0;
      ticket->start_xy[0] = -px;
      ticket->start_xy[1] = -py;
#endif

      /* decide wether to refresh the cached rectangle of our static image */
      if( context->out_ &&
          (!oyRectangle_IsEqual( display_rectangle, old_display_rectangle ) ||
           !oyRectangle_IsEqual( ticket->output_image_roi, old_roi_rectangle )||
           ticket->start_xy[0] != ticket->start_xy_old[0] ||
           ticket->start_xy[1] != ticket->start_xy_old[1]) ||
           dirty )
      {
#ifdef DEBUG
        printf( "%s:%d new display rectangle: %s +%d+%d\n", __FILE__,__LINE__,
                oyRectangle_Show(display_rectangle), x(), y() ),
#endif

        /* convert the image data */
        oyConversion_RunPixels( context, ticket );

        /* remember the old rectangle */
        oyRectangle_SetByRectangle( old_display_rectangle, display_rectangle );
        oyRectangle_SetByRectangle( old_roi_rectangle,ticket->output_image_roi);
        ticket->start_xy_old[0] = ticket->start_xy[0];
        ticket->start_xy_old[1] = ticket->start_xy[1];

        dirty = 0;
      }

      if(verbose)
        oyShowGraph_( context->input, 0 ); verbose = 0;


      /* get the data and draw the image */
      if(image)
      for(i = 0; i < image->height; ++i)
      {
        image_data = image->getLine( image, i, &height, -1, &is_allocated );
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

  void setImage( oyImage_s * image_in )
  {
  oyFilterNode_s * in, * out;
  int error = 0;
  oyConversion_s * conversion = 0;
  oyOptions_s * options = 0;

  if(!image_in)
    return;

#if 0
  conversion = oyConversion_CreateBasicPixels( image_in, image_out, options,0 );
#else
  /* start with an empty conversion object */
  conversion = oyConversion_New( 0 );
  /* create a filter node */
  in = oyFilterNode_NewWith( "//" OY_TYPE_STD "/root", 0, 0 );
  /* set the above filter node as the input */
  oyConversion_Set( conversion, in, 0 );


  /* create a new filter node */
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/icc", options, 0 );
  /* append the new to the previous one */
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  if(error > 0)
    fprintf( stderr, "could not add  filter: %s\n", "//" OY_TYPE_STD "/icc" );

  /* Set the image to the first/only socket of the filter node.
   * oyFilterNode_Connect() has now no chance to copy it it the other nodes.
   * We rely on resolving the image later.
   */
  oyFilterNode_DataSet( in, (oyStruct_s*)image_in, 0, 0 );

  /* swap in and out */
  in = out;


  /* create a node for preparing the image for displaying */
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/display", 0, 0 );
  options = oyFilterNode_OptionsGet( out, OY_SELECT_FILTER );
  /* 8 bit data for FLTK */
  error = oyOptions_SetFromInt( &options,
                                "//" OY_TYPE_STD "/display/datatype",
                                oyUINT8, 0, OY_CREATE_NEW );
  /* alpha might be support once by FLTK? */
  error = oyOptions_SetFromInt( &options,
                                "//" OY_TYPE_STD "/display/preserve_alpha",
                                1, 0, OY_CREATE_NEW );
  oyOptions_Release( &options );
  /* append the node */
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  if(error > 0)
    fprintf( stderr, "could not add  filter: %s\n", "//" OY_TYPE_STD "/display" );
  in = out;


  /* add a closing node */
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", 0, 0 );
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  /* set the output node of the conversion */
  oyConversion_Set( conversion, 0, out );
#endif

  /* apply policies */
  /*error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "//verbose",
                                 "true", OY_CREATE_NEW );*/
  oyConversion_Correct( conversion, "//" OY_TYPE_STD "/icc", options );
  oyOptions_Release( &options );


  setConversion( conversion );

  /* release unneeded objects; in C style */
  oyConversion_Release( &conversion );
  }
};

Oy_Fl_Double_Window * createWindow (Fl_Oy_Box ** oy_box, oyFilterNode_s *node);

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
      printf("%s:%d WARNING: \n\t%s %s: %s[%d]->%s[%d]\n",
                    strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__,
                    __LINE__, _("Signal"),
                    oySignalToString(signal_type),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );

    Fl_Oy_Box * oy_box = (Fl_Oy_Box*) ((oyBlob_s*)observer->user_data)->ptr;
    oy_box->damage( FL_DAMAGE_USER1 );

  }

  return handled;
}
}

int
main(int argc, char** argv)
{
  /* some Oyranos types */
  oyConversion_s * conversion = 0;
  oyFilterNode_s * in, * out, * icc;
  oyOptions_s * options = 0;
  oyImage_s * image_in = 0, * image_out = 0;
  int error = 0,
      file_pos = 1;
  const char * file_name = 0;


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


  /* start with an empty conversion object */
  conversion = oyConversion_New( 0 );
  /* create a filter node */
  in = oyFilterNode_NewWith( "//" OY_TYPE_STD "/input_ppm", 0, 0 );
  /* set the above filter node as the input */
  oyConversion_Set( conversion, in, 0 );

  if(argc > 1 && strcmp(argv[1], "-v") == 0)
  {
    verbose = 1;
    ++file_pos;
  }
  if(argc > file_pos && argv[file_pos])
    file_name = argv[file_pos];
  else
    file_name = "../../oyranos_logo.ppm";

  /* add a file name argument */
  /* get the options of the input node */
  if(in)
  options = oyFilterNode_OptionsGet( in, OY_SELECT_FILTER );
  /* add a new option with the appropriate value */
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/file_read.input_ppm/filename",
                                 file_name, OY_CREATE_NEW );
  /* release the options object, this means its not any more refered from here*/
  oyOptions_Release( &options );

#if !defined(USE_RESOLVE)
  /* ask for the input image, here it will be loaded from the input filter */
  if(in)
  image_in = oyConversion_GetImage( conversion, OY_INPUT );

  if(!image_in)
    return 1;

  /* create an output image */
  /* FLTK needs a 8-bit image */
  oyPixel_t pixel_layout = 0;
  pixel_layout = oyImage_PixelLayoutGet( image_in );
  oyDATATYPE_e data_type = oyUINT8;
  data_type = oyToDataType_m(pixel_layout);
  pixel_layout &= (~oyDataType_m(data_type));
  pixel_layout |= oyDataType_m(oyUINT8);
  pixel_layout &= (~oyChannels_m( oyToChannels_m(pixel_layout) ));
  pixel_layout |= oyChannels_m(3);
  /* eigther copy the input image with a oyObject_s argument or
   * create it as follows */
  image_out = oyImage_CreateForDisplay( image_in->width, image_in->height,
                                        0, pixel_layout,
                                        0, 0,0,0,0, 0 );
#endif

#if 0
  // write to ppm image
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/write_ppm", 0, 0 );
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  if(error <= 0)
  options = oyFilterNode_OptionsGet( out, OY_FILTER_GET_DEFAULT );
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/write_ppm/filename",
                                 "test_dbg_in.ppm", OY_CREATE_NEW );
  oyOptions_Release( &options );
  oyFilterNode_DataSet( out, (oyStruct_s*)image_in, 0, 0 );
  /*r = oyRectangle_NewWith(0.25,0,0,0,0);
  o = oyOption_New( "//" OY_TYPE_STD "/input/offset", 0 );
  error = oyOption_StructMoveIn( o, (oyStruct_s**)&r );
  error = oyOptions_MoveIn( options, &o, -1 );*/
  in = out;
#endif

  /* create a new filter node */
  icc = out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/icc", options, 0 );
  /* append the new to the previous one */
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  if(error > 0)
    fprintf( stderr, "could not add  filter: %s\n", "//" OY_TYPE_STD "/icc" );
#if !defined(USE_RESOLVE)
  /* set the image to the first/only socket of the filter node */
  oyFilterNode_DataSet( out, (oyStruct_s*)image_out, 0, 0 );
#endif
  /* swap in and out */
  in = out;

  /* create a node for preparing the image for displaying */
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/display", 0, 0 );
#if defined(USE_RESOLVE)
  options = oyFilterNode_OptionsGet( out, OY_SELECT_FILTER );
  /* 8 bit data for FLTK */
  error = oyOptions_SetFromInt( &options,
                                "//" OY_TYPE_STD "/display/datatype",
                                oyUINT8, 0, OY_CREATE_NEW );
  /* alpha might be support once by FLTK? */
  error = oyOptions_SetFromInt( &options,
                                "//" OY_TYPE_STD "/display/preserve_alpha",
                                1, 0, OY_CREATE_NEW );
  oyOptions_Release( &options );
#endif
  /* append the node */
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  if(error > 0)
    fprintf( stderr, "could not add  filter: %s\n", "//" OY_TYPE_STD "/display" );
  in = out;



#if 0
  // write to ppm image
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/write_ppm", 0, 0 );
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  if(error <= 0)
  options = oyFilterNode_OptionsGet( out, OY_FILTER_GET_DEFAULT );
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/write_ppm/filename",
                                 "test_dbg_out.ppm", OY_CREATE_NEW );
  oyOptions_Release( &options );
  oyFilterNode_DataSet( out, (oyStruct_s*)image_out, 0, 0 );
  in = out;
#endif

  /* add a closing node */
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", 0, 0 );
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
#if !defined(USE_RESOLVE)
  oyFilterNode_DataSet( out, (oyStruct_s*)image_out, 0, 0 );
#endif
  /* set the output node of the conversion */
  oyConversion_Set( conversion, 0, out );

  /* apply policies */
  /*error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "//verbose",
                                 "true", OY_CREATE_NEW );*/
  oyConversion_Correct( conversion, "//" OY_TYPE_STD "/icc", options );
  oyOptions_Release( &options );


  /* the colour conversion is done in oy_box::draw() with
   * oyConversion_RunPixels() */


  Fl_Oy_Box * oy_box = 0;
  Oy_Fl_Double_Window * win = createWindow( &oy_box, icc );
  /* observe the node */
  oyBlob_s * b = oyBlob_New(0);
  b->ptr = oy_box;
  oyStruct_ObserverAdd( (oyStruct_s*)icc, (oyStruct_s*)conversion,
                        (oyStruct_s*)b,
                        conversionObserve );
  oyBlob_Release( &b );

  oy_box->setConversion( conversion );

  /* release unneeded objects; in C style */
  oyConversion_Release( &conversion );
  oyImage_Release( &image_in );
  oyImage_Release( &image_out );


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

Oy_Fl_Double_Window * createWindow (Fl_Oy_Box ** oy_box,
                                    oyFilterNode_s * node)
{
  int w = 640,
      h = 480;

  struct box_n_opts * arg = new box_n_opts;

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
      Fl_Menu_Button  *menue_;
      Fl_Menu_Button  *menue_button_;
      menue_button_ = new Fl_Menu_Button(0,0,win->w(),win->h(),0);
      menue_button_->type(Fl_Menu_Button::POPUP3);
      menue_button_->box(FL_NO_BOX);
      menue_button_->clear();
      menue_ = new Fl_Menu_Button(0,0,win->w(),win->h(),""); menue_->hide();
      arg->node = node;
      arg->box = *oy_box;
      menue_->add( _("Edit Options"),
                   FL_CTRL + 'e', callback, (void*)arg, 0 );
      menue_button_->copy(menue_->menu());
  win->end();
  win->resizable(*oy_box);

  return win;
}

