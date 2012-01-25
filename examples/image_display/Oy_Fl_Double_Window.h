#ifndef Oy_Fl_Double_Window_H
#define Oy_Fl_Double_Window_H

#include <FL/Fl_Double_Window.H>

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
    int wx,wy;
    wx=wy=0;
#if ((FL_MAJOR_VERSION * 100 + FL_MINOR_VERSION) > 106)
    int lx_,ly_,lw_,lh_;
    lx_=ly_=lw_=lh_=wx=wy=0;
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

#endif /* Oy_Fl_Double_Window_H */
