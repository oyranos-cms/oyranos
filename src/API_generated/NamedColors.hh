/** @file NamedColors.hh

   [Template file inheritance graph]
   +-> NamedColors.template.hh
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


#ifndef NAMED_COLORS_HH
#define NAMED_COLORS_HH

  
  
#include "oyNamedColors_s.h"

namespace oy {

class NamedColors {
  private:
    oyNamedColors_s * m_oy;

  public:
    NamedColors(Object object = NULL);

    // Overloaded constructor methods
    

    //NamedColors(const oyNamedColors_s* c);
    //NamedColors(const oyNamedColors_s_* c);
    //NamedColors(const NamedColors& c);
    ~NamedColors();

    // Generic NamedColors methods
    oyNamedColors_s * c_struct() { return m_oy; }

    // Specific NamedColors methods
    

    // Static functions
    
} // class NamedColors

} // namespace oy
#endif // NAMED_COLORS_HH
