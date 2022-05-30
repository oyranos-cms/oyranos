/** @file Lis.cc

   [Template file inheritance graph]
   +-> Lis.template.cc
   |
   +-- Base.cc

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



#include "Lis.hh"

using namespace oy;


Lis::Lis(Object object)
{
  m_oy = oyLis_New(object);
}

Lis::~Lis()
{
  oyLis_Release(&m_oy);
}

// Overloaded constructor methods


// Specific Lis methods


// Static functions

