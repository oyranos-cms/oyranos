/** @file oyranos_cmm.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2008-2019 (C) Kai-Uwe Behrmann
 *
 *  @brief    CMM core APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2009/04/28
 *
 *  basic and / or common functions for modules
 */


#include "oyranos_cmm.h"
#include "oyranos_generic.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_object_internal.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"
#include "config.icc_profile.printer.CUPS.json.h"
#include "config.icc_profile.raw-image.oyRE.json.h"
#include "config.icc_profile.scanner.SANE.json.h"


#include "oyConnectorImaging_s.h"
#include "oyRectangle_s_.h"
#include "oyCMMapi8_s_.h"

#if !defined(_WIN32)
#include <dlfcn.h>
#endif
#include <math.h>
#include <stdarg.h> /* va_list */

#define OY_ERR if(l_error != 0) error = l_error;

#ifdef DEBUG_
#define DEBUG_OBJECT 1
#endif

/** @addtogroup module_api
 *  @{ */

/** @brief   implement oyCMMFilter_GetNext_f()
 *
 *  Copy the socket data image to the __ticket__'s output image according to the
 *  __ticket__ region and offset (alias start).
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/09/09
 *  @since   2008/07/10 (Oyranos: 0.1.8)
 */
int      oyFilterPlug_ImageRootRun   ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int result = 0, error = 0;
  oyFilterSocket_s * socket = oyFilterPlug_GetSocket( requestor_plug );
  oyImage_s * image = (oyImage_s*)oyFilterSocket_GetData( socket ),
            * output_image;
  int output_image_width;
  oyRectangle_s * output_array_roi = NULL;

  if(oy_debug)
    oyMessageFunc_p( oyMSG_DBG, (oyStruct_s*)ticket, "%s[%d] %s", "Work on remote socket image",
                oyStruct_GetId( (oyStruct_s*)image ),
                oyPixelAccess_Show(ticket) );

  oyFilterSocket_Release( &socket );

  /* Do not work on non existent data. */
  output_image = oyPixelAccess_GetOutputImage( ticket );
  if(!image || !output_image)
    goto oyFilterPlug_ImageRootRun_clean;

  output_array_roi = oyPixelAccess_GetArrayROI( ticket );
  output_image_width = oyImage_GetWidth(output_image);


  /* Set a unknown output image dimension to something appropriate. */
  if(!oyImage_GetWidth(output_image) && !oyImage_GetHeight(output_image))
  {
    DBGs_PROG7_S( ticket, "%s[%d] %s %.04gx%.04g %.04gx%.04g",
                 "Set dimensions on ticket->output_image",
                 oyStruct_GetId( (oyStruct_s*)output_image ),
                 oyRectangle_Show( output_array_roi ),
                 oyImage_GetWidth(output_image),oyImage_GetHeight(output_image),
                 oyImage_GetWidth(image), oyImage_GetHeight(image) );
    oyImage_SetCritical( output_image,
                         oyImage_GetPixelLayout(image, oyLAYOUT), 0, 0,
                         oyImage_GetWidth(image), oyImage_GetHeight(image) );
  }

  {
    char * t = 0;
    int channels = oyImage_GetPixelLayout( image, oyCHANS );
    oyArray2d_s * array = oyPixelAccess_GetArray( ticket );
    int array_is_focussed = 0;
    oyRectangle_s_ image_roi_pix = {oyOBJECT_RECTANGLE_S,0,0,0, 0,0,0,0},
                   array_roi_pix = {oyOBJECT_RECTANGLE_S,0,0,0, 0,0,0,0};
    oyRectangle_s * arp = (oyRectangle_s*)&array_roi_pix,
                  * image_roi = oyRectangle_New(0);

    if(array)
    {
      error = oyPixelAccess_SetArrayFocus( ticket, 0 );
      if(error > 0)
        oyMessageFunc_p( oyMSG_WARN, (oyStruct_s*)ticket,
                         OY_DBG_FORMAT_ "set focus: %s", OY_DBG_ARGS_,
                         oyPixelAccess_Show(ticket) );
      array_is_focussed = oyPixelAccess_ArrayIsFocussed( ticket );
    }

    oyPixelAccess_RoiToPixels( ticket, 0, &arp );

    image_roi_pix.x = oyPixelAccess_GetStart(ticket,0) * output_image_width;
    image_roi_pix.y = oyPixelAccess_GetStart(ticket,1) * output_image_width;
    image_roi_pix.width = array_roi_pix.width;
    image_roi_pix.height = array_roi_pix.height;
    oyImage_PixelsToSamples( image, (oyRectangle_s*)&image_roi_pix, image_roi );
    oyImage_SamplesToRoi( image, image_roi, &image_roi );

    {
      int val = OY_ROUND( image_roi_pix.x );
      double diff = (image_roi_pix.x - val) * channels;
      if(diff > 0.5)
      {
        error = -1;
        oyMessageFunc_p( oyMSG_WARN, (oyStruct_s*)ticket,
                         OY_DBG_FORMAT_ "sub pixel access is pretty uncommon: %s x:%d/%g diff:%g %dc", OY_DBG_ARGS_,
                         oyPixelAccess_Show(ticket), val, image_roi_pix.x, diff, channels );
      }
    }

    if(oy_debug || error > 0)
    {
      oyRectangle_s * output_array_roi_pix = NULL;

      STRING_ADD( t, oyRectangle_Show( (oyRectangle_s*)&image_roi_pix ) );
      oyPixelAccess_RoiToPixels( ticket, output_array_roi, &output_array_roi_pix ); 
      oyMessageFunc_p( error?oyMSG_WARN:oyMSG_DBG, (oyStruct_s*)ticket,
                       OY_DBG_FORMAT_ "Fill ticket->array[%d] from %s[%d] "
                       "image_roi_pix: %s output_array_roi:%s %s err:%d",
                       OY_DBG_ARGS_,
                       oyStruct_GetId( (oyStruct_s*)array ),
                       _("Image"), oyStruct_GetId( (oyStruct_s*)image ), t,
                       oyRectangle_Show( output_array_roi_pix ),
                       oyArray2d_Show(array, channels), error );
      oyRectangle_Release( &output_array_roi_pix );
      if(t) oyFree_m_( t );
    }

    result = oyImage_FillArray( image, image_roi, 1,
                               /* The array should be already focussed. Then
                                * the array_roi.
                                * Without a array, use array_roi to generate a
                                * sufficient big array to fit array_roi. */
                               &array, array_is_focussed ? NULL:output_array_roi, 0 );
    if(oy_debug || error > 0 || result > 0)
      oyMessageFunc_p( error?oyMSG_WARN:oyMSG_DBG, (oyStruct_s*)ticket,
                       OY_DBG_FORMAT_"%carray: %s", OY_DBG_ARGS_,
                       oyPixelAccess_ArrayIsFocussed(ticket)?' ':'~',
                       "filled ticket->array",
                       oyArray2d_Show( array, channels ) );

    oyPixelAccess_SetArray( ticket, array, array_is_focussed );

    oyArray2d_Release( &array );
    if(error > 0)
      result = error;
    oyRectangle_Release( &image_roi );
  }

  oyFilterPlug_ImageRootRun_clean:
  oyImage_Release( &output_image );
  oyImage_Release( &image );
  oyRectangle_Release( &output_array_roi );

  return result;
}

