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
 *  The given example displays a 8-bit PPM file on screen.
 */

#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <oyranos.h>
#include <oyranos_alpha.h>
#include <oyranos_cmm.h>   /* for hacking into backend API */

#include "../../oyranos_logo.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>

#ifdef HAVE_X11
#include <FL/x.H>
#endif

#include <cmath>


using namespace oyranos;


extern "C" {
/* forward declaration of oyranos_alpha.c */
int                oyWriteMemToFile_ ( const char*, void*, size_t );
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
  oyRectangle_s * old_display_rectangle;

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
      int count = oyOptions_CountType( image_tags, "//image/display/window_id",
                                       oyOBJECT_BLOB_S );
      if(!count && w)
      {
        oyBlob_s * win_id = oyBlob_New(0);
        if(win_id)
        {
          win_id->ptr = (oyPointer)w;
          o = oyOption_New( "//image/display/window_id", 0 );
          oyOption_StructMoveIn( o, (oyStruct_s**)&win_id );

          oyOptions_SetFromText( &image_tags, "//image/display/display_name",
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
      oyOptions_MoveIn( image_tags, &o, -1 );
#endif

      /* check if the actual data can be displayed */
      pt = oyImage_PixelLayoutGet( image );
      data_type = oyToDataType_m( pt );
      channels = oyToChannels_m( pt );
      if((channels != 4 && channels != 3) || data_type != oyUINT8)
      {
        printf( "WARNING: wrong image data format: %s\n%s\n"
                "need 4 or 3 channels with 8-bit\n",
                oyOptions_FindString( image_tags, "filename", 0 ),
                oyObject_GetName( image->oy_, oyNAME_NICK ));
        return;
      }

      oyOptions_Release( &image_tags );

      /* Inform about the images display coverage.  */
      image_tags = oyImage_TagsGet( image );
      display_rectangle = (oyRectangle_s*) oyOptions_GetType( image_tags, -1,
                                    "display_rectangle", oyOBJECT_RECTANGLE_S );
      oyOptions_Release( &image_tags );
      oyRectangle_SetGeo( display_rectangle, X,Y,W,H );

      /* decide wether to refresh the cached rectangle of our static image */
      if(context->out_ &&
         !oyRectangle_IsEqual( display_rectangle, old_display_rectangle ) )
      {
#ifdef DEBUG
        printf( "%s:%d new display rectangle: %s +%d+%d\n", __FILE__,__LINE__,
                oyRectangle_Show(display_rectangle), x(), y() ),
#endif

        /* convert the image data */
        oyConversion_RunPixels( context, 0 );

        /* remember the old rectangle */
        oyRectangle_SetByRectangle( old_display_rectangle, display_rectangle );
      }

      if(verbose)
        oyShowGraph_( context->input, 0 ); verbose = 0;


      /* get the data and draw the image */
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

public:
  Fl_Oy_Box(int x, int y, int w, int h) : Fl_Box(x,y,w,h)
  {
    context = 0;
    old_display_rectangle = oyRectangle_NewWith( 0,0,0,0, 0 );
  };

  ~Fl_Oy_Box(void)
  {
    oyConversion_Release( &context );
    oyRectangle_Release( &old_display_rectangle );
  };

  void setConversion( oyConversion_s * c ) 
  {
    oyConversion_Release( &context );
    context = oyConversion_Copy( c, 0 );
  }
};

Oy_Fl_Double_Window * createWindow (Fl_Oy_Box ** oy_box);


int
main(int argc, char** argv)
{
  /* some Oyranos types */
  oyConversion_s * conversion = 0;
  oyFilterNode_s * in, * out;
  oyOptions_s * options = 0;
  oyImage_s * image_in = 0, * image_out = 0;
  int error = 0,
      file_pos = 1;
  const char * file_name = 0;
  oyPixel_t pixel_layout = 0;
  oyDATATYPE_e data_type = oyUINT8;

  /* start with an empty conversion object */
  conversion = oyConversion_New( 0 );
  /* create a filter node */
  in = oyFilterNode_NewWith( "//image/input_ppm", 0,0, 0 );
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
  options = oyFilterNode_OptionsGet( in, OY_FILTER_GET_DEFAULT );
  /* add a new option with the appropriate value */
  error = oyOptions_SetFromText( &options, "//image/input_ppm/filename",
                                 file_name, OY_CREATE_NEW );
  /* release the options object, this means its not any more refered from here*/
  oyOptions_Release( &options );

  /* ask for the input image, here it will be loaded from the input filter */
  if(in)
  image_in = oyConversion_GetImage( conversion, OY_INPUT );

  if(!image_in)
    return 1;

  /* create an output image */
  /* FLTK needs a 8-bit image */
  pixel_layout = oyImage_PixelLayoutGet( image_in );
  data_type = oyToDataType_m(pixel_layout);
  pixel_layout &= (~oyDataType_m(data_type));
  pixel_layout |= oyDataType_m(oyUINT8);
  /* eigther copy the input image with a oyObject_s argument or
   * create it as follows */
  image_out = oyImage_CreateForDisplay( image_in->width, image_in->height,
                                        0, pixel_layout,
                                        0, 0,0,0,0, 0 );

#if 0
  // write to ppm image
  out = oyFilterNode_NewWith( "//image/write_ppm", 0,0, 0 );
  error = oyFilterNode_Connect( in, "Img", out, "Img", 0 );
  if(error <= 0)
  options = oyFilterNode_OptionsGet( out, OY_FILTER_GET_DEFAULT );
  error = oyOptions_SetFromText( &options, "//image/write_ppm/filename",
                                 "test_dbg_in.ppm", OY_CREATE_NEW );
  oyOptions_Release( &options );
  oyFilterNode_DataSet( out, (oyStruct_s*)image_in, 0, 0 );
  /*r = oyRectangle_NewWith(0.25,0,0,0,0);
  o = oyOption_New( "//image/input/offset", 0 );
  error = oyOption_StructMoveIn( o, (oyStruct_s**)&r );
  error = oyOptions_MoveIn( options, &o, -1 );*/
  in = out;
#endif

  /* create a new filter node */
  out = oyFilterNode_NewWith( "//colour/icc", 0,0, 0 );
  /* append the new to the previous one */
  error = oyFilterNode_Connect( in, "Img", out, "Img", 0 );
  if(error > 0)
    fprintf( stderr, "could not add  filter: %s\n", "//colour/icc" );
  /* set the image to the first/only socket of the filter node */
  oyFilterNode_DataSet( out, (oyStruct_s*)image_out, 0, 0 );
  /* swap in and out */
  in = out;

  /* create a node for preparing the image for displaying */
  out = oyFilterNode_NewWith( "//image/display", 0,0, 0 );
  /* append the node */
  error = oyFilterNode_Connect( in, "Img", out, "Img", 0 );
  if(error > 0)
    fprintf( stderr, "could not add  filter: %s\n", "//image/display" );
  in = out;



#if 0
  // write to ppm image
  out = oyFilterNode_NewWith( "//image/write_ppm", 0,0, 0 );
  error = oyFilterNode_Connect( in, "Img", out, "Img", 0 );
  if(error <= 0)
  options = oyFilterNode_OptionsGet( out, OY_FILTER_GET_DEFAULT );
  error = oyOptions_SetFromText( &options, "//image/write_ppm/filename",
                                 "test_dbg_out.ppm", OY_CREATE_NEW );
  oyOptions_Release( &options );
  oyFilterNode_DataSet( out, (oyStruct_s*)image_out, 0, 0 );
  in = out;
#endif

  /* add a closing node */
  out = oyFilterNode_NewWith( "//image/output", 0,0, 0 );
  error = oyFilterNode_Connect( in, "Img", out, "Img", 0 );
  oyFilterNode_DataSet( out, (oyStruct_s*)image_out, 0, 0 );
  /* set the output node of the conversion */
  oyConversion_Set( conversion, 0, out );


  /* the colour conversion is done in oy_box::draw() with
   * oyConversion_RunPixels() */


  Fl_Oy_Box * oy_box = 0;
  Oy_Fl_Double_Window * win = createWindow( &oy_box );

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

