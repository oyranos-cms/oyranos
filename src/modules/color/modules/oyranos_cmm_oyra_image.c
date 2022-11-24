/** @file oyranos_cmm_oyra_image.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2008-2014 (C) Kai-Uwe Behrmann
 *
 *  @brief    modules for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2008/01/02
 */

#include "oyCMMapi4_s.h"
#include "oyCMMapi4_s_.h"
#include "oyCMMapi7_s.h"
#include "oyCMMapi7_s_.h"
#include "oyCMMapiFilters_s.h"
#include "oyCMMui_s_.h"
#include "oyConnectorImaging_s_.h"
#include "oyFilterNode_s_.h"         /* for oyFilterNode_TextToInfo_ */
#include "oyRectangle_s_.h"

#include "oyranos_config_internal.h"
#include "oyranos_cmm.h"
#include "oyranos_cmm_oyra.h"
#include "oyranos_generic.h"         /* oy_connector_imaging_static_object */
#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_definitions.h"
#include "oyranos_module_internal.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"

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


/** \addtogroup graph_modules
 *  @{ */
/** \addtogroup oyra_graph oyra Module
 *  @brief      Image Processing Nodes
 *
 *  The module provides file I/O, region handling, source and output.
 *
 *  - ::oyra_api7_image_load -
 *      a abstraction filter for image file loading.
 *  - ::oyra_api7_image_write - 
 *      a abstraction filter for image file writing.
 *  - ::oyra_api7_image_rectangles - 
 *      a filter collecting results from several rectangles.
 *  - ::oyra_api7_image_root - 
 *      a filter providing a source image
 *  - ::oyra_api7_image_output - 
 *      a filter providing a target image as output
 *
 *  @{ */

oyDATATYPE_e oyra_image_data_types[7] = {oyUINT8, oyUINT16, oyUINT32,
                                         oyHALF, oyFLOAT, oyDOUBLE, 0};

/* OY_IMAGE_WRITE_REGISTRATION ---------------------------------------------*/


oyOptions_s* oyraFilter_ImageWriteValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate OY_UNUSED,
                                       int                 statical OY_UNUSED,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = filter->type_ != oyOBJECT_FILTER_CORE_S;

  *result = error;

  return 0;
}


/** @brief   Target File Image meta filter
 *
 *  implement oyCMMFilter_GetNext_f()
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
    socket = oyFilterPlug_GetSocket( requestor_plug );
  else if(requestor_plug->type_ == oyOBJECT_FILTER_SOCKET_S)
    socket = (oyFilterSocket_s*) requestor_plug;

  node = oyFilterSocket_GetNode( socket );

  image = (oyImage_s*)oyFilterSocket_GetData( socket );
  if(image)
  {
    uint32_t i, j, k, n,
           * rank_list = 0;
    const char * filename;
    const char * fileext = 0;
    char * file_ext = 0;
    int run = -1;

    {
      oyOptions_s * opts = oyFilterNode_GetOptions( node, 0 );
      filename = oyOptions_FindString( opts, "filename", 0 );
      oyOptions_Release( &opts );
    }
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
      while(file_ext && file_ext[i]) { file_ext[i]=tolower(file_ext[i]); ++i; }
    }
    else
      file_ext = oyjlStringCopy( "png", 0 );

    apis = oyCMMsGetFilterApis_( "//" OY_TYPE_STD "/file_write", 
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
        oyCMMapi7_s_ * api7;
        char * api_ext = 0;

        j = 0;
        api = oyCMMapiFilters_Get( apis, i );
        api7 = (oyCMMapi7_s_*) api;

        if(api7->properties)
          while(api7->properties[j] && api7->properties[j][0])
          {
            const char * prop = api7->properties[j];
            if(strcmp( prop, "file=write" ) == 0)
              file_write = 1;

            if(strstr( prop, "image=" ) != 0 &&
               strstr( prop, "pixel" ) != 0)
              image_pixel = 1;

            if(file_ext && strstr( prop, "ext=" ) != 0)
            {
              STRING_ADD( api_ext,  &prop[4] );
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
          DBGs_PROG2_S( ticket, "%s={%s}", "Run ticket through api7",
                       api7->registration );
          result = api7->oyCMMFilterPlug_Run( requestor_plug, ticket );
          if(result > 0)
            oyra_msg( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_ "Could not write to file: %s",
             OY_DBG_ARGS_, oyNoEmptyString_m_(filename) );
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

  oyFilterSocket_Release( &socket );
  oyFilterNode_Release( &node );
  oyImage_Release( &image );

  return result;
}


oyConnectorImaging_s_ oyra_imageWrite_plug = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorPlugText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  1, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_color_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_color_count; */
  255, /* max_color_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap color channels (BGR)*/
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
oyConnectorImaging_s_ *oyra_imageWrite_plugs[2] = {&oyra_imageWrite_plug,0};

