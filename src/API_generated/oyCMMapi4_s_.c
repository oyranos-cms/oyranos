/** @file oyCMMapi4_s_.c

   [Template file inheritance graph]
   +-> oyCMMapi4_s_.template.c
   |
   +-> oyCMMapiFilter_s_.template.c
   |
   +-> oyCMMapi_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2016 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



  
#include "oyCMMapi4_s.h"
#include "oyCMMapi4_s_.h"



#include "oyCMMapi_s_.h"
#include "oyCMMapiFilter_s_.h"



#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"



#include "oyCMMapiFilter_s_.h"
  


/* Include "CMMapi4.private_custom_definitions.c" { */
/** Function    oyCMMapi4_Release__Members
 *  @memberof   oyCMMapi4_s
 *  @brief      Custom CMMapi4 destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapi4  the CMMapi4 object
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/09 (Oyranos: 0.9.5)
 *  @date    2013/06/09
 */
void oyCMMapi4_Release__Members( oyCMMapi4_s_ * cmmapi4 )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapi4->member );
   */

  if(cmmapi4 && cmmapi4->oy_ && cmmapi4->oy_->deallocateFunc_)
  {
#   if 0
    oyDeAlloc_f deallocateFunc = cmmapi4->oy_->deallocateFunc_;
#   endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapi4->member );
     */

    oyCMMui_Release( (oyCMMui_s**) &cmmapi4->ui );
    oyCMMapiFilter_Release( (oyCMMapiFilter_s**)&cmmapi4 );
  }
}

/** Function    oyCMMapi4_Init__Members
 *  @memberof   oyCMMapi4_s
 *  @brief      Custom CMMapi4 constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapi4  the CMMapi4 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi4_Init__Members( oyCMMapi4_s_ * cmmapi4 )
{
  return 0;
}

/** Function    oyCMMapi4_Copy__Members
 *  @memberof   oyCMMapi4_s
 *  @brief      Custom CMMapi4 copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapi4_s_ input object
 *  @param[out]  dst  the output oyCMMapi4_s_ object
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/09 (Oyranos: 0.9.5)
 *  @date    2013/06/09
 */
int oyCMMapi4_Copy__Members( oyCMMapi4_s_ * dst, oyCMMapi4_s_ * src)
{
  if(!dst || !src)
    return 1;

  /* Copy each value of src to dst here */

  memcpy( dst->context_type, src->context_type, 8 );
  dst->oyCMMFilterNode_ContextToMem = src->oyCMMFilterNode_ContextToMem;
  dst->oyCMMFilterNode_GetText = src->oyCMMFilterNode_GetText;
  dst->ui = (oyCMMui_s_*) oyCMMui_Copy( (oyCMMui_s*)src->ui, src->oy_ );

  return 0;
}

/* } Include "CMMapi4.private_custom_definitions.c" */



/** @internal
 *  Function oyCMMapi4_New_
 *  @memberof oyCMMapi4_s_
 *  @brief   allocate a new oyCMMapi4_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi4_s_ * oyCMMapi4_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_API4_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyCMMapi4_s_ * s = 0;

  if(s_obj)
    s = (oyCMMapi4_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi4_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyCMMapi4_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMapi4_Copy_x;
  s->release = (oyStruct_Release_f) oyCMMapi4_Release;

  s->oy_ = s_obj;

  
  
  
  /* ---- start of custom CMMapi constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_S, (oyPointer)s );
  /* ---- end of custom CMMapi constructor ------- */
  /* ---- start of custom CMMapiFilter constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_FILTER_S, (oyPointer)s );
  /* ---- end of custom CMMapiFilter constructor ------- */
  /* ---- start of custom CMMapi4 constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API4_S, (oyPointer)s );
  /* ---- end of custom CMMapi4 constructor ------- */
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  
  
  /* ---- start of custom CMMapi constructor ----- */
  error += oyCMMapi_Init__Members( (oyCMMapi_s_*)s );
  /* ---- end of custom CMMapi constructor ------- */
  /* ---- start of custom CMMapiFilter constructor ----- */
  error += oyCMMapiFilter_Init__Members( (oyCMMapiFilter_s_*)s );
  /* ---- end of custom CMMapiFilter constructor ------- */
  /* ---- start of custom CMMapi4 constructor ----- */
  error += oyCMMapi4_Init__Members( s );
  /* ---- end of custom CMMapi4 constructor ------- */
  
  

  if(error)
    WARNc1_S("%d", error);

  return s;
}

