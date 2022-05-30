/** @file Lis.hh

   [Template file inheritance graph]
   +-> Lis.template.hh
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


#ifndef LIS_HH
#define LIS_HH

  
  
#include "oyLis_s.h"

namespace oy {

class Lis {
  private:
    oyLis_s * m_oy;

  public:
    Lis(Object object = NULL);

    // Overloaded constructor methods
    

    //Lis(const oyLis_s* c);
    //Lis(const oyLis_s_* c);
    //Lis(const Lis& c);
    ~Lis();

    // Generic Lis methods
    oyLis_s * c_struct() { return m_oy; }

    // Specific Lis methods
    

    // Static functions
    
} // class Lis

} // namespace oy
#endif // LIS_HH