oyConnectorImaging_s_ oyra_imageWrite_socket = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorSocketText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_color_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_color_count; */
  255, /* max_color_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap color channels (BGR)*/
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
oyConnectorImaging_s_ *oyra_imageWrite_sockets[2] = {&oyra_imageWrite_socket,0};


/** @brief registration string for \b oyra CMM */
#define OY_IMAGE_WRITE_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "file_write.meta._" CMM_NICK
/** @brief    Target File Image Meta Node
 *
 *  oyra oyCMMapi7_s implementation
 *
 *  A abstraction filter for image file loading.
 *  The filter searches for a suitable filter by the file name ending and
 *  runs it for writing the DAG to a given image file name.
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/07/19 (Oyranos: 0.5.0)
 *  @date    2012/07/19
 */
oyCMMapi7_s_   oyra_api7_image_write = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api10_profile_graph2d_SaturationLine, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMReset, /* oyCMMReset_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_WRITE_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  oyraFilterPlug_ImageWriteRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  (oyConnector_s**) oyra_imageWrite_plugs,   /* plugs */
  1,   /* plugs_n */
  0,   /* plugs_last_add */
  (oyConnector_s**) oyra_imageWrite_sockets,   /* sockets */
  1,   /* sockets_n */
  0,   /* sockets_last_add */

  0    /* char ** properties */
};

const char * oyraApi4UiImageWriteGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
  if(strcmp(select,"name") == 0)
  {
    if(type == oyNAME_NICK)
      return "write";
    else if(type == oyNAME_NAME)
      return _("Write");
    else if(type == oyNAME_DESCRIPTION)
      return _("Load Image File Object");
  } else if(strcmp(select,"help") == 0)
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

/** @brief    Target File Image Meta Node UI
 *
 *  oyra oyCMMapi4_s::ui implementation
 *
 *  The UI for filter image write.
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/07/19 (Oyranos: 0.5.0)
 *  @date    2012/07/19
 */
oyCMMui_s_ oyra_api4_ui_image_write = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */

  oyraFilter_ImageRootValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyraWidgetEvent, /* oyWidgetEvent_f */

  "Graph/File Load", /* category */
  0,   /* const char * options */
  0,   /* oyCMMuiGet_f oyCMMuiGet */

  oyraApi4UiImageWriteGetText,  /* oyCMMGetText_f getText */
  oyra_api4_ui_image_write_texts,  /* (const char**)texts */
  (oyCMMapiFilter_s*)&oyra_api4_image_write /* oyCMMapiFilter_s*parent */
};

/** @brief    Target File Image Meta Node (UI only)
 *
 *  oyra oyCMMapi4_s implementation
 *
 *  A abstraction filter for image file writing.
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/07/19 (Oyranos: 0.5.0)
 *  @date    2012/07/19
 */
oyCMMapi4_s_   oyra_api4_image_write = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api7_image_write, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMReset, /* oyCMMReset_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_WRITE_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  (oyCMMFilterNode_ContextToMem_f)oyFilterNode_TextToInfo_, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem */
  {0}, /* char context_type[8] */

  &oyra_api4_ui_image_write             /**< filter UI */
};

/* OY_IMAGE_WRITE_REGISTRATION ---------------------------------------------*/

/* OY_IMAGE_LOAD_REGISTRATION ---------------------------------------------*/


oyOptions_s* oyraFilter_ImageLoadValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate OY_UNUSED,
                                       int                 statical OY_UNUSED,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = filter->type_ != oyOBJECT_FILTER_CORE_S;

  *result = error;

  return 0;
}