/** @brief   load Rank Map from disk
 *
 *  The rank map is selected by __rank_file_pattern__ and searched with
 *  oyRankMapList(). After reading from disk,
 *  oyRankMapFromJSON() creates a oyRankMap array and assigns that to the
 *  device config __filter__ structure.
 *
 *  @param[in]     filter              the oyCMMapi8_s device config filter
 *  @param[in]     rank_file_pattern   the ID or class name
 *  @param[in]     rank_optional       no serious warnings as the filter can add a rankmap by itself
 *  @return                            error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/02/25
 *  @since   2015/08/05 (Oyranos: 0.9.6)
 */
int oyDeviceCMMInit                  ( oyStruct_s        * filter,
                                       const char        * rank_file_pattern,
                                       int                 rank_optional )
{
  int error = !(filter && filter->type_ == oyOBJECT_CMM_API8_S) || !rank_file_pattern;
  char ** rank_name = NULL;
  const char * rfilter = rank_file_pattern;
  oyCMMapi8_s_ * s = (oyCMMapi8_s_*) filter;
  const char * rank_json = NULL;

  if(oy_debug)
    oyMessageFunc_p( oyMSG_DBG, filter, "%s() %s", __func__, rank_file_pattern );

  if(!error)
  {
    error = oyRankMapList( rfilter, NULL, &rank_name, oyAllocateFunc_ );

    if(error > 0 || !rank_name || !rank_name[0])
    {
      if(strstr(rank_file_pattern, "CUPS"))
        rank_json = (const char *) config_icc_profile_printer_CUPS_json;
      else
      if(strstr(rank_file_pattern, "oyRE"))
        rank_json = (const char *) config_icc_profile_raw_image_oyRE_json;
      else
      if(strstr(rank_file_pattern, "SANE"))
        rank_json = (const char *) config_icc_profile_scanner_SANE_json;
    }

    if(!rank_json && (error > 0 || !rank_name || !rank_name[0]))
    {
      if(rank_optional == 0)
        WARNc2_S("Problems loading rank map: %s %d", rfilter, error);

    } else
    {
      oyRankMap * rank_map = NULL;
      char * json_text = NULL;
      size_t json_size = 0;

      if(!rank_json && rank_name && rank_name[0])
        json_text = oyReadFileToMem_( rank_name[0], &json_size, oyAllocateFunc_ );
      if(!rank_json && (!json_text || !json_text[0]))
        oyMessageFunc_p( oyMSG_WARN, filter, "%s() %s: %s", __func__,
                         _("File not loaded!"), rank_name[0] );
      else if(json_text)
        rank_json = json_text;

      error = oyRankMapFromJSON ( rank_json, NULL, &rank_map, oyAllocateFunc_ );

      if(!rank_map || error || !rank_map[0].key)
        oyMessageFunc_p( oyMSG_WARN, filter, "%s() %s: %s  %d", __func__,
                         _("Creation of rank_map failed from"), rank_name?rank_name[0]:rank_json, error );
      else
        s->rank_map = rank_map;

      if(json_text) oyFree_m_( json_text );
    }

    if(rank_name)
    {
      int count = 0;
      while( rank_name[count] ) ++count;
      oyStringListRelease_( &rank_name, count, oyDeAllocateFunc_ );
    }
  }

  return error;
}

