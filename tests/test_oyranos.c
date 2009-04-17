/** @file test_oyranos.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2004-2009  Kai-Uwe Behrmann
 *
 *  @brief    Oyranos test siute
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2008/12/04
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oyranos.h"


/* --- general test routines --- */

typedef enum {
  oyTESTRESULT_SUCCESS,
  oyTESTRESULT_FAIL,
  oyTESTRESULT_XFAIL,
  oyTESTRESULT_SYSERROR,
  oyTESTRESULT_UNKNOWN
} oyTESTRESULT_e;

int results[oyTESTRESULT_UNKNOWN+1];

const char * oyTestResultToString( oyTESTRESULT_e error )
{
  const char * text = "";
  switch(error)
  {
    case oyTESTRESULT_SUCCESS: text = "SUCCESS"; break;
    case oyTESTRESULT_FAIL:    text = "FAIL"; break;
    case oyTESTRESULT_XFAIL:   text = "XFAIL"; break;
    case oyTESTRESULT_SYSERROR:text = "SYSERROR"; break;
    case oyTESTRESULT_UNKNOWN: text = "UNKNOWN"; break;
    default:                   text = "Huuch, whats that?"; break;
  }
  return text;
}

oyTESTRESULT_e oyTestRun             ( oyTESTRESULT_e    (*test)(void),
                                       const char        * test_name )
{
  oyTESTRESULT_e error = 0;

  fprintf( stdout, "\n________________________________________________________________\n" );
  fprintf(stderr, "Test: %s ... ", test_name );

  error = test();

  fprintf(stderr, "\t%s", oyTestResultToString(error));

  results[error] += 1;

  /* print */
  if(error && error != oyTESTRESULT_XFAIL)
    fprintf(stderr, " !!! ERROR !!!" );
  fprintf(stderr, "\n" );

  return error;
}

#define PRINT_SUB( result_, ... ) { \
  if(result == oyTESTRESULT_XFAIL || \
     result == oyTESTRESULT_SUCCESS || \
     result == oyTESTRESULT_UNKNOWN ) \
    result = result_; \
  fprintf(stdout, ## __VA_ARGS__ ); \
  fprintf(stdout, " ..\t%s", oyTestResultToString(result_)); \
  if(result_ && result_ != oyTESTRESULT_XFAIL) \
    fprintf(stdout, " !!! ERROR !!!" ); \
  fprintf(stdout, "\n" ); \
}


/* --- actual tests --- */

oyTESTRESULT_e testVersion()
{
  char * vs = oyVersionString(2,0);
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );
  fprintf(stdout, "compiled version:     %d\n", OYRANOS_VERSION );
  fprintf(stdout, " runtime version:     %d\n", oyVersion(0) );
  fprintf(stdout, "compiled git version: %s\n", OYRANOS_GIT_MASTER );
  fprintf(stdout, " runtime git version: %s\n", vs ? vs : "---" );

  if(OYRANOS_VERSION == oyVersion(0))
    result = oyTESTRESULT_SUCCESS;
  else
    result = oyTESTRESULT_FAIL;

  if(!result && vs && strlen(OYRANOS_GIT_MASTER))
    if(strcmp(OYRANOS_GIT_MASTER, vs?vs:"quark") != 0)
      result = oyTESTRESULT_FAIL;

  return result;
}

#include <locale.h>
#include "oyranos_sentinel.h"

oyTESTRESULT_e testI18N()
{
  const char * lang = 0;
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  oyI18Nreset();

  lang = oyLanguage();
  if(lang && strcmp(lang, "C") == 0)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyLanguage() uninitialised good %s                ", lang );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyLanguage() uninitialised failed                 " );
  }

  setlocale(LC_ALL,"");
  oyI18Nreset();

  lang = oyLanguage();
  if(strcmp(lang, "C") != 0)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyLanguage() initialised good %s                  ", lang );
  } else
  { PRINT_SUB( oyTESTRESULT_XFAIL, 
    "oyLanguage() initialised failed %s                ", lang );
  }

  return result;
}

