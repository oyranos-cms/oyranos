/** @file oyFilterPlug_s_.c

   [Template file inheritance graph]
   +-> oyFilterPlug_s_.template.c
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



  
#include "oyFilterPlug_s.h"
#include "oyFilterPlug_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  


/* Include "FilterPlug.private_custom_definitions.c" { */
/** Function    oyFilterPlug_Release__Members
 *  @memberof   oyFilterPlug_s
 *  @brief      Custom FilterPlug destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  filterplug  the FilterPlug object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyFilterPlug_Release__Members( oyFilterPlug_s_ * filterplug )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &filterplug->member );
   */
  oyFilterNode_Release( (oyFilterNode_s**)&filterplug->node );

  oyFilterSocket_Callback( (oyFilterPlug_s*)filterplug, oyCONNECTOR_EVENT_RELEASED );
  oyFilterSocket_Release( (oyFilterSocket_s**)&filterplug->remote_socket_ );

  oyConnector_Release( &filterplug->pattern );

  if(filterplug->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = filterplug->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( filterplug->member );
     */
    if(filterplug->relatives_)
      deallocateFunc( filterplug->relatives_ );
    filterplug->relatives_ = 0;
  }
}

/** Function    oyFilterPlug_Init__Members
 *  @memberof   oyFilterPlug_s
 *  @brief      Custom FilterPlug constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  filterplug  the FilterPlug object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterPlug_Init__Members( oyFilterPlug_s_ * filterplug )
{
  return 0;
}

/** Function    oyFilterPlug_Copy__Members
 *  @memberof   oyFilterPlug_s
 *  @brief      Custom FilterPlug copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyFilterPlug_s_ input object
 *  @param[out]  dst  the output oyFilterPlug_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterPlug_Copy__Members( oyFilterPlug_s_ * dst, oyFilterPlug_s_ * src)
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
  dst->pattern = oyConnector_Copy( src->pattern, dst->oy_ );
  dst->node = (oyFilterNode_s_*)oyFilterNode_Copy( (oyFilterNode_s*)src->node, 0 );

  return error;
}

/* } Include "FilterPlug.private_custom_definitions.c" */



/** @internal
 *  Function oyFilterPlug_New_
 *  @memberof oyFilterPlug_s_
 *  @brief   allocate a new oyFilterPlug_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterPlug_s_ * oyFilterPlug_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_PLUG_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyFilterPlug_s_ * s = 0;

  if(s_obj)
    s = (oyFilterPlug_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlug_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyFilterPlug_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyFilterPlug_Copy;
  s->release = (oyStruct_Release_f) oyFilterPlug_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom FilterPlug constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_FILTER_PLUG_S, (oyPointer)s );
  /* ---- end of custom FilterPlug constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom FilterPlug constructor ----- */
  error += oyFilterPlug_Init__Members( s );
  /* ---- end of custom FilterPlug constructor ------- */
  
  
  
  

  if(error)
    WARNc1_S("%d", error);

  return s;
}

/** @internal
 *  Function oyFilterPlug_Copy__
 *  @memberof oyFilterPlug_s_
 *  @brief   real copy a FilterPlug object
 *
 *  @param[in]     filterplug                 FilterPlug struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterPlug_s_ * oyFilterPlug_Copy__ ( oyFilterPlug_s_ *filterplug, oyObject_s object )
{
  oyFilterPlug_s_ *s = 0;
  int error = 0;

  if(!filterplug || !object)
    return s;

  s = (oyFilterPlug_s_*) oyFilterPlug_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom FilterPlug copy constructor ----- */
    error = oyFilterPlug_Copy__Members( s, filterplug );
    /* ---- end of custom FilterPlug copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyFilterPlug_Release_( &s );

  return s;
}

/** @internal
 *  Function oyFilterPlug_Copy_
 *  @memberof oyFilterPlug_s_
 *  @brief   copy or reference a FilterPlug object
 *
 *  @param[in]     filterplug                 FilterPlug struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterPlug_s_ * oyFilterPlug_Copy_ ( oyFilterPlug_s_ *filterplug, oyObject_s object )
{
  oyFilterPlug_s_ *s = filterplug;

  if(!filterplug)
    return 0;

  if(filterplug && !object)
  {
    s = filterplug;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilterPlug_Copy__( filterplug, object );

  return s;
}
 
/** @internal
 *  Function oyFilterPlug_Release_
 *  @memberof oyFilterPlug_s_
 *  @brief   release and possibly deallocate a FilterPlug object
 *
 *  @param[in,out] filterplug                 FilterPlug struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyFilterPlug_Release_( oyFilterPlug_s_ **filterplug )
{
  /* ---- start of common object destructor ----- */
  oyFilterPlug_s_ *s = 0;

  if(!filterplug || !*filterplug)
    return 0;

  s = *filterplug;

  *filterplug = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom FilterPlug destructor ----- */
  oyFilterPlug_Release__Members( s );
  /* ---- end of custom FilterPlug destructor ------- */
  
  
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "FilterPlug.private_methods_definitions.c" { */

/* } Include "FilterPlug.private_methods_definitions.c" */

