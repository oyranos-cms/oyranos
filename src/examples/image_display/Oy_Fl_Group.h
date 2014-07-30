#ifndef Oy_Fl_Group_H
#define Oy_Fl_Group_H

#include <assert.h>

#include <FL/Fl_Group.H>
#include <FL/fl_draw.H>

#include "Oy_Fl_Window_Base.h"
#include "oyranos_display_helpers.h"

extern "C" {
void               oyShowConversion_ ( oyConversion_s    * conversion,
                                       uint32_t            flags );
}

class Oy_Fl_Group : public Fl_Group
{
  Fl_Offscreen off;
  unsigned char * off_buf;
  oyPixelAccess_s * ticket;
  int W,H;
  int e, px, py;
  int dirty;
  oyConversion_s * context;

public:
  oyRectangle_s * old_display_rectangle;

  void damage( char c )
  {
    if(c & FL_DAMAGE_USER1)
      dirty = 1;
    Fl_Group::damage( c );
  }
  void drawPrepare( oyImage_s ** draw_image, oyDATATYPE_e data_type_request,
                    int center_aligned )
  {
    {
      Oy_Fl_Window_Base * topWindow_is_a_Oy_Fl_Window_Base = 0, * win = 0;
      win = topWindow_is_a_Oy_Fl_Window_Base = 
                                    dynamic_cast<Oy_Fl_Window_Base*> (window());
      assert(topWindow_is_a_Oy_Fl_Window_Base != NULL);
      int Oy_Fl_Window_Base_is_initialised_by_calling_its_handleFunc_from_topWindowHandleFunc = win->initialised();
      assert(Oy_Fl_Window_Base_is_initialised_by_calling_its_handleFunc_from_topWindowHandleFunc);
      int X = win->pos_x + x();
      int Y = win->pos_y + y();
      int W = w();
      int H = h();
      int channels = 0;
      oyPixel_t pt;
      oyDATATYPE_e data_type;
      oyImage_s * image = 0;
      oyRectangle_s * display_rectangle = 0;
      oyFilterNode_s * node_out = oyConversion_GetNode( conversion(),OY_OUTPUT);
      void * display = 0,
           * window = 0;

#if defined(XCM_HAVE_X11)
      /* add X11 window and display identifiers to output image */
      display = fl_display;
      window = (void*)fl_xid( Fl_Widget::window() );
#endif

      /* Load the image before creating the oyPicelAccess_s object. */
      image = oyConversion_GetImage( conversion(), OY_OUTPUT );

      if(image && !ticket)
      {
        oyFilterPlug_s * plug = oyFilterNode_GetPlug( node_out, 0 );
        ticket = oyPixelAccess_Create( 0,0, plug, oyPIXEL_ACCESS_IMAGE, 0 );
      }

      if(image)
      {
        /* take care to not go over the borders */
        if(px < W - oyImage_GetWidth( image )) px = W - oyImage_GetWidth( image );
        if(py < H - oyImage_GetHeight( image )) py = H - oyImage_GetHeight( image );
        if(px > 0) px = 0;
        if(py > 0) py = 0;

        /* Inform about the images display coverage.  */
        int offset_x = 0, offset_y = 0;
        if(center_aligned)
        {
          if(W > oyImage_GetWidth( image ))
            offset_x = (W - oyImage_GetWidth( image )) / 2;
          if(H > oyImage_GetHeight( image ))
            offset_y = (H - oyImage_GetHeight( image )) / 2;
        }
        display_rectangle = oyRectangle_NewWith( X+offset_x,Y+offset_y,W,H, 0 );
      }

#if DEBUG
      if(oy_debug > 1)
      printf( "%s:%d new display rectangle: %s +%d+%d +%d+%d\n",
              strrchr(__FILE__,'/')+1, __LINE__,
              oyRectangle_Show(display_rectangle), x(), y(), px, py );
#endif

      if(ticket)
      {
        oyRectangle_s * output_rectangle = oyRectangle_New(0);
        oyImage_PixelsToSamples( image, 0, output_rectangle );
        *oyRectangle_SetGeo1(output_rectangle,2) = OY_MIN( W, oyImage_GetWidth( image ) );
        *oyRectangle_SetGeo1(output_rectangle,3) = OY_MIN( H, oyImage_GetHeight( image ) );
        oyRectangle_Scale( output_rectangle, 1.0/oyImage_GetWidth( image ) );
#if DEBUG
        if(oy_debug > 1)
        {
          static int old_px = 0;
          if(px != old_px)
          {
            old_px = px;
            oyRectangle_s * r = oyRectangle_New( 0 );
            oyRectangle_SetByRectangle( r, output_rectangle );
            oyRectangle_Scale( r, oyImage_GetWidth( image ) );
            printf( "%s:%d output rectangle: %s start_xy:%.04g %.04g\n",
                strrchr(__FILE__,'/')+1, __LINE__,
                oyRectangle_Show(r),
                oyPixelAccess_GetStart( ticket,0 )*oyImage_GetWidth( image ),
                oyPixelAccess_GetStart( ticket,1 )*oyImage_GetWidth( image ) );
            oyRectangle_Release( &r );
          }
        }
#endif
        oyPixelAccess_ChangeRectangle( ticket,
                                       -px/(double)oyImage_GetWidth( image ),
                                       -py/(double)oyImage_GetWidth( image ),
                                       output_rectangle );
        oyRectangle_Release( &output_rectangle );
      }

      if(image)
        dirty = oyDrawScreenImage(conversion(), ticket, display_rectangle,
                                old_display_rectangle,
                                NULL, "X11",
                                data_type_request,
                                display, window, dirty,
                                image );

      oyRectangle_Release( &display_rectangle );

      if(oy_display_verbose)
      {
        static int done = 0;
        if(!done)
        {
          int oy_debug_old = oy_debug;
          oy_debug = 1;
          oyShowConversion_( conversion(), 0 );
          oy_debug = oy_debug_old;
        }
        done = 1;
      }

      /* some error checks */
      pt = oyImage_GetPixelLayout( image, oyLAYOUT );
      data_type = oyToDataType_m( pt );
      channels = oyToChannels_m( pt );
      if(pt != 0 &&
         ((channels != 4 && channels != 3) || data_type != data_type_request))
      {
        printf( "%s:%d WARNING: wrong image data format: %s\n"
                "need 4 or 3 channels with %s\n", __FILE__,__LINE__,
                image ? oyObject_GetName( image->oy_, oyNAME_NICK ) : "",
                oyDataTypeToText( data_type_request ) );
        return;
      }

      *draw_image = image;
    }
  }