#include "oyranos_elektra.h"
oyTESTRESULT_e testElektra()
{
  int error = 0;
  char * value = 0,
       * start = 0;
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  oyExportReset_(EXPORT_SETTING);

  fprintf(stdout, "\n" );

  error = oyAddKey_valueComment_("sw/Oyranos/Tests/test_key",
                                 "NULLTestValue", "NULLTestComment" );
  start = oyGetKeyString_("sw/Oyranos/Tests/test_key", 0);
  if(!start)
  {
    oyExportStart_(EXPORT_CHECK_NO);
    oyExportEnd_();
    error = oyAddKey_valueComment_("sw/Oyranos/Tests/test_key",
                                 "NULLTestValue", "NULLTestComment" );
    start = oyGetKeyString_("sw/Oyranos/Tests/test_key", 0);
    
    PRINT_SUB( start?oyTESTRESULT_SUCCESS:oyTESTRESULT_XFAIL,
    "Elektra not initialised? try oyExportStart_(EXPORT_CHECK_NO)" );
  }
  if(!start)
  {
    oyExportStart_(EXPORT_SETTING);
    oyExportEnd_();
    error = oyAddKey_valueComment_("sw/Oyranos/Tests/test_key",
                                 "NULLTestValue", "NULLTestComment" );
    start = oyGetKeyString_("sw/Oyranos/Tests/test_key", 0);
    PRINT_SUB( start?oyTESTRESULT_SUCCESS:oyTESTRESULT_XFAIL, 
    "Elektra not initialised? try oyExportStart_(EXPORT_SETTING)" );
  }
  if(start)
    fprintf(stdout, "start key value: %s\n", start );
  else
    fprintf(stdout, "could not initialise\n" );

  error = oyAddKey_valueComment_("sw/Oyranos/Tests/test_key",
                                 "myTestValue", "myTestComment" );
  value = oyGetKeyString_("sw/Oyranos/Tests/test_key", 0);
  if(value)
    fprintf(stdout, "result key value: %s\n", value );

  if(error)
  {
    PRINT_SUB( oyTESTRESULT_SYSERROR, 
    "Elektra error: %d", error );
  } else
  if(start && value && strcmp(start,value) == 0)
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "Elektra (start!=value) failed: %s|%s", start, value );
  } else
  if(!value)
  {
    if(!value)
      PRINT_SUB( oyTESTRESULT_FAIL, 
      "Elektra (value) failed" );
    if(!start)
      PRINT_SUB( oyTESTRESULT_FAIL, 
      "Elektra (init) failed" );
  } else
  if(value)
  {
    if(strcmp(value,"myTestValue") == 0)
    {
      PRINT_SUB( oyTESTRESULT_SUCCESS, 
      "Elektra (value): %s", value );
    } else
    {
      PRINT_SUB( oyTESTRESULT_FAIL, 
      "Elektra (value) wrong: %s", value );
    }
  } else
    result = oyTESTRESULT_SUCCESS;

  if(start)
    oyDeAllocateFunc_(start);
  if(value)
    oyDeAllocateFunc_(value);

  return result;
}


#include <oyranos_alpha.h>

oyTESTRESULT_e testOption ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  int error = 0;
  oyOption_s * o;
  const char * test_buffer = "test";
  size_t size = strlen(test_buffer);
  oyPointer ptr = oyAllocateFunc_( size );

  oyExportReset_(EXPORT_SETTING);

  fprintf(stdout, "\n" );

  o = oyOption_New( "blabla", 0 );
  if(!o)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_New() with wrong registration rejected: ok");
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_New() with wrong registration not rejected");
  }

  o = oyOption_New( 0, 0 );
  if(o)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_New() good                               " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_New() failed                             " );
  }

  memcpy( ptr, test_buffer, size );
  error = oyOption_SetFromData( o, ptr, size );
  if(!error)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromData() good                       " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromData() failed                     " );
  }

  oyDeAllocateFunc_( ptr ); ptr = 0;
  size = 0;

  ptr = oyOption_GetData( o, &size, oyAllocateFunc_ );
  if(ptr && size && memcmp( ptr, test_buffer, 4 ) == 0)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_GetData() good                           " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_GetData() failed                         " );
  }

  oyOption_Release( &o );
  oyDeAllocateFunc_( ptr ); ptr = 0;
  size = 0;

  return result;
}


#include <libxml/parser.h>
#include <libxml/xmlsave.h>

