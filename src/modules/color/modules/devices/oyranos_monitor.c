/** @file oyranos_monitor.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2005-2010 (C) Kai-Uwe Behrmann
 *
 *  @brief    monitor device detection common functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2005/01/31
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "limits.h"
#include <unistd.h>  /* intptr_t */

#include "oyranos_config_internal.h"

# include <X11/Xcm/XcmEdidParse.h>

#include "oyConfig_s_.h"
#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_monitor.h"
#include "oyranos_monitor_internal.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"

/* ---  Helpers  --- */

#define OPTIONS_ADD(opts, name, clear) if(!error && name) { char * t = 0; \
        STRING_ADD( t , registration ); STRING_ADD( t, OY_SLASH #name ); \
        error = oyOptions_SetFromText( &opts, t, \
                                       name, OY_CREATE_NEW ); \
        if(clear && name) { oyDeAllocateFunc_( (char*)name ); name = 0; } \
        if(t) oyFree_m_(t); }
#define OPTIONS_ADD_INT(opts, name) if(!error && name) { char * t = 0; \
        STRING_ADD( t , registration ); STRING_ADD( t, OY_SLASH #name ); \
        oySprintf_( num, "%d", name ); \
        error = oyOptions_SetFromText( &opts, t, \
                                       num, OY_CREATE_NEW ); \
        if(t) oyFree_m_(t); }

/* --- internal API definition --- */

