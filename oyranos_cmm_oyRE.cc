/** @file oyranos_cmm_oyRE.cc
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

#include <oyranos/oyranos_cmm.h>

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <libraw/libraw.h>

/* --- internal definitions --- */

/* select a own four byte identifier string instead of "dDev" and replace the
 * dDev in the below macros.
 */
#define CMM_NICK "oyRE"
#define CMM_BASE_REG OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "config.raw-image." CMM_NICK

#define catCMMfunc(nick,func) nick ## func

#define CMMInit                 catCMMfunc( oyRE , CMMInit )
#define CMMallocateFunc         catCMMfunc( oyRE , CMMallocateFunc )
#define CMMdeallocateFunc       catCMMfunc( oyRE , CMMdeallocateFunc )
#define CMMMessageFuncSet       catCMMfunc( oyRE , CMMMessageFuncSet )
#define CMMCanHandle            catCMMfunc( oyRE , CMMCanHandle )
#define ConfigsFromPatternUsage catCMMfunc( oyRE , ConfigsFromPatternUsage )
#define InstrumentFromName_     catCMMfunc( oyRE , InstrumentFromName_ )
#define GetDevices              catCMMfunc( oyRE , GetDevices )
#define _api8                   catCMMfunc( oyRE , _api8 )
#define _rank_map               catCMMfunc( oyRE , _rank_map )
#define Configs_FromPattern     catCMMfunc( oyRE , Configs_FromPattern )
#define Config_Check            catCMMfunc( oyRE , Config_Check )
#define GetText                 catCMMfunc( oyRE , GetText )
#define _texts                  catCMMfunc( oyRE , _texts )
#define _cmm_module             catCMMfunc( oyRE , _cmm_module )

#define _DBG_FORMAT_ "%s:%d %s()"
#define _DBG_ARGS_ __FILE__,__LINE__,__func__
#define _(x) x
#define STRING_ADD(a,b) sprintf( &a[strlen(a)], b )

/** @instance _rank_map
 *  @brief    oyRankPad map for mapping instrument to configuration informations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 *
 *  \todo { In progress }
 */
oyranos::oyRankPad _rank_map[] = {
  {"instrument_name", 2, -1, 0},/**< is good */
  {"profile_name", 0, 0, 0},/**< non relevant for instrument properties*/

	/* EXIF Fields */
  {"Exif.Image.Make", 1, -1, 0},					/**< is nice */
  {"Exif.Image.Model", 5, -5, 0},				/**< important, should not fail */
  {"Exif.SerialNumber", 10, -2, 0},				/**< important, could slightly fail */ /*Exif.Make.SerialNumber*/
  {"Exif.Photo.ISOSpeedRatings", 1, 0, 0},	/**< is nice */
  {"Exif.Photo.ExposureProgram", 1, 0, 0},	/**< nice to match */
  {"Exif.Photo.Flash", 1, 0, 0},					/**< nice to match */

  /* Possibly not relevant options are marked with: O->Output R->Repair */
  /* LibRaw Options affecting open_file() */
  /* LibRaw Options affecting unpack() */
  {"use_camera_wb", 1, -1, 0},					/**< is nice */
  {"use_camera_matrix", 1, -1, 0},				/**< is nice */
  {"half_size", 1, -1, 0},							/**< is nice */
//{"filtering_mode", 1, -1, 0},					/**< is nice */ /*not in libraw-lite*/
//This is a bit-field. Out of all the possible flags, only LIBRAW_FILTERING_NORAWCURVE
//seems to be relevant to color [From LibRaw API docs]:
//This bit turns off tone curve processing (for tone curves read from file metadata or
//calculated from constants). This setting is supported only for bayer-pattern cameras
//with tone curve; 
  {"threshold", 1, -1, 0},							/**< is nice */ /*R*/
  {"aber[0]", 1, -1, 0},							/**< is nice */ /*R*/
  {"aber[1]", 1, -1, 0},							/**< is nice */ /*R*/
  {"aber[2]", 1, -1, 0},							/**< is nice */ /*R*/
  {"aber[3]", 1, -1, 0},							/**< is nice */ /*R*/
  /* LibRaw Options affecting dcraw_process() */
  {"greybox[0]", 1, -1, 0},						/**< is nice */
  {"greybox[1]", 1, -1, 0},						/**< is nice */
  {"greybox[2]", 1, -1, 0},						/**< is nice */
  {"greybox[3]", 1, -1, 0},						/**< is nice */
  {"gamm[0]", 1, -1, 0},							/**< is nice */
  {"gamm[1]", 1, -1, 0},							/**< is nice */
  {"gamm[2]", 1, -1, 0},							/**< is nice */
  {"gamm[3]", 1, -1, 0},							/**< is nice */
  {"gamm[4]", 1, -1, 0},							/**< is nice */
  {"gamm[5]", 1, -1, 0},							/**< is nice */
  {"user_mul[0]", 1, -1, 0},						/**< is nice */
  {"user_mul[1]", 1, -1, 0},						/**< is nice */
  {"user_mul[2]", 1, -1, 0},						/**< is nice */
  {"user_mul[3]", 1, -1, 0},						/**< is nice */
  {"bright", 1, -1, 0},								/**< is nice */
  {"four_color_rgb", 1, -1, 0},					/**< is nice */
  {"highlight", 1, -1, 0},							/**< is nice */
  {"use_auto_wb", 1, -1, 0},						/**< is nice */
  {"output_color", 1, -1, 0},						/**< is nice */
  {"camera_profile", 1, -1, 0},					/**< is nice */
  {"output_bps", 1, -1, 0},						/**< is nice */
  {"user_qual", 1, -1, 0},							/**< is nice */
  {"user_black", 1, -1, 0},						/**< is nice */
  {"user_sat", 1, -1, 0},							/**< is nice */
  {"med_passes", 1, -1, 0},						/**< is nice */
  {"auto_bright_thr", 1, -1, 0},					/**< is nice */
  {"no_auto_bright", 1, -1, 0},					/**< is nice */

  /* Extra options (user supplied) */
  {"illumination_source", 1, -1, 0},			/**< is nice */
  {0,0,0,0}												/**< end of list */
};