  void draw()
  {
    if(oy_display_verbose)
      printf("%s:%d W:%d H:%d %dx%d+%d+%d %dx%d+%d+%d\n",
              strrchr(__FILE__,'/')+1, __LINE__,
              W, H, w(),h(),x(),y(),
              Fl_Group::w(),Fl_Group::h(),Fl_Group::x(),Fl_Group::y() );
    if(!off || W != w() || H != h())
    {
      W = w();
      H = h();
      if(off)
        fl_delete_offscreen(off);
      if(off_buf)
        delete [] off_buf;
      off = fl_create_offscreen(W,H);
      off_buf = new unsigned char[W*H*4];
      setImage();
    } else if(!context)
      setImage();

    fl_begin_offscreen(off);
    int X = x(), Y = y();
    position(0,0);
    Fl_Group::draw();
    position(X,Y);
    dirty=1;
    fl_read_image(off_buf,0,0,W,H);
    fl_end_offscreen();

    if(conversion())
    {
      oyImage_s * image = 0;

      drawPrepare( &image, oyUINT8, 0 );

      if(oy_debug > 2)
      printf("%s:%d draw() %s\n",
              strrchr(__FILE__,'/')+1, __LINE__,
              oyFilterNode_GetText( icc, oyNAME_NAME ) );
      oyImage_Release( &image );
    }
    fl_draw_image(off_buf, x(),y(),W,H);
  }

  oyConversion_s * conversion()
  {
    return context;
  }

private:
  oyImage_s * image;
  oyImage_s * image_display;
  oyProfile_s * editing;
  oyFilterNode_s * icc;
public:
  oyFilterNode_s * setImage( )
  {
    oyImage_Release( &image );
    oyImage_Release( &image_display );
    image = oyImage_Create( W, H,
                         off_buf ,
                         oyChannels_m(oyProfile_GetChannelsCount(editing)) |
                          oyUINT8,
                         editing,
                         0 );
    image_display = oyImage_Create( W, H,
                         off_buf ,
                         oyChannels_m(oyProfile_GetChannelsCount(editing)) |
                          oyUINT8,
                         editing,
                         0 );
    if(oy_debug > 1)
      printf("%s:%d image:%s image_display:%s\n",
              strrchr(__FILE__,'/')+1, __LINE__,
              image?"created":"failed",
              image_display?"created":"failed");
    oyConversion_Release( &context );
    oyFilterNode_Release( &icc );
    context = oyConversion_FromImageForDisplay( 
                             image, image_display, &icc,
                             oyOPTIONATTRIBUTE_ADVANCED, oyUINT8, 0, 0 );
    if(oy_debug > 2)
      printf("%s:%d context:%s\n",
              strrchr(__FILE__,'/')+1, __LINE__,
              oyFilterNode_GetText( icc, oyNAME_NAME ) );
    oyPixelAccess_Release( &ticket );
    return icc;
  }

  void setProfile( oyProfile_s * p )
  {
    if(p)
      editing = oyProfile_Copy( p, NULL );
    else
    {
      oyFilterNode_s * node = icc ? icc : oyFilterNode_FromOptions( OY_CMM_STD, "//" OY_TYPE_STD "/icc_color", NULL, NULL );
      const char * reg = oyFilterNode_GetRegistration( node );
      uint32_t icc_profile_flags = oyICCProfileSelectionFlagsFromRegistration( reg );
      editing = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, NULL );
      if(!icc)
        oyFilterNode_Release( &node );
    }
    oyConversion_Release( &context );
  }

  Oy_Fl_Group( int X, int Y, int W, int H, oyProfile_s * p = NULL )
  : Fl_Group(X,Y,W,H)
  {
    off = 0;
    off_buf = NULL;
    ticket = NULL;
    context = NULL;
    old_display_rectangle = oyRectangle_NewWith( 0,0,0,0, 0 );
    dirty = 1;
    image = NULL;
    image_display = NULL;
    icc = NULL;
    W = 0; H = 0;
    e = 0, px = 0, py = 0;
    setProfile( p );
  }
  ~Oy_Fl_Group( )
  {
    oyConversion_Release( &context );
    oyRectangle_Release( &old_display_rectangle );
    oyPixelAccess_Release( &ticket );
    oyImage_Release( &image );
    oyImage_Release( &image_display );
    oyFilterNode_Release( &icc );
  }
};

#endif /* Oy_Fl_Group_H */
