/** @file oyranos_cmm_oyra.c
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

#include "oyranos_config_internal.h"

#include "oyCMM_s.h"
#include "oyranos_cmm.h"
#include "oyranos_cmm_oyra.h"
#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_definitions.h"
#include "oyranos_texts.h"
#include "oyranos_string.h"

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* --- internal definitions --- */

#define CMM_NICK "oyra"

oyMessage_f oyra_msg = oyMessageFunc;



/* --- implementations --- */

/** Function oyraCMMInit
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 *  @date    2009/12/17
 */
int                oyraCMMInit       ( oyStruct_s        * filter OY_UNUSED )
{
  int error = 0;
  return error;
}

/** Function oyraCMMReset
 *  @brief API requirement
 *
 *  @version Oyranos: 0.9.7
 *  @since   2019/09/03 (Oyranos: 0.9.7)
 *  @date    2019/09/03
 */
int                oyraCMMReset      ( oyStruct_s        * filter OY_UNUSED )
{
  int error = 0;
  return error;
}



/** Function oyraIconv
 *  @brief convert between codesets
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/05/27
 *  @since   2008/07/23 (Oyranos: 0.1.8)
 */
int                oyraIconv         ( const char        * input,
                                       size_t              len,
                                       char              * output,
                                       const char        * from_codeset )
{
  return oyIconv(input, len, len, output, from_codeset, 0);
}




/** Function oyraCMMMessageFuncSet
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/02
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 */
int            oyraCMMMessageFuncSet ( oyMessage_f         message_func )
{
  oyra_msg = message_func;
  return 0;
}





const char * oyraWidget_GetDummy     ( const char        * func_name OY_UNUSED,
                                       uint32_t          * result OY_UNUSED )
{return 0;}
oyWIDGET_EVENT_e oyraWidget_EventDummy
                                     ( const char        * wid OY_UNUSED,
                                       oyWIDGET_EVENT_e    type OY_UNUSED )
{return 0;}


oyWIDGET_EVENT_e   oyraWidgetEvent   ( oyOptions_s       * options OY_UNUSED,
                                       oyWIDGET_EVENT_e    type OY_UNUSED,
                                       oyStruct_s        * event OY_UNUSED )
{return 0;}



/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2008/12/23
 */
const char * oyraGetText             ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return CMM_NICK;
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
      return _("Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/wiki/index.php?title=Oyranos/Download");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return "newBSD";
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 2005-2010 Kai-Uwe Behrmann; newBSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/BSD-3-Clause");
  }
  return 0;
}
const char *oyra_texts[4] = {"name","copyright","manufacturer",0};

extern oyCMM_s oyra_cmm_module;
static int oyra_initialised = 0;
int  oyraInit                        ( oyStruct_s        * module_info OY_UNUSED )
{
  oyCMMapi_s * a = 0,
             * a_tmp = 0,
             * m = 0;

  if(oyra_initialised > 1)
    oyra_msg( oyMSG_WARN, module_info, OY_DBG_FORMAT_ "multiple inits: %d", OY_DBG_ARGS_, oyra_initialised );

  if(oyra_initialised)
    return 0;
  ++oyra_initialised;

  /* search the last filter */
  a = oyCMMinfo_GetApi( (oyCMMinfo_s*) &oyra_cmm_module );
  while((a_tmp = oyCMMapi_GetNext( a )) != 0)
    a = a_tmp;

  /* append new items */
  m = oyraApi4ImageChannelCreate();
  oyCMMapi_SetNext( a, m ); a = m;
  m = oyraApi7ImageChannelCreate();
  oyCMMapi_SetNext( a, m ); a = m;

  m = oyraApi4ImageExposeCreate();
  oyCMMapi_SetNext( a, m ); a = m;
  m = oyraApi7ImageExposeCreate();
  oyCMMapi_SetNext( a, m ); a = m;

  m = oyraApi4ImageScaleCreate();
  oyCMMapi_SetNext( a, m ); a = m;
  m = oyraApi7ImageScaleCreate();
  oyCMMapi_SetNext( a, m ); a = m;

  return 0;
}
int  oyraReset                       ( oyStruct_s        * module_info )
{
  if(!oyra_initialised)
    return 0;

  oyra_initialised = 0;
  if(oy_debug)
    oyra_msg( oyMSG_DBG, module_info, OY_DBG_FORMAT_, OY_DBG_ARGS_ );

  if((oyStruct_s*)&oyra_cmm_module != module_info)
    oyra_msg( oyMSG_WARN, module_info, OY_DBG_FORMAT_ "wrong module info passed in", OY_DBG_ARGS_ );

  return 0;
}

oyIcon_s oyra_icon = {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"};

/** @brief    oyra module infos
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 *  @date    2008/01/02
 */
oyCMM_s oyra_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.1.8",
  oyraGetText,        /**< getText */
  (char**)oyra_texts, /**< texts; list of arguments to getText */
  OYRANOS_VERSION,

  (oyCMMapi_s*) & oyra_api4_image_root,

  &oyra_icon,
  oyraInit,
  oyraReset
};