#ifdef __cplusplus
extern "C" {
namespace oyranos {
#endif /* __cplusplus */

oyMessage_f message = 0;

extern oyranos::oyCMMapi8_s _api8;

#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

using namespace oyranos;

/* --- implementations --- */

int                CMMInit       ( )
{
  int error = 0;
  return error;
}



oyPointer          CMMallocateFunc   ( size_t              size )
{
  oyPointer p = 0;
  if(size)
    p = malloc(size);
  return p;
}

void               CMMdeallocateFunc ( oyPointer           mem )
{
  if(mem)
    free(mem);
}

/** @func  CMMMessageFuncSet
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int            CMMMessageFuncSet     ( oyMessage_f         message_func )
{
  message = message_func;
  return 0;
}

/** @func  CMMCanHandle
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 *  @date    2009/02/09
 */
int                CMMCanHandle      ( oyCMMQUERY_e        type,
                                       uint32_t            value ) {return 0;}

#define OPTIONS_ADD(opts, name) if(!error && name) \
        error = oyOptions_SetFromText( &opts, \
                                       CMM_BASE_REG OY_SLASH #name, \
                                       name, OY_CREATE_NEW );

void         ConfigsFromPatternUsage( oyStruct_s        * options )
{
    /** oyMSG_WARN should make shure our message is visible. */
    message( oyMSG_WARN, options, _DBG_FORMAT_ "\n %s",
             _DBG_ARGS_,
      "The following help text informs about the communication protocol.");
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"command=list\" will provide a list of\n"
      " available instruments. The actual instrument name can be found in\n"
      " option \"instrument_name\". The call is as lightwight as possible.\n"
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
      "The presence of option \"command=properties\" will provide the\n"
      " instruments properties. Requires a instrument_name identifier\n"
      " returned with the \"list\" option.\n"
      " The properties may cover following entries:\n"
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
      "The presence of option \"command=setup\" will setup the instrument\n"
      " from a profile.\n"
      " The option \"instrument_name\" must be present, see \"list\" above.\n"
      " The option \"profile_name\" must be present, containing a ICC profile\n"
      " file name."
      );
    message( oyMSG_WARN, options, "%s()\n %s", __func__,
      "The presence of option \"command=unset\" will invalidate a profile of\n"
      " a instrument.\n"
      " The option \"instrument_name\" must be present, see \"list\" above.\n"
      );

  return;
}

int              InstrumentFromName_ ( const char        * instrument_name,
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
        message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_
                "The \"instrument_name\" argument is\n"
                " missed to select a appropriate instrument for the"
                " \"properties\" call.", _DBG_ARGS_ );
        error = 1;
        return error;
      }

      /* now get the data from somewhere*/
      if(strcmp(instrument_name, "dDev_1") == 0)
      {
        manufacturer = "People_1";
        model = "people-one";
        serial = "11";
        system_port = "usb-01";
      } else if(strcmp(instrument_name, "dDev_2") == 0)
      {
        manufacturer = "Village_2";
        model = "yard-two";
        serial = "22";
        system_port = "usb-02";
      }

      host = "localhost";

      if(error != 0)
        message( oyMSG_WARN, (oyStruct_s*)options, 
                 _DBG_FORMAT_ "Could not complete \"properties\" call.\n"
                 " oyGetMonitorInfo_lib returned with %s; instrument_name:"
                 " \"%s\"", _DBG_ARGS_, error > 0 ? "error(s)" : "issue(s)",
                 instrument_name ? instrument_name : "" );

      if(error <= 0)
      {
        if(!*instrument)
          *instrument = oyConfig_New( CMM_BASE_REG, 0 );
        error = !*instrument;
        if(!error && instrument_name)
        error = oyOptions_SetFromText( &(*instrument)->backend_core,
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

int     GetDevices                   ( char            *** list,
                                       oyAlloc_f           allocateFunc )
{
	/*
  int len = sizeof(char*) * 3;
  char ** texts = allocateFunc( len );

  memset( texts, 0, len );
  texts[0] = allocateFunc(24); sprintf( texts[0], "dDev_1" );
  texts[1] = allocateFunc(24); sprintf( texts[1], "dDev_2" );

  *list = texts;
  */
  return 2;
}

/** Function Configs_FromPattern
 *  @brief   CMM_NICK oyCMMapi8_s raw images
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int              Configs_FromPattern ( const char        * registration,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** s )
{
  oyConfigs_s * instruments = 0;
  oyConfig_s * instrument = 0;
  oyOption_s * o = 0;
  oyProfile_s * p = 0;
  char ** texts = 0;
  char * text = 0;
  int texts_n = 0, i,
      error = !s;
  const char * value1 = 0,
             * value2 = 0,
             * value3 = 0,
             * value4 = 0;
  int rank = oyFilterRegistrationMatch( _api8.registration, registration,
                                        oyOBJECT_CMM_API8_S );
  oyAlloc_f allocateFunc = malloc;
  static char * num = 0;
  const char * tmp = 0;

  if(!num)
    num = (char*)malloc( 80 );

  if(!options || !oyOptions_Count( options ))
  {
    /** oyMSG_WARN should make shure our message is visible. */
    ConfigsFromPatternUsage( (oyStruct_s*)options );
    return 0;
  }

  if(rank && error <= 0)
  {
    instruments = oyConfigs_New(0);

    /* "list" call section */
    value1 = oyOptions_FindString( options, "instrument_name", 0 );
    value2 = oyOptions_FindString( options, "command", "list" );
    if(oyOptions_FindString( options, "command", "list" ) ||
       (!oyOptions_FindString( options, "command", "properties" ) &&
        !oyOptions_FindString( options, "command", "setup" ) &&
        !oyOptions_FindString( options, "command", "unset" ))
      )
    {
      texts_n = GetDevices( &texts, allocateFunc );

      for( i = 0; i < texts_n; ++i )
      {
        /* filter */
        if(value1 && strcmp(value1, texts[i]) != 0)
          continue;

        instrument = oyConfig_New( CMM_BASE_REG, 0 );
        error = !instrument;

        if(error <= 0)
        error = oyOptions_SetFromText( &instrument->backend_core,
                                       CMM_BASE_REG OY_SLASH "instrument_name",
                                       texts[i], OY_CREATE_NEW );


        value4 = oyOptions_FindString( options, "icc_profile", 0 );
        if(value4 || oyOptions_FindString( options, "oyNAME_NAME", 0 ))
        {
          size_t size = 6;
          const char * data = "raw-image";

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
            message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
                "Could not obtain icc_profile information for %s",
                _DBG_ARGS_, texts[i]);
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
          text = (char*)calloc( 4096, sizeof(char) );

          o = oyOptions_Find( instrument->data, "icc_profile" );

          if( o && o->value && o->value->oy_struct && 
              o->value->oy_struct->type_ == oyOBJECT_PROFILE_S)
          {
            /* our raw image profile will certainly fail */
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
          error = oyOptions_SetFromText( &instrument->data,
                                         CMM_BASE_REG OY_SLASH "oyNAME_NAME",
                                         text, OY_CREATE_NEW );
          free( text );
        }

        if(error <= 0)
          instrument->rank_map = oyRankMapCopy( _rank_map,
                                                instrument->oy_->allocateFunc_);

        oyConfigs_MoveIn( instruments, &instrument, -1 );
      }

      if(error <= 0)
        *s = instruments;

      return error;
    }

    /* "properties" call section */
    value2 = oyOptions_FindString( options, "command", "properties" );
    if(value2)
    {
      texts_n = GetDevices( &texts, allocateFunc );

      for( i = 0; i < texts_n; ++i )
      {
        /* filter */
        if(value1 && strcmp(value1, texts[i]) != 0)
          continue;

        instrument = oyConfig_New( CMM_BASE_REG, 0 );
        error = !instrument;

        if(error <= 0)
        error = oyOptions_SetFromText( &instrument->backend_core,
                                       CMM_BASE_REG OY_SLASH "instrument_name",
                                       texts[i], OY_CREATE_NEW );

        error = InstrumentFromName_( value1, options, &instrument,
                                         allocateFunc );

        if(error <= 0 && instrument)
          instrument->rank_map = oyRankMapCopy( _rank_map,
                                                instrument->oy_->allocateFunc_);
        oyConfigs_MoveIn( instruments, &instrument, -1 );
      }

      if(error <= 0)
        *s = instruments;

      return error;
    }

    /* "setup" call section */
    value2 = oyOptions_FindString( options, "command", "setup" );
    value3 = oyOptions_FindString( options, "profile_name", 0 );
    if(error <= 0 && value2)
    {
      error = !value1 || !value3;
      if(error >= 1)
        message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
              "The instrument_name/profile_name option is missed. Options:\n%s",
                _DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );
      else
        error = 0; /* doSetup( value1, value3 ); */
      return error;
    }

    /* "unset" call section */
    value2 = oyOptions_FindString( options, "command", "unset" );
    if(error <= 0 && value2)
    {
      error = !value1;
      if(error >= 1)
        message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
                "The instrument_name option is missed. Options:\n%s",
                _DBG_ARGS_, oyOptions_GetText( options, oyNAME_NICK )
                );
      else
        error = 0; /* doUnset( value1 ); */
      return error;
    }
  }


  /* not to be reached section, e.g. warning */
  message(oyMSG_WARN, (oyStruct_s*)options, _DBG_FORMAT_ "\n "
                "This point should not be reached. Options:\n%s", _DBG_ARGS_,
                oyOptions_GetText( options, oyNAME_NICK )
                );

  ConfigsFromPatternUsage( (oyStruct_s*)options );

  return error;
}

