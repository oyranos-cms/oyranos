/** @file oyCMMapi10_s_.c

   [Template file inheritance graph]
   +-> oyCMMapi10_s_.template.c
   |
   +-> oyCMMapiFilter_s_.template.c
   |
   +-> oyCMMapi_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2015 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



  
#include "oyCMMapi10_s.h"
#include "oyCMMapi10_s_.h"



#include "oyCMMapi_s_.h"
#include "oyCMMapiFilter_s_.h"



#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"



#include "oyCMMapiFilter_s_.h"
  


/* Include "CMMapi10.private_custom_definitions.c" { */
/** Function    oyCMMapi10_Release__Members
 *  @memberof   oyCMMapi10_s
 *  @brief      Custom CMMapi10 destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapi10  the CMMapi10 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapi10_Release__Members( oyCMMapi10_s_ * cmmapi10 )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapi10->member );
   */

  if(cmmapi10->oy_->deallocateFunc_)
  {
    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapi10->member );
     */
  }
}

/** Function    oyCMMapi10_Init__Members
 *  @memberof   oyCMMapi10_s
 *  @brief      Custom CMMapi10 constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapi10  the CMMapi10 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi10_Init__Members( oyCMMapi10_s_ * cmmapi10 )
{
  return 0;
}

/** Function    oyCMMapi10_Copy__Members
 *  @memberof   oyCMMapi10_s
 *  @brief      Custom CMMapi10 copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapi10_s_ input object
 *  @param[out]  dst  the output oyCMMapi10_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi10_Copy__Members( oyCMMapi10_s_ * dst, oyCMMapi10_s_ * src)
{
#if 0
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;
#endif

  if(!dst || !src)
    return 1;

#if 0
  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;
#endif

  /* Copy each value of src to dst here */

  return 0;
}

/* } Include "CMMapi10.private_custom_definitions.c" */



/** @internal
 *  Function oyCMMapi10_New_
 *  @memberof oyCMMapi10_s_
 *  @brief   allocate a new oyCMMapi10_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi10_s_ * oyCMMapi10_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_API10_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyCMMapi10_s_ * s = 0;

  if(s_obj)
    s = (oyCMMapi10_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi10_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyCMMapi10_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMapi10_Copy;
  s->release = (oyStruct_Release_f) oyCMMapi10_Release;

  s->oy_ = s_obj;

  
  
  
  /* ---- start of custom CMMapi constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_S, (oyPointer)s );
  /* ---- end of custom CMMapi constructor ------- */
  /* ---- start of custom CMMapiFilter constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_FILTER_S, (oyPointer)s );
  /* ---- end of custom CMMapiFilter constructor ------- */
  /* ---- start of custom CMMapi10 constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API10_S, (oyPointer)s );
  /* ---- end of custom CMMapi10 constructor ------- */
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  
  
  /* ---- start of custom CMMapi constructor ----- */
  error += oyCMMapi_Init__Members( (oyCMMapi_s_*)s );
  /* ---- end of custom CMMapi constructor ------- */
  /* ---- start of custom CMMapiFilter constructor ----- */
  error += oyCMMapiFilter_Init__Members( (oyCMMapiFilter_s_*)s );
  /* ---- end of custom CMMapiFilter constructor ------- */
  /* ---- start of custom CMMapi10 constructor ----- */
  error += oyCMMapi10_Init__Members( s );
  /* ---- end of custom CMMapi10 constructor ------- */
  
  

  if(error)
    WARNc1_S("%d", error);

  return s;
}

/** @internal
 *  Function oyCMMapi10_Copy__
 *  @memberof oyCMMapi10_s_
 *  @brief   real copy a CMMapi10 object
 *
 *  @param[in]     cmmapi10                 CMMapi10 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi10_s_ * oyCMMapi10_Copy__ ( oyCMMapi10_s_ *cmmapi10, oyObject_s object )
{
  oyCMMapi10_s_ *s = 0;
  int error = 0;

  if(!cmmapi10 || !object)
    return s;

  s = (oyCMMapi10_s_*) oyCMMapi10_New( object );
  error = !s;

  if(!error) {
    
    
    
    /* ---- start of custom CMMapi copy constructor ----- */
    error = oyCMMapi_Copy__Members( (oyCMMapi_s_*)s, (oyCMMapi_s_*)cmmapi10 );
    /* ---- end of custom CMMapi copy constructor ------- */
    /* ---- start of custom CMMapiFilter copy constructor ----- */
    error = oyCMMapiFilter_Copy__Members( (oyCMMapiFilter_s_*)s, (oyCMMapiFilter_s_*)cmmapi10 );
    /* ---- end of custom CMMapiFilter copy constructor ------- */
    /* ---- start of custom CMMapi10 copy constructor ----- */
    error = oyCMMapi10_Copy__Members( s, cmmapi10 );
    /* ---- end of custom CMMapi10 copy constructor ------- */
    
    
    
    
  }

  if(error)
    oyCMMapi10_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMapi10_Copy_
 *  @memberof oyCMMapi10_s_
 *  @brief   copy or reference a CMMapi10 object
 *
 *  @param[in]     cmmapi10                 CMMapi10 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi10_s_ * oyCMMapi10_Copy_ ( oyCMMapi10_s_ *cmmapi10, oyObject_s object )
{
  oyCMMapi10_s_ *s = cmmapi10;

  if(!cmmapi10)
    return 0;

  if(cmmapi10 && !object)
  {
    s = cmmapi10;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCMMapi10_Copy__( cmmapi10, object );

  return s;
}
 
/** @internal
 *  Function oyCMMapi10_Release_
 *  @memberof oyCMMapi10_s_
 *  @brief   release and possibly deallocate a CMMapi10 object
 *
 *  @param[in,out] cmmapi10                 CMMapi10 struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyCMMapi10_Release_( oyCMMapi10_s_ **cmmapi10 )
{
  /* ---- start of common object destructor ----- */
  oyCMMapi10_s_ *s = 0;

  if(!cmmapi10 || !*cmmapi10)
    return 0;

  s = *cmmapi10;

  *cmmapi10 = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  
  
  /* ---- start of custom CMMapi destructor ----- */
  oyCMMapi_Release__Members( (oyCMMapi_s_*)s );
  /* ---- end of custom CMMapi destructor ------- */
  /* ---- start of custom CMMapiFilter destructor ----- */
  oyCMMapiFilter_Release__Members( (oyCMMapiFilter_s_*)s );
  /* ---- end of custom CMMapiFilter destructor ------- */
  /* ---- start of custom CMMapi10 destructor ----- */
  oyCMMapi10_Release__Members( s );
  /* ---- end of custom CMMapi10 destructor ------- */
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "CMMapi10.private_methods_definitions.c" { */

/* } Include "CMMapi10.private_methods_definitions.c" */