/** @brief   Provide text info for debugging and hashing
 *
 *  implement oyCMMFilter_ContextToMem_f()
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
  return oyFilterNode_TextToInfo_( (oyFilterNode_s_*)node, size, allocateFunc );
}

/** @brief   Source File Image meta filter
 *
 *  implement oyCMMFilter_GetNext_f()
 *
 *  The filter searches for a suitable filter and select it for opening
 *  a given image file name. The loaded image is set to the filters socket.
 *  The pixels are copied into the ticket, during running the DAG with
 *  oyConversion_RunPixels().
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
  oyFilterSocket_s * socket = NULL, * socket_tmp = NULL;
  oyFilterNode_s * node = NULL;
  oyImage_s * image = NULL;
  oyCMMapiFilter_s * api = 0;
  oyCMMapiFilters_s * apis = 0;

  if(requestor_plug->type_ == oyOBJECT_FILTER_PLUG_S)
    socket = socket_tmp = oyFilterPlug_GetSocket( requestor_plug );
  else if(requestor_plug->type_ == oyOBJECT_FILTER_SOCKET_S)
    socket = (oyFilterSocket_s*) requestor_plug;

  node = oyFilterSocket_GetNode( socket );

  image = (oyImage_s*)oyFilterSocket_GetData( socket );
  if(!image)
  {
    uint32_t i, j, k, n,
           * rank_list = 0;
    const char * filename;
    const char * fileext = 0;
    char * file_ext = 0;

    {
      oyOptions_s * opts = oyFilterNode_GetOptions( node, 0 );
      filename = oyOptions_FindString( opts, "filename", 0 );
      oyOptions_Release( &opts );
    }
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
      while(file_ext && file_ext[i]) { file_ext[i]=tolower(file_ext[i]); ++i; }
    }

    apis = oyCMMsGetFilterApis_( "//" OY_TYPE_STD "/file_read", 
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
        oyCMMapi7_s_ * api7;
        char * api_ext = 0;

        j = 0;
        api = oyCMMapiFilters_Get( apis, i );
        api7 = (oyCMMapi7_s_*) api;

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
          DBGs_PROG2_S( ticket, "%s={%s}", "Run ticket through api7",
                        api7->registration );
          oyFilterPlug_s * plug = oyFilterNode_GetPlug( node, 0 );
          oyFilterSocket_s * sock = oyFilterPlug_GetSocket( plug );
          oyFilterNode_s * format = NULL, * tmp_node = NULL;

          if(!sock)
          {
            oyOptions_s * opts = oyFilterNode_GetOptions( node, 0 );
            format = oyFilterNode_NewWith( api7->registration, opts, 0 );
            oyOptions_Release( &opts );

            /* set the proper plug->socket, for detecting the calling node inside the filter's _Run method */
            error = oyFilterNode_Connect( format, "//" OY_TYPE_STD "/data",
                                          node, "//" OY_TYPE_STD "/data", 0 );
            if(error)
              WARNc1_S( "could not add  filter: %s\n", api7->registration );
          } else
            format = tmp_node = oyFilterSocket_GetNode( sock );

          result = oyFilterNode_Run( format, plug, ticket );

          /* move the image to the old requestor_plug, as it is expected there */
          if(!sock)
            sock = oyFilterPlug_GetSocket( plug );
          image = (oyImage_s*) oyFilterSocket_GetData( sock );
          if(image)
            oyFilterSocket_SetData( socket, (oyStruct_s*)image );

          oyFilterPlug_Release( &plug );
          oyFilterSocket_Release( &sock );
          oyImage_Release( &image );
          oyFilterNode_Release( &tmp_node );
          i = n;
        } else
          DBG_PROG2_S( "%s={%s}", "api7 not fitting",
                       api7->registration );

        if(api->release)
          api->release( (oyStruct_s**)&api );
      }
      oyCMMapiFilters_Release( &apis );
    }

    if( !n )
      oyra_msg( oyMSG_WARN, (oyStruct_s*)requestor_plug,
             OY_DBG_FORMAT_ "Could not find any file_load plugin.",
             OY_DBG_ARGS_ );
    else if(!result)
      oyra_msg( oyMSG_WARN, (oyStruct_s*)requestor_plug,
             OY_DBG_FORMAT_ "Could not find file_load plugin for: %s %s",
             OY_DBG_ARGS_, oyNoEmptyString_m_(filename), oyNoEmptyString_m_(file_ext) );

    oyFree_m_(file_ext);
  }

  /* set the data */
  if(requestor_plug->type_ == oyOBJECT_FILTER_PLUG_S &&
     image)
  {
    error = oyraFilterPlug_ImageRootRun( requestor_plug, ticket );

    return error;
  }

  oyImage_Release( &image );
  oyFilterNode_Release( &node );
  oyFilterSocket_Release( &socket_tmp );

  return result;
}


oyConnectorImaging_s_ oyra_imageLoad_plug = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorPlugText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/generator.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  1, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_color_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_color_count; */
  255, /* max_color_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap color channels (BGR)*/
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
oyConnectorImaging_s_ *oyra_imageLoad_plugs[2] = {&oyra_imageLoad_plug,0};

