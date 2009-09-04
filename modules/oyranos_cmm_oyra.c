/** @file oyranos_cmm_oyra.c
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
#include "oyranos_cmm.h"
#include "oyranos_cmm_oyra.h"
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

#define CMM_NICK "oyra"

int oyraCMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... );
oyMessage_f message = oyraCMMWarnFunc;



/* --- implementations --- */

/** Function oyraCMMInit
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/02
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 */
int                oyraCMMInit       ( )
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
  return oyIconv(input, len, output, from_codeset, 0);
}





/** Function oyraCMMWarnFunc
 *  @brief message handling
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/02
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 */
int oyraCMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... )
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

/** Function oyraCMMMessageFuncSet
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/02
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 */
int            oyraCMMMessageFuncSet ( oyMessage_f         message_func )
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



const char * oyraWidget_GetDummy     ( const char        * func_name,
                                       uint32_t          * result )
{return 0;}
oyWIDGET_EVENT_e oyraWidget_EventDummy
                                     ( const char        * wid,
                                       oyWIDGET_EVENT_e    type )
{return 0;}


oyWIDGET_EVENT_e   oyraWidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event )
{return 0;}



/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2008/12/23
 */
const char * oyraGetText             ( const char        * select,
                                       oyNAME_e            type )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return _(CMM_NICK);
    else if(type == oyNAME_NAME)
      return _("Oyranos modules");
    else
      return _("Oyranos supplied modules");
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
      return _("Copyright (c) 2005-2009 Kai-Uwe Behrmann; newBSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/bsd-license.php");
  }
  return 0;
}
const char *oyra_texts[4] = {"name","copyright","manufacturer",0};


/** @instance oyra_cmm_module
 *  @brief    oyra module infos
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 *  @date    2008/01/02
 */
oyCMMInfo_s oyra_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.1.8",
  oyraGetText,        /**< getText */
  (char**)oyra_texts, /**< texts; list of arguments to getText */
  OYRANOS_VERSION,

  (oyCMMapi_s*) & oyra_api4_image_root,

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"},
};

