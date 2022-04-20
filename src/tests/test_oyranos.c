/** @file test_oyranos.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2004-2019  Kai-Uwe Behrmann
 *
 *  @brief    Oyranos test suite
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2008/12/04
 */

#define TESTS_RUN \
  TEST_RUN( testVersion, "Version matching", 1 ); \
  TEST_RUN( testI18N, "Internationalisation", 1 ); \
  TEST_RUN( testElektra, "Elektra", 1 ); \
  TEST_RUN( testOption, "basic oyOption_s", 1 ); \
  TEST_RUN( testSettings, "default oyOptions_s settings", 1 ); \
  TEST_RUN( testProfiles, "Profiles reading", 1 ); \
  TEST_RUN( testMonitor,  "Monitor profiles", 1 ); \
  TEST_RUN( testRegistrationMatch,  "Registration matching", 1 );\
  TEST_RUN( testObserver,  "Generic Object Observation", 1 );

char * oyAlphaPrint_(int);
#define OYJL_TEST_MAIN_SETUP  printf("\n    Oyranos test\n"); if(getenv(OY_DEBUG)) oy_debug = atoi(getenv(OY_DEBUG)); // oy_debug_objects = 9; oy_debug_signals = 1;
#define OYJL_TEST_MAIN_FINISH printf("\n    Oyranos test finished\n\n"); if(testobj) testobj->release( &testobj ); if(verbose) { char * t = oyAlphaPrint_(0); puts(t); free(t); } oyLibConfigRelease(0); if(oyjl_print_sub) free(oyjl_print_sub);


#include "oyranos.h"
#include "oyranos_definitions.h"
#include "oyranos_string.h"
#include "oyranos_version.h"
oyObject_s testobj = NULL;

#include <oyjl_test_main.h>

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif
#define OBJECT_COUNT_SETUP \
  int object_count = 0, oy_debug_objects_old = -9999; \
\
  if(oy_debug_objects < 0) oyLibConfigRelease(0); \
  if(oy_debug_objects == -1) { oy_debug_objects_old = -1; oy_debug_objects = -2; }

#define OBJECT_COUNT_PRINT( onfail, reset, stop, msg ) \
  if(oy_debug_objects >= 0 || oy_debug_objects == -2) \
  { \
    oyLibConfigRelease(FINISH_IGNORE_OBJECT_LIST | FINISH_IGNORE_CORE); \
    object_count = oyObjectCountCurrentObjectIdList(); \
    if(object_count) \
    { \
      if(onfail == oyjlTESTRESULT_FAIL) \
      { \
        char * text = NULL; \
        OY_BACKTRACE_STRING(0) \
        oyObjectTreePrint( 0x01 | 0x02 | 0x08, text ? text : __func__ ); \
        oyFree_m_( text ) \
      } \
      PRINT_SUB_INT( onfail,                 object_count, "%s:", msg?msg:"objects" ); \
    } else \
    { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, object_count, "%s:", msg?msg:"objects" ); \
    } \
    oyLibConfigRelease(0); \
  } \
  if(oy_debug_objects_old != -9999 && reset) \
  { \
    oy_debug_objects = oy_debug_objects_old; \
  } \
  if( stop && object_count ) \
    return result;


/* --- actual tests --- */

oyjlTESTRESULT_e testVersion()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );
  fprintf(zout, "compiled version:     %d\n", OYRANOS_VERSION );
  fprintf(zout, " runtime version:     %d\n", oyVersion(0) );

  fprintf(zout, " XDG_DATA_DIRS: %s\n", oyNoEmptyString_m_(getenv("XDG_DATA_DIRS")));
  fprintf(zout, " OY_MODULE_PATH: %s\n", oyNoEmptyString_m_(getenv("OY_MODULE_PATH")));

  if(OYRANOS_VERSION == oyVersion(0))
    result = oyjlTESTRESULT_SUCCESS;
  else
    result = oyjlTESTRESULT_FAIL;

  testobj = oyObject_NewWithAllocators( myAllocFunc, myDeAllocFunc, "testobj" );

  return result;
}

#include <locale.h>
#include "oyranos_sentinel.h"
#include "oyranos_i18n.h"

oyjlTESTRESULT_e testI18N()
{
  const char * lang = 0, * t = 0;
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(zout, "\n" );

  oyI18Nreset();

  lang = oyLanguage();
  if((lang && strcmp(lang, "C") == 0) || !lang)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyLanguage() uninitialised good %s", lang?lang:"---" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyLanguage() uninitialised failed" );
  }

