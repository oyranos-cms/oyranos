/** @file oyranos_cmm_dFil.c
 *
 *  a filter for Oyranos 
 *
 *  @par Copyright:
 *            2014-2015 (C) My name
 *
 *  @brief    my filter for Oyranos
 *  @internal
 *  @author   My Name <web@adress.com>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2009/06/14
 */

#include "oyCMM_s.h"
#include "oyCMMapi4_s_.h"
#include "oyCMMapi7_s_.h"
#include "oyCMMapi10_s_.h"
#include "oyCMMui_s_.h"
#include "oyConnectorImaging_s_.h"
#include "oyFilterNode_s_.h"        /* for oyFilterNode_TextToInfo_ */

#include "oyranos_cmm.h"
#include "oyranos_definitions.h"
#include "oyranos_generic.h"         /* oy_connector_imaging_static_object */

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* --- internal definitions --- */

/** The CMM_NICK consists of four bytes, which appear as well in the library name. This is important for Oyranos to identify the required filter struct name. */
#define CMM_NICK "dFil"
#define OY_DUMMY_FILTER_REGISTRATION OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "my_filter"
#define OY_DUMMY_OPTIONS_HANDLER_REG OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "my_handler"

/** The message function pointer to use for messaging. */
oyMessage_f dFil_msg = 0;

extern oyCMMapi4_s_   dFil_api4_my_filter;
extern oyCMMapi7_s_   dFil_api7_my_filter;
extern oyCMMapi10_s_  dFil_api10_my_handler;

#ifndef USE_I18N
/** i18n prototype */
#define _(text) text
#endif
#define _DBG_FORMAT_ "%s:%d %s()"
#define _DBG_ARGS_ __FILE__,__LINE__,__func__

/* --- implementations --- */

/** Function dFilCMMInit
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
int                dFilCMMInit       ( )
{
  int error = 0;
  return error;
}



/** Function dFilCMMMessageFuncSet
 *  @brief API requirement
 *
 *  A Oyranos user might want its own message function and omit the default
 *  one.
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
int            dFilCMMMessageFuncSet ( oyMessage_f         message_func )
{
  dFil_msg = message_func;
  return 0;
}


/** For very simple options we do not need event handling. Dummies suffice in
 *  this case. */
const char * dFilWidget_GetDummy     ( const char        * func_name,
                                       uint32_t          * result )
{return 0;}
oyWIDGET_EVENT_e dFilWidget_EventDummy
                                     ( const char        * wid,
                                       oyWIDGET_EVENT_e    type )
{return (oyWIDGET_EVENT_e)0;}


oyWIDGET_EVENT_e   dFilWidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event )
{return (oyWIDGET_EVENT_e)0;}


/** Function dFilFilterNode_MyContextToMem
 *  @brief   implement oyCMMFilter_ContextToMem_f()
 *
 *  Serialise into a Oyranos specific ICC profile containers "Info" tag.
 *  We do not have any binary context to include.
 *  Thus oyFilterNode_TextToInfo_() is fine.
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
oyPointer  dFilFilterNode_MyContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  return oyFilterNode_TextToInfo_( (oyFilterNode_s_*)node, size, allocateFunc );
}


/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  Implement at least "name", "manufacturer" and "copyright". If you like with
 *  internationalisation.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 *  @date    2009/06/23
 */
const char * dFilGetText             ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return _(CMM_NICK);
    else if(type == oyNAME_NAME)
      return _("libraw input filter");
    else
      return _("libraw input filter");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return _("Me");
    else if(type == oyNAME_NAME)
      return _("My Name");
    else
      return _("My project; www: http://www.my-adress.com; support/email: my@adress.com; sources: http://www.my-adress.com/download");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return _("newBSD");
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 2009 My Name; newBSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/BSD-3-Clause");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return _("help");
    else if(type == oyNAME_NAME)
      return _("My filter introduction.");
    else
      return _("All the small details for using this module.");
  }
  return 0;
}
const char *dFil_texts[5] = {"name","copyright","manufacturer","help",0};

