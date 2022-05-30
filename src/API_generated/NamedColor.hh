/** @file NamedColor.hh

   [Template file inheritance graph]
   +-> NamedColor.template.hh
   |
   +-- Base.hh

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef NAMED_COLOR_HH
#define NAMED_COLOR_HH

  
  
#include "oyNamedColor_s.h"

namespace oy {

class NamedColor {
  private:
    oyNamedColor_s * m_oy;

  public:
    NamedColor(Object object = NULL);

    // Overloaded constructor methods
    

    //NamedColor(const oyNamedColor_s* c);
    //NamedColor(const oyNamedColor_s_* c);
    //NamedColor(const NamedColor& c);
    ~NamedColor();

    // Generic NamedColor methods
    oyNamedColor_s * c_struct() { return m_oy; }

    // Specific NamedColor methods
    

    // Static functions
    
} // class NamedColor

} // namespace oy
#endif // NAMED_COLOR_HH