# ifdef __APPLE__
  {
# define TEXTLEN 128
  char *locale = (char*) calloc(sizeof(char), TEXTLEN);
  const char* tmp = 0;
  /* 1. get the locale info */
  CFLocaleRef userLocaleRef = CFLocaleCopyCurrent();
  CFStringRef cfstring = CFLocaleGetIdentifier( userLocaleRef );

    /* copy to a C buffer */
  CFIndex gr = 36;
  char text[36];
  Boolean fehler = CFStringGetCString( cfstring, text, gr, kCFStringEncodingISOLatin1 );

  if(fehler) {
    snprintf(locale,TEXTLEN, "%s", text);
  }

  /* set the locale info */
  if(strlen(locale))
  {
     tmp = setlocale (LC_ALL, locale);
  }
  if (tmp)
    snprintf(locale,TEXTLEN, "%s", tmp);
  /*set_codeset = 0;*/
  }
# else
  lang = setlocale(LC_ALL,"");
# endif
  oyI18Nreset();

  lang = oyLanguage();
  if(lang && (strcmp(lang, "C") != 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyLanguage() initialised good %s", lang?lang:"---" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyLanguage() initialised failed %s", lang?lang:"---" );
  }

  lang = setlocale(LC_ALL,"de_DE.UTF-8");
  oyI18Nreset();
  lang = oyLanguage();
  if(lang && (strcmp(lang, "de") == 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyLanguage() initialised good %s", lang?lang:"---" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyLanguage() initialised failed %s", lang?lang:"---" );
  }

  t = _("Sunrise");
  if(strcmp(t,"Sonnenaufgang") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "dgettext() good \"%s\"", t );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "dgettext() failed \"%s\"", t );
  }

  setlocale(LC_ALL,"");

  return result;
}

#include "oyranos_db.h"
#include "oyranos_texts.h"
oyjlTESTRESULT_e testElektra()
{
  int error = 0;
  char * value = 0,
       * start = 0;
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  oyExportReset_(EXPORT_SETTING);

  fprintf(zout, "\n" );

  error = oySetPersistentString("sw/Oyranos/Tests/test_key", oySCOPE_USER,
                                 "NULLTestValue", "NULLTestComment" );
  start = oyGetPersistentString("sw/Oyranos/Tests/test_key", 0,
                                oySCOPE_USER_SYS, 0);
  if(!start || !start[0])
  {
    oyExportStart_(EXPORT_CHECK_NO);
    oyExportEnd_();
    error = oySetPersistentString("sw/Oyranos/Tests/test_key", oySCOPE_USER,
                                 "NULLTestValue", "NULLTestComment" );
    start = oyGetPersistentString("sw/Oyranos/Tests/test_key", 0,
                                  oySCOPE_USER_SYS, 0);
    
    PRINT_SUB( error == 0?oyjlTESTRESULT_SUCCESS:oyjlTESTRESULT_XFAIL,
    "Elektra not initialised? try oyExportStart_(EXPORT_CHECK_NO)" );
  }
  if(!start || !start[0])
  {
    oyExportStart_(EXPORT_SETTING);
    oyExportEnd_();
    error = oySetPersistentString("sw/Oyranos/Tests/test_key", oySCOPE_USER,
                                 "NULLTestValue", "NULLTestComment" );
    start = oyGetPersistentString("sw/Oyranos/Tests/test_key", 0,
                                  oySCOPE_USER_SYS, 0);
    PRINT_SUB( error == 0?oyjlTESTRESULT_SUCCESS:oyjlTESTRESULT_XFAIL, 
    "Elektra not initialised? try oyExportStart_(EXPORT_SETTING)" );
  }
  if(start && start[0])
    fprintf(zout, "start key value: %s\n", start );
  else
    fprintf(zout, "could not initialise\n" );

  error = oySetPersistentString("sw/Oyranos/Tests/test_key", oySCOPE_USER,
                                 "myTestValue", "myTestComment" );
  value = oyGetPersistentString("sw/Oyranos/Tests/test_key", 0,
                                 oySCOPE_USER_SYS, 0);
  if(value)
    fprintf(zout, "result key value: %s\n", value );

  if(error)
  {
    PRINT_SUB( oyjlTESTRESULT_SYSERROR, 
    "Elektra error: %d", error );
  } else
  if(start && start[0] && value && strcmp(start,value) == 0)
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "Elektra (start!=value) failed: %s|%s", start, value );
  } else
  if(!value)
  {
    if(!value)
      PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "Elektra (value) failed" );
    if(!start || !start[0])
      PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "Elektra (init) failed" );
  } else
  if(value)
  {
    if(strcmp(value,"myTestValue") == 0)
    {
      PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
      "Elektra (value): %s", value );
    } else
    {
      PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "Elektra (value) wrong: %s", value );
    }
  }

  if(start)
    oyDeAllocateFunc_(start);
  if(value)
    oyDeAllocateFunc_(value);

  return result;
}