oyIcon_s dFil_icon = {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"};

/** @instance dFil_cmm_module
 *  @brief    dFil module infos
 *
 *  This structure is dlopened by Oyranos. Its name has to consist of the
 *  following elements:
 *  - the four byte CMM_NICK plus
 *  - "_cmm_module"
 *  This string must be included in the the filters filename.
 *
 *  @version Oyranos: 0.9.0
 *  @date    2012/10/11
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
oyCMM_s dFil_cmm_module = {

  oyOBJECT_CMM_INFO_S, /**< ::type; the object type */
  0,0,0,               /**< static objects omit these fields */
  CMM_NICK,            /**< ::cmm; the four char filter id */
  (char*)"0.1.10",     /**< ::backend_version */
  dFilGetText,         /**< ::getText; UI texts */
  (char**)dFil_texts,  /**< ::texts; list of arguments to getText */
  OYRANOS_VERSION,     /**< ::oy_compatibility; last supported Oyranos CMM API*/

  /** ::api; The first filter api structure. */
  (oyCMMapi_s*) & dFil_api4_my_filter,

  /** ::icon; zero terminated list of a icon pyramid */
  &dFil_icon
};


/* OY_DUMMY_FILTER_REGISTRATION ----------------------------------------------*/



oyOptions_s* dFilFilter_MyFilterValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

#if 0
  if(!error)
    error = !oyOptions_FindString( validate, "my_options", 0 );
#endif

  *result = error;

  return 0;
}

/** Function dFilFilterPlug_MyFilterRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  The primary filter entry for data processing.
 *
 *  @param         requestor_plug      the plug of the requesting node after 
 *                                     my filter in the graph
 *  @param         ticket              the job ticket
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
int      dFilFilterPlug_MyFilterRun (
                                       oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  oyFilterSocket_s * socket;
  oyFilterNode_s * node;
  oyArray2d_s * array;
  uint8_t * u8_array;
  uint16_t * u16_array;
  int array_width;
  int i;
  double my_max_value = 0.0;

  /** my filters socket */
  socket = oyFilterPlug_GetSocket( requestor_plug );
  /** my filter node */
  node = oyFilterSocket_GetNode( socket );

  if( !ticket )
  {
    dFil_msg( oyMSG_WARN, (oyStruct_s*)node,
             "failed to get a job ticket");
    return 1;
  }

#if 0
  oyFilterPlug_s * plug;

  /** my filters plug */
  plug = (oyFilterPlug_s *)node->plugs[0];
  /** get the full image data in case my filter needs it */
  image_input = oyFilterPlug_ResolveImage( plug, socket, ticket );
#endif

  array = oyPixelAccess_GetArray( ticket );
  array_width =  (int)(oyArray2d_GetWidth(array)+0.5);

  /** Handle all supported data types. Here is the core of my filter. */
  if(oyArray2d_GetType(array) == oyUINT8)
  {
    u8_array = (uint8_t*)oyArray2d_GetData( array );
    for(i = 0; i < array_width; ++i)
      if(u8_array[i] > my_max_value)
        my_max_value = u8_array[i];
  } else if(oyArray2d_GetType(array) == oyUINT16)
  {
    u16_array = (uint16_t*)oyArray2d_GetData( array );
    for(i = 0; i < array_width; ++i)
      if(u16_array[i] > my_max_value)
        my_max_value = u16_array[i];
  }

  oyFilterSocket_Release( &socket );
  oyFilterNode_Release( &node );

  return 0;
}


oyDATATYPE_e dFil_data_types[3] = {oyUINT8, oyUINT16, (oyDATATYPE_e)0};


/** My filters socket for delivering results */
oyConnectorImaging_s_ dFil_myFilter_connectorSocket = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorSocketText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  dFil_data_types,
  2, /* data_types_n; elements in data_types array */
  -1, /* max_color_offset */
  1, /* min_channels_count; */
  3, /* max_channels_count; */
  1, /* min_color_count; */
  3, /* max_color_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  0, /* can_swap; can swap color channels (BGR)*/
  0, /* can_swap_bytes; non host byte order */
  0, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  0, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* count in channel_types */
  1, /* id; relative to oyFilter_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
/** My filter has just one socket connector. Tell about it. */
oyConnectorImaging_s_ * dFil_myFilter_connectorSockets[2] = 
             { &dFil_myFilter_connectorSocket, 0 };

