/** @file oyCMMapis_s_.c

   [Template file inheritance graph]
   +-> oyCMMapis_s_.template.c
   |
   +-> BaseList_s_.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2017 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



  
#include "oyCMMapis_s.h"
#include "oyCMMapis_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"

  


/* Include "CMMapis.private_custom_definitions.c" { */
/** Function    oyCMMapis_Release__Members
 *  @memberof   oyCMMapis_s
 *  @brief      Custom CMMapis destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapis  the CMMapis object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapis_Release__Members( oyCMMapis_s_ * cmmapis )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapis->member );
   */

  if(cmmapis->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmapis->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapis->member );
     */
  }
}

/** Function    oyCMMapis_Init__Members
 *  @memberof   oyCMMapis_s
 *  @brief      Custom CMMapis constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapis  the CMMapis object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapis_Init__Members( oyCMMapis_s_ * cmmapis )
{
  return 0;
}

/** Function    oyCMMapis_Copy__Members
 *  @memberof   oyCMMapis_s
 *  @brief      Custom CMMapis copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapis_s_ input object
 *  @param[out]  dst  the output oyCMMapis_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapis_Copy__Members( oyCMMapis_s_ * dst, oyCMMapis_s_ * src)
{
  int error = 0;
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

  return error;
}

/* } Include "CMMapis.private_custom_definitions.c" */



static int oy_cmmapis_init_ = 0;
static const char * oyCMMapis_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyCMMapis_s_ * s = (oyCMMapis_s_*) obj;
  static char * text = 0;
  static int text_n = 0;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if(s->oy_ && s->oy_->allocateFunc_)
    alloc = s->oy_->allocateFunc_;

  if( text == NULL || text_n == 0 )
  {
    text_n = 128;
    text = (char*) alloc( text_n );
    if(text)
      memset( text, 0, text_n );
  }

  if( text == NULL || text_n == 0 )
    return "Memory problem";

  text[0] = '\000';

  if(!(flags & 0x01))
    sprintf(text, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return text;
}
/** @internal
 *  Function oyCMMapis_New_
 *  @memberof oyCMMapis_s_
 *  @brief   allocate a new oyCMMapis_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapis_s_ * oyCMMapis_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_APIS_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyCMMapis_s_ * s = 0;

  if(s_obj)
    s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyCMMapis_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMapis_Copy;
  s->release = (oyStruct_Release_f) oyCMMapis_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom CMMapis constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_APIS_S, (oyPointer)s );
  /* ---- end of custom CMMapis constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  s->list_ = oyStructList_Create( s->type_, 0, 0 );


  
  /* ---- start of custom CMMapis constructor ----- */
  error += oyCMMapis_Init__Members( s );
  /* ---- end of custom CMMapis constructor ------- */
  
  
  
  

  if(!oy_cmmapis_init_)
  {
    oy_cmmapis_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyCMMapis_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyCMMapis_Copy__
 *  @memberof oyCMMapis_s_
 *  @brief   real copy a CMMapis object
 *
 *  @param[in]     cmmapis                 CMMapis struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapis_s_ * oyCMMapis_Copy__ ( oyCMMapis_s_ *cmmapis, oyObject_s object )
{
  oyCMMapis_s_ *s = 0;
  int error = 0;

  if(!cmmapis || !object)
    return s;

  s = (oyCMMapis_s_*) oyCMMapis_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom CMMapis copy constructor ----- */
    error = oyCMMapis_Copy__Members( s, cmmapis );
    /* ---- end of custom CMMapis copy constructor ------- */
    
    
    
    
    
    s->list_ = oyStructList_Copy( cmmapis->list_, s->oy_ );

  }

  if(error)
    oyCMMapis_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMapis_Copy_
 *  @memberof oyCMMapis_s_
 *  @brief   copy or reference a CMMapis object
 *
 *  @param[in]     cmmapis                 CMMapis struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapis_s_ * oyCMMapis_Copy_ ( oyCMMapis_s_ *cmmapis, oyObject_s object )
{
  oyCMMapis_s_ *s = cmmapis;

  if(!cmmapis)
    return 0;

  if(cmmapis && !object)
  {
    s = cmmapis;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCMMapis_Copy__( cmmapis, object );

  return s;
}
 
/** @internal
 *  Function oyCMMapis_Release_
 *  @memberof oyCMMapis_s_
 *  @brief   release and possibly deallocate a CMMapis list
 *
 *  @param[in,out] cmmapis                 CMMapis struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyCMMapis_Release_( oyCMMapis_s_ **cmmapis )
{
  /* ---- start of common object destructor ----- */
  oyCMMapis_s_ *s = 0;

  if(!cmmapis || !*cmmapis)
    return 0;

  s = *cmmapis;

  *cmmapis = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom CMMapis destructor ----- */
  oyCMMapis_Release__Members( s );
  /* ---- end of custom CMMapis destructor ------- */
  
  
  
  

  oyStructList_Release( &s->list_ );


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "CMMapis.private_methods_definitions.c" { */

/* } Include "CMMapis.private_methods_definitions.c" */