#include "oyOption_s.h"

oyjlTESTRESULT_e testOption ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  int error = 0;
  oyOption_s * o;
  const char * test_buffer = "test";
  size_t size = strlen(test_buffer);
  oyPointer ptr = oyAllocateFunc_( size );

  oyExportReset_(EXPORT_SETTING);

  fprintf(zout, "\n" );

  o = oyOption_FromRegistration( "blabla", 0 );
  if(!o)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOption_New() with wrong registration rejected: ok" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOption_New() with wrong registration not rejected");
  }

  o = oyOption_FromRegistration( 0, 0 );
  if(o)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOption_New() good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOption_New() failed" );
  }

  memcpy( ptr, test_buffer, size );
  error = oyOption_SetFromData( o, ptr, size );
  if(!error)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOption_SetFromData() good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOption_SetFromData() failed" );
  }

  oyDeAllocateFunc_( ptr ); ptr = 0;
  size = 0;

  ptr = oyOption_GetData( o, &size, oyAllocateFunc_ );
  if(ptr && size && memcmp( ptr, test_buffer, 4 ) == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOption_GetData() good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOption_GetData() failed" );
  }

  oyOption_Release( &o );
  oyDeAllocateFunc_( ptr ); ptr = 0;
  size = 0;

  return result;
}


#ifdef HAVE_LIBXML2
#include <libxml/parser.h>
#ifdef LIBXML_WRITER_ENABLED
#include <libxml/xmlsave.h>
#endif
#endif

oyjlTESTRESULT_e testSettings ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

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
#ifdef HAVE_LIBXML2
  xmlDocPtr doc = 0;
#endif

  oyExportReset_(EXPORT_SETTING);

  fprintf(zout, "\n" );

  /* we check for out standard CMM */
  opts = oyOptions_ForFilter( "//" OY_TYPE_STD "/lcm2",
                                            oyOPTIONATTRIBUTE_ADVANCED /* |
                                            oyOPTIONATTRIBUTE_FRONT |
                                            OY_OPTIONSOURCE_META */, 0 );

  oyOptions_SetFromString( &opts, "cmyk_cmyk_black_preservation", "1", 0 );

  count = oyOptions_Count( opts );
  if(!count)
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "No options found for lcm2" );
  } else
  {
    for(i = 0; i < count; ++i)
    {
      o = oyOptions_Get( opts, i );
      tmp = oyOption_GetValueText( o, 0 );
      fprintf(zout, "%s:", tmp );
      oyDeAllocateFunc_(tmp);

      tmp = oyFilterRegistrationToText( oyOption_GetText(o, oyNAME_DESCRIPTION),
                                        oyFILTER_REG_OPTION, 0 );
      if(strcmp( oyNoEmptyString_m_(tmp),"cmyk_cmyk_black_preservation") == 0)
        ++size;

      oyDeAllocateFunc_(tmp);
      oyOption_Release( &o );
    }

    if(!size)
    {
      PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "oyOptions_FindString() could not find option" );
    } else
    if(size > 1)
    {
      PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "oyOptions_FindString() returned doubled options %u",
                       (unsigned int)size );
    } else
      PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
      "oyOptions_FindString() returned one option" );
  }

  /*ptr = xmlSaveToBuffer( buf, 0, 0 );*/


  text = oyStringCopy_(oyOptions_GetText( opts, oyNAME_NAME ), oyAllocateFunc_);
  {
    if(!text || !strlen(text))
    {
      PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "oyOptions_GetText() returned no text" );
    } else
    {
      PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(text),
      "oyOptions_GetText() returned text" );
      /*if(verbose)
        fprintf(zout, "%s\n", text );*/
    }
  }
  count = oyOptions_Count( opts );
  if(verbose)
  {
    const char * t = oyOptions_GetText( opts, oyNAME_NICK );
    fprintf( zout, "%s\n", t );
    for(i = 0; i < count; ++i)
    {
      o = oyOptions_Get( opts, i );
      const char * type_name = oyStructTypeToText( o->type_ );
      int id = 0;
      if(o->type_ < oyOBJECT_MAX_STRUCT)
        id = oyObject_GetId( o->oy_ );
      const char * id_text = oyStruct_GetInfo( (oyStruct_s*)o, oyNAME_NAME, 0x01 );
      fprintf( zout, "%s[%d]%s%s%s\n", type_name, id,
             id_text ? "=\"" : "", id_text ? id_text : "", id_text ? "\"" : "");

      oyOption_Release( &o );
    }
  }
  oyOptions_Release( &opts );


  /* Roundtrip test */
  opts = oyOptions_FromText( text, 0, 0 );
  countB = oyOptions_Count( opts );
  if(verbose)
  {
    const char * t = oyOptions_GetText( opts, oyNAME_NICK );
    fprintf( zout, "%s\n", t );
  }

  {
    if(count == countB)
    {
      PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
      "obtained same count from oyOptions_FromText %d|%d",
                      count, countB );
    } else
    {
      PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "oyOptions_FromText() returned different count %d|%d",
                      count, countB );
    }
  }
  
  tmp = oyStringAppend_( "<a>\n", text, 0 );
  oyStringAdd_( &tmp, "</a>", 0, 0 );
  oyDeAllocateFunc_(text); text = tmp; tmp = 0;

