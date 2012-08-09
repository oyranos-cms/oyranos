/** @file oyranos_cmm_oyra_image.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2008-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    modules for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2008/01/02
 */

#include "config.h"
#include "oyranos_alpha.h"
#include "oyranos_alpha_internal.h"
#include "oyranos_cmm.h"
#include "oyranos_cmm_oyra.h"
#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_definitions.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"
#include <iconv.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_POSIX
#include <stdint.h>  /* UINT32_MAX */
#endif

/* OY_IMAGE_LOAD_REGISTRATION */
/* OY_IMAGE_WRITE_REGISTRATION */
/* OY_IMAGE_REGIONS_REGISTRATION */
/* OY_IMAGE_ROOT_REGISTRATION */
/* OY_IMAGE_OUTPUT_REGISTRATION */


oyDATATYPE_e oyra_image_data_types[7] = {oyUINT8, oyUINT16, oyUINT32,
                                         oyHALF, oyFLOAT, oyDOUBLE, 0};

/* OY_IMAGE_WRITE_REGISTRATION ---------------------------------------------*/


oyOptions_s* oyraFilter_ImageWriteValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = filter->type_ != oyOBJECT_FILTER_CORE_S;

  *result = error;

  return 0;
}

/** @func    oyraFilterNode_ImageWriteContextToMem
 *  @brief   implement oyCMMFilter_ContextToMem_f()
 *
 *  Serialise into a Oyranos specific ICC profile containers "Info" tag.
 *  We do not have any binary context to include.
 *  Thus oyFilterNode_TextToInfo_() is fine.
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/07/19 (Oyranos: 0.5.0)
 *  @date    2012/07/19
 */
oyPointer  oyraFilterNode_ImageWriteContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  return oyFilterNode_TextToInfo_( node, size, allocateFunc );
}

/** @func    oyraFilterPlug_ImageWriteRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/07/19 (Oyranos: 0.5.0)
 *  @date    2012/07/19
 */
int      oyraFilterPlug_ImageWriteRun (
                                       oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int result = 0;
  oyFilterSocket_s * socket = NULL;
  oyFilterNode_s * node = NULL;
  oyImage_s * image = NULL;
  oyCMMapiFilter_s * api = 0;
  oyCMMapiFilters_s * apis = 0;

  if(requestor_plug->type_ == oyOBJECT_FILTER_PLUG_S)
    socket = requestor_plug->remote_socket_;
  else if(requestor_plug->type_ == oyOBJECT_FILTER_SOCKET_S)
    socket = (oyFilterSocket_s*) requestor_plug;

  node = socket->node;

  image = (oyImage_s*)socket->data;
  if(image)
  {
    uint32_t i, j, k, n,
           * rank_list = 0;
    const char * filename = oyOptions_FindString( node->core->options_, "filename", 0 );
    const char * fileext = 0;
    char * file_ext = 0;
    int run = -1;

    if(filename)
    {
      fileext = strrchr( filename, '.' );
      if(fileext)
        ++fileext;
    } else
    {
      oyra_msg( oyMSG_WARN, (oyStruct_s*)requestor_plug,
         OY_DBG_FORMAT_ "Could not find a filename extension to select module.",
               OY_DBG_ARGS_ );
      result = 1;
      return result;
    }

    if(fileext)
    {
      STRING_ADD( file_ext, fileext );
      i = 0;
      while(file_ext[i]) { file_ext[i] = tolower( file_ext[i] ); ++i; }
    }

    apis = oyCMMsGetFilterApis_( 0,0, "//" OY_TYPE_STD "/file_write", 
                                 oyOBJECT_CMM_API7_S,
                                 oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                 &rank_list,0 );

    n = oyCMMapiFilters_Count( apis );
    if(apis)
    {
      for(i = 0; i < n; ++i)
      {
        int file_write = 0,
            image_pixel = 0,
            found = 0;
        oyCMMapi7_s * api7;
        char * api_ext = 0;

        j = 0;
        api = oyCMMapiFilters_Get( apis, i );
        api7 = (oyCMMapi7_s*) api;

        if(api7->properties)
          while(api7->properties[j] && api7->properties[j][0])
          {
            if(strcmp( api7->properties[j], "file=write" ) == 0)
              file_write = 1;

            if(strstr( api7->properties[j], "image=" ) != 0 &&
               strstr( api7->properties[j], "pixel" ) != 0)
              image_pixel = 1;

            if(file_ext && strstr( api7->properties[j], "ext=" ) != 0)
            {
              STRING_ADD( api_ext,  &api7->properties[j][4] );
              k = 0;
              while(api_ext[k]) { api_ext[k] = tolower( api_ext[k] ); ++k; }
              if(strstr( api_ext, file_ext ) != 0)
                found = 1;
              oyFree_m_( api_ext );
            }
            ++j;
          }


        if(file_write && image_pixel && found)
        {
          DBGs_NUM2_S( ticket, "%s={%s}", _("Run ticket through api7"),
                       api7->registration );
          result = api7->oyCMMFilterPlug_Run( requestor_plug, ticket );
          i = n;
          run = i;
        }

        if(api->release)
          api->release( (oyStruct_s**)&api );
      }
      oyCMMapiFilters_Release( &apis );
    }

    if( run < 0 )
      oyra_msg( oyMSG_WARN, (oyStruct_s*)requestor_plug,
             OY_DBG_FORMAT_ "Could not find fitting file_write plugin. %d",
             OY_DBG_ARGS_, n );

    if( !n )
      oyra_msg( oyMSG_WARN, (oyStruct_s*)requestor_plug,
             OY_DBG_FORMAT_ "Could not find any file_write plugin.",
             OY_DBG_ARGS_ );

    oyFree_m_(file_ext);
  }

  return result;
}


