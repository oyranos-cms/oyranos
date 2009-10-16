/** @file oyranos_forms.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    XFORMS renderer arguments handling
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/10/12
 */



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_forms.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_alpha_internal.h"



/** Function oyFormsArgs_New
 *  @memberof oyFormsArgs_s
 *  @brief   allocate a new FormsArgs object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/12 (Oyranos: 0.1.10)
 *  @date    2009/10/12
 */
OYAPI oyFormsArgs_s * OYEXPORT
           oyFormsArgs_New           ( void              * object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FORMS_ARGS_S;
# define STRUCT_TYPE oyFormsArgs_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;

  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = oyFormsArgs_Copy;
  s->release = oyFormsArgs_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */


  return s;
}

/** @internal
 *  Function oyFormsArgs_Copy_
 *  @memberof oyFormsArgs_s
 *  @brief   real copy a FormsArgs object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/12 (Oyranos: 0.1.10)
 *  @date    2009/10/12
 */
oyFormsArgs_s * oyFormsArgs_Copy_
                                     ( oyFormsArgs_s     * obj,
                                       oyObject_s          object )
{
  oyFormsArgs_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!obj || !object)
    return s;

  s = oyFormsArgs_New( object );
  error = !s;

  if(!error)
  {
    allocateFunc_ = ((oyObject_s)s->oy_)->allocateFunc_;
  }

  if(error)
    oyFormsArgs_Release( &s );

  return s;
}

/** Function oyFormsArgs_Copy
 *  @memberof oyFormsArgs_s
 *  @brief   copy or reference a FormsArgs object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/12 (Oyranos: 0.1.10)
 *  @date    2009/10/12
 */
OYAPI oyFormsArgs_s * OYEXPORT
           oyFormsArgs_Copy          ( oyFormsArgs_s     * obj,
                                       void              * object )
{
  oyFormsArgs_s * s = obj;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_FORMS_ARGS_S, return 0 )

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( (oyObject_s)s->oy_ );
    return s;
  }

  s = oyFormsArgs_Copy_( obj, object );

  return s;
}
 
/** Function oyFormsArgs_Release
 *  @memberof oyFormsArgs_s
 *  @brief   release and possibly deallocate a FormsArgs object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/12 (Oyranos: 0.1.10)
 *  @date    2009/10/12
 */
OYAPI int  OYEXPORT
           oyFormsArgs_Release       ( oyFormsArgs_s    ** obj )
{
  /* ---- start of common object destructor ----- */
  oyFormsArgs_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_FORMS_ARGS_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef((oyObject_s)s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */


  if(((oyObject_s)s->oy_)->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = ((oyObject_s)s->oy_)->deallocateFunc_;

    oyObject_Release( (oyObject_s*)&s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** Function oyFormsArgs_ModelGet
 *  @memberof oyFormsArgs_s
 *  @brief   obtain a XFORMS model from a FormsArgs object
 *
 *  @param[in,out] args                struct object
 *  @param[in]     alloc               user allocate function
 *  @return                            the model XML
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/12 (Oyranos: 0.1.10)
 *  @date    2009/10/12
 */
const char * oyFormsArgs_ModelGet    ( oyFormsArgs_s     * args )
{
  if(args)
    return oyOptions_GetText( args->xforms_data_model_, oyNAME_NAME );
  else
    return 0;
}