/** Function Config_Check
 *  @brief   CMM_NICK oyCMMapi8_s instrument check
 *
 *  @param[in]     config              the monitor instrument configuration
 *  @return                            rank value
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int                Config_Check      ( oyConfig_s        * config )
{
  int error = !config,
      rank = 1;

  if(!config)
  {
    message(oyMSG_DBG, (oyStruct_s*)config, _DBG_FORMAT_ "\n "
                "No config argument provided.\n", _DBG_ARGS_ );
    return 0;
  }

  if(error <= 0)
  {
    /* evaluate a driver specific part of the options */
  }

  return rank;
}

/** @instance _api8
 *  @brief    CMM_NICK oyCMMapi8_s implementations
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
oyCMMapi8_s oyranos::_api8 = {
  oyOBJECT_CMM_API8_S,
  0,0,0,
  0,                         /**< next */

  CMMInit,                   /**< oyCMMInit_f      oyCMMInit */
  CMMMessageFuncSet,         /**< oyCMMMessageFuncSet_f oyCMMMessageFuncSet */
  CMMCanHandle,              /**< oyCMMCanHandle_f oyCMMCanHandle */

  CMM_BASE_REG,              /**< registration */
  {0,1,0},                   /**< int32_t version[3] */
  0,                         /**< char * id_ */

  0,                         /**< oyCMMapi5_s * api5_ */
  Configs_FromPattern,       /**<oyConfigs_FromPattern_f oyConfigs_FromPattern*/
  Config_Check,              /**< oyConfig_Check_f oyConfig_Check */
  _rank_map                  /**< oyRankPad ** rank_map */
};