oyConnectorImaging_s_ oyra_imageLoad_socket = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorSocketText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/generator.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_color_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_color_count; */
  255, /* max_color_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap color channels (BGR)*/
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
oyConnectorImaging_s_ *oyra_imageLoad_sockets[2] = {&oyra_imageLoad_socket,0};


/** @brief registration string for \b oyra CMM */
#define OY_IMAGE_LOAD_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "file_read.meta._" CMM_NICK
/** @brief   Source File Image Meta Node
 *
 *  oyra oyCMMapi7_s implementation
 *
 *  A abstraction filter for image file loading.
 *  The filter searches for a suitable filter and select it for opening
 *  a given image file name. The loaded image is set to the filters socket.
 *  The pixels are copied into the ticket, during running the DAG with
 *  oyConversion_RunPixels().
 *
 *  @see oyraFilterPlug_ImageLoadRun() and oyra_api4_image_load
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/15 (Oyranos: 0.1.10)
 *  @date    2009/07/15
 */
oyCMMapi7_s_   oyra_api7_image_load = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api4_image_write, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMReset, /* oyCMMReset_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_LOAD_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  oyraFilterPlug_ImageLoadRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  (oyConnector_s**) oyra_imageLoad_plugs,   /* plugs */
  1,   /* plugs_n */
  0,   /* plugs_last_add */
  (oyConnector_s**) oyra_imageLoad_sockets,   /* sockets */
  1,   /* sockets_n */
  0,   /* sockets_last_add */

  0    /* char ** properties */
};

const char * oyraApi4UiImageLoadGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
  if(strcmp(select,"name") == 0)
  {
    if(type == oyNAME_NICK)
      return "load";
    else if(type == oyNAME_NAME)
      return _("Load");
    else if(type == oyNAME_DESCRIPTION)
      return _("Load Image File Object");
  } else if(strcmp(select,"help") == 0)
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

/** @brief    Source File Image Meta Node UI
 *
 *  oyra oyCMMapi4_s::ui implementation
 *
 *  The UI for filter image load.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/09/09
 */
oyCMMui_s_ oyra_api4_ui_image_load = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */

  oyraFilter_ImageRootValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyraWidgetEvent, /* oyWidgetEvent_f */

  "Graph/File Load", /* category */
  0,   /* const char * options */
  0,   /* oyCMMuiGet_f oyCMMuiGet */

  oyraApi4UiImageLoadGetText,  /* oyCMMGetText_f getText */
  oyra_api4_ui_image_load_texts,  /* (const char**)texts */
  (oyCMMapiFilter_s*)&oyra_api4_image_load /* oyCMMapiFilter_s*parent */
};

/** @brief    Source File Image Meta Node (UI only)
 *
 *  oyra oyCMMapi4_s implementation
 *
 *  A abstraction filter for image file loading.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/15 (Oyranos: 0.1.10)
 *  @date    2009/07/15
 */
oyCMMapi4_s_   oyra_api4_image_load = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api7_image_load, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMReset, /* oyCMMReset_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_LOAD_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  (oyCMMFilterNode_ContextToMem_f)oyFilterNode_TextToInfo_, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem */
  {0}, /* char context_type[8] */

  &oyra_api4_ui_image_load             /**< filter UI */
};

/* OY_IMAGE_LOAD_REGISTRATION ---------------------------------------------*/

/* OY_IMAGE_REGIONS_REGISTRATION ---------------------------------------------*/


oyOptions_s* oyraFilter_ImageRectanglesValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate OY_UNUSED,
                                       int                 statical OY_UNUSED,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = filter->type_ != oyOBJECT_FILTER_CORE_S;

  *result = error;

  return 0;
}


