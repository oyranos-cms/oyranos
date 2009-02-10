/** @file oyranos_cmm_dDev.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyranos dummy device backend for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2009/02/09
 *
 *
 *
 *  The library name must cover the four char nick + "_cmm_module", which is
 *  "dDev_cmm_module" for this example module. E.g.:
 *  $(CC) $(CFLAGS) -shared oyranos_cmm_dDev.c -o liboyranos_dDev_cmm_module.so
 */

#include "oyranos_cmm.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


/* --- internal definitions --- */

#define CMM_NICK "dDev"
#define CMM_BASE_REG OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "config.dummy." CMM_NICK

#define dDev_DBG_FORMAT_ "%s:%d %s()"
#define dDev_DBG_ARGS_ __FILE__,__LINE__,__func__
#define _(x) x
#define STRING_ADD(a,b) sprintf( &a[strlen(a)], b )

oyMessage_f message = 0;

extern oyCMMapi8_s dDev_api8;
oyRankPad dDev_rank_map[];

/* --- implementations --- */

int                dDevCMMInit       ( )
{
  int error = 0;
  return error;
}



oyPointer          dDevCMMallocateFunc   ( size_t            size )
{
  oyPointer p = 0;
  if(size)
    p = malloc(size);
  return p;
}

void               dDevCMMdeallocateFunc ( oyPointer         mem )
{
  if(mem)
    free(mem);
}

/** @func  dDevCMMMessageFuncSet
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int            dDevCMMMessageFuncSet ( oyMessage_f         message_func )
{
  message = message_func;
  return 0;
}

/** @func  dDevCMMCanHandle
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 *  @date    2009/02/09
 */
int            dDevCMMCanHandle      ( oyCMMQUERY_e        type,
                                       uint32_t            value ) {return 0;}

#define OPTIONS_ADD(opts, name) if(!error && name) \
        error = oyOptions_SetFromText( opts, \
                                       CMM_BASE_REG OY_SLASH #name, \
                                       name, OY_CREATE_NEW );

void     dDevConfigsFromPatternUsage( oyStruct_s        * options )
{
    /** oyMSG_WARN should make shure our message is visible. */
    message( oyMSG_WARN, options, dDev_DBG_FORMAT_ "\n %s",
             dDev_DBG_ARGS_,
      "The following help text informs about the communication protocol.");
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"list\" will provide a list of available\n"
      " instruments. The actual instrument name can be found in option\n"
      " \"instrument_name\". The call is as lightwight as possible.\n"
      " The option \"oyNAME_NAME\" returns a short string containting\n"
      " informations and if available, the profile name or size.\n"
      " The bidirectional option \"icc_profile\" will add a oyProfile_s.\n"
      " The bidirectional option \"oyNAME_DESCRIPTION\" adds a string\n" 
      " containting all properties. The text is separated by newline. The\n"
      " first line contains the actual key word, the even one the belonging\n" 
      " string.\n" 
      " The bidirectional \"oyNAME_DESCRIPTION\" option turns the \"list\" \n"
      " call into a expensive one.\n"
      " The option \"instrument_name\" may be added as a filter.\n"
      " \"list\" is normally a cheap call, see oyNAME_DESCRIPTION above."
      " Informations are stored in the returned oyConfig_s::data member."
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"properties\" will provide the instruments \n"
      " properties. Requires a instrument_name identifier returned with the \n"
      " \"list\" option. The properties may cover following entries:\n"
      " - \"manufacturer\"\n"
      " - \"model\"\n"
      " - \"serial\"\n"
      " - \"host\"\n"
      " - \"system_port\"\n"
      " - and more as appropriate for the device ...\n"
      " \n"
      " One option \"instrument_name\" will select the according device.\n"
      " If not the backend might be able to get this information from \n"
      " elsewhere, but this is optional.\n"
      " The \"instrument_name\" should be identical with the one\n"
      " returned from a \"list\" request.\n"
      " The \"properties\" call might be a expensive one. Informations are\n" 
      " stored in the returned oyConfig_s::backend_core member."
       );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"setup\" will setup the instrument from a profile.\n"
      " The option \"instrument_name\" must be present, see \"list\" above.\n"
      " The option \"profile_name\" must be present, containing a ICC profile\n"
      " file name."
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"unset\" will invalidate a profile of a instrument.\n"
      " The option \"instrument_name\" must be present, see \"list\" above.\n"
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"get\" will provide a oyProfile_s of the\n"
      " instrument in a \"icc_profile\" option.\n"
      " The option \"instrument_name\" must be present, see \"list\" above.\n"
      );

  return;
}