oyConnectorImaging_s oyra_imageWrite_plug = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  oyCMMgetImageConnectorPlugText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  1, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_colour_count; */
  255, /* max_colour_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap colour channels (BGR)*/
  1, /* can_swap_bytes; non host byte order */
  1, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  1, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* count in channel_types */
  1, /* id; relative to oyFilter_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s *oyra_imageWrite_plugs[2] = {&oyra_imageWrite_plug,0};

oyConnectorImaging_s oyra_imageWrite_socket = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  oyCMMgetImageConnectorSocketText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_colour_count; */
  255, /* max_colour_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap colour channels (BGR)*/
  1, /* can_swap_bytes; non host byte order */
  1, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  1, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* count in channel_types */
  2, /* id; relative to oyFilter_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s *oyra_imageWrite_sockets[2] = {&oyra_imageWrite_socket,0};


#define OY_IMAGE_WRITE_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "file_write.meta._" CMM_NICK
/** @instance oyra_api7
 *  @brief    oyra oyCMMapi7_s implementation
 *
 *  a filter abstraction image file writing
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/07/19 (Oyranos: 0.5.0)
 *  @date    2012/07/19
 */
oyCMMapi7_s   oyra_api7_image_write = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  0/*(oyCMMapi_s*) & oyra_api4_image_write*/, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_WRITE_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  {0,3,0},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilterPlug_ImageWriteRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  (oyConnector_s**) oyra_imageWrite_plugs,   /* plugs */
  1,   /* plugs_n */
  0,   /* plugs_last_add */
  (oyConnector_s**) oyra_imageWrite_sockets,   /* sockets */
  1,   /* sockets_n */
  0,   /* sockets_last_add */

  0    /* char * properties */
};

const char * oyraApi4UiImageWriteGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  if(strcmp(select,"name"))
  {
    if(type == oyNAME_NICK)
      return "write";
    else if(type == oyNAME_NAME)
      return _("Write");
    else if(type == oyNAME_DESCRIPTION)
      return _("Load Image File Object");
  } else if(strcmp(select,"help"))
  {
    if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("unfinished");
    else if(type == oyNAME_DESCRIPTION)
      return _("unfinished");
  }
  return 0;
}
const char * oyra_api4_ui_image_write_texts[] = {"name", "help", 0};

/** @instance oyra_api4_ui_image_write
 *  @brief    oyra oyCMMapi4_s::ui implementation
 *
 *  The UI for filter image write.
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/07/19 (Oyranos: 0.5.0)
 *  @date    2012/07/19
 */
oyCMMui_s oyra_api4_ui_image_write = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  {0,5,0},                            /**< int32_t module_api[3] */

  oyraFilter_ImageRootValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyraWidgetEvent, /* oyWidgetEvent_f */

  "Graph/File Load", /* category */
  0,   /* const char * options */
  0,   /* oyCMMuiGet_f oyCMMuiGet */

  oyraApi4UiImageWriteGetText,  /* oyCMMGetText_f getText */
  oyra_api4_ui_image_write_texts  /* (const char**)texts */
};