/** @brief   Split image processing into rectangles
 *
 *  implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/02/02
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 */
int      oyraFilterPlug_ImageRectanglesRun (
                                       oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int n = 0, i;
  int result = 0, l_result = 0;
  oyFilterSocket_s * socket;
  oyFilterNode_s * input_node = 0,
                 * node;
  oyImage_s * image;
  oyOption_s * o = 0;
  oyOptions_s * node_opts = 0;
  oyRectangle_s * r;

  int dirty = 0;

  socket = oyFilterPlug_GetSocket( requestor_plug );
  node = oyFilterSocket_GetNode( socket );

  image = (oyImage_s*)oyFilterSocket_GetData( socket );
  if(!image)
  {
    result = 1;
    goto oyraFilterPlug_ImageRectanglesRun_clean;
  }

  {
    oyRectangle_s * ticket_roi = oyPixelAccess_GetArrayROI( ticket );
    oyRectangle_s_  t_roi_pix_ = {oyOBJECT_RECTANGLE_S,0,0,0, 0,0,0,0};
    oyRectangle_s * t_roi_pix = (oyRectangle_s*)&t_roi_pix_;
    oyArray2d_s   * ticket_array = oyPixelAccess_GetArray( ticket );

    node_opts = oyFilterNode_GetOptions( node, 0 );
    if(!node_opts)
      dirty = 1;

    if(dirty)
    {
      result = dirty;
      goto oyraFilterPlug_ImageRectanglesRun_clean2;
    }

    oyPixelAccess_RoiToPixels( ticket, NULL, &t_roi_pix );

    n = oyOptions_CountType( node_opts,
                             "//" OY_TYPE_STD "/rectangles/rectangle",
                             oyOBJECT_RECTANGLE_S );

      if(oy_debug)
        oyra_msg( oy_debug?oyMSG_DBG:oyMSG_WARN, (oyStruct_s*)ticket,
                  OY_DBG_FORMAT_"n: %d %s", OY_DBG_ARGS_, n, oyPixelAccess_Show( ticket ) );

    /* rectangles stuff */
    for(i = 0; i < n; ++i)
    {
      oyPixelAccess_s * new_ticket = NULL,
                      * pticket = ticket;
      oyArray2d_s     * new_ticket_array = NULL;
      oyImage_s       * new_ticket_image = oyPixelAccess_GetOutputImage( ticket );
      oyRectangle_s_  rect_       = {oyOBJECT_RECTANGLE_S,0,0,0, 0,0,0,0},
                      newt_roi_pix_={oyOBJECT_RECTANGLE_S,0,0,0, 0,0,0,0};
      oyRectangle_s * rect = (oyRectangle_s*)&rect_,
                    * newt_roi_pix = (oyRectangle_s*)&newt_roi_pix_,
                    * newt_roi = NULL;
      int    nti_width = oyImage_GetWidth( new_ticket_image );
      double ntx = oyPixelAccess_GetStart( ticket, 0 ) * nti_width,
             nty = oyPixelAccess_GetStart( ticket, 1 ) * nti_width,
             otx = ntx,
             oty = nty;
      int changed = 0;

      /* select current rectangle */
      r = (oyRectangle_s*)oyOptions_GetType( node_opts, i,
                                       "//" OY_TYPE_STD "/rectangles/rectangle",
                                             oyOBJECT_RECTANGLE_S );
      oyRectangle_SetByRectangle( rect, r );
      oyRectangle_Release( &r );

      ntx += rect_.x;
      nty += rect_.y;

      newt_roi_pix_.x      = t_roi_pix_.x + rect_.x;
      newt_roi_pix_.y      = t_roi_pix_.y + rect_.y;
      newt_roi_pix_.width  = rect_.width;
      newt_roi_pix_.height = rect_.height;
      oyPixelAccess_PixelsToRoi( ticket, newt_roi_pix, &newt_roi );

      /* detect dimension changes */
      if(oyRectangle_CountPoints(  newt_roi_pix ) > 0 &&
         (ntx != otx ||
          nty != oty ||
          !oyRectangle_IsEqual( t_roi_pix, newt_roi_pix)
         )
        )
        changed = 1;

      if(oyRectangle_CountPoints( newt_roi ) > 0 && changed)
      {
        /* create a new ticket */
        new_ticket = pticket = oyPixelAccess_Copy( ticket, ticket->oy_ ),
        /* use the correct source pixels */
        oyPixelAccess_ChangeRectangle( new_ticket,
                                     ntx / (double)nti_width,
                                     nty / (double)nti_width,
                                     newt_roi );

        /* keep old ticket array */
        oyPixelAccess_SetArray( new_ticket, ticket_array, 0 );
        oyPixelAccess_SetArrayFocus( new_ticket, 0 );
      }

      if(oy_debug)
          oyra_msg( oy_debug?oyMSG_DBG:oyMSG_WARN, (oyStruct_s*)ticket,
                    OY_DBG_FORMAT_ "[%d] %s %s", OY_DBG_ARGS_, i, changed?"Created new_ticket":"Use old ticket",
                    oyPixelAccess_Show( pticket ) );

      if(oyRectangle_CountPoints( newt_roi ) > 0)
      {
        oyFilterPlug_s * plug;

        /* select node */
        input_node = oyFilterNode_GetPlugNode( node, i );

        /* start new call into branch */
        if(oy_debug)
        {
          oyRectangle_s_ nt_roi_ = {oyOBJECT_RECTANGLE_S,0,0,0, 0,0,0,0},
                        *nt_roi = &nt_roi_;
          oyPixelAccess_RoiToPixels( pticket, NULL, (oyRectangle_s**)&nt_roi );
          new_ticket_array = oyPixelAccess_GetArray( pticket );
          DBGs_PROG4_S( pticket, "%s[%d] pticket_array[%d](%s)",
                     "Run ticket through filter in node",
                     oyStruct_GetId( (oyStruct_s*)node ),
                     oyStruct_GetId( (oyStruct_s*)new_ticket_array ),
                     oyRectangle_Show( (oyRectangle_s*)nt_roi ) );
        }

        plug = oyFilterNode_GetPlug( node, i );
        l_result = oyFilterNode_Run( input_node, plug, pticket );
        if(l_result != 0 && (result <= 0 || l_result > 0))
          result = l_result;

        oyFilterPlug_Release( &plug );
      }

      oyPixelAccess_Release( &new_ticket );
      oyArray2d_Release( &new_ticket_array );
      oyImage_Release( &new_ticket_image );
      oyRectangle_Release( &newt_roi );

      oyOption_Release( &o );
    }

    oyraFilterPlug_ImageRectanglesRun_clean2:
    oyRectangle_Release( &ticket_roi );
    oyArray2d_Release( &ticket_array );
    oyFilterNode_Release( &input_node );
    oyOptions_Release( &node_opts );
  }

  oyraFilterPlug_ImageRectanglesRun_clean:
  oyImage_Release( &image );
  oyFilterNode_Release( &node );
  oyFilterSocket_Release( &socket );

  return result;
}