oyTESTRESULT_e testSettings ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  int i;
  uint32_t size = 0;
  int count = 0,
      countB = 0;
  char * text = 0, * tmp = 0;
  int error = 0;
  oyOptions_s * opts = 0;
  oyOption_s * o;
  /*xmlSaveCtxtPtr ptr;
  xmlBufferPtr buf;*/
  xmlDocPtr doc = 0;

  oyExportReset_(EXPORT_SETTING);

  fprintf(stdout, "\n" );

  /* we check for out standard CMM */
  opts = oyOptions_ForFilter( "//colour", "lcms",
                                            oyOPTIONATTRIBUTE_ADVANCED /* |
                                            oyOPTIONATTRIBUTE_FRONT |
                                            OY_OPTIONSOURCE_META */, 0 );

  oyOptions_SetFromText( &opts, "cmyk_cmky_black_preservation", "1", 0 );

  count = oyOptions_Count( opts );
  if(!count)
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "No options found for lcms" );
  } else
  {
    for(i = 0; i < count; ++i)
    {
      o = oyOptions_Get( opts, i );
      tmp = oyOption_GetValueText( o, 0 );
      fprintf(stdout, "%s:", tmp );
      oyDeAllocateFunc_(tmp);

      tmp = oyFilterRegistrationToText( oyOption_GetText(o, oyNAME_DESCRIPTION),
                                        oyFILTER_REG_OPTION, 0 );
      if(strcmp( oyNoEmptyString_m_(tmp),"cmyk_cmky_black_preservation") == 0)
        ++size;

      oyDeAllocateFunc_(tmp);
      oyOption_Release( &o );
    }

    if(!size)
    {
      PRINT_SUB( oyTESTRESULT_FAIL, 
      "oyOptions_FindString() could not find option" );
    } else
    if(size > 1)
    {
      PRINT_SUB( oyTESTRESULT_FAIL, 
      "oyOptions_FindString() returned doubled options %d",
                       size );
    } else
      PRINT_SUB( oyTESTRESULT_SUCCESS, 
      "oyOptions_FindString() returned one option" );
  }

  /*ptr = xmlSaveToBuffer( buf, 0, 0 );*/


  text = oyStringCopy_(oyOptions_GetText( opts, oyNAME_NAME ), oyAllocateFunc_);

  {
    if(!text || !strlen(text))
    {
      PRINT_SUB( oyTESTRESULT_FAIL, 
      "oyOptions_GetText() returned no text             " );
    } else
    {
      PRINT_SUB( oyTESTRESULT_SUCCESS, 
      "oyOptions_GetText() returned text               %d", strlen(text) );
    }
  }
  oyOptions_Release( &opts );


  /* Roundtrip test */
  opts = oyOptions_FromText( text, 0, 0 );
  countB = oyOptions_Count( opts );

  {
    if(count == countB)
    {
      PRINT_SUB( oyTESTRESULT_SUCCESS, 
      "obtained same count from oyOptions_FromText %d|%d  ",
                      count, countB );
    } else
    {
      PRINT_SUB( oyTESTRESULT_FAIL, 
      "oyOptions_FromText() returned different count %d|%d",
                      count, countB );
    }
  }
  
  for( i = 0; i < countB; ++i)
  {
    o = oyOptions_Get( opts, i );
    /*printf("%d: \"%s\": \"%s\" %s %d\n", i, 
           oyOption_GetText( o, oyNAME_DESCRIPTION ), o->value->string,
           oyFilterRegistrationToText( oyOption_GetText( o, oyNAME_DESCRIPTION),
                                       oyFILTER_REG_OPTION, 0 ),
           o->flags );*/

    oyOption_Release( &o );
  }

  tmp = oyStringAppend_( "<a>\n", text, 0 );
  oyStringAdd_( &tmp, "</a>", 0, 0 );
  oyDeAllocateFunc_(text); text = tmp; tmp = 0;

  doc = xmlParseMemory( text, oyStrlen_( text ) );
  error = !doc;
  {
    if(!doc)
    {
      PRINT_SUB( oyTESTRESULT_FAIL, 
      "libxml2::xmlParseMemory() returned could not parse the document" );
    } else
    {
      PRINT_SUB( oyTESTRESULT_SUCCESS, 
      "libxml2 returned document                        " );
    }
  }

  oyDeAllocateFunc_(text); text = 0;
  xmlDocDumpFormatMemory( doc, &text, &i, 1 );
  oyDeAllocateFunc_(text); text = 0;
  /*xmlSaveDoc( ptr, doc );*/
  xmlFreeDoc( doc ); doc = 0;

  oyOptions_Release( &opts );

  return result;
}

