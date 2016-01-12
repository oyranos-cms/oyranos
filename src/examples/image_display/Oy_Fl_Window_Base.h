#ifndef Oy_Fl_Window_Base_H
#define Oy_Fl_Window_Base_H

#include <assert.h>
#include <FL/Fl_Double_Window.H>
#if defined(HAVE_XRANDR)
# include <X11/extensions/Xrandr.h> // XResourceManagerString() ...
#endif
#if defined(HAVE_X11)
# include <X11/Xlib.h>  // XOpenDisplay(), XResourceManagerString() ...
#endif
#include <stdio.h>  // printf()
#include <string.h> // sscanf() ...
#include <stdlib.h> // atoi()
#ifndef OY_ROUND
#include <cmath>
#define OY_ROUND(a)    ((int)floor((a)+0.5))
#endif

class Oy_Fl_Window_Base
{
  int init;
  public:
  int pos_x;
  int pos_y;

  Oy_Fl_Window_Base()  {init = 0;}
  ~Oy_Fl_Window_Base() {};

  static void handle (Fl_Window * win, int e)
  { 
    int wx,wy;
    wx=wy=0;
#if ((FL_MAJOR_VERSION * 100 + FL_MINOR_VERSION) > 106)
    int lx_,ly_,lw_,lh_;
    lx_=ly_=lw_=lh_=0;
    Fl::screen_xywh(lx_,ly_,lw_,lh_);
#endif
    wx = win->x();
    wy = win->y();

    Oy_Fl_Window_Base * oywin = dynamic_cast<Oy_Fl_Window_Base*> (win);
    if(!oywin)
      assert(!oywin);
    oywin->init = 1;
#if defined(__APPLE__)
    /* osX virtualises window placement, but we need exact positioning */
    if(oywin->pos_x != win->x() || oywin->pos_y != win->y())
    {
      win->redraw();
    }
#endif

    oywin->pos_x = wx;
    oywin->pos_y = wy;

    if(e == FL_FOCUS)
    {
      oywin->scale_ = getWinScale( win );
      FL_NORMAL_SIZE = OY_ROUND(oywin->scale_ * 14); // global and non sufficient
      win->redraw();
    }
  };

  int initialised() {return init;}

private:
  float scale_;

  // @param prefered_output  set 0 for first output
  static float getWinScale(Fl_Window * win)
  {
    float scale = 1.0f;
#if defined(HAVE_X11)
    int screen = DefaultScreen( fl_display );
    Window w = RootWindow(fl_display, screen);
    float xft_dpi = 0.0f;

    int center_x = win->x() + win->w()/2;
    int center_y = win->y() + win->h()/2;

    // check Xft.dpi settings as used by DE's and apps 
    // tested with KDE
    char * xdefs = XResourceManagerString (fl_display);
    if(xdefs != NULL)
    {
      const char * xft = strstr(xdefs, "Xft.dpi:\t");
      if(xft)
      {
        sscanf( xft, "Xft.dpi:\t%f", &xft_dpi );
        fprintf( stdout, "XResource Xft.dpi: %.02f\n", xft_dpi );
        if(xft_dpi > 30.0f)
          scale = xft_dpi / 96.0f;
      }
    }

# if defined(HAVE_XRANDR)
    {
      XRRScreenResources * res = XRRGetScreenResources(fl_display, w);
      for(int i=0; i < res->noutput; ++i)
      {
        XRROutputInfo * output_info = XRRGetOutputInfo( fl_display, res, res->outputs[i]);
        if(output_info->crtc)
        {
          XRRCrtcInfo * crtc_info = XRRGetCrtcInfo( fl_display, res,
                                                    output_info->crtc );
          unsigned int pixel_width = crtc_info->width,
                       pixel_height = crtc_info->height;
          float xdpi = pixel_width * 25.4f / (float)output_info->mm_width,
                ydpi = pixel_height * 25.4f / (float)output_info->mm_height;
          int inside = 0;
          if(crtc_info->x <= center_x && center_x <= (int)(crtc_info->x + crtc_info->width) &&
             crtc_info->y <= center_y && center_y <= (int)(crtc_info->y + crtc_info->height))
            inside = 1;
          printf( "%s[%d] %upx x %upx  Dimensions: %limm x %limm  DPI: %.02f x %.02f\n",
                  inside ? "*":" ", i, pixel_width, pixel_height,
                  output_info->mm_width, output_info->mm_height,
                  xdpi, ydpi );
        
          if(crtc_info->x <= center_x && center_x <= (int)(crtc_info->x + crtc_info->width) &&
             crtc_info->y <= center_y && center_y <= (int)(crtc_info->y + crtc_info->height) &&
             xft_dpi == 0.0f &&
             xdpi > 30.0f)
            scale = xdpi / 96.0f;

        }
        XRRFreeOutputInfo( output_info );
      }
      XRRFreeScreenResources(res);
    }
# endif
#else
    float x,y
    Fl::screen_dpi(x,y);
    scale = x;
#endif
    return scale;
  }
};


#endif /* Oy_Fl_Double_Window_Base_H */
