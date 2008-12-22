/** @file oyranos_cmm_oyIM.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    backends for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2008/12/16
 */

#include "config.h"
#include "oyranos_alpha.h"
#include "oyranos_cmm.h"
#include "oyranos_cmm_oyIM.h"
#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_definitions.h"
#include "oyranos_texts.h"
#include <iconv.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* --- internal definitions --- */


int oyIMCMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... );
oyMessage_f message = oyIMCMMWarnFunc;



/* --- implementations --- */

/** @func  oyIMCMMInit
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/02
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 */
int                oyIMCMMInit       ( )
{
  int error = 0;
  return error;
}



/** @func  oyIMIconv
 *  @brief convert between codesets
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/05/27
 *  @since   2008/07/23 (Oyranos: 0.1.8)
 */
int                oyIMIconv         ( const char        * input,
                                       size_t              len,
                                       char              * output,
                                       const char        * from_codeset )
{
  return oyIconv(input, len, output, from_codeset, 0);
}





/** @func  oyIMCMMWarnFunc
 *  @brief message handling
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/02
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 */
int oyIMCMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... )
{
  char* text = (char*)calloc(sizeof(char), 4096);
  va_list list;
  const char * type_name = "";
  int id = -1;

  if(context && oyOBJECT_NONE < context->type_)
  {
    type_name = oyStruct_TypeToText( context );
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

/** @func  oyIMCMMMessageFuncSet
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/02
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 */
int            oyIMCMMMessageFuncSet ( oyMessage_f         message_func )
{
  message = message_func;
  return 0;
}


int oyStructList_MoveInName( oyStructList_s * texts, char ** text, int pos )
{
  int error = !texts || ! text;
  oyName_s * name = 0;
  oyStruct_s * oy_struct = 0;
  if(!error)
  {
     name = oyName_new(0);
     name->name = *text;
     *text = 0;
     oy_struct = (oyStruct_s*) name;
     oyStructList_MoveIn( texts, &oy_struct, pos );
  }
  return error;
}

int oyStructList_AddName( oyStructList_s * texts, const char * text, int pos )
{
  int error = !texts;
  oyName_s * name = 0;
  oyStruct_s * oy_struct = 0;
  char * tmp = 0;
  if(!error)
  {
     name = oyName_new(0);
     if(!name) return 1;
     if(text)
     {
       tmp = oyAllocateFunc_( strlen(text) + 1 );
       if(!tmp) return 1;
       sprintf( tmp, "%s", text ); 
       name->name = tmp;
     }
     oy_struct = (oyStruct_s*) name;
     oyStructList_MoveIn( texts, &oy_struct, pos );
  }
  return error;
}



const char * oyIMWidget_GetDummy     ( const char        * func_name,
                                       uint32_t          * result )
{return 0;}
oyWIDGET_EVENT_e oyIMWidget_EventDummy
                                     ( const char        * wid,
                                       oyWIDGET_EVENT_e    type )
{return 0;}


oyWIDGET_EVENT_e   oyIMWidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event )
{return 0;}





/** @instance oyIM_cmm_module
 *  @brief    oyIM module infos
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 *  @date    2008/01/02
 */
oyCMMInfo_s oyIM_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.1.10",
  {oyOBJECT_NAME_S, 0,0,0, CMM_NICK, "Oyranos modules", "Oyranos supplied modules"},
  {oyOBJECT_NAME_S, 0,0,0, "Kai-Uwe", "Kai-Uwe Behrmann", "Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/wiki/index.php?title=Oyranos/Download"},
  {oyOBJECT_NAME_S, 0,0,0, "new BSD", "Copyright (c) 2005-2008 Kai-Uwe Behrmann", "new BSD license: http://www.opensource.org/licenses/bsd-license.php"},
  OYRANOS_VERSION,

  (oyCMMapi_s*) & oyIM_api3,
  0,

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"}

};

