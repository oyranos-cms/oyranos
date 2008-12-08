/** @file test_oyranos.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2004-2008  Kai-Uwe Behrmann
 *
 */

/**
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

oyTESTRESULT_e test_version()
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

#include "oyranos_elektra.h"
oyTESTRESULT_e test_elektra()
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
#include <oyranos_texts.h> /* oyStringListRelease_ */
#include <oyranos_cmm.h>
#include <oyranos_xml.h>   /* for hacking into xml API */

#include <libxml/parser.h>
#include <libxml/xmlsave.h>

oyTESTRESULT_e test_settings ()
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

  oyOptions_SetFromText( opts, "cmyk_cmky_black_preservation", "1" );

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
      if(strcmp(tmp,"cmyk_cmky_black_preservation") == 0)
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

  xmlDocDumpFormatMemory( doc, &text, &i, 1 );
  /*xmlSaveDoc( ptr, doc );*/

  oyOptions_Release( &opts );

  return result;
}

oyTESTRESULT_e test_profiles ()
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

oyTESTRESULT_e test_monitor ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  int screen1, screen2, screen3;
  char * block, * text, * display_name;
  size_t size = 0;
  oyProfile_s * p, * p2;

  oyExportReset_(EXPORT_SETTING);
  fprintf(stdout, "\n" );

  screen1 = oyGetScreenFromPosition( 0, 0,0 );
  screen2 = oyGetScreenFromPosition( 0, 2000,0 );
  screen3 = oyGetScreenFromPosition( 0, 3000,0 );
  if(1)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "screens %d %d %d", screen1, screen2, screen3 );
  }

  display_name = oyGetDisplayNameFromPosition( 0, 0,0, malloc);
  block = oyGetMonitorProfile( display_name, &size, malloc );
  p = oyProfile_FromMem( size, block, 0,0 );

  if(block)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "monitor profile from server \"%s\" %d", oyProfile_GetText( p, oyNAME_DESCRIPTION ), size );
  } else
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "no default monitor profile %d", size );
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

  if(text && strcmp( oyProfile_GetText( p2, oyNAME_DESCRIPTION ),
                     oyProfile_GetText( p, oyNAME_DESCRIPTION )) == 0)
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

  oyTestRun( test_version, "Version matching" );
  oyTestRun( test_elektra, "Elektra" );
  oyTestRun( test_settings, "default oyOptions_s settings" );
  oyTestRun( test_profiles, "Profiles reading" );
  oyTestRun( test_monitor,  "Monitor profiles" );

  /* give a summary */

  fprintf( stderr, "\n################################################################\n" );
  fprintf( stderr, "#                                                              #\n" );
  fprintf( stderr, "#                     Results                                  #\n" );
  for(i = 0; i <= oyTESTRESULT_UNKNOWN; ++i)
    fprintf( stderr, "    Tests with status %s: %d\n", oyTestResultToString(i),
                                                   results[i] );

  error = (results[oyTESTRESULT_FAIL] ||
           results[oyTESTRESULT_SYSERROR] ||
           results[oyTESTRESULT_UNKNOWN]
          );

  if(error)
    fprintf( stderr, "    Tests FAILED\n" );
  else
    fprintf( stderr, "    Tests SUCCEEDED\n" );

  return error;
}