/** @brief   reset Rank Map from disk
 *
 *  @param[in]     filter              the oyCMMapi8_s device config filter
 *  @return                            error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/09/03
 *  @since   2019/09/03 (Oyranos: 0.9.7)
 */
int oyDeviceCMMReset                 ( oyStruct_s        * filter )
{
  int error = !(filter && filter->type_ == oyOBJECT_CMM_API8_S);
  oyCMMapi8_s_ * s = (oyCMMapi8_s_*) filter;

  if(oy_debug)
    oyMessageFunc_p( oyMSG_DBG, filter, "%s() %s", __func__, s&&s->registration?s->registration:"----" );

  if(!error)
    oyRankMapRelease( &s->rank_map, oyDeAllocateFunc_ );

  return error;
}

/**
 *  This function implements oyCMMinfoGetText_f for modules shipped with
 *  Oyranos.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2010/09/06
 */
const char * oyCMMgetText             ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return _("NoNo");
    else if(type == oyNAME_NAME)
      return _("Oyranos modules");
    else
      return _("Oyranos supplied modules");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return "Kai-Uwe";
    else if(type == oyNAME_NAME)
      return "Kai-Uwe Behrmann";
    else
      return _("Oyranos project; www: http://www.oyranos.org; support/email: ku.b@gmx.de; sources: http://www.oyranos.org/downloads/");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return "newBSD";
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 2005-2014 Kai-Uwe Behrmann; newBSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/BSD-3-Clause");
  }
  return 0;
}
const char *oyCMM_texts[4] = {"name","copyright","manufacturer",0};

/**
 *  This function implements oyCMMinfoGetText_f for image connector plugs.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
const char * oyCMMgetImageConnectorPlugText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
  if(strcmp(select, "name")==0)
  {
    if(type == oyNAME_NICK)
      return _("Img");
    else if(type == oyNAME_NAME)
      return _("Image");
    else if(type == oyNAME_DESCRIPTION)
      return _("Image Plug");
  }
  return NULL;
}
const char * oy_image_connector_texts[] = {"name", 0};
/**
 *  This function implements oyCMMinfoGetText_f for image connector sockets.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
const char * oyCMMgetImageConnectorSocketText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
  if(strcmp(select, "name")==0)
  {
    if(type == oyNAME_NICK)
      return _("Img");
    else if(type == oyNAME_NAME)
      return _("Image");
    else if(type == oyNAME_DESCRIPTION)
      return _("Image Socket");
  }
  return NULL;
}

/**  @} *//* module_api */
