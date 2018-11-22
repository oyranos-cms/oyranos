/**
 *  Oyranos is an open source Color Management System 
 * 
 *  @par Copyright:
 *            2010 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2010/09/10
 *
 *  Helpers to create color managed widgets with Oyranos.
 */

#ifndef Oy_Widget_H
#define Oy_Widget_H

#include <oyranos.h>
#include <oyranos_alpha.h>

#include "oyranos_config_internal.h" /* I18N */
#include "oyranos_i18n.h"
#if defined(HAVE_XCM)
#include <X11/Xcm/Xcm.h>             /* check for proper X11/XCM support */
#endif

class Oy_Widget
{
  oyConversion_s * context;
  oyPixelAccess_s * pixel_access;

public:
  oyRectangle_s * old_display_rectangle;
  oyRectangle_s * old_roi_rectangle;
  int dirty;

  Oy_Widget()
  {
    context = 0;
    pixel_access = 0;
    old_display_rectangle = oyRectangle_NewWith( 0,0,0,0, 0 );
    old_roi_rectangle = oyRectangle_NewWith( 0,0,0,0, 0 );
    dirty = 0;
  };

  ~Oy_Widget(void)
  {
    oyConversion_Release( &context );
    oyPixelAccess_Release( &pixel_access );
    oyRectangle_Release( &old_display_rectangle );
    oyRectangle_Release( &old_roi_rectangle );
  };

  virtual void conversion( oyConversion_s * c ) 
  {
    oyConversion_Release( &context );
    context = oyConversion_Copy( c, 0 );
    oyPixelAccess_Release( &pixel_access );
  }
  virtual oyConversion_s * conversion() { return context; }

  virtual void ticket( oyPixelAccess_s ** t ) 
  {
    oyPixelAccess_Release( &pixel_access );
    pixel_access = *t;
    *t = NULL;
  }
  virtual oyPixelAccess_s * ticket() { return pixel_access; }

  oyFilterNode_s * setImageType( oyImage_s * image, oyImage_s * display_image, oyDATATYPE_e data_type, oyOptions_s * cc_options )
  {
    oyFilterNode_s * icc = 0;
    oyConversion_s * c = oyConversion_FromImageForDisplay( 
                             image, display_image, &icc, oyOPTIONATTRIBUTE_ADVANCED,
                             data_type, cc_options, 0 );

    conversion( c );

    oyConversion_Release( &c );

    return icc;
  }

  void observeICC(                     oyFilterNode_s * icc,
                     int(*observator)( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data ) )
  {
    oyPointer_s * oy_ptr = oyPointer_New(0);
    oyPointer_Set( oy_ptr,
                   __FILE__,
                   "Oy_Widget*",
                   this, 0, 0 );
    /* add a asynchronous oyJob_s for expensive transforms */
    oyOptions_s * module_options = oyFilterNode_GetOptions( icc, 0 );
    oyJob_s * job = oyJob_New(0);
    job->cb_progress = iccProgressCallback;
    job->cb_progress_context = (oyStruct_s*) oyPointer_Copy( oy_ptr, 0 );
    oyOptions_MoveInStruct( &module_options, OY_BEHAVIOUR_STD "/expensive_callback", (oyStruct_s**)&job, OY_CREATE_NEW );
    /* wait no longer than approximately 2 seconds */
    oyOptions_SetFromString( &module_options, OY_BEHAVIOUR_STD "/expensive", "11", OY_CREATE_NEW );

    /* observe the icc node */
    oyStruct_ObserverAdd( (oyStruct_s*)icc, (oyStruct_s*)conversion(),
                          (oyStruct_s*)oy_ptr,
                          observator );
    oyPointer_Release( &oy_ptr );
  }

  virtual void markDirty() { dirty = 1; }

  static void iccProgressCallback (    double              progress_zero_till_one,
                                       char              * status_text OY_UNUSED,
                                       int                 thread_id_,
                                       int                 job_id,
                                       oyStruct_s        * cb_progress_context )
  {
    oyPointer_s * context = (oyPointer_s *) cb_progress_context;
    Oy_Widget * oy_widget = (Oy_Widget*) oyPointer_GetPointer( context );
    printf( "%s() job_id: %d thread: %d %g\n", __func__, job_id, thread_id_,
            progress_zero_till_one );
    if(progress_zero_till_one >= 1.0)
    {
      oy_widget->markDirty();
    }
  }
};


#endif /* Oy_Widget_H */
