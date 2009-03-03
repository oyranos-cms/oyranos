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


/* C++ includes and definitions */
void* myAllocFunc(size_t size) { return new char [size]; }
#include <fstream>
#include <iostream>
using namespace oyranos;


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
  oyTESTRESULT_e error = oyTESTRESULT_UNKNOWN;

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

  o = oyOption_New( 0, 0 );
  if(o)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_New() good                               " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_New() failed                             " );
  }

  error = oyOption_New( "blabla", 0 ) != 0;
  if(o)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_New() with wrong registration rejected: ok");
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_New() with wrong registration not rejected");
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
  xmlSaveCtxtPtr ptr;
  xmlBufferPtr buf;
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

  text = oyStringAppend_( "<a>\n", text, 0 );
  oyStringAdd_( &text, "</a>", 0, 0 );

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

  xmlDocDumpFormatMemory( doc, (xmlChar**)&text, &i, 1 );
  /*xmlSaveDoc( ptr, doc );*/

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

  if((int)size < count)
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
  if((int)size < countB)
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


oyTESTRESULT_e testProfileLists ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  double start_time = DBG_UHR_;

  std::cout << "Start: " << start_time << std::endl;

  uint32_t ref_count = 0;
  char ** reference = oyranos::oyProfileListGet(0, &ref_count, myAllocFunc);

  if((int)ref_count)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyProfileListGet() returned profiles %d", (int)ref_count );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oyProfileListGet() zero" );
  }

  for(int i = 0; i < 1000; ++i)
  {
    uint32_t count = 0;
    char ** names = oyranos::oyProfileListGet(0, &count, myAllocFunc);
    if(count != ref_count)
    {
      PRINT_SUB( oyTESTRESULT_FAIL, 
      ": wrong profile count: %d/%d", count, ref_count );
    }
    for(int j = 0; j < (int)count; ++j)
    {
      if(!(names[j] && strlen(names[j])) ||
         strcmp( names[j], reference[j] ) != 0 )
      {
        PRINT_SUB( oyTESTRESULT_FAIL, 
        "\n no profile name found: run %d profile #%d", i , j );
      }
      if( names[j] ) delete [] names[j];
    }
    if( names ) delete [] names;
    std::cout << "." << std::flush;
  }

  double end = DBG_UHR_;

  std::cout << std::endl;
  std::cout << "1000 + 1 calls to oyProfileListGet() took: "<< end - start_time
            << " seconds" << std::endl;

  return result;
}


oyTESTRESULT_e testMonitor ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  int n, i, error = 0;
  char * block, * text, * display_name;
  size_t size = 0;
  oyProfile_s * p, * p2;
  oyConfigs_s * devices = 0;
  oyConfig_s * c = 0;

  oyExportReset_(EXPORT_SETTING);
  fprintf(stdout, "\n" );


  error = oyDevicesGet( "colour", "monitor", 0, &devices );
  n = oyConfigs_Count( devices );
  if(!error)
  {
    for(i = 0; i < n; ++i)
    {
      c = oyConfigs_Get( devices, i );

      error = oyDeviceGetInfo( c, oyNAME_NICK, 0, &text, 0 );

      if(text && text[0])
        PRINT_SUB( oyTESTRESULT_SUCCESS, "device: %s", text )
      else
        PRINT_SUB( oyTESTRESULT_XFAIL, "device: ---" )

      if(text)
        free( text );

      error = oyDeviceGetInfo( c, oyNAME_NAME, 0, &text, 0 );

      if(text && text[0])
        PRINT_SUB( oyTESTRESULT_SUCCESS, "device: %s", text )
      else
        PRINT_SUB( oyTESTRESULT_XFAIL, "device: ---" )

      if(text)
        free( text );

      error = oyDeviceGetInfo( c, oyNAME_DESCRIPTION, 0, &text, 0 );

      if(text && text[0])
        PRINT_SUB( oyTESTRESULT_SUCCESS, "device:\n%s", text )
      else
        PRINT_SUB( oyTESTRESULT_XFAIL, "device: ---" )

      if(text)
        free( text );

      oyConfig_Release( &c );
    }
  }
  oyConfigs_Release( &devices );

  display_name = oyGetDisplayNameFromPosition( 0, 0,0, malloc);
  block = oyGetMonitorProfile( display_name, &size, malloc );
  p = oyProfile_FromMem( size, block, 0,0 );

  if(block)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "monitor profile from server \"%s\" %d \"%s\"", oyProfile_GetText( p, oyNAME_DESCRIPTION ), size, display_name );
  } else
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "no default monitor profile %d \"%s\"", size, display_name );
  }

  text = oyGetMonitorProfileNameFromDB( display_name, malloc );
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
     strcmp( oyNoEmptyString_m_(oyProfile_GetText( p2, oyNAME_DESCRIPTION )),
             oyNoEmptyString_m_(oyProfile_GetText( p , oyNAME_DESCRIPTION )))
     == 0)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "monitor profile from Oyranos DB matches the server one" );
  } else
  {
    PRINT_SUB( oyTESTRESULT_XFAIL,
    "no monitor profile from Oyranos DB differs from the server one" );
  }


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

  return result;
}


