/** @file oyCMMapiFilters_s_.c

   [Template file inheritance graph]
   +-> oyCMMapiFilters_s_.template.c
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



  
#include "oyCMMapiFilters_s.h"
#include "oyCMMapiFilters_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"

  


/* Include "CMMapiFilters.private_custom_definitions.c" { */
/** Function    oyCMMapiFilters_Release__Members
 *  @memberof   oyCMMapiFilters_s
 *  @brief      Custom CMMapiFilters destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapifilters  the CMMapiFilters object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapiFilters_Release__Members( oyCMMapiFilters_s_ * cmmapifilters )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapifilters->member );
   */

  if(cmmapifilters->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmapifilters->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapifilters->member );
     */
  }
}

/** Function    oyCMMapiFilters_Init__Members
 *  @memberof   oyCMMapiFilters_s
 *  @brief      Custom CMMapiFilters constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapifilters  the CMMapiFilters object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapiFilters_Init__Members( oyCMMapiFilters_s_ * cmmapifilters )
{
  return 0;
}

/** Function    oyCMMapiFilters_Copy__Members
 *  @memberof   oyCMMapiFilters_s
 *  @brief      Custom CMMapiFilters copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapiFilters_s_ input object
 *  @param[out]  dst  the output oyCMMapiFilters_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapiFilters_Copy__Members( oyCMMapiFilters_s_ * dst, oyCMMapiFilters_s_ * src)
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

/* } Include "CMMapiFilters.private_custom_definitions.c" */



static int oy_cmmapifilters_init_ = 0;
static const char * oyCMMapiFilters_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyCMMapiFilters_s_ * s = (oyCMMapiFilters_s_*) obj;
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
 *  Function oyCMMapiFilters_New_
 *  @memberof oyCMMapiFilters_s_
 *  @brief   allocate a new oyCMMapiFilters_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapiFilters_s_ * oyCMMapiFilters_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_API_FILTERS_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyCMMapiFilters_s_ * s = 0;

  if(s_obj)
    s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyCMMapiFilters_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMapiFilters_Copy;
  s->release = (oyStruct_Release_f) oyCMMapiFilters_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom CMMapiFilters constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_FILTERS_S, (oyPointer)s );
  /* ---- end of custom CMMapiFilters constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  s->list_ = oyStructList_Create( s->type_, 0, 0 );


  
  /* ---- start of custom CMMapiFilters constructor ----- */
  error += oyCMMapiFilters_Init__Members( s );
  /* ---- end of custom CMMapiFilters constructor ------- */
  
  
  
  

  if(!oy_cmmapifilters_init_)
  {
    oy_cmmapifilters_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyCMMapiFilters_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyCMMapiFilters_Copy__
 *  @memberof oyCMMapiFilters_s_
 *  @brief   real copy a CMMapiFilters object
 *
 *  @param[in]     cmmapifilters                 CMMapiFilters struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapiFilters_s_ * oyCMMapiFilters_Copy__ ( oyCMMapiFilters_s_ *cmmapifilters, oyObject_s object )
{
  oyCMMapiFilters_s_ *s = 0;
  int error = 0;

  if(!cmmapifilters || !object)
    return s;

  s = (oyCMMapiFilters_s_*) oyCMMapiFilters_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom CMMapiFilters copy constructor ----- */
    error = oyCMMapiFilters_Copy__Members( s, cmmapifilters );
    /* ---- end of custom CMMapiFilters copy constructor ------- */
    
    
    
    
    
    s->list_ = oyStructList_Copy( cmmapifilters->list_, s->oy_ );

  }

  if(error)
    oyCMMapiFilters_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMapiFilters_Copy_
 *  @memberof oyCMMapiFilters_s_
 *  @brief   copy or reference a CMMapiFilters object
 *
 *  @param[in]     cmmapifilters                 CMMapiFilters struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapiFilters_s_ * oyCMMapiFilters_Copy_ ( oyCMMapiFilters_s_ *cmmapifilters, oyObject_s object )
{
  oyCMMapiFilters_s_ *s = cmmapifilters;

  if(!cmmapifilters)
    return 0;

  if(cmmapifilters && !object)
  {
    s = cmmapifilters;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCMMapiFilters_Copy__( cmmapifilters, object );

  return s;
}
 
/** @internal
 *  Function oyCMMapiFilters_Release_
 *  @memberof oyCMMapiFilters_s_
 *  @brief   release and possibly deallocate a CMMapiFilters list
 *
 *  @param[in,out] cmmapifilters                 CMMapiFilters struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyCMMapiFilters_Release_( oyCMMapiFilters_s_ **cmmapifilters )
{
  /* ---- start of common object destructor ----- */
  oyCMMapiFilters_s_ *s = 0;

  if(!cmmapifilters || !*cmmapifilters)
    return 0;

  s = *cmmapifilters;

  *cmmapifilters = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom CMMapiFilters destructor ----- */
  oyCMMapiFilters_Release__Members( s );
  /* ---- end of custom CMMapiFilters destructor ------- */
  
  
  
  

  oyStructList_Release( &s->list_ );


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "CMMapiFilters.private_methods_definitions.c" { */

/* } Include "CMMapiFilters.private_methods_definitions.c" */