oyConnectorImaging_s_ oyra_imageRectangles_plug = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorPlugText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/splitter.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  1, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_color_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_color_count; */
  255, /* max_color_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap color channels (BGR)*/
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
oyConnectorImaging_s_ *oyra_imageRectangles_plugs[2] = {&oyra_imageRectangles_plug,0};

oyConnectorImaging_s_ oyra_imageRectangles_socket = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorSocketText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_color_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_color_count; */
  255, /* max_color_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap color channels (BGR)*/
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
oyConnectorImaging_s_ *oyra_imageRectangles_sockets[2] = {&oyra_imageRectangles_socket,0};

/** @brief registration string for \b oyra CMM */
#define OY_IMAGE_REGIONS_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "rectangles"
/** @brief   Rectangles Node
 *
 *  oyra oyCMMapi7_s implementation
 *
 *  The filter expects "//" OY_TYPE_STD "/rectangles/rectangle/[#]" options containing each a oyRectangle_s object and will create, fill and process a according rectangle. Each rectangle shall be in pixel coordinates. It describes a individual ROI inside the source image and the output image.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/02/24
 */
oyCMMapi7_s_   oyra_api7_image_rectangles = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api4_image_write_ppm, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMReset, /* oyCMMReset_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_REGIONS_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  oyraFilterPlug_ImageRectanglesRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  (oyConnector_s**) oyra_imageRectangles_plugs,   /* plugs */
  1,   /* plugs_n */
  UINT16_MAX,   /* plugs_last_add */
  (oyConnector_s**) oyra_imageRectangles_sockets,   /* sockets */
  1,   /* sockets_n */
  0,   /* sockets_last_add */
  NULL /* char ** properties */
};

const char * oyraApi4UiImageRectanglesGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
  if(strcmp(select,"name") == 0)
  {
    if(type == oyNAME_NICK)
      return "rectangles";
    else if(type == oyNAME_NAME)
      return _("Rectangles");
    else if(type == oyNAME_DESCRIPTION)
      return _("Rectangles Splitter Object");
  } else if(strcmp(select,"help") == 0)
  {
    if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Apply regions of interesst in form of simple rectangles.");
    else if(type == oyNAME_DESCRIPTION)
      return _("The filter expects \"//\" OY_TYPE_STD \"/rectangles/rectangle/[#]\" options containing each a oyRectangle_s object and will create, fill and process a according rectangle. Each rectangle shall be in pixel coordinates. It describes a individual ROI inside the source image and the output image.");
  }
  return 0;
}
const char * oyra_api4_ui_image_rectangles_texts[] = {"name", "help", 0};

/** @brief    Rectangles Node UI
 *
 *  oyra oyCMMapi4_s::ui implementation
 *
 *  The UI for filter image rectangles.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/09/09
 */
