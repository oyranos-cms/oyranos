/** @file oyranos_cmm_dFil.c
 *
 *  a filter for Oyranos 
 *
 *  @par Copyright:
 *            2009 (C) My name
 *
 *  @brief    my filter for Oyranos
 *  @internal
 *  @author   My Name <web@adress.com>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/06/14
 */


#include "oyranos_alpha.h"
#include "oyranos_cmm.h"
#include "oyranos_definitions.h"

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* --- internal definitions --- */

/** The CMM_NICK consists of four bytes, which appear as well in the library name. This is important for Oyranos to identify the required filter struct name. */
#define CMM_NICK "dFil"
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}
#define OY_DUMMY_FILTER_REGISTRATION OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "my_filter"

int dFilCMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... );
/** The message function pointer to use for messaging. */
oyMessage_f message = dFilCMMWarnFunc;

extern oyCMMapi4_s   dFil_api4_my_filter;
extern oyCMMapi7_s   dFil_api7_my_filter;

#ifndef USE_I18N
/** i18n prototype */
#define _(text) text
#endif

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



/** Function dFilCMMWarnFunc
 *  @brief message handling
 *
 *  A internal implementation of a message function. Oyranos can select a 
 *  different one. Do not call this directly.
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
int dFilCMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... )
{
  char* text = (char*)calloc(sizeof(char), 4096);
  va_list list;
  const char * type_name = "";
  int id = -1;

  if(context && oyOBJECT_NONE < context->type_)
  {
    type_name = oyStructTypeToText( context->type_ );
    id = oyObject_GetId( context->oy_ );
  }

  va_start( list, format);
  vsprintf( text, format, list);
  va_end  ( list );

  switch(code)
  {
    case oyMSG_WARN:
         fprintf( stderr, "WARNING"); fprintf( stderr, ": " );
         break;
    case oyMSG_ERROR:
         fprintf( stderr, "!!! ERROR"); fprintf( stderr, ": " );
         break;
  }

  fprintf( stderr, "%s[%d] ", type_name, id );

  fprintf( stderr, text ); fprintf( stderr, "\n" );
  free( text );

  return 0;
}

/** Function dFilCMMMessageFuncSet
 *  @brief API requirement
 *
 *  A Oyranos user might want its own message function and omit our internal
 *  one.
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
int            dFilCMMMessageFuncSet ( oyMessage_f         message_func )
{
  message = message_func;
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
  return oyFilterNode_TextToInfo_( node, size, allocateFunc );
}


/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  Implement at least "name", "manufacturer" and "copyright". If you like with
 *  internationalisation.
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
const char * dFilGetText             ( const char        * select,
                                       oyNAME_e            type )
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
      return _("new BSD license: http://www.opensource.org/licenses/bsd-license.php");
  }
  return 0;
}


/** @instance dFil_cmm_module
 *  @brief    dFil module infos
 *
 *  This structure is dlopened by Oyranos. Its name has to consist of the
 *  following elements:
 *  - the four byte CMM_NICK plus
 *  - "_cmm_module"
 *  This string must be included in the the filters filename.
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
oyCMMInfo_s dFil_cmm_module = {

  oyOBJECT_CMM_INFO_S, /**< ::type; the object type */
  0,0,0,               /**< static objects omit these fields */
  CMM_NICK,            /**< ::cmm; the four char filter id */
  (char*)"0.1.10",     /**< ::backend_version */
  dFilGetText,         /**< ::getText; UI texts */
  OYRANOS_VERSION,     /**< ::oy_compatibility; last supported Oyranos CMM API*/

  /** ::api; The first filter api structure. */
  (oyCMMapi_s*) & dFil_api4_my_filter,

  /** ::icon; zero terminated list of a icon pyramid */
  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, (char*)"oyranos_logo.png"}
};


/* OY_DUMMY_FILTER_REGISTRATION ----------------------------------------------*/