#ifdef HAVE_LIBXML2
  doc = xmlParseMemory( text, oyStrlen_( text ) );
  {
    if(error)
    {
      PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "libxml2::xmlParseMemory() returned could not parse the document" );
    } else
    {
      PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
      "libxml2 returned document" );
    }
  }

  oyDeAllocateFunc_(text); text = 0;
#ifdef LIBXML_WRITER_ENABLED
  xmlDocDumpFormatMemory( doc, (xmlChar**)&text, &i, 1 );
#endif
  oyDeAllocateFunc_(text); text = 0;
  /*xmlSaveDoc( ptr, doc );*/
  xmlFreeDoc( doc ); doc = 0;
#endif

  oyOptions_Release( &opts );

  return result;
}

#include "oyProfiles_s.h"

oyjlTESTRESULT_e testProfiles ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

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

  fprintf(zout, "\n" );

  /* compare the usual conversion profiles with the total of profiles */
  profs = oyProfiles_ForStd( oyDEFAULT_PROFILE_START, 0, &current, 0 );
  count = oyProfiles_Count( profs );
  if(!count)
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "No profiles found for oyDEFAULT_PROFILE_START" );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "profiles found for oyDEFAULT_PROFILE_START: %d (%d)", count, current );
  }
  oyProfiles_Release( &profs );

  texts = oyProfileListGet( 0, &size, 0 );
  if(!size)
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "No profiles found for oyProfileListGet()" );
  } else
  {
    PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, size,
    "profiles found for oyProfileListGet:" );
  }
  oyStringListRelease_( &texts, size, oyDeAllocateFunc_ );

  if(size < (size_t)count)
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyProfileListGet() returned less than oyDEFAULT_PROFILE_START %u|%d", (unsigned int)size, count );
  } else if(count)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyProfileListGet and oyDEFAULT_PROFILE_START ok %u|%d", (unsigned int)size, count );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyProfileListGet and/or oyDEFAULT_PROFILE_START are wrong %u|%d", (unsigned int)size, count );
  }

  /* compare the default profile spaces with the total of profiles */
  countB = 0;
  for(i = oyEDITING_XYZ; i <= oyEDITING_GRAY; ++i)
  {
    profs = oyProfiles_ForStd( (oyPROFILE_e)i, 0, &current, 0 );

    count = oyProfiles_Count( profs );
    countB += count;
    if(!count)
    {
      PRINT_SUB( oyjlTESTRESULT_XFAIL, 
      "No profiles found for oyPROFILE_e %d             ", i );
    } else
    {
      p = oyProfiles_Get( profs, current );
      tmp = oyProfile_GetText( p, oyNAME_DESCRIPTION );
      PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
      "profiles found for oyPROFILE_e %d: %d \"%s\"", i, count, tmp ? tmp :"" );
      oyProfile_Release( &p );
    }

    oyProfiles_Release( &profs );
  }
  if(size < (size_t)countB)
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyProfileListGet() returned less than oyPROFILE_e %u|%d", (unsigned int)size, count );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyProfileListGet and oyPROFILE_e ok %u|%d", (unsigned int)size, countB );
  }


  return result;
}

