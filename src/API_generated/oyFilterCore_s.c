/** @file oyFilterCore_s.c

   [Template file inheritance graph]
   +-> oyFilterCore_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyFilterCore_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyFilterCore_s_.h"

#include "oyOptions_s.h"
  


/** Function oyFilterCore_New
 *  @memberof oyFilterCore_s
 *  @brief   allocate a new FilterCore object
 */
OYAPI oyFilterCore_s * OYEXPORT
  oyFilterCore_New( oyObject_s object )
{
  oyObject_s s = object;
  oyFilterCore_s_ * filtercore = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  filtercore = oyFilterCore_New_( s );

  return (oyFilterCore_s*) filtercore;
}

/** Function  oyFilterCore_Copy
 *  @memberof oyFilterCore_s
 *  @brief    Copy or Reference a FilterCore object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     filtercore                 FilterCore struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyFilterCore_s* OYEXPORT
  oyFilterCore_Copy( oyFilterCore_s *filtercore, oyObject_s object )
{
  oyFilterCore_s_ * s = (oyFilterCore_s_*) filtercore;

  if(s)
  {
    oyCheckType__m( oyOBJECT_FILTER_CORE_S, return NULL )
  }
  else
    return NULL;

  s = oyFilterCore_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyFilterCore_s" );

  return (oyFilterCore_s*) s;
}
 
/** Function oyFilterCore_Release
 *  @memberof oyFilterCore_s
 *  @brief   release and possibly deallocate a oyFilterCore_s object
 *
 *  @param[in,out] filtercore                 FilterCore struct object
 */
OYAPI int OYEXPORT
  oyFilterCore_Release( oyFilterCore_s **filtercore )
{
  oyFilterCore_s_ * s = 0;

  if(!filtercore || !*filtercore)
    return 0;

  s = (oyFilterCore_s_*) *filtercore;

  oyCheckType__m( oyOBJECT_FILTER_CORE_S, return 1 )

  *filtercore = 0;

  return oyFilterCore_Release_( &s );
}



/* Include "FilterCore.public_methods_definitions.c" { */
/** Function  oyFilterCore_GetCategory
 *  @memberof oyFilterCore_s
 *  @brief    Get category string
 *
 *  @param[in,out] filter              filter object
 *  @param         nontranslated       switch for translation; not implemented
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
const char * oyFilterCore_GetCategory( oyFilterCore_s    * filter,
                                       int                 nontranslated OY_UNUSED )
{
  if(!filter)
    return 0;

  return ((oyFilterCore_s_*)filter)->category_;
}

/** Function  oyFilterCore_GetName
 *  @memberof oyFilterCore_s
 *  @brief    Get name
 *
 *  provides the original filter names
 *
 *  @param[in,out] filter              filter object
 *  @param         name_type           type of name
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
const char * oyFilterCore_GetName    ( oyFilterCore_s    * filter,
                                       oyNAME_e            name_type )
{
  oyFilterCore_s_ * s = (oyFilterCore_s_*)filter;

  if(!s)
    return 0;

  return oyNoEmptyName_m_( s->api4_->ui->getText( "name", name_type,
                           (oyStruct_s*)s->api4_->ui ) );
}

/** Function  oyFilterCore_GetText
 *  @memberof oyFilterCore_s
 *  @brief    Get text
 *
 *  oyNAME_NAME provides a XML element with child elements and attributes
 *
 *  @param[in,out] filter              filter object
 *  @param         name_type           type of name
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/18 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
const char * oyFilterCore_GetText    ( oyFilterCore_s    * filter,
                                       oyNAME_e            name_type )
{
  char * text = 0;
  oyFilterCore_s_ * s = (oyFilterCore_s_*)filter;
  int error = !s;

  if(error)
    return 0;

  if( (name_type == oyNAME_NAME || name_type == oyNAME_JSON) &&
      !oyObject_GetName(s->oy_, name_type))
  {
    text = oyAllocateWrapFunc_( 512, s->oy_ ? s->oy_->allocateFunc_ : 0 );
    if(!text)
      error = 1;
    if(!error)
    {
      if(name_type == oyNAME_NAME)
        sprintf(text, "<oyFilterCore_s registration=\"%s\" category=\"%s\" version=\"%d.%d.%d\"/>\n",
                  s->registration_,
                  s->category_,
                  s->api4_->version[0],
                  s->api4_->version[1],
                  s->api4_->version[2]
           );
      else
        sprintf(text, "{\n \"oyFilterCore_s\": {\n  \"registration\": \"%s\",\n  \"category\": \"%s\",\n  \"version\": \"%d.%d.%d\"\n }\n}",
                  s->registration_,
                  s->category_,
                  s->api4_->version[0],
                  s->api4_->version[1],
                  s->api4_->version[2]
           );
    }

#if 0
    if(error <= 0 && s->profiles_)
    {
      int i = 0, n = oyProfiles_Count(s->profiles_);
      oyProfile_s * profile = 0;
      for(i = 0; i < n; ++i)
      {
        profile = oyProfiles_Get( s->profiles_, i );
        sprintf( &text[oyStrlen_(text)], "    %s\n",
        oyNoEmptyName_m_(oyProfile_GetText( profile, oyNAME_NAME)) );
      }
    }
#endif

    if(error <= 0)
    {
      error = oyObject_SetName( s->oy_, text, name_type );
      oySTRUCT_FREE_m( s, text );
    }

    if(error)
      WARNc_S(_("MEM Error."));
  }

  return oyObject_GetName(s->oy_, name_type);
}

/** Function  oyFilterCore_NewWith
 *  @memberof oyFilterCore_s
 *  @brief    Lookup and initialise a new filter object
 *
 *  back end selection: \n
 *  - the user knows, which kind of filter is requested -> registration, e.g. "//color"
 *  - the user probably knows, which special CMM to use (e.g. lcms, icc, shiva)
 *
 *  @param[in]     registration        the filter @ref registration pattern
 *  @param[in]     options             the supplied filter options
 *  @param[in]     object              the optional object
 *  @return                            a filter core
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2009/07/27
 */
