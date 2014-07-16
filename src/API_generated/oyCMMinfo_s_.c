/** @file oyCMMinfo_s_.c

   [Template file inheritance graph]
   +-> oyCMMinfo_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2014 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2014/07/07
 */



  
#include "oyCMMinfo_s.h"
#include "oyCMMinfo_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"

  


/* Include "CMMinfo.private_custom_definitions.c" { */
/** Function    oyCMMinfo_Release__Members
 *  @memberof   oyCMMinfo_s
 *  @brief      Custom CMMinfo destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmminfo  the CMMinfo object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMinfo_Release__Members( oyCMMinfo_s_ * cmminfo )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmminfo->member );
   */

  if(cmminfo->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = cmminfo->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmminfo->member );
     */

    if(cmminfo->backend_version)
      deallocateFunc( cmminfo->backend_version );
  }
}

/** Function    oyCMMinfo_Init__Members
 *  @memberof   oyCMMinfo_s
 *  @brief      Custom CMMinfo constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmminfo  the CMMinfo object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMinfo_Init__Members( oyCMMinfo_s_ * cmminfo )
{
  return 0;
}

/** Function    oyCMMinfo_Copy__Members
 *  @memberof   oyCMMinfo_s
 *  @brief      Custom CMMinfo copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMinfo_s_ input object
 *  @param[out]  dst  the output oyCMMinfo_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMinfo_Copy__Members( oyCMMinfo_s_ * dst, oyCMMinfo_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;
#if 0
  oyDeAlloc_f deallocateFunc_ = 0;
#endif
  int error = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
#if 0
  deallocateFunc_ = dst->oy_->deallocateFunc_;
#endif

  /* Copy each value of src to dst here */
  error = !memcpy(dst->cmm, src->cmm, 8);

  if(src->backend_version)
    dst->backend_version = oyStringCopy_( src->backend_version, allocateFunc_ );

  dst->getText = src->getText;

  dst->oy_compatibility = src->oy_compatibility;

  return error;
}

/* } Include "CMMinfo.private_custom_definitions.c" */



/** @internal
 *  Function oyCMMinfo_New_
 *  @memberof oyCMMinfo_s_
 *  @brief   allocate a new oyCMMinfo_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMinfo_s_ * oyCMMinfo_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_INFO_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyCMMinfo_s_ * s = 0;

  if(s_obj)
    s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyCMMinfo_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMinfo_Copy;
  s->release = (oyStruct_Release_f) oyCMMinfo_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom CMMinfo constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_INFO_S, (oyPointer)s );
  /* ---- end of custom CMMinfo constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom CMMinfo constructor ----- */
  error += oyCMMinfo_Init__Members( s );
  /* ---- end of custom CMMinfo constructor ------- */
  
  
  
  

  if(error)
    WARNc1_S("%d", error);

  return s;
}

/** @internal
 *  Function oyCMMinfo_Copy__
 *  @memberof oyCMMinfo_s_
 *  @brief   real copy a CMMinfo object
 *
 *  @param[in]     cmminfo                 CMMinfo struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMinfo_s_ * oyCMMinfo_Copy__ ( oyCMMinfo_s_ *cmminfo, oyObject_s object )
{
  oyCMMinfo_s_ *s = 0;
  int error = 0;

  if(!cmminfo || !object)
    return s;

  s = (oyCMMinfo_s_*) oyCMMinfo_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom CMMinfo copy constructor ----- */
    error = oyCMMinfo_Copy__Members( s, cmminfo );
    /* ---- end of custom CMMinfo copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyCMMinfo_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMinfo_Copy_
 *  @memberof oyCMMinfo_s_
 *  @brief   copy or reference a CMMinfo object
 *
 *  @param[in]     cmminfo                 CMMinfo struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMinfo_s_ * oyCMMinfo_Copy_ ( oyCMMinfo_s_ *cmminfo, oyObject_s object )
{
  oyCMMinfo_s_ *s = cmminfo;

  if(!cmminfo)
    return 0;

  if(cmminfo && !object)
  {
    s = cmminfo;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCMMinfo_Copy__( cmminfo, object );

  return s;
}
 
/** @internal
 *  Function oyCMMinfo_Release_
 *  @memberof oyCMMinfo_s_
 *  @brief   release and possibly deallocate a CMMinfo object
 *
 *  @param[in,out] cmminfo                 CMMinfo struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyCMMinfo_Release_( oyCMMinfo_s_ **cmminfo )
{
  /* ---- start of common object destructor ----- */
  oyCMMinfo_s_ *s = 0;

  if(!cmminfo || !*cmminfo)
    return 0;

  s = *cmminfo;

  *cmminfo = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom CMMinfo destructor ----- */
  oyCMMinfo_Release__Members( s );
  /* ---- end of custom CMMinfo destructor ------- */
  
  
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "CMMinfo.private_methods_definitions.c" { */

/* } Include "CMMinfo.private_methods_definitions.c" */

