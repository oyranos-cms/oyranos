/** @file oyranos_widgets_fltk.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2009-2011 (C) Kai-Uwe Behrmann
 *
 *  @brief    additional widgets for the FLTK toolkit
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2009/10/10
 */

#ifndef OYRANOS_WIDGETS_FLTK_H
#define OYRANOS_WIDGETS_FLTK_H


#include <FL/Fl.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Widget.H>
#include "oyranos_types.h"
#include "oyranos_forms.h"

extern float oy_forms_fltk_scale;
#define H_SPACING     (OY_ROUND(10 * oy_forms_fltk_scale))
#define V_SPACING     (OY_ROUND(3 * oy_forms_fltk_scale))
#define BUTTON_HEIGHT (OY_ROUND(25 * oy_forms_fltk_scale))
#define BOX_WIDTH     (OY_ROUND(176 * oy_forms_fltk_scale))
#define SELECT_WIDTH  (OY_ROUND(176 * oy_forms_fltk_scale))

namespace oyranos {
namespace forms {


class OyFl_Box_c : public Fl_Box {
public:
  OyFl_Box_c(int x, int y, int w, int h , const char *t = 0);
  ~OyFl_Box_c() {};
private:
  void draw();
  char *label_orig; 
public:
  const char * label( );
  void label(const char * l);
  void copy_label(const char* l);
  oyCallback_s * hint_callback;
  virtual int handle(int);
};

class OyFl_Pack_c : public Fl_Pack {
public:
  OyFl_Pack_c(int x, int y, int w, int h , const char *t = 0);
private:
  void draw();
};

#if 0
class OyOption_c : public OyFl_Pack_c {
public:
  OyFl_Box_c   *box;
  Fl_Choice*choice;

  int       i; 
  OyOption_c( int x, int y, int w, int h, const char *name,
  int choices_n,
  const char **choices,
  int current,
  const char *tooltip,
  int flags) ;
};
#endif

}
}

#endif /* OYRANOS_WIDGETS_FLTK.H */
