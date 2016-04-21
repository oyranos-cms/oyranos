/** @file oyCMMapiFilter_s_.c

   [Template file inheritance graph]
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



  
#include "oyCMMapiFilter_s.h"
#include "oyCMMapiFilter_s_.h"


#include "oyCMMapi_s_.h"




#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"



#include "oyCMMapiFilter_s_.h"
  


/* Include "CMMapiFilter.private_custom_definitions.c" { */
/** Function    oyCMMapiFilter_Release__Members
 *  @memberof   oyCMMapiFilter_s
 *  @brief      Custom CMMapiFilter destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapifilter  the CMMapiFilter object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapiFilter_Release__Members( oyCMMapiFilter_s_ * cmmapifilter )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapifilter->member );
   */

  if(cmmapifilter->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmapifilter->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapifilter->member );
     */
  }
}

/** Function    oyCMMapiFilter_Init__Members
 *  @memberof   oyCMMapiFilter_s
 *  @brief      Custom CMMapiFilter constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapifilter  the CMMapiFilter object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapiFilter_Init__Members( oyCMMapiFilter_s_ * cmmapifilter )
{
  return 0;
}

/** Function    oyCMMapiFilter_Copy__Members
 *  @memberof   oyCMMapiFilter_s
 *  @brief      Custom CMMapiFilter copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapiFilter_s_ input object
 *  @param[out]  dst  the output oyCMMapiFilter_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapiFilter_Copy__Members( oyCMMapiFilter_s_ * dst, oyCMMapiFilter_s_ * src)
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

/* } Include "CMMapiFilter.private_custom_definitions.c" */



static int oy_cmmapifilter_init_ = 0;
static const char * oyCMMapiFilter_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyCMMapiFilter_s_ * s = (oyCMMapiFilter_s_*) obj;
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
 *  Function oyCMMapiFilter_New_
 *  @memberof oyCMMapiFilter_s_
 *  @brief   allocate a new oyCMMapiFilter_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapiFilter_s_ * oyCMMapiFilter_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_API_FILTER_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyCMMapiFilter_s_ * s = 0;

  if(s_obj)
    s = (oyCMMapiFilter_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilter_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyCMMapiFilter_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMapiFilter_Copy;
  s->release = (oyStruct_Release_f) oyCMMapiFilter_Release;

  s->oy_ = s_obj;

  
  
  /* ---- start of custom CMMapi constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_S, (oyPointer)s );
  /* ---- end of custom CMMapi constructor ------- */
  /* ---- start of custom CMMapiFilter constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_FILTER_S, (oyPointer)s );
  /* ---- end of custom CMMapiFilter constructor ------- */
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  
  /* ---- start of custom CMMapi constructor ----- */
  error += oyCMMapi_Init__Members( (oyCMMapi_s_*)s );
  /* ---- end of custom CMMapi constructor ------- */
  /* ---- start of custom CMMapiFilter constructor ----- */
  error += oyCMMapiFilter_Init__Members( s );
  /* ---- end of custom CMMapiFilter constructor ------- */
  
  
  

  if(!oy_cmmapifilter_init_)
  {
    oy_cmmapifilter_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyCMMapiFilter_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyCMMapiFilter_Copy__
 *  @memberof oyCMMapiFilter_s_
 *  @brief   real copy a CMMapiFilter object
 *
 *  @param[in]     cmmapifilter                 CMMapiFilter struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapiFilter_s_ * oyCMMapiFilter_Copy__ ( oyCMMapiFilter_s_ *cmmapifilter, oyObject_s object )
{
  oyCMMapiFilter_s_ *s = 0;
  int error = 0;

  if(!cmmapifilter || !object)
    return s;

  s = (oyCMMapiFilter_s_*) oyCMMapiFilter_New( object );
  error = !s;

  if(!error) {
    
    
    /* ---- start of custom CMMapi copy constructor ----- */
    error = oyCMMapi_Copy__Members( (oyCMMapi_s_*)s, (oyCMMapi_s_*)cmmapifilter );
    /* ---- end of custom CMMapi copy constructor ------- */
    /* ---- start of custom CMMapiFilter copy constructor ----- */
    error = oyCMMapiFilter_Copy__Members( s, cmmapifilter );
    /* ---- end of custom CMMapiFilter copy constructor ------- */
    
    
    
    
    
  }

  if(error)
    oyCMMapiFilter_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMapiFilter_Copy_
 *  @memberof oyCMMapiFilter_s_
 *  @brief   copy or reference a CMMapiFilter object
 *
 *  @param[in]     cmmapifilter                 CMMapiFilter struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapiFilter_s_ * oyCMMapiFilter_Copy_ ( oyCMMapiFilter_s_ *cmmapifilter, oyObject_s object )
{
  oyCMMapiFilter_s_ *s = cmmapifilter;

  if(!cmmapifilter)
    return 0;

  if(cmmapifilter && !object)
  {
    s = cmmapifilter;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCMMapiFilter_Copy__( cmmapifilter, object );

  return s;
}
 
/** @internal
 *  Function oyCMMapiFilter_Release_
 *  @memberof oyCMMapiFilter_s_
 *  @brief   release and possibly deallocate a CMMapiFilter object
 *
 *  @param[in,out] cmmapifilter                 CMMapiFilter struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyCMMapiFilter_Release_( oyCMMapiFilter_s_ **cmmapifilter )
{
  /* ---- start of common object destructor ----- */
  oyCMMapiFilter_s_ *s = 0;

  if(!cmmapifilter || !*cmmapifilter)
    return 0;

  s = *cmmapifilter;

  *cmmapifilter = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  
  /* ---- start of custom CMMapi destructor ----- */
  oyCMMapi_Release__Members( (oyCMMapi_s_*)s );
  /* ---- end of custom CMMapi destructor ------- */
  /* ---- start of custom CMMapiFilter destructor ----- */
  oyCMMapiFilter_Release__Members( s );
  /* ---- end of custom CMMapiFilter destructor ------- */
  
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "CMMapiFilter.private_methods_definitions.c" { */

/* } Include "CMMapiFilter.private_methods_definitions.c" */