/** My filters plug for obtaining data */
oyConnectorImaging_s_ dFil_myFilter_connectorPlug = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorPlugText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  1, /* is_plug == oyFilterPlug_s */
  dFil_data_types,
  2, /* data_types_n; elements in data_types array */
  -1, /* max_color_offset */
  1, /* min_channels_count; */
  65535, /* max_channels_count; */
  1, /* min_color_count; */
  65535, /* max_color_count; */
  1, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  0, /* can_swap; can swap color channels (BGR)*/
  0, /* can_swap_bytes; non host byte order */
  0, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  0, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* count in channel_types */
  1, /* id; relative to oyFilter_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
/** My filter has just one plug connector. Tell about it. */
oyConnectorImaging_s_ * dFil_myFilter_connectorPlugs[2] = 
             { &dFil_myFilter_connectorPlug, 0 };


/**
 *  This function implements oyCMMGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/22 (Oyranos: 0.1.10)
 *  @date    2009/12/22
 */
const char * dFilApi4UiGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  static char * category = 0;
  if(strcmp(select,"name"))
  {
         if(type == oyNAME_NICK)
      return "my_filter";
    else if(type == oyNAME_NAME)
      return _("Image[my_filter]");
    else
      return _("My Filter Object");
  }
  else if(strcmp(select,"help"))
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Some help for My example filter.");
    else
      return _("More indepth help  for My example filter.");
  }
  else if(strcmp(select,"category"))
  {
    if(!category)
    {
      /* The following strings must match the categories for a menu entry. */
      const char * i18n[] = {_("Color"),_("My Filter"),0};
      int len =  strlen(i18n[0]) + strlen(i18n[1]);
      category = (char*)malloc( len + 64 );
      if(category)
        /* Create a translation for dFil_api4_ui_my_filter::category. */
        sprintf( category,"%s/%s", i18n[0], i18n[1] );
      else
        dFil_msg(oyMSG_WARN, (oyStruct_s *) 0, _DBG_FORMAT_ "\n " "Could not allocate enough memory.", _DBG_ARGS_);
    }
         if(type == oyNAME_NICK)
      return "category";
    else if(type == oyNAME_NAME)
      return category;
    else
      return category;
  }
  return 0;
}
const char * dFil_api4_ui_texts[] = {"name", "category", "help", 0};
/** @instance dFil_api4_ui_my_filter
 *  @brief    dFil oyCMMapi4_s::ui implementation
 *
 *  The UI for dFil.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/09/09
 */
oyCMMui_s_   dFil_api4_ui_my_filter = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}, /**< version[3] */
  {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}, /**< int32_t module_api[3] */

  dFilFilter_MyFilterValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  dFilWidgetEvent, /* oyWidgetEvent_f */

  "Filter/My Filter", /* UI category */
  0,   /* const char * options */
  0,   /* oyCMMuiGet_f oyCMMuiGet */

  dFilApi4UiGetText, /* oyCMMGetText_f   getText */
  dFil_api4_ui_texts /* const char    ** texts */
};

/** @instance dFil_api4
 *  @brief    dFil oyCMMapi4_s implementation
 *
 *  My filter api4 object to provide basic informations.
 *
 *  A context handler for preprocessing can be attached here. See for instance
 *  the 'lcms' filter.
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
oyCMMapi4_s_ dFil_api4_my_filter = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & dFil_api7_my_filter, /* oyCMMapi_s * next */
  
  dFilCMMInit, /* oyCMMInit_f */
  dFilCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_DUMMY_FILTER_REGISTRATION,

  {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},/**< version[3] */
  {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}, /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,                         /**< oyPointer_s * runtime_context */

  dFilFilterNode_MyContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_GetText_f        oyCMMFilterNode_GetText */
  {0}, /* char context_type[8] */

  &dFil_api4_ui_my_filter              /**< oyCMMui_s *ui */
};

