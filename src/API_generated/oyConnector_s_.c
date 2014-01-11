/** @file oyConnector_s_.c

   [Template file inheritance graph]
   +-> oyConnector_s_.template.c
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
 *  @date     2014/01/11
 */



  
#include "oyConnector_s.h"
#include "oyConnector_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  


/* Include "Connector.private_custom_definitions.c" { */
/** Function    oyConnector_Release__Members
 *  @memberof   oyConnector_s
 *  @brief      Custom Connector destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  connector  the Connector object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyConnector_Release__Members( oyConnector_s_ * connector )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &connector->member );
   */

  if(connector->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = connector->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( connector->member );
     */
    if(connector->connector_type) {
      deallocateFunc( connector->connector_type );
      connector->connector_type = 0;
    }
  }
}

/** Function    oyConnector_Init__Members
 *  @memberof   oyConnector_s
 *  @brief      Custom Connector constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  connector  the Connector object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConnector_Init__Members( oyConnector_s_ * connector )
{
  connector->is_plug = -1;

  return 0;
}

/** Function    oyConnector_Copy__Members
 *  @memberof   oyConnector_s
 *  @brief      Custom Connector copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyConnector_s_ input object
 *  @param[out]  dst  the output oyConnector_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConnector_Copy__Members( oyConnector_s_ * dst, oyConnector_s_ * src)
{
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;
#if 0
  oyDeAlloc_f deallocateFunc_ = 0;
#endif

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
#if 0
  deallocateFunc_ = dst->oy_->deallocateFunc_;
#endif

  /* Copy each value of src to dst here */
  error = oyObject_CopyNames( dst->oy_, src->oy_ );

  dst->connector_type = oyStringCopy_( src->connector_type, allocateFunc_ );
  dst->is_plug = src->is_plug;

  return error;
}

/* } Include "Connector.private_custom_definitions.c" */



/** @internal
 *  Function oyConnector_New_
 *  @memberof oyConnector_s_
 *  @brief   allocate a new oyConnector_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConnector_s_ * oyConnector_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONNECTOR_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyConnector_s_ * s = 0;

  if(s_obj)
    s = (oyConnector_s_*)s_obj->allocateFunc_(sizeof(oyConnector_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyConnector_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyConnector_Copy;
  s->release = (oyStruct_Release_f) oyConnector_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Connector constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CONNECTOR_S, (oyPointer)s );
  /* ---- end of custom Connector constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Connector constructor ----- */
  error += oyConnector_Init__Members( s );
  /* ---- end of custom Connector constructor ------- */
  
  
  
  

  if(error)
    WARNc1_S("%d", error);

  return s;
}

/** @internal
 *  Function oyConnector_Copy__
 *  @memberof oyConnector_s_
 *  @brief   real copy a Connector object
 *
 *  @param[in]     connector                 Connector struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConnector_s_ * oyConnector_Copy__ ( oyConnector_s_ *connector, oyObject_s object )
{
  oyConnector_s_ *s = 0;
  int error = 0;

  if(!connector || !object)
    return s;

  s = (oyConnector_s_*) oyConnector_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Connector copy constructor ----- */
    error = oyConnector_Copy__Members( s, connector );
    /* ---- end of custom Connector copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyConnector_Release_( &s );

  return s;
}

/** @internal
 *  Function oyConnector_Copy_
 *  @memberof oyConnector_s_
 *  @brief   copy or reference a Connector object
 *
 *  @param[in]     connector                 Connector struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConnector_s_ * oyConnector_Copy_ ( oyConnector_s_ *connector, oyObject_s object )
{
  oyConnector_s_ *s = connector;

  if(!connector)
    return 0;

  if(connector && !object)
  {
    s = connector;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyConnector_Copy__( connector, object );

  return s;
}
 
/** @internal
 *  Function oyConnector_Release_
 *  @memberof oyConnector_s_
 *  @brief   release and possibly deallocate a Connector object
 *
 *  @param[in,out] connector                 Connector struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyConnector_Release_( oyConnector_s_ **connector )
{
  /* ---- start of common object destructor ----- */
  oyConnector_s_ *s = 0;

  if(!connector || !*connector)
    return 0;

  s = *connector;

  *connector = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom Connector destructor ----- */
  oyConnector_Release__Members( s );
  /* ---- end of custom Connector destructor ------- */
  
  
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Connector.private_methods_definitions.c" { */

/* } Include "Connector.private_methods_definitions.c" */