/** @instance oyra_api4
 *  @brief    oyra oyCMMapi4_s implementation
 *
 *  a filter abstraction image file writeing
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/07/19 (Oyranos: 0.5.0)
 *  @date    2012/07/19
 */
oyCMMapi4_s   oyra_api4_image_write = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api7_image_write, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_WRITE_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  {0,5,0},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilterNode_ImageRootContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem */
  {0}, /* char context_type[8] */

  &oyra_api4_ui_image_write             /**< filter UI */
};

/* OY_IMAGE_WRITE_REGISTRATION ---------------------------------------------*/

/* OY_IMAGE_LOAD_REGISTRATION ---------------------------------------------*/


oyOptions_s* oyraFilter_ImageLoadValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = filter->type_ != oyOBJECT_FILTER_CORE_S;

  *result = error;

  return 0;
}

/** @func    oyraFilterNode_ImageLoadContextToMem
 *  @brief   implement oyCMMFilter_ContextToMem_f()
 *
 *  Serialise into a Oyranos specific ICC profile containers "Info" tag.
 *  We do not have any binary context to include.
 *  Thus oyFilterNode_TextToInfo_() is fine.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/15 (Oyranos: 0.1.10)
 *  @date    2009/07/15
 */
oyPointer  oyraFilterNode_ImageLoadContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  return oyFilterNode_TextToInfo_( node, size, allocateFunc );
}

/** @func    oyraFilterPlug_ImageLoadRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/15 (Oyranos: 0.1.10)
 *  @date    2010/06/24
 */
int      oyraFilterPlug_ImageLoadRun (
                                       oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int result = 0, error = 0;
  oyFilterSocket_s * socket = NULL;
  oyFilterNode_s * node = NULL;
  oyImage_s * image = NULL;
  oyCMMapiFilter_s * api = 0;
  oyCMMapiFilters_s * apis = 0;

  if(requestor_plug->type_ == oyOBJECT_FILTER_PLUG_S)
    socket = requestor_plug->remote_socket_;
  else if(requestor_plug->type_ == oyOBJECT_FILTER_SOCKET_S)
    socket = (oyFilterSocket_s*) requestor_plug;

  node = socket->node;
  image = (oyImage_s*)socket->data;

  image = (oyImage_s*)socket->data;
  if(!image)
  {
    uint32_t i, j, k, n,
           * rank_list = 0;
    const char * filename = oyOptions_FindString( node->core->options_, "filename", 0 );
    const char * fileext = 0;
    char * file_ext = 0;

    if(filename)
    {
      fileext = strrchr( filename, '.' );
      if(fileext)
        ++fileext;
    } else
    {
      oyra_msg( oyMSG_WARN, (oyStruct_s*)requestor_plug,
         OY_DBG_FORMAT_ "Could not find a filename extension to select module.",
               OY_DBG_ARGS_ );
      result = 1;
      return result;
    }

    if(fileext)
    {
      STRING_ADD( file_ext, fileext );
      i = 0;
      while(file_ext[i]) { file_ext[i] = tolower( file_ext[i] ); ++i; }
    }

    apis = oyCMMsGetFilterApis_( 0,0, "//" OY_TYPE_STD "/file_read", 
                                 oyOBJECT_CMM_API7_S,
                                 oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                 &rank_list,0 );

    n = oyCMMapiFilters_Count( apis );
    if(apis)
    {
      for(i = 0; i < n; ++i)
      {
        int file_read = 0,
            image_pixel = 0,
            found = 0;
        oyCMMapi7_s * api7;
        char * api_ext = 0;

        j = 0;
        api = oyCMMapiFilters_Get( apis, i );
        api7 = (oyCMMapi7_s*) api;

        if(api7->properties)
          while(api7->properties[j] && api7->properties[j][0])
          {
            if(strcmp( api7->properties[j], "file=read" ) == 0)
              file_read = 1;

            if(strstr( api7->properties[j], "image=" ) != 0 &&
               strstr( api7->properties[j], "pixel" ) != 0)
              image_pixel = 1;

            if(file_ext && strstr( api7->properties[j], "ext=" ) != 0)
            {
              STRING_ADD( api_ext,  &api7->properties[j][4] );
              k = 0;
              while(api_ext[k]) { api_ext[k] = tolower( api_ext[k] ); ++k; }
              if(strstr( api_ext, file_ext ) != 0)
                found = 1;
              oyFree_m_( api_ext );
            }
            ++j;
          }


        if(file_read && image_pixel && found)
        {
          DBGs_NUM2_S( ticket, "%s={%s}", _("Run ticket through api7"),
                       api7->registration );
          result = api7->oyCMMFilterPlug_Run( requestor_plug, ticket );
          i = n;
        }

        if(api->release)
          api->release( (oyStruct_s**)&api );
      }
      oyCMMapiFilters_Release( &apis );
    }

    if( !n )
      oyra_msg( oyMSG_WARN, (oyStruct_s*)requestor_plug,
             OY_DBG_FORMAT_ "Could not find any file_load plugin.",
             OY_DBG_ARGS_ );

    oyFree_m_(file_ext);
  }

  /* set the data */
  if(requestor_plug->type_ == oyOBJECT_FILTER_PLUG_S &&
     requestor_plug->remote_socket_->data)
  {
    error = oyraFilterPlug_ImageRootRun( requestor_plug, ticket );

    return error;
  }

  return result;
}


