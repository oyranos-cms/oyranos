
/** @file @internal
 *  @brief Oyranos X11 backend for Oyranos
 *
 */

/** @date      12. 12. 2007 */



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

int            oyX1CMMMessageFuncSet ( oyMessageFunc_t     message_func )
{
  message = message_func;
  return 0;
}


oyCMMapi2_s oyX1_api2 = {

  oyOBJECT_TYPE_CMM_API2_S,
  0,

  oyX1CMMInit,
  oyX1CMMMessageFuncSet,

  oyGetMonitorInfo,
  oyGetScreenFromPosition,

  oyGetDisplayNameFromPosition,
  oyGetMonitorProfile,
  oyGetMonitorProfileName,

  oySetMonitorProfile,
  oyActivateMonitorProfiles
};


#define oyX1Signature 0x6f795831

oyCMMInfo_s oyX1_cmm_module = {

  oyOBJECT_TYPE_CMM_INFO_S,

  oyX1Signature,
  "0.2",
  {oyOBJECT_TYPE_NAME_S, "oyX1", "Oyranos X11", "The window support backend of Oyranos."},
  {oyOBJECT_TYPE_NAME_S, "Kai-Uwe", "Kai-Uwe Behrmann", "Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/#download"},
  {oyOBJECT_TYPE_NAME_S, "new BSD", "Copyright (c) 2005-2007 Kai-Uwe Behrmann", "new BSD license: http://www.opensource.org/licenses/bsd-license.php"},
  108,

  (oyCMMapi_s*) & oyX1_api2,
  1,

  {oyOBJECT_TYPE_ICON_S, 0,0,0, "oyranos_logo.png"}

};