oyCMMui_s_ oyra_api4_ui_image_rectangles = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */

  oyraFilter_ImageRootValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyraWidgetEvent, /* oyWidgetEvent_f */

  "Graph/Rectangles", /* category */
  0,   /* const char * options */
  0,   /* oyCMMuiGet_f oyCMMuiGet */

  oyraApi4UiImageRectanglesGetText,  /* oyCMMGetText_f getText */
  oyra_api4_ui_image_rectangles_texts,  /* (const char**)texts */
  (oyCMMapiFilter_s*)&oyra_api4_image_rectangles /* oyCMMapiFilter_s*parent */
};

/** @brief    Rectangles Node (only UI)
 *
 *  oyra oyCMMapi4_s implementation
 *
 *  A filter collecting results from several rectangles
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/24 (Oyranos: 0.1.10)
 *  @date    2009/02/24
 */
oyCMMapi4_s_   oyra_api4_image_rectangles = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api7_image_rectangles, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMReset, /* oyCMMReset_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_REGIONS_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  (oyCMMFilterNode_ContextToMem_f)oyFilterNode_TextToInfo_, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem */
  {0}, /* char context_type[8] */

  &oyra_api4_ui_image_rectangles       /**< oyCMMui_s *ui */
};

/* OY_IMAGE_REGIONS_REGISTRATION ---------------------------------------------*/

/* OY_IMAGE_ROOT_REGISTRATION ------------------------------------------------*/


oyOptions_s* oyraFilter_ImageRootValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate OY_UNUSED,
                                       int                 statical OY_UNUSED,
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

/** @brief   Source in memory image obtaining
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


oyConnectorImaging_s_ oyra_imageRoot_plug = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorPlugText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  1, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_color_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_color_count; */
  255, /* max_color_count; */
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
oyConnectorImaging_s_ * oyra_imageRoot_plugs[2] = {&oyra_imageRoot_plug,0};

oyConnectorImaging_s_ oyra_imageRoot_socket = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorSocketText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_color_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_color_count; */
  255, /* max_color_count; */
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
oyConnectorImaging_s_ * oyra_imageRoot_sockets[2] = {&oyra_imageRoot_socket,0};


/** @brief registration string for \b oyra CMM */
#define OY_IMAGE_ROOT_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "root"
/** @brief    Source in memory Image Node
 *
 *  oyra oyCMMapi7_s implementation
 *
 *  a filter providing a source image
 *
 *  The __root__ image filter can hold pixel data for processing in a graph.
 *  It copies the socket data image to the __ticket__'s output image
 *  according to the __ticket__ region and offset (alias start), during
 *  running the DAG with oyConversion_RunPixels().
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
oyCMMapi7_s_   oyra_api7_image_root = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api4_image_output, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMReset, /* oyCMMReset_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_ROOT_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  oyraFilterPlug_ImageRootRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  (oyConnector_s**) oyra_imageRoot_plugs,   /* plugs */
  1,   /* plugs_n */
  0,   /* plugs_last_add */
  (oyConnector_s**) oyra_imageRoot_sockets,   /* sockets */
  1,   /* sockets_n */
  0,   /* sockets_last_add */
  NULL /* char ** properties */
};

const char * oyraApi4UiImageRootGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
  if(strcmp(select,"name") == 0)
  {
    if(type == oyNAME_NICK)
      return "image_root";
    else if(type == oyNAME_NAME)
      return _("Root Image");
    else if(type == oyNAME_DESCRIPTION)
      return _("Root Image Filter Object");
  } else if(strcmp(select,"help") == 0)
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

/** @brief    Source in memory Image Node UI
 *
 *  oyra oyCMMapi4_s::ui implementation
 *
 *  The UI for filter image root.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/09/09
 */
oyCMMui_s_ oyra_api4_ui_image_root = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */

  oyraFilter_ImageRootValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyraWidgetEvent, /* oyWidgetEvent_f */

  "Image/Simple Image[in]", /* category */
  0,   /* const char * options */
  0,   /* oyCMMuiGet_f oyCMMuiGet */

  oyraApi4UiImageRootGetText, /* oyCMMGetText_f getText */
  oyra_api4_ui_image_root_texts, /* (const char**)texts */
  (oyCMMapiFilter_s*)&oyra_api4_image_root /* oyCMMapiFilter_s*parent */
};

