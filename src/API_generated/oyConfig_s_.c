/** @file oyConfig_s_.c

   [Template file inheritance graph]
   +-> oyConfig_s_.template.c
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



  
#include "oyConfig_s.h"
#include "oyConfig_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  


/* Include "Config.private_custom_definitions.c" { */
/** @internal
 +  Function    oyConfig_Release__Members
 *  @memberof   oyConfig_s
 *  @brief      Custom Config destructor
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  config  the Config object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyConfig_Release__Members( oyConfig_s_ * config )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &config->member );
   */

  oyOptions_Release( &config->db );
  oyOptions_Release( &config->backend_core );
  oyOptions_Release( &config->data );

  if(config->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = config->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( config->member );
     */
    int i = 0;
    if(config->rank_map)
    {
      while(config->rank_map[i].key)
        deallocateFunc( config->rank_map[i++].key );
      deallocateFunc( config->rank_map );
    }
  }
}

/** @internal
 +  Function    oyConfig_Init__Members
 *  @memberof   oyConfig_s
 *  @brief      Custom Config constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  config  the Config object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConfig_Init__Members( oyConfig_s_ * config )
{
  config->db = oyOptions_New( 0 );
  config->backend_core = oyOptions_New( 0 );
  config->data = oyOptions_New( 0 );

  return 0;
}

/** @internal
 +  Function    oyConfig_Copy__Members
 *  @memberof   oyConfig_s
 *  @brief      Custom Config copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyConfig_s_ input object
 *  @param[out]  dst  the output oyConfig_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConfig_Copy__Members( oyConfig_s_ * dst, oyConfig_s_ * src)
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
  dst->db = oyOptions_Copy( src->db, dst->oy_ );
  dst->backend_core = oyOptions_Copy( src->backend_core, dst->oy_ );
  dst->data = oyOptions_Copy( src->data, dst->oy_ );
  error = !memcpy( dst->version, src->version, 3*sizeof(int) );

  dst->rank_map = oyRankMapCopy( src->rank_map, allocateFunc_ );

  return error;
}

/* } Include "Config.private_custom_definitions.c" */



/** @internal
 *  Function oyConfig_New_
 *  @memberof oyConfig_s_
 *  @brief   allocate a new oyConfig_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConfig_s_ * oyConfig_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONFIG_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyConfig_s_ * s = 0;

  if(s_obj)
    s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyConfig_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyConfig_Copy;
  s->release = (oyStruct_Release_f) oyConfig_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Config constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CONFIG_S, (oyPointer)s );
  /* ---- end of custom Config constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Config constructor ----- */
  error += oyConfig_Init__Members( s );
  /* ---- end of custom Config constructor ------- */
  
  
  
  

  if(error)
    WARNc1_S("%d", error);

  return s;
}

/** @internal
 *  Function oyConfig_Copy__
 *  @memberof oyConfig_s_
 *  @brief   real copy a Config object
 *
 *  @param[in]     config                 Config struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConfig_s_ * oyConfig_Copy__ ( oyConfig_s_ *config, oyObject_s object )
{
  oyConfig_s_ *s = 0;
  int error = 0;

  if(!config || !object)
    return s;

  s = (oyConfig_s_*) oyConfig_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Config copy constructor ----- */
    error = oyConfig_Copy__Members( s, config );
    /* ---- end of custom Config copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyConfig_Release_( &s );

  return s;
}

/** @internal
 *  Function oyConfig_Copy_
 *  @memberof oyConfig_s_
 *  @brief   copy or reference a Config object
 *
 *  @param[in]     config                 Config struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConfig_s_ * oyConfig_Copy_ ( oyConfig_s_ *config, oyObject_s object )
{
  oyConfig_s_ *s = config;

  if(!config)
    return 0;

  if(config && !object)
  {
    s = config;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyConfig_Copy__( config, object );

  return s;
}
 
/** @internal
 *  Function oyConfig_Release_
 *  @memberof oyConfig_s_
 *  @brief   release and possibly deallocate a Config object
 *
 *  @param[in,out] config                 Config struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyConfig_Release_( oyConfig_s_ **config )
{
  /* ---- start of common object destructor ----- */
  oyConfig_s_ *s = 0;

  if(!config || !*config)
    return 0;

  s = *config;

  *config = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom Config destructor ----- */
  oyConfig_Release__Members( s );
  /* ---- end of custom Config destructor ------- */
  
  
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Config.private_methods_definitions.c" { */

/* } Include "Config.private_methods_definitions.c" */