/** @instance dFil_api7
 *  @brief    dFil oyCMMapi7_s implementation
 *
 *  My filter api7 object, to provide the processing routine and
 *  node connectors.
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
oyCMMapi7_s_ dFil_api7_my_filter = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & dFil_api10_my_handler, /* oyCMMapi_s * next */
  
  dFilCMMInit, /* oyCMMInit_f */
  dFilCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_DUMMY_FILTER_REGISTRATION,

  {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},/**< version[3] */
  {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}, /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,                         /**< oyPointer_s * runtime_context */

  dFilFilterPlug_MyFilterRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  (oyConnector_s**) dFil_myFilter_connectorPlugs,   /* plugs */
  1,   /* plugs_n */
  0,   /* plugs_last_add */
  (oyConnector_s**) dFil_myFilter_connectorSockets,   /* sockets */
  1,   /* sockets_n */
  0    /* sockets_last_add */
};


/* OY_DUMMY_FILTER_REGISTRATION ----------------------------------------------*/

/* OY_DUMMY_OPTIONS_HANDLER_REG ----------------------------------------------*/

/**
 *  This function implements oyMOptions_Handle_f.
 *
 *  @version Oyranos: 0.4.0
 *  @since   2012/01/11 (Oyranos: 0.4.0)
 *  @date    2012/01/11
 */
int          dFilMOptions_Handle     ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  oyOption_s * o = 0;
  int error = 0;

  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"my_handler", 0))
    {
      o = oyOptions_Find( options, "print", oyNAME_PATTERN );
      if(!o)
      {
        dFil_msg( oyMSG_WARN, (oyStruct_s*)options,
                 "no option \"print\" found");
        error = 1;
      }
      oyOption_Release( &o );

      return error;
    }
    else
      return 1;
  }
  else if(oyFilterRegistrationMatch(command,"my_handler", 0))
  {
    o = oyOptions_Find( options, "print", oyNAME_PATTERN );
    if(o)
    {
      /* now handle the options */
      dFil_msg( oyMSG_WARN, (oyStruct_s*)options,
                 "\"print\" contains: %s",
                 oyOption_GetText( o, oyNAME_NAME ) );
      
      oyOption_Release( &o );

      o = oyOption_FromRegistration( OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "my_handler.result." CMM_NICK,
                        0 );
      oyOption_SetFromText( o, "done", 0 );
      if(!*result)
        *result = oyOptions_New(0);
      oyOptions_MoveIn( *result, &o, -1 );
    }
  }

  return 0;
}

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.4.0
 *  @since   2012/01/11 (Oyranos: 0.4.0)
 *  @date    2012/01/11
 */
const char * dFilInfoGetTextMyHandler( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, "my_handler")==0)
  {
         if(type == oyNAME_NICK)
      return "my_handler";
    else if(type == oyNAME_NAME)
      return _("So something with options.");
    else
      return _("The my_handler takes a option with key name \"print\" and prints it.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return _("help");
    else if(type == oyNAME_NAME)
      return _("Handle options.");
    else
      return _("This example module \"my_handler\" lets you print a option.");
  }
  return 0;
}
const char *dFil_texts_my_handler[4] = {"can_handle","my_handler","help",0};

/** @instance dFil_api10_my_handler
 *  @brief    dFil oyCMMapi10_s implementation
 *
 *  a simple example for handling options
 *
 *  @version Oyranos: 0.4.0
 *  @since   2012/01/11 (Oyranos: 0.4.0)
 *  @date    2012/01/11
 */
oyCMMapi10_s_    dFil_api10_my_handler = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  (oyCMMapi_s*) NULL,

  dFilCMMInit,
  dFilCMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "my_handler._" CMM_NICK,

  {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},/**< version[3] */
  {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}, /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,                         /**< oyPointer_s * runtime_context */
 
  dFilInfoGetTextMyHandler,             /**< getText */
  (char**)dFil_texts_my_handler,       /**<texts; list of arguments to getText*/
 
  dFilMOptions_Handle                  /**< oyMOptions_Handle_f oyMOptions_Handle */
};

/* OY_DUMMY_OPTIONS_HANDLER_REG ----------------------------------------------*/


