/** @file oyranos_cmm_oyX1.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2007-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyranos X11 backend for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2007/12/12
 */

#include "oyranos_cmm.h"
#include "oyranos_debug.h"
#include "oyranos_i18n.h"
#include "oyranos_monitor.h"
#include "oyranos_texts.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


/* --- internal definitions --- */

#define CMM_NICK "oyX1"
#define CMM_BASE_REG OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "config.monitor." CMM_NICK

int oyX1CMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... );
oyMessage_f message = oyX1CMMWarnFunc;

extern oyCMMapi8_s oyX1_api8;

/* --- implementations --- */

int                oyX1CMMInit       ( )
{
  int error = 0;
  return error;
}


/*
oyPointer          oyCMMallocateFunc   ( size_t            size )
{
  oyPointer p = 0;
  if(size)
    p = malloc(size);
  return p;
}

void               oyCMMdeallocateFunc ( oyPointer         mem )
{
  if(mem)
    free(mem);
}*/

/** @func    oyX1CMMWarnFunc
 *  @brief   API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/12
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 */
int oyX1CMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... )
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

/** @func  oyX1CMMMessageFuncSet
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/12
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 */
int            oyX1CMMMessageFuncSet ( oyMessage_f         message_func )
{
  message = message_func;
  return 0;
}

/** @func  oyX1CMMCanHandle
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/12
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 */
int            oyX1CMMCanHandle      ( oyCMMQUERY_e        type,
                                       uint32_t            value ) {return 0;}

#define OPTIONS_ADD(opts, name) if(!error && name) \
        error = oyOptions_SetFromText( opts, \
                                       CMM_BASE_REG OY_SLASH #name, \
                                       name, OY_CREATE_NEW );

/** Function oyX1Configs_FromPattern
 *  @brief   oyX1 oyCMMapi8_s Xorg monitors
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/01/19
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 */
oyConfigs_s *  oyX1Configs_FromPattern (
                                       const char        * registration,
                                       oyOptions_s       * options )
{
  oyConfigs_s * configs = 0;
  oyConfig_s * config = 0;
  oyOption_s * o = 0;
  char * text = 0;
  char ** texts = 0;
  int texts_n = 0, i, n,
      error = 0;
  const char * value1 = 0,
             * value2 = 0,
             * value3 = 0;
  int rank = oyFilterRegistrationMatch( oyX1_api8.registration, registration,
                                        oyOBJECT_CMM_API8_S );
  oyAlloc_f allocateFunc = malloc;
  oyDeAlloc_f deallocateFunc = free;

  if(!options || !oyOptions_Count( options ))
  {
    /** oyMSG_WARN should make shure our message is visible. */
    message( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "\n %s",
             OY_DBG_ARGS_,
      "The following help text informs about the communication protocol.");
    message( oyMSG_WARN, (oyStruct_s*)options, "%s()\n %s", __func__,
      "The presence of option \"list\" will provide a list of available\n"
      " devices. Use the returned values as option \"display_name\" to select a"
      " device.");
    message( oyMSG_WARN, (oyStruct_s*)options, "%s()\n %s", __func__,
      "The presence of option \"properties\" will provide a devices \n"
      " properties. Requires one device identifier returned with the \n"
      " \"list_all\" option. The properties may cover following entries, wich\n"
      " can be used as well as filters:\n"
      " - \"manufacturer\"\n"
      " - \"model\"\n"
      " - \"serial\"\n"
      " - \"host\"\n"
      " - \"system_port\"\n"
      " - \"display_geometry\" (specific) x,y,widthxheight ,e.g."
      " \"0,0,1024x786\"\n"
      " If the option key word \"edid\" (specific) is present the EDID\n"
      " information will be passed inside a oyBlob_s struct.\n"
      " \n"
      " One option \"device_name\" (specific) will select the according X\n"
      " display. If not the backend will try to get this information from \n"
      " your \"DISPLAY\" environment variable or uses what the system\n"
      " provides. The option is identical with the entries returned from a\n"
      " \"list\" request."
       );
    return 0;
  }

  if(rank)
  {
    configs = oyConfigs_New(0);

    value1 = oyOptions_FindString( options, "device_name", 0 );
    //message(oyMSG_WARN, (oyStruct_s*)options, "list: %s", value2);

    value2 = oyOptions_FindString( options, "list", 0 );
    if(value2)
    {
      texts_n = oyGetAllScreenNames( value1, &texts, allocateFunc );

      for( i = 0; i < texts_n; ++i )
      {
        config = oyConfig_New(0);
        error = !config;

        if(!error)
        error = oyOptions_SetFromText( config->options,
                                       CMM_BASE_REG OY_SLASH "device_name",
                                       texts[i], OY_CREATE_NEW );

        oyConfigs_MoveIn( configs, &config, -1 );
      }
    }

    value2 = oyOptions_FindString( options, "properties", 0 );
    value3 = oyOptions_FindString( options, "edid", 0 );
    if(value2)
    {
      char * manufacturer=0, *model=0, *serial=0, *host=0, *display_geometry=0,
           * system_port=0;
      oyBlob_s * edid = 0;

      if(!value1)
      {
        message(oyMSG_WARN, (oyStruct_s*)options, "The \"device_name\" argument is missed to select a appropriate device for the \"properties\" call.");
      }

      error = oyGetMonitorInfo_lib( value1,
               &manufacturer, &model, &serial, &display_geometry, &system_port,
                                    value3 ? &edid : 0, allocateFunc,
                                    (oyStruct_s*)options );
 
      {
        config = oyConfig_New(0);
        error = !config;
        if(!error && value1)
        error = oyOptions_SetFromText( config->options,
                                       CMM_BASE_REG OY_SLASH "device_name",
                                       value1, OY_CREATE_NEW );

        OPTIONS_ADD( config->options, manufacturer )
        OPTIONS_ADD( config->options, model )
        OPTIONS_ADD( config->options, serial )
        OPTIONS_ADD( config->options, display_geometry )
        OPTIONS_ADD( config->options, system_port )
        OPTIONS_ADD( config->options, host )

        oyConfigs_MoveIn( configs, &config, -1 );
      }
      if(error)
        message( oyMSG_WARN, (oyStruct_s*)options, 
                 OY_DBG_FORMAT_ "Could not complete \"properties\" call.\n"
               " oyGetMonitorInfo_lib returned with error; device_name: \"%s\"",
                 OY_DBG_ARGS_, oyNoEmptyString_m_( value1) );
    }
  }

  return configs;
}