oyTESTRESULT_e testProfiles ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  int i;
  uint32_t size = 0;
  int current = -1;
  int count = 0,
      countB = 0;
  char ** texts = 0;
  const char * tmp = 0;
  oyProfiles_s * profs = 0;
  oyProfile_s * p;

  oyExportReset_(EXPORT_SETTING);

  fprintf(stdout, "\n" );

  /* compare the usual conversion profiles with the total of profiles */
  profs = oyProfiles_ForStd( oyDEFAULT_PROFILE_START, &current, 0 );
  count = oyProfiles_Count( profs );
  if(!count)
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "No profiles found for oyDEFAULT_PROFILE_START" );
  } else
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "profiles found for oyDEFAULT_PROFILE_START: %d (%d)", count, current );
  }
  oyProfiles_Release( &profs );

  texts = oyProfileListGet( 0, &size, 0 );
  if(!size)
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "No profiles found for oyProfileListGet()" );
  } else
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "profiles found for oyProfileListGet:        %d", size );
  }

  if(size < count)
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyProfileListGet() returned less than oyDEFAULT_PROFILE_START %d|%d", size, count );
  } else if(count)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyProfileListGet and oyDEFAULT_PROFILE_START ok %d|%d", size, count );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oyProfileListGet and/or oyDEFAULT_PROFILE_START are wrong %d|%d", size, count );
  }

  /* compare the default profile spaces with the total of profiles */
  countB = 0;
  for(i = oyEDITING_XYZ; i <= oyEDITING_GRAY; ++i)
  {
    profs = oyProfiles_ForStd( (oyPROFILE_e)i, &current, 0 );

    count = oyProfiles_Count( profs );
    countB += count;
    if(!count)
    {
      PRINT_SUB( oyTESTRESULT_FAIL, 
      "No profiles found for oyPROFILE_e %d             ", i );
    } else
    {
      p = oyProfiles_Get( profs, current );
      tmp = oyProfile_GetText( p, oyNAME_DESCRIPTION );
      PRINT_SUB( oyTESTRESULT_SUCCESS, 
      "profiles found for oyPROFILE_e %d: %d \"%s\"", i, count, tmp ? tmp :"");
    }

    oyProfiles_Release( &profs );
  }
  if(size < countB)
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyProfileListGet() returned less than oyPROFILE_e %d|%d", size, count );
  } else
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyProfileListGet and oyPROFILE_e ok %d|%d", size, countB );
  }


  return result;
}

