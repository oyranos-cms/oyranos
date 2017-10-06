#ifndef Oy_Fl_Image_Widget_H
#define Oy_Fl_Image_Widget_H

#include <assert.h>

#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>

#include "Oy_Fl_Double_Window.h"
#include "Oy_Widget.h"

#ifndef _DBG_FORMAT_
#define _DBG_FORMAT_ "%s:%d %s() "
#define _DBG_ARGS_ (strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__),__LINE__,__func__
#endif

extern "C" {
void               oyShowConversion_ ( oyConversion_s    * conversion,
                                       uint32_t            flags );
}

oyOptions_s * findOpts( oyFilterNode_s * node, const char * filter_name )
{
  oyFilterGraph_s * g = oyFilterGraph_FromNode( node, 0 );
  oyFilterNode_s * n = oyFilterGraph_GetNode( g, -1, filter_name, NULL );
  oyOptions_s * opts = oyFilterNode_GetOptions( n, 0 );
  oyFilterGraph_Release( &g );
  oyFilterNode_Release( &n );

  return opts;
}

#define DEBUG_MOVE DEBUG

class Oy_Fl_Image_Widget : public Fl_Widget, public Oy_Widget
{
  int e, ox, oy;
  double scale_;
public:
  void resetScale( ) { scale_ = 1; }
  double scale_changer;
  int px, py, mx, my;
  int handle(int event)
  {
      e = event;
      mx = Fl::event_x();
      my = Fl::event_y();
      switch(e)
      {
        case FL_PUSH:
          ox = x() - Fl::event_x();
          oy = y() - Fl::event_y();
          fl_cursor( FL_CURSOR_MOVE, FL_BLACK, FL_WHITE );
#if DEBUG_MOVE
          if(oy_debug) printf(_DBG_FORMAT_"e: %d ox:%d px:%d\n",_DBG_ARGS_,e,ox,px);
#endif
          return (1);
        case FL_RELEASE:
          fl_cursor( FL_CURSOR_DEFAULT, FL_BLACK, FL_WHITE );
#if DEBUG_MOVE
          if(oy_debug) printf(_DBG_FORMAT_"e: %d ox:%d px:%d\n",_DBG_ARGS_,e,ox,px);
#endif
          return (1);
        case FL_DRAG:
          px += ox + Fl::event_x();
          py += oy + Fl::event_y();
          ox = x() - Fl::event_x();
          oy = y() - Fl::event_y();
#if DEBUG_MOVE
          if(oy_debug >= 4)
            printf(_DBG_FORMAT_"e: %d ox:%d px:%d oy: %d py:%d\n",_DBG_ARGS_,e, ox, px,oy,py);
#endif
          redraw();
          return (1);
        case FL_MOUSEWHEEL:
          if(Fl::event_dy())
          {
            oyFilterNode_s * node_out = oyConversion_GetNode( conversion(),OY_OUTPUT);

            double scale = 1.0;
            double wheel_scale_changer = (scale_changer-1.0)/10.0+1.0;
            oyOptions_s * opts = findOpts( node_out, "//" OY_TYPE_STD "/scale" );

            oyOptions_FindDouble( opts, "scale",
                                   0, &scale );
            if(Fl::event_dy() > 0)
            {
              scale *= wheel_scale_changer;
              // scale relative to the middle of the image
              px = int((double)(px - mx) * wheel_scale_changer) + mx;
              py = int((double)(py - my) * wheel_scale_changer) + my;
            }
            else
            if(Fl::event_dy() < 0)
            {
              scale /= wheel_scale_changer;
              px = int((double)(px - mx) / wheel_scale_changer) + mx;
              py = int((double)(py - my) / wheel_scale_changer) + my;
            }
            oyOptions_SetFromDouble( &opts,
                                   "//" OY_TYPE_STD "/scale/scale",
                                   scale, 0, OY_CREATE_NEW );
            oyOptions_Release( &opts );
            redraw();
            return (1);
          }
      }

      if(oy_debug > 3)
        printf(_DBG_FORMAT_"e: %d x: %d y:%d %dx%d\n",_DBG_ARGS_,e, Fl::event_x(),Fl::event_y(), px, py );
      int ret = Fl_Widget::handle(e);
      return ret;
  }

public:
  int drawPrepare( oyImage_s ** draw_image, oyDATATYPE_e data_type_request,
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
      oyImage_s * image_input = 0, * image_output = 0;
      oyRectangle_s * display_rectangle = 0;
      oyFilterNode_s * node_out = oyConversion_GetNode( conversion(),OY_OUTPUT);
      void * display = 0,
           * window = 0;
      int width_input, width_output, width_scale, width_roi;
      int height_input, height_output, height_scale, height_roi;

      double scale = 1.0;
      oyOptions_s * opts = findOpts( node_out, "//" OY_TYPE_STD "/scale" );

      oyOptions_FindDouble( opts, "scale",
                                   0, &scale );

#if defined(XCM_HAVE_X11)
      /* add X11 window and display identifiers to output image */
      display = fl_display;
      window = (void*)fl_xid( Fl_Widget::window() );
#endif

      /* Get the source dimensions */
      image_input = oyConversion_GetImage( conversion(), OY_INPUT );
      width_input = oyImage_GetWidth( image_input );
      height_input = oyImage_GetHeight( image_input );

      /* Handle automatic scaling */
      if(scale <= 0)
        scale_ = scale;
      if(scale_ <= 0)
      {
        double widget_width = W,
               widget_height = H;
        if(((scale_ == -1) && widget_width/width_input < widget_height/height_input) ||
            scale_ == -2)
          scale = widget_width/width_input;
        else /* scale_ == -1 || scale_ == -3 */
          scale = widget_height/height_input;
        oyOption_s * opt = oyOptions_Find( opts, "scale", oyNAME_PATTERN );
        oyOption_SetFromDouble( opt, scale, 0,0 );
        oyOption_Release( &opt );
        if(oy_debug)
          printf("found negative scale: %g ==> %g\n", scale_, scale);
      }
      oyOptions_Release( &opts );

      width_scale = width_roi = width_input * scale;
      height_scale = height_roi = height_input * scale;
      if(width_roi > W)
        width_roi = W;
      if(height_roi > H)
        height_roi = H;

      /* Load the image before creating the oyPicelAccess_s object. */
      image_output = oyConversion_GetImage( conversion(), OY_OUTPUT );
      width_output = oyImage_GetWidth( image_output );
      height_output = oyImage_GetHeight( image_output );

      if(image_output && !ticket())
      {
        oyFilterPlug_s * plug = oyFilterNode_GetPlug( node_out, 0 );
        ticket( oyPixelAccess_Create( 0,0, plug, oyPIXEL_ACCESS_IMAGE, 0 ) );
      }


      /* update output image dimensions to fit scale */
      if(image_output &&
         (width_output != width_roi ||
          height_output != height_roi))
      {
        if(oy_debug > 2)
          printf(_DBG_FORMAT_"image_input [%d](%d) update image_output [%d](%d)",_DBG_ARGS_, oyStruct_GetId((oyStruct_s*)image_input),oyImage_GetWidth(image_input), oyStruct_GetId((oyStruct_s*)image_output),oyImage_GetWidth(image_output) );
        if(ticket())
        {
          oyImage_s * output_image = oyPixelAccess_GetOutputImage( ticket() );
          oyImage_SetCritical( output_image, 0, NULL, NULL, width_roi, height_roi );
          if(oy_debug > 2)
            printf(" output_image [%d](%d)", oyStruct_GetId((oyStruct_s*)output_image),oyImage_GetWidth(output_image) );
          oyImage_Release( &output_image );
          oyPixelAccess_SetArray( ticket(), NULL, 0 );
        }
        if(oy_debug > 2) printf("\n");
      }

      if(image_output)
      {
        /* take care to not go over the borders */
        if(px < W - width_scale) px = W - width_scale;
        if(py < H - height_scale) py = H - height_scale;
        if(px > 0) px = 0;
        if(py > 0) py = 0;

        /* Inform about the images display coverage.  */
        int offset_x = 0, offset_y = 0;
        if(center_aligned)
        {
          if(W > width_scale)
            offset_x = (W - width_scale) / 2;
          if(H > height_scale)
            offset_y = (H - height_scale) / 2;
        }
        display_rectangle = oyRectangle_NewWith( X+offset_x,Y+offset_y,W,H, 0 );
      }

#if DEBUG_MOVE
      if(oy_debug >= 4)
      printf( _DBG_FORMAT_"new display rectangle: %s +%d+%d +%d+%d\n",
              _DBG_ARGS_,
              oyRectangle_Show(display_rectangle), x(), y(), px, py );
#endif

      if(ticket())
      {
        oyRectangle_s * output_rectangle = oyRectangle_NewWith(0,0, width_roi,height_roi, 0);
        oyRectangle_Scale( output_rectangle, 1.0/width_roi );

        oyPixelAccess_ChangeRectangle( ticket(),
                                       -px/(double)width_roi,
                                       -py/(double)width_roi,
                                       output_rectangle );

        oyRectangle_Release( &output_rectangle );
      }

      /* limit a too big display texture */
      if(display_rectangle)
      {
        *oyRectangle_SetGeo1(display_rectangle,2) = OY_MIN( oyRectangle_GetGeo1(display_rectangle,2), width_roi );
        *oyRectangle_SetGeo1(display_rectangle,3) = OY_MIN( oyRectangle_GetGeo1(display_rectangle,3), height_roi );
      }

      if(image_output)
        dirty = oyDrawScreenImage(conversion(), ticket(), display_rectangle,
                                old_display_rectangle,
                                old_roi_rectangle, "X11",
                                data_type_request,
                                display, window, dirty,
                                image_output );

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
      pt = oyImage_GetPixelLayout( image_output, oyLAYOUT );
      data_type = oyToDataType_m( pt );
      channels = oyToChannels_m( pt );
      if(pt != 0 &&
         ((channels != 4 && channels != 3) || data_type != data_type_request))
      {
        printf( _DBG_FORMAT_"WARNING: wrong image_output data format: %s\n"
                "need 4 or 3 channels with %s\n",_DBG_ARGS_,
                image_output ? oyObject_GetName( image_output->oy_, oyNAME_NICK ) : "",
                oyDataTypeToText( data_type_request ) );
        return 1;
      }

      *draw_image = image_output;

      oyImage_Release( &image_input );
    }

    return dirty;
  }

public:
  void damage( char c )
  {
    if(c & FL_DAMAGE_USER1)
      dirty = 1;
    Fl_Widget::damage( c );
  }
  /* implement Oy_Widget::markDirty() */
  void markDirty()
  {
    damage( FL_DAMAGE_USER1 );
    damage( FL_DAMAGE_ALL );
    deactivate();
    activate();
  }

  Oy_Fl_Image_Widget(int x, int y, int w, int h) : Fl_Widget(x,y,w,h)
  {
    px=py=ox=oy=0;
    scale_changer = 1.2;
    scale_ = 1;
  };

  ~Oy_Fl_Image_Widget(void)
  {
  };
};

#endif /* Oy_Fl_Image_Widget_H */