/** @instance oyX1_api8
 *  @brief    oyX1 oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/01/19
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 */
oyCMMapi8_s oyX1_api8 = {
  oyOBJECT_CMM_API8_S,
  0,0,0,
  0,                         /**< next */

  oyX1CMMInit,               /**< oyCMMInit_f      oyCMMInit */
  oyX1CMMMessageFuncSet,     /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */
  oyX1CMMCanHandle,          /**< oyCMMCanHandle_f oyCMMCanHandle */

  CMM_BASE_REG,              /**< registration */
  {0,1,0},                   /**< int32_t version[3] */
  0,                         /**< char * id_ */

  0,                         /**< oyCMMapi5_s * api5_ */
  oyX1Configs_FromPattern,   /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
  0/*oyX1Config_Match*/      /**< oyConfig_Check_f oyConfig_Match */
};


/** @instance oyX1_api2
 *  @brief    oyX1 oyCMMapi2_s implementations
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/12
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 */
oyCMMapi2_s oyX1_api2 = {

  oyOBJECT_CMM_API2_S,
  0,0,0,
  (oyCMMapi_s*) & oyX1_api8,

  oyX1CMMInit,
  oyX1CMMMessageFuncSet,
  oyX1CMMCanHandle,

  oyGetMonitorInfo_lib,
  oyGetScreenFromPosition_lib,

  oyGetDisplayNameFromPosition_lib,
  oyGetMonitorProfile_lib,
  oyGetMonitorProfileNameFromDB_lib,

  oySetMonitorProfile_lib,
  oyActivateMonitorProfiles_lib
};



/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2008/12/30
 */
const char * oyX1GetText             ( const char        * select,
                                       oyNAME_e            type )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return _(CMM_NICK);
    else if(type == oyNAME_NAME)
      return _("Oyranos X11");
    else
      return _("The window support backend of Oyranos.");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return _("Kai-Uwe");
    else if(type == oyNAME_NAME)
      return _("Kai-Uwe Behrmann");
    else
      return _("Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/wiki/index.php?title=Oyranos/Download");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return _("newBSD");
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 2005-2008 Kai-Uwe Behrmann; newBSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/bsd-license.php");
  }
  return 0;
}

/** @instance oyX1_cmm_module
 *  @brief    oyX1 module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 *  @date    2008/12/30
 */
oyCMMInfo_s oyX1_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.2",
  oyX1GetText, /* oyCMMInfoGetText_f */
  OYRANOS_VERSION,

  (oyCMMapi_s*) & oyX1_api8,

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"},
};