#include "oyRectangle_s.h"
#include "oyranos_devices.h"

oyjlTESTRESULT_e testMonitor ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  int n, i, error = 0;
  char * block, * text = 0, * display_name;
  const char * tmp = 0;
  size_t size = 0;
  oyProfile_s * p = NULL, * p2 = NULL;
  oyConfigs_s * devices = 0;
  oyConfig_s * c = 0;
  oyOptions_s * options = 0;
  oyOption_s * o = 0;
  oyRectangle_s * r = 0;

  oyExportReset_(EXPORT_SETTING);
  fprintf(zout, "\n" );

  /* get all monitors */
  error = oyDevicesGet( 0, "monitor", 0, &devices );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDevicesGet() error: %d", error )
  /* see how many are included */
  n = oyConfigs_Count( devices );
  if(n)
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
               "\"list\" device(s): %d", n )
  else
    PRINT_SUB( oyjlTESTRESULT_XFAIL,
               "\"list\" device(s): ---" )

  error = oyOptions_SetFromString( &options,
                               "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
                                       "yes", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyOptions_SetFromString( &options,
                                 "//"OY_TYPE_STD"/config/command",
                                 "list", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )

  for( i = 0; i < n; ++i )
  {
    c = oyConfigs_Get( devices, i );
    oyDeviceGetProfile( c, options, &p );

    if(p)
    {
      tmp = oyProfile_GetText( p, oyNAME_DESCRIPTION );
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
               "device[%d]: \"%s\"              ", i,
               tmp ? tmp :"" );
    } else
    {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
               "device[%d]: no profile                             ", i );
    }

    oyProfile_Release( &p );
    oyConfig_Release( &c );
  }
  /* release devices */
  oyConfigs_Release( &devices );
  oyOptions_Release( &options );


  /* get all monitors */
  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                 "list", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/device_rectangle",
                                 "true", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyDevicesGet( 0, "monitor", options, &devices );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDevicesGet() error: %d", error )
  oyOptions_Release( &options );
  /* see how many are included */
  n = oyConfigs_Count( devices );
  if(n)
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
               "\"list\" + device_rectangle device(s): %d", n )
  else
    PRINT_SUB( oyjlTESTRESULT_XFAIL,
               "\"list\" device(s): ---" )

  for( i = 0; i < n; ++i )
  {
    c = oyConfigs_Get( devices, i );
    o = oyConfig_Find( c, "device_rectangle" );
    r = (oyRectangle_s *) oyOption_GetStruct( o, oyOBJECT_RECTANGLE_S );

    if(r)
    {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
               "device[%d]: %s", i,
               oyRectangle_Show(r)  );
    } else
    {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
               "device[%d]: no rectangle", i );
    }

    oyRectangle_Release( &r );
    oyOption_Release( &o );
    oyConfig_Release( &c );
  }
  /* release devices */
  oyConfigs_Release( &devices );


#if 0
  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD"/config/command",
                                 "properties", OY_CREATE_NEW );
