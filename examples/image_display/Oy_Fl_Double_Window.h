#ifndef Oy_Fl_Double_Window_H
#define Oy_Fl_Double_Window_H

#include <FL/Fl_Double_Window.H>
#include "Oy_Fl_Window_Base.h"

class Oy_Fl_Double_Window : public Fl_Double_Window, public Oy_Fl_Window_Base
{
  public:
  Oy_Fl_Double_Window(int X, int Y, int W, int H, const char *title = 0)
   : Fl_Double_Window(X,Y,W,H,title) {};
  Oy_Fl_Double_Window(int W, int H, const char* title = 0) 
   : Fl_Double_Window(W,H,title) {};
  ~Oy_Fl_Double_Window() {};

  int  handle (int e)
  { 
    Oy_Fl_Window_Base::handle(this, e);
    return Fl_Double_Window::handle(e);
  };
};

#endif /* Oy_Fl_Double_Window_H */