oyTESTRESULT_e testPolicy ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;
  char *data = 0;

  fprintf(stdout, "\n" );

#if 0
  if(argc > 1)
  {
    printf("%s\n", argv[1]);
    std::ifstream f( argv[1], std::ios::binary | std::ios::ate);
    if(f.good())
    {
      size_t size = f.tellg();
      f.seekg(0);
      if(size) {
        data = (char*) new char [size+1];
        f.read ((char*)data, size);
        f.close();
        std::cout << "Opened file: " << argv[1] << std::endl;
      }
    }
  }
#endif

  char *xml = data;
  if( !xml)
    xml = oyranos::oyPolicyToXML( oyranos::oyGROUP_ALL, 1, myAllocFunc );

  if( xml && xml[0] )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyranos::oyPolicyToXML                " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyranos::oyPolicyToXML                " );
  }

  if(xml) {
    oyranos::oyReadXMLPolicy(oyranos::oyGROUP_ALL, xml);
    printf("xml text: \n%s", xml);

    data = oyranos::oyPolicyToXML( oyranos::oyGROUP_ALL, 1, myAllocFunc );

    if( strcmp( data, xml ) == 0 )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "Policy rereading                      " );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
      "Policy rereading                      " );
    }

    delete [] xml;
    delete [] data;
  }


  return result;
}

/* forward declaration for oyranos_alpha.c */
extern "C" {
char ** oyCMMsGetLibNames_           ( uint32_t          * n,
                                       const char        * required_cmm );
}