oyFilterCore_s * oyFilterCore_NewWith( const char        * registration,
                                       oyOptions_s       * options,
                                       oyObject_s          object )
{
  oyFilterCore_s * s = (oyFilterCore_s*)oyFilterCore_New_( object );
  int error = !s;
  oyCMMapi4_s_ * api4_ = 0;

  if(error <= 0)
  {
    api4_ = (oyCMMapi4_s_*) oyCMMsGetFilterApi_(
                                            registration, oyOBJECT_CMM_API4_S );
    error = !api4_;
  }

  if(error <= 0)
    error = oyFilterCore_SetCMMapi4_( (oyFilterCore_s_*)s, api4_ );

  if(error <= 0 && !options)
  {
#if 0
    oyFilterCorePriv_m(s)->options_ = api4_->oyCMMFilter_ValidateOptions( s, options, 0, &ret );
#endif
  }

  if(error <= 0 && options)
  {
    oyFilterCorePriv_m(s)->options_ = oyOptions_Copy( options, 0 );
  }

  if(error && s)
  {
    oyFilterCore_Release( &s );
    WARNc2_S("%s: \"%s\"", _("Could not create filter"), oyNoEmptyName_m_(registration));
  }
  else if(s && oy_debug)
  {
    const char * txt = oyStruct_GetInfo( (oyStruct_s*)s, oyNAME_DESCRIPTION, 0x01 );
    oyObject_SetName( s->oy_, txt, oyNAME_DESCRIPTION );
  }

  return s;
}

/** Function  oyFilterCore_GetRegistration
 *  @memberof oyFilterCore_s
 *  @brief    Get registration text
 *
 *  @param[in]     filter              filter object
 *  @return                            @ref registration string
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/04
 *  @since    2012/10/04 (Oyranos: 0.5.0)
 */
OYAPI const char *  OYEXPORT
                oyFilterCore_GetRegistration (
                                       oyFilterCore_s    * filter )
{
  oyFilterCore_s_ * s = (oyFilterCore_s_*)filter;

  if(!s)
    return 0;

  return s->registration_;
}

/**
 *  @memberof oyFilterCore_s
 *  @brief   set backend specific runtime data
 *
 *  Runtime data can be used as context by a backend during execution. The data
 *  is typical set during oyCMMapi4_s creation. This function provides 
 *  access for a backend inside a DAC in order to change that data during 
 *  backend lifetime.
 *
 *  @param[in,out] obj                 the node object
 *  @param[in]     ptr                 the data needed to run the filter type
 *  @return                            error
 *
 *  @version Oyranos: 0.9.5
 *  @date    2014/01/08
 *  @since   2014/01/08(Oyranos: 0.9.5)
 */
OYAPI int  OYEXPORT
           oyFilterCore_SetBackendContext (
                                       oyFilterCore_s    * obj,
                                       oyPointer_s       * ptr )
{
  oyFilterCore_s_ * s = (oyFilterCore_s_*)obj;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_FILTER_CORE_S, return 1 )

  return oyCMMapi4_SetBackendContext( (oyCMMapi4_s*)s->api4_, ptr );
}

/**
 *  @memberof oyFilterCore_s
 *  @brief   get backend specific runtime data
 *
 *  Backend context is used by a filter type during execution.
 *
 *  @param[in]     obj                 the node object
 *  @return                            the context needed to run the filter type
 *
 *  @version Oyranos: 0.9.5
 *  @date    2014/01/08
 *  @since   2014/01/08(Oyranos: 0.9.5)
 */
OYAPI oyPointer_s * OYEXPORT
           oyFilterCore_GetBackendContext
                                     ( oyFilterCore_s    * obj )
{
  oyFilterCore_s_ * s = (oyFilterCore_s_*)obj;

  if(!s)
    return NULL;

  oyCheckType__m( oyOBJECT_FILTER_CORE_S, return NULL )

  return oyCMMapi4_GetBackendContext( (oyCMMapi4_s*)s->api4_ );
}

/* } Include "FilterCore.public_methods_definitions.c" */