oyTESTRESULT_e testMonitor ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  int n, i, error = 0;
  char * block, * text = 0, * display_name;
  const char * tmp = 0;
  size_t size = 0;
  oyProfile_s * p = 0, * p2;
  oyConfigs_s * devices = 0;
  oyConfig_s * c = 0;
  oyOptions_s * options = 0;
  oyOption_s * o = 0;
  oyRectangle_s * r = 0;

  oyExportReset_(EXPORT_SETTING);
  fprintf(stdout, "\n" );

  /* get all monitors */
  error = oyDevicesGet( 0, "monitor", 0, &devices );
  /* see how many are included */
  n = oyConfigs_Count( devices );
  if(n)
    PRINT_SUB( oyTESTRESULT_SUCCESS,
               "\"list\" device(s): %d                    ", n )
  else
    PRINT_SUB( oyTESTRESULT_XFAIL,
               "\"list\" device(s): ---                   " )

  for( i = 0; i < n; ++i )
  {
    c = oyConfigs_Get( devices, i );
    oyDeviceGetProfile( c, &p );

    if(p)
    {
      tmp = oyProfile_GetText( p, oyNAME_DESCRIPTION );
    PRINT_SUB( oyTESTRESULT_SUCCESS,
               "device[%d]: \"%s\"              ", i,
               tmp ? tmp :"" );
    } else
    {
    PRINT_SUB( oyTESTRESULT_FAIL,
               "device[%d]: no profile                             ", i );
    }

    oyProfile_Release( &p );
    oyConfig_Release( &c );
  }
  /* release devices */
  oyConfigs_Release( &devices );


  /* get all monitors */
  error = oyOptions_SetFromText( &options, "//colour/config/list",
                                 "true", OY_CREATE_NEW );
  error = oyOptions_SetFromText( &options, "//colour/config/device_rectangle",
                                 "true", OY_CREATE_NEW );
  error = oyDevicesGet( 0, "monitor", options, &devices );
  oyOptions_Release( &options );
  /* see how many are included */
  n = oyConfigs_Count( devices );
  if(n)
    PRINT_SUB( oyTESTRESULT_SUCCESS,
               "\"list\" + device_rectangle device(s): %d    ", n )
  else
    PRINT_SUB( oyTESTRESULT_XFAIL,
               "\"list\" device(s): ---                   " )

  for( i = 0; i < n; ++i )
  {
    c = oyConfigs_Get( devices, i );
    o = oyConfig_Find( c, "device_rectangle" );
    r = (oyRectangle_s *) oyOption_StructGet( o, oyOBJECT_RECTANGLE_S );

    if(r)
    {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
               "device[%d]: %s    ", i,
               oyRectangle_Show(r) );
    } else
    {
    PRINT_SUB( oyTESTRESULT_FAIL,
               "device[%d]: no rectangle                          ", i );
    }

    oyRectangle_Release( &r );
    oyOption_Release( &o );
    oyConfig_Release( &c );
  }
  /* release devices */
  oyConfigs_Release( &devices );


  error = oyOptions_SetFromText( &options, "//colour/config/properties",
                                 "true", OY_CREATE_NEW );

  error = oyDevicesGet( "colour", "monitor", options, &devices );
  oyOptions_Release( &options );

  n = oyConfigs_Count( devices );
  if(n)
    PRINT_SUB( oyTESTRESULT_SUCCESS,
               "\"properties\" device(s): %d              ", n )
  else
    PRINT_SUB( oyTESTRESULT_XFAIL,
               "\"properties\" device(s): ---             " )

  if(!error)
  {
    for(i = 0; i < n; ++i)
    {
      c = oyConfigs_Get( devices, i );

      error = oyDeviceGetInfo( c, oyNAME_NAME, 0, &text, 0 );

      if(text && text[0])
        PRINT_SUB( oyTESTRESULT_SUCCESS, "device: %s", text)
      else
        PRINT_SUB( oyTESTRESULT_XFAIL, "device: ---" )

      if(text)
        free( text );

      error = oyDeviceGetInfo( c, oyNAME_DESCRIPTION, 0, &text, 0 );

      if(text && text[0])
      {
        char * list = text, * tmp = 0, * line = malloc(128);
        int even = 1;

        PRINT_SUB( oyTESTRESULT_SUCCESS, "device    " )

        tmp = list;
        while(list && list[0])
        {
          snprintf( line, 128, "%s", list );
          if(strchr( line, '\n' ))
          {
            tmp = strchr( line, '\n' );
            tmp[0] = 0;
          }
          if(even)
            printf( "%s\n", line );
          else
            printf( "  %s\n", line );
          list = strchr( list, '\n' );
          if(list) ++list;
          even = !even;
        }
        free( line );

      } else
        PRINT_SUB( oyTESTRESULT_XFAIL, "device: ---" )

      if(text)
        free( text );


      /* get the old oyMonitorxxx API conforming display name */
      error = oyDeviceGetInfo( c, oyNAME_NICK, 0, &text, 0 );

      if(text && text[0])
        PRINT_SUB( oyTESTRESULT_SUCCESS, "device: %s", text )
      else
        PRINT_SUB( oyTESTRESULT_XFAIL, "device: ---" )

      display_name = text;

      size = 0;
      block = oyGetMonitorProfile( display_name, &size, malloc );
      p = oyProfile_FromMem( size, block, 0,0 );

      if(block)
      {
        PRINT_SUB( oyTESTRESULT_SUCCESS,
        "monitor profile from server \"%s\" %d",
        oyProfile_GetText( p, oyNAME_DESCRIPTION ), size );
      } else
      {
        PRINT_SUB( oyTESTRESULT_SUCCESS,
        "no default monitor profile %d", size );
      }

      error = oyDeviceProfileFromDB( c, &text, malloc );
      if(display_name) free(display_name);
      if(text)
      {
        PRINT_SUB( oyTESTRESULT_SUCCESS,
        "monitor profile from Oyranos DB %s", text );
      } else
      {
        PRINT_SUB( oyTESTRESULT_XFAIL,
        "no monitor profile from Oyranos DB" );
      }

      p2 = oyProfile_FromFile( text, 0, 0 );

      if(text &&
         strcmp( oyNoEmptyString_m_(oyProfile_GetText( p2,oyNAME_DESCRIPTION )),
                 oyNoEmptyString_m_(oyProfile_GetText( p ,oyNAME_DESCRIPTION )))
         == 0)
      {
        PRINT_SUB( oyTESTRESULT_SUCCESS,
        "monitor profile from Oyranos DB matches the server one" );
      } else
      {
        PRINT_SUB( oyTESTRESULT_XFAIL,
        "monitor profile from Oyranos DB differs from the server one" );
      }

      if(text)
        free( text );

      oyConfig_Release( &c );
      fprintf(stdout, "\n" );
    }
  }
  oyConfigs_Release( &devices );

  display_name = oyGetDisplayNameFromPosition( 0, 0,0, malloc);

  return result;
}