int            oyDeviceFillInfos     ( const char        * registration,
                                       oyConfig_s       ** device_,
                                       const char        * device_name,
                                       const char        * host,
                                       const char        * display_geometry,
                                       const char        * system_port,
                                       const char        * EDID_manufacturer,
                                       const char        * EDID_mnft,
                                       const char        * EDID_model,
                                       const char        * EDID_serial,
                                       const char        * EDID_vendor,
                                       int                 week,
                                       int                 year,
                                       int                 EDID_mnft_id,
                                       int                 EDID_model_id,
                                       double            * colors,
                                       oyOptions_s       * options )
{
  int error = !device_;
  char * text = 0;
  oyConfig_s_ ** device = (oyConfig_s_**)device_;

  if(error <= 0)
  {
      char * EDID_date=0;
      char num[16];

      if(error <= 0)
      {
        char* t = 0;
        const char * manufacturer = EDID_manufacturer,
                   * model = EDID_model,
                   * serial = EDID_serial,
                   * prefix = "EDID_";
        STRING_ADD(t, registration);
        STRING_ADD(t, OY_SLASH );
        STRING_ADD(t, "device_name");

        sprintf( num, "%d", year );
        STRING_ADD(EDID_date, num);
        STRING_ADD(EDID_date, "-T");
        sprintf( num, "%d", week );
        STRING_ADD(EDID_date, num);
        if(!*device)
          *device = (oyConfig_s_*)oyConfig_FromRegistration( registration, 0 );
        error = !*device;
        if(!error && device_name)
        error = oyOptions_SetFromText( &(*device)->backend_core, t,
                                       device_name, OY_CREATE_NEW );
        oyFree_m_(t);

        OPTIONS_ADD( (*device)->backend_core, prefix, 0 )
        OPTIONS_ADD( (*device)->backend_core, EDID_manufacturer, 0 )
        OPTIONS_ADD( (*device)->backend_core, manufacturer, 0 )
        OPTIONS_ADD( (*device)->backend_core, EDID_mnft, 0 )
        OPTIONS_ADD( (*device)->backend_core, EDID_model, 0 )
        OPTIONS_ADD( (*device)->backend_core, model, 0 )
        OPTIONS_ADD( (*device)->backend_core, EDID_serial, 0 )
        OPTIONS_ADD( (*device)->backend_core, serial, 0 )
        OPTIONS_ADD( (*device)->backend_core, EDID_vendor, 0 )
        OPTIONS_ADD( (*device)->backend_core, display_geometry, 0 )
        OPTIONS_ADD( (*device)->backend_core, system_port, 0 )
        OPTIONS_ADD( (*device)->backend_core, host, 0 )
        OPTIONS_ADD( (*device)->backend_core, EDID_date, 0 )
        OPTIONS_ADD_INT( (*device)->backend_core, EDID_mnft_id )
        OPTIONS_ADD_INT( (*device)->backend_core, EDID_model_id )
        if(!error)
        {
          int  i;
          char * save_locale = 0;

          if(colors[0] != 0.0 && colors[1] != 0.0 && colors[2] != 0.0 &&
             colors[3] != 0.0 && colors[4] != 0.0 && colors[5] != 0.0 && 
             colors[6] != 0.0 && colors[7] != 0.0 && colors[8] != 0.0 )
          {
            STRING_ADD(t, registration);
            STRING_ADD(t, OY_SLASH );
            STRING_ADD(t, "color_matrix.from_edid."
                     "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma");
              
            for(i = 0; i < 9; ++i)
              error = oyOptions_SetFromDouble( &(*device)->data, t,
                                             colors[i], i, OY_CREATE_NEW );

            oyFree_m_( t );
          }
          else
          {
            oyMessageFunc_p( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_
                     "  No EDID matrix found; device_name: \"%s\"",OY_DBG_ARGS_,
                     oyNoEmptyString_m_( device_name ) );
            error = -1;
          }

          text = oyAllocateFunc_(1024);

          if(!error && text)
          {
            /* sensible printing */
            save_locale = oyStringCopy_( setlocale( LC_NUMERIC, 0 ),
                                         oyAllocateFunc_ );
            setlocale( LC_NUMERIC, "C" );
            sprintf( text, "%g", colors[0] );
            STRING_ADD(t, registration); STRING_ADD(t, OY_SLASH );
            STRING_ADD(t, "EDID_red_x" );
            error = oyOptions_SetFromText( &(*device)->backend_core, t,
                                         text, OY_CREATE_NEW );
            oyFree_m_(t);
            STRING_ADD(t, registration); STRING_ADD(t, OY_SLASH );
            STRING_ADD(t, "EDID_red_y" );
            sprintf( text, "%g", colors[1] );
            error = oyOptions_SetFromText( &(*device)->backend_core, t,
                                         text, OY_CREATE_NEW );
            oyFree_m_(t);
            STRING_ADD(t, registration); STRING_ADD(t, OY_SLASH );
            STRING_ADD(t, "EDID_green_x" );
            sprintf( text, "%g", colors[2] );
            error = oyOptions_SetFromText( &(*device)->backend_core, t,
                                         text, OY_CREATE_NEW );
            oyFree_m_(t);
            STRING_ADD(t, registration); STRING_ADD(t, OY_SLASH );
            STRING_ADD(t, "EDID_green_y" );
            sprintf( text, "%g", colors[3] );
            error = oyOptions_SetFromText( &(*device)->backend_core, t,
                                         text, OY_CREATE_NEW );
            oyFree_m_(t);
            STRING_ADD(t, registration); STRING_ADD(t, OY_SLASH );
            STRING_ADD(t, "EDID_blue_x" );
            sprintf( text, "%g", colors[4] );
            error = oyOptions_SetFromText( &(*device)->backend_core, t,
                                         text, OY_CREATE_NEW );
            oyFree_m_(t);
            STRING_ADD(t, registration); STRING_ADD(t, OY_SLASH );
            STRING_ADD(t, "EDID_blue_y" );
            sprintf( text, "%g", colors[5] );
            error = oyOptions_SetFromText( &(*device)->backend_core, t,
                                         text, OY_CREATE_NEW );
            oyFree_m_(t);
            STRING_ADD(t, registration); STRING_ADD(t, OY_SLASH );
            STRING_ADD(t, "EDID_white_x" );
            sprintf( text, "%g", colors[6] );
            error = oyOptions_SetFromText( &(*device)->backend_core, t,
                                         text, OY_CREATE_NEW );
            oyFree_m_(t);
            STRING_ADD(t, registration); STRING_ADD(t, OY_SLASH );
            STRING_ADD(t, "EDID_white_y" );
            sprintf( text, "%g", colors[7] );
            error = oyOptions_SetFromText( &(*device)->backend_core, t,
                                         text, OY_CREATE_NEW );
            oyFree_m_(t);
            STRING_ADD(t, registration); STRING_ADD(t, OY_SLASH );
            STRING_ADD(t, "EDID_gamma" );
            sprintf( text, "%g", colors[8] );
            error = oyOptions_SetFromText( &(*device)->backend_core, t,
                                         text, OY_CREATE_NEW );

            setlocale(LC_NUMERIC, save_locale);
            if(save_locale)
              oyFree_m_( save_locale );
          }
          oyDeAllocateFunc_( text ); text = 0;
        }
      }
  }

  return error;
}

