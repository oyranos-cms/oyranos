/**
 *  Oyranos is an open source Color Management System 
 * 
 *  @par Copyright:
 *            2009-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2010/09/10
 *
 *  Classes to to build color managed widgets with Oyranos in FLTK.
 */

#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "oyranos_display_helpers.c"
#include "Oy_Widget.h"



#include <FL/Fl.H>
#include <FL/x.H>                /* fl_offscreen */

#include "Oy_Fl_Image_Widget.h"

#include <cmath>

extern "C" {
int      conversionObserve           ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data )
{
  int handled = 0;
  oyObserver_s * obs = observer;

  if(observer && observer->model &&
     observer->model->type_ == oyOBJECT_FILTER_NODE_S)
  {
    /*if(oy_debug_signals)*/
      printf("%s:%d INFO: \n\t%s %s: %s[%d]->%s[%d]\n",
                    strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__,
                    __LINE__, _("Signal"),
                    oySignalToString(signal_type),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );

    oyConversion_Correct( (oyConversion_s*)obs->observer,
                          "//" OY_TYPE_STD "/icc_color", oyOPTIONATTRIBUTE_ADVANCED,
                          0 );

    Oy_Fl_Image_Widget * oy_widget = (Oy_Fl_Image_Widget*) oyPointer_GetPointer(
                                             (oyPointer_s*)observer->user_data);
    oy_widget->damage( FL_DAMAGE_USER1 );

  }

  return handled;
}
}