#endif

  error = oyOptions_SetFromString( &options,
                               "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
                                       "yes", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyOptions_SetFromString( &options,
                                 "//"OY_TYPE_STD"/config/command",
                                 "properties", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )

  error = oyDevicesGet( OY_TYPE_STD, "monitor", options, &devices );
  if(error)
    PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDevicesGet() error: %d", error )

  n = oyConfigs_Count( devices );
  if(n)
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
               "\"properties\" device(s): %d", n )
  else
    PRINT_SUB( oyjlTESTRESULT_XFAIL,
               "\"properties\" device(s): ---" )

  if(!error)
  {
    for(i = 0; i < n; ++i)
    {
      c = oyConfigs_Get( devices, i );

      error = oyDeviceGetInfo( c, oyNAME_NAME, 0, &text, 0 );
      if(error)
        PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDeviceGetInfo() error: %d", error )

      if(text && text[0])
        PRINT_SUB( oyjlTESTRESULT_SUCCESS,
            "device: %s", text )
      else
        PRINT_SUB( oyjlTESTRESULT_XFAIL, "device: --- %d", error )

      if(text)
        oyDeAllocateFunc_( text );

      error = oyDeviceGetInfo( c, oyNAME_DESCRIPTION, options, &text, 0 );
      if(error)
        PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDeviceGetInfo() error: %d", error )

      if(text && text[0])
      {
        char * list = text, * tmp = 0, * line = malloc(128);
        int even = 1;

        PRINT_SUB( oyjlTESTRESULT_SUCCESS,
            "device" )

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
        PRINT_SUB( oyjlTESTRESULT_XFAIL, "device: ---" )

      if(text)
      { oyDeAllocateFunc_( text ); text = 0; }


      /* get the old oyMonitorxxx API conforming display name */
      error = oyDeviceGetInfo( c, oyNAME_NICK, 0, &text, 0 );
      if(error)
        PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDeviceGetInfo() error: %d", error )

      if(text && text[0])
        PRINT_SUB( oyjlTESTRESULT_SUCCESS,
            "device: %s", text )
      else
        PRINT_SUB( oyjlTESTRESULT_XFAIL, "device: ---" )

      display_name = text; text = 0;

      size = 0;
      error = oyDeviceGetProfile( c, options, &p );
      if(error)
        PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDeviceGetProfile() error: %d", error )
      block = oyProfile_GetMem( p, &size, 0, malloc );

      if(block)
      {
        PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "monitor profile from server \"%s\" %d",
        oyProfile_GetText( p, oyNAME_DESCRIPTION ), (int)size );
      } else
      {
        PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "no default monitor profile %d", (int)size );
      }

      error = oyDeviceProfileFromDB( c, &text, malloc );
      if(error)
        PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDeviceProfileFromDB() error: %d", error )
      if(display_name) { free(display_name); display_name = 0; }
      if(text)
      {
        PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "monitor profile from Oyranos DB %s", text );
      } else
      {
        PRINT_SUB( oyjlTESTRESULT_XFAIL,
        "no monitor profile from Oyranos DB" );
      }

      if(text)
        p2 = oyProfile_FromFile( text, 0, 0 );

      if(text &&
         strcmp( oyNoEmptyString_m_(oyProfile_GetText( p2,oyNAME_DESCRIPTION )),
                 oyNoEmptyString_m_(oyProfile_GetText( p ,oyNAME_DESCRIPTION )))
         == 0)
      {
        PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "monitor profile from Oyranos DB matches the server one" );
      } else
      {
        PRINT_SUB( oyjlTESTRESULT_XFAIL,
        "monitor profile from Oyranos DB differs from the server one" );
      }

      if(text) { free( text ); text = NULL; }

      if(block) { free(block); block = 0; }
      oyConfig_Release( &c );
      oyProfile_Release( &p );
      oyProfile_Release( &p2 );
      fprintf(zout, "\n" );
    }
  }
  oyConfigs_Release( &devices );
  oyOptions_Release( &options );

  display_name = oyGetDisplayNameFromPosition( 0, 0,0, malloc);
  if(display_name) free(display_name);

  return result;
}

oyjlTESTRESULT_e testRegistrationMatch ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(zout, "\n" );

  if( oyFilterRegistrationMatch(OY_INTERNAL "/icc_color.lcms",
                                "//" OY_TYPE_STD "/icc_color",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "simple CMM selection" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "simple CMM selection" );
  }

  if(!oyFilterRegistrationMatch( OY_INTERNAL "/icc_color.lcms",
                                "//" OY_TYPE_STD "/icc_color.octl",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "simple CMM selection no match" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "simple CMM selection no match" );
  }

  if( oyFilterRegistrationMatch( OY_INTERNAL "/icc_color.lcms",
                                "//" OY_TYPE_STD "/icc_color.4+lcms",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "special CMM selection" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "special CMM selection" );
  }

  if(!oyFilterRegistrationMatch( OY_INTERNAL "/icc_color.lcms",
                                "//" OY_TYPE_STD "/icc_color.4-lcms",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "special CMM avoiding " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "special CMM avoiding" );
  }

  if( oyFilterRegistrationMatch( OY_INTERNAL "/icc_color.lcms",
                                "//" OY_TYPE_STD "/icc_color.7-lcms",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "special CMM avoiding, other API" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "special CMM avoiding, other API" );
  }

  if( oyFilterRegistrationMatch( OY_INTERNAL "/icc_color.lcms",
                                "lcms", 0 ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "select by keyname" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "select by keyname" );
  }

  return result;
}