int            oyDeviceFillEdid      ( const char        * registration,
                                       oyConfig_s       ** device_,
                                       oyPointer           edi,
                                       size_t              edi_size,
                                       const char        * device_name,
                                       const char        * host,
                                       const char        * display_geometry,
                                       const char        * system_port,
                                       oyOptions_s       * options )
{
  int error = !device_ || !edi;

  if(error <= 0)
  {
      char * EDID_manufacturer=0, * EDID_mnft=0, * EDID_model=0,
           * EDID_serial=0, * EDID_vendor = 0;
      double colors[9] = {0,0,0,0,0,0,0,0,0};
      uint32_t week=0, year=0, EDID_mnft_id=0, EDID_model_id=0;
      unsigned char hash[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
      uint32_t * i = (uint32_t*)&hash;
      char hash_txt[64];
      char* t = 0;
      oyConfig_s_ ** device = (oyConfig_s_**)device_;

      error = oyUnrollEdid1_( edi, &EDID_manufacturer, &EDID_mnft,
                      &EDID_model, &EDID_serial, &EDID_vendor,
                      &week, &year, &EDID_mnft_id, &EDID_model_id, colors,
                      oyAllocateFunc_);

      error = oyDeviceFillInfos( registration, device_,
                                   device_name, host, display_geometry,
                                   system_port,
                                   EDID_manufacturer, EDID_mnft, EDID_model,
                                   EDID_serial, EDID_vendor,
                                   week, year,
                                   EDID_mnft_id, EDID_model_id,
                                   colors, options );

      error = oyMiscBlobGetMD5_(edi, edi_size, hash);
      oySprintf_( hash_txt, "%08x%08x%08x%08x", i[0], i[1], i[2], i[3] );
      STRING_ADD(t, registration);
      STRING_ADD(t, OY_SLASH );
      STRING_ADD(t, "EDID_md5");
      error = oyOptions_SetFromText( &(*device)->backend_core, t,
                                     hash_txt, OY_CREATE_NEW );
  }

  return error;
}


/** @internal oyUnrollEdid1_ */
int          oyUnrollEdid1_          ( void              * edid,
                                       char             ** manufacturer,
                                       char             ** mnft,
                                       char             ** model,
                                       char             ** serial,
                                       char             ** vendor,
                                       uint32_t          * week,
                                       uint32_t          * year,
                                       uint32_t          * mnft_id,
                                       uint32_t          * model_id,
                                       double            * c,
                                       oyAlloc_f           allocate_func)
{
  int i, count = 0;
  XcmEdidKeyValue_s * list = 0;
  XCM_EDID_ERROR_e err = 0;

  DBG_PROG_START

  err = XcmEdidParse( edid, &list, &count );
  if(err)
    WARNc_S(XcmEdidErrorToString(err));

  if(list)
  for(i = 0; i < count; ++i)
  {
         if(manufacturer && oyStrcmp_( list[i].key, "manufacturer") == 0)
      *manufacturer = oyStringCopy_(list[i].value.text, allocate_func);
    else if(mnft && oyStrcmp_( list[i].key, "mnft") == 0)
      *mnft = oyStringCopy_(list[i].value.text, allocate_func);
    else if(model && oyStrcmp_( list[i].key, "model") == 0)
      *model = oyStringCopy_(list[i].value.text, allocate_func);
    else if(serial && oyStrcmp_( list[i].key, "serial") == 0)
      *serial = oyStringCopy_(list[i].value.text, allocate_func);
    else if(vendor && oyStrcmp_( list[i].key, "vendor") == 0)
      *vendor = oyStringCopy_(list[i].value.text, allocate_func);
    else if(week && oyStrcmp_( list[i].key, "week") == 0)
      *week = list[i].value.integer;
    else if(year && oyStrcmp_( list[i].key, "year") == 0)
      *year = list[i].value.integer;
    else if(mnft_id && oyStrcmp_( list[i].key, "mnft_id") == 0)
      *mnft_id = list[i].value.integer;
    else if(model_id && oyStrcmp_( list[i].key, "model_id") == 0)
      *model_id = list[i].value.integer;
    else if(c && oyStrcmp_( list[i].key, XCM_EDID_KEY_REDx) == 0)
      c[0] = list[i].value.dbl;
    else if(c && oyStrcmp_( list[i].key, XCM_EDID_KEY_REDy) == 0)
      c[1] = list[i].value.dbl;
    else if(c && oyStrcmp_( list[i].key, XCM_EDID_KEY_GREENx) == 0)
      c[2] = list[i].value.dbl;
    else if(c && oyStrcmp_( list[i].key, XCM_EDID_KEY_GREENy) == 0)
      c[3] = list[i].value.dbl;
    else if(c && oyStrcmp_( list[i].key, XCM_EDID_KEY_BLUEx) == 0)
      c[4] = list[i].value.dbl;
    else if(c && oyStrcmp_( list[i].key, XCM_EDID_KEY_BLUEy) == 0)
      c[5] = list[i].value.dbl;
    else if(c && oyStrcmp_( list[i].key, XCM_EDID_KEY_WHITEx) == 0)
      c[6] = list[i].value.dbl;
    else if(c && oyStrcmp_( list[i].key, XCM_EDID_KEY_WHITEy) == 0)
      c[7] = list[i].value.dbl;
    else if(c && oyStrcmp_( list[i].key, "gamma") == 0)
      c[8] = list[i].value.dbl;
  }

  XcmEdidFree( &list );

  DBG_PROG_ENDE
  return err;
}