oyTESTRESULT_e testCMMConfigsListing ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int i, j, k, l, j_n, k_n;
  uint32_t count = 0,
         * rank_list = 0;
  int error = 0;
  char ** texts = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif

  texts = oyCMMsGetLibNames_( &count, 0 );

  if( count )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyCMMsGetLibNames_ Found CMM's %d     ", (int)count );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyCMMsGetLibNames_ Found CMM's %d     ", (int)count );
  }
  for( i = 0; i < (int)count; ++i)
  {
    fprintf( stdout, "%d: %s\n", i, texts[i] );
  }
  fprintf(stdout, "\n" );

  oyConfigs_s * configs = 0;
  oyConfig_s * config = 0,
             * device = 0;
  oyOptions_s * options_list = 0,
              * options = 0,
              * options_devices = 0;
  oyOption_s * o = 0;
  int devices_n = 0;

  error = oyConfigDomainList  ( "//colour", &texts, &count, &rank_list, 0 );
  if( count )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfigDomainList \"%s\": %d         ", "//colour", (int)count );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfigDomainList \"%s\": %d         ", "//colour", (int)count );
  }
  for( i = 0; i < (int)count; ++i)
  {
    fprintf( stdout, "%d: %s\n", i, texts[i] );
  }
  fprintf( stdout, "\n");

  /* send a empty query to one backend to obtain instructions in a message */
  error = oyConfigs_FromDomain( texts[0], 0, &configs, 0 );
  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfigs_FromDomain \"%s\" help text ", texts[0] );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfigs_FromDomain \"%s\" help text ", texts[0] );
  }
  fprintf( stdout, "\n");


  /* add list call to backend arguments */
  error = oyOptions_SetFromText( &options_list, "//colour/config/list", "true",
                                 OY_CREATE_NEW );

  fprintf( stdout, "oyConfigs_FromDomain() \"list\" call:\n" );
  for( i = 0; i < (int)count; ++i)
  {
    const char * registration_domain = texts[i];
    printf("%d[rank %d]: %s\n", i, rank_list[i], registration_domain);

    error = oyConfigs_FromDomain( registration_domain,
                                       options_list, &configs, 0 );
    j_n = oyConfigs_Count( configs );
    for( j = 0; j < j_n; ++j )
    {
      config = oyConfigs_Get( configs, j );

      k_n = oyConfig_Count( config );
      for( k = 0; k < k_n; ++k )
      {
        o = oyConfig_Get( config, k );

        /* collect the device_name's into a set of options for later */
        error = oyOptions_SetFromText( &options_devices, o->registration,
                                       o->value->string,
                                       OY_CREATE_NEW | OY_ADD_ALWAYS );
        printf("  %d::%d::%d %s %s\n", i,j,k,
               o->registration, o->value->string );

        oyOption_Release( &o );
      }

      oyConfig_Release( &config );
    }

    oyConfigs_Release( &configs );
  }
  fprintf( stdout, "\n");

  fprintf( stdout, "oyConfigs_FromDomain() \"properties\" call:\n" );
  devices_n = oyOptions_Count( options_devices );
  for( i = 0; i < (int)count; ++i)
  {
    const char * registration_domain = texts[i];
    printf("%d[rank %d]: %s\n", i, rank_list[i], registration_domain);

    for( l = 0; l < devices_n; ++l )
    {
      /* set a general request */
      error = oyOptions_SetFromText( &options, "//colour/config/properties",
                                     "true", OY_CREATE_NEW );

      /* set the device_name */
      o = oyOptions_Get( options_devices, l );
      error = oyOptions_SetFromText( &options, o->registration,
                                     o->value->string, OY_CREATE_NEW );
      oyOption_Release( &o );

      /* send the query to a backend */
      error = oyConfigs_FromDomain( registration_domain,
                                       options, &configs, 0 );
      /* display results */
      j_n = oyConfigs_Count( configs );
      for( j = 0; j < j_n; ++j )
      {
        config = oyConfigs_Get( configs, j );

        if(l == 0 && j == 0)
          device = oyConfig_Copy( config, 0 );

        k_n = oyConfig_Count( config );
        for( k = 0; k < k_n; ++k )
        {
          o = oyConfig_Get( config, k );

          printf( "  %d::%d::%d::%d %s %s\n", i,l,j,k, 
                  o->registration, o->value->string );

          oyOption_Release( &o );
        }

        //error = oyConfig_SaveToDB( config );
        oyConfig_Release( &config );
      }

      oyConfigs_Release( &configs );
    }
  }

  oyOptions_Release( &options_list );
  oyOptions_Release( &options );
  fprintf( stdout, "\n");

  config = oyConfig_New( texts[0], 0 );
  error = oyConfig_AddDBData( config, "k1", "bla1", OY_CREATE_NEW );
  error = oyConfig_AddDBData( config, "k2", "bla2", OY_CREATE_NEW );
  error = oyConfig_AddDBData( config, "k3", "bla3", OY_CREATE_NEW );

  if( !error  && oyOptions_Count( config->db ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfig_AddDBData                    " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfig_AddDBData                    " );
  }
  fprintf( stdout, "\n");

  return result;
}

oyTESTRESULT_e testCMMMonitorListing ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int i, k, k_n;
  int error = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif

  oyConfigs_s * configs = 0;
  oyConfig_s * config = 0;
  oyOption_s * o = 0;
  int devices_n = 0;
  char * text = 0;

  error = oyDevicesGet( 0, "monitor", 0, &configs );
  devices_n = oyConfigs_Count( configs );
  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyDevicesGet() \"monitor\": %d     ", devices_n );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyDevicesGet() \"monitor\": %d     ", devices_n );
  }
  for( i = 0; i < devices_n; ++i )
  {
    config = oyConfigs_Get( configs, i );
    printf( "  %d oyConfig_FindString(..\"device_name\"..): %s\n", i,
            oyConfig_FindString( config, "device_name",0 ) );

    error = oyDeviceProfileFromDB( config, &text, myAllocFunc );
    if(text)
      fprintf( stdout, "  %d oyDeviceProfileFromDB(): %s\n", i, text );
    else
      fprintf( stdout, "  %d oyDeviceProfileFromDB(): ---\n", i );

    error = oyDeviceGetInfo( config, oyNAME_NICK, 0, &text, 0 );
    fprintf( stdout, "  %d oyDeviceGetInfo)(..oyNAME_NICK..): \"%s\"\n",
             i, text? text:"???");
    error = oyDeviceGetInfo( config, oyNAME_NAME, 0, &text, 0 );
    fprintf( stdout, "  %d oyDeviceGetInfo)(..oyNAME_NAME..): \"%s\"\n",
             i, text? text:"???");

    oyConfig_Release( &config );
  }
  oyConfigs_Release( &configs );
  fprintf( stdout, "\n");

  error = oyDeviceGet( 0, "monitor", ":0.1", 0, &config );
  k_n = oyConfig_Count( config );
  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyDeviceGet(..\"monitor\" \":0.1\"..) %d     ", k_n );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyDeviceGet(..\"monitor\" \":0.1\"..) %d     ", k_n );
  }
    for( k = 0; k < k_n; ++k )
    {
      o = oyConfig_Get( config, k );

      printf( "  %d %s: \"%s\"\n", k,
              o->registration, o->value->string );

      oyOption_Release( &o );
    }
  oyConfig_Release( &config );
  oyConfigs_Release( &configs );
  fprintf( stdout, "\n");


  return result;
}