oyConnectorImaging_s oyra_imageLoad_plug = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  oyCMMgetImageConnectorPlugText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/generator.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  1, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_colour_count; */
  255, /* max_colour_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap colour channels (BGR)*/
  1, /* can_swap_bytes; non host byte order */
  1, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  1, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* count in channel_types */
  1, /* id; relative to oyFilter_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s *oyra_imageLoad_plugs[2] = {&oyra_imageLoad_plug,0};

oyConnectorImaging_s oyra_imageLoad_socket = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  oyCMMgetImageConnectorSocketText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/generator.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_colour_count; */
  255, /* max_colour_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap colour channels (BGR)*/
  1, /* can_swap_bytes; non host byte order */
  1, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  1, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* count in channel_types */
  2, /* id; relative to oyFilter_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s *oyra_imageLoad_sockets[2] = {&oyra_imageLoad_socket,0};


#define OY_IMAGE_LOAD_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "file_read.meta._" CMM_NICK
/** @instance oyra_api7
 *  @brief    oyra oyCMMapi7_s implementation
 *
 *  a filter abstraction image file loading
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/15 (Oyranos: 0.1.10)
 *  @date    2009/07/15
 */
oyCMMapi7_s   oyra_api7_image_load = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api4_image_write, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_LOAD_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  {0,3,0},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilterPlug_ImageLoadRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  (oyConnector_s**) oyra_imageLoad_plugs,   /* plugs */
  1,   /* plugs_n */
  0,   /* plugs_last_add */
  (oyConnector_s**) oyra_imageLoad_sockets,   /* sockets */
  1,   /* sockets_n */
  0,   /* sockets_last_add */

  0    /* char * properties */
};

const char * oyraApi4UiImageLoadGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  if(strcmp(select,"name"))
  {
    if(type == oyNAME_NICK)
      return "load";
    else if(type == oyNAME_NAME)
      return _("Load");
    else if(type == oyNAME_DESCRIPTION)
      return _("Load Image File Object");
  } else if(strcmp(select,"help"))
  {
    if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("unfinished");
    else if(type == oyNAME_DESCRIPTION)
      return _("unfinished");
  }
  return 0;
}
const char * oyra_api4_ui_image_load_texts[] = {"name", "help", 0};

/** @instance oyra_api4_ui_image_load
 *  @brief    oyra oyCMMapi4_s::ui implementation
 *
 *  The UI for filter image load.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/09/09
 */
oyCMMui_s oyra_api4_ui_image_load = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  {0,3,0},                            /**< int32_t module_api[3] */

  oyraFilter_ImageRootValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyraWidgetEvent, /* oyWidgetEvent_f */

  "Graph/File Load", /* category */
  0,   /* const char * options */
  0,   /* oyCMMuiGet_f oyCMMuiGet */

  oyraApi4UiImageLoadGetText,  /* oyCMMGetText_f getText */
  oyra_api4_ui_image_load_texts  /* (const char**)texts */
};

/** @instance oyra_api4
 *  @brief    oyra oyCMMapi4_s implementation
 *
 *  a filter abstraction image file loading
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/15 (Oyranos: 0.1.10)
 *  @date    2009/07/15
 */
oyCMMapi4_s   oyra_api4_image_load = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api7_image_load, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_LOAD_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  {0,3,0},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilterNode_ImageRootContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem */
  {0}, /* char context_type[8] */

  &oyra_api4_ui_image_load             /**< filter UI */
};

