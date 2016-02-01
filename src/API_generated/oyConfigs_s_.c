/** @file oyConfigs_s_.c

   [Template file inheritance graph]
   +-> oyConfigs_s_.template.c
   |
   +-> BaseList_s_.c
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



  
#include "oyConfigs_s.h"
#include "oyConfigs_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  


/* Include "Configs.private_custom_definitions.c" { */
/** Function    oyConfigs_Release__Members
 *  @memberof   oyConfigs_s
 *  @brief      Custom Configs destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  configs  the Configs object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyConfigs_Release__Members( oyConfigs_s_ * configs )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &configs->member );
   */

  if(configs->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = configs->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( configs->member );
     */
  }
}

/** Function    oyConfigs_Init__Members
 *  @memberof   oyConfigs_s
 *  @brief      Custom Configs constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  configs  the Configs object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConfigs_Init__Members( oyConfigs_s_ * configs )
{
  return 0;
}

/** Function    oyConfigs_Copy__Members
 *  @memberof   oyConfigs_s
 *  @brief      Custom Configs copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyConfigs_s_ input object
 *  @param[out]  dst  the output oyConfigs_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConfigs_Copy__Members( oyConfigs_s_ * dst, oyConfigs_s_ * src)
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

/* } Include "Configs.private_custom_definitions.c" */



/** @internal
 *  Function oyConfigs_New_
 *  @memberof oyConfigs_s_
 *  @brief   allocate a new oyConfigs_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConfigs_s_ * oyConfigs_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONFIGS_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyConfigs_s_ * s = 0;

  if(s_obj)
    s = (oyConfigs_s_*)s_obj->allocateFunc_(sizeof(oyConfigs_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyConfigs_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyConfigs_Copy;
  s->release = (oyStruct_Release_f) oyConfigs_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Configs constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CONFIGS_S, (oyPointer)s );
  /* ---- end of custom Configs constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  s->list_ = oyStructList_Create( s->type_, 0, 0 );


  
  /* ---- start of custom Configs constructor ----- */
  error += oyConfigs_Init__Members( s );
  /* ---- end of custom Configs constructor ------- */
  
  
  
  

  if(error)
    WARNc1_S("%d", error);

  return s;
}

/** @internal
 *  Function oyConfigs_Copy__
 *  @memberof oyConfigs_s_
 *  @brief   real copy a Configs object
 *
 *  @param[in]     configs                 Configs struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConfigs_s_ * oyConfigs_Copy__ ( oyConfigs_s_ *configs, oyObject_s object )
{
  oyConfigs_s_ *s = 0;
  int error = 0;

  if(!configs || !object)
    return s;

  s = (oyConfigs_s_*) oyConfigs_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Configs copy constructor ----- */
    error = oyConfigs_Copy__Members( s, configs );
    /* ---- end of custom Configs copy constructor ------- */
    
    
    
    
    
    s->list_ = oyStructList_Copy( configs->list_, s->oy_ );

  }

  if(error)
    oyConfigs_Release_( &s );

  return s;
}

/** @internal
 *  Function oyConfigs_Copy_
 *  @memberof oyConfigs_s_
 *  @brief   copy or reference a Configs object
 *
 *  @param[in]     configs                 Configs struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConfigs_s_ * oyConfigs_Copy_ ( oyConfigs_s_ *configs, oyObject_s object )
{
  oyConfigs_s_ *s = configs;

  if(!configs)
    return 0;

  if(configs && !object)
  {
    s = configs;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyConfigs_Copy__( configs, object );

  return s;
}
 
/** @internal
 *  Function oyConfigs_Release_
 *  @memberof oyConfigs_s_
 *  @brief   release and possibly deallocate a Configs list
 *
 *  @param[in,out] configs                 Configs struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyConfigs_Release_( oyConfigs_s_ **configs )
{
  /* ---- start of common object destructor ----- */
  oyConfigs_s_ *s = 0;

  if(!configs || !*configs)
    return 0;

  s = *configs;

  *configs = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom Configs destructor ----- */
  oyConfigs_Release__Members( s );
  /* ---- end of custom Configs destructor ------- */
  
  
  
  

  oyStructList_Release( &s->list_ );


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Configs.private_methods_definitions.c" { */

/* } Include "Configs.private_methods_definitions.c" */

