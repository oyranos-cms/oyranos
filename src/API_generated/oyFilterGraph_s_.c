/** @file oyFilterGraph_s_.c

   [Template file inheritance graph]
   +-> oyFilterGraph_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2015 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 */



  
#include "oyFilterGraph_s.h"
#include "oyFilterGraph_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  


/* Include "FilterGraph.private_custom_definitions.c" { */
/** Function    oyFilterGraph_Release__Members
 *  @memberof   oyFilterGraph_s
 *  @brief      Custom FilterGraph destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  filtergraph  the FilterGraph object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyFilterGraph_Release__Members( oyFilterGraph_s_ * filtergraph )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &filtergraph->member );
   */
  oyFilterNodes_Release( &filtergraph->nodes );
  oyFilterPlugs_Release( &filtergraph->edges );
  oyOptions_Release( &filtergraph->options );

  if(filtergraph->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = filtergraph->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( filtergraph->member );
     */
  }
}

/** Function    oyFilterGraph_Init__Members
 *  @memberof   oyFilterGraph_s
 *  @brief      Custom FilterGraph constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  filtergraph  the FilterGraph object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterGraph_Init__Members( oyFilterGraph_s_ * filtergraph )
{
  filtergraph->options = oyOptions_New( 0 );

  return 0;
}

/** Function    oyFilterGraph_Copy__Members
 *  @memberof   oyFilterGraph_s
 *  @brief      Custom FilterGraph copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyFilterGraph_s_ input object
 *  @param[out]  dst  the output oyFilterGraph_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterGraph_Copy__Members( oyFilterGraph_s_ * dst, oyFilterGraph_s_ * src)
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
  dst->nodes = oyFilterNodes_Copy( src->nodes, 0 );
  dst->edges = oyFilterPlugs_Copy( src->edges, 0 );
  dst->options = oyOptions_Copy( src->options, dst->oy_ );

  return error;
}

/* } Include "FilterGraph.private_custom_definitions.c" */



/** @internal
 *  Function oyFilterGraph_New_
 *  @memberof oyFilterGraph_s_
 *  @brief   allocate a new oyFilterGraph_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterGraph_s_ * oyFilterGraph_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_GRAPH_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyFilterGraph_s_ * s = 0;

  if(s_obj)
    s = (oyFilterGraph_s_*)s_obj->allocateFunc_(sizeof(oyFilterGraph_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyFilterGraph_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyFilterGraph_Copy;
  s->release = (oyStruct_Release_f) oyFilterGraph_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom FilterGraph constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_FILTER_GRAPH_S, (oyPointer)s );
  /* ---- end of custom FilterGraph constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom FilterGraph constructor ----- */
  error += oyFilterGraph_Init__Members( s );
  /* ---- end of custom FilterGraph constructor ------- */
  
  
  
  

  if(error)
    WARNc1_S("%d", error);

  return s;
}

/** @internal
 *  Function oyFilterGraph_Copy__
 *  @memberof oyFilterGraph_s_
 *  @brief   real copy a FilterGraph object
 *
 *  @param[in]     filtergraph                 FilterGraph struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterGraph_s_ * oyFilterGraph_Copy__ ( oyFilterGraph_s_ *filtergraph, oyObject_s object )
{
  oyFilterGraph_s_ *s = 0;
  int error = 0;

  if(!filtergraph || !object)
    return s;

  s = (oyFilterGraph_s_*) oyFilterGraph_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom FilterGraph copy constructor ----- */
    error = oyFilterGraph_Copy__Members( s, filtergraph );
    /* ---- end of custom FilterGraph copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyFilterGraph_Release_( &s );

  return s;
}

/** @internal
 *  Function oyFilterGraph_Copy_
 *  @memberof oyFilterGraph_s_
 *  @brief   copy or reference a FilterGraph object
 *
 *  @param[in]     filtergraph                 FilterGraph struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterGraph_s_ * oyFilterGraph_Copy_ ( oyFilterGraph_s_ *filtergraph, oyObject_s object )
{
  oyFilterGraph_s_ *s = filtergraph;

  if(!filtergraph)
    return 0;

  if(filtergraph && !object)
  {
    s = filtergraph;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilterGraph_Copy__( filtergraph, object );

  return s;
}
 
/** @internal
 *  Function oyFilterGraph_Release_
 *  @memberof oyFilterGraph_s_
 *  @brief   release and possibly deallocate a FilterGraph object
 *
 *  @param[in,out] filtergraph                 FilterGraph struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyFilterGraph_Release_( oyFilterGraph_s_ **filtergraph )
{
  /* ---- start of common object destructor ----- */
  oyFilterGraph_s_ *s = 0;

  if(!filtergraph || !*filtergraph)
    return 0;

  s = *filtergraph;

  *filtergraph = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom FilterGraph destructor ----- */
  oyFilterGraph_Release__Members( s );
  /* ---- end of custom FilterGraph destructor ------- */
  
  
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "FilterGraph.private_methods_definitions.c" { */

/* } Include "FilterGraph.private_methods_definitions.c" */