int          dDevInstrumentFromName_ ( const char        * instrument_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** instrument,
                                       oyAlloc_f           allocateFunc )
{
  const char * value3 = 0;
  oyOption_s * o = 0;
  int error = !instrument;

    value3 = oyOptions_FindString( options, "data_blob", 0 );

    if(!error)
    {
      char * manufacturer=0, *model=0, *serial=0, *host=0, *system_port=0;
      oyBlob_s * data_blob = 0;

      if(!instrument_name)
      {
        message(oyMSG_WARN, (oyStruct_s*)options, dDev_DBG_FORMAT_
                "The \"instrument_name\" argument is\n"
                " missed to select a appropriate instrument for the"
                " \"properties\" call.", dDev_DBG_ARGS_ );
        error = 1;
      }

      /* now get the data from somewhere*/
      manufacturer = "People";
      model = "people-one";
      serial = "12";
      system_port = "usb-2";
      host = "localhost";

      if(error != 0)
        message( oyMSG_WARN, (oyStruct_s*)options, 
                 dDev_DBG_FORMAT_ "Could not complete \"properties\" call.\n"
                 " oyGetMonitorInfo_lib returned with %s; instrument_name:"
                 " \"%s\"", dDev_DBG_ARGS_, error > 0 ? "error(s)" : "issue(s)",
                 instrument_name ? instrument_name : "" );

      if(error <= 0)
      {
        if(!*instrument)
          *instrument = oyConfig_New( CMM_BASE_REG, 0 );
        error = !*instrument;
        if(!error && instrument_name)
        error = oyOptions_SetFromText( (*instrument)->backend_core,
                                       CMM_BASE_REG OY_SLASH "instrument_name",
                                       instrument_name, OY_CREATE_NEW );

        OPTIONS_ADD( (*instrument)->backend_core, manufacturer )
        OPTIONS_ADD( (*instrument)->backend_core, model )
        OPTIONS_ADD( (*instrument)->backend_core, serial )
        OPTIONS_ADD( (*instrument)->backend_core, system_port )
        OPTIONS_ADD( (*instrument)->backend_core, host )
        if(!error && data_blob)
        {
          o = oyOption_New( CMM_BASE_REG OY_SLASH "data_blob", 0 );
          error = !o;
          if(!error)
          error = oyOption_SetFromData( o, data_blob->ptr, data_blob->size );
          if(!error)
            oyOptions_MoveIn( (*instrument)->data, &o, -1 );
          oyBlob_Release( &data_blob );
        }
      }
    }

  return error;
}