/* OY_IMAGE_LOAD_REGISTRATION ---------------------------------------------*/

/* OY_IMAGE_REGIONS_REGISTRATION ---------------------------------------------*/


oyOptions_s* oyraFilter_ImageRectanglesValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = filter->type_ != oyOBJECT_FILTER_CORE_S;

  *result = error;

  return 0;
}

/** @func    oyraFilterNode_ImageRectanglesContextToMem
 *  @brief   implement oyCMMFilter_ContextToMem_f()
 *
 *  Serialise into a Oyranos specific ICC profile containers "Info" tag.
 *  We do not have any binary context to include.
 *  Thus oyFilterNode_TextToInfo_() is fine.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 *  @date    2009/02/23
 */
oyPointer  oyraFilterNode_ImageRectanglesContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  return oyFilterNode_TextToInfo_( node, size, allocateFunc );
}

/** @func    oyraFilterPlug_ImageRectanglesRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
int      oyraFilterPlug_ImageRectanglesRun (
                                       oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int x = 0, y = 0, n = 0, i;
  int result = 0, l_result = 0, error = 0;
  int is_allocated = 0;
  oyPointer * ptr = 0;
  oyFilterSocket_s * socket = requestor_plug->remote_socket_;
  oyFilterNode_s * input_node = 0,
                 * node = socket->node;
  oyImage_s * image = (oyImage_s*)socket->data;
  oyOption_s * o = 0;
  oyRectangle_s * r;
  oyRectangle_s array_pix = {oyOBJECT_RECTANGLE_S,0,0,0};

  oyPixelAccess_s * new_ticket = 0;
  int dirty = 0;

  image = (oyImage_s*)socket->data;
  if(!image)
    return 1;

  x = ticket->start_xy[0] * image->width;
  y = ticket->start_xy[1] * image->width;

  if(x < image->width &&
     y < image->height &&
     ticket->pixels_n)
  {
    n = ticket->pixels_n;
    if(n == 1)
      ptr = image->getPoint( image, x, y, 0, &is_allocated );

    result = !ptr;

  } else {

    if(!node->core->options_)
      dirty = 1;

    if(dirty)
      return dirty;

    n = oyOptions_CountType( node->core->options_,
                             "//" OY_TYPE_STD "/rectangles/rectangle",
                             oyOBJECT_RECTANGLE_S );

    /* rectangles stuff */
    for(i = 0; i < n; ++i)
    {
      /* select current rectangle */
      r = (oyRectangle_s*)oyOptions_GetType( node->core->options_, i,
                                       "//" OY_TYPE_STD "/rectangles/rectangle",
                                             oyOBJECT_RECTANGLE_S );

      /* Map each matching plug to a new ticket with a corrected rectangle. */
      new_ticket = oyPixelAccess_Copy( ticket, ticket->oy_ );
      oyArray2d_Release( &new_ticket->array );
      DBGs_NUM2_S( ticket, "%s[%d]", _("Created new_ticket"),
                   oyStruct_GetId( (oyStruct_s*)new_ticket ) );

      if(r)
        oyRectangle_SetByRectangle( new_ticket->output_image_roi, r );

      /* select node */
      input_node = node->plugs[i]->remote_socket_->node;
      /* adapt the rectangle of interesst to the new image dimensions */
      oyRectangle_Trim( new_ticket->output_image_roi, ticket->output_image_roi );

      /* use the correct source pixels */
      new_ticket->start_xy[0] += new_ticket->output_image_roi->x;
      new_ticket->start_xy[1] += new_ticket->output_image_roi->y;

      if(oyRectangle_CountPoints(  new_ticket->output_image_roi ) > 0)
      {

        /* fill the array rectangle for the following filter */
        if(!new_ticket->array)
        {
          DBGs_NUM2_S( new_ticket, "%s[%d]", 
                      _("Fill new_ticket->array from new_ticket->output_image"),
                      oyStruct_GetId( (oyStruct_s*)new_ticket->output_image ) );
          oyImage_FillArray( new_ticket->output_image,
                             new_ticket->output_image_roi, 0,
                             &new_ticket->array, new_ticket->output_image_roi,
                             0 );
        }

        /* start new call into branch */
        DBGs_NUM2_S( new_ticket, "%s[%d]",
                     _("Run new_ticket through filter in node"),
                     oyStruct_GetId( (oyStruct_s*)node ) );
        l_result = input_node->api7_->oyCMMFilterPlug_Run( node->plugs[i],
                                                           new_ticket );
        if(l_result != 0 && (result <= 0 || l_result > 0))
          result = l_result;

        /* The image is used as intermediate cache between the forward and
           backward array. @todo use direct array copy oyArray2d_DataCopy() */
        DBGs_NUM2_S( new_ticket,"%s[%d]",_("Read new_ticket->array into image"),
                     oyStruct_GetId( (oyStruct_s*)image ) );
        error = oyImage_ReadArray( new_ticket->output_image,
                                   new_ticket->output_image_roi,
                                   new_ticket->array, 0 );
        if(error) WARNc2_S("%s %d", _("found issues"),error);
        DBGs_NUM2_S( ticket, "%s[%d]",
                     _("Fill ticket->array from new_ticket->output_image"),
                    oyStruct_GetId( (oyStruct_s*)new_ticket->output_image ) );
        error = oyImage_FillArray( new_ticket->output_image,
                                   new_ticket->output_image_roi, 1,
                                   &ticket->array, new_ticket->output_image_roi,
                                   0 );
        if(error) WARNc2_S("%s %d", _("found issues"),error);
      }
      oyPixelAccess_Release( &new_ticket );

      oyOption_Release( &o );
    }

    oyRectangle_SetGeo( &array_pix, 0,0,
              ticket->array->data_area.width, ticket->array->data_area.height );
    error = oyArray2d_SetFocus( ticket->array, &array_pix );
    if(error) WARNc2_S("%s %d", _("found issues"),error);
  }

  return result;
}