oyTESTRESULT_e testRegistrationMatch ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  if( oyFilterRegistrationMatch("sw/oyranos.org/colour/icc.lcms",
                                "//colour/icc",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "simple CMM selection                  " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "simple CMM selection                  " );
  }

  if(!oyFilterRegistrationMatch("sw/oyranos.org/colour/icc.lcms",
                                "//colour/icc.octl",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "simple CMM selection no match         " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "simple CMM selection no match         " );
  }

  if( oyFilterRegistrationMatch("sw/oyranos.org/colour/icc.lcms",
                                "//colour/icc.4+lcms",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "special CMM selection                 " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "special CMM selection                 " );
  }

  if(!oyFilterRegistrationMatch("sw/oyranos.org/colour/icc.lcms",
                                "//colour/icc.4-lcms",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "special CMM avoiding                  " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "special CMM avoiding                  " );
  }

  if( oyFilterRegistrationMatch("sw/oyranos.org/colour/icc.lcms",
                                "//colour/icc.7-lcms",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "special CMM avoiding, other API       " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "special CMM avoiding, other API       " );
  }

  if( oyFilterRegistrationMatch("sw/oyranos.org/colour/icc.lcms",
                                "lcms", 0 ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "select by keyname                     " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "select by keyname                     " );
  }

  return result;
}


#define TEST_RUN( prog, text ) { \
  if(argc > 1) { \
    if(strcmp("-l", argv[1]) == 0) \
    { \
      printf( "%s\n", text); \
    } else { \
      for(i = 1; i < argc; ++i) \
        if(strstr(text, argv[i]) != 0) \
          oyTestRun( prog, text ); \
    } \
  } else \
    oyTestRun( prog, text ); \
}

/*  main */
int main(int argc, char** argv)
{
  int i, error;

  /* init */
  for(i = 0; i <= oyTESTRESULT_UNKNOWN; ++i)
    results[i] = 0;

  fprintf( stderr, "\nOyranos Tests v" OYRANOS_VERSION_NAME
           "  developed: " OYRANOS_DATE  "\n   git id:" OYRANOS_GIT_MASTER
           "  configured: " OYRANOS_CONFIG_DATE "\n\n" );


  /* do tests */

  TEST_RUN( testVersion, "Version matching" );
  TEST_RUN( testI18N, "Internationalisation" );
  TEST_RUN( testElektra, "Elektra" );
  TEST_RUN( testOption, "basic oyOption_s" );
  TEST_RUN( testSettings, "default oyOptions_s settings" );
  TEST_RUN( testProfiles, "Profiles reading" );
  TEST_RUN( testMonitor,  "Monitor profiles" );
  TEST_RUN( testRegistrationMatch,  "Registration matching" );

  /* give a summary */

  if(!(argc > 1 &&  
       strcmp("-l", argv[1]) == 0))
  {

    fprintf( stderr, "\n################################################################\n" );
    fprintf( stderr, "#                                                              #\n" );
    fprintf( stderr, "#                     Results                                  #\n" );
    for(i = 0; i <= oyTESTRESULT_UNKNOWN; ++i)
      fprintf( stderr, "    Tests with status %s: %d\n",
                       oyTestResultToString( (oyTESTRESULT_e)i ), results[i] );

    error = (results[oyTESTRESULT_FAIL] ||
             results[oyTESTRESULT_SYSERROR] ||
             results[oyTESTRESULT_UNKNOWN]
            );

    if(error)
      fprintf( stderr, "    Tests FAILED\n" );
    else
      fprintf( stderr, "    Tests SUCCEEDED\n" );

    fprintf( stderr, "\n    Hint: the '-l' option will list all test names\n" );

  }

  oyFinish_(0);

  return error;
}