/** Function dDevConfigs_FromPattern
 *  @brief   dDev oyCMMapi8_s dummy instruments
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int            dDevConfigs_FromPattern (
                                       const char        * registration,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** s )
{
  oyConfigs_s * instruments = 0;
  oyConfig_s * instrument = 0;
  oyOption_s * o = 0;
  oyProfile_s * p = 0;
  char ** texts = 0;
  char * text = 0;
  int texts_n = 0, i, n,
      error = !s;
  const char * value1 = 0,
             * value2 = 0,
             * value3 = 0,
             * value4 = 0;
  int rank = oyFilterRegistrationMatch( dDev_api8.registration, registration,
                                        oyOBJECT_CMM_API8_S );
  oyAlloc_f allocateFunc = malloc;
  static char * num = 0;
  const char * tmp = 0;

  if(!num)
    num = malloc( 80 );

  if(!options || !oyOptions_Count( options ))
  {
    /** oyMSG_WARN should make shure our message is visible. */
    dDevConfigsFromPatternUsage( (oyStruct_s*)options );
    return 0;
  }

  if(rank && error <= 0)
  {
    instruments = oyConfigs_New(0);

    /* "list" call section */
    value1 = oyOptions_FindString( options, "instrument_name", 0 );
    value2 = oyOptions_FindString( options, "list", 0 );
    if(value2)
    {
      texts_n = 0; /* getDevices( value1, &texts, allocateFunc ); */

      for( i = 0; i < texts_n; ++i )
      {
        /* filter */
        if(value1 && strcmp(value1, texts[i]) != 0)
          continue;

        instrument = oyConfig_New( CMM_BASE_REG, 0 );
        error = !instrument;

        if(error <= 0)
        error = oyOptions_SetFromText( instrument->backend_core,
                                       CMM_BASE_REG OY_SLASH "instrument_name",
                                       texts[i], OY_CREATE_NEW );


        value4 = oyOptions_FindString( options, "icc_profile", 0 );
        if(value4 || oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          size_t size = 6;
          const char * data = "dummy";

          /* In case the devices do not support network transparent ICC profile
           * setup, then use the DB stored profile, e.g.
           * @see oyInstrumentProfileFromDB() + oyProfile_FromFile()
           * This will then turn the backend in a pure local one.
           *
           * One the opposite the Xorg-"oyX1" backend puts the profile in 
           * X server.
           * Then it is up to Oyranos to take action. The backend needs to
           * report a issue to inform Oyranos, as seen below.
           */

          /** Warn and return issue on not found profile. */
          if(!size || !data)
          {
            message(oyMSG_WARN, (oyStruct_s*)options, dDev_DBG_FORMAT_ "\n "
                "Could not obtain icc_profile information for %s",
                dDev_DBG_ARGS_, texts[i]);
            error = -1;
          } else
          {
            p = oyProfile_FromMem( size, (const oyPointer)data, 0, 0 );
            o = oyOption_New( CMM_BASE_REG OY_SLASH "icc_profile", 0 );
            error = oyOption_StructMoveIn( o, (oyStruct_s**) &p );
            oyOptions_MoveIn( instrument->data, &o, -1 );
          }
        }

        if(oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          text = calloc( 4096, sizeof(char) );

          o = oyOptions_Find( instrument->data, "icc_profile" );

          if( o && o->value && o->value->oy_struct && 
              o->value->oy_struct->type_ == oyOBJECT_PROFILE_S)
          {
            /* our dummy profile will certainly fail */
            p = oyProfile_Copy( (oyProfile_s*) o->value->oy_struct, 0 );
            tmp = oyProfile_GetFileName( p, 0 );

            STRING_ADD( text, "  " );
            if(strrchr( tmp, OY_SLASH_C ))
              STRING_ADD( text, strrchr( tmp, OY_SLASH_C ) + 1 );
            else
              STRING_ADD( text, tmp );

            oyProfile_Release( &p );
          }

          if(error <= 0)
          error = oyOptions_SetFromText( instrument->data,
                                         CMM_BASE_REG OY_SLASH "oyNAME_NAME",
                                         text, OY_CREATE_NEW );
          free( text );
        }

        if(oyOptions_FindString( options, "oyNAME_DESCRIPTION", 0 ))
        {
          text = calloc( 4096, sizeof(char) );

          error = dDevInstrumentFromName_( value1, options, &instrument,
                                           allocateFunc );
          if(error <= 0 && instruments)
          {
            n = oyOptions_Count( instrument->backend_core );
            for( i = 0; i < n; ++i )
            {
              o = oyOptions_Get( instrument->backend_core, i );

              STRING_ADD( text, strrchr( o->registration, OY_SLASH_C ) + 1 );
              STRING_ADD( text, ":\n" );
              STRING_ADD( text, o->value->string );
              STRING_ADD( text, "\n" );
              
              oyOption_Release( &o );
            }
          }

          if(error <= 0)
          error = oyOptions_SetFromText( instrument->data,
                                     CMM_BASE_REG OY_SLASH "oyNAME_DESCRIPTION",
                                         text, OY_CREATE_NEW );
          free( text );
        }

        if(error <= 0)
          instrument->rank_map = oyRankMapCopy( dDev_rank_map,
                                                instrument->oy_->allocateFunc_);

        oyConfigs_MoveIn( instruments, &instrument, -1 );
      }

      if(error <= 0)
        *s = instruments;

      return error;
    }

    /* "properties" call section */
    value2 = oyOptions_FindString( options, "properties", 0 );
    if(value2)
    {
      error = dDevInstrumentFromName_( value1, options, &instrument,
                                       allocateFunc );

      if(error <= 0 && instrument)
        instrument->rank_map = oyRankMapCopy( dDev_rank_map,
                                                instrument->oy_->allocateFunc_);
      oyConfigs_MoveIn( instruments, &instrument, -1 );

      if(error <= 0)
        *s = instruments;

      return error;
    }

    /* "setup" call section */
    value2 = oyOptions_FindString( options, "setup", 0 );
    value3 = oyOptions_FindString( options, "profile_name", 0 );
    if(error <= 0 && value2)
    {
      error = !value1 || !value3;
      if(error >= 1)
        message(oyMSG_WARN, (oyStruct_s*)options, dDev_DBG_FORMAT_ "\n "
              "The instrument_name/profile_name option is missed. Options:\n%s",
                dDev_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );
      else
        error = 0; /* doSetup( value1, value3 ); */
      return error;
    }

    /* "unset" call section */
    value2 = oyOptions_FindString( options, "unset", 0 );
    if(error <= 0 && value2)
    {
      error = !value1;
      if(error >= 1)
        message(oyMSG_WARN, (oyStruct_s*)options, dDev_DBG_FORMAT_ "\n "
                "The instrument_name option is missed. Options:\n%s",
                dDev_DBG_ARGS_, oyOptions_GetText( options, oyNAME_NICK )
                );
      else
        error = 0; /* doUnset( value1 ); */
      return error;
    }
  }


  /* not to be reached section, e.g. warning */
  message(oyMSG_WARN, (oyStruct_s*)options, dDev_DBG_FORMAT_ "\n "
                "This point should not be reached. Options:\n%s", dDev_DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );

  dDevConfigsFromPatternUsage( (oyStruct_s*)options );

  return error;
}

