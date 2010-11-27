/**
 *  Oyranos is an open source Colour Management System 
 * 
 *  @par Copyright:
 *            2009-2010 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2010/09/10
 *
 *  Classes to to build colour managed widgets with Oyranos in FLTK.
 */

#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "oyranos_display_helpers.c"
#include "oyranos_display_helper_classes.cpp"

#include <oyranos.h>
#include <oyranos_alpha.h>

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
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

int oy_display_verbose = 0;


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


class Oy_Fl_Box : public Fl_Box, public Oy_Widget
{
  int e, ox, oy, px, py;
  int handle(int event)
  {
      e = event;
      switch(e) {
        case FL_PUSH:
          ox = x() - Fl::event_x();
          oy = y() - Fl::event_y();
          return (1);
        case FL_RELEASE:
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
      int ret = Fl_Widget::handle(e);
      return ret;
  }

  oyPixelAccess_s * ticket;
  void draw()
  {
    if(conversion())
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
      image = oyConversion_GetImage( conversion(), OY_OUTPUT );

      if(!ticket)
      {
        oyFilterPlug_s * plug = oyFilterNode_GetPlug( conversion()->out_, 0 );
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

      dirty = oyDrawScreenImage(conversion(), ticket, display_rectangle,
                                old_display_rectangle,
                                old_roi_rectangle, "X11",
                                display, window, dirty,
                                image );

      oyRectangle_Release( &display_rectangle );

      if(oy_display_verbose)
        oyShowGraph_( conversion()->input, 0 ); oy_display_verbose = 0;

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

public:
  void damage( char c )
  {
    if(c & FL_DAMAGE_USER1)
      dirty = 1;
    Fl_Box::damage( c );
  }

  Oy_Fl_Box(int x, int y, int w, int h) : Fl_Box(x,y,w,h)
  {
    px=py=ox=oy=0;
    ticket = 0;
  };

  ~Oy_Fl_Box(void)
  {
    oyPixelAccess_Release( &ticket );
  };
};


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
                          "//" OY_TYPE_STD "/icc", oyOPTIONATTRIBUTE_ADVANCED,
                          0 );

    Oy_Fl_Box * oy_box = (Oy_Fl_Box*) ((oyBlob_s*)observer->user_data)->ptr;
    oy_box->damage( FL_DAMAGE_USER1 );

  }

  return handled;
}
}


