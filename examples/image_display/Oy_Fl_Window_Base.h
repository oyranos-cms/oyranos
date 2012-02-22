#ifndef Oy_Fl_Window_Base_H
#define Oy_Fl_Window_Base_H

#include <assert.h>
#include <FL/Fl_Double_Window.H>


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
      win->redraw();
  };

  int initialised() {return init;}
};


#endif /* Oy_Fl_Double_Window_Base_H */