/** @brief    Source in memory Image Node (only UI)
 *
 *  oyra oyCMMapi4_s implementation
 *
 *  a filter providing a source image
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/08 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyCMMapi4_s_   oyra_api4_image_root = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api7_image_root, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMReset, /* oyCMMReset_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_ROOT_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  (oyCMMFilterNode_ContextToMem_f)oyFilterNode_TextToInfo_, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem */
  {0}, /* char context_type[8] */

  &oyra_api4_ui_image_root             /**< oyCMMui_s *ui */
};


/* OY_IMAGE_ROOT_REGISTRATION ------------------------------------------------*/

/* OY_IMAGE_OUTPUT_REGISTRATION ----------------------------------------------*/

oyConnectorImaging_s_ oyra_imageOutput_connector = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorPlugText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  1, /* is_plug == oyFilterPlug_s */
  oyra_image_data_types, /* data_types */
  6, /* data_types_n; elements in data_types array */
  -1, /* max_color_offset */
  1, /* min_channels_count; */
  255, /* max_channels_count; */
  1, /* min_color_count; */
  255, /* max_color_count; */
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
oyConnectorImaging_s_* oyra_imageOutput_connectors[2] = {&oyra_imageOutput_connector,0};


/** @brief   Start Processing
 *
 *  This node is itself pretty passive.
 *  Implements oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/19 (Oyranos: 0.1.8)
 *  @date    2008/10/03
 */
int      oyraFilterPlug_ImageOutputRun(oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  oyFilterSocket_s * socket = oyFilterPlug_GetSocket( requestor_plug );
  oyFilterNode_s * node = 0;
  int result = 0;

  node = oyFilterSocket_GetNode( socket );
  oyFilterSocket_Release( &socket );

  /* to reuse the requestor_plug is a exception for the starting request */
  if(node)
  {
    DBGs_PROG2_S( ticket, "%s[%d]", "Call next filter in node",
                 oyStruct_GetId( (oyStruct_s*)node ) );
    result = oyFilterNode_Run( node, requestor_plug, ticket );
    oyFilterNode_Release( &node );
  } else
    result = 1;

  return result;
}

/** @brief registration string for \b oyra CMM */
#define OY_IMAGE_OUTPUT_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "output"
/** @brief    Output in memory Image Node
 *
 *  oyra oyCMMapi7_s implementation
 *
 *  a filter providing a target image as output
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
oyCMMapi7_s_   oyra_api7_image_output = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api4_image_rectangles, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMReset, /* oyCMMReset_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_OUTPUT_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  oyraFilterPlug_ImageOutputRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  (oyConnector_s**) oyra_imageOutput_connectors,   /* plugs */
  1,   /* plugs_n */
  0,   /* plugs_last_add */
  0,   /* sockets */
  0,   /* sockets_n */
  0,   /* sockets_last_add */
  NULL /* char ** properties */
};

const char * oyraApi4UiImageOutputGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
  if(strcmp(select,"name") == 0)
  {
    if(type == oyNAME_NICK)
      return "image_out";
    else if(type == oyNAME_NAME)
      return _("Image[out]");
    else if(type == oyNAME_DESCRIPTION)
      return _("Output Image Filter Object");
  } else if(strcmp(select,"help") == 0)
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

/** @brief    Image Output UI
 *
 *  oyra oyCMMapi4_s::ui implementation
 *
 *  The UI for filter image output.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/09/09
 */
oyCMMui_s_ oyra_api4_ui_image_output = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */

  oyraFilter_ImageRootValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyraWidgetEvent, /* oyWidgetEvent_f */

  "Image/Simple Image[out]", /* category */
  0,   /* const char * options */
  0,   /* oyCMMuiGet_f oyCMMuiGet */

  oyraApi4UiImageOutputGetText,  /* oyCMMGetText_f getText */
  oyra_api4_ui_image_output_texts,  /* (const char**)texts */
  (oyCMMapiFilter_s*)&oyra_api4_image_output /* oyCMMapiFilter_s*parent */
};

/** @brief    Image Output (only UI)
 *
 *  A filter providing a target image.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/19 (Oyranos: 0.1.8)
 *  @date    2008/07/19
 */
oyCMMapi4_s_   oyra_api4_image_output = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api7_image_output, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMReset, /* oyCMMReset_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_IMAGE_OUTPUT_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  (oyCMMFilterNode_ContextToMem_f)oyFilterNode_TextToInfo_, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem */
  {0}, /* char context_type[8] */

  &oyra_api4_ui_image_output           /**< oyCMMui_s *ui */
};

/* OY_IMAGE_OUTPUT_REGISTRATION ----------------------------------------------*/
/* ---------------------------------------------------------------------------*/

/**  @} *//* oyra_graph */
/**  @} *//* graph_modules */