/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 *
 *  \todo { Add usage info }
 */
const char * GetText                 ( const char        * select,
                                       oyNAME_e            type )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return _(CMM_NICK);
    else if(type == oyNAME_NAME)
      return _("Oyranos RAW Image");
    else
      return _("The raw image (hopefully)usefull backend of Oyranos.");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return _("orion");
    else if(type == oyNAME_NAME)
      return _("Yiannis Belias");
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
const char * _texts[5] = {"name","copyright","manufacturer","help",0};

/** @instance _cmm_module
 *  @brief    CMM_NICK module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/12 (Oyranos: 0.1.10)
 *  @date    2009/06/23
 */
oyCMMInfo_s _cmm_module = {

  oyOBJECT_CMM_INFO_S, /**< ::type; the object type */
  0,0,0,               /**< static objects omit these fields */
  CMM_NICK,            /**< ::cmm; the four char filter id */
  (char*)"0.2",        /**< ::backend_version */
  GetText,             /**< ::getText; UI texts */
  (char**)_texts,      /**< ::texts; list of arguments to getText */
  OYRANOS_VERSION,     /**< ::oy_compatibility; last supported Oyranos CMM API*/

  /** ::api; The first filter api structure. */
  (oyCMMapi_s*) & _api8,

  /** ::icon; zero terminated list of a icon pyramid */
  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"},
};