/** Function dDevConfig_Check
 *  @brief   dDev oyCMMapi8_s instrument check
 *
 *  @param[in]     config              the monitor instrument configuration
 *  @return                            rank value
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int            dDevConfig_Check      ( oyConfig_s        * config )
{
  int error = !config,
      rank = 1;

  if(!config)
  {
    message(oyMSG_DBG, (oyStruct_s*)config, dDev_DBG_FORMAT_ "\n "
                "No config argument provided.\n", dDev_DBG_ARGS_ );
    return 0;
  }

  if(error <= 0)
  {
    /* evaluate a driver specific part of the options */
  }

  return rank;
}

/** @instance dDev_rank_map
 *  @brief    oyRankPad map for mapping instrument to configuration informations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
oyRankPad dDev_rank_map[] = {
  {"instrument_name", 2, -1, 0},       /**< is good */
  {"profile_name", 0, 0, 0},           /**< non relevant for instrument properties*/
  {"manufacturer", 1, -1, 0},          /**< is nice */
  {"model", 5, -5, 0},                 /**< important, should not fail */
  {"serial", 10, -2, 0},               /**< important, could slightly fail */
  {"host", 1, 0, 0},                   /**< nice to match */
  {"system_port", 2, 0, 0},            /**< good to match */
  {0,0,0,0}                            /**< end of list */
};

/** @instance dDev_api8
 *  @brief    dDev oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
oyCMMapi8_s dDev_api8 = {
  oyOBJECT_CMM_API8_S,
  0,0,0,
  0,                         /**< next */

  dDevCMMInit,               /**< oyCMMInit_f      oyCMMInit */
  dDevCMMMessageFuncSet,     /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */
  dDevCMMCanHandle,          /**< oyCMMCanHandle_f oyCMMCanHandle */

  CMM_BASE_REG,              /**< registration */
  {0,1,0},                   /**< int32_t version[3] */
  0,                         /**< char * id_ */

  0,                         /**< oyCMMapi5_s * api5_ */
  dDevConfigs_FromPattern,   /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
  dDevConfig_Check,          /**< oyConfig_Check_f oyConfig_Check */
  dDev_rank_map              /**< oyRankPad ** rank_map */
};




/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
const char * dDevGetText             ( const char        * select,
                                       oyNAME_e            type )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return _(CMM_NICK);
    else if(type == oyNAME_NAME)
      return _("Oyranos Dummy");
    else
      return _("The dummy useless backend of Oyranos.");
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
      return _("MIT");
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 2009 Kai-Uwe Behrmann; MIT");
    else
      return _("MIT license: http://www.opensource.org/licenses/mit-license.php");
  }
  return 0;
}

/** @instance dDev_cmm_module
 *  @brief    dDev module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
oyCMMInfo_s dDev_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.2",
  dDevGetText, /* oyCMMInfoGetText_f */
  OYRANOS_VERSION,

  (oyCMMapi_s*) & dDev_api8,

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"},
};