oyConnectorImaging_s oyra_imageRectangles_plug = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  oyCMMgetImageConnectorPlugText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/splitter.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  1, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_colour_count; */
  255, /* max_colour_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap colour channels (BGR)*/
  1, /* can_swap_bytes; non host byte order */
  1, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  1, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* count in channel_types */
  1, /* id; relative to oyFilter_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s *oyra_imageRectangles_plugs[2] = {&oyra_imageRectangles_plug,0};

oyConnectorImaging_s oyra_imageRectangles_socket = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  oyCMMgetImageConnectorSocketText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_colour_count; */
  255, /* max_colour_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap colour channels (BGR)*/
  1, /* can_swap_bytes; non host byte order */
  1, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  1, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* count in channel_types */
  2, /* id; relative to oyFilter_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s *oyra_imageRectangles_sockets[2] = {&oyra_imageRectangles_socket,0};


#define OY_IMAGE_REGIONS_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "rectangles"
/** @instance oyra_api7
 *  @brief    oyra oyCMMapi7_s implementation
 *
 *  a filter routing the graph to several rectangles
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/02/24
 */
oyCMMapi7_s   oyra_api7_image_rectangles = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api4_image_write_ppm, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_REGIONS_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  {0,3,0},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilterPlug_ImageRectanglesRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  (oyConnector_s**) oyra_imageRectangles_plugs,   /* plugs */
  1,   /* plugs_n */
  UINT16_MAX,   /* plugs_last_add */
  (oyConnector_s**) oyra_imageRectangles_sockets,   /* sockets */
  1,   /* sockets_n */
  0    /* sockets_last_add */
};

const char * oyraApi4UiImageRectanglesGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  if(strcmp(select,"name"))
  {
    if(type == oyNAME_NICK)
      return "rectangles";
    else if(type == oyNAME_NAME)
      return _("Rectangles");
    else if(type == oyNAME_DESCRIPTION)
      return _("Rectangles Splitter Object");
  } else if(strcmp(select,"help"))
  {
    if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Apply regions of interesst in form of simple rectangles.");
    else if(type == oyNAME_DESCRIPTION)
      return _("The filter will expect a \"rectangle\" option and will create, fill and process a according rectangle with a new job ticket.");
  }
  return 0;
}
const char * oyra_api4_ui_image_rectangles_texts[] = {"name", "help", 0};

/** @instance oyra_api4_ui_image_rectangles
 *  @brief    oyra oyCMMapi4_s::ui implementation
 *
 *  The UI for filter image rectangles.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/09/09
 */
oyCMMui_s oyra_api4_ui_image_rectangles = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  {0,3,0},                            /**< int32_t module_api[3] */

  oyraFilter_ImageRootValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyraWidgetEvent, /* oyWidgetEvent_f */

  "Graph/Rectangles", /* category */
  0,   /* const char * options */
  0,   /* oyCMMuiGet_f oyCMMuiGet */

  oyraApi4UiImageRectanglesGetText,  /* oyCMMGetText_f getText */
  oyra_api4_ui_image_rectangles_texts  /* (const char**)texts */
};

