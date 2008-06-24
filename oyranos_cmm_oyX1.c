/** @file oyranos_cmm_oyX1.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2007-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    Oyranos X11 backend for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2007/12/12
 */

#include "oyranos_cmm.h"
#include "oyranos_monitor.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>


/* --- internal definitions --- */

#define CMM_NICK "oyX1"

int oyX1CMMWarnFunc( int code, const char * format, ... );
oyMessageFunc_t message = oyX1CMMWarnFunc;

int                lcmsCMMCheckPointer(oyCMMptr_s        * cmm_ptr,
                                       const char        * resource );

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
int oyX1CMMWarnFunc( int code, const char * format, ... )
{
  char* text = (char*)calloc(sizeof(char), 4096);
  va_list list;

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
int            oyX1CMMMessageFuncSet ( oyMessageFunc_t     message_func )
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


/** @instance oyX1_api2
 *  @brief    oyX1 oyCMMapi2_s implementations
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/12
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 */
oyCMMapi2_s oyX1_api2 = {

  oyOBJECT_TYPE_CMM_API2_S,
  0,0,0,
  0,

  oyX1CMMInit,
  oyX1CMMMessageFuncSet,
  oyX1CMMCanHandle,

  oyGetMonitorInfo,
  oyGetScreenFromPosition,

  oyGetDisplayNameFromPosition,
  oyGetMonitorProfile,
  oyGetMonitorProfileName,

  oySetMonitorProfile,
  oyActivateMonitorProfiles
};



/** @instance oyX1_cmm_module
 *  @brief    oyX1 module infos
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/12
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 */
oyCMMInfo_s oyX1_cmm_module = {

  oyOBJECT_TYPE_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.2",
  {oyOBJECT_TYPE_NAME_S, 0,0,0, "oyX1", "Oyranos X11", "The window support backend of Oyranos."},
  {oyOBJECT_TYPE_NAME_S, 0,0,0, "Kai-Uwe", "Kai-Uwe Behrmann", "Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/#download"},
  {oyOBJECT_TYPE_NAME_S, 0,0,0, "new BSD", "Copyright (c) 2005-2008 Kai-Uwe Behrmann", "new BSD license: http://www.opensource.org/licenses/bsd-license.php"},
  108,

  (oyCMMapi_s*) & oyX1_api2,
  1,

  {oyOBJECT_TYPE_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"}

};

