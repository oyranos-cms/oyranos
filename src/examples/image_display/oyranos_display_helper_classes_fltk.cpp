/**
 *  Oyranos is an open source Color Management System 
 * 
 *  @par Copyright:
 *            2009-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
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
                                       oyStruct_s        * signal_data OY_UNUSED )
{
  int handled = 0;
  oyObserver_s * obs = observer;

  if(observer && observer->model &&
     observer->model->type_ == oyOBJECT_FILTER_NODE_S)
  {
    if(oy_debug || oy_debug_signals)
      fprintf( stderr, "INFO: %s:%d \n\t%s %s: %s[%d]->%s[%d]"
               "\n\tCALLING: oyConversion_Correct() + oy_widget->damage()\n",
                    strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__,
                    __LINE__, _("Signal"),
                    oySignalToString(signal_type),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );

    oyConversion_Correct( (oyConversion_s*)obs->observer,
                          "//" OY_TYPE_STD "/icc_color",
                          oyOPTIONATTRIBUTE_ADVANCED, 0 );

    Oy_Widget * oy_widget = (Oy_Widget*) oyPointer_GetPointer(
                                             (oyPointer_s*)observer->user_data);
    if(!oy_widget)
      fprintf( stderr, "INFO: %s:%d found no Oy_Widget object: %s\n",
                    strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__,
                    __LINE__, oyPointer_GetResourceName( (oyPointer_s*)observer->user_data));
    Oy_Fl_Image_Widget * oy_image_widget = dynamic_cast<Oy_Fl_Image_Widget*>(oy_widget);

    if(!oy_image_widget)
      fprintf( stderr, "INFO: %s:%d found no Oy_Fl_Image_Widget object\n",
                    strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__,
                    __LINE__);
    else
      oy_image_widget->damage( FL_DAMAGE_USER1 );
  }

  return handled;
}
}