/** Function dFilFilter_MyFilterCanHandle
 *  @brief   inform about filter capabilities
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
int    dFilFilter_MyFilterCanHandle (
                                       oyCMMQUERY_e      type,
                                       uint32_t          value )
{
  int ret = -1;
  return ret;
}

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
  oyFilterPlug_s * plug;
  oyFilterNode_s * node;
  oyImage_s * image_input;
  uint8_t * u8_array;
  uint16_t * u16_array;
  int i;
  double my_max_value = 0.0;

  /** my filters socket */
  socket = requestor_plug->remote_socket_;
  /** my filter node */
  node = socket->node;
  /** my filters plug */
  plug = (oyFilterPlug_s *)node->plugs[0];

#if 0
  /** get the full image data in case my filter needs it */
  image_input = oyFilterPlug_ResolveImage( plug, socket, ticket );
#endif

  if( !ticket )
  {
    message( oyMSG_WARN, (oyStruct_s*)node,
             "failed to get a job ticket");
    return 1;
  }

  /** Handle all supported data types. Here is the core of my filter. */
  if(ticket->array->t == oyUINT8)
  {
    u8_array = (uint8_t*)ticket->array->array2d[0];
    for(i = 0; i < ticket->array->width; ++i)
      if(u8_array[i] > my_max_value)
        my_max_value = u8_array[i];
  } else if(ticket->array->t == oyUINT16)
  {
    u16_array = (uint16_t*)ticket->array->array2d[0];
    for(i = 0; i < ticket->array->width; ++i)
      if(u16_array[i] > my_max_value)
        my_max_value = u16_array[i];
  }

  oyImage_Release( &image_input );

  return 0;
}


oyDATATYPE_e dFil_data_types[3] = {oyUINT8, oyUINT16, (oyDATATYPE_e)0};


/** My filters socket for delivering results */
oyConnectorImaging_s dFil_myFilter_connectorSocket = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "My Filter Socket"},
  "//" OY_TYPE_STD "/image", /* connector_type */
  0, /* is_plug == oyFilterPlug_s */
  dFil_data_types,
  2, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  3, /* max_channels_count; */
  1, /* min_colour_count; */
  3, /* max_colour_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  0, /* can_swap; can swap colour channels (BGR)*/
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
oyConnectorImaging_s * dFil_myFilter_connectorSockets[2] = 
             { &dFil_myFilter_connectorSocket, 0 };

/** My filters plug for obtaining data */
oyConnectorImaging_s dFil_myFilter_connectorPlug = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,0,
  {oyOBJECT_NAME_S, 0,0,0, "Img", "Image", "My Filter Plug"},
  "//" OY_TYPE_STD "/image", /* connector_type */
  1, /* is_plug == oyFilterPlug_s */
  dFil_data_types,
  2, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  65535, /* max_channels_count; */
  1, /* min_colour_count; */
  65535, /* max_colour_count; */
  1, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  0, /* can_swap; can swap colour channels (BGR)*/
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
oyConnectorImaging_s * dFil_myFilter_connectorPlugs[2] = 
             { &dFil_myFilter_connectorPlug, 0 };


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
oyCMMapi4_s   dFil_api4_my_filter = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & dFil_api7_my_filter, /* oyCMMapi_s * next */
  
  dFilCMMInit, /* oyCMMInit_f */
  dFilCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */
  dFilFilter_MyFilterCanHandle, /* oyCMMCanHandle_f */

  /* registration */
  OY_DUMMY_FILTER_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  dFilFilter_MyFilterValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  dFilWidgetEvent, /* oyWidgetEvent_f */

  dFilFilterNode_MyContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_GetText_f        oyCMMFilterNode_GetText */
  {0}, /* char context_type[8] */

  {oyOBJECT_NAME_S, 0,0,0, "my_filter", "Image[my_filter]", "My Filter Object"}, /* name; translatable, eg "scale" "image scaling" "..." */
  "Filter/My Filter", /* UI category */
  0,   /* options */
  0    /* opts_ui_ */
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
oyCMMapi7_s   dFil_api7_my_filter = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) 0, /* oyCMMapi_s * next */
  
  dFilCMMInit, /* oyCMMInit_f */
  dFilCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */
  dFilFilter_MyFilterCanHandle, /* oyCMMCanHandle_f */

  /* registration */
  OY_DUMMY_FILTER_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

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