oyTESTRESULT_e testCMMDBListing ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int j, k, j_n, k_n;
  int error = 0;
  oyConfigs_s * configs = 0;
  oyConfig_s * config = 0;
  oyOption_s * o = 0;

  error = oyConfigs_FromDB( "//colour", &configs, 0 );
  j_n = oyConfigs_Count( configs );
  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfigs_FromDB( \"//colour\" ) count: %d     ", j_n );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfigs_FromDB( \"//colour\" ) count: %d     ", j_n );
  }
  for( j = 0; j < j_n; ++j )
  {
    config = oyConfigs_Get( configs, j );

    k_n = oyConfig_Count( config );
    for( k = 0; k < k_n; ++k )
    {
      o = oyConfig_Get( config, k );

      printf( "  %d::%d %s %s\n", j,k,
              o->registration, o->value->string );

      oyOption_Release( &o );
    }

    oyConfig_Release( &config );
  }

  return result;
}

oyTESTRESULT_e testCMMmonitorDBmatch ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int k, k_n;
  int32_t rank = 0;
  int error = 0;
  oyConfig_s * config = 0,
             * device = 0;
  oyOption_s * o = 0;

  fprintf( stdout, "load a device ...\n");
  error = oyDeviceGet( 0, "monitor", ":0.0", 0, &device );
  k_n = oyConfig_Count( config );
  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyDeviceGet(..\"monitor\" \":0.0\".. &device ) %d", k_n );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyDeviceGet(..\"monitor\" \":0.0\".. &device) %d", k_n );
  }

  fprintf( stdout, "... and search for the devices DB entry ...\n");
  error = oyConfig_GetDB( device, &rank );
  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfig_GetDB( device )                       " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfig_GetDB( device )                       " );
  }
  if(device && rank > 0)
  {
    printf("rank: %d\n", rank);
    k_n = oyConfig_Count( device );
    for( k = 0; k < k_n; ++k )
    {
      o = oyConfig_Get( device, k );

      printf( "  d::%d %s: \"%s\"\n", k,
      strchr(strchr(strchr(strchr(o->registration,'/')+1,'/')+1,'/')+1,'/')+1,
              o->value->string );

      oyOption_Release( &o );
    }
    //error = oyConfig_EraseFromDB( config );
  }


  return result;
}


#include "oyranos_alpha_internal.h"