/** @instance oyra_api4
 *  @brief    oyra oyCMMapi4_s implementation
 *
 *  a filter routing the graph to several rectangles
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/02/24
 */
oyCMMapi4_s   oyra_api4_image_rectangles = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api7_image_rectangles, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_REGIONS_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  {0,3,0},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilterNode_ImageRootContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem */
  {0}, /* char context_type[8] */

  &oyra_api4_ui_image_rectangles       /**< oyCMMui_s *ui */
};

/* OY_IMAGE_REGIONS_REGISTRATION ---------------------------------------------*/

/* OY_IMAGE_ROOT_REGISTRATION ------------------------------------------------*/


oyOptions_s* oyraFilter_ImageRootValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = filter->type_ != oyOBJECT_FILTER_CORE_S;

#if 0
  if(!error)
    if(filter->image_ && filter->image_->layout_)
    {
      oyDATATYPE_e data_type = oyToDataType_m( filter->image_->layout_[0] );
      if(!(data_type == oyUINT8 ||
           data_type == oyUINT16 ||
           data_type == oyUINT32 ||
           data_type == oyHALF ||
           data_type == oyFLOAT ||
           data_type == oyDOUBLE
                                   ))
        error = 1;
    }
#endif

  *result = error;

  return 0;
}

/** @func    oyraFilterNode_ImageRootContextToMem
 *  @brief   implement oyCMMFilter_ContextToMem_f()
 *
 *  Serialise into a Oyranos specific ICC profile containers "Info" tag.
 *  We do not have any binary context to include.
 *  Thus oyFilterNode_TextToInfo_() is fine.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/17 (Oyranos: 0.1.8)
 *  @date    2008/07/18
 */
oyPointer  oyraFilterNode_ImageRootContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  return oyFilterNode_TextToInfo_( node, size, allocateFunc );
}

/** @func    oyraFilterPlug_ImageRootRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/10 (Oyranos: 0.1.8)
 *  @date    2009/05/01
 */
int      oyraFilterPlug_ImageRootRun ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  return oyFilterPlug_ImageRootRun( requestor_plug, ticket );
}


oyConnectorImaging_s oyra_imageRoot_connector = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  oyCMMgetImageConnectorSocketText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_colour_count; */
  255, /* max_colour_count; */
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
oyConnectorImaging_s * oyra_imageRoot_connectors[2] = {&oyra_imageRoot_connector,0};


#define OY_IMAGE_ROOT_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "root"
/** @instance oyra_api7
 *  @brief    oyra oyCMMapi7_s implementation
 *
 *  a filter providing a source image
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
oyCMMapi7_s   oyra_api7_image_root = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api4_image_output, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_ROOT_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  {0,3,0},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilterPlug_ImageRootRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  0,   /* plugs */
  0,   /* plugs_n */
  0,   /* plugs_last_add */
  (oyConnector_s**) oyra_imageRoot_connectors,   /* sockets */
  1,   /* sockets_n */
  0    /* sockets_last_add */
};

const char * oyraApi4UiImageRootGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  if(strcmp(select,"name"))
  {
    if(type == oyNAME_NICK)
      return "image_root";
    else if(type == oyNAME_NAME)
      return _("Root Image");
    else if(type == oyNAME_DESCRIPTION)
      return _("Root Image Filter Object");
  } else if(strcmp(select,"help"))
  {
    if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("The filter provides a generic image source.");
    else if(type == oyNAME_DESCRIPTION)
      return _("The root image filter can hold pixel data for processing in a graph.");
  }
  return 0;
}
const char * oyra_api4_ui_image_root_texts[] = {"name", "help", 0};

/** @instance oyra_api4_ui_image_root
 *  @brief    oyra oyCMMapi4_s::ui implementation
 *
 *  The UI for filter image root.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/09/09
 */
oyCMMui_s oyra_api4_ui_image_root = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  {0,3,0},                            /**< int32_t module_api[3] */

  oyraFilter_ImageRootValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyraWidgetEvent, /* oyWidgetEvent_f */

  "Image/Simple Image[in]", /* category */
  0,   /* const char * options */
  0,   /* oyCMMuiGet_f oyCMMuiGet */

  oyraApi4UiImageRootGetText, /* oyCMMGetText_f getText */
  oyra_api4_ui_image_root_texts /* (const char**)texts */
};

