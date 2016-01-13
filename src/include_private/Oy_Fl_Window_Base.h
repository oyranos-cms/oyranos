#ifndef Oy_Fl_Window_Base_H
#define Oy_Fl_Window_Base_H

#include <assert.h>
#include <FL/Fl_Browser_.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Help_View.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Menu_.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Text_Display.H>
#if defined(HAVE_XRANDR)
# include <X11/extensions/Xrandr.h> // XResourceManagerString() ...
#endif
#if defined(HAVE_X11)
# include <X11/Xlib.h>  // XOpenDisplay(), XResourceManagerString() ...
# include <FL/x.H>
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
    if(oywin->init == 0)
    {
      oywin->scale_ = getWinScale( win );
      float scale_ = oywin->scale();
      win->size(OY_ROUND(scale_ * win->w()), OY_ROUND(scale_ * win->h()));
    }
    oywin->init = 1;
    /* osX virtualises window placement, but we need exact positioning */
    if(oywin->pos_x != win->x() || oywin->pos_y != win->y())
    {
      oywin->scale_ = getWinScale( win );
      FL_NORMAL_SIZE = OY_ROUND(oywin->scale() * 14); // global and non sufficient

      Fl_Group *g = dynamic_cast<Fl_Group*> (win);
      oywin->scaleWidgets( g );
      printf("x: %d y: %d\n",wx,wy);

      win->redraw();
    }

    oywin->pos_x = wx;
    oywin->pos_y = wy;

    if(e == FL_FOCUS)
    {
      win->redraw();
    }
  };

  int initialised() {return init;}

  float scale( ) {return scale_ * scale_factor_();}
  static void setScaleFactorPreferenceDomain( const char * org, const char * app )
  { const char ** org_ = scale_factor_pref_org(); *org_ = org;
    const char ** app_ = scale_factor_pref_app(); *app_ = app;
  }
  static float getScaleFactor()
  {
    if(*scale_factor_pref_org() && *scale_factor_pref_app())
    {
      Fl_Preferences pref( Fl_Preferences::USER, *scale_factor_pref_org(), *scale_factor_pref_app() );
      Fl_Preferences app_pref( pref, *scale_factor_pref_app() );
      app_pref.get(  "user_scale", scale_factor_(), 1.0f );
    }
    return scale_factor_();
  }
  static void setScaleFactor( float multiply_user_scale )
  { scale_factor_() *= multiply_user_scale;
    if(*scale_factor_pref_org() && *scale_factor_pref_app())
    {
      Fl_Preferences pref( Fl_Preferences::USER, *scale_factor_pref_org(), *scale_factor_pref_app() );
      Fl_Preferences app_pref( pref, *scale_factor_pref_app() );
      app_pref.set(  "user_scale", scale_factor_() );
    }
  }


private:
  float scale_;
  static const char ** scale_factor_pref_org()
  { static const char * scale_factor_pref_org_ = NULL; return & scale_factor_pref_org_;
  }
  static const char ** scale_factor_pref_app()
  { static const char * scale_factor_pref_app_ = NULL; return & scale_factor_pref_app_;
  }
  static float & scale_factor_()
  { static float scale_factor__ = 1.0f; return scale_factor__;
  }
  void updateMenuSize(Fl_Menu_Item * menue)
  {
    int size = 0;
    if(menue)
      size = menue->size();
    for(int i = 0; i < size ; ++i) {
      const char* text = menue[i].label();
      if(text)
        menue[i].labelsize( FL_NORMAL_SIZE );
    }
  }
  void scaleWidgets(Fl_Group * g)
  {
        if( g )
          for(int i = 0; i < g->children(); ++i)
          {
            Fl_Widget * w = g->child(i);
            w->labelsize( FL_NORMAL_SIZE );
            Fl_Browser_ * b;
            Fl_Group * sub;
            Fl_Help_View * hv;
            Fl_Hold_Browser * h;
            Fl_Menu_ * m;
            Fl_Text_Display * t;
            printf("Fl_Widget %d %s\n", i, w->label()? w->label():"");
            if((sub = dynamic_cast<Fl_Group*> (w)) != NULL)
            {
              sub->labelsize( FL_NORMAL_SIZE );
              printf("Fl_Group\n");
              scaleWidgets( sub );
            }
            else if((m = dynamic_cast<Fl_Menu_*> (w)) != NULL)
            {
              m->textsize( FL_NORMAL_SIZE );
              printf("Fl_Menu_\n");
              updateMenuSize(const_cast<Fl_Menu_Item*>(m->menu()));
            }

            if((b = dynamic_cast<Fl_Browser_*> (w)) != NULL)
            {
              b->textsize( FL_NORMAL_SIZE );
              printf("Fl_Browser_\n");
            }
            if((h = dynamic_cast<Fl_Hold_Browser*> (w)) != NULL)
            {
              h->textsize( FL_NORMAL_SIZE );
              printf("Fl_Hold_Browser\n");
            }
            if((hv = dynamic_cast<Fl_Help_View*> (w)) != NULL)
            {
              hv->textsize( FL_NORMAL_SIZE );
              printf("Fl_Help_View\n");
            }
            if((t = dynamic_cast<Fl_Text_Display*> (w)) != NULL)
            {
              t->textsize( FL_NORMAL_SIZE );
              printf("Fl_Text_Display\n");
            }
          }
  }

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

#define OY_FL_SCALE(base) ((dynamic_cast<Oy_Fl_Window_Base*>(base->window()))?(dynamic_cast<Oy_Fl_Window_Base*>(base->window()))->scale():1.0)
#define OY_FL_WSCALE(x) OY_ROUND( OY_FL_SCALE(this) * (x) )
#define OY_FL_OSCALE(x,obj) OY_ROUND( OY_FL_SCALE(obj) * (x) )

#endif /* Oy_Fl_Double_Window_Base_H */
