#ifndef Oy_Fl_Image_Widget_H
#define Oy_Fl_Image_Widget_H

#include <assert.h>

#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>

#include "Oy_Fl_Double_Window.h"
#include "Oy_Widget.h"

class Oy_Fl_Image_Widget : public Fl_Widget, public Oy_Widget
{
  int e, ox, oy, px, py;
public:
  int handle(int event)
  {
      e = event;
      switch(e) {
        case FL_PUSH:
          ox = x() - Fl::event_x();
          oy = y() - Fl::event_y();
          fl_cursor( FL_CURSOR_MOVE, FL_BLACK, FL_WHITE );
#if DEBUG_
      printf("%s:%d e: %d ox:%d px:%d\n",strrchr(__FILE__,'/')+1,__LINE__,e, ox, px);
#endif
          return (1);
        case FL_RELEASE:
          fl_cursor( FL_CURSOR_DEFAULT, FL_BLACK, FL_WHITE );
#if DEBUG_
      printf("%s:%d e: %d ox:%d px:%d\n",strrchr(__FILE__,'/')+1,__LINE__,e, ox, px);
#endif
          return (1);
        case FL_DRAG:
          px += ox + Fl::event_x();
          py += oy + Fl::event_y();
          ox = x() - Fl::event_x();
          oy = y() - Fl::event_y();
#if DEBUG_
      printf("%s:%d e: %d ox:%d px:%d oy: %d py:%d\n",strrchr(__FILE__,'/')+1,__LINE__,e, ox, px,oy,py);
#endif
          redraw();
          return (1);
      }
      int ret = Fl_Widget::handle(e);
      return ret;
  }

private:
  oyPixelAccess_s * ticket;
public:
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

#if defined(HAVE_X)
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
        if(px < W - image->width) px = W - image->width;
        if(py < H - image->height) py = H - image->height;
        if(px > 0) px = 0;
        if(py > 0) py = 0;

        /* Inform about the images display coverage.  */
        int offset_x = 0, offset_y = 0;
        if(center_aligned)
        {
          if(W > image->width)
            offset_x = (W - image->width) / 2;
          if(H > image->height)
            offset_y = (H - image->height) / 2;
        }
        display_rectangle = oyRectangle_NewWith( X+offset_x,Y+offset_y,W,H, 0 );
      }

#if DEBUG_
      printf( "%s:%d new display rectangle: %s +%d+%d +%d+%d\n",
              strrchr(__FILE__,'/')+1, __LINE__,
              oyRectangle_Show(display_rectangle), x(), y(), px, py );
#endif

      if(ticket)
      {
        oyRectangle_s output_rectangle = {oyOBJECT_RECTANGLE_S,0,0,0};
        oyRectangle_SamplesFromImage( image, 0, &output_rectangle );
        output_rectangle.width = OY_MIN( W, image->width );
        output_rectangle.height = OY_MIN( H, image->height );
        oyRectangle_Scale( &output_rectangle, 1.0/image->width );
#if DEBUG_
        static int old_px = 0;
        if(px != old_px)
        {
        old_px = px;
        oyRectangle_s r = {oyOBJECT_RECTANGLE_S,0,0,0};
        oyRectangle_SetByRectangle( &r, &output_rectangle );
        oyRectangle_Scale( &r, image->width );
        printf( "%s:%d output rectangle: %s start_xy:%.04g %.04g\n",
                strrchr(__FILE__,'/')+1, __LINE__,
                oyRectangle_Show(&r),
                ticket->start_xy[0]*image->width, ticket->start_xy[1]*image->width );
        }
#endif
        oyPixelAccess_ChangeRectangle( ticket,
                                       -px/(double)image->width,
                                       -py/(double)image->width,
                                       &output_rectangle );
      }

      if(image)
        dirty = oyDrawScreenImage(conversion(), ticket, display_rectangle,
                                old_display_rectangle,
                                old_roi_rectangle, "X11",
                                data_type_request,
                                display, window, dirty,
                                image );

      oyRectangle_Release( &display_rectangle );

      if(oy_display_verbose)
      {
        static int done = 0;
        if(!done)
          oyShowGraph_( conversion()->input, 0 );
        done = 1;
      }

      /* some error checks */
      pt = oyImage_PixelLayoutGet( image );
      data_type = oyToDataType_m( pt );
      channels = oyToChannels_m( pt );
      if(pt != 0 &&
         ((channels != 4 && channels != 3) || data_type != data_type_request))
      {
        printf( "WARNING: wrong image data format: %s\n"
                "need 4 or 3 channels with %s\n",
                image ? oyObject_GetName( image->oy_, oyNAME_NICK ) : "",
                oyDatatypeToText( data_type_request ) );
        return;
      }

      *draw_image = image;
    }
  }

public:
  void damage( char c )
  {
    if(c & FL_DAMAGE_USER1)
      dirty = 1;
    Fl_Widget::damage( c );
  }

  Oy_Fl_Image_Widget(int x, int y, int w, int h) : Fl_Widget(x,y,w,h)
  {
    px=py=ox=oy=0;
    ticket = 0;
  };

  ~Oy_Fl_Image_Widget(void)
  {
    oyPixelAccess_Release( &ticket );
  };

  void observeICC(                     oyFilterNode_s * icc,
                     int(*observator)( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data ) )
  {
    /* observe the icc node */
    oyPointer_s * oy_box_ptr = oyPointer_New(0);
    oyPointer_Set( oy_box_ptr,
                   __FILE__,
                   "Oy_Fl_Image_Widget",
                   this, 0, 0 );
    oyStruct_ObserverAdd( (oyStruct_s*)icc, (oyStruct_s*)conversion(),
                          (oyStruct_s*)oy_box_ptr,
                          observator );
    oyPointer_Release( &oy_box_ptr );
  }
};

#endif /* Oy_Fl_Image_Widget_H */
