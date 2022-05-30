/** @file NamedColor.cc

   [Template file inheritance graph]
   +-> NamedColor.template.cc
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



#include "NamedColor.hh"

using namespace oy;


NamedColor::NamedColor(Object object)
{
  m_oy = oyNamedColor_New(object);
}

NamedColor::~NamedColor()
{
  oyNamedColor_Release(&m_oy);
}

// Overloaded constructor methods


// Specific NamedColor methods


// Static functions