/** @internal
 *  Function oyCMMapi4_Copy__
 *  @memberof oyCMMapi4_s_
 *  @brief   real copy a CMMapi4 object
 *
 *  @param[in]     cmmapi4                 CMMapi4 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi4_s_ * oyCMMapi4_Copy__ ( oyCMMapi4_s_ *cmmapi4, oyObject_s object )
{
  oyCMMapi4_s_ *s = 0;
  int error = 0;

  if(!cmmapi4 || !object)
    return s;

  s = (oyCMMapi4_s_*) oyCMMapi4_New( object );
  error = !s;

  if(!error) {
    
    
    
    /* ---- start of custom CMMapi copy constructor ----- */
    error = oyCMMapi_Copy__Members( (oyCMMapi_s_*)s, (oyCMMapi_s_*)cmmapi4 );
    /* ---- end of custom CMMapi copy constructor ------- */
    /* ---- start of custom CMMapiFilter copy constructor ----- */
    error = oyCMMapiFilter_Copy__Members( (oyCMMapiFilter_s_*)s, (oyCMMapiFilter_s_*)cmmapi4 );
    /* ---- end of custom CMMapiFilter copy constructor ------- */
    /* ---- start of custom CMMapi4 copy constructor ----- */
    error = oyCMMapi4_Copy__Members( s, cmmapi4 );
    /* ---- end of custom CMMapi4 copy constructor ------- */
    
    
    
    
  }

  if(error)
    oyCMMapi4_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMapi4_Copy_
 *  @memberof oyCMMapi4_s_
 *  @brief   copy or reference a CMMapi4 object
 *
 *  @param[in]     cmmapi4                 CMMapi4 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi4_s_ * oyCMMapi4_Copy_ ( oyCMMapi4_s_ *cmmapi4, oyObject_s object )
{
  oyCMMapi4_s_ *s = cmmapi4;

  if(!cmmapi4)
    return 0;

  if(cmmapi4 && !object)
  {
    s = cmmapi4;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCMMapi4_Copy__( cmmapi4, object );

  return s;
}
 
/** @internal
 *  Function oyCMMapi4_Release_
 *  @memberof oyCMMapi4_s_
 *  @brief   release and possibly deallocate a CMMapi4 object
 *
 *  @param[in,out] cmmapi4                 CMMapi4 struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyCMMapi4_Release_( oyCMMapi4_s_ **cmmapi4 )
{
  /* ---- start of common object destructor ----- */
  oyCMMapi4_s_ *s = 0;

  if(!cmmapi4 || !*cmmapi4)
    return 0;

  s = *cmmapi4;

  *cmmapi4 = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  
  
  /* ---- start of custom CMMapi destructor ----- */
  oyCMMapi_Release__Members( (oyCMMapi_s_*)s );
  /* ---- end of custom CMMapi destructor ------- */
  /* ---- start of custom CMMapiFilter destructor ----- */
  oyCMMapiFilter_Release__Members( (oyCMMapiFilter_s_*)s );
  /* ---- end of custom CMMapiFilter destructor ------- */
  /* ---- start of custom CMMapi4 destructor ----- */
  oyCMMapi4_Release__Members( s );
  /* ---- end of custom CMMapi4 destructor ------- */
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "CMMapi4.private_methods_definitions.c" { */
/** @internal
 *  Function oyCMMapi4_SelectFilter_
 *  @brief   filter the desired api
 *  @memberof oyCMMapi_s_
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/16 (Oyranos: 0.1.9)
 *  @date    2008/12/16
 */
oyOBJECT_e   oyCMMapi4_SelectFilter_ ( oyCMMapi_s_       * api,
                                       oyPointer           data )
{
  oyOBJECT_e type = oyOBJECT_NONE,
             searched = oyOBJECT_CMM_API4_S;
  int error = !data || !api;
  oyRegistrationData_s * reg_filter;
  oyCMMapi4_s_ * cmm_api = (oyCMMapi4_s_ *) api;
  int found = 0;

  if(error <= 0)
    reg_filter = (oyRegistrationData_s*) data;

  if(error <= 0 &&
     api->type_ == searched &&
     reg_filter->type == searched)
  {
    if(reg_filter->registration)
    {
      if(oyFilterRegistrationMatch( cmm_api->registration,
                                    reg_filter->registration, api->type_ ))
        found = 1;
    } else
      found = 1;

    if( found )
      type = api->type_;
  }

  return type;
}

/* } Include "CMMapi4.private_methods_definitions.c" */