/** @instance oyra_api4
 *  @brief    oyra oyCMMapi4_s implementation
 *
 *  a filter providing a source image
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/08 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyCMMapi4_s   oyra_api4_image_root = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api7_image_root, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_ROOT_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  {0,3,0},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilterNode_ImageRootContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem */
  {0}, /* char context_type[8] */

  &oyra_api4_ui_image_root             /**< oyCMMui_s *ui */
};


/* OY_IMAGE_ROOT_REGISTRATION ------------------------------------------------*/

/* OY_IMAGE_OUTPUT_REGISTRATION ----------------------------------------------*/

oyConnectorImaging_s oyra_imageOutput_connector = {
  oyOBJECT_CONNECTOR_S,0,0,0,
  oyCMMgetImageConnectorPlugText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  1, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_colour_count; */
  255, /* max_colour_count; */
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
oyConnectorImaging_s* oyra_imageOutput_connectors[2] = {&oyra_imageOutput_connector,0};


/** @func    oyraFilter_ImageOutputRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/19 (Oyranos: 0.1.8)
 *  @date    2008/10/03
 */
int      oyraFilterPlug_ImageOutputRun(oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  oyFilterSocket_s * socket = requestor_plug->remote_socket_;
  oyFilterNode_s * node = 0;
  int result = 0;

  if(socket)
    node = socket->node;

  /* to reuse the requestor_plug is a exception for the starting request */
  if(node)
  {
    DBGs_NUM2_S( ticket, "%s[%d]", _("Call next filter in node"),
                 oyStruct_GetId( (oyStruct_s*)node ) );
    result = node->api7_->oyCMMFilterPlug_Run( requestor_plug, ticket );
  } else
    result = 1;

  return result;
}


#define OY_IMAGE_OUTPUT_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "output"
/** @instance oyra_api7
 *  @brief    oyra oyCMMapi7_s implementation
 *
 *  a filter providing a target image
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
oyCMMapi7_s   oyra_api7_image_output = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api4_image_rectangles, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_OUTPUT_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  {0,3,0},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilterPlug_ImageOutputRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  (oyConnector_s**) oyra_imageOutput_connectors,   /* plugs */
  1,   /* plugs_n */
  0,   /* plugs_last_add */
  0,   /* sockets */
  0,   /* sockets_n */
  0    /* sockets_last_add */
};

const char * oyraApi4UiImageOutputGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  if(strcmp(select,"name"))
  {
    if(type == oyNAME_NICK)
      return "image_out";
    else if(type == oyNAME_NAME)
      return _("Image[out]");
    else if(type == oyNAME_DESCRIPTION)
      return _("Output Image Filter Object");
  } else if(strcmp(select,"help"))
  {
    if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("The filter is a possible pixel target in a graph.");
    else if(type == oyNAME_DESCRIPTION)
      return _("The filter can query pixels from its successors in a graph. The results are stored in the internal buffer.");
  }
  return 0;
}
const char * oyra_api4_ui_image_output_texts[] = {"name", "help", 0};

/** @instance oyra_api4_ui_image_output
 *  @brief    oyra oyCMMapi4_s::ui implementation
 *
 *  The UI for filter image output.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/09/09
 */
oyCMMui_s oyra_api4_ui_image_output = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  {0,3,0},                            /**< int32_t module_api[3] */

  oyraFilter_ImageRootValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyraWidgetEvent, /* oyWidgetEvent_f */

  "Image/Simple Image[out]", /* category */
  0,   /* const char * options */
  0,   /* oyCMMuiGet_f oyCMMuiGet */

  oyraApi4UiImageOutputGetText,  /* oyCMMGetText_f getText */
  oyra_api4_ui_image_output_texts  /* (const char**)texts */
};

/** @instance oyra_api4
 *  @brief    oyra oyCMMapi4_s implementation
 *
 *  a filter providing a target image
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/19 (Oyranos: 0.1.8)
 *  @date    2008/07/19
 */
oyCMMapi4_s   oyra_api4_image_output = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api7_image_output, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_OUTPUT_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  {0,3,0},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  oyraFilterNode_ImageRootContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem */
  {0}, /* char context_type[8] */

  &oyra_api4_ui_image_output           /**< oyCMMui_s *ui */
};

/* OY_IMAGE_OUTPUT_REGISTRATION ----------------------------------------------*/
/* ---------------------------------------------------------------------------*/