oyTESTRESULT_e testCMMsShow ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  int i, j, k;
  uint32_t size = 0;
  int32_t texts_n = 0;
  uint32_t count = 0;
  char ** profiles = 0,
       ** texts = 0,
        * text = 0,
        * text_tmp = (char*)oyAllocateFunc_(65535);
  oyProfiles_s * iccs;
  oyProfile_s * temp_prof;
  oyProfileTag_s * tag = 0;
  oyCMMInfo_s * cmm_info = 0;
  oyCMMapi4_s * cmm_api4 = 0;
  oyCMMapi6_s * cmm_api6 = 0;
  oyCMMapi_s * tmp = 0;
  oyCMMapiFilter_s * cmm_filter = 0;
  oyPROFILE_e type = oyDEFAULT_PROFILE_START;
  int current = 0;


  fprintf(stdout, "\n" );

  texts = oyCMMsGetLibNames_( &count, 0 );

  for( i = 0; i < (int)count; ++i)
  {
    cmm_info = oyCMMInfoFromLibName_( texts[i] );
    text = oyCMMInfoPrint_( cmm_info );
    tmp = cmm_info->api;


        while(tmp)
        {
          oyOBJECT_e type = oyOBJECT_NONE;
          char num[48],
               * api_reg = 0;

          type = oyCMMapi_Check_(tmp);

          oySprintf_(num,"    %d:", type );
          oyStringAdd_( &text, num, oyAllocateFunc_, oyDeAllocateFunc_ );
          oyStringAdd_( &text, oyStruct_TypeToText((oyStruct_s*)tmp),
                        oyAllocateFunc_, oyDeAllocateFunc_ );
          STRING_ADD( text, "\n" );

          if(type == oyOBJECT_CMM_API5_S)
          {
            cmm_filter = (oyCMMapiFilter_s*) tmp;

            {
              oyCMMapiFilter_s * api = 0;
              oyCMMapiFilters_s * apis = 0;
              uint32_t * rank_list = 0;
              uint32_t apis_n = 0;
              char * classe = 0;

              classe = oyFilterRegistrationToText( cmm_filter->registration,
                                                   oyFILTER_REG_TYPE, 0 );
              api_reg = oyStringCopy_("//", oyAllocateFunc_ );
              STRING_ADD( api_reg, classe );
              if(classe)
              oyFree_m_( classe );


              STRING_ADD( text, "    API(s) load from Meta backend:\n" );

              for(j = oyOBJECT_CMM_API4_S; j <= (int)oyOBJECT_CMM_API8_S; j++)
              {
                apis = oyCMMsGetFilterApis_( 0, 0, api_reg, (oyOBJECT_e)j,
                                             &rank_list, &apis_n );

                apis_n = oyCMMapiFilters_Count( apis );
                for(k = 0; k < (int)apis_n; ++k)
                {
                  api = oyCMMapiFilters_Get( apis, k );

                  snprintf( text_tmp, 65535,
                            "      [%s]: \"%s\"  %d\n        %s\n",
                            oyStructTypeToText(api->type),
                            api->registration,
                            (int)rank_list[k], api->id_ );
                  STRING_ADD( text, text_tmp );

                  if(api->type == oyOBJECT_CMM_API4_S)
                  {
                    cmm_api4 = (oyCMMapi4_s*) api;
                    oyStringAdd_( &text, "        category: ",
                                  oyAllocateFunc_, oyDeAllocateFunc_ );
                    if(cmm_api4->category)
                    oyStringAdd_( &text, cmm_api4->category,
                                  oyAllocateFunc_, oyDeAllocateFunc_ );
                    oyStringAdd_( &text, "\n        options: ",
                                  oyAllocateFunc_, oyDeAllocateFunc_ );
                    if(cmm_api4->options)
                    oyStringAdd_( &text, cmm_api4->options,
                                  oyAllocateFunc_, oyDeAllocateFunc_ );
                    oyStringAdd_( &text, oyXMLgetElement_(cmm_api4->options,
                                  "freedesktop.org/default/profile",
                                  "editing_rgb" ),
                                  oyAllocateFunc_, oyDeAllocateFunc_ );
                    STRING_ADD( text, "\n" );
                  }

                  if(api->type == oyOBJECT_CMM_API6_S)
                  {
                    cmm_api6 = (oyCMMapi6_s*) api;
                    snprintf( text_tmp, 65535,
                            "        \"%s\" -> \"%s\"\n",
                            cmm_api6->data_type_in,
                            cmm_api6->data_type_out );
                    STRING_ADD( text, text_tmp );
                  }

                  STRING_ADD( text, "\n" );
                  //oyCMMapiFilter_Release( &api );
                }
                oyCMMapiFilters_Release( &apis );
              }
              oyFree_m_(api_reg);
            }
          } else
          if(oyIsOfTypeCMMapiFilter( type ))
          {
            cmm_filter = (oyCMMapiFilter_s*) tmp;

            snprintf( text_tmp, 65535, "%s: %s\n",
                      oyStructTypeToText( tmp->type ),
                      cmm_filter->registration );
            STRING_ADD( text, text_tmp );

          }

          tmp = tmp->next;
        }

    printf("%d: \"%s\": %s\n\n", i, texts[i], text );

  }
  oyStringListRelease_( &texts, count, free );

  if( count )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyCMMsGetLibNames_( ) found %d                     ", count );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyCMMsGetLibNames_( ) found %d                     ", count );
  }

  oyDeAllocateFunc_( text_tmp );

  return result;
}

typedef struct {
  oyTESTRESULT_e (*oyTestRun)        ( oyTESTRESULT_e    (*test)(void),
                                       const char        * test_name );
  const char * title;
} oyTestRegistration_s;

oyTestRegistration_s test_registration[100];

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
  TEST_RUN( testProfileLists, "Profile lists" );
  TEST_RUN( testMonitor,  "Monitor profiles" );
  TEST_RUN( testRegistrationMatch,  "Registration matching" );
  TEST_RUN( testPolicy, "Policy handling" );
  TEST_RUN( testCMMConfigsListing, "CMM configs listing" );
  TEST_RUN( testCMMMonitorListing, "CMM monitor listing" );
  TEST_RUN( testCMMDBListing, "CMM DB listing" );
  TEST_RUN( testCMMmonitorDBmatch, "CMM monitor DB match" );
  TEST_RUN( testCMMsShow, "CMMs show" );

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

  return error;
}