/* Helper functions */

/** @internal
 * @brief Get the options from LibRaw, and put them in a configuration object
 *
 * @param[in]	params				The LibRaw parameter structure
 * @param[out]	config				The configuration object to hold the option-value pairs
 *
 * \todo { Untested }
 */
#include <string>
#include <sstream>
int OyAddLibRawOptions_( libraw_output_params_t* params, oyConfig_s** config )
{
	int status;
	std::string value;
	std::ostringstream out(value);
#define ADD_DB_DATA( opt ) out<<params->opt; \
									if (!oyConfig_AddDBData( 	*config, \
																		#opt, \
																		value.c_str(), \
																		OY_CREATE_NEW )) \
										message( oyMSG_WARN, 0, "%s()\n Unable to save option %s\n", __func__, #opt ); \
									value.clear();

	ADD_DB_DATA( use_camera_wb )
	ADD_DB_DATA( use_camera_matrix )
	ADD_DB_DATA( half_size )
	ADD_DB_DATA( threshold )
	ADD_DB_DATA( aber[0] )
	ADD_DB_DATA( aber[1] )
	ADD_DB_DATA( aber[2] )
	ADD_DB_DATA( aber[3] )
	ADD_DB_DATA( greybox[0] )
	ADD_DB_DATA( greybox[1] )
	ADD_DB_DATA( greybox[2] )
	ADD_DB_DATA( greybox[3] )
	ADD_DB_DATA( gamm[0] )
	ADD_DB_DATA( gamm[1] )
	ADD_DB_DATA( gamm[2] )
	ADD_DB_DATA( gamm[3] )
	ADD_DB_DATA( gamm[4] )
	ADD_DB_DATA( gamm[5] )
	ADD_DB_DATA( user_mul[0] )
	ADD_DB_DATA( user_mul[1] )
	ADD_DB_DATA( user_mul[2] )
	ADD_DB_DATA( user_mul[3] )
	ADD_DB_DATA( bright )
	ADD_DB_DATA( four_color_rgb )
	ADD_DB_DATA( highlight )
	ADD_DB_DATA( use_auto_wb )
	ADD_DB_DATA( output_color )
	ADD_DB_DATA( camera_profile )
	ADD_DB_DATA( output_bps )
	ADD_DB_DATA( user_qual )
	ADD_DB_DATA( user_black )
	ADD_DB_DATA( user_sat )
	ADD_DB_DATA( med_passes )
	ADD_DB_DATA( auto_bright_thr )
	ADD_DB_DATA( no_auto_bright )

	return status;
}