#include <oyranos_cmm.h>
#include <oyObserver_s_.h>

int myFilterSignalHandler            ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data OY_UNUSED )
{
  oyFilterNode_s * node = 0;
  int handled = 0;
  switch(signal_type)
  {
  case oySIGNAL_OK:
       fprintf(zout, "Signal: oySIGNAL_OK\n" );
       break;
  case oySIGNAL_CONNECTED:                  /**< connection established */
       fprintf(zout, "Signal: oySIGNAL_CONNECTED\n" );
       break;
  case oySIGNAL_RELEASED:                   /**< released the connection */
       fprintf(zout, "Signal: oySIGNAL_RELEASED\n" );
       break;
  case oySIGNAL_DATA_CHANGED:               /**< call to update image views */
       fprintf(zout, "Signal: oySIGNAL_DATA_CHANGED\n" );
       {
         oyObserver_s_ * obs = (oyObserver_s_ *)observer;
         if(obs->observer->type_ == oyOBJECT_FILTER_NODE_S)
         {
           oyPointer_s * node_context;
           node = (oyFilterNode_s*) obs->observer;
           node_context = oyFilterNode_GetContext( node );
           if(node && node_context)
           {
             if(obs->model->type_ == oyOBJECT_OPTION_S)
             {
               fprintf( zout, "release context %s\n",
                        oyStruct_TypeToText( obs->observer ) );
               oyFilterNode_SetContext( node, 0 );
             } else
               fprintf( zout, "Model type not expected: %s\n",
                        oyStruct_TypeToText( obs->model ) );
             oyPointer_Release( &node_context );
           } else
             fprintf( zout, "no context %s\n",
                      oyStruct_TypeToText( obs->observer ) );
        }
         else
           fprintf( zout, "wrong signal handler for %s\n",
                    oyStruct_TypeToText( obs->observer ) );
       }
       break;
  case oySIGNAL_STORAGE_CHANGED:            /**< new data accessors */
       fprintf(zout, "Signal: oySIGNAL_STORAGE_CHANGED\n" );
       break;
  case oySIGNAL_INCOMPATIBLE_DATA:          /**< can not process image */
       fprintf(zout, "Signal: oySIGNAL_INCOMPATIBLE_DATA\n" );
       break;
  case oySIGNAL_INCOMPATIBLE_OPTION:        /**< can not handle option */
       fprintf(zout, "Signal: oySIGNAL_INCOMPATIBLE_OPTION\n" );
       break;
  case oySIGNAL_INCOMPATIBLE_CONTEXT:       /**< can not handle profile */
       fprintf(zout, "Signal: oySIGNAL_INCOMPATIBLE_CONTEXT\n" );
       break;
  case oySIGNAL_USER1: 
       fprintf(zout, "Signal: oySIGNAL_USER1\n" );
       break;
  case oySIGNAL_USER2: 
       fprintf(zout, "Signal: oySIGNAL_USER2\n" );
       break;
  case oySIGNAL_USER3:                      /**< more signal types are possible */
       fprintf(zout, "Signal: oySIGNAL_USER3\n" );
       break;
  default:
       fprintf(zout, "Signal: unknown\n" );
       break;
  }
  fflush(zout);
  fflush(zout);
  fflush(zout);
  return handled;
}

oyjlTESTRESULT_e testObserver ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP
  oyOption_s * o = oyOption_FromRegistration( "a/b/c/d/my_key", 0 );
  oyFilterNode_s * node = oyFilterNode_NewWith( "//" OY_TYPE_STD "/icc_color", 0, 0 );

  fprintf(zout, "\n" );

  oyOption_SetFromString( o, "my_value", 0 );

  if( !oyStruct_ObserverAdd( (oyStruct_s*)o, (oyStruct_s*)node, 0,
                             myFilterSignalHandler ) )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "Added Observer" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "Added Observer" );
  }

  oyOption_SetFromString( o, "new_value", 0 );

  if( !oyStruct_ObserverRemove( (oyStruct_s*)o, (oyStruct_s*)node,
                                myFilterSignalHandler ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "Removed Observer" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "Removed Observer" );
  }

  oyOption_SetFromString( o, "other_value", 0 );
  oyOption_Release( &o );
  oyFilterNode_Release( &node );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_XFAIL, 1, 0, NULL )

  return result;
}

/* --- end actual tests --- */


