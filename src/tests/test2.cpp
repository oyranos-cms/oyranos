/** @file test2.cpp
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2004-2022  Kai-Uwe Behrmann
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
  TEST_RUN( testDB, "elDB basics", 1 ); \
  TEST_RUN( testDB2, "oiDB basics", 1 ); \
  TEST_RUN( testStringRun, "String handling", 1 ); \
  TEST_RUN( testJson, "JSON handling", 1 ); \
  TEST_RUN( testOption, "basic oyOption_s", 1 ); \
  TEST_RUN( testOptionInt,  "oyOption_s integers", 1 ); \
  TEST_RUN( testOptionsSet,  "Set oyOptions_s", 1 ); \
  TEST_RUN( testOptionsCopy,  "Copy oyOptions_s", 1 ); \
  TEST_RUN( testOptionsType,  "Objects inside oyOptions_s", 1 ); \
  TEST_RUN( testBlob, "oyBlob_s", 1 ); \
  TEST_RUN( testDAGbasic, "basic DAG", 1 ); \
  TEST_RUN( testSettings, "default oyOptions_s settings", 1 ); \
  TEST_RUN( testConfDomain, "oyConfDomain_s", 1 ); \
  TEST_RUN( testInterpolation, "Interpolation oyLinInterpolateRampU16", 1 ); \
  TEST_RUN( testProfile, "Profile handling", 1 ); \
  TEST_RUN( testProfiles, "Profiles reading", 1 ); \
  TEST_RUN( testProfileLists, "Profile lists", 1 ); \
  TEST_RUN( testEffects, "Effects", 1 ); \
  TEST_RUN( testDeviceLinkProfile, "CMM deviceLink", 1 ); \
  TEST_RUN( testClut, "CMM clut", 1 ); \
  TEST_RUN( testRegistrationMatch,  "Registration matching", 1 ); \
  TEST_RUN( test_oyTextIccDictMatch,  "IccDict matching", 1 ); \
  TEST_RUN( testPolicy, "Policy handling", 1 ); \
  TEST_RUN( testWidgets, "Widgets", 1 ); \
  TEST_RUN( testCMMDevicesListing, "CMM devices listing", 1 ); \
  TEST_RUN( testCMMDevicesDetails, "CMM devices details", 1 ); \
  TEST_RUN( testCMMRankMap, "rank map handling", 1 ); \
  TEST_RUN( testCMMMonitorJSON, "monitor JSON", displayFail() == oyjlTESTRESULT_FAIL ); \
  TEST_RUN( testCMMMonitorListing, "CMM monitor listing", displayFail() == oyjlTESTRESULT_FAIL ); \
  TEST_RUN( testCMMMonitorModule, "CMM monitor module", displayFail() == oyjlTESTRESULT_FAIL ); \
  TEST_RUN( testCMMDBListing, "CMM DB listing", 1 ); \
  TEST_RUN( testCMMmonitorDBmatch, "CMM monitor DB match", displayFail() == oyjlTESTRESULT_FAIL ); \
  TEST_RUN( testCMMsShow, "CMMs show", 1 ); \
  TEST_RUN( testCMMnmRun, "CMM named color run", 1 ); \
  TEST_RUN( testImagePixel, "CMM Image Pixel run", 1 ); \
  TEST_RUN( testRectangles, "Image Rectangles", 1 ); \
  TEST_RUN( testDAG2, "screen DAG", 1 ); \
  TEST_RUN( testScreenPixel, "Draw Screen Pixel run", 1 ); \
  TEST_RUN( testFilterNode, "FilterNode Options", 1 ); \
  TEST_RUN( testConversion, "CMM selection", 1 ); \
  TEST_RUN( testCMMlists, "CMMs listing", 1 ); \
  TEST_RUN( testICCsCheck, "CMMs ICC conversion check", 1 ); \
  TEST_RUN( testCCorrectFlags, "Conversion Correct Option Flags", 1 ); \
  TEST_RUN( testCache, "Cache", 1 ); \
  TEST_RUN( testPaths, "Paths", 1 ); \
  TEST_RUN( testIO, "I/O", 1 );

#include "oyranos.h"
#include "oyranos_string.h"
oyObject_s testobj = NULL;
extern "C" { char * oyAlphaPrint_(int); }
#define OYJL_TEST_NAME "test2"
#define OYJL_TEST_MAIN_SETUP  printf("\n    Oyranos test2\n"); if(getenv(OY_DEBUG)) oy_debug = atoi(getenv(OY_DEBUG));  if(getenv(OY_DEBUG_SIGNALS)) oy_debug_signals = atoi(getenv(OY_DEBUG_SIGNALS)); if(getenv(OY_DEBUG_OBJECTS)) oy_debug_objects = atoi(getenv(OY_DEBUG_OBJECTS)); //else  oy_debug_objects = 2851;  // oy_debug_signals = 1;
#define OYJL_TEST_MAIN_FINISH printf("\n    Oyranos test2 finished\n\n"); if(testobj) testobj->release( &testobj ); if(verbose) { char * t = oyAlphaPrint_(0); puts(t); free(t); } oyLibConfigRelease(0); if(oyjl_print_sub) free(oyjl_print_sub);
#include <oyjl_test_main.h>

#include <cmath>

#include "oyranos_debug.h" // OY_BACKTRACE_STRING
#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif

double d[6] = {0.5,0.5,0.5,0,0,0};

#define OBJECT_COUNT_SETUP \
  int object_count = 0, oy_debug_objects_old = -9999; \
\
  if(oy_debug_objects < 0) oyLibConfigRelease(0); \
  if(oy_debug_objects == -1) { oy_debug_objects_old = -1; oy_debug_objects = -2; }

#define OBJECT_COUNT_PRINT( onfail, reset, stop, msg ) { \
  if(oy_debug_objects >= 0 || oy_debug_objects <= -2) \
  { \
    oyLibConfigRelease(FINISH_IGNORE_OBJECT_LIST | FINISH_IGNORE_CORE); \
    object_count = oyObjectCountCurrentObjectIdList(); \
    if(object_count) \
    { \
      if(onfail == oyjlTESTRESULT_FAIL) \
      { \
        char * text = oyBT(-1); \
        oyObjectTreePrint( 0x01 | 0x02 | 0x08, text ? text : __func__ ); \
        oyFree_m_( text ) \
      } \
      PRINT_SUB_INT( onfail,                 object_count, "%s:", msg?msg:"objects"); \
    } else \
    { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, object_count, "%s:", msg?msg:"objects"); \
    } \
    oyLibConfigRelease(0); \
  } \
  if(oy_debug_objects_old != -9999 && reset) \
  { \
    oy_debug_objects = oy_debug_objects_old; \
  } \
  if( stop && object_count ) \
    return result; \
}

/* --- actual tests --- */

oyjlTESTRESULT_e testVersion()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  const char * t;
  int i;

  fprintf(stdout, "\n" );
  fprintf(zout, "compiled version:     %d\n", OYRANOS_VERSION );
  fprintf(zout, " runtime version:     %d\n", oyVersion(0) );

  fprintf(zout, "XDG_DATA_DIRS: %s\n", oyNoEmptyString_m_(getenv("XDG_DATA_DIRS")));
  fprintf(zout, "OY_MODULE_PATH: %s\n", oyNoEmptyString_m_(getenv("OY_MODULE_PATH")));

  if(OYRANOS_VERSION == oyVersion(0))
    result = oyjlTESTRESULT_SUCCESS;
  else
    result = oyjlTESTRESULT_FAIL;

  for(i = 1; i < 5; ++i)
  {
    t = oyVersionString(i);
    fprintf(zout, "oyVersionString( %d ) = %s\n", i, t );
  }

  testobj = oyObject_NewWithAllocators( myAllocFunc, myDeAllocFunc, "testobj" );

  return result;
}

#include <locale.h>
#include "oyranos_sentinel.h"
#include "oyranos_i18n.h"

oyjlTESTRESULT_e testI18N()
{
  const char * lang = NULL;
  const char * clang = getenv("LANG");
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

#ifdef USE_GETTEXT
  if(!clang || strcmp(clang,"C") == 0)
  {
    char * tmp = oyStringCopy("LANG=de_DE.UTF-8", 0);
    putenv(tmp);
    clang = getenv("LANG");
    if(!clang || strcmp(clang,"de_DE.UTF-8") != 0)
      fprintf( stderr, "Could not modify LANG environment variable. Test will not be useful.\n" );
  }
#endif

  oyI18Nreset();

  lang = oyLanguage();
  if((lang && (strcmp(lang, "C") == 0)) || !lang)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyLanguage() uninitialised good %s",lang?lang:"---" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyLanguage() uninitialised failed %s",lang?lang:"---" );
  }

  lang = oyLang();
  if((lang && (strcmp(lang, "C") == 0)) || !lang)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyLang() uninitialised %s",lang?lang:"---" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyLang() uninitialised %s",lang?lang:"---" );
  }

  setlocale(LC_ALL,"");
  oyI18Nreset();

  lang = oyLanguage();
  if(lang && (strcmp(lang, "C") != 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyLanguage() initialised good %s", lang?lang:"---" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyLanguage() initialised failed %s", lang?lang:"---" );
  }

  const char * loc = setlocale(LC_ALL,"de_DE.UTF8");
  int use_gettext = 0;
#ifdef USE_GETTEXT
  use_gettext = 1;
#endif
  static int debug = 0; /* needs to be in global scope */
  oyjlTr_s * trc = oyjlTr_New( loc, 0, 0,0,0,0,0 );
  oyjlInitLanguageDebug( "Oyranos", "OY_DEBUG", &debug, use_gettext, "OY_LOCALEDIR", OY_LOCALEDIR, &trc, oyMessageFunc_p );

  lang = setlocale(LC_ALL, NULL);
  if(lang && (strcmp(lang, "C") != 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "setlocale() initialised good %s", lang );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "setlocale() initialised failed %s", lang );
  }

  const char * text = _("Example");
  if(strcmp(text,"Beispiel") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "dgettext() good \"%s\"", text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "dgettext() failed \"%s\"", text );
  }

  setlocale(LC_ALL,"");


  return result;
}

#define TEST_DOMAIN "sw/Oyranos/Tests"
#define TEST_KEY "/test_key"

#include "oyranos_db.h"
#include "oyranos_texts.h"
#include "oyranos_i18n.h"
oyjlTESTRESULT_e testDBDefault()
{
  int error = 0;
  char * value = 0,
       * start = 0;
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  oyExportReset_(EXPORT_SETTING);

  fprintf(stdout, "\n" );

  error = oySetPersistentString( TEST_DOMAIN TEST_KEY, oySCOPE_USER,
                                 NULL, NULL );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oySetPersistentString() error: %d", error )
  start = oyGetPersistentString( TEST_DOMAIN TEST_KEY, 0, oySCOPE_USER_SYS, 0);
  if(start && start[0])
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyGetPersistentString(%s) no", TEST_DOMAIN TEST_KEY );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyGetPersistentString(%s) no", TEST_DOMAIN TEST_KEY );
  }
  oyFree_m_(start);

  int ready = oyDbHandlingReady();
  if(ready)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyDbHandlingReady()" );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyDbHandlingReady()" );
  }

  error = oySetPersistentString( TEST_DOMAIN TEST_KEY, oySCOPE_USER,
                                 "NULLTestValue", "NULLTestComment" );
  if(error)
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oySetPersistentString(%s)", TEST_DOMAIN TEST_KEY );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oySetPersistentString(%s)", TEST_DOMAIN TEST_KEY );
  }

  start = oyGetPersistentString(TEST_DOMAIN TEST_KEY, 0, oySCOPE_USER_SYS, 0);
  if(start && start[0])
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyGetPersistentString(%s)", TEST_DOMAIN TEST_KEY );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyGetPersistentString(%s)", TEST_DOMAIN TEST_KEY );
  }

  printf ("start is %s\n", oyNoEmptyString_m_(start));
  oyjlTESTRESULT_e r;
  if(!(start && start[0]))
  {
    oyExportStart_(EXPORT_CHECK_NO);
    oyExportEnd_();
    error = oySetPersistentString(TEST_DOMAIN TEST_KEY, oySCOPE_USER,
                                 "NULLTestValue", "NULLTestComment" );
    if(error)
      PRINT_SUB( oyjlTESTRESULT_XFAIL,
      "oySetPersistentString(%s) == %d", TEST_DOMAIN TEST_KEY, error );
    if(start && start[0]) r = oyjlTESTRESULT_SUCCESS; else r = oyjlTESTRESULT_XFAIL;
    start = oyGetPersistentString(TEST_DOMAIN TEST_KEY, 0, oySCOPE_USER_SYS, 0);
    printf ("start is %s\n", start);
    if(start && start[0]) r = oyjlTESTRESULT_SUCCESS; else r = oyjlTESTRESULT_XFAIL;
    PRINT_SUB( r,
    "DB not initialised? try oyExportStart_(EXPORT_CHECK_NO)" );
  }
  if(!(start && start[0]))
  {
    oyExportStart_(EXPORT_SETTING);
    oyExportEnd_();
    error = oySetPersistentString(TEST_DOMAIN TEST_KEY, oySCOPE_USER,
                                 "NULLTestValue", "NULLTestComment" );
    if(error)
      PRINT_SUB( oyjlTESTRESULT_XFAIL,
      "oySetPersistentString(%s) == %d", TEST_DOMAIN TEST_KEY, error );
    start = oyGetPersistentString(TEST_DOMAIN TEST_KEY, 0, oySCOPE_USER_SYS, 0);
    if(start && start[0]) r = oyjlTESTRESULT_SUCCESS; else r = oyjlTESTRESULT_XFAIL;
    PRINT_SUB( r, 
    "DB not initialised? try oyExportStart_(EXPORT_SETTING)" );
  }
  if(start && start[0])
    fprintf(zout, "start key value: %s\n", start );
  else
    fprintf(zout, "could not initialise\n" );

  error = oySetPersistentString(TEST_DOMAIN TEST_KEY, oySCOPE_USER,
                                 "myTestValue", "myTestComment" );
  value = oyGetPersistentString(TEST_DOMAIN TEST_KEY, 0, oySCOPE_USER_SYS, 0);
  if(value)
    fprintf(zout, "result key value: %s\n", value );

  if(error)
  {
    PRINT_SUB( oyjlTESTRESULT_SYSERROR, 
    "DB error: %d", error );
  } else
  /* we want "start" to be different from "value" */
  if(start && value && strcmp(start,value) == 0)
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "DB (start!=value) failed: %s|%s", start, value );
  } else
  if(!value)
  {
    if(!value)
      PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "DB (value) failed" );
    if(!(start && start[0]))
      PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "DB (init) failed" );
  } else
  if(value)
  {
    if(strcmp(value,"myTestValue") == 0)
    {
      PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
      "DB (value):                            %s", value );
    } else
    {
      PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "DB (value) wrong:                      %s", value );
    }
  }
  oyFree_m_( start );
  oyFree_m_( value );

  error = oyDBEraseKey( TEST_DOMAIN TEST_KEY, oySCOPE_USER );
  if(error)
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyDBEraseKey(%s)", TEST_DOMAIN TEST_KEY );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyDBEraseKey(%s)", TEST_DOMAIN TEST_KEY );
  }
  oyDB_s * db = oyDB_newFrom( TEST_DOMAIN, oySCOPE_USER_SYS, oyAllocateFunc_, oyDeAllocateFunc_ );
  value = oyDB_getString(db, TEST_DOMAIN TEST_KEY);
  oyDB_release( &db );
  if(value && strlen(value))
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "DB key not erased" );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "DB key erased" );
  }
  if(value) { oyFree_m_(value); }


  error = oySetPersistentString( OY_STD "/device" TEST_KEY "/[0]/key-01", oySCOPE_USER,
                                 "SomeValue", "SomeComment" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oySetPersistentString() error: %d", error )
  error = oySetPersistentString( OY_STD "/device" TEST_KEY "/[0]/key-02", oySCOPE_USER,
                                 "SomeValue", "SomeComment" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oySetPersistentString() error: %d", error )
  error = oySetPersistentString( OY_STD "/device" TEST_KEY "/[1]/key-01", oySCOPE_USER,
                                 "SomeValue", "SomeComment" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oySetPersistentString() error: %d", error )
  error = oySetPersistentString( OY_STD "/device" TEST_KEY "/[1]/key-02", oySCOPE_USER,
                                 "SomeValue", "SomeComment" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oySetPersistentString() error: %d", error )
  value = oyDBSearchEmptyKeyname(OY_STD "/device" TEST_KEY, oySCOPE_USER);
  if(value && strstr( value, OY_STD "/device" TEST_KEY "/[2]" ) )
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyDBSearchEmptyKeyname()=%s", value );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyDBSearchEmptyKeyname(%s)", OY_STD "/device" TEST_KEY );
  }
  if(value) oyFree_m_( value );

  error = oyDBEraseKey( OY_STD "/device" TEST_KEY, oySCOPE_USER );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDBEraseKey() error: %d", error )
  value = oyDBSearchEmptyKeyname(OY_STD "/device" TEST_KEY, oySCOPE_USER);
  if(value && strstr( value, OY_STD "/device" TEST_KEY "/[0]" ) )
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyDBSearchEmptyKeyname()=%s", value );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyDBSearchEmptyKeyname(%s)", OY_STD "/device" TEST_KEY );
  }
  oyFree_m_( value );

  error = oySetPersistentString( TEST_DOMAIN "/device" TEST_KEY "/[0]/key-01", oySCOPE_USER,
                                 "SomeValue", "SomeComment" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oySetPersistentString() error: %d", error )
  error = oySetPersistentString( TEST_DOMAIN "/device" TEST_KEY "/[0]/key-02", oySCOPE_USER,
                                 "SomeValue", "SomeComment" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oySetPersistentString() error: %d", error )
  error = oySetPersistentString( TEST_DOMAIN "/device" TEST_KEY "/[1]/key-01", oySCOPE_USER,
                                 "SomeValue", "SomeComment" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oySetPersistentString() error: %d", error )
  error = oySetPersistentString( TEST_DOMAIN "/device" TEST_KEY "/[1]/key-02", oySCOPE_USER,
                                 "SomeValue", "SomeComment" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oySetPersistentString() error: %d", error )
  value = oyDBSearchEmptyKeyname(TEST_DOMAIN "/device" TEST_KEY, oySCOPE_USER);
  if(value && strstr( value, TEST_DOMAIN "/device" TEST_KEY "/[2]" ) )
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyDBSearchEmptyKeyname()=%s", value );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyDBSearchEmptyKeyname()=%s", value );
  }
  oyFree_m_( value );

  error = oyDBEraseKey( TEST_DOMAIN "/device" TEST_KEY, oySCOPE_USER );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDBEraseKey() error: %d", error )
  value = oyDBSearchEmptyKeyname(TEST_DOMAIN "/device" TEST_KEY, oySCOPE_USER);
  if(value && strstr( value, TEST_DOMAIN "/device" TEST_KEY "/[0]" ) )
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyDBSearchEmptyKeyname()=%s", value );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyDBSearchEmptyKeyname()=%s", TEST_DOMAIN "/device" TEST_KEY );
  }

  char * key = 0;
  oyStringAddPrintf( &key, oyAllocateFunc_, oyDeAllocateFunc_,
                     "%s/array_key", value );
  error = oySetPersistentString( key, oySCOPE_USER_SYS,
                                 "ArrayValue", "ArrayComment" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oySetPersistentString() error: %d", error )
  oyFree_m_( value );
  value = oyGetPersistentString(strchr(key,'/')+1, 0, oySCOPE_USER_SYS, 0);
  if(value && strcmp(value, "ArrayValue") == 0)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oySetPersistentString(%s, oySCOPE_USER_SYS)", key );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oySetPersistentString(%s, oySCOPE_USER_SYS)", key );
  }
  error = oyDBEraseKey( key, oySCOPE_USER );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDBEraseKey() error: %d", error )
  oyFree_m_( key );
  oyFree_m_( value );

  // testing outside editing
  int old_effect_switch = oyGetBehaviour( oyBEHAVIOUR_EFFECT );
  oyStringAddPrintf( &value, oyAllocateFunc_, oyDeAllocateFunc_,
                     "%d", old_effect_switch >= 1 ? 0 : 1 );
  oyDBSetString( OY_DEFAULT_EFFECT, oySCOPE_USER, value,
                 "testing");
  /* clear the DB cache */
  oyGetPersistentStrings( NULL );
  int effect_switch = oyGetBehaviour( oyBEHAVIOUR_EFFECT );
  if(old_effect_switch != effect_switch)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyGetBehaviour() detected value change %d %s %d", old_effect_switch, value, effect_switch );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyGetBehaviour() detected value change %d %s %d", old_effect_switch, value, effect_switch );
  }
  oyFree_m_( value );
  // reset to old value
  oySetBehaviour( oyBEHAVIOUR_EFFECT, oySCOPE_USER, old_effect_switch );

  // test outside DB change
  char * old_daemon = oyGetPersistentString(OY_DEFAULT_DISPLAY_WHITE_POINT_DAEMON, 0, oySCOPE_USER_SYS, 0);
  int old_display_white_point = oyGetBehaviour( oyBEHAVIOUR_DISPLAY_WHITE_POINT );
  oyStringAddPrintf( &value, oyAllocateFunc_, oyDeAllocateFunc_,
                     "%d", old_display_white_point != 0 ? 0 : 7 );
  oyDBSetString( OY_DEFAULT_DISPLAY_WHITE_POINT_DAEMON, oySCOPE_USER,
                 old_daemon ? NULL : "oyranos-monitor-white-point",
                 "testing");
  oyDBSetString( OY_DEFAULT_DISPLAY_WHITE_POINT, oySCOPE_USER, value,
                 "testing");
  /* clear the DB cache */
  oyGetPersistentStrings( NULL );
  int display_white_point = oyGetBehaviour( oyBEHAVIOUR_DISPLAY_WHITE_POINT );
  if(verbose) fprintf(zout, "old_value: %d -> setting oyBEHAVIOUR_DISPLAY_WHITE_POINT: %s  check %d\n", old_display_white_point, value, display_white_point );
  oyFree_m_( value );

  // reset to old value
  oySetBehaviour( oyBEHAVIOUR_DISPLAY_WHITE_POINT, oySCOPE_USER, old_display_white_point );
  if(old_daemon)
  {
    oySetPersistentString( OY_DEFAULT_DISPLAY_WHITE_POINT_DAEMON, oySCOPE_USER, old_daemon, NULL);
    oyFree_m_(old_daemon);
  }
  
  return result;
}
oyjlTESTRESULT_e testDB()
{
  /* init the default DB handler */
  oySetPersistentString( TEST_DOMAIN TEST_KEY, oySCOPE_USER, NULL, NULL );
  fprintf(stdout, "%s\n", oyDbHandlingCurrent() );
  return testDBDefault();
}

oyjlTESTRESULT_e testDB2()
{
  OBJECT_COUNT_SETUP

  oy_prefered_db_ = "//" OY_TYPE_STD "/db_handler.oiDB";

  oyDbHandlingReset();

  /* clear the DB cache */
  oyGetPersistentStrings(NULL);
  /* init the new DB handler */
  oySetPersistentString( TEST_DOMAIN TEST_KEY, oySCOPE_USER, NULL, NULL );

  fprintf(stdout, "%s\n", oyDbHandlingCurrent() );

  oyjlTESTRESULT_e result = testDBDefault();

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

#ifdef __cplusplus
extern "C" {
#endif
char *         oyStrnchr_            ( char              * text,
                                       char                delimiter,
                                       int                 len );
int    oyStringSegmentsN_            ( const char        * text,
                                       int                 len,
                                       char                delimiter );
int    oyStringSegments_             ( const char        * text,
                                       char                delimiter );
char *             oyStringSegment_  ( char              * text,
                                       char                delimiter,
                                       int                 segment,
                                       int               * end );
char *             oyStringSegmentN_ ( char              * text,
                                       int                 len,
                                       char                delimiter,
                                       int                 segment,
                                       int               * end );
char *         oyFilterRegistrationToSTextField (
                                       const char        * registration,
                                       oyFILTER_REG_e      field,
                                       int               * end );

#ifdef __cplusplus
}
#endif

oyjlTESTRESULT_e testStringRun ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int error = 0,
      i;

  const char * test = OY_INTERNAL "/display.oydi/display_name_long";
  
  int test_n = oyStringSegmentsN_( test, oyStrlen_(test), OY_SLASH_C );
  test_n = oyStringSegments_( test, OY_SLASH_C );
  char * test_out = (char*) malloc(strlen(test));
  char * test_sub = 0;
  int test_end;

  if(verbose) fprintf(zout, "\"%s\"\n", test );

  error = 0;
  if(test_n != 5) error = 1;
  for(i = 0; !error && i < test_n; ++i)
  {
    int test_end = 0;
    test_sub = oyStringSegment_( (char*) test, OY_SLASH_C, i,
                                           &test_end );
    int test_sub_n = oyStringSegmentsN_( test_sub, test_end, '.' );

    switch(i) {
      case 0: if(test_sub_n != 1) error = 1; break;
      case 1: if(test_sub_n != 1) error = 1; break;
      case 2: if(test_sub_n != 1) error = 1; break;
      case 3: if(test_sub_n != 2) error = 1; break;
      case 4: if(test_sub_n != 1) error = 1; break;
      default: error = 1;
    }

    int j;
    for(j = 0; j < test_sub_n; ++j)
    {
      int test_end2 = 0;
      char * test_sub2 = oyStringSegmentN_( test_sub, test_end, '.', j,
                                               &test_end2 );
      memcpy( test_out, test_sub2, test_end2 );
      test_out[test_end2] = 0;
      switch(i) {
      case 0: if(strcmp(test_out,"org") != 0) error = 1; break;
      case 1: if(strcmp(test_out,"oyranos") != 0) error = 1; break;
      case 2: if(strcmp(test_out,"openicc") != 0) error = 1; break;
      case 3: if((j == 0 && strcmp(test_out,"display") != 0) || (j == 1 && strcmp(test_out,"oydi") != 0)) error = 1; break;
      case 4: if(strcmp(test_out,"display_name_long") != 0) error = 1; break;
      default: error = 1;
      }
      if(verbose || error)
        fprintf(zout, "%d%c%d%c \"%s\"\n", i, j?' ':'/',j, j ? '.': ' ',
                      test_out);
    }
  }

  if( !error )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyStringSegmentxxx()..." );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyStringSegmentxxx()..." );
    fprintf(zout, "\"%s\"\n", test );
  }


  double clck = oyClock();
  for(i = 0; i < 100000; ++i)
  {
    test_sub = oyFilterRegistrationToSTextField( test, oyFILTER_REG_OPTION,
                                                 &test_end );
    error = test_end != 17;
  }
  clck = oyClock() - clck;

  if( !error )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,(double)clck/(double)CLOCKS_PER_SEC,"pices",
    "oyFilterRegistrationToSTextField()    %d", i );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyFilterRegistrationToSTextField()" );
  }

  error = 0;
  clck = oyClock();
  for(i = 0; i < 100000; ++i)
  {
    test_sub = oyFilterRegistrationToText( test, oyFILTER_REG_OPTION, 0 );
    if(test_sub)
    { oyFree_m_(test_sub); }
    else
      error = 1;
  }
  clck = oyClock() - clck;

  if( !error )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,(double)clck/(double)CLOCKS_PER_SEC,"pices",
    "oyFilterRegistrationToText()          %d", i );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyFilterRegistrationToText())" );
  }



  test = "//" OY_TYPE_STD "/display.oydi/";
  if(verbose) fprintf(zout, "\"%s\"\n", test );
  test_n = oyStringSegmentsN_( test, oyStrlen_(test), OY_SLASH_C );
  test_n = oyStringSegments_( test, OY_SLASH_C );

  error = 0;
  if(test_n != 5) error = 1;
  for(i = 0; !error && i < test_n; ++i)
  {
    int test_end = 0;
    test_sub = oyStringSegment_( (char*) test, OY_SLASH_C, i,
                                           &test_end );
    int test_sub_n = oyStringSegmentsN_( test_sub, test_end, '.' );

    switch(i) {
      case 0: if(test_sub_n != 1) error = 1; break;
      case 1: if(test_sub_n != 1) error = 1; break;
      case 2: if(test_sub_n != 1) error = 1; break;
      case 3: if(test_sub_n != 2) error = 1; break;
      case 4: if(test_sub_n != 1) error = 1; break;
      default: error = 1;
    }

    int j;
    for(j = 0; j < test_sub_n; ++j)
    {
      int test_end2 = 0;
      char * test_sub2 = oyStringSegmentN_( test_sub, test_end, '.', j,
                                               &test_end2 );
      memcpy( test_out, test_sub2, test_end2 );
      test_out[test_end2] = 0;
      if(verbose)
        fprintf(zout, "%d%c%d%c \"%s\"\n", i, j?' ':'/', j, j ? '.': ' ',
                      test_out);
    }
  }
  free(test_out);

  if( !error )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyStringSegmentxxx()..." );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyStringSegmentxxx()..." );
  }

  test = OY_INTERNAL "/display.oydi/display_name";
  test_out = oyjlStringCopy(test,0);
  oyjlStringReplace( &test_out, OY_INTERNAL, OY_STD, 0,0 );
  //fprintf(zout, "test %s \"%s\"\n", test, test_out);
  if( strstr(test_out, OY_STD ) != NULL )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringReplace(start)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(start)" );
  }
  oyFree_m_(test_out);

  test_out = oyjlStringCopy(test,0);
  oyjlStringReplace( &test_out, "display.", "foo.", 0,0 );
  //fprintf(zout, "test %s \"%s\"\n", test, test_out);
  if( strstr(test_out, "foo" ) != NULL )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringReplace(middle)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(middle)" );
  }
  oyFree_m_(test_out);

  test_out = oyjlStringCopy(test,0);
  oyjlStringReplace( &test_out, "display_name", "bar", 0,0 );
  //fprintf(zout, "test %s \"%s\"\n", test, test_out);
  if( strstr(test_out, "bar" ) != NULL &&
      strstr(test_out, "barbar" ) == NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringReplace(end)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(end)" );
  }
  oyFree_m_(test_out);

  int list_n = 0, filt_n = 0, orig_n;
  const char * separateable_text = "org/domain/eins.lib;org/domain/eins.lib;org/domain/zwei.txt;org/domain/drei.lib;org/domain/zwei.txt;org/domain/eins.lib;net/welt/vier.lib;net/welt/vier.txt;/net/welt/fuenf;/net/welt/fuenf";
  char ** list = oyStringSplit( separateable_text, ';', &list_n, oyAllocateFunc_ );
  char ** filt = oyStringListFilter_( (const char**)list, list_n,
                                      "org/domain", NULL, "lib", &filt_n,
                                      oyAllocateFunc_ );
  orig_n = list_n;
  if( filt_n == 4 &&
      strcmp(filt[0], list[0] ) == 0 &&
      strcmp(filt[2], list[3] ) == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyStringListFilter(path=org/domain,suffix=lib)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyStringListFilter(path=org/domain,suffix=lib)" );
    for(i = 0; i < list_n; ++i)
      fprintf(zout, " list[%d] \"%s\"\n", i, list[i] );
    for(i = 0; i < filt_n; ++i)
      fprintf(zout, " filt[%d] \"%s\"\n", i, filt[i] );
  }
  oyStringListRelease_( &filt, filt_n, oyDeAllocateFunc_ );

  filt = oyStringListFilter_( (const char**)list, list_n,
                              NULL, "vier", "", &filt_n,
                              oyAllocateFunc_ );
  if( filt_n == 2 &&
      strcmp(filt[0], list[6] ) == 0 &&
      strcmp(filt[1], list[7] ) == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyStringListFilter(name=vier)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyStringListFilter(name=vier)" );
    for(i = 0; i < list_n; ++i)
      fprintf(zout, " list[%d] \"%s\"\n", i, list[i] );
    for(i = 0; i < filt_n; ++i)
      fprintf(zout, " filt[%d] \"%s\"\n", i, filt[i] );
  }
  oyStringListRelease_( &filt, filt_n, oyDeAllocateFunc_ );

  oyStringListFreeDoubles( list, &list_n, oyDeAllocateFunc_ );
  if( list_n == 6 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyStringListFreeDoubles()  %d/%d", orig_n, list_n );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyStringListFreeDoubles()" );
    for(i = 0; i < list_n; ++i)
      fprintf(zout, " list[%d] \"%s\"\n", i, list[i] );
  }

  oyStringListSetHeadingWhiteSpace( list, list_n, 4, oyAllocateFunc_,oyDeAllocateFunc_ );
  if(strcmp(list[0],"    org/domain/eins.lib") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyStringListSetHeadingWhiteSpace()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyStringListSetHeadingWhiteSpace()" );
    for(i = 0; i < list_n; ++i)
      fprintf(zout, " list[%d] \"%s\"\n", i, list[i] );
  }
  oyStringListRelease_( &list, list_n, oyDeAllocateFunc_ );

  char * t = NULL;
  size_t size = 0;
  error = oyStringFromData_( separateable_text,
                             strlen(separateable_text), &t,
                             &size, oyAllocateFunc_ );
  if( size == 186 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyStringFromData_()  %d", (int)size );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyStringFromData_()  %d", (int)size );
  }
  oyFree_m_(t);

  if(oyStringCaseCmp_("Lab","lab") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyStringCaseCmp_()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyStringCaseCmp_()" );
  }

  return result;
}

oyjlTESTRESULT_e testJson ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int i;
  const char * json = "{\"org\":{\"free\":[{\"s1key_a\":\"val_a\",\"s1key_b\":\"val_b\"},{\"s2key_c\":\"val_c\",\"s2key_d\":\"val_d\"}],\"key_e\":\"val_e_yyy\",\"key_f\":\"val_f\"}}";
  const char * t;

  if(verbose) fprintf( zout, "%s\n", json );

  oyjl_val value = 0;
  oyjl_val root = 0;
  for(i = 0; i < 5; ++i)
  {
    int level = 0;
    const char * xpath = NULL;
    char error_buffer[128];
    int flags = 0;

    root = oyjlTreeParse( json, error_buffer, 128 );
    if(!root) continue;

    switch(i) {
    case 1: xpath = "org/free/[1]"; break;
    case 2: xpath = "org/free/[1]/"; break;
    case 3: xpath = "/org/free/[1]"; break;
    case 4: xpath = "org/objects/new_one";
	    flags = OYJL_CREATE_NEW;
	    break;
    default: xpath = NULL; break;
    }

    if(!xpath)
    {
      char * json = 0;
      oyjlTreeToJson( root, &level, &json );
      t = oyjlTermColorToPlain( json, 0 );
      if(json && json[0] && strlen(t) == 210)
      { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(t),
        "oyjlTreeToJson()" );
        if(verbose) fprintf( zout, "%s\n", json );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlTreeToJson()" );
      }
      oyFree_m_(json);
      t = NULL;

      int count;
      char ** paths = oyjlTreeToPaths( root, 10, NULL, 0, &count );
      if(count == 10)
      { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
        "oyjlTreeToPaths()" );
      } else
      { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
        "oyjlTreeToPaths()" );
      }
      if(paths && count)
        oyStringListRelease( &paths, count, free );

      paths = oyjlTreeToPaths( root, 10, NULL, OYJL_KEY, &count );
      if(count == 6)
      { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
        "oyjlTreeToPaths( OYJL_KEY )" );
      } else
      { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
        "oyjlTreeToPaths( OYJL_KEY )" );
      }
      const char * match = NULL;
      const char * xpath = "org///s2key_d";
      for(int j = 0; j < count; ++j)
      {
        if(oyjlPathMatch( paths[j], xpath, 0 ))
          match = paths[j];
        if(verbose)
        fprintf( zout, "%d: %s\n", j, paths[j] );
      }
      if(match && strcmp(match,"org/free/[1]/s2key_d") == 0)
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "oyjlPathMatch(%s, %s)", match, xpath );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlPathMatch(%s, %s)", match, xpath );
      }
      if(paths && count)
        oyStringListRelease( &paths, count, free );

      paths = oyjlTreeToPaths( root, 10, NULL, OYJL_PATH, &count );
      if(count == 4)
      { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
        "oyjlTreeToPaths( OYJL_PATH )" );
      } else
      { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
        "oyjlTreeToPaths( OYJL_PATH )" );
      }
      if(verbose)
        for(int j = 0; j < count; ++j)
          fprintf( zout, "%d: %s\n", j, paths[j] );

      double clck = oyjlClock();
      const char * p = "org/free/[0]/s1key_b";
      int k = 0, n = 500;
      for(k = 0; k < n; ++k)
      {
        value = oyjlTreeGetValue( root, flags, p );
        char * t = oyjlValueText(value, oyAllocateFunc_);
        if(!t) break;
        oyFree_m_(t);
      }
      clck = oyjlClock() - clck;
      if( k == n )
      { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, n,clck/(double)CLOCKS_PER_SEC,"key",
        "oyjlValueText(%s)", p );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlValueText(%s) (%d) (%s)", p, k, value?"oyjlTreeGetValue() good":"oyjlTreeGetValue() failed" );
      }

      if(paths && count)
        oyStringListRelease( &paths, count, free );
    }
    if(xpath)
    {
      int success = 0;
      char * rjson = NULL;
      value = oyjlTreeGetValue( root, flags, xpath );
      if( value  )
      {
        if(i == 4)
        {
          oyjlTreeToJson( root, &level, &rjson );
        } else
          oyjlTreeToJson( value, &level, &rjson );
      }
      if(rjson && rjson[0])
        success = 1;
      else if(i == 2 || i == 3)
        success = 1;
      if(success)
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "oyjlTreeGetValue(flags=%d)", flags );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlTreeGetValue(flags=%d)", flags );
      }
      if(verbose)
        fprintf( zout, "%s xpath \"%s\" %s\n", value?"found":"found not", xpath, success?"ok":"" );
      if(rjson && rjson[0])
      {
        success = 1;
        if(verbose) fprintf( zout, "%s\n", rjson );
      }
      if(rjson) oyFree_m_(rjson);
      oyjlTreeFree( value );
    }

    oyjlTreeFree( root );
  }

  value = oyjlTreeGetValue( NULL, OYJL_CREATE_NEW, "not/existing" );
  if(!value)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeGetValue( NULL, flags=OYJL_CREATE_NEW, xpath ) == NULL" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeGetValue( NULL, flags=OYJL_CREATE_NEW, xpath ) == NULL" );
  }

  root = oyjlTreeNew( NULL );
  if(root)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeNew( NULL )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeNew( NULL )" );
  }
  oyjlTreeFree( root );

  root = oyjlTreeNew( "new/tree/key" );
  char * rjson = NULL; i = 0;
  oyjlTreeToJson( root, &i, &rjson ); i = 0;
  t = oyjlTermColorToPlain( rjson, 0 );
  size_t len = strlen(t);
  if(root && len == 56)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "oyjlTreeNew( \"new/tree/key\" )" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, len,
    "oyjlTreeNew( \"new/tree/key\" )" );
  }
  if(verbose) fprintf( zout, "%s\n", rjson );
  oyFree_m_( rjson );

  value = oyjlTreeGetValue( root, 0, "new/tree/key" );
  oyjlValueSetString( value, "value" );
  oyjlTreeToJson( root, &i, &rjson ); i = 0;
  t = oyjlTermColorToPlain( rjson, 0 );
  if(len < strlen(t))
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(t),
    "oyjlValueSetString( \"value\" )" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(t),
    "oyjlValueSetString( \"value\" )" );
  }
  if(verbose) fprintf( zout, "%s\n", rjson );
  len = strlen(t);
  oyFree_m_( rjson );

  char * v = oyjlValueText(value, oyAllocateFunc_);
  if(v && strlen(v) == 5)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlValueText( \"new/tree/key\" ) = \"%s\"", v );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlValueText( \"new/tree/key\" ) = \"value\"" );
  }
  oyFree_m_(v)

  oyjlTreeClearValue( root,"new/tree/key" );
  oyjlTreeToJson( root, &i, &rjson ); i = 0;
  t = oyjlTermColorToPlain( rjson, 0 );
  if(rjson && strcmp(t,"null") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeClearValue( \"new/tree/key\" )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeClearValue( \"new/tree/key\" )" );
  }
  if(verbose) fprintf( zout, "%s\n", rjson?rjson:"----" );
  oyjlTreeFree( root );
  oyFree_m_(rjson);

  return result;
}


oyjlTESTRESULT_e testOption ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  int error = 0;
  oyOption_s * o, * copy;
  const char * test_buffer = "test";
  size_t size = strlen(test_buffer);
  oyPointer ptr = oyAllocateFunc_( size );
  OBJECT_COUNT_SETUP

  oyExportReset_(EXPORT_SETTING);

  fprintf(stdout, "\n" );

  o = oyOption_FromRegistration( 0, testobj );
  if(o)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOption_New() good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOption_New() failed" );
  }

  error = oyOption_FromRegistration( "blabla", testobj ) != 0;
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOption_FromRegistration() error: %d", error )
  if(o)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOption_New() with wrong registration rejected: ok");
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOption_New() with wrong registration not rejected");
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

  oyFree_m_(ptr); ptr = NULL;

  if(!testobj)
    testobj = oyObject_NewWithAllocators( myAllocFunc, myDeAllocFunc, "testobj" );
  copy = oyOption_Copy( o, testobj );
  if(oyStruct_GetId((oyStruct_s*)o) != oyStruct_GetId((oyStruct_s*)copy))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOption_Copy()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOption_Copy()" );
  }

  oyOption_Release( &o );
  oyOption_Release( &copy );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

#include "oyOption_s_.h"

oyjlTESTRESULT_e testOptionInt ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  int error = 0;
  oyOption_s * o = 0;
  oyOption_s_ * oi = 0;
  int32_t erg[4] = { -1,-1,-1,-1 };
  OBJECT_COUNT_SETUP

  oyExportReset_(EXPORT_SETTING);

  fprintf(stdout, "\n" );

  o = oyOption_FromRegistration( "//" OY_TYPE_STD "/filter/x", testobj );
  oi = (oyOption_s_*) o;

  error = oyOption_SetFromInt( o, 0, 0, 0 );
  if(!error && oi->value &&
     oi->value->int32 == 0 &&
     oi->value_type == oyVAL_INT)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOption_SetFromInt() failed" );
  }

  error = oyOption_SetFromInt( o, 58293, 0, 0 );
  if(!error && oi->value &&
     oi->value->int32 == 58293 &&
     oi->value_type == oyVAL_INT)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() single int32_t good" );
    erg[0] = oyOption_GetValueInt( o, 0 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOption_SetFromInt() single int32_t failed" );
  }

  error = oyOption_SetFromInt( o, 58293, 1, 0 );
  if(!error && oi->value &&
     oi->value->int32_list[0] == 2 &&
     oi->value->int32_list[1] == 58293 &&
     oi->value->int32_list[2] == 58293 &&
     oi->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() add int32_t list good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOption_SetFromInt() add int32_t list failed" );
  }

  error = oyOption_SetFromInt( o, 58293, 2, 0 );
  if(!error && oi->value &&
     oi->value->int32_list[0] == 3 &&
     oi->value->int32_list[1] == 58293 &&
     oi->value->int32_list[2] == 58293 &&
     oi->value->int32_list[3] == 58293 &&
     oi->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() add int32_t list good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOption_SetFromInt() add int32_t list failed" );
  }

  error = oyOption_SetFromInt( o, 58293, 1, 0 );
  if(!error && oi->value &&
     oi->value->int32_list[0] == 3 &&
     oi->value->int32_list[1] == 58293 &&
     oi->value->int32_list[2] == 58293 &&
     oi->value->int32_list[3] == 58293 &&
     oi->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyOption_SetFromInt() new int32_t list good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOption_SetFromInt() new int32_t list failed" );
  }

  error = oyOption_SetFromInt( o, 293, 1, 0 );
  if(!error && oi->value &&
     oi->value->int32_list[0] == 3 &&
     oi->value->int32_list[1] == 58293 &&
     oi->value->int32_list[2] == 293 &&
     oi->value->int32_list[3] == 58293 &&
     oi->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() modify int32_t list good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOption_SetFromInt() modify int32_t list failed" );
  }

  error = oyOption_SetFromInt( o, 58293, 0, 0 );
  if(!error && oi->value &&
     oi->value->int32_list[0] == 3 &&
     oi->value->int32_list[1] == 58293 &&
     oi->value->int32_list[2] == 293 &&
     oi->value->int32_list[3] == 58293 &&
     oi->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() set int32_t list good" );
    erg[0] = oyOption_GetValueInt( o, 0 );
    erg[1] = oyOption_GetValueInt( o, 1 );
    erg[2] = oyOption_GetValueInt( o, 2 );
    erg[3] = oyOption_GetValueInt( o, 3 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOption_SetFromInt() set int32_t list failed" );
  }

  if(!error && erg[0] == 58293 && erg[1] == 293 && erg[2] == 58293 &&
               erg[3] == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOption_GetValueInt() good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOption_GetValueInt() failed" );
  }

  erg[0] = oyOption_GetValueInt( o, -1 );
  if(!error && erg[0] == 3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOption_GetValueInt(-1) == %d", erg[0] );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOption_GetValueInt(-1) == %d", erg[0] );
  }

  oyOption_Release( &o );


  o = oyOption_FromRegistration( "//" OY_TYPE_STD "/filter/y", testobj );
  oi = (oyOption_s_*) o;
  /*o->value_type = oyVAL_INT_LIST;*/
  error = oyOption_SetFromInt( o, 58293, 2, 0 );
  if(!error && oi->value &&
     oi->value->int32_list[0] == 3 &&
     oi->value->int32_list[1] == 0 &&
     oi->value->int32_list[2] == 0 &&
     oi->value->int32_list[3] == 58293 &&
     oi->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() explicite int32_t list good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOption_SetFromInt() explicite int32_t list failed" );
  }

  oyOption_Release( &o );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

oyjlTESTRESULT_e testOptionsSet ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  int error = 0;
  oyOptions_s * setA = 0;
  const char * t = NULL;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  error = oyOptions_SetFromString( &setA,
                                 "org/test/" OY_TYPE_STD "/filter/gamma_A",
                                 "1", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyOptions_SetFromString( &setA,
                                 "org/test/" OY_TYPE_STD "/filter/gamma_A1",
                                 "1", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyOptions_SetFromString( &setA,
                                 "org/test/" OY_TYPE_STD "/filter/gamma_A12",
                                 "1", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )

  if(!error && oyOptions_Count( setA ) == 3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_SetFromString() similiar registration good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOptions_SetFromString() similiar registration failed" );
  }

  error = oyOptions_SetFromString( &setA,
                                 "org/oyranos/" OY_TYPE_STD "/filter/gamma_A2",
                                 "one\ntwo\nthree\nfour",
                                 OY_CREATE_NEW | OY_STRING_LIST);
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  t = oyOptions_GetText( setA, oyNAME_NAME );
  if(t && t[0] && oyOptions_Count( setA ) == 4)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_GetText()                             good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOptions_GetText()                           failed" );
  }
  if(t && t[0])
  {
    oyOption_s * opt = oyOptions_Get( setA, 3 );
    OYJL_TEST_WRITE_RESULT( t, strlen(t), "oyOptions_GetText_oyNAME_NAME", "txt" )
    if(verbose) fprintf( zout, "%s\n", t );
    if(verbose) fprintf( zout, "fourth option\n" );
    char * t = oyOption_GetValueText(opt, malloc );
    if(verbose) fprintf( zout, "ValueText: %s\n", t );
    if(t) free(t);
    const char * text = oyOption_GetText(opt, oyNAME_NICK);
    if(strlen(text) == 51)
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_GetText(oyNAME_NICK)                  good" );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOptions_GetText(oyNAME_NICK) %d %s         failed", (int)strlen(text), text );
    }
    OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyOptions_GetText_oyNAME_NICK", "txt" )
    if(verbose) fprintf( zout, "NICK: %s\n", text );
    text = oyOption_GetText(opt, oyNAME_NAME);
    if(strlen(text) == 126)
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_GetText(oyNAME_NAME)                  good" );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOptions_GetText(oyNAME_NAME) %d %s         failed", (int)strlen(text), text );
    }
    OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyOptions_GetText_oyNAME_NAME", "txt" )
    if(verbose) fprintf( zout, "NAME: %s\n", text );
    text = oyOption_GetText(opt, oyNAME_DESCRIPTION);
    if(strlen(text) == 35)
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyOptions_GetText(oyNAME_DESCRIPTION)           good" );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_GetText(oyNAME_DESCRIPTION) %d %s  failed", (int)strlen(text), text );
    }
    OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyOptions_GetText_oyNAME_DESCRIPTION", "txt" )
    if(verbose) fprintf( zout, "DESCRIPTION: %s\n", text );

    oyOption_Release( &opt );
  }

  /* In the following code snippet every unique key shall be stored.
   * That is usually not desired for keeping the key set matchable.
   * Group keys shall be avoided. However for caching group keys are
   * useful.
   */
  oyOption_s * o;
  int found = 0;
  o = oyOptions_Find( setA, OY_STD "/filter", oyNAME_REGISTRATION );
  if(o)
    found = 1;
  else
    o = oyOption_FromRegistration( OY_STD "/filter", testobj );
  oyOption_SetFromString( o, "", 0 );
  if(found)
    oyOption_Release( &o );
  else
    oyOptions_MoveIn( setA, &o, -1 );

  found = 0;
  o = oyOptions_Find( setA, OY_STD "/filter/level_one", oyNAME_REGISTRATION );
  if(o)
    found = 1;
  else
    o = oyOption_FromRegistration( OY_STD "/filter/level_one", testobj );
  oyOption_SetFromString( o, "", 0 );
  if(found)
    oyOption_Release( &o );
  else
    oyOptions_MoveIn( setA, &o, -1 );

  found = 0;
  o = oyOptions_Find( setA, OY_STD "/filter/level_one/level_two", oyNAME_REGISTRATION );
  if(o)
    found = 1;
  else
    o = oyOption_FromRegistration( OY_STD "/filter/level_one/level_two", testobj );
  oyOption_SetFromString( o, "1", 0 );
  if(found)
    oyOption_Release( &o );
  else
    oyOptions_MoveIn( setA, &o, -1 );

  t = oyOptions_GetText( setA, oyNAME_NICK );
  if(t && t[0] && oyOptions_Count( setA ) == 7)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_GetText() hierarchical                good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_GetText() hierarchical              failed" );
  }
  if(verbose)
  fprintf( zout, "%s\n", t );

  oyOptions_Release( &setA );

  const char * json = "{\"org\":{\"free\":[{\"s1key_a\":\"val_a\",\"s1key_b\":\"val_b\"},{\"s2key_c\":\"val_c\",\"s2key_d\":\"val_d\"}],\"key_e\":\"val_e\"}}";
  oyOptions_s * options = NULL;
  error = oyOptions_FromJSON( json, options, &setA, "org" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_FromJSON() error: %d", error )
  int count = oyOptions_Count(setA);
  if(count == 1)
  {
    PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count, 
    "oyOptions_FromJSON() simple first" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
    "oyOptions_FromJSON() simple first" );
  }

  const char * json2 = "{\"org\":{\"free\":[{\"s1key_a\":\"val_a\",\"s1key_b\":\"val_b\"},{\"s2key_c\":\"val_c\",\"s2key_d\":\"val_d\"}],\"key_e\":\"val_e_xxx\"}}";
  error = oyOptions_FromJSON( json2, options, &setA, "org" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_FromJSON() error: %d", error )
  count = oyOptions_Count(setA);
  t = oyOptions_FindString(setA, "key_e",0);
  if(count == 1 && t && strcmp(t,"val_e_xxx") == 0)
  {
    PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
    "oyOptions_FromJSON() simple second" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
    "oyOptions_FromJSON() simple second" );
  }
  OYJL_TEST_WRITE_RESULT( t, strlen(t), "oyOptions_FromJSON-simple-second", "txt" )

  const char * json3 = "{\"org\":{\"free\":[{\"s1key_a\":\"val_a\",\"s1key_b\":\"val_b\"},{\"s2key_c\":\"val_c\",\"s2key_d\":\"val_d\"}],\"key_e\":\"val_e_yyy\",\"key_f\":\"val_f\"}}";
  error = oyOptions_FromJSON( json3, options, &setA, "org" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_FromJSON() error: %d", error )
  count = oyOptions_Count(setA);
  t = oyOptions_FindString(setA, "key_e",0);
  if(count == 2 && t && strcmp(t,"val_e_yyy") == 0)
  {
    PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count, 
    "oyOptions_FromJSON() simple thierd" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
    "oyOptions_FromJSON() simple thierd" );
  }
  OYJL_TEST_WRITE_RESULT( t, strlen(t), "oyOptions_FromJSON-simple-thierd", "txt" )

  oyOptions_SetFromString( &options, OY_STD "/key_path", 
                                   "org/host/path", OY_CREATE_NEW);
  error = oyOptions_FromJSON( json3, options, &setA, "org/free/[1]" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_FromJSON() error: %d", error )
  count = oyOptions_Count(setA);
  t = oyOptions_FindString(setA, "org/host/path/s2key_c",0);
  if(count == 4 && t && strcmp(t,"val_c") == 0)
  {
    PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
    "oyOptions_FromJSON() key_path" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
    "oyOptions_FromJSON() key_path" );
  }
  OYJL_TEST_WRITE_RESULT( t, strlen(t), "oyOptions_FromJSON-key_path", "txt" )
  t = oyOptions_GetText( setA, (oyNAME_e) oyNAME_JSON );
  if(verbose) fprintf( zout, "%s\n", t?t:0 );

  int state = 0;
  oyjl_val root = oyjlTreeParse2( t, 0, __func__, &state );
  const char * plain = oyjlTermColorToPlain(t, 0);
  if(root && strlen(plain) == 154)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_GetText(oyNAME_JSON)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_GetText(oyNAME_JSON) %d              %s", t?(int)strlen(plain):-1, oyjlPARSE_STATE_eToString( state ) );
  }
  OYJL_TEST_WRITE_RESULT( t, strlen(t), "oyOptions_GetText-JSON", "txt" )
  oyjlTreeFree( root );

  oyOptions_SetFromString( &options, OY_STD "/key_path", 
                                   "org/host/path2", OY_CREATE_NEW);
  error = oyOptions_FromJSON( json3, options, &setA, "org/free/[1]" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_FromJSON() error: %d", error )
  oyOptions_SetFromString( &options, OY_STD "/key_path", 
                                   "org/host/path2/four", OY_CREATE_NEW);
  error = oyOptions_FromJSON( json3, options, &setA, "org/free/[1]" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_FromJSON() error: %d", error )
  t = oyOptions_GetText( setA, (oyNAME_e) oyNAME_JSON );
  plain = oyjlTermColorToPlain(t, 0);
  oyOptions_Release( &options );
  count = oyOptions_Count(setA);
  if(count == 8 && t && strlen(plain) == 315)
  {
    PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(t),
    "oyOptions_FromJSON() key_paths               %d", count );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, t?strlen(t):0,
    "oyOptions_FromJSON() key_paths               %d", count );
    fprintf( zout, "%s\n", t?t:0 );
  }
  OYJL_TEST_WRITE_RESULT( t, strlen(t), "oyOptions_FromJSON-key_paths", "txt" )

  error = oyOptions_FromJSON( t, NULL, &options, "org" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_FromJSON() error: %d", error )
  t = oyOptions_GetText( options, (oyNAME_e) oyNAME_JSON );
  plain = oyjlTermColorToPlain(t, 0);
  if(!error && t && t && strlen(plain) == 60)
  {
    PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, t?strlen(plain):0,
    "oyOptions_FromJSON() validity" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, t?strlen(plain):0,
    "oyOptions_FromJSON() validity" );
    fprintf( zout, "%s\n", t?t:0 );
  }
  OYJL_TEST_WRITE_RESULT( t, strlen(t), "oyOptions_FromJSON-validity", "txt" )

  oyOptions_Release( &options );

  oyOptions_Release( &setA );

  options = oyOptions_FromText( json3, 0, testobj );
  t = oyOptions_GetText( options, (oyNAME_e) oyNAME_JSON );
  if(!error && t && t && strcmp(t, json3))
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_FromText( json ) roundtrip" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_FromText( json ) roundtrip" );
    fprintf( zout, "%s\n", t?t:0 );
  }
  OYJL_TEST_WRITE_RESULT( t, strlen(t), "oyOptions_FromText-roundtrip", "txt" )
  oyOptions_Release( &options );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

oyjlTESTRESULT_e testOptionsCopy ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  int error = 0;
  oyOptions_s * setA = 0, * setB = 0, * setC = 0,
              * resultA = 0, * resultB = 0;
  int32_t count = 0;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  error = oyOptions_SetFromString( &setA,
                OY_INTERNAL "/lcm2.color.icc/rendering_bpc.advanced",
                                 "1", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyOptions_SetFromString( &setA,
                                 "//" OY_TYPE_STD "/image/A", "true",
                                 OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyOptions_SetFromString( &setA,
                                 "//" OY_TYPE_STD "/image/A", "true",
                                 OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )

  if(!error && oyOptions_Count( setA ) == 2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_SetFromString() good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOptions_SetFromString() failed" );
  }

  error = oyOptions_SetFromString( &setB,
                                 "//" OY_TYPE_STD "/config/A", "true",
                                 OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyOptions_SetFromString( &setB,
                                 "//" OY_TYPE_STD "/config/B", "true",
                                 OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyOptions_SetFromString( &setB,
                                 "//" OY_TYPE_STD "/config/C", "true",
                                 OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )

  error = oyOptions_SetFromString( &setC,
                                 "//" OY_TYPE_STD "/config/B", "true",
                                 OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyOptions_SetFromString( &setC,
                                 "//" OY_TYPE_STD "/config/D", "true",
                                 OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyOptions_SetFromString( &setC,
                                 "//" OY_TYPE_STD "/config/C", "true",
                                 OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )

  if(verbose)
  {
    const char * t = oyOptions_GetText( resultA, (oyNAME_e) oyNAME_DESCRIPTION );
    const char * a = oyOptions_GetText( setA, (oyNAME_e) oyNAME_DESCRIPTION );
    fprintf( zout, "resultA/setA:\n----\n%s----\n%s----\n", t?t:"", a?a:"" );
  }
  error = oyOptions_CopyFrom( &resultA, setA, oyBOOLEAN_UNION,
                              oyFILTER_REG_NONE,0 );

  if(!error && oyOptions_Count( resultA ) == 2 &&
     oyOptions_FindString( resultA, "rendering_bpc", 0 ) &&
     oyOptions_FindString( resultA, "A", 0 ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyOptions_CopyFrom() oyBOOLEAN_UNION good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_CopyFrom() oyBOOLEAN_UNION failed" );
  }
  if(verbose)
  {
    const char * t = oyOptions_GetText( resultA, (oyNAME_e) oyNAME_DESCRIPTION );
    const char * a = oyOptions_GetText( setA, (oyNAME_e) oyNAME_DESCRIPTION );
    fprintf( zout, "resultA/setA:\n----\n%s----\n%s----\n", t?t:"", a?a:"" );
  }

  error = oyOptions_CopyFrom( &resultA, setB, oyBOOLEAN_DIFFERENZ,
                              oyFILTER_REG_OPTION,0 );

  if(!error && oyOptions_Count( resultA ) == 3 &&
     oyOptions_FindString( resultA, "rendering_bpc", 0 ) &&
     oyOptions_FindString( resultA, "B", 0 ) &&
     oyOptions_FindString( resultA, "C", 0 ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_CopyFrom() oyBOOLEAN_DIFFERENZ good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOptions_CopyFrom() oyBOOLEAN_DIFFERENZ failed" );
  }


  error = oyOptions_Filter( &resultB, &count, 0, oyBOOLEAN_INTERSECTION,
                  OY_INTERNAL "/image", setA  );

  if(!error && oyOptions_Count( resultB ) == 1 &&
     oyOptions_FindString( resultB, "A", 0 ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_Filter() oyBOOLEAN_INTERSECTION good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOptions_Filter() oyBOOLEAN_INTERSECTION failed" );
  }

  // Add similar options the crude way
  oyOption_s * option = oyOption_FromRegistration(OY_STD"/behaviour/rendering_bpc", testobj);
  oyOptions_MoveIn( setA, &option, -1 );
  option = oyOption_FromRegistration(OY_STD"/behaviour/rendering_bpc", testobj);
  oyOptions_MoveIn( setA, &option, -1 );
  if(!error && oyOptions_Count( setA ) == 4)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_MoveIn()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOptions_MoveIn()" );
  }

  // Add sensible: skip existing option
  count = oyOptions_Count( setA );
  if(verbose)
    fprintf( zout, "setA:%d\n", count );
  option = oyOption_FromRegistration(OY_STD"/behaviour/rendering_bpc", testobj);
  error = oyOptions_Add( setA, option, -1, testobj );
  if(error == -2 && oyOptions_Count( setA ) == 4)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_Add(duplicates unfiltered)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOptions_Add(duplicates unfiltered)" );
  }
  if(verbose)
  {
    const char * t = oyOptions_GetText( setA, (oyNAME_e) oyNAME_DESCRIPTION );
    fprintf( zout, "setA:%d %d %d\n%s\n", t?(int)strlen(t):0, error, oyOptions_Count( setA ), t?t:"" );
  }
  oyOption_Release( &option );

  option = oyOption_FromRegistration(OY_STD"/behaviour/not_seen_before", testobj);
  error = oyOptions_Add( setA, option, -1, testobj );
  if(error == 0 && oyOptions_Count( setA ) == count + 1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_Add(unique)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOptions_Add(unique)" );
  }
  if(verbose)
  {
    const char * t = oyOptions_GetText( setA, (oyNAME_e) oyNAME_DESCRIPTION );
    fprintf( zout, "setA:%u %d %d\n%s\n", t?(int)strlen(t):0, error, oyOptions_Count( setA ), t?t:0 );
  }

  // Add many options the crude way
  int countA = oyOptions_Count( setA );
  error = oyOptions_Add( setB, option, -1, testobj );
  int countB = oyOptions_Count( setB );
  oyOptions_AppendOpts( setA, setB );
  count = oyOptions_Count( setA );
  if(countA == 5 && countB == 4 && count == countA + countB)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_AppendOpts(unique + duplicates unfiltered)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOptions_AppendOpts(unique + duplicates unfiltered)" );
  }

  oyOptions_Release( &resultA );
  resultA = oyOptions_FromBoolean( setA, setC, oyBOOLEAN_UNION, testobj );
  countA = oyOptions_Count( setA );
  int countC = oyOptions_Count( setC );
  count = oyOptions_Count( resultA );
  if(countA == 9 && countC == 3 && count == 6)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_FromBoolean(oyBOOLEAN_UNION)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOptions_FromBoolean(oyBOOLEAN_UNION)" );
  }
  if(verbose)
  {
    const char * t = oyOptions_GetText( resultA, (oyNAME_e) oyNAME_DESCRIPTION );
    const char * a = oyOptions_GetText( setA, (oyNAME_e) oyNAME_DESCRIPTION );
    const char * c = oyOptions_GetText( setC, (oyNAME_e) oyNAME_DESCRIPTION );
    fprintf( zout, "setA:%d %d\n----\n%s----\n%s----\n%s----\n", t?(int)strlen(t):0, count, a?a:"", c?c:"", t?t:"" );
  }
  oyOptions_Release( &resultA );


  oyOption_Release( &option );

  oyOptions_Release( &setA );
  oyOptions_Release( &setB );
  oyOptions_Release( &setC );
  oyOptions_Release( &resultA );
  oyOptions_Release( &resultB );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

#include "oyBlob_s.h"

oyjlTESTRESULT_e testBlob ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  int error = 0;
  oyBlob_s * a = 0, * b = 0;
  oyPointer ptr = 0;
  const char static_ptr[16] = {0,1,0,1,0,1,0,1,  0,1,0,1,0,1,0,1};
  const char type[8] = "test";
  oyObject_s object = oyObject_New("testBlobobject");
  const char * t;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  a = oyBlob_New( testobj );
  ptr = (oyPointer) static_ptr;
  error = oyBlob_SetFromStatic( a, ptr, 16, type );

  if(!error)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyBlob_SetFromSatic() good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyBlob_SetFromSatic() failed" );
  }

  t = oyObject_Show(object);
  fprintf( zout, "oyObject_Show(testBlobobject) %s\n", t );
  t = oyObject_Show(a->oy_);
  fprintf( zout, "oyObject_Show(oyBlob_s) %s", t );

  b = oyBlob_Copy( a, object );

  if(!error && b && oyBlob_GetPointer(b) && oyBlob_GetSize(b) &&
     oyBlob_GetPointer( b ) == static_ptr)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyBlob_Copy( static ) good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyBlob_Copy( static ) failed" );
  }

  error = oyBlob_Release( &b );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyBlob_Release() error: %d", error )

  error = oyBlob_SetFromStatic( a, ptr, 0, type );

  if(!error)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyBlob_SetFromSatic(0) good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyBlob_SetFromSatic(0) failed" );
  }

  b = oyBlob_Copy( a, object );

  if(!error && b && oyBlob_GetPointer(b) && !oyBlob_GetSize(b) &&
     oyBlob_GetPointer(b) == static_ptr)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyBlob_Copy( static 0) good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyBlob_Copy( static 0) failed" );
  }

  ptr = malloc(1024);
  error = oyBlob_SetFromData( a, ptr, 1024, type );

  if(!error && oyBlob_GetPointer(a) && oyBlob_GetSize(a) == 1024 &&
     oyBlob_GetPointer(a) != ptr)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyBlob_SetFromData() good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyBlob_SetFromData() failed" );
  }
  
  error = oyBlob_Release( &b );

  if(!error && !b)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyBlob_Release() good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyBlob_Release() failed" );
  }

  b = oyBlob_Copy( a, object );

  if(!error && b && a != b && oyBlob_GetPointer(b) &&
     oyBlob_GetSize(a) == oyBlob_GetSize(b) &&
     oyBlob_GetPointer(a) != oyBlob_GetPointer(b) )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyBlob_Copy() good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyBlob_Copy() failed" );
  }

  oyBlob_Release( &a );
  oyBlob_Release( &b );
  oyObject_Release( &object );
  free(ptr);

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

int mySignal( oyObserver_s *observer, oySIGNAL_e signal_type, oyStruct_s *signal_data )
{
  if(verbose || oy_debug_signals)
  {
    const char * desc = oyStruct_GetText(signal_data, oyNAME_NAME, 0);
    fprintf( zout, "%s - %s\n", oySignalToString(signal_type),
                                oyNoEmptyString_m_(desc) );
  }
  return 0;
}

#include "oyStructList_s.h"
oyjlTESTRESULT_e testDAGbasic ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  oyBlob_s * blob = NULL;
  oyOptions_s * options = NULL;
  int error = 0;

  oyStructList_s * list1 = oyStructList_New( testobj ),
                 * list2 = oyStructList_New( testobj ),
                 * tmp = oyStructList_Copy( list2, NULL ),
                 * tmp_list1 = list1; // do not use plain pointer assignment without oyXXX_Copy() referencing like above
  oyStructList_MoveIn(list1, (oyStruct_s**)&tmp, -1, 0 );
  tmp = oyStructList_Copy( list1, NULL );
  oyStructList_MoveIn(list2, (oyStruct_s**)&tmp, -1, 0 );
  oyStructList_Release( &list1 );
  oyStructList_Release( &list2 );

  if(oy_debug_objects >= 0 || oy_debug_objects <= -2)
  {
    const char * msg = "Circular DAG";
    oyLibConfigRelease(FINISH_IGNORE_OBJECT_LIST | FINISH_IGNORE_CORE);
    object_count = oyObjectCountCurrentObjectIdList();
    if(object_count != 2)
    {
      PRINT_SUB_INT( oyjlTESTRESULT_XFAIL, object_count,
        "%s:", msg?msg:"objects");
    } else
    { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, object_count,
        "%s:", msg?msg:"objects" );
    }

    if(verbose)
    {
      char * text = oyBT(-1);
      oyObjectTreePrint( 0x01 | 0x02 | 0x08, text ? text : __func__ );
      oyFree_m_( text )
    }
  }
  oyStructList_Release( &tmp_list1 );
  if(oy_debug_objects >= 0 || oy_debug_objects <= -2)
    oyLibConfigRelease(0);

  blob = oyBlob_New( testobj );
  options = oyOptions_New( testobj );
  error = oyOptions_ObserverAdd( options, (oyStruct_s*)blob,
                                 0, NULL );
  if(verbose)
  {
    char * text = oyBT(-1);
    oyObjectTreePrint( 0x01 | 0x02 | 0x08, text ? text : __func__ );
    oyFree_m_( text )
  }
  oyOptions_Release( &options );
  oyBlob_Release( &blob );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "Blob Observer" )

  blob = oyBlob_New( testobj );
  oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/var1", "1",
                           OY_CREATE_NEW );
  oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/var2", "2",
                           OY_CREATE_NEW );
  error = oyOptions_ObserverAdd( options, (oyStruct_s*)blob,
                                 (oyStruct_s*)blob, mySignal );
  if(verbose)
  {
    char * text = oyBT(-1);
    oyObjectTreePrint( 0x01 | 0x02 | 0x08, text ? text : __func__ );
    oyFree_m_( text )
  }
  oyBlob_Release( &blob );
  oyOptions_Release( &options );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 1, "Blob Options" )

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
#ifdef HAVE_LIBXML2
  xmlDocPtr doc = 0;
#endif
  OBJECT_COUNT_SETUP

  oyExportReset_(EXPORT_SETTING);

  fprintf(zout, "\n" );

  opts = oyOptions_FromText( "{ \"org\": { \"oyranos\": { \"openicc\": { \"my_opt\": \"value\" } }, \"other\": { \"outside\": \"value2\" } } }", 0, testobj );
  const char * reg = "org/oyranos/openicc/icc_color._lcm2._icc_version_2._icc_version_4._CPU._NOACCEL._effect";
  char * type_txt  = oyFilterRegistrationToText( reg, oyFILTER_REG_TYPE, 0 );
  error = oyOptions_DoFilter( opts, oyOPTIONATTRIBUTE_ADVANCED, type_txt );
  count = oyOptions_Count( opts );
  if(count == 1 && error == 0)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_DoFilter()" );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOptions_DoFilter()" );
  }
  oyFree_m_(type_txt);
  oyOptions_Release( &opts );

  /* we check for our standard CMM */
  opts = oyOptions_ForFilter( "//" OY_TYPE_STD "/lcm2",
                                            oyOPTIONATTRIBUTE_ADVANCED /* |
                                            oyOPTIONATTRIBUTE_FRONT |
                                            OY_SELECT_COMMON */, testobj );

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
      if(verbose)
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
      "oyOptions_FindString() could not find option    " );
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


  text = oyStringCopy(oyOptions_GetText( opts, verbose ? oyNAME_NAME : (oyNAME_e)oyNAME_JSON ), oyAllocateFunc_);

  {
    if(!text || !strlen(text))
    {
      PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "oyOptions_GetText() returned no text" );
    } else
    {
      PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(text),
      "oyOptions_GetText() returned text" );
    }
  }


  oyOptions_Release( &opts );

  /* Roundtrip test */
  opts = oyOptions_FromText( text, 0, testobj );
  countB = oyOptions_Count( opts );

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
  
  for( i = 0; i < countB; ++i)
  {
    char * t, *t2, *t3;
    o = oyOptions_Get( opts, i );
    t = oyStringCopy(oyOption_GetText(o, oyNAME_DESCRIPTION), oyAllocateFunc_);
    t2 = oyOption_GetValueText( o, malloc );
    t3 = oyFilterRegistrationToText( oyOption_GetText( o, oyNAME_DESCRIPTION),
                                     oyFILTER_REG_OPTION, 0 );
    if(verbose)
    fprintf(zout,"%d: \"%s\": \"%s\" %s %u\n", i, 
           t, t2, t3,
           (unsigned int)((oyOption_s_*)o)->flags );

    oyOption_Release( &o );
    oyFree_m_( t );
    oyFree_m_( t2 );
    oyFree_m_( t3 );
  }

  char * t = text;
  text = oyStringAppend_( "<a>\n", t, 0 );
  oyFree_m_( t );
  oyStringAdd_( &text, "</a>", 0, 0 );

#ifdef HAVE_LIBXML2
  if(verbose)
  {
    doc = xmlParseMemory( text, oyStrlen_( text ) );
    error = !doc;
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
    oyFree_m_( text );

#ifdef LIBXML_WRITER_ENABLED
    xmlDocDumpFormatMemory( doc, (xmlChar**)&text, &i, 1 );
    if(verbose)
      fprintf(zout,"xmlDocDump: %s\n", text);
#endif
    xmlFreeDoc( doc );
    free(text);
    /*xmlSaveDoc( ptr, doc );*/
  }
  else
    oyFree_m_( text );
#endif

  oyOptions_Release( &opts );

  opts = oyOptions_ForFilter( "//" OY_TYPE_STD "/lcm2",
                                            oyOPTIONATTRIBUTE_ADVANCED  |
                                            oyOPTIONATTRIBUTE_FRONT |
                                            OY_SELECT_COMMON, testobj );
  if(verbose)
    fprintf(zout,"Show advanced common front end options:\n");
  countB = oyOptions_Count( opts );
  for( i = 0; i < countB; ++i)
  {
    char * t, *t2, *t3;
    o = oyOptions_Get( opts, i );
    t = oyStringCopy(oyOption_GetText(o, oyNAME_DESCRIPTION), oyAllocateFunc_);
    t2 = oyOption_GetValueText( o, malloc );
    t3 = oyFilterRegistrationToText( oyOption_GetText( o, oyNAME_DESCRIPTION),
                                     oyFILTER_REG_OPTION, 0 );
    if(verbose)
    fprintf(zout,"%d: \"%s\": \"%s\" %s %u\n", i, 
           t, t2, t3,
           (unsigned int)((oyOption_s_*)o)->flags );

    oyFree_m_( t );
    oyFree_m_( t2 );
    oyFree_m_( t3 );
    oyOption_Release( &o );
  }
  oyOptions_Release( &opts );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )


  return result;
}

#include "oyranos_helper.h"
oyjlTESTRESULT_e testInterpolation ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  uint16_t ramp[4] = { 0, 21845, 43690, 65535 };
  float pol;

  fprintf(stdout, "\n" );

  pol = oyLinInterpolateRampU16( ramp, 4, 0.499999 );
  if(pol < ramp[3]/2.0)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "Interpolation is fine %f", pol );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "Interpolation failed: %f < %f", pol, ramp[3]/2.0 );
  }

  pol = oyLinInterpolateRampU16( ramp, 4, 0.5 );
  if(pol == ramp[3]/2.0)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "Interpolation is fine." );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "Interpolation failed: %f == %f", pol, ramp[3]/2.0 );
  }

  pol = oyLinInterpolateRampU16( ramp, 4, 2.0/3.0 );
  if(pol == ramp[2])
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "Interpolation is fine." );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "Interpolation failed: %f == %u", pol, ramp[2] );
  }

  pol = oyLinInterpolateRampU16( ramp, 4, -1 );
  if(pol == 0.0)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "Interpolation is fine." );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "Interpolation failed: %f == 0.0", pol );
  }

  pol = oyLinInterpolateRampU16( ramp, 4, 1.1 );
  if(pol == ramp[3])
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "Interpolation is fine." );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "Interpolation failed: %f == %u", pol, ramp[3] );
  }

  uint16_t ramp3[12] = { 0,0,65535, 21845,21845,43690, 43690,43690,21845, 65535,65535,0 };
  pol = oyLinInterpolateRampU16c( ramp3, 4, 1, 3, 2.0/3.0 );
  if(pol == ramp3[2*3+1] &&
     oyLinInterpolateRampU16c( ramp3, 4, 0, 3, 2.0/3.0 ) == ramp3[2*3+0])
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "Interpolation c1 is fine: %g == %u", pol, ramp3[2*3+1] );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "Interpolation c failed: %f == %u", pol, ramp3[2*3+1] );
  }
  pol = oyLinInterpolateRampU16c( ramp3, 4, 2, 3, 2.0/3.0 );
  if(pol == ramp3[2*3+2])
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "Interpolation c2 is fine: %g == %u", pol, ramp3[2*3+2] );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "Interpolation c failed: %f == %u", pol, ramp3[2*3+2] );
  }

  float rampF[4] = { 0, 21845, 43690, 65535 };
  pol = oyLinInterpolateRampF32( rampF, 4, 2.0/3.0 );
  if(pol == rampF[2])
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "Interpolation is fine." );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "Interpolation failed: %f == %f", pol, rampF[2] );
  }

  return result;
}

#include "oyProfile_s.h"
oyjlTESTRESULT_e testOptionsType ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  int error = 0;

  fprintf(stdout, "\n" );

  oyOptions_s * os = oyOptions_New(testobj);
  oyOptions_Release( &os );

  oyProfile_s * p = oyProfile_FromStd( oyASSUMED_WEB, 0,testobj );
  oyOptions_s * opts = oyOptions_New(testobj);
  oyOptions_MoveInStruct ( &opts, OY_STD "/display.abstract.icc_profile.white_point.automatic.oydi",
                           (oyStruct_s**)&p, OY_CREATE_NEW );
  p = oyProfile_FromStd( oyASSUMED_WEB, 0,0 );
  oyOptions_MoveInStruct ( &opts, OY_STD "/display.abstract.icc_profile.gamma.automatic.oydi",
                           (oyStruct_s**)&p, OY_CREATE_NEW );
  if(verbose)
    fprintf( zout, "opts: %s\n", oyOptions_GetText( opts, oyNAME_NICK ) );
  p = oyProfile_FromStd( oyASSUMED_WEB, 0,0 );
  oyOptions_MoveInStruct ( &opts, OY_STD "/display.abstract.icc_profile.environment.automatic.my_app.passive",
                           (oyStruct_s**)&p, OY_CREATE_NEW );
  oyOption_s * o = NULL;
  error = oyOptions_GetType2( opts, -1, "abstract", oyNAME_PATTERN,
                                  oyOBJECT_PROFILE_S, NULL, &o );
  const char * reg = oyOption_GetRegistration( o );
  char * reg_mod = oyjlStringCopy( reg, 0 );
  oyjlStringReplace( &reg_mod, ".oydi", ".my", 0,0 );
  oyjlStringReplace( &reg_mod, ".automatic", "", 0,0 );
  oyStringAdd_( &reg_mod, ".passive", 0,0 );
  oyOption_SetRegistration( o, reg_mod );
  if(verbose)
    fprintf( zout, "%s -> %s :: %s\n", reg, reg_mod, oyOption_GetRegistration(o) );

  if(error == 0 &&
     strcmp(reg_mod, oyOption_GetRegistration(o)) == 0)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyOptions_GetType2() + replace + SetRegistration" );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_GetType2() + replace + SetRegistration" );
  }
  oyOption_Release( &o );
  oyFree_m_(reg_mod);

  int count = oyOptions_CountType( opts, "display.abstract.icc_profile", oyOBJECT_PROFILE_S );
  for(int i = 0; i < count; ++i)
  {
    error = oyOptions_GetType2( opts, i, "display.abstract.icc_profile", oyNAME_PATTERN,
                                oyOBJECT_PROFILE_S, NULL, &o );
    if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_GetType2() error: %d", error )
    reg = oyOption_GetRegistration( o );
    p = (oyProfile_s*) oyOption_GetStruct( o, oyOBJECT_PROFILE_S );
    if(verbose)
      fprintf( zout, "display.abstract.icc_profile[%d]: %s:%s\n", i,
               reg, oyProfile_GetText(p,oyNAME_DESCRIPTION) );
    oyOption_Release( &o );
    oyProfile_Release( &p );
  }

  if(verbose)
    fprintf( zout, "abstract: %d  white_point: %d\n",
             oyOptions_CountType( opts, "abstract", oyOBJECT_PROFILE_S ),
             oyOptions_CountType( opts, "white_point", oyOBJECT_PROFILE_S ) );

  oyOptions_Release( &opts );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

#include "oyranos_icc.h"

oyjlTESTRESULT_e testProfile ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  size_t size = 0;
  oyPointer data;
  oyProfile_s * p_a,
              * p_b;
  uint32_t icc_profile_flags = OY_ICC_VERSION_2;
  OBJECT_COUNT_SETUP

  oyExportReset_(EXPORT_SETTING);

  fprintf(stdout, "\n" );

  p_a = oyProfile_FromStd ( oyASSUMED_WEB, 0, testobj );
  if(!p_a)
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "No assumed WEB profile found" );
  } else
  {
    const char * t = oyProfile_GetText(p_a, (oyNAME_e) oyNAME_JSON);
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "found oyASSUMED_WEB" );
    if(verbose)
    {
      fprintf( zout, "oyNAME_NICK: %s\n", oyProfile_GetText(p_a, oyNAME_NICK) );
      fprintf( zout, "oyNAME_NAME: %s\n", oyProfile_GetText(p_a, oyNAME_NAME) );
      fprintf( zout, "oyNAME_DESCRIPTION: %s\n", oyProfile_GetText(p_a, oyNAME_DESCRIPTION) );
      fprintf( zout, "oyNAME_JSON: %s\n", t );
    }
  }


  /* compare the usual conversion profiles with the total of profiles */
  data = oyProfile_GetMem( p_a, &size, 0, malloc );
  if(!data || ! size)
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "Could get memory from profile" );
  } else
  {
    PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, size,
    "Obtained memory block from oyASSUMED_WEB:" );
  }

  p_b = oyProfile_FromMem( size, data, 0,testobj );
  if(!p_b)
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "Could not load profile from memory." );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "Loaded profile from memory of oyASSUMED_WEB" );
  }
  if(data)
  {
    free(data);
    data = NULL; size = 0;
  }

  if(!oyProfile_Equal( p_a, p_b ))
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "Reload of oyASSUMED_WEB failed. Unexplained difference." );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyASSUMED_WEB is equal to memory loaded oyProfile_s." );
  }

  oyProfile_Release( &p_b );
  oyProfile_Release( &p_a );
  p_a = oyProfile_FromStd ( oyPROFILE_EFFECT, 0, testobj );
  char * name = oyGetDefaultProfileName ( oyPROFILE_EFFECT, oyAllocateFunc_ );
  oyjlTESTRESULT_e re;
  if(!p_a)
  {
    if(name && name[0]) re = oyjlTESTRESULT_XFAIL; else re = oyjlTESTRESULT_SUCCESS;
    PRINT_SUB( re, 
    "No default effect profile found" );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "found oyPROFILE_EFFECT" );
  }
  oyProfile_Release( &p_a );
  int error = oySetDefaultProfile( oyPROFILE_EFFECT, oySCOPE_USER, name );
  if(!error)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oySetDefaultProfile(oyPROFILE_EFFECT)" );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oySetDefaultProfile(oyPROFILE_EFFECT)" );
  }
  if(name) oyFree_m_( name );

  /* get D50 */
  double XYZ[3] = {-1.0, -1.0, -1.0};
  error = oyGetDisplayWhitePoint( 2, XYZ );
  if(!error && XYZ[0] != -1.0 && XYZ[1] != -1.0 && XYZ[2] != -1.0)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyGetDisplayWhitePoint() %g %g %g mode[2]", XYZ[0], XYZ[1], XYZ[2] );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyGetDisplayWhitePoint() %g %g %g mode[2]", XYZ[0], XYZ[1], XYZ[2] );
  }
  XYZ[0] = XYZ[1] = XYZ[2] = -1.0;

  /* obtain first monitor white point */
  error = oyGetDisplayWhitePoint( 7, XYZ );
  if(!error && XYZ[0] != -1.0 && XYZ[1] != -1.0 && XYZ[2] != -1.0)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyGetDisplayWhitePoint() %g %g %g mode[7] %d   ", XYZ[0], XYZ[1], XYZ[2], error );
  } else if( displayFail() == oyjlTESTRESULT_FAIL )
  {
    PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyGetDisplayWhitePoint() %g %g %g mode[7] %d   ", XYZ[0], XYZ[1], XYZ[2], error );
  }

  double XYZold[3] = {-1, -1, -1};
  error = oyGetDisplayWhitePoint( 1, XYZold );
  /* set a custom white point */
  XYZ[0] = XYZ[1] = XYZ[2] = 0.333;
  oySetDisplayWhitePoint( XYZ, oySCOPE_USER, "test white point" );
  memset(XYZ,0,sizeof(double)*3);
  error = oyGetDisplayWhitePoint( 1, XYZ );
  if(!error && XYZ[0] == 0.333 && XYZ[1] == 0.333 && XYZ[2] == 0.333)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyGetDisplayWhitePoint() %g %g %g mode[1]", XYZ[0], XYZ[1], XYZ[2] );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyGetDisplayWhitePoint() %g %g %g mode[1]", XYZ[0], XYZ[1], XYZ[2] );
  }
  oySetDisplayWhitePoint( XYZold, oySCOPE_USER, "old automatic wtpt" );

  oyOption_s * matrix = oyOption_FromRegistration("///color_matrix."
              "from_primaries."
              "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma", NULL );
  {
    /* http://www.color.org/chardata/rgb/rommrgb.xalter
     * original gamma is 1.8, we adapt to typical cameraRAW gamma of 1.0 */
      oyOption_SetFromDouble( matrix, 0.7347, 0, 0);
      oyOption_SetFromDouble( matrix, 0.2653, 1, 0);
      oyOption_SetFromDouble( matrix, 0.1596, 2, 0);
      oyOption_SetFromDouble( matrix, 0.8404, 3, 0);
      oyOption_SetFromDouble( matrix, 0.0366, 4, 0);
      oyOption_SetFromDouble( matrix, 0.0001, 5, 0);
      oyOption_SetFromDouble( matrix, 0.3457, 6, 0);
      oyOption_SetFromDouble( matrix, 0.3585, 7, 0);
  }
  oyOption_SetFromDouble( matrix, 1.8, 8, 0);

  oyOptions_s * opts = oyOptions_New(testobj),
              * r = 0;

  oyOptions_SetFromInt( &opts, "///icc_profile_flags", icc_profile_flags, 0, OY_CREATE_NEW );
  oyOptions_MoveIn( opts, &matrix, -1 );
  const char * reg = "//" OY_TYPE_STD "/create_profile.color_matrix.icc";
  oyOptions_Handle( reg, opts, "create_profile.icc_profile.color_matrix",
                    &r );

  oyProfile_s * p = NULL;
  p = (oyProfile_s*)oyOptions_GetType( r, -1, "icc_profile",
                                               oyOBJECT_PROFILE_S );
  oyOptions_Release( &r );

# define ICC_TEST_NAME "TEST ROMM gamma 1.0"
  oyProfile_AddTagText( p, icSigProfileDescriptionTag,
                           ICC_TEST_NAME );

  oyOptions_Release( &opts );

  if(!p )
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyOptions_Handle( \"create_profile.icc_profile.color_matrix\")" );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyOptions_Handle( \"create_profile.icc_profile.color_matrix\")" );
  }

  icSignature vs = (icUInt32Number) oyValueUInt32( (icUInt32Number) oyProfile_GetSignature( p, oySIGNATURE_VERSION ) );
  char * v = (char*)&vs;
  if( v[0] == 2 )
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyOptions_Handle( \"create_profile.icc_profile.color_matrix\") %d.%d.%d", (int)v[0], (int)v[1]/16, (int)v[1]%16 );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_Handle( \"create_profile.icc_profile.color_matrix\") version: %d.%d.%d", (int)v[0], (int)v[1]/16, (int)v[1]%16 );
  }

  FILE * fp = fopen( ICC_TEST_NAME".icc", "rb" );
  if(fp)
  {
    fclose(fp); fp = 0;
    int r OY_UNUSED = remove( ICC_TEST_NAME".icc" );
  }

  size = 0;
  data = oyProfile_GetMem( p, &size, 0, malloc );
  error = oyWriteMemToFile_( ICC_TEST_NAME".icc", data, size );
  if(!error )
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyWriteMemToFile_( \"%s\")", ICC_TEST_NAME".icc" );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyWriteMemToFile_( \"%s\")", ICC_TEST_NAME".icc" );
  }

  if(size >= 128 &&
     oyCheckProfileMem( data, 128, 0 ) == 0)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyCheckProfileMem( \"%d\")", (int)size );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyCheckProfileMem( \"%d\")", (int)size );
  }
  if(data) { free(data); data = NULL; }

  icColorSpaceSignature csp = (icColorSpaceSignature) oyProfile_GetSignature( p, oySIGNATURE_COLOR_SPACE );
  const char * t = oyICCColorSpaceGetName( csp );
  if(t && t[0])
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyICCColorSpaceGetName() %s", oyNoEmptyString_m_(t) );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyICCColorSpaceGetName() %s", oyNoEmptyString_m_(t) );
  }
  t = oyICCColorSpaceGetChannelName( csp, 0, oyNAME_NAME);
  if(t && t[0])
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyICCColorSpaceGetChannelName(0) %s", oyNoEmptyString_m_(t) );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyICCColorSpaceGetChannelName(0) %s", oyNoEmptyString_m_(t) );
  }

  icProfileClassSignature cl = (icProfileClassSignature) oyProfile_GetSignature( p, oySIGNATURE_CLASS );
  t = oyICCDeviceClassDescription( cl );
  if(t && t[0])
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyICCDeviceClassDescription() %s", oyNoEmptyString_m_(t) );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyICCDeviceClassDescription() %s", oyNoEmptyString_m_(t) );
  }

  icPlatformSignature plf = (icPlatformSignature) oyProfile_GetSignature( p, oySIGNATURE_PLATFORM );
  t = oyICCPlatformDescription( plf );
  if(t && t[0])
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyICCPlatformDescription() %s", oyNoEmptyString_m_(t) );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyICCPlatformDescription() %s", oyNoEmptyString_m_(t) );
  }

  icSignature cmm = oyProfile_GetSignature( p, oySIGNATURE_CMM );
  t = oyICCCmmDescription( cmm );
  if(t && t[0])
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyICCCmmDescription() %s", oyNoEmptyString_m_(t) );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyICCCmmDescription() %s", oyNoEmptyString_m_(t) );
  }
  oyProfile_Release( &p );

  const char * fn = "ITULab.icc";
  error = oyCheckProfile(fn, 0);
  if(!error)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyCheckProfile(%s) %d", fn, error );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyCheckProfile(%s) %d", fn, error );
  }

  size = oyGetProfileSize( fn );
  if(size > 128)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyGetProfileSize(%s) %d", fn, (int)size );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyGetProfileSize(%s) %d", fn, (int)size );
  }
  char * pn = oyGetPathFromProfileName( fn, oyAllocateFunc_ );
  if(pn)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyGetPathFromProfileName(%s) %s", fn, oyNoEmptyString_m_(pn) );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyGetPathFromProfileName(%s) %s", fn, oyNoEmptyString_m_(pn) );
  }

  p = oyProfile_FromFile( ICC_TEST_NAME".icc", OY_SKIP_NON_DEFAULT_PATH, testobj );
  if(!p )
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyProfile_FromFile( OY_SKIP_NON_DEFAULT_PATH )" );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyProfile_FromFile( OY_SKIP_NON_DEFAULT_PATH )" );
  }
  oyProfile_Release( &p );

  const char * names[] = {"sRGB.icc",
                          "sRGB",
                          "7fb30d688bf82d32a0e748daf3dba95d",
                          "web" };
  for(int i = 0; i < 4; ++i)
  {
    p = oyProfile_FromName( names[i], icc_profile_flags, NULL );

    icSignature vs = (icUInt32Number) oyValueUInt32( (icUInt32Number) oyProfile_GetSignature( p, oySIGNATURE_VERSION ) );
    char * v = (char*)&vs;
    const char * hash = NULL;

    if(i == 2)
      hash = oyProfile_GetText( p, (oyNAME_e)oyNAME_REGISTRATION );
    if( p && v[0] == 2 && !(i == 2 && strcmp(names[2], hash) != 0))
    {
      PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyProfile_FromName( \"%s\") version: %d.%d.%d", names[i], (int)v[0], (int)v[1]/16, (int)v[1]%16 );
    } else
    {
      PRINT_SUB( oyjlTESTRESULT_FAIL,
      "oyProfile_FromName( \"%s\") version: %d.%d.%d", names[i], (int)v[0], (int)v[1]/16, (int)v[1]%16 );
    }

    oyProfile_Release( &p );
  }

  union { char c[8]; icUInt64Number ic; } test64 = { .c = {'T','e','s','t',0,0,0,0} };
  union { char c[8]; uint64_t lu; } test = { .c = {'T','e','s','t',0,0,0,0} };
  test.lu = oyValueUInt64( test64.ic );
  if( memcmp(test.c,"Test",4) == 0 || memcmp(test.c,"tseT",4) == 0 )
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyValueUInt64( \"Test\")  = %c %c %c %c %c %c %c %c", test.c[0], test.c[1], test.c[2], test.c[3], test.c[4], test.c[5], test.c[6], test.c[7] );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyValueUInt64( \"Test\")  = %c %c %c %c %c %c %c %c", test.c[0], test.c[1], test.c[2], test.c[3], test.c[4], test.c[5], test.c[6], test.c[7] );
  }

  union { unsigned char c[4]; icUInt32Number ic; uint32_t lu; } test32 = { .c = {'T','e','s','t'} };
  test32.lu = oyValueUInt32( test32.ic );
  if( memcmp(test32.c,"Test",4) == 0 || memcmp(test32.c,"tseT",4) == 0 )
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyValueUInt32( \"Test\")  = %c %c %c %c", test32.c[0], test32.c[1], test32.c[2], test32.c[3] );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyValueUInt32( \"Test\")  = %c %c %c %c", test32.c[0], test32.c[1], test32.c[2], test32.c[3] );
  }
  test32.ic = oyByteSwapUInt32( test32.ic );
  if( memcmp(test32.c,"Test",4) == 0 || memcmp(test32.c,"tseT",4) == 0 )
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyByteSwapUInt32( \"Test\")  = %c %c %c %c", test32.c[0], test32.c[1], test32.c[2], test32.c[3] );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyByteSwapUInt32( \"Test\")  = %c %c %c %c", test32.c[0], test32.c[1], test32.c[2], test32.c[3] );
  }

  union { unsigned char c[2]; icUInt16Number ic; } test16 = { .c = {'T','e'} };
  test16.ic = oyByteSwapUInt16( test16.ic );
  if( memcmp(test16.c,"eT",2) == 0 )
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyByteSwapUInt16( \"Te\")  = %c %c", test16.c[0], test16.c[1] );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyByteSwapUInt16( \"Te\")  = %c %c", test16.c[0], test16.c[1] );
  }


  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

#include "oyProfiles_s.h"
#include "oyranos_conversion.h"

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
  OBJECT_COUNT_SETUP

  oyExportReset_(EXPORT_SETTING);

  fprintf(stdout, "\n" );

  /* compare the usual conversion profiles with the total of profiles */
  profs = oyProfiles_ForStd( oyDEFAULT_PROFILE_START, 0, &current, testobj );
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

  if((int)size < count)
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyProfileListGet() returned less than oyDEFAULT_PROFILE_START %u|%d", (unsigned int)size, count );
  } else if(count)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyProfileListGet and oyDEFAULT_PROFILE_START %u|%d", (unsigned int)size, count );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyProfileListGet and/or oyDEFAULT_PROFILE_START are wrong %u|%d", (unsigned int)size, count );
  }

  /* compare the default profile spaces with the total of profiles */
  countB = 0;
  /* thread testing needs initialisation through oyThreadLockingSet()
#pragma omp parallel for private(current,count,p,tmp,profs) */
  for(i = oyEDITING_XYZ; i <= oyEDITING_GRAY; ++i)
  {
    profs = oyProfiles_ForStd( (oyPROFILE_e)i, 0, &current, testobj );

    count = oyProfiles_Count( profs );
    countB += count;
    if(!count)
    {
      PRINT_SUB( i == oyEDITING_CMYK ? oyjlTESTRESULT_XFAIL : oyjlTESTRESULT_FAIL, 
      "No profiles found for oyPROFILE_e %d", i );
    } else
    {
      p = oyProfiles_Get( profs, current );
      tmp = oyProfile_GetText( p, oyNAME_DESCRIPTION );
      oyProfile_Release( &p );
      PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "profiles found for oyPROFILE_e %d: %d \"%s\"", i, count, tmp ? tmp :"" );
    }

    oyProfiles_Release( &profs );
  }
  if((int)size < countB)
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyProfileListGet() returned less than oyPROFILE_e %u|%d", (unsigned int)size, countB );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyProfileListGet and oyPROFILE_e ok %u|%d     ", (unsigned int)size, countB );
  }
  oyProfile_s * pattern = oyProfile_FromFile( "sRGB", OY_NO_LOAD, testobj );
  oyProfiles_s* patterns = oyProfiles_New(testobj);
  oyProfiles_MoveIn( patterns, &pattern, -1 );
  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  profs = oyProfiles_Create( patterns, icc_profile_flags, testobj );
  count = oyProfiles_Count( profs );
  if((int)size > count && count)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyProfiles_Create( pattern = \"sRGB\" )     ok %u|%d", (unsigned int)size, count );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyProfiles_Create( pattern = \"sRGB\" )        %u|%d", (unsigned int)size, count );
  }
  oyProfiles_Release( &profs );
  oyProfile_Release( &pattern );
  oyProfiles_Release( &patterns );


  char * text = oyGetInstallPath( oyPATH_ICC, oySCOPE_SYSTEM, oyAllocateFunc_ );
  pattern = oyProfile_FromFile( text, OY_NO_LOAD, testobj );
  if(verbose)
  fprintf( zout, "oyPATH_ICC::oySCOPE_SYSTEM=%s\n", text );
  oyFree_m_(text);
  patterns = oyProfiles_New(testobj);
  oyProfiles_MoveIn( patterns, &pattern, -1 );
  profs = oyProfiles_Create( patterns, icc_profile_flags, NULL );
  count = oyProfiles_Count( profs );
  if((int)size >= count && count)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyProfiles_Create( pattern = system/icc ) ok %u|%d", (unsigned int)size, count );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyProfiles_Create( pattern = system/icc )    %u|%d", (unsigned int)size, count );
  }
  oyProfiles_Release( &profs );
  oyProfile_Release( &pattern );
  oyProfiles_Release( &patterns );

  {
    // Put all ICC Display Class profiles in "profiles"
    icSignature profile_class = icSigDisplayClass;
    oyProfile_s * pattern = 0;
    oyProfiles_s * patterns = oyProfiles_New( 0 ),
                 * profiles = 0;
    uint32_t icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( 
                                      OY_CMM_STD, "//" OY_TYPE_STD "/icc_color",
                                                                     NULL, 0 );

    // only display profiles
    pattern = oyProfile_FromSignature( profile_class, oySIGNATURE_CLASS, testobj );
    oyProfiles_MoveIn( patterns, &pattern, -1 );

    // ... and only profiles installed in system paths
    char * text = oyGetInstallPath( oyPATH_ICC, oySCOPE_SYSTEM, oyAllocateFunc_ );
    pattern = oyProfile_FromFile( text, OY_NO_LOAD, testobj );
    oyFree_m_( text );
    oyProfiles_MoveIn( patterns, &pattern, -1 );

    profiles = oyProfiles_Create( patterns, icc_profile_flags, testobj );
    oyProfiles_Release( &patterns );

    count = oyProfiles_Count( profiles );
    if((int)size >= count && count)
    {
      PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyProfiles_Create( pattern = system/icc ) ok %u|%d", (unsigned int)size, count );
    } else
    {
      PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "oyProfiles_Create( pattern = system/icc )    %u|%d", (unsigned int)size, count );
    }
    oyProfiles_Release( &profiles );
  }

  {
    // Get all ICC profiles, which can be used as assumed RGB profile
    uint32_t icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( 
                                      OY_CMM_STD, "//" OY_TYPE_STD "/icc_color",
                                                                     NULL, 0 );
    oyProfiles_s * p_list = oyProfiles_ForStd( oyASSUMED_RGB,
                                               icc_profile_flags, 0,testobj );
    int32_t * rank_list = (int32_t*) malloc( oyProfiles_Count(p_list) *
                                             sizeof(int32_t) );
    oyRankMap * rank_map = 0;
    const char * rank_map_text = "{\"org\":{\"freedesktop\":{\"openicc\":{\"rank_map\":{\"meta\":[{\"STANDARD_space\": [2,-1,0,\"matching\",\"not matching\",\"key not found\"]}]}}}}}";
    oyRankMapFromJSON( rank_map_text, NULL, &rank_map, malloc );

    const char * filter_text = "{\"org\":{\"freedesktop\":{\"openicc\":{\"config\":{\"meta\":[{\"STANDARD_space\": \"rgb\"}]}}}}}",
               * filter_registration = "org/freedesktop/openicc/config/meta";
    oyConfig_s * config = NULL;
    oyConfig_FromJSON( filter_registration, filter_text, 0,0, &config );
    oyConfig_SetRankMap( config, rank_map );
    oyRankMapRelease( &rank_map, free );

    // Sort the profiles according to eaches match to a given device
    oyProfiles_Rank( p_list, config, '/', ',', OY_MATCH_SUB_STRING |
		     OY_SYNTAX_SKIP_PATTERN | OY_SYNTAX_SKIP_REG, rank_list );

    int n = oyProfiles_Count( p_list );
    if(verbose)
    for(i = 0; i < n; ++i)
    {
      oyProfile_s * temp_prof = oyProfiles_Get( p_list, i );
      // Show the rank value, the profile internal and file names on the command line
      printf("%d %d: \"%s\" %s\n", rank_list[i], i,
             oyProfile_GetText( temp_prof, oyNAME_DESCRIPTION ),
             oyProfile_GetFileName(temp_prof, 0));
      oyProfile_Release( &temp_prof );
    } // Done print
    oyFree_m_( rank_list );

    if(n)
    {
      PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, n,
      "oyProfiles_Rank( rgb )" );
    } else
    {
      PRINT_SUB_INT( oyjlTESTRESULT_XFAIL, n,
      "oyProfiles_Rank( rgb )" );
    }
    oyProfiles_Release( &p_list );
    oyConfig_Release( &config );
  }

  {
    // Get all ICC profiles, which can be used as desaturating effect profiles
    uint32_t icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( 
                                      OY_CMM_STD, "//" OY_TYPE_STD "/icc_color",
                                                                     NULL, 0 );
    oyProfiles_s * patterns = oyProfiles_New( testobj ),
                 * profiles = 0;

    // only linear effects
    oyProfile_s * pattern = oyProfile_FromFile( "meta:EFFECT_linear;yes", OY_NO_LOAD, testobj );
    oyProfiles_MoveIn( patterns, &pattern, -1 );

    profiles = oyProfiles_Create( patterns, icc_profile_flags, testobj );
    oyProfiles_Release( &patterns );

    count = oyProfiles_Count( profiles );
    if((int)size > count && count)
    {
      PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyProfiles_Create( meta:EFFECT_linear;yes )  %u|%d", (unsigned int)size, count );
    } else
    {
      PRINT_SUB( oyjlTESTRESULT_XFAIL, 
      "oyProfiles_Create( meta:EFFECT_linear;yes )  %u|%d", (unsigned int)size, count );
    }
    oyProfiles_Release( &profiles );
  }

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}


oyjlTESTRESULT_e testProfileLists ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
#ifdef _WIN32
  int repeat = 100;
#else
  int repeat = 20;
#endif

  fprintf(stdout, "\n" );

  double clck = oyClock();

  uint32_t ref_count = 0;
  char ** reference = oyProfileListGet(0, &ref_count, myAllocFunc);

  if((int)ref_count)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyProfileListGet() returned profiles %d", (int)ref_count );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyProfileListGet() zero" );
  }

  int i,j;
  for(i = 0; i < repeat; ++i)
  {
    uint32_t count = 0;
    char ** names = oyProfileListGet(0, &count, myAllocFunc);
    if(count != ref_count)
    {
      PRINT_SUB( oyjlTESTRESULT_FAIL, 
      ": wrong profile count: %u/%u", (unsigned int)count, (unsigned int)ref_count );
    }
    for(j = 0; j < (int)count; ++j)
    {
      if(!(names[j] && strlen(names[j])) ||
         strcmp( names[j], reference[j] ) != 0 )
      {
        PRINT_SUB( oyjlTESTRESULT_FAIL, 
        "\n no profile name found: run %d profile #%d", i , j );
      }
      if( names[j] )
#ifdef USE_NEW
        delete [] names[j];
#else
        free(names[j]);
#endif
    }
#ifdef USE_NEW
    if( names ) delete [] names;
    std::cout << "." << std::flush;
#else
    if( names ) free( names );
    fprintf(zout, "." ); fflush(zout);
#endif

  }

  clck = oyClock() - clck;
  oyjlStringListRelease( &reference, ref_count, myDeAllocFunc );

#ifdef __cplusplus
  std::cout << std::endl;
  std::cout << repeat << " + 1 calls to oyProfileListGet()    "
            << oyjlProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "calls")
            << std::endl;
#else
  fprintf(zout, "\n%d + 1 calls to oyProfileListGet()         %s\n",
                repeat,  oyjlProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "calls") );
#endif

  int n = 0;
  char ** path_names =  oyProfilePathsGet( &n, oyAllocateFunc_ );
  if(n)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyProfilePathsGet() %d", n );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyProfilePathsGet() %d", n );
  }

  return result;
}

#include "oyProfile_s_.h"           /* oyProfile_ToFile_ */
#include "oyranos_color.h"
#include "oyranos_color_internal.h" /* oyGetTemperature() */
#include "oyranos_monitor_effect.h" /* oyProfile_CreateEffectVCGT() */

oyjlTESTRESULT_e testEffects ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  oyOptions_s * opts,
              * result_opts = 0;
  const char * text = 0;
  oyProfile_s * prof,
              * abstract;
  int error;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  opts = oyOptions_New(testobj);
  prof = oyProfile_FromStd( oyEDITING_CMYK, 0, testobj );

  error = oyOptions_MoveInStruct( &opts, "//" OY_TYPE_STD "/icc_profile.proofing_profile",
                                  (oyStruct_s**) &prof, OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_MoveInStruct() error: %d", error )
  error = oyOptions_Handle( "//" OY_TYPE_STD "/create_profile.proofing_effect",
                            opts,"create_profile.proofing_effect",
                            &result_opts );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_Handle() error: %d", error )
  abstract = (oyProfile_s*)oyOptions_GetType( result_opts, -1, "icc_profile",
                                              oyOBJECT_PROFILE_S );
  oyOptions_Release( &result_opts );
  oyOptions_Release( &opts );

  text = oyProfile_GetText( abstract, oyNAME_DESCRIPTION );

  if(abstract)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyOptions_Handle(\"create_profile\"): %s", text );
  } else if(error == -1)
  {
    PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyOptions_Handle(\"create_profile\") no" );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_Handle(\"create_profile\") zero" );
  }

  oyProfile_ToFile_( (oyProfile_s_*)abstract, "test_proof_effect.icc" );
  oyProfile_Release( &abstract );


  double XYZ[3] = {-1, -1, -1};
  double Lab[3], cie_a, cie_b, dst_cie_a, dst_cie_b;
  int count = 10,i;
  double clck = oyClock();
  for(i = 0; i < count; ++i)
  {
    error = oyGetDisplayWhitePoint( 2, XYZ );
    oyXYZ2Lab( XYZ, Lab );
    cie_a = Lab[1]/256.0+0.5; cie_b = Lab[2]/256.0+0.5;
    error = oyGetDisplayWhitePoint( 5, XYZ );
    oyXYZ2Lab( XYZ, Lab );
    dst_cie_a = Lab[1]/256.0+0.5; dst_cie_b = Lab[2]/256.0+0.5;
    error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/cie_a",
                                     dst_cie_a - cie_a, 0, OY_CREATE_NEW );
    error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/cie_b",
                                     dst_cie_b - cie_b, 0, OY_CREATE_NEW );
    error = oyOptions_Handle( "//" OY_TYPE_STD "/create_profile.white_point_adjust.lab",
                              opts,"create_profile.white_point_adjust.lab",
                              &result_opts );
    abstract = (oyProfile_s*)oyOptions_GetType( result_opts, -1, "icc_profile",
                                                oyOBJECT_PROFILE_S );
    if(i+1 < count)
      oyProfile_Release( &abstract );
  }
  clck = oyClock() - clck;
  if(verbose) fprintf(zout,"source white point cie_ab %g %g -> %g %g\n", cie_a, cie_b, dst_cie_a, dst_cie_b );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_Handle() error: %d", error )
  oyOptions_Release( &result_opts );
  oyOptions_Release( &opts );

  text = oyProfile_GetText( abstract, oyNAME_DESCRIPTION );

  if(abstract)
  {
    PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, count,clck/(double)CLOCKS_PER_SEC,"Prof",
    "oyOptions_Handle(\"create_profile\"): %s", text );
  } else if(error == -1)
  {
    PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyOptions_Handle(\"create_profile\") no" );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_Handle(\"create_profile\") zero" );
  }
  oyProfile_ToFile_( (oyProfile_s_*)abstract, "test_wtpt_effect-lab.icc" );
  oyProfile_Release( &abstract );


  clck = oyClock();
  double        src_XYZ[3] = {0.0, 0.0, 0.0}, dst_XYZ[3] = {0.0, 0.0, 0.0};
  count = 1000;
  for(i = 0; i < count; ++i)
  {
    error = oyGetDisplayWhitePoint( 2, src_XYZ );
    error = oyGetDisplayWhitePoint( 5, dst_XYZ );
    error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/src_iccXYZ", src_XYZ[0], 0, OY_CREATE_NEW );
    error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/src_iccXYZ", src_XYZ[1], 1, OY_CREATE_NEW );
    error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/src_iccXYZ", src_XYZ[2], 2, OY_CREATE_NEW );
    error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/illu_iccXYZ", dst_XYZ[0], 0, OY_CREATE_NEW );
    error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/illu_iccXYZ", dst_XYZ[1], 1, OY_CREATE_NEW );
    error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/illu_iccXYZ", dst_XYZ[2], 2, OY_CREATE_NEW );
    if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromDouble() error: %d", error )
    error = oyOptions_Handle( "//" OY_TYPE_STD "/create_profile.white_point_adjust.bradford",
                              opts,"create_profile.white_point_adjust.bradford.file_name",
                              &result_opts );
    if(i+1 < count)
      oyOptions_Release( &result_opts );
    oyOptions_Release( &opts );
  }
  clck = oyClock() - clck;
  if(verbose) fprintf(zout,"white point XYZ %g %g %g -> %g %g %g\n", src_XYZ[0], src_XYZ[1], src_XYZ[2], dst_XYZ[0], dst_XYZ[1], dst_XYZ[2] );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_Handle() error: %d", error )
  text = oyOptions_FindString(result_opts, "file_name", 0);
  if(text)
  {
    PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, count,clck/(double)CLOCKS_PER_SEC,"Prof",
    "oyOptions_Handle(\"create_profile.file_name\"): %s", text );
  } else if(error == -1)
  {
    PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyOptions_Handle(\"create_profile.file_name\") no" );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_Handle(\"create_profile.filename\") zero" );
  }
  oyOptions_Release( &result_opts );

  clck = oyClock();
  count = 100;
  for(i = 0; i < count; ++i)
  {
    error = oyGetDisplayWhitePoint( 2, src_XYZ );
    error = oyGetDisplayWhitePoint( 5, dst_XYZ );
    error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/src_iccXYZ", src_XYZ[0], 0, OY_CREATE_NEW );
    error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/src_iccXYZ", src_XYZ[1], 1, OY_CREATE_NEW );
    error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/src_iccXYZ", src_XYZ[2], 2, OY_CREATE_NEW );
    error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/illu_iccXYZ", dst_XYZ[0], 0, OY_CREATE_NEW );
    error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/illu_iccXYZ", dst_XYZ[1], 1, OY_CREATE_NEW );
    error = oyOptions_SetFromDouble( &opts, "//" OY_TYPE_STD "/illu_iccXYZ", dst_XYZ[2], 2, OY_CREATE_NEW );
    error = oyOptions_Handle( "//" OY_TYPE_STD "/create_profile.white_point_adjust.bradford",
                              opts,"create_profile.white_point_adjust.bradford",
                              &result_opts );
    if(i+1 < count)
      oyOptions_Release( &result_opts );
    oyOptions_Release( &opts );
  }
  clck = oyClock() - clck;
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_Handle() error: %d", error )
  abstract = (oyProfile_s*)oyOptions_GetType( result_opts, -1, "icc_profile",
                                              oyOBJECT_PROFILE_S );
  oyOptions_Release( &result_opts );
  oyOptions_Release( &opts );

  /* "Bradford Bluish CIE*a -0.00979487 CIE*b -0.10989" */
  text = oyProfile_GetText( abstract, oyNAME_DESCRIPTION );
  if(abstract && strlen(text) >= 49)
  {
    PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, count,clck/(double)CLOCKS_PER_SEC,"Prof",
    "oyOptions_Handle(\"create_profile\"): %s", text );
  } else if(abstract)
  {
    PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyOptions_Handle(\"create_profile\") %s", text );
  } else if(error == -1)
  {
    PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyOptions_Handle(\"create_profile\") no" );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_Handle(\"create_profile\") zero" );
  }

  oyProfile_ToFile_( (oyProfile_s_*)abstract, "test_wtpt_effect-bradford.icc" );
  oyProfile_Release( &abstract );

  /* would need a profile containing VCGT */
  oyProfile_s * p = oyProfile_FromStd( oyEDITING_RGB, 0, testobj );
  int found = oyExistPersistentString( OY_DISPLAY_STD "/night", "1", 0, oySCOPE_USER_SYS );
  fprintf( zout, "night is \"1\" %s\n", found?"on":"off" );
  error = oyProfile_CreateEffectVCGT(p);
  if(error == 0)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyProfile_CreateEffectVCGT() error = %d", error );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyProfile_CreateEffectVCGT() error = %d", error );
  }
  oyProfile_Release( &p );

  error = oyGetDisplayWhitePoint( 2 /* D50 */, src_XYZ );
  if(!error)
    error = oyGetDisplayWhitePoint( 6 /* D95 */, dst_XYZ );
  oyMAT3 wtpt_adapt;
  oyCIEXYZ srcWtpt = {src_XYZ[0], src_XYZ[1], src_XYZ[2]},
           dstIllu = {dst_XYZ[0], dst_XYZ[1], dst_XYZ[2]};
  error = !oyAdaptationMatrix( &wtpt_adapt, NULL, &srcWtpt, &dstIllu );
  oyMAT3 CHAD = { { { {1,0,0} },
                    { {0,1,0} },
                    { {0,0,1} } } };
  const char * t = oyMAT3show( &CHAD );
  if(t)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyMAT3show(oyMAT3)" );
    fprintf( zout, "%s", t );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyMAT3show(oyMAT3)" );
  }

  double temperature = oyGetTemperature(-1);
  fprintf( zout, "oyGetTemperature() = %g°K\n", temperature );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

#include "oyImage_s.h"
#include "oyConversion_s.h"

oyjlTESTRESULT_e testDeviceLinkProfile ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  double buf[24];
  uint32_t icc_profile_flags = 0;
  oyProfile_s * prof = NULL, *dl = NULL;
  oyImage_s * in = NULL, * out = NULL;
  oyOptions_s * options = NULL;
  oyConversion_s * cc = NULL;
  oyFilterGraph_s * graph = NULL;
  oyFilterNode_s * node = NULL;
  oyFilterNode_s * inode = NULL;
  oyOptions_s * opts =  NULL;
  oyBlob_s * blob = NULL;
  int error = 0;
  const char * fn = NULL, * prof_fn = NULL;
  int i,n=0, len;

  fprintf(stdout, "\n" );

  if(verbose) fprintf(stdout, "creating DL from sRGB to CIE*XYZ\n" );

  memset( buf, 0, sizeof(double)*24);

  /*oyConversion_RunPixels( cc, 0 );*/

  icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyFilterNode_s * in_node = oyFilterNode_NewWith( "//" OY_TYPE_STD "/root", 0, testobj );
  //oyFilterNode_SetData( in_node, (oyStruct_s*)input, 0, 0 );
  oyFilterNode_s * out_node = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", 0, testobj );
  /*oyFilterNode_SetData( out_node, (oyStruct_s*)output, 0, 0 );*/
  error = oyFilterNode_Connect( in_node, "//" OY_TYPE_STD "/data",
                                out_node, "//" OY_TYPE_STD "/data", 0 );
  if(error == 0)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyFilterNode_Connect()" );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyFilterNode_Connect()                            %d", error );
  }
  oyFilterNode_Release( &out_node );
  oyFilterNode_Release( &in_node );
  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "Node Connect" )

  in_node = oyFilterNode_NewWith( "//" OY_TYPE_STD "/root", 0, testobj );
  out_node = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", 0, testobj );
  error = oyFilterNode_Connect( in_node, "//" OY_TYPE_STD "/data",
                                out_node, "//" OY_TYPE_STD "/data", 0 );
  n = oyFilterNode_EdgeCount( out_node, 1, 0 ); // count plugs
  error = oyFilterNode_Disconnect( out_node, 0 ); // release plug connection
  if(n && error == 0)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyFilterNode_Disconnect() %d", n );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyFilterNode_Disconnect() %d", n );
  }
  oyFilterNode_Release( &out_node );
  oyFilterNode_Release( &in_node );
  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "Node Connect" )

  cc = oyConversion_New( NULL );
  oyConversion_Set( cc, in_node, NULL );
  oyConversion_Set( cc, 0, out_node );
  error = oyConversion_Release( &cc );
  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "Conversion" )


  prof = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  prof_fn = oyProfile_GetFileName( prof, -1 );
  in = oyImage_Create( 2, 2, buf, OY_TYPE_123_DBL, prof, testobj );
  out = oyImage_CreateForDisplay( 2, 2, buf, OY_TYPE_123_DBL, 0,
                                              0,0, 12,12,
                                              icc_profile_flags, testobj );
  cc = oyConversion_CreateBasicPixels( in, out, NULL, NULL );
  error = oyConversion_Release( &cc );
  if(error == 0)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyConversion_Release() simple" );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyConversion_Release() simple %d", error );
  }
  oyImage_Release( &in );
  oyImage_Release( &out );
  oyProfile_Release( &prof );
  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 0, NULL )

  prof = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  prof_fn = oyProfile_GetFileName( prof, -1 );
  in = oyImage_Create( 2, 2, buf, OY_TYPE_123_DBL, prof, testobj );
  out = oyImage_CreateForDisplay( 2, 2, buf, OY_TYPE_123_DBL, 0,
                                              0,0, 12,12,
                                              icc_profile_flags, testobj );
  cc = oyConversion_CreateBasicPixels( in, out, options, NULL );
  node = oyConversion_GetNode( cc, OY_OUTPUT);
  inode = oyFilterNode_GetPlugNode( node, 0 );
  opts =  oyFilterNode_GetOptions( inode, 0 );
  oyFilterNode_Release( &node );
  oyFilterNode_Release( &inode );
  oyImage_Release( &in );
  oyImage_Release( &out );
  oyProfile_Release( &prof );
  error = oyConversion_Release( &cc );
  oyOptions_Release( &opts );
  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 0, "Node Opts" )

  prof = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  prof_fn = oyProfile_GetFileName( prof, -1 );
  in = oyImage_Create( 2, 2, buf, OY_TYPE_123_DBL, prof, testobj );
  out = oyImage_CreateForDisplay( 2, 2, buf, OY_TYPE_123_DBL, 0,
                                              0,0, 12,12,
                                              icc_profile_flags, testobj );
  cc = oyConversion_CreateBasicPixels( in, out, options, NULL );
  node = oyConversion_GetNode( cc, OY_OUTPUT);
  inode = oyFilterNode_GetPlugNode( node, 0 );
  opts =  oyFilterNode_GetOptions( inode, 0 );
  oyFilterNode_s * rectangles = oyFilterNode_NewWith( "//" OY_TYPE_STD "/rectangles", 0, 0 );
  /* mark the new node as belonging to this node */
  oyOptions_s * rectangles_tags = oyFilterNode_GetTags( rectangles ),
              * node_options = oyFilterNode_GetOptions( node, 0 );
  oyOptions_SetFromString( &rectangles_tags, "some/key", "true", OY_CREATE_NEW );
  oyOptions_Release( &rectangles_tags );
  oyOptions_Release( &node_options );
  /* insert "rectangles" between "display" and its input_node */
  oyFilterNode_Disconnect( node, 0 );
  error = oyFilterNode_Connect( inode, "//" OY_TYPE_STD "/data",
                                rectangles, "//" OY_TYPE_STD "/data",0 );
  error = oyFilterNode_Connect( rectangles, "//" OY_TYPE_STD "/data",
                                node, "//" OY_TYPE_STD "/data",0 );
  if(verbose)
    oyObjectTreePrint( 0x01 | 0x02 | 0x04 | 0x08, "Node Insert" );
  oyFilterNode_Release( &node );
  oyFilterNode_Release( &inode );
  oyImage_Release( &in );
  oyImage_Release( &out );
  oyProfile_Release( &prof );
  error = oyConversion_Release( &cc );
  oyOptions_Release( &opts );
  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 0, "Node Insert" )

  prof = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  prof_fn = oyProfile_GetFileName( prof, -1 );
  in = oyImage_Create( 2, 2, buf, OY_TYPE_123_DBL, prof, testobj );
  out = oyImage_CreateForDisplay( 2, 2, buf, OY_TYPE_123_DBL, 0,
                                              0,0, 12,12,
                                              icc_profile_flags, testobj );
  cc = oyConversion_CreateBasicPixels( in, out, options, NULL );
  node = oyConversion_GetNode( cc, OY_OUTPUT);
  inode = oyFilterNode_GetPlugNode( node, 0 );
  opts =  oyFilterNode_GetOptions( inode, 0 );
  oyFilterNode_Release( &node );
  const char * hash = oyFilterNode_GetText( inode, oyNAME_NAME );
  oyFilterNode_Release( &inode );
  oyImage_Release( &in );
  oyImage_Release( &out );
  oyProfile_Release( &prof );
  error = oyConversion_Release( &cc );
  oyOptions_Release( &opts );
  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 0, "Node Hash" )

  prof = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  prof_fn = oyProfile_GetFileName( prof, -1 );
  in = oyImage_Create( 2, 2, buf, OY_TYPE_123_DBL, prof, testobj );
  out = oyImage_CreateForDisplay( 2, 2, buf, OY_TYPE_123_DBL, 0,
                                              0,0, 12,12,
                                              icc_profile_flags, testobj );
  cc = oyConversion_CreateBasicPixels( in, out, options, NULL );
  node = oyConversion_GetNode( cc, OY_OUTPUT);
  inode = oyFilterNode_GetPlugNode( node, 0 );
  opts =  oyFilterNode_GetOptions( inode, 0 );
  oyFilterNode_Release( &node );
  blob = oyFilterNode_ToBlob( inode, NULL );
  oyBlob_Release( &blob );
  oyFilterNode_Release( &inode );
  error = oyConversion_Release( &cc );
  oyOptions_Release( &opts );
  oyImage_Release( &in );
  oyImage_Release( &out );
  oyProfile_Release( &prof );
  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 0, "Node Context" )

  prof = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  prof_fn = oyProfile_GetFileName( prof, -1 );
  in = oyImage_Create( 2, 2, buf, OY_TYPE_123_DBL, prof, testobj );
  out = oyImage_CreateForDisplay( 2, 2, buf, OY_TYPE_123_DBL, 0,
                                              0,0, 12,12,
                                              icc_profile_flags, testobj );
  oyOptions_SetFromString( &options, OY_CMM_STD"/context", "lcm2", OY_CREATE_NEW );

  cc = oyConversion_CreateBasicPixels( in, out, options, testobj );
  if(cc)
    graph = oyConversion_GetGraph( cc );
  error = oyConversion_Release( &cc );
  if(graph)
    n = oyFilterGraph_CountEdges( graph );
  for(i = 0; i < n; ++i)
  {
    node = oyFilterGraph_GetNode( graph, i, NULL, NULL );
    blob = oyFilterNode_ToBlob( node, NULL );
    if(blob && oyBlob_GetSize( blob ))
    {
      char name[64];
      sprintf( name, "oy_dl_test_%d_", i );
      len = strlen(name);
      memcpy( &name[len], oyBlob_GetType( blob ), 4 );
      name[len+4] = 0;
      len = strlen(name);
      sprintf( &name[len], ".icc" );
      error = oyWriteMemToFile_( name, oyBlob_GetPointer( blob ),
                                 oyBlob_GetSize( blob) );
      if(!error)
        fprintf(zout,"wrote: %s\n", name );
      else
        fprintf(zout,"writing failed: %s\n", name );
      dl = oyProfile_FromMem( oyBlob_GetSize( blob ),
                              oyBlob_GetPointer( blob ), 0,0 );
    }

    oyBlob_Release( &blob );
    oyFilterNode_Release( &node );
  }
  error = oyConversion_Release( &cc );
  oyFilterGraph_Release( &graph );
  oyImage_Release( &in );
  oyImage_Release( &out );
  oyOptions_Release( &options );

  fn = oyProfile_GetFileName( dl, 0 );
  if(fn && prof_fn && strcmp(fn,prof_fn) == 0)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyProfile_GetFileName(dl, 0): %s", fn );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyProfile_GetFileName(dl, 0): %s  %s", oyNoEmptyString_m_(fn), oyNoEmptyString_m_(prof_fn) );
  }

  fn = oyProfile_GetFileName( dl, 1 );
  if(fn)
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyProfile_GetFileName(dl, 1): %s", fn );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyProfile_GetFileName(dl, 1): %s", oyNoEmptyString_m_(fn) );
  }

  fn = oyProfile_GetText( dl, oyNAME_NAME );
  if(verbose) fprintf(zout,"oyProfile_GetText( dl, oyNAME_NAME ): %s\n", fn );
  oyProfile_Release( &prof );
  oyProfile_Release( &dl );
  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 0, "Node Graph" )


  prof = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  prof_fn = oyProfile_GetFileName( prof, -1 );
  in = oyImage_Create( 2, 2, buf, OY_TYPE_123_DBL, prof, testobj );
  out = oyImage_CreateForDisplay( 2, 2, buf, OY_TYPE_123_DBL, 0,
                                              0,0, 12,12,
                                              icc_profile_flags, testobj );
  oyOptions_SetFromString( &options, OY_CMM_STD"/context", "lcm2", OY_CREATE_NEW );
  oyOptions_SetFromString( &options, OY_CMM_STD"/precalculation", "4", OY_CREATE_NEW );
  cc = oyConversion_CreateBasicPixels( in, out, options, testobj );
  if(cc)
    graph = oyConversion_GetGraph( cc );
  oyConversion_Release( &cc );
  if(graph)
    n = oyFilterGraph_CountEdges( graph );
  for(i = 0; i < n; ++i)
  {
    node = oyFilterGraph_GetNode( graph, i, NULL, NULL );
    blob = oyFilterNode_ToBlob( node, NULL );
    if(blob && oyBlob_GetSize( blob ))
    {
      char name[64];
      sprintf( name, "oy_dl_null_test_%d_", i );
      len = strlen(name);
      memcpy( &name[len], oyBlob_GetType( blob ), 4 );
      name[len+4] = 0;
      len = strlen(name);
      sprintf( &name[len], ".icc" );
      error = oyWriteMemToFile_( name, oyBlob_GetPointer( blob ),
                                 oyBlob_GetSize( blob) );
      if(!error)
        fprintf(zout,"wrote: %s\n", name );
      else
        fprintf(zout,"writing failed: %s\n", name );
      dl = oyProfile_FromMem( oyBlob_GetSize( blob ),
                              oyBlob_GetPointer( blob ), 0,0 );
    }

    oyBlob_Release( &blob );
    oyFilterNode_Release( &node );
  }

  const char * desc = oyProfile_GetText( dl, oyNAME_DESCRIPTION);
  if(desc && strstr(desc,"pass through"))
  {
    PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "pass through created \"%s\"", desc );
  } else
  {
    PRINT_SUB( oyjlTESTRESULT_FAIL,
    "pass through created \"%s\"", oyNoEmptyString_m_(desc) );
  }


  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyConversion_Release() error: %d", error )
  error = oyImage_Release( &in );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyImage_Release() error: %d", error )
  error = oyImage_Release( &out );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyImage_Release() error: %d", error )
  error = oyProfile_Release( &prof );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyProfile_Release() error: %d", error )
  error = oyProfile_Release( &dl );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyProfile_Release() error: %d", error )
  error = oyOptions_Release( &options );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_Release() error: %d", error )
  error = oyFilterGraph_Release( &graph );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyFilterGraph_Release() error: %d", error )

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}


/* make plain external project code happy */
#define GLushort uint16_t
#define GLuint uint32_t
#define GLfloat float
#define cicc_free oyFree_m_
#define cdCreateTexture( ignore_opengl )

#define CompLogLevelDebug oyMSG_WARN
#define CompLogLevelWarn oyMSG_WARN
#define CompLogLevelInfo oyMSG_DBG
#define DBG_STRING OY_DBG_FORMAT_
#define DBG_ARGS OY_DBG_ARGS_
#define oyCompLogMessage(ignore, prog, level, format, ... ) \
        oyMessageFunc_p( level,0, format, __VA_ARGS__)
static oyStructList_s * oy_test_cache_ = NULL;
oyStructList_s *   pluginGetPrivatesCache ( )
{
  if(!oy_test_cache_)
    oy_test_cache_ = oyStructList_Create( oyOBJECT_NONE, "oy_test_cache_ p", 0 );

  return oy_test_cache_;
}
void         oyTestCacheListClear_     ( )
{
  oyStructList_Release( &oy_test_cache_ );
}

/* taken from compicc.c f268e681e3b73004376e8d3ea9db4ae3e669fad8 20150227 */
#define GRIDPOINTS 64
typedef struct {
  oyProfile_s * src_profile;         /* the data profile or device link */
  oyProfile_s * dst_profile;         /* the monitor profile or none */
  char * output_name;                /* the intented output device */
  GLushort clut[GRIDPOINTS][GRIDPOINTS][GRIDPOINTS][3]; /* lookup table */
  GLuint glTexture;                  /* texture reference */
  GLfloat scale, offset;             /* texture parameters */
  int ref;                           /* reference counter */
} PrivColorContext;
typedef void CompScreen;
typedef struct pcc_s {
  char * id;
  oyConversion_s * cc;
  oyArray2d_s * clut;
  char * hash_text;
  oyHash_s * hash;
} pcc_t;
static void * setupColourTable_cb( void * data )
{
  pcc_t * d OY_UNUSED = (pcc_t*)data;

  //setupColourTable( d->ccontext, d->advanced, d->screen );
  //updateOutputConfiguration( d->screen, FALSE, -1 );

  return NULL;
}
static void iccProgressCallback (    double              progress_zero_till_one,
                                     char              * status_text OY_UNUSED,
                                     int                 thread_id_,
                                     int                 job_id,
                                     oyStruct_s        * cb_progress_context )
{
  oyPointer_s * context = (oyPointer_s *) cb_progress_context;
  pcc_t * pcontext = (pcc_t*) oyPointer_GetPointer( context );
  printf( "%s() job_id: %d thread: %d %g\n", __func__, job_id, thread_id_,
          progress_zero_till_one );
  if(progress_zero_till_one >= 1.0)
  {
    setupColourTable_cb( pcontext );
  }
}
static int privateClutContextRelease( oyPointer * data )
{
  pcc_t ** context = (pcc_t **) data;
  pcc_t * pcc   = *context;
  oyConversion_Release( &pcc->cc );
  oyArray2d_Release( &pcc->clut );
  oyFree_m_( pcc->hash_text );
  oyHash_Release( &pcc->hash );
  if(verbose) fprintf(zout, DBG_STRING "%s\n", DBG_ARGS, pcc->id );
  free(pcc->id);
  oyFree_m_( pcc );
  return 0;
}

#include <oyranos_threads.h>
#define DBG_S_ if(oy_debug >= 1)DBG_S
static int icc_profile_flags = 0;
static oyConversion_s * setupColourConversion (
                                       oyProfile_s       * dst_profile,
                                       oyProfile_s       * src_profile,
                                       int                 advanced,
                                       const char        * output_name,
                                       oyArray2d_s       * clut )
{
  oyConversion_s * cc = NULL;
  int error = 0;
  oyProfile_s * web = 0;
  int ** ptr = (int**) oyArray2d_GetData( clut );

    if(!dst_profile)
      dst_profile = web = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, 0 );

    {
      int flags = 0;

      oyOptions_s * options = 0;
      oyImage_s * image_in = NULL;
      oyImage_s * image_out= NULL;

      oyPixel_t pixel_layout = OY_TYPE_123_16;
      /*oyCompLogMessage(NULL, "compicc", CompLogLevelDebug,
             DBG_STRING "%s -> %s",
             DBG_ARGS, oyProfile_GetText( src_profile, oyNAME_DESCRIPTION ),
                       oyProfile_GetText( dst_profile, oyNAME_DESCRIPTION ) );*/

      /* skip web to web conversion */
      if(oyProfile_Equal( src_profile, web ))
      {
        oyCompLogMessage(NULL, "compicc", CompLogLevelDebug,
             DBG_STRING "src_profile == web",
             DBG_ARGS );
        goto clean_setupColourConversion;
      }

      if(!src_profile)
        src_profile = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, 0 );

      if(!src_profile)
        oyCompLogMessage(NULL, "compicc", CompLogLevelWarn,
             DBG_STRING "Output %s: no oyASSUMED_WEB src_profile",
             DBG_ARGS, output_name );

      /* optionally set advanced options from Oyranos */
      if(advanced)
        flags = oyOPTIONATTRIBUTE_ADVANCED;

      /*oyCompLogMessage( NULL, "compicc", CompLogLevelDebug,
                      DBG_STRING "oyConversion_Correct(///icc_color,%d,0) %s %s",
                      DBG_ARGS, flags, ccontext->output_name,
                      advanced?"advanced":"");*/
      image_in = oyImage_Create( GRIDPOINTS,GRIDPOINTS*GRIDPOINTS,
                                 ptr[0], pixel_layout, src_profile, 0 );
      image_out= oyImage_Create( GRIDPOINTS,GRIDPOINTS*GRIDPOINTS,
                                 ptr[0], pixel_layout, dst_profile, 0 );

      cc = oyConversion_CreateBasicPixels( image_in, image_out, options, 0 );
      if (cc == NULL)
      {
        oyCompLogMessage( NULL, "compicc", CompLogLevelWarn,
                      DBG_STRING "no conversion created for %s",
                      DBG_ARGS, output_name);
        goto clean_setupColourConversion;
      }
      oyOptions_Release( &options );

      error = oyOptions_SetFromString( &options,
                                     "//" OY_TYPE_STD "/config/display_mode", "1",
                                     OY_CREATE_NEW );
      error = oyConversion_Correct(cc, "//" OY_TYPE_STD "/icc_color", flags, options);
      if(error)
      {
        oyCompLogMessage( NULL, "compicc", CompLogLevelWarn,
                      DBG_STRING "oyConversion_Correct(///icc_color,%d,0) failed %s",
                      DBG_ARGS, flags, output_name);
        goto clean_setupColourConversion;
      }

    clean_setupColourConversion:
      oyOptions_Release( &options );
      oyImage_Release( &image_in );
      oyImage_Release( &image_out );
      oyProfile_Release( &web );
    }

  return cc;
}
static oyFilterNode_s *  getColourNode(oyConversion_s    * cc )
{
      oyFilterGraph_s * cc_graph = oyConversion_GetGraph( cc );
      oyFilterNode_s * icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
      oyFilterGraph_Release( &cc_graph );
      return icc;
}
static oyHash_s * getColourHash      ( oyFilterNode_s    * icc,
                                       char             ** hash_text )
{
      uint32_t exact_hash_size = 0;
      const char * t = 0;
      {
        t = oyFilterNode_GetText( icc, oyNAME_NAME );
        if(t)
          *hash_text = strdup(t);
      }
      oyHash_s * entry;
      oyStructList_s * cache = pluginGetPrivatesCache();
      entry = oyStructList_GetHash( cache, exact_hash_size, *hash_text );

      return entry;
}
static void          fillColourClut  ( PrivColorContext  * ccontext )
{
        uint16_t in[3];
        for (int r = 0; r < GRIDPOINTS; ++r)
        {
          in[0] = floor((double) r / (GRIDPOINTS - 1) * 65535.0 + 0.5);
          for (int g = 0; g < GRIDPOINTS; ++g) {
            in[1] = floor((double) g / (GRIDPOINTS - 1) * 65535.0 + 0.5);
            for (int b = 0; b < GRIDPOINTS; ++b)
            {
              in[2] = floor((double) b / (GRIDPOINTS - 1) * 65535.0 + 0.5);
              for(int j = 0; j < 3; ++j)
                /* BGR */
                ccontext->clut[b][g][r][j] = in[j];
            }
          }
        }
}
#ifdef _OPENMP
#define USE_OPENMP 1
#include <omp.h>
#endif
static int           fillColourClut2 ( oyArray2d_s       * array )
{
  int ** ptr = (int**) oyArray2d_GetData( array );
  uint16_t (*clut)[GRIDPOINTS][GRIDPOINTS][GRIDPOINTS][3] = (uint16_t(*)[GRIDPOINTS][GRIDPOINTS][GRIDPOINTS][3]) ptr[0];

#define myFLOOR(d) ((int)(d))
#if defined(USE_OPENMP)
#pragma omp parallel for
#endif
        for (int r = 0; r < GRIDPOINTS; ++r)
        {
          uint16_t in[3];
          in[0] = myFLOOR((double) r / (GRIDPOINTS - 1) * 65535.0 + 0.5);
          for (int g = 0; g < GRIDPOINTS; ++g) {
            in[1] = myFLOOR((double) g / (GRIDPOINTS - 1) * 65535.0 + 0.5);
            for (int b = 0; b < GRIDPOINTS; ++b)
            {
              in[2] = myFLOOR((double) b / (GRIDPOINTS - 1) * 65535.0 + 0.5);
              for(int j = 0; j < 3; ++j)
              {
                /* BGR */
                (*clut)[b][g][r][j] = in[j];
              }
            }
          }
        }
        return GRIDPOINTS*GRIDPOINTS*GRIDPOINTS;
}
void pingNativeDisplay()
{
  /* ping X11 observers about option change
   * ... by setting a known property again to its old value
   */
  oyOptions_s * opts = oyOptions_New(NULL), * results = 0;
  oyOptions_Handle( "//" OY_TYPE_STD "/send_native_update_event",
                      opts,"send_native_update_event",
                      &results );
  oyOptions_Release( &opts );
}
static int computeClut( oyJob_s * job )
{
  oyPointer_s * context = (oyPointer_s *) job->context;
  pcc_t * pcontext = (pcc_t*) oyPointer_GetPointer( context );
  oyArray2d_s * clut = (oyArray2d_s*) oyHash_GetPointer( pcontext->hash, oyOBJECT_ARRAY2D_S);
  /* make sure no one else finished already work on this */
  if(!clut)
  {
    fillColourClut2( pcontext->clut );
    oyConversion_RunPixels( pcontext->cc, 0 );
    oyHash_SetPointer( pcontext->hash, (oyStruct_s*) pcontext->clut );
    pingNativeDisplay();
  }
  fprintf(zout, DBG_STRING "hash_text: %s %s %s\n", DBG_ARGS, verbose?pcontext->hash_text:"use -v (verbose) to see;", oyArray2d_Show(pcontext->clut, 3), clut==NULL?"newly computed":"already there" );
  return 0;
}
int clut_finished = 0;
static int finishClut( oyJob_s * job )
{ 
  if(getenv("OY_DEBUG_THREADS") || verbose) fprintf(zout, DBG_STRING "\n", DBG_ARGS );
  clut_finished = 1;
  return 0;
}
static oyJob_s *   setupColourJob    ( oyConversion_s   ** cc,
                                       char             ** hash_text,
                                       oyHash_s         ** hash,
                                       oyArray2d_s      ** clut )
{
      oyJob_s * job = oyJob_New(0);
      job->cb_progress = iccProgressCallback;
      oyPointer_s * oy_ptr = oyPointer_New(0);
      pcc_t * pcc   = (pcc_t*)calloc( sizeof(pcc_t), 1 );
      oyjlStringAdd( &pcc->id, 0,0, "setupColourJob[%d]", job->id_ );
      pcc->cc = *cc;
      *cc = NULL;
      pcc->hash_text = *hash_text;
      *hash_text = NULL;
      pcc->hash = *hash;
      *hash = NULL;
      pcc->clut = *clut;
      *clut = NULL;
      oyPointer_Set( oy_ptr,
                     __FILE__,
                     "struct pcc_s*",
                     pcc, "privateClutContextRelease", privateClutContextRelease );
      job->cb_progress_context = (oyStruct_s*) oyPointer_Copy( oy_ptr, 0 );
      job->context = (oyStruct_s*) oyPointer_Copy( oy_ptr, 0 );
      job->work = computeClut;
      job->finish = finishClut; /* optionally call from main thread, e.g. for texture creation in updateOutputConfiguration() */
      oyPointer_Release( &oy_ptr );
      return job;
}
static void          runColourClut   ( PrivColorContext  * ccontext,
                                       oyConversion_s    * cc )
{
  oyFilterNode_s * icc = getColourNode( cc );
  char * hash_text = NULL;
  oyHash_s * entry = getColourHash( icc, &hash_text );
  oyArray2d_s * clut;
  int error = 0;
  int ** ptr;
  int id = entry->oy_->id_;
  fillColourClut( ccontext );

        clut = oyArray2d_Create( NULL, GRIDPOINTS*3, GRIDPOINTS*GRIDPOINTS,
                                 oyUINT16, NULL );

        error = oyConversion_RunPixels( cc, 0 );

        if(error)
        {
          oyCompLogMessage( NULL, "compicc", CompLogLevelWarn,
                      DBG_STRING "oyConversion_RunPixels() error: %d %s",
                      DBG_ARGS, error, ccontext->output_name);
          goto clean_runColourClut;
        }

        ptr = (int**)oyArray2d_GetData(clut);
        memcpy( ptr[0], ccontext->clut,
                sizeof(GLushort) * GRIDPOINTS*GRIDPOINTS*GRIDPOINTS * 3 );

        oyHash_SetPointer( entry, (oyStruct_s*) clut );


clean_runColourClut:
  if(hash_text)
  { cicc_free(hash_text); hash_text = 0; }
  oyFilterNode_Release( &icc );
  oyHash_Release( &entry );
  oyArray2d_Release( &clut );
}
static int     setupColourTable      ( PrivColorContext  * ccontext,
                                       int                 advanced )
{
  int dl_count = 0;

  oyConversion_s * cc;
  int error = 0;
  oyProfile_s * dst_profile = ccontext->dst_profile, * web = 0;
  uint32_t icc_profile_flags;

  DBG_S_( oyPrintTime() );
  icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  DBG_S_( oyPrintTime() );

  if(!ccontext->dst_profile)
      dst_profile = web = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );


    {
      int flags = 0;
      int ** ptr;

      oyProfile_s * src_profile = oyProfile_Copy( ccontext->src_profile, 0 );
      oyOptions_s * options = 0;

      oyPixel_t pixel_layout = OY_TYPE_123_16;
      oyCompLogMessage(NULL, "compicc", CompLogLevelDebug,
             DBG_STRING "%s -> %s",
             DBG_ARGS, oyProfile_GetText( src_profile, oyNAME_DESCRIPTION ),
                       oyProfile_GetText( dst_profile, oyNAME_DESCRIPTION ) );

      /* skip web to web conversion */
      if(oyProfile_Equal( src_profile, web ))
      {
        oyCompLogMessage(NULL, "compicc", CompLogLevelDebug,
             DBG_STRING "src_profile == web",
             DBG_ARGS );
        goto clean_setupColourTable;
      }

      if(!src_profile)
        src_profile = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );

      if(!src_profile)
        oyCompLogMessage(NULL, "compicc", CompLogLevelWarn,
             DBG_STRING "Output %s: no oyASSUMED_WEB src_profile",
             DBG_ARGS, ccontext->output_name );

      /* optionally set advanced options from Oyranos */
      if(advanced)
        flags = oyOPTIONATTRIBUTE_ADVANCED;

      DBG_S_( oyPrintTime() );

      oyCompLogMessage( NULL, "compicc", CompLogLevelDebug,
                      DBG_STRING "oyConversion_Correct(///icc_color,%d,0) %s %s",
                      DBG_ARGS, flags, ccontext->output_name,
                      advanced?"advanced":"");
      oyImage_s * image_in = oyImage_Create( GRIDPOINTS,GRIDPOINTS*GRIDPOINTS,
                                             ccontext->clut,
                                             pixel_layout, src_profile, testobj );
      oyImage_s * image_out= oyImage_Create( GRIDPOINTS,GRIDPOINTS*GRIDPOINTS,
                                             ccontext->clut,
                                             pixel_layout, dst_profile, testobj );

      oyProfile_Release( &src_profile );

      oyOptions_SetFromString( &options, "org/oyranos/cmm/any/cached", "1", OY_CREATE_NEW );
      cc = oyConversion_CreateBasicPixels( image_in, image_out, options, testobj );
      if (cc == NULL)
      {
        oyCompLogMessage( NULL, "compicc", CompLogLevelWarn,
                      DBG_STRING "no conversion created for %s",
                      DBG_ARGS, ccontext->output_name);
        goto clean_setupColourTable;
      }
      oyOptions_Release( &options );

      oyOptions_SetFromString( &options,
                                     "//" OY_TYPE_STD "/config/display_mode", "1",
                                     OY_CREATE_NEW );
      DBG_S_( oyPrintTime() );

      error = oyConversion_Correct(cc, "//" OY_TYPE_STD "/icc_color", flags, options);
      if(error > 0)
      {
        oyCompLogMessage( NULL, "compicc", CompLogLevelWarn,
                      DBG_STRING "oyConversion_Correct(///icc_color,%d,0) failed %s, %d",
                      DBG_ARGS, flags, ccontext->output_name, error);
        goto clean_setupColourTable;
      }
      oyOptions_Release( &options );
      DBG_S_( oyPrintTime() );


      oyFilterGraph_s * cc_graph = oyConversion_GetGraph( cc );
      oyFilterNode_s * icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
      uint32_t exact_hash_size = 0;
      char * hash_text = 0;
      const char * t = 0;
      {
        t = oyFilterNode_GetText( icc, oyNAME_NAME );
        if(t)
          hash_text = strdup(t);
        if(oy_debug)
        {
          char * fn = NULL;
          static int pos = 0;
          oyjlStringAdd( &fn, 0,0, "clut-hash-%d.json", pos++ );
          oyWriteMemToFile_(fn, t, strlen(t));
        }
      }
      oyHash_s * entry;
      oyArray2d_s * clut = NULL;
      oyStructList_s * cache = pluginGetPrivatesCache();
      entry = oyStructList_GetHash( cache, exact_hash_size, hash_text );
      clut = (oyArray2d_s*) oyHash_GetPointer( entry, oyOBJECT_ARRAY2D_S);

      DBG_S_( oyPrintTime() );

      if(oy_debug)
      oyCompLogMessage( NULL, "compicc", CompLogLevelDebug,
                      DBG_STRING "clut from cache %s %s",
                      DBG_ARGS, clut?"obtained":"not obtained", oyNoEmptyString_m_(hash_text) );
      if(clut) // found in cache
      {
        ptr = (int**)oyArray2d_GetData(clut);
        memcpy( ccontext->clut, ptr[0], 
                sizeof(GLushort) * GRIDPOINTS*GRIDPOINTS*GRIDPOINTS * 3 );
      } else
      {
        uint16_t in[3];
        oyBlob_s * blob = oyFilterNode_ToBlob( icc, testobj );

        DBG_S_( oyPrintTime() );

        if(!blob)
        {
          oyConversion_Release( &cc );
          oyFilterNode_Release( &icc );

          oyOptions_SetFromString( &options, OY_DEFAULT_CMM_CONTEXT, "lcm2", OY_CREATE_NEW );
          DBG_S_( oyPrintTime() );
          cc = oyConversion_CreateBasicPixels( image_in, image_out, options, testobj );
          if (cc == NULL)
          {
            oyCompLogMessage( NULL, "compicc", CompLogLevelWarn,
                      DBG_STRING "no conversion created for %s",
                      DBG_ARGS, ccontext->output_name);
            goto clean_setupColourTable;
          }
          oyOptions_Release( &options );
          oyOptions_SetFromString( &options,
                                     "//" OY_TYPE_STD "/config/display_mode", "1",
                                     OY_CREATE_NEW );
          DBG_S_( oyPrintTime() );
          error = oyConversion_Correct(cc, "//" OY_TYPE_STD "/icc_color", flags, options);
          DBG_S_( oyPrintTime() );
          icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
          DBG_S_( oyPrintTime() );
          blob = oyFilterNode_ToBlob( icc, NULL );
        }

        DBG_S_( oyPrintTime() );

        {
          oyOptions_s * node_opts = oyFilterNode_GetOptions( icc, 0 );
          oyProfile_s * dl;
          DBG_S_( oyPrintTime() );
          dl = oyProfile_FromMem( oyBlob_GetSize( blob ),
                                  oyBlob_GetPointer( blob ), 0,testobj );
          oyBlob_Release( &blob );
          const char * fn;
          int j = 0;
          while((fn = oyProfile_GetFileName( dl, j )) != NULL)
          {
            fprintf( zout, " -> \"%s\"[%d]", fn, j );
            ++j;
          }
          dl_count = j;
          fprintf( zout, " %d\n", dl_count );
          if(oy_debug) fprintf( zout, "%s\n", oyOptions_GetText( node_opts, oyNAME_NAME ) );
          oyProfile_Release( &dl );
          oyOptions_Release( &node_opts );
        }

        DBG_S_( oyPrintTime() );
        for (int r = 0; r < GRIDPOINTS; ++r)
        {
          in[0] = floor((double) r / (GRIDPOINTS - 1) * 65535.0 + 0.5);
          for (int g = 0; g < GRIDPOINTS; ++g) {
            in[1] = floor((double) g / (GRIDPOINTS - 1) * 65535.0 + 0.5);
            for (int b = 0; b < GRIDPOINTS; ++b)
            {
              in[2] = floor((double) b / (GRIDPOINTS - 1) * 65535.0 + 0.5);
              for(int j = 0; j < 3; ++j)
                /* BGR */
                ccontext->clut[b][g][r][j] = in[j];
            }
          }
        }

        DBG_S_( oyPrintTime() );
        clut = oyArray2d_Create( NULL, GRIDPOINTS*3, GRIDPOINTS*GRIDPOINTS,
                                 oyUINT16, testobj );

        DBG_S_( oyPrintTime() );
        error = oyConversion_RunPixels( cc, 0 );

        if(error)
        {
          oyCompLogMessage( NULL, "compicc", CompLogLevelWarn,
                      DBG_STRING "oyConversion_RunPixels() error: %d %s",
                      DBG_ARGS, error, ccontext->output_name);
          goto clean_setupColourTable;
        }

        ptr = (int**)oyArray2d_GetData(clut);
        memcpy( ptr[0], ccontext->clut,
                sizeof(GLushort) * GRIDPOINTS*GRIDPOINTS*GRIDPOINTS * 3 );

        oyHash_SetPointer( entry, (oyStruct_s*) clut );
      }

      DBG_S_( oyPrintTime() );

      if(hash_text)
      {
        cicc_free(hash_text); hash_text = 0;
      }


      oyFilterNode_Release( &icc );
      oyFilterGraph_Release( &cc_graph );
      oyOptions_Release( &options );
      oyImage_Release( &image_in );
      oyImage_Release( &image_out );
      oyConversion_Release( &cc );
      oyArray2d_Release( &clut );
      oyHash_Release( &entry );

      cdCreateTexture( ccontext );

      DBG_S_( oyPrintTime() );

    }

    if(!ccontext->dst_profile)
    {
      oyCompLogMessage( NULL, "compicc", CompLogLevelInfo,
                      DBG_STRING "Output \"%s\": no profile",
                      DBG_ARGS, ccontext->output_name);
    }

    clean_setupColourTable:
    if(web)
      oyProfile_Release( &web );

    DBG_S_( oyPrintTime() );

  return dl_count;
}

// TODO: needs optmimisation, as it's much slower than command line
oyjlTESTRESULT_e testClut ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  oyTestCacheListClear_();
  //oyThreadId(); // possibly initialise threading early for proper cache locks

  oyConversion_s * cc = NULL;
  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  PrivColorContext pc = {
    oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, NULL ), /* src_profile: the data profile or device link */
    oyProfile_FromFile( "compatibleWithAdobeRGB1998.icc", icc_profile_flags, NULL ), /* dst_profile: the monitor profile or none */
    oyStringCopy( "*TEST*", oyAllocateFunc_ ), /* the intented output device */
    {{{0}}},
    0,
    0,0,
    0
  };
  double clck = oyClock();
  int dl_count = setupColourTable( &pc, 0 );
  int r=12,g=12,b=2;
  clck = oyClock() - clck;
  uint16_t c[3] = {pc.clut[r][g][b][0],pc.clut[r][g][b][1],pc.clut[r][g][b][2]};
  if(verbose) fprintf(zout, "compatibleWithAdobeRGB1998.icc %d,%d,%d\n", pc.clut[r][g][b][0],pc.clut[r][g][b][1],pc.clut[r][g][b][2]);

  int count = oyStructList_Count( oy_test_cache_ );
  if( count )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, 1,clck/(double)CLOCKS_PER_SEC,"Tex.",
    "clut new                %d", count );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "clut cache empty" );
  }

  clck = oyClock();
  dl_count = setupColourTable( &pc, 0 );
  clck = oyClock() - clck;

  count = oyStructList_Count( oy_test_cache_ );
  if( count == 1 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, 1,clck/(double)CLOCKS_PER_SEC,"Tex.",
    "clut cached             %d", count );
  } else
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_XFAIL, 1,clck/(double)CLOCKS_PER_SEC," Tex",
    "clut cached             %d", count );
  }

  int i;
  oyArray2d_s * clut = oyArray2d_Create( NULL, GRIDPOINTS*3, GRIDPOINTS*GRIDPOINTS,
                                 oyUINT16, NULL );
  clck = oyClock();
  for(i = 0; i < 10; ++i)
  {
    cc = setupColourConversion( pc.dst_profile, pc.src_profile, 0, pc.output_name, clut );
    oyConversion_Release( &cc );
  }
  clck = oyClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling(-1, 10,clck/(double)CLOCKS_PER_SEC,"node",
      "setupColourConversion() 10") );

  oyProfile_Release( &pc.dst_profile );
  pc.dst_profile = oyProfile_FromName( "Lab.icc", icc_profile_flags, NULL );
  cc = setupColourConversion( pc.dst_profile, pc.src_profile, 0, pc.output_name, clut );
  clck = oyClock();
  for(i = 0; i < 10; ++i)
  {
    oyFilterNode_s * icc = getColourNode( cc );
    oyOptions_s * options = oyFilterNode_GetOptions( icc, 0 );
    oyOptions_Release( &options );
    oyFilterNode_Release( &icc );
  }
  clck = oyClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling(-1, 10,clck/(double)CLOCKS_PER_SEC,"node",
      "getColourNode()+options 10") );

  oyFilterNode_s * icc = NULL;
  oyHash_s * hash = NULL;
  char * hash_text = NULL;

  icc = getColourNode( cc );
  clck = oyClock();
  hash = getColourHash( icc, &hash_text );
  oyJob_s * job = setupColourJob( &cc, &hash_text, &hash, &clut );
  oyJob_Add( &job, 0, 0 );
  clck = oyClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling(-1, 1,clck/(double)CLOCKS_PER_SEC,"job",
      "setupColourJob()        1") );

  clck = oyClock();
  for(i = 0; i < 10; ++i)
  {
    hash = getColourHash( icc, &hash_text );
    free(hash_text); hash_text = NULL;
    oyHash_Release( &hash );
  }
  clck = oyClock() - clck;
  oyFilterNode_Release( &icc );
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling(-1, i,clck/(double)CLOCKS_PER_SEC,"hash",
      "getColourHash()         %d",i) );

  for( i = 0; i < 10; ++i )
  {
    oyJobResult();
    if(clut_finished)
      break;
    else
      sleep(1); // wait for the job to finish
  }
  if(clut_finished == 0)
    fprintf( zout, "Job did not finish: expecting problems\n" );

  clck = oyClock();
  for(i = 0; i < 10; ++i)
  {
    fillColourClut( &pc );
  }
  clck = oyClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling(-1, i,clck/(double)CLOCKS_PER_SEC,"fill",
      "fillColourClut()        %d",i) );

  oyArray2d_s * clut2 = oyArray2d_Create( NULL, 3, GRIDPOINTS*GRIDPOINTS*GRIDPOINTS,
                                 oyUINT16, NULL );
  oyArray2d_Release( &clut );
  clut = clut2; clut2 = NULL;
  clck = oyClock();
  int n;
  for(i = 0; i < 10; ++i)
  {
    n = fillColourClut2( clut );
  }
  clck = oyClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling(-1, i,clck/(double)CLOCKS_PER_SEC,"fill",
      "fillColourClut2 %d  %d", n,i) );

  cc = setupColourConversion( pc.dst_profile, pc.src_profile, 0, pc.output_name, clut );
  clck = oyClock();
  for(i = 0; i < 10; ++i)
  {
    runColourClut( &pc, cc );
  }
  clck = oyClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling(-1, i,clck/(double)CLOCKS_PER_SEC,"run",
      "runColourClut()         %d",i) );

  clck = oyClock();
  for(i = 0; i < 10; ++i)
  {
    oyConversion_RunPixels( cc, 0 );
  }
  clck = oyClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling(-1, i,clck/(double)CLOCKS_PER_SEC,"run",
      "oyConversion_RunPixels  %d",i) );

  oyConversion_Release( &cc );
  oyProfile_Release( &pc.dst_profile );

  double clck2 = oyClock();
  clck = oyClock();
  pc.dst_profile = oyProfile_FromName( "ProPhoto-RGB.icc", icc_profile_flags, NULL );
  n = fillColourClut2( clut ); i = 1;
  clck = oyClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling(-1, i,clck/(double)CLOCKS_PER_SEC,"fill",
      "PP+fillColourClut2      %d", i) );

  clck = oyClock();
  cc = setupColourConversion( pc.dst_profile, pc.src_profile, 0, pc.output_name, clut );
  clck = oyClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling(-1, 1,clck/(double)CLOCKS_PER_SEC,"node",
      "setupColourConversionPP") );
  clck = oyClock();
  runColourClut( &pc, cc );
  clck = oyClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling(-1, i,clck/(double)CLOCKS_PER_SEC,"run",
      "runColourClut(PP)       %d",i) );

  clck = oyClock();
  icc = getColourNode( cc );
  hash = getColourHash( icc, &hash_text );
  oyHash_SetPointer( hash, (oyStruct_s*) clut );
  clck = oyClock() - clck;
  oyHash_Release( &hash );
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling(-1, i,clck/(double)CLOCKS_PER_SEC,"hash",
      "oyHash_SetPointer(PP)   %d",i) );

  free(hash_text); hash_text = NULL;
  oyFilterNode_Release( &icc );
  oyConversion_Release( &cc );
  oyProfile_Release( &pc.src_profile );
  oyProfile_Release( &pc.dst_profile );
  oyArray2d_Release( &clut );

  clck2 = oyClock() - clck2;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling(-1, i,clck2/(double)CLOCKS_PER_SEC,"clut",
      "PP fill node run hash - %d",i) );

  // test outside DB change
  char * old_daemon = oyGetPersistentString(OY_DEFAULT_DISPLAY_WHITE_POINT_DAEMON, 0, oySCOPE_USER_SYS, 0);
  int old_display_white_point = oyGetBehaviour( oyBEHAVIOUR_DISPLAY_WHITE_POINT );
  char * value = NULL;
  oyStringAddPrintf( &value, oyAllocateFunc_, oyDeAllocateFunc_,
                     "%d", old_display_white_point != 0 ? 0 : 7 );
  oyDBSetString( OY_DEFAULT_DISPLAY_WHITE_POINT_DAEMON, oySCOPE_USER,
                 old_daemon ? NULL : "oyranos-monitor-white-point",
                 "testing");
  oyDBSetString( OY_DEFAULT_DISPLAY_WHITE_POINT, oySCOPE_USER, value,
                 "testing");
  /* clear the DB cache */
  oyGetPersistentStrings( NULL );
  int display_white_point = oyGetBehaviour( oyBEHAVIOUR_DISPLAY_WHITE_POINT );
  if(verbose) fprintf(zout, "old_value: %d -> setting oyBEHAVIOUR_DISPLAY_WHITE_POINT: %s  check %d\n", old_display_white_point, value, display_white_point );
  oyFree_m_( value );

  pc.dst_profile = oyProfile_FromFile( "LStar-RGB.icc", icc_profile_flags, testobj );
  int dl_count2 = setupColourTable( &pc, 0 ); 
  if(verbose) fprintf(zout, "LStar-RGB.icc %d,%d,%d\n", pc.clut[r][g][b][0],pc.clut[r][g][b][1],pc.clut[r][g][b][2]);
  count = oyStructList_Count( oy_test_cache_ );
  if( !(c[0] == pc.clut[r][g][b][0] || c[1] == pc.clut[r][g][b][1] || c[2] == pc.clut[r][g][b][2]) &&
      count > 1 && dl_count != dl_count2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "cache difference                          %d %d!=%d", count, dl_count, dl_count2 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "cache difference                          %d %d!=%d", count, dl_count, dl_count2 );
  }
  oyProfile_Release( &pc.dst_profile );

  // reset to old value
  oySetBehaviour( oyBEHAVIOUR_DISPLAY_WHITE_POINT, oySCOPE_USER, old_display_white_point );
  if(old_daemon)
  {
    oySetPersistentString( OY_DEFAULT_DISPLAY_WHITE_POINT_DAEMON, oySCOPE_USER, old_daemon, NULL);
    oyFree_m_(old_daemon);
  }
  
  oyStructList_Release( &oy_test_cache_ );
  free(pc.output_name);

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}


oyjlTESTRESULT_e testRegistrationMatch ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  if( oyFilterRegistrationMatch(OY_INTERNAL "/icc_color.lcms",
                                "//" OY_TYPE_STD "/icc_color",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "simple CMM selection" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "simple CMM selection" );
  }

  if(!oyFilterRegistrationMatch(OY_INTERNAL "/icc_color.lcms",
                                "//" OY_TYPE_STD "/icc_color.octl",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "simple CMM selection no match" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "simple CMM selection no match" );
  }

  if( oyFilterRegistrationMatch(OY_INTERNAL "/icc_color.lcms",
                                "//" OY_TYPE_STD "/icc_color.4+lcms",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "special CMM selection" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "special CMM selection" );
  }

  if(!oyFilterRegistrationMatch(OY_INTERNAL "/icc_color.lcms",
                                "//" OY_TYPE_STD "/icc_color.4-lcms",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "special CMM avoiding" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "special CMM avoiding" );
  }

  if( oyFilterRegistrationMatch(OY_INTERNAL "/icc_color.lcms",
                                "//" OY_TYPE_STD "/icc_color.7-lcms",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "special CMM avoiding, other API" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "special CMM avoiding, other API" );
  }

  if( oyFilterRegistrationMatch("org/freedesktop/openicc/device/monitor/manufacturer",
                                "org/freedesktop/openicc/device/monitor/model",
                                oyOBJECT_NONE ) == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "device key mismatch" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "device key mismatch" );
  }

  if( oyFilterRegistrationMatch("org/freedesktop/openicc/device/monitor/manufacturer",
                                "org/freedesktop/openicc/device/monitor/manufacturer",
                                oyOBJECT_NONE ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "device key match" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "device key match" );
  }

  if( oyFilterRegistrationMatch("org/freedesktop/openicc/device/monitor/manufacturer/short",
                                "org/freedesktop/openicc/device/monitor/manufacturer/full",
                                oyOBJECT_NONE ) == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "long device key mismatch" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "long device key mismatch" );
  }

  if( oyFilterRegistrationMatch("org/freedesktop/openicc/device/monitor/manufacturer/short",
                                "org/freedesktop/openicc/device/monitor/manufacturer/short",
                                oyOBJECT_NONE ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "long device key match" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "long device key match" );
  }

  if( oyFilterRegistrationMatch("org/freedesktop/openicc/icc_color/display.icc_profile.abstract.white_point.automatic.oy-monitor",
                                "display.abstract.icc_profile",
                                oyOBJECT_NONE ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "changed key order match" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "changed key order match" );
  }

  if( oyFilterStringMatch( "abc-def-ghi",
                           "+def._ghi.-jkl", oyOBJECT_NONE, '/', '.',
                           OY_MATCH_SUB_STRING ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyFilterStringMatch(sub string match)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyFilterStringMatch(sub string match)" );
  }

  return result;
}

#include "oyranos_object_internal.h"
oyjlTESTRESULT_e test_oyTextIccDictMatch ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  const char * value  = "ABC",
             * value2 = "ABC";
  if( oyTextIccDictMatch(value, value2, 0, '/', ','))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "simple text    matching \"%s\"/\"%s\"", value, value2 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "simple text    matching \"%s\"/\"%s\"", value, value2 );
  }

  value  = "ABC";
  value2 = "ABCD";
  if(!oyTextIccDictMatch(value, value2, 0, '/', ','))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "simple text mismatching \"%s\"/\"%s\"", value, value2 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "simple text mismatching \"%s\"/\"%s\"", value, value2 );
  }

  value  = "abcd,ABC,efgh";
  value2 = "abcdef,12345,ABC";
  if( oyTextIccDictMatch(value, value2, 0, '/', ','))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "multiple text    matching \"%s\"/\"%s\"", value, value2 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "multiple text    matching \"%s\"/\"%s\"", value, value2 );
  }

  value  = "abcd,ABC,efgh";
  value2 = "abcdef,12345,ABCD";
  if(!oyTextIccDictMatch(value, value2, 0, '/', ','))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "multiple text mismatching \"%s\"/\"%s\"", value, value2 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "multiple text mismatching \"%s\"/\"%s\"", value, value2 );
  }

  value  = "abcd,ABC,efgh,12345px";
  value2 = "abcdef,12345,ABCD";
  if( oyTextIccDictMatch(value, value2, 0.0005, '/', ','))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "multiple integer    matching \"%s\"/\"%s\"", value, value2 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "multiple integer    matching \"%s\"/\"%s\"", value, value2 );
  }

  value  = "abcd,ABC,efgh,12345px";
  value2 = "abcdef,12345ABCD";
  if(!oyTextIccDictMatch(value, value2, 0.0005, '/', ','))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "multiple integer mismatching \"%s\"/\"%s\"", value, value2 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "multiple integer mismatching \"%s\"/\"%s\"", value, value2 );
  }

  value  = "abcd,ABC,efgh,123.45001";
  value2 = "abcdef,123.45,ABCD";
  if( oyTextIccDictMatch(value, value2, 0.0005, '/', ','))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "multiple float    matching \"%s\"/\"%s\"", value, value2 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "multiple float    matching \"%s\"/\"%s\"", value, value2 );
  }

  value  = "abcd,ABC,efgh,123.45";
  value2 = "abcdef,123";
  if(!oyTextIccDictMatch(value, value2, 0.0005, '/', ','))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "multiple float mismatching \"%s\"/\"%s\"", value, value2 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "multiple float mismatching \"%s\"/\"%s\"", value, value2 );
  }
  return result;
}

oyjlTESTRESULT_e testPolicy ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  char *data = 0;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

#if 0
  if(argc > 1)
  {
    fprintf(zout,"%s\n", argv[1]);
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
    xml = oyPolicyToXML( oyGROUP_ALL, 1, myAllocFunc );

  if( xml && xml[0] )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyPolicyToXML" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyPolicyToXML" );
  }

  if(xml)
  {
    oyReadXMLPolicy(oyGROUP_ALL, xml);
    if(verbose) fprintf(zout,"xml text: \n%s", xml);

    data = oyPolicyToXML( oyGROUP_ALL, 1, myAllocFunc );

    if( strcmp( data, xml ) == 0 )
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "Policy rereading" );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "Policy rereading                                  " );
    }

#ifdef USE_NEW
    delete [] xml;
    delete [] data;
#else
    free(xml);
    free(data);
#endif
  }

  const char ** names = NULL;
  int count = 0, i, current = -1;
  oyOptionChoicesGet( oyWIDGET_POLICY, &count, &names, &current );

  for(i = 0; i < count; ++i)
  {
    char * full_name = NULL;
    int error = oyPolicyFileNameGet( names[i],
                                 &full_name,
                                 oyAllocateFunc_ );
    if( full_name && error == 0 )
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyPolicyFileNameGet( %s ) = %s", names[i], full_name );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "oyPolicyFileNameGet( %s ) = %s", names[i], full_name );
    }
  }

  const char * opts[] = {"add_html_header","1",
                         "add_oyranos_title","1",
                         "add_oyranos_copyright","1",
                         "title","Title",
                         "version","0.5",
                         NULL};
  xml = oyDescriptionToHTML( oyGROUP_ALL, opts, oyAllocateFunc_ );
  if(xml)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyDescriptionToHTML() %d", (int)strlen(xml) );
    oyFree_m_(xml);
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyDescriptionToHTML() %d", (int)strlen(xml) );
  }

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

#include "oyranos_devices.h"

oyjlTESTRESULT_e testWidgets ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  const char * name = NULL,
             * tooltip = NULL;
  int          flags = 0;
  const oyGROUP_e * categories = NULL;
  OBJECT_COUNT_SETUP

  oyWIDGET_e option = oyWIDGET_RENDERING_INTENT;
  oyWIDGET_TYPE_e type = oyWidgetTitleGet( option, 
                                           &categories, &name, &tooltip,
                                           &flags );

  if( name && tooltip )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyWidgetTitleGet \"%s\" %d %s %d", name, type, tooltip, flags );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyWidgetTitleGet" );
  }

  int choices = 0;
  const char ** choices_string_list = NULL;
  int current = -1;
  int error = oyOptionChoicesGet2( option, 0, oyNAME_NAME,
                                   &choices, &choices_string_list,
                                   &current );

  if( !error && choices && choices_string_list && current != -1 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyOptionChoicesGet2 %d [%d]:            %s", choices, current, choices_string_list[current] );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptionChoicesGet2" );
  }
  const char * description = NULL;
  int choice = current;
  type = oyWidgetDescriptionGet( option, &description, choice );
  if( type == oyWIDGETTYPE_BEHAVIOUR && description )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyWidgetDescriptionGet  %s", description );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyWidgetDescriptionGet" );
  }

  oyOptionChoicesFree( option, &choices_string_list, choices );

  option = oyWIDGET_ASSUMED_WEB;
  type = oyWidgetTitleGet( option, &categories, &name, &tooltip, &flags );
  if( name && tooltip && (flags & OY_LAYOUT_NO_CHOICES) &&
      type == oyWIDGETTYPE_DEFAULT_PROFILE )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyWidgetTitleGet \"%s\"  %s %d", name, tooltip, flags );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyWidgetTitleGet" );
  }

  option = oyWIDGET_DISPLAY_WHITE_POINT;
  type = oyWidgetTitleGet( option, &categories, &name, &tooltip, &flags );
  error = oyOptionChoicesGet2( option, 0, oyNAME_NAME,
                               &choices, &choices_string_list,
                               &current );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptionChoicesGet2() error: %d", error )

  oyConfigs_s * configs = 0;
  oyOptions_s * options = NULL;
  error = oyOptions_SetFromString( &options,
                                     "//" OY_TYPE_STD "/config/command",
                                     "list", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyDevicesGet( 0, "monitor", options, &configs );
  if(error && displayFail() == oyjlTESTRESULT_FAIL) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDevicesGet() error: %d", error )
  int devices_n = oyConfigs_Count( configs );
  oyOptions_Release( &options );
  oyConfigs_Release( &configs );
  int i,j, duplicate = 0;
  for(i = 0; i < choices; ++i)
    for(j = i+1; j < choices; ++j)
      if(strcmp(choices_string_list[i],choices_string_list[j]) == 0)
        ++duplicate;
  if(choices == (7 + devices_n) &&
     duplicate == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyOptionChoicesGet2(%s) %d [%d]: %s", name, choices, current, choices_string_list[current] );
  } else
  { if(duplicate)
    {
      PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptionChoicesGet2(%s) duplicates detected: %d", name, duplicate );
    } else {
      PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptionChoicesGet2(%s) %d != 7 + %d", name, choices, devices_n );
    }
    for(i = 0; i < choices; ++i)
      fprintf(zout,"\t%s %s\n", i==current?"*":" ", choices_string_list[i]);
  }

  oyOptionChoicesFree( option, &choices_string_list, choices );

  oyMessageFuncSet( oyGuiMessageFunc );
  oyMessageFunc_p( oyMSG_ERROR, 0, "oyShowMessage() Test" );
  oyMessageFuncSet( oyMessageFunc );

  const char * t = oyPrintTime();
  if( strlen(t) )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyPrintTime() = %s", t );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyPrintTime() = %s", t );
  }

  fprintf( zout, "\n");

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}



oyjlTESTRESULT_e testCMMDevicesListing ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  int i, j, k, j_n, k_n;
  uint32_t count = 0,
         * rank_list = 0;
  int error = 0;
  char ** texts = 0,
        * val = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif

  /* get all configuration filters */
  oyConfigDomainList( "//" OY_TYPE_STD "/device/config.icc_profile",
                      &texts, &count, &rank_list ,0 );

  if( count )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
    "oyConfigDomainList Found CMM's" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
    "oyConfigDomainList Found CMM's" );
  }
  if(verbose) for( i = 0; texts && i < (int)count; ++i)
  {
    fprintf( zout, "%d: %s\n", i, texts[i] );
  }
  if(verbose) fprintf(zout, "\n" );

  oyConfigs_s * configs = 0;
  oyConfig_s * config = 0;
  oyOptions_s * options_list = 0;
  oyOption_s * o = 0;
  oyOption_s_ * oi = 0;
  oyProfile_s * p = 0;

  config = oyConfig_New( testobj );
  error = oyOptions_SetFromString( oyConfig_GetOptions( config,"backend_core"),
                                     "//" OY_TYPE_STD "/config/test",
                                     "testA", OY_CREATE_NEW );
  error = oyOptions_SetFromString( oyConfig_GetOptions( config,"db"),
                                     "//" OY_TYPE_STD "/config/test",
                                     "testB", OY_CREATE_NEW );
  error = oyOptions_SetFromString( oyConfig_GetOptions( config,"data"),
                                     "//" OY_TYPE_STD "/config/test",
                                     "testC", OY_CREATE_NEW );
  o = oyConfig_Find( config, "test" );
  oyOption_Release( &o );
  oyConfig_Release( &config );
  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "MultiConfig" )

  /* send a empty query to one module to obtain instructions in a message */
  if(texts && count)
  error = oyConfigs_FromDomain( texts[0], 0, &configs, testobj );
  if( !error )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyConfigs_FromDomain \"%s\" help text ", texts ? 
                                              oyNoEmptyString_m_(texts[0]) :"----");
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyConfigs_FromDomain \"%s\" help text ", texts ? 
                                              oyNoEmptyString_m_(texts[0]) :"----");
  }
  if(verbose) fprintf( zout, "\n");


  /* add list call to module arguments */
  error = oyOptions_SetFromString( &options_list,
                                 "//" OY_TYPE_STD "/config/command", "list",
                                 OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyOptions_SetFromString( &options_list,
                                 "//" OY_TYPE_STD "/config/icc_profile",
                                 "true", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )

  if(verbose) fprintf( zout, "oyConfigs_FromDomain() \"list\" call:\n" );
  for( i = 0; texts && i < (int)count; ++i)
  {
    const char * registration_domain = texts[i];
    if(verbose) fprintf(zout,"%d[rank %u]: %s\n", i, (unsigned int)rank_list[i], registration_domain);

    error = oyConfigs_FromDomain( registration_domain,
                                  options_list, &configs, testobj );
    if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyConfigs_FromDomain() error: %d", error )
    j_n = oyConfigs_Count( configs );
    for( j = 0; j < j_n; ++j )
    {
      oyConfigs_s * dbs = 0,
                  * heap = 0;
      int precise_count = 0,
          serial_count = 0,
          mnft_count = 0,
          dev_name_count = 0;

      config = oyConfigs_Get( configs, j );

      if(verbose) fprintf(zout, "--------------------------------------------------------------------------------\n\"%s\":\n", oyConfig_FindString( config, "device_name", 0 ) );
      {
        oyOptions_s * options = 0;
        const char * t = 0;
        oyOptions_SetFromString( &options,
                   "//" OY_TYPE_STD "/config/icc_profile.x_color_region_target",
                         "yes", OY_CREATE_NEW );
        error = oyDeviceGetProfile( config, options, &p );
        if(error > 0) PRINT_SUB( oyjlTESTRESULT_FAIL, "oyDeviceGetProfile() error: %d", error )
        oyOptions_Release( &options );
        t = oyProfile_GetText( p, oyNAME_DESCRIPTION);
        if(verbose) fprintf(zout, "oyDeviceGetProfile(): \"%s\"\n", t ? t : "----" );
        oyProfile_Release( &p );
      }

      error = oyConfigs_FromDB( oyConfig_GetRegistration( config ), NULL, &heap, testobj );
      if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyConfigs_FromDB() error: %d", error )

      error = oyDeviceSelectSimiliar( config, heap, 0, &dbs );
      if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDeviceSelectSimiliar() error: %d", error )
      precise_count = oyConfigs_Count( dbs );
      oyConfigs_Release( &dbs );

      error = oyDeviceSelectSimiliar( config, heap, 1, &dbs );
      if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDeviceSelectSimiliar() error: %d", error )
      serial_count = oyConfigs_Count( dbs );
      oyConfigs_Release( &dbs );

      error = oyDeviceSelectSimiliar( config, heap, 2, &dbs );
      if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDeviceSelectSimiliar() error: %d", error )
      mnft_count = oyConfigs_Count( dbs );
      oyConfigs_Release( &dbs );

      error = oyDeviceSelectSimiliar( config, heap, 4, &dbs );
      if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDeviceSelectSimiliar() error: %d", error )
      dev_name_count = oyConfigs_Count( dbs );
      oyConfigs_Release( &dbs );

      oyConfigs_Release( &heap );


      if(verbose) fprintf(zout, "\"%s\" has %d precise matches,\n"
              "\t%d manufacturer/model/serial, %d manufacturer/model and\n"
              "\t%d \"device_name\" entries in DB\n",
              oyConfig_FindString( config, "device_name", 0 ),
              precise_count,serial_count,mnft_count,dev_name_count );

      k_n = oyConfig_Count( config );
      for( k = 0; k < k_n; ++k )
      {
        char * r = 0;
        int mnft = -1, mn, pos;

        o = oyConfig_Get( config, k );
        oi = (oyOption_s_*) o;

        r = oyFilterRegistrationToText( oyOption_GetRegistration( o ),
                                        oyFILTER_REG_OPTION, 0 );
        if(r && strcmp(r,"supported_devices_info") == 0 &&
           oi->value_type == oyVAL_STRING_LIST)
        {
          /* print first line special */
          while(oi->value->string_list[++mnft])
          {
            mn = 0; pos = -1;
            while(oi->value->string_list[mnft][++pos])
            {
              if(oi->value->string_list[mnft][pos] == '\n')
              {
                if(mn && oi->value->string_list[mnft][pos+1])
                  putc(',', zout);
                else if(mn == 0)
                {
                  putc(':', zout);
                  putc('\n', zout);
                  putc(' ', zout);
                  putc(' ', zout);
                }
                ++mn;

              } else
                putc(oi->value->string_list[mnft][pos], zout);
            }
            putc('\n', zout);
          }
        } else
        {
          val = oyOption_GetValueText( o, oyAllocateFunc_ );
          if(verbose) fprintf(zout,"  %d::%d::%d \"%s\": \"%s\"\n", i,j,k,
                 oyOption_GetRegistration(o), val?val:"(nix)" );
        }

        if(r) { oyDeAllocateFunc_(r); r = 0; }
        if(val)
        { oyDeAllocateFunc_( val ); val = 0; }
        oyOption_Release( &o ); oi = 0;
      }

      oyConfig_Release( &config );
    }

    oyConfigs_Release( &configs );
  }
  if(verbose) fprintf( zout, "\n");
  oyOptions_Release( &options_list );

  fprintf( zout, "\n");

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

oyjlTESTRESULT_e testCMMDevicesDetails ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  int i, k, l, k_n;
  uint32_t count = 0,
         * rank_list = 0;
  int error = 0;
  char ** texts = 0,
        * val = 0;

  oyConfigs_s * configs = 0;
  oyConfig_s * config = 0;
  oyOptions_s * options = 0;
  oyOption_s * o = 0;
  int devices_n = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif

  /* get all configuration filters */
  oyConfigDomainList( "//" OY_TYPE_STD "/device/config.icc_profile",
                      &texts, &count, &rank_list ,0 );

  if( count )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
    "oyConfigDomainList Found CMM's" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
    "oyConfigDomainList Found CMM's" );
  }


  if(verbose)
  fprintf( zout, "oyConfigs_FromDomain() \"properties\" call:\n" );
  for( i = 0; rank_list && i < (int)count; ++i)
  {
    const char * registration_domain = texts[i];
    if(verbose)
    fprintf(zout,"%d[rank %u]: %s\n", i, (unsigned int)rank_list[i], registration_domain);

    /* set a general request */
    error = oyOptions_SetFromString( &options,
                                     "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );
    if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
    /* send the query to a module */
    error = oyConfigs_FromDomain( registration_domain,
                                  options, &configs, testobj );
    if(error) fprintf( zout, "oyConfigs_FromDomain(%s) error: %d", registration_domain, error );
    devices_n = oyConfigs_Count( configs );
    for( l = 0; l < devices_n; ++l )
    {
      /* display results */
      if(verbose)
      fprintf(zout, "--------------------------------------------------------------------------------\n%s:\n", registration_domain );
      config = oyConfigs_Get( configs, l );

      k_n = oyConfig_Count( config );
      for( k = 0; k < k_n; ++k )
      {
        o = oyConfig_Get( config, k );

        val = oyOption_GetValueText( o, oyAllocateFunc_ );
        if(verbose)
        fprintf(zout, "  %d::%d %s: \"%s\"\n", l,k, 
                  oyStrrchr_(oyOption_GetRegistration(o),'/')+1, val );

        if(val) { oyDeAllocateFunc_( val ); val = 0; }
        oyOption_Release( &o );
      }

      o = oyConfig_Find( config, "icc_profile" );
      if(o)
      {
        val = oyOption_GetValueText( o, oyAllocateFunc_ );
        if(verbose)
        fprintf(zout, "  %d %s: \"%s\"\n", l, 
                oyStrrchr_(oyOption_GetRegistration(o),'/')+1, val );

        if(val) { oyDeAllocateFunc_( val ); val = 0; }
        oyOption_Release( &o );
      }

        //error = oyConfig_SaveToDB( config );
      oyConfig_Release( &config );
    }

    oyConfigs_Release( &configs );
    oyOptions_Release( &options );
  }

  if(verbose) fprintf( zout, "\n");


  if(texts && texts[0])
  {
    oyConfDomain_s * domain = oyConfDomain_FromReg( texts[0], testobj );
    const char * device_class = oyConfDomain_GetText( domain, "device_class", oyNAME_NICK );
    char * class_key = NULL;
    oyStringAddPrintf( &class_key, oyAllocateFunc_, oyDeAllocateFunc_,
                       OY_STD"/device/%s", device_class );
    config = oyConfig_FromRegistration( class_key, 0 );
    oyFree_m_( class_key );
    oyConfDomain_Release( &domain );
  }
  error = oyConfig_AddDBData( config, "k1", "bla1", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyConfig_AddDBData() error: %d", error )
  error = oyConfig_AddDBData( config, "k2", "bla2", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyConfig_AddDBData() error: %d", error )
  error = oyConfig_AddDBData( config, "k3", "bla3", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyConfig_AddDBData() error: %d", error )

  if( !error  && config && oyOptions_Count(*oyConfig_GetOptions( config,"db") ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyConfig_AddDBData" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyConfig_AddDBData" );
  }

  char * registration = 0;
  if(config)
    registration = oyStringCopy( oyConfig_GetRegistration( config ),
                                  oyAllocateFunc_ );
  error = oyConfig_SaveToDB( config, oySCOPE_USER );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyConfig_SaveToDB() error: %d", error )

  error = oyConfigs_FromDB( registration, NULL, &configs, 0 );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyConfigs_FromDB() error: %d", error )
  count = oyConfigs_Count( configs );
  oyConfigs_Release( &configs );

  if( count > 0 )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
    "oyConfigs_FromDB()" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
    "oyConfigs_FromDB()" );
  }


  int32_t rank = 0;
  error = oyConfig_GetDB( config, NULL, &rank );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyConfig_GetDB() error: %d", error )
  const char * key_set_name = oyConfig_FindString( config, "key_set_name", 0 );
  char * key = 0;
  STRING_ADD( key, key_set_name );
  STRING_ADD( key, OY_SLASH );
  STRING_ADD( key, "k1" );
  o = oyConfig_Get( config, 0 );
  oyOption_SetRegistration( o, key );
  oyOption_Release( &o );
  if(key)
  { oyDeAllocateFunc_( key ); key = 0; }
  error = oyConfig_EraseFromDB( config, oySCOPE_USER );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyConfig_EraseFromDB() error: %d", error )
  oyConfig_Release( &config );

  error = oyConfigs_FromDB( registration, NULL, &configs, testobj );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyConfigs_FromDB() error: %d", error )
  i = oyConfigs_Count( configs );
  oyConfigs_Release( &configs );

  if( count - i == 1 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyConfig_EraseFromDB()                         %d/%d", (int)count,i );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyConfig_EraseFromDB()                         %d/%d", (int)count,i );
  }

  if(registration)
  { oyDeAllocateFunc_( registration ); registration = 0; }

  fprintf( zout, "\n");

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

oyjlTESTRESULT_e testCMMRankMap ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif

  int error = 0;
  oyConfig_s * device;
  oyOptions_s * options = 0;
  oyConfigs_s * devices = 0;

  oyOptions_SetFromString( &options,
                   "//" OY_TYPE_STD "/config/icc_profile.x_color_region_target",
                         "yes", OY_CREATE_NEW );
  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                   "properties", OY_CREATE_NEW );  
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyDevicesGet( OY_TYPE_STD, "monitor", options, &devices );
  if(error && displayFail() == oyjlTESTRESULT_FAIL) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDevicesGet() error: %d", error )
  oyOptions_Release( &options );

  int count = oyConfigs_Count( devices ),
      i;

  if( count )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "monitor(s) found                             %d (%d)", (int)count, error );
  } else if(displayFail() == oyjlTESTRESULT_FAIL)
  { PRINT_SUB( displayFail(),
    "no monitor found                            %d (%d)", (int)count, error );
  }

  for(i = 0; i < count; ++i)
  {
    char * json_text = 0;
    device = oyConfigs_Get( devices, i );
    oyDeviceToJSON( device, 0, &json_text, malloc );

    if( json_text && json_text[0] )
    { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(json_text),
    "json from device [%d]", i );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "json from device failed for [%d]", i );
    }
    OYJL_TEST_WRITE_RESULT( json_text, strlen(json_text), "oyDeviceToJSON", "txt" )

    const oyRankMap * map = oyConfig_GetRankMap( device );
    if( map )
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "Map from device  [%d]", i );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "Map from device failed for [%d]", i );
    }

    char * rank_map_text = 0;
    error = oyRankMapToJSON( map, options, &rank_map_text, malloc );
    if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyRankMapToJSON() error: %d", error )
    if( rank_map_text )
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "JSON from Map    [%d]                           %lu", i, (long unsigned int)strlen(rank_map_text) );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "JSON from Map failed  [%d]                   %d    ", i, error );
    }
    OYJL_TEST_WRITE_RESULT( rank_map_text, strlen(rank_map_text), "oyRankMapFromJSON", "txt" )

    oyRankMap * rank_map2 = 0;
    error = oyRankMapFromJSON( rank_map_text, options, &rank_map2, malloc );
    if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyRankMapFromJSON() error: %d", error )
    if( rank_map2 )
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "Map from JSON    [%d]", i );
    oyRankMapRelease( &rank_map2, free );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "Map from JSON failed [%d]      %d", i, error );
    }

    oyFree_m_(rank_map_text);
    oyConfig_Release( &device );


    oyDeviceFromJSON( json_text, 0, &device );
    if(json_text) { free( json_text ); json_text = 0; }
    oyDeviceToJSON( device, 0, &json_text, malloc );

    if( json_text && strlen(json_text) )
    { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(json_text),
    "json from device [%d]", i );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "json from device failed for %d", i );
    }
    OYJL_TEST_WRITE_RESULT( json_text, strlen(json_text), "oyDeviceToJSON", "txt" )

    oyConfig_Release( &device );
    oyFree_m_(json_text);
    if(verbose)
    fprintf( zout, "\n");
  }
  oyConfigs_Release( &devices );

  char ** list = NULL;
  error = oyRankMapList( NULL, NULL, &list, oyAllocateFunc_ );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyRankMapList() error: %d", error )
  count = 0;
  while( list && list[count]) ++count;
  if( count >= 3 )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
    "found installed rank maps" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "found too few installed rank m. %d %d ", count, error );
  }

  if(verbose)
  for( i = 0; i < count; ++i )
    fprintf( zout, "%d: %s\n", i, list[i] );

  oyStringListRelease_( &list, count, oyDeAllocateFunc_ );

  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/path",
                                   "1", OY_CREATE_NEW );  
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyRankMapList( NULL, options, &list, oyAllocateFunc_ );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyRankMapList() error: %d", error )
  oyOptions_Release( &options );
  count = 0;
  while( list && list[count]) ++count;
  if( count >= 1 )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
    "found rank map paths" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "found too few rank map paths    %d %d ", count, error );
  }

  if(verbose)
  for( i = 0; i < count; ++i )
  {
    fprintf( zout, "%d: %s\n", i, list[i] );
  }
  oyStringListRelease_( &list, count, oyDeAllocateFunc_ );

  fprintf( zout, "\n");

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

oyjlTESTRESULT_e testCMMMonitorJSON ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  int i;
  int error = 0;
  double clck = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif

  oyConfigs_s * configs = 0;
  oyConfig_s * config = 0;
  int devices_n = 0;
  oyOptions_s * options = NULL;
  char * first_json = NULL;

  clck = oyClock();
  error = oyOptions_SetFromString( &options,
                                     "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  error = oyDevicesGet( 0, "monitor", options, &configs );
  if( error <= 0 && configs )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyDevicesGet(\"monitor\")" );
  } else if(displayFail() == oyjlTESTRESULT_FAIL)
  { PRINT_SUB( displayFail(),
    "oyDevicesGet(\"monitor\")" );
  }

  devices_n = oyConfigs_Count( configs );
  if( error <= 0 && devices_n > 0 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyDevicesGet(\"monitor\")               devices:   %d", devices_n );
  } else if(displayFail() == oyjlTESTRESULT_FAIL)
  { PRINT_SUB( displayFail(),
    "oyDevicesGet(\"monitor\")               devices:   %d", devices_n );
  }

  clck = oyClock() - clck;
  for( i = 0; i < devices_n; ++i )
  {
    char * json_text = 0;
    config = oyConfigs_Get( configs, i );
    error = oyDeviceToJSON( config, 0, &json_text, malloc );
    if(verbose) fprintf(zout, "  %d oyDeviceToJSON():\n%s\n", i,
            json_text?json_text:"---" );

    oyConfig_Release( &config );
    if( !error && json_text )
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyDeviceToJSON() \"monitor\"" );
      if(i == 0)
        first_json = strdup(json_text);
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "oyDeviceToJSON() \"monitor\"                   %d", error );
    }

    if(i == 1)
    {
      if(strcmp(json_text,first_json) != 0)
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "found second unique monitor" );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "first and second monitor are equal" );
      }
    }

    error = oyDeviceFromJSON( json_text, 0, &config );
    if( !error && config )
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyDeviceFromJSON() %d", oyConfig_Count(config) );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "oyDeviceFromJSON() %d %d", oyConfig_Count(config), error );
    }

 
#   ifdef HAVE_X11
    if(i == 0)
    {
      int r OY_UNUSED;
      r = system("xprop -remove _ICC_PROFILE -root; xprop -remove _ICC_DEVICE_PROFILE -root");
    }
#   endif
    oyProfile_s * p = NULL;
    oyOptions_SetFromString( &options,
                  "//" OY_TYPE_STD "/config/command",
                           "list", OY_CREATE_NEW );
    oyOptions_SetFromString( &options,
                   "//" OY_TYPE_STD "/config/icc_profile.x_color_region_target",
                          "yes", OY_CREATE_NEW );
    error = oyDeviceGetProfile( config, options, &p );
    if(error > 0) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDeviceGetProfile() error: %d", error )
    if( p )
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyDeviceGetProfile(\"list\") \"%s\"", oyProfile_GetText(p,oyNAME_DESCRIPTION) );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "oyDeviceGetProfile(\"list\")  %d", error );
    }
    oyProfile_s * list_profile = p; p = NULL;

#   ifdef HAVE_X11
    if(i == 0 && verbose)
    {
      int r OY_UNUSED;
      r = system("xprop -root -len 4 | grep _ICC");
    }
#   endif

    oyOptions_SetFromString( &options,
                   "//" OY_TYPE_STD "/config/icc_profile.fallback",
                          "yes", OY_CREATE_NEW );
    error = oyDeviceGetProfile( config, options, &p );
    if(error == -1)
      PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyDeviceGetProfile(\"fallback\") error:           %d", error )
    else
      PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDeviceGetProfile(\"fallback\") error:       %d", error )
    if( p )
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyDeviceGetProfile(\"fallb.\")\"%s\"", oyProfile_GetText(p,oyNAME_DESCRIPTION) );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "oyDeviceGetProfile(\"fallback\")    " );
    }
    oyProfile_Release( &p );
    oyConfig_Release( &config );
    oyOptions_Release( &options );



    error = oyDeviceFromJSON( json_text, 0, &config );
    if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDeviceFromJSON() error: %d", error )
    oyOptions_SetFromString( &options,
                  "//" OY_TYPE_STD "/config/command",
                           "properties", OY_CREATE_NEW );
    oyOptions_SetFromString( &options,
                   "//" OY_TYPE_STD "/config/icc_profile.x_color_region_target",
                          "yes", OY_CREATE_NEW );
    error = oyDeviceGetProfile( config, options, &p );
    if( error <= 0 && p )
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyDeviceGetProfile(\"properties\") \"%s\"", oyProfile_GetText(p,oyNAME_DESCRIPTION) );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "oyDeviceGetProfile(\"properties\")  %d", error );
    }


    if(oyProfile_Equal(p,list_profile))
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "\"list\" == \"properties\" profile                    " );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "\"list\" == \"properties\" profile                    " );
    }
    oyProfile_Release( &p );
    oyProfile_Release( &list_profile );


    oyOptions_SetFromString( &options,
                   "//" OY_TYPE_STD "/config/icc_profile.fallback",
                          "yes", OY_CREATE_NEW );
    error = oyDeviceGetProfile( config, options, &p );
    if(error == -1)
      PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyDeviceGetProfile(\"fallback\") error:           %d", error )
    else
      PRINT_SUB( oyjlTESTRESULT_XFAIL,
      "oyDeviceGetProfile(\"fallback\") error:           %d", error )
    if( p )
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyDeviceGetProfile(\"fallb.\") \"%s\"", oyProfile_GetText(p,oyNAME_DESCRIPTION) );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "oyDeviceGetProfile(\"fallb.\")    " );
    }


    oyProfileTag_s * tag = oyProfile_GetTagById( p, (icTagSignature)icSigMetaDataTag );
    if( tag )
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyProfile_GetTagById(icSigMetaDataTag)" );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "oyProfile_GetTagById(icSigMetaDataTag)" );
    }

    int32_t texts_n = 0, tag_size = 0;
    char ** texts = oyProfileTag_GetText( tag, &texts_n, NULL, NULL,
                                          &tag_size, oyAllocateFunc_ );
    if( texts_n > 2 && tag_size )
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyProfileTag_GetText(meta) texts: %d tag size: %d", texts_n, tag_size );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "oyProfileTag_GetText(meta) texts: %d tag size: %d", texts_n, tag_size );
    }

    int edid_mnft_count = 0;
    for(int j = 0; j < texts_n; ++j)
    {
      if(verbose)
        fprintf( zout, "%s\n", texts[j] );
      if(strstr(texts[j],"EDID_mnft_id") != NULL)
        ++edid_mnft_count;
    }
    // one single key is needed
    // otherwise the monitor is defect or
    // there is key duplication
    if( edid_mnft_count == 1 )
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "Found EDID_mnft_id keys in meta tag:              %d", edid_mnft_count );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "Found EDID_mnft_id keys in meta tag:              %d", edid_mnft_count );
    }

    oyjlStringListRelease( &texts, texts_n, oyDeAllocateFunc_ );
    oyConfig_Release( &config );
    oyOptions_Release( &options );
    oyProfileTag_Release( &tag );
    oyProfile_Release( &p );
    oyFree_m_( json_text );
  }

  oyFree_m_( first_json );
  oyConfigs_Release( &configs );
  fprintf( zout, "\n");

  if(result == oyjlTESTRESULT_UNKNOWN && displayFail() == oyjlTESTRESULT_XFAIL)
    result = oyjlTESTRESULT_SUCCESS;

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}


oyjlTESTRESULT_e testCMMMonitorListing ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  int i, k, k_n;
  int error = 0;
  double clck = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif

  oyConfigs_s * configs = 0;
  oyConfig_s * config = 0;
  oyOption_s * o = 0;
  int devices_n = 0;
  char * device_name = 0;
  char * text = 0,
       * val = 0;

  clck = oyClock();
  error = oyDevicesGet( 0, "monitor", 0, &configs );
  clck = oyClock() - clck;
  devices_n = oyConfigs_Count( configs );
  if( !error )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, 1, clck/(double)CLOCKS_PER_SEC,"Obj.",
    "oyDevicesGet() \"monitor\": %d", devices_n );
  } else if(displayFail() == oyjlTESTRESULT_FAIL)
  { PRINT_SUB( displayFail(),
    "oyDevicesGet() \"monitor\": %d", devices_n );
  }
  for( i = 0; i < devices_n; ++i )
  {
    config = oyConfigs_Get( configs, i );
    fprintf(zout, "  %d oyConfig_FindString(..\"device_name\"..): %s\n", i,
            oyConfig_FindString( config, "device_name",0 ) );
    if(i==0)
      device_name = oyStringCopy(oyConfig_FindString( config, "device_name",0),
                                  oyAllocateFunc_ );

    clck = oyClock();
    error = oyDeviceProfileFromDB( config, &text, myAllocFunc );
    if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDeviceProfileFromDB() error: %d", error )
    clck = oyClock() - clck;
    if(text)
      fprintf( zout, "  %d oyDeviceProfileFromDB(): %s %s\n", i, text,
                   oyjlProfilingToString(1,clck/(double)CLOCKS_PER_SEC,"Obj."));
    else
      fprintf( zout, "  %d oyDeviceProfileFromDB(): ---\n", i );

    clck = oyClock();
    error = oyDeviceGetInfo( config, oyNAME_NICK, 0, &text, 0 );
    if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDeviceGetInfo() error: %d", error )
    clck = oyClock() - clck;
    fprintf( zout, "  %d oyDeviceGetInfo)(..oyNAME_NICK..): \"%s\" %s\n",
             i, text? text:"???",
                   oyjlProfilingToString(1,clck/(double)CLOCKS_PER_SEC,"Obj."));
    clck = oyClock();
    error = oyDeviceGetInfo( config, oyNAME_NAME, 0, &text, 0 );
    if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDeviceGetInfo() error: %d", error )
    clck = oyClock() - clck;
    fprintf( zout, "  %d oyDeviceGetInfo)(..oyNAME_NAME..): \"%s\" %s\n",
             i, text? text:"???",
                   oyjlProfilingToString(1,clck/(double)CLOCKS_PER_SEC,"Obj."));

    oyConfig_Release( &config );
  }
  oyConfigs_Release( &configs );
  if(verbose)
  fprintf( zout, "\n");

  error = oyDeviceGet( 0, "monitor", device_name, 0, &config );
  k_n = oyConfig_Count( config );
  if( !error )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, k_n,
    "oyDeviceGet(..\"monitor\" \"%s\"..)", device_name );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, k_n,
    "oyDeviceGet(..\"monitor\" \"%s\"..)", device_name );
  }
  if(verbose)
    for( k = 0; k < k_n; ++k )
    {
      o = oyConfig_Get( config, k );
      val = oyOption_GetValueText( o, oyAllocateFunc_ );

      fprintf(zout, "  %d %s: \"%s\"\n", k, oyOption_GetRegistration(o), val );

      if(val) { oyDeAllocateFunc_( val ); val = 0; }
      oyOption_Release( &o );
    }
  oyConfig_Release( &config );
  oyConfigs_Release( &configs );
  oyFree_m_( device_name );
  fprintf( zout, "\n");


  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

oyjlTESTRESULT_e testCMMDBListing ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  int j, k, j_n, k_n;
  int error = 0;
  oyConfigs_s * configs = 0;
  oyConfig_s * config = 0;
  oyOption_s * o = 0;
  char * val = 0;

  error = oyConfigs_FromDB( OY_DEVICE_STD "/monitor", NULL, &configs, testobj );
  j_n = oyConfigs_Count( configs );
  if( !error )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyConfigs_FromDB( \"" OY_DEVICE_STD "/monitor" "\" ) count: %d", j_n );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyConfigs_FromDB( \"" OY_DEVICE_STD "/monitor" "\" ) count: %d", j_n );
  }
  for( j = 0; j < j_n; ++j )
  {
    config = oyConfigs_Get( configs, j );

    k_n = oyConfig_Count( config );
    for( k = 0; k < k_n; ++k )
    {
      o = oyConfig_Get( config, k );

      val = oyOption_GetValueText( o, oyAllocateFunc_ );
      if(verbose)
      fprintf(zout, "  %d::%d %s %s\n", j,k, oyOption_GetRegistration(o), val );

      if(val) { oyDeAllocateFunc_( val ); val = 0; }
      oyOption_Release( &o );
    }

    oyConfig_Release( &config );
  }
  oyConfigs_Release( &configs );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

oyjlTESTRESULT_e testCMMMonitorModule ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  oyConfigs_s * devices = 0;
  oyOptions_s * options = 0;
  const char * t = ":0.100";
  int error = 0;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  /* non existing display */
  error = oyOptions_SetFromString( &options,
                                 "//" OY_TYPE_STD "/config/device_name",
                                 t, OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
  /* clean up */
  error = oyOptions_SetFromString( &options,
                                 "//" OY_TYPE_STD "/config/command",
                                 "unset", OY_CREATE_NEW );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "(oyOptions_SetFromString) error: %d", error )
  error = oyDevicesGet( OY_TYPE_STD, "monitor", options, &devices );
  oyConfigs_Release( &devices );
  oyOptions_Release( &options );

  if( error == -1
#ifdef __APPLE__
      ||  error == 0
#endif
    )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyDevicesGet( \"//" OY_TYPE_STD "\", unset, ... ) = %d", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyDevicesGet( \"//" OY_TYPE_STD "\", unset, ... ) = %d", error );
  }

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

oyjlTESTRESULT_e testCMMmonitorDBmatch ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  int k, k_n;
  int32_t rank = 0;
  int error = 0;
  oyConfig_s * device = 0;
  oyOption_s * o = 0;
  char * val = 0;
  double clck = 0;
  char * device_name = NULL;

  /* X11 */
#if !defined(__APPLE__)
  if(getenv("DISPLAY"))
  {
    device_name = oyStringCopy( getenv("DISPLAY"), oyAllocateFunc_ );
    if(strchr(device_name, '.') == NULL)
      oyStringAddPrintf( &device_name, oyAllocateFunc_, oyDeAllocateFunc_,
                         ".0" );
  }
#endif
  if(!device_name)
    device_name = oyStringCopy( "0", oyAllocateFunc_ );

  if(verbose) fprintf( zout, "load a device ...\n");
  clck = oyClock();
  error = oyDeviceGet( 0, "monitor", device_name, 0, &device );
  clck = oyClock() - clck;
  k_n = oyConfig_Count( device );
  if( !error )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, 1, clck/(double)CLOCKS_PER_SEC,"Obj.",
    "oyDeviceGet(..\"monitor\" \"%s\".. &device) %d ", device_name, k_n );
  } else if(displayFail() == oyjlTESTRESULT_FAIL)
  { PRINT_SUB( displayFail(),
    "oyDeviceGet(..\"monitor\" \"%s\".. &device) %d", device_name, k_n );
  }

  if(verbose) fprintf( zout, "... and search for the devices DB entry ...\n");
  clck = oyClock();
  error = oyConfig_GetDB( device, NULL, &rank );
  clck = oyClock() - clck;
  if( !error )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, 1, clck/(double)CLOCKS_PER_SEC,"Obj.",
    "oyConfig_GetDB( device )                  %d", (int)rank);
  } else
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_FAIL, 1, clck/(double)CLOCKS_PER_SEC,"Obj.",
    "oyConfig_GetDB( device )" );
  }
  if(device && rank > 0)
  {
    if(verbose) fprintf(zout,"rank: %d\n", (int)rank);
    k_n = oyConfig_Count( device );
    for( k = 0; k < k_n; ++k )
    {
      o = oyConfig_Get( device, k );

      val = oyOption_GetValueText( o, oyAllocateFunc_ );
      if(verbose) fprintf(zout, "  d::%d %s: \"%s\"\n", k,
      strchr(strchr(strchr(strchr(oyOption_GetRegistration(o),'/')+1,'/')+1,'/')+1,'/')+1,
              val );

      if(val) { oyDeAllocateFunc_( val ); val = 0; }
      oyOption_Release( &o );
    }
    //error = oyConfig_EraseFromDB( config );
  }

  oySetPersistentString( OY_STD "/device/test/[0]/system_port", oySCOPE_USER, "TEST-port", "TESTcomment" );
  oySetPersistentString( OY_STD "/device/test/[0]/model", oySCOPE_USER, "TEST-model", "TESTcomment" );
  oySetPersistentString( OY_STD "/device/test/[1]/system_port", oySCOPE_USER, "TEST-port2", "TESTcomment2" );
  oySetPersistentString( OY_STD "/device/test/[1]/model", oySCOPE_USER, "TEST-model2", "TESTcomment2" );
  if(verbose) fprintf(zout, "creating DB device class: \"%s\"\n", OY_STD "/device/test/#[0,1]/system_port: TEST-port/1" );

  const char * reg = OY_STD "/device/test";
  oyConfigs_s * configs = NULL;
  error = oyConfigs_FromDB( reg, NULL, &configs, testobj );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyConfigs_FromDB() error: %d", error )
  int count = oyConfigs_Count( configs );
  oyConfig_Release( &device );

  if(count == 2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyConfigs_FromDB( %s ) %d", reg, count );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyConfigs_FromDB( %s )  ", reg);
  }

  for(k = 0; k < count; ++k)
  {
    oyOptions_s * db;
    device = oyConfigs_Get( configs, k );
    db = *oyConfig_GetOptions(device,"db");
    if(verbose) fprintf(zout, "  d::%d %d: \"%s\"\n", k, oyConfig_Count( device ),
                  oyOptions_GetText( db, oyNAME_NICK ));
    int k_n = oyOptions_Count(db);
    if(k_n == 2)
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyConfig_s[%d] = %d", k, k_n );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "oyConfig_s[%d] = %d", k, k_n);
    }
    oyConfig_Release( &device );
  }

  oyConfigs_Release( &configs );

  error = oyDBEraseKey( reg, oySCOPE_USER );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyDBEraseKey() error: %d", error )

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}


#include "oyranos_forms.h"

#define H(type,value) oyFormsAddHeadline( &t, type, value,\
                                           oyAllocateFunc_, oyDeAllocateFunc_ );
#define CHOICE(ref,label,help) oyFormsStartChoice( &t, ref, label, help,\
                                           oyAllocateFunc_, oyDeAllocateFunc_ );
#define ITEM(value,label) oyFormsAddItem( &t, value, label,\
                                           oyAllocateFunc_, oyDeAllocateFunc_ );
#define CHOICE_END STRING_ADD( t, "      </xf:choices>\n     </xf:select1>\n" );

#include "oyCMMapi4_s_.h"
#include "oyCMMapi6_s_.h"
#include "oyCMMapi7_s_.h"
#include "oyCMMapi8_s_.h"
#include "oyCMMapi9_s_.h"
#include "oyCMMapi10_s_.h"
#include "oyCMMapiFilter_s_.h"
#include "oyCMMinfo_s_.h"

#include "oyranos_module.h"
#include "oyranos_module_internal.h"
#include "oyranos_xml.h"

oyjlTESTRESULT_e testCMMsShow ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  int i, j, k, l;
  uint32_t count = 0;
  char ** texts = 0,
        * text = 0,
        * text_tmp = (char*)oyAllocateFunc_(65535),
        * t = 0,
        * rfile = 0;
  oyCMMinfo_s_ * cmm_info = 0;
  oyCMMapi4_s_ * cmm_api4 = 0;
  oyCMMapi6_s_ * cmm_api6 = 0;
  oyCMMapi7_s_ * cmm_api7 = 0;
  oyCMMapi8_s_ * cmm_api8 = 0;
  oyCMMapi9_s_ * cmm_api9 = 0;
  oyCMMapi10_s_ * cmm_api10 = 0;
  oyCMMapi_s_ * tmp = 0;
  oyCMMapiFilter_s_ * cmm_filter = 0;
  OBJECT_COUNT_SETUP


  fprintf(zout, "\n" );

  texts = oyCMMsGetLibNames_p( &count );

  /* Create a oforms style xhtml to present in a XFORMS viewer like
   * oyranos-xforms-fltk or FF with XFORMS plug-in.
   * Pretty large here.
   */
  STRING_ADD( t, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n\
<html xmlns=\"http://www.w3.org/1999/xhtml\"\n\
      xmlns:xf=\"http://www.w3.org/2002/xforms\"\n\
      xmlns:oicc=\"http://www.oyranos.org/2009/oyranos_icc\">\n" );
  STRING_ADD( t, "<head>\n\
  <title>Filter options</title>\n\
  <xf:model>\n\
    <xf:instance xmlns=\"\">\n\
     <shared><dummy>0</dummy></shared>\n\
    </xf:instance>\n\
  </xf:model>\n");
  STRING_ADD( t, "  <style type=\"text/css\">\n\
  @namespace xf url(\"http://www.w3.org/2002/xforms\");\n\
  xf|label {\n\
   font-family: Helvetica, Geneva, Lucida, sans-serif;\n\
   width: 24ex;\n\
   text-align: right;\n\
   padding-right: 1em;\n\
  }\n\
  xf|select1 { display: table-row; }\n\
  xf|select1 xf|label, xf|choices xf|label  { display: table-cell; }\n\
  </style></head>\n\
<body>\n\
 <xf:group>\n" );
  H(3,"Oyranos Module Overview")

  for( i = 0; i < (int)count; ++i)
  {
    cmm_info = (oyCMMinfo_s_*)oyCMMinfoFromLibName_p( texts[i] );
    if(cmm_info)
      tmp = (oyCMMapi_s_*)cmm_info->api;
    else
      tmp = 0;

    /* short skip for non compatible modules */
    if(oyCMMapi_Check_((oyCMMapi_s*)tmp) == oyOBJECT_NONE)
    {
      text = oyCMMinfoPrint_( (oyCMMinfo_s*)cmm_info, 1 );
      STRING_ADD( text, "    Not accepted by oyCMMapi_Check_() - Stop\n");
      fprintf(zout,"%d: \"%s\": %s\n\n", i, texts[i], text );
      continue;
    }

    text = oyCMMinfoPrint_( (oyCMMinfo_s*)cmm_info, 0 );

        while(tmp)
        {
          oyOBJECT_e type = oyOBJECT_NONE;
          char num[48],
               * api_reg = 0;
          const char * ctmp;

          /* oforms */
          CHOICE( "shared/dummy", oyStructTypeToText(tmp->type_), text )
          ITEM( "0", cmm_info->cmm )
          CHOICE_END

          type = oyCMMapi_Check_((oyCMMapi_s*)tmp);

          oySprintf_(num,"    %d:", type );
          STRING_ADD( text, num );
          ctmp = oyStruct_TypeToText((oyStruct_s*)tmp);
          STRING_ADD( text, ctmp );
          STRING_ADD( text, "\n" );

          if(type == oyOBJECT_CMM_API5_S)
          {
            cmm_filter = (oyCMMapiFilter_s_*) tmp;

            {
              oyCMMapiFilter_s_ * api = 0;
              oyCMMapiFilters_s * apis = 0;
              uint32_t * rank_list = 0;
              uint32_t apis_n = 0;
              char * classe = 0;

              classe = oyFilterRegistrationToText( cmm_filter->registration,
                                                   oyFILTER_REG_TYPE, 0 );
              api_reg = oyStringCopy("//", oyAllocateFunc_ );
              STRING_ADD( api_reg, classe );
              if(classe)
              oyFree_m_( classe );


              H( 4, "API(s) load from Meta module" )
              STRING_ADD( text, "    API(s) load from Meta module:\n" );

              for(j = oyOBJECT_CMM_API4_S; j <= (int)oyOBJECT_CMM_API10_S; j++)
              {
                apis = oyCMMsGetFilterApis_( api_reg, (oyOBJECT_e)j,
                                             oyFILTER_REG_MODE_NONE,
                                             &rank_list, &apis_n );

                apis_n = oyCMMapiFilters_Count( apis );
                for(k = 0; rank_list && k < (int)apis_n; ++k)
                {
                  api = (oyCMMapiFilter_s_*)oyCMMapiFilters_Get( apis, k );

                  if(api)
                  snprintf( text_tmp, 65535,
                            "      [%s]: \"%s\"  %d\n        %s\n",
                            oyStructTypeToText(api->type_),
                            api->registration,
                            (int)rank_list[k], api->id_ );
                  else
                    sprintf(text_tmp,"      no api obtained %d",k);
                  STRING_ADD( text, text_tmp );
                  /* oforms */
                  CHOICE( "shared/dummy", oyStructTypeToText(api->type_), text_tmp )
                  ITEM( "0", api->registration )
                  CHOICE_END

                  if(api->type_ == oyOBJECT_CMM_API4_S)
                  {
                    cmm_api4 = (oyCMMapi4_s_*) api;
                    oyStringAdd_( &text, "        category: ",
                                  oyAllocateFunc_, oyDeAllocateFunc_ );
                    if(cmm_api4->ui->category)
                    oyStringAdd_( &text, cmm_api4->ui->category,
                                  oyAllocateFunc_, oyDeAllocateFunc_ );
                    oyStringAdd_( &text, "\n        options: ",
                                  oyAllocateFunc_, oyDeAllocateFunc_ );
                    if(cmm_api4->ui->options)
                    oyStringAdd_( &text, cmm_api4->ui->options,
                                  oyAllocateFunc_, oyDeAllocateFunc_ );
                    oyStringAdd_( &text, oyXMLgetElement_(cmm_api4->ui->options,
                                  "freedesktop.org/default/profile",
                                  "editing_rgb" ),
                                  oyAllocateFunc_, oyDeAllocateFunc_ );
                    STRING_ADD( text, "\n" );
                  }

                  if(api->type_ == oyOBJECT_CMM_API6_S)
                  {
                    cmm_api6 = (oyCMMapi6_s_*) api;
                    snprintf( text_tmp, 65535,
                            "        \"%s\" -> \"%s\"\n",
                            cmm_api6->data_type_in,
                            cmm_api6->data_type_out );
                    STRING_ADD( text, text_tmp );
                  }

                  if(api->type_ == oyOBJECT_CMM_API7_S)
                  {
                    cmm_api7 = (oyCMMapi7_s_*) api;
                    snprintf( text_tmp, 65535,
                            "        context type \"%s\" plugs: %d  sockets: %d\n",
                            cmm_api7->context_type,
                            cmm_api7->plugs_n + cmm_api7->plugs_last_add,
                            cmm_api7->sockets_n + cmm_api7->sockets_last_add );
                    STRING_ADD( text, text_tmp );
                    for(l = 0; l < (int)cmm_api7->plugs_n; ++l)
                    {
                      snprintf( text_tmp, 65535,
                            "        plug[%d]: type:\"%s\" id:\"%s\" \"%s\" \"%s\"\n", l,
                            oyNoEmptyString_m_(oyConnector_GetReg( cmm_api7->plugs[l])),
                            oyNoEmptyString_m_(oyConnector_GetText(
                                cmm_api7->plugs[l],"name",oyNAME_NICK)),
                            oyNoEmptyString_m_(oyConnector_GetText(
                                cmm_api7->plugs[l],"name",oyNAME_NAME)),
                            oyNoEmptyString_m_(oyConnector_GetText(
                                cmm_api7->plugs[l],"name",oyNAME_DESCRIPTION))
                            );
                      STRING_ADD( text, text_tmp );
                    }
                    for(l = 0; l < (int)cmm_api7->sockets_n; ++l)
                    {
                      snprintf( text_tmp, 65535,
                            "        sock[%d]: type:\"%s\" id:\"%s\" \"%s\" \"%s\"\n", l,
                            oyNoEmptyString_m_(oyConnector_GetReg( cmm_api7->sockets[l])),
                            oyNoEmptyString_m_(oyConnector_GetText(
                                cmm_api7->sockets[l],"name",oyNAME_NICK)),
                            oyNoEmptyString_m_(oyConnector_GetText(
                                cmm_api7->sockets[l],"name",oyNAME_NAME)),
                            oyNoEmptyString_m_(oyConnector_GetText(
                                cmm_api7->sockets[l],"name",oyNAME_DESCRIPTION))
                            );
                      STRING_ADD( text, text_tmp );
                    }
                  }

                  if(api->type_ == oyOBJECT_CMM_API8_S)
                  {
                    l = 0;
                    cmm_api8 = (oyCMMapi8_s_*) api;
                    snprintf( text_tmp, 65535,
                              "        rank_map[#]:"
                                      " \"key\"  match,none_match,not_found\n" );
                    STRING_ADD( text, text_tmp );
                    while(cmm_api8->rank_map && cmm_api8->rank_map[l].key)
                    {
                      snprintf( text_tmp, 65535,
                              "        rank_map[%d]: \"%s\"  %d,%d,%d\n", l,
                              cmm_api8->rank_map[l].key,
                              cmm_api8->rank_map[l].match_value,
                              cmm_api8->rank_map[l].none_match_value,
                              cmm_api8->rank_map[l].not_found_value
                              );
                      STRING_ADD( text, text_tmp );
                      ++l;
                    }
                  }

                  if(api->type_ == oyOBJECT_CMM_API9_S)
                  {
                    cmm_api9 = (oyCMMapi9_s_*) api;
                    snprintf( text_tmp, 65535,
                            "        \"%s\"\n"
                            "        supported pattern: \"%s\"\n",
                            cmm_api9->options,
                            cmm_api9->pattern );
                    STRING_ADD( text, text_tmp );
                  }

                  if(api->type_ == oyOBJECT_CMM_API10_S)
                  {
                    cmm_api10 = (oyCMMapi10_s_*) api;
                    for(l = 0; l < 3; ++l)
                    {
                      if(cmm_api10->texts[l])
                      {
                        snprintf( text_tmp, 65535,
                            "        \"%s\":\n"
                            "        \"%s\"\n",
                            cmm_api10->texts[l],
                            cmm_api10->getText( cmm_api10->texts[l],
                                                oyNAME_DESCRIPTION,
                                                (oyStruct_s*)cmm_api10 ) );
                        STRING_ADD( text, text_tmp );
                      } else
                        break;
                    }
                  }

                  STRING_ADD( text, "\n" );
                  if(api->release)
                    api->release( (oyStruct_s**)&api );
                }
                oyCMMapiFilters_Release( &apis );
                if( rank_list ) oyFree_m_( rank_list );
              }
              oyFree_m_(api_reg);
            }
          } else
          if(oyIsOfTypeCMMapiFilter( type ))
          {
            cmm_filter = (oyCMMapiFilter_s_*) tmp;

            snprintf( text_tmp, 65535, "%s: %s\n",
                      oyStructTypeToText( tmp->type_ ),
                      cmm_filter->registration );
            STRING_ADD( text, text_tmp );

          }
          tmp = (oyCMMapi_s_*)tmp->next;
        }

    if(verbose)
      fprintf(zout,"%d: \"%s\": %s\n\n", i, texts[i], text );

    oyFree_m_(text);
  }
  oyStringListRelease_( &texts, count, free );
  if(text) oyFree_m_( text );

  if( count )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyCMMsGetLibNames_p( ) found %u", (unsigned int)count );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyCMMsGetLibNames_p( ) found %u", (unsigned int)count );
  }

  oyDeAllocateFunc_( text_tmp );

  STRING_ADD( t, "     </xf:group>\n</body>\n</html>\n" );
  int r OY_UNUSED;
  r = remove("test2_CMMs.xhtml");
  oyWriteMemToFile2_( "test2_CMMs.xhtml", t, strlen(t),0/*OY_FILE_NAME_SEARCH*/,
                      &rfile, malloc );
  fprintf(zout, "Wrote %s\n", rfile?rfile:"test2_CMMs.xhtml" );
  oyFree_m_( rfile );
  oyFree_m_( t );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

#include "oyFilterCore_s_.h"
#include "oyNamedColor_s.h"
#include "oyNamedColors_s.h"
#include "oyranos_alpha_internal.h"
extern int oy_debug_image_read_array_count;

oyjlTESTRESULT_e testCMMnmRun ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP
  oyNamedColor_s * c = 0;
  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyProfile_s * prof = oyProfile_FromStd( oyEDITING_XYZ, icc_profile_flags, testobj );
  int error = 0, l_error = 0,
      i,n = 10, bign = 1;
  oyOptions_s * options = NULL;
  oyConversion_s * s = 0;
  oyConversion_s * conv   = NULL;
  oyImage_s * input  = NULL,
            * output = NULL;
  double * buf_in = &d[0],
         * buf_out = &d[3];
  oyDATATYPE_e buf_type_in = oyDOUBLE,
               buf_type_out = oyDOUBLE;
  oyProfile_s * p_in =  NULL,
              * p_out = NULL;
  oyFilterPlug_s * plug = NULL;
  oyFilterNode_s * out = NULL;
  oyPixelAccess_s * pixel_access = NULL;


  fprintf(stdout, "\n" );

  fprintf(zout, "clearing caches\n" );
  oyLibConfigRelease(0); \

  double clck = oyClock();
  for(i = 0; i < n*bign; ++i)
  {
    c = oyNamedColor_Create( NULL, NULL,0, prof, testobj );
    oyNamedColor_Release( &c );
  }
  clck = oyClock() - clck;
  oyProfile_Release( &prof );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "objects nm" )

  const char * key_domain = OY_STD"/behaviour";
  const char * key_name = OY_STD"/behaviour/rendering_bpc";
  clck = oyClock();
  for(i = 0; i < n*3; ++i)
  {
    oyDB_s * db = oyDB_newFrom( key_domain, oySCOPE_USER_SYS, oyAllocateFunc_, oyDeAllocateFunc_ );
    char * value = oyDB_getString(db, key_name);
    if(!value)
      break;
    oyFree_m_(value);
    oyDB_release( &db );
  }
  clck = oyClock() - clck;

  if( i )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"key",
    "oyDB_getString()");
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyDB_getString_(%s)", key_name );
  }


  clck = oyClock();
  oyDB_s * db = oyDB_newFrom( key_domain, oySCOPE_USER_SYS, oyAllocateFunc_, oyDeAllocateFunc_ );
  for(i = 0; i < n*3; ++i)
  {
    char * value = oyDB_getString(db, key_name);
    if(!value)
      break;
    oyFree_m_(value);
  }
  char * value_db = oyDB_getString(db, key_name);
  oyDB_release( &db );
  clck = oyClock() - clck;

  if( i )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"key",
    "oyDB_getString() shared  ");
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyDB_getString_(%s)", key_name );
  }

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "objects db" )


  char * value = oyGetPersistentString( key_name, 0, oySCOPE_USER_SYS,0 );
  if( (!value && !value_db) ||
      (value && value_db && strcmp(value,value_db) == 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "DB and cached values are equal");
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "DB and cached values are equal: \"%s\"/\"%s\"", value, value_db );
  }
  oyFree_m_(value);
  clck = oyClock();
  for(i = 0; i < n*3*17; ++i)
  {
    char * t = oyGetPersistentString( key_name, 0, oySCOPE_USER_SYS,0 );
    if(!t)
      break;
  }
  clck = oyClock() - clck;

  if( i>n )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"key",
    "oyGetPersistentString() cached ");
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyGetPersistentString(%s)", key_name );
  }


  clck = oyClock();
  oyOption_s * option = oyOption_FromRegistration(OY_STD"/behaviour/rendering_bpc", testobj);
  oyOption_SetFromString(option, "-1", 0);

  for(i = 0; i < n*3*17; ++i)
  {
    error = oyOption_SetValueFromDB( option );
    if(error > 0) break;
  }
  clck = oyClock() - clck;

  if( i > 1 && error <= 0 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Opt.",
    "oyOption_SetValueFromDB()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOption_SetValueFromDB(%s)", oyOption_GetText(option, oyNAME_NICK) );
  }
  oyOption_Release( &option );

  clck = oyClock();
  for(i = 0; i < 1; ++i)
  {
    oyOptions_s * options = oyOptions_ForFilter( "//" OY_TYPE_STD "/lcm2",
                                            oyOPTIONATTRIBUTE_ADVANCED  |
                                            oyOPTIONATTRIBUTE_FRONT |
                                            OY_SELECT_COMMON, testobj );
    oyOptions_Release( &options );
  }
  clck = oyClock() - clck;

  if( i )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Filter",
    "oyOptions_ForFilter() first");
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_ForFilter() first" );
  }

  clck = oyClock();
  for(i = 0; i < n; ++i)
  {
    oyOptions_s * options = oyOptions_ForFilter( "//" OY_TYPE_STD "/lcm2",
                                            oyOPTIONATTRIBUTE_ADVANCED  |
                                            oyOPTIONATTRIBUTE_FRONT |
                                            OY_SELECT_COMMON, testobj );
    oyOptions_Release( &options );
  }
  clck = oyClock() - clck;

  if( i )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Filter",
    "oyOptions_ForFilter()");
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_ForFilter()" );
  }

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "objects opts" )


  options = oyOptions_New(0);
  clck = oyClock();
  for(i = 0; i < n*bign; ++i)
  {
    oyFilterCore_s * core = oyFilterCore_NewWith( "//" OY_TYPE_STD "/root",
                                                  options, testobj );
    if(!core) break;
    oyFilterCore_Release( &core );
  }
  clck = oyClock() - clck;
  oyOptions_Release( &options );

  if( i )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Cores",
    "oyFilterCore_New()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyFilterCore_New()" );
  }


  clck = oyClock();
  const char * registration = "//" OY_TYPE_STD "/root";
  for(i = 0; i < n*bign; ++i)
  {
    oyCMMapi4_s_ * api4 = 0;
    api4 = (oyCMMapi4_s_*) oyCMMsGetFilterApi_(
                                            registration, oyOBJECT_CMM_API4_S );
    error = !api4;
    if(!(i%30000)) { fprintf(zout, "." ); fflush(zout); }
  }
  fprintf(zout,"\n");
  clck = oyClock() - clck;

  if( !error )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Cores",
    "oyFilterCore_New() oyCMMapi4_s" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyFilterCore_New() oyCMMapi4_s" );
  }

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "objects filts" )


  buf_in = &d[0];
  buf_out = &d[3];
  buf_type_in = oyDOUBLE;
  buf_type_out = oyDOUBLE;
  p_in =  oyProfile_FromStd( oyEDITING_XYZ, icc_profile_flags, testobj );
  p_out = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );

  input =oyImage_Create( 1,1, 
                         buf_in ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         0 );
  output=oyImage_Create( 1,1, 
                         buf_out ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                          oyDataType_m(buf_type_out),
                         p_out,
                         0 );
  error = !input || !output;

  options = oyOptions_New(0);

  clck = oyClock();
  for(i = 0; i < n; ++i)
  if(error <= 0)
  {
    s = oyConversion_CreateBasicPixels( input, output, options, testobj );
    oyConversion_Release( &s );
    //if(!(i%10)) fprintf(zout, "." ); fflush(stdout);
  }
  clck = oyClock() - clck;

  //fprintf(zout,"\n");
  oyImage_Release( &input );
  oyImage_Release( &output );

  if( !error )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Obj.",
    "oyConversion_CreateBasicPixels()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyConversion_CreateBasicPixels()" );
  }

  input =oyImage_Create( 1,1,
                         buf_in,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         testobj );
  output=oyImage_Create( 1,1,
                         buf_out,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                          oyDataType_m(buf_type_out),
                         p_out,
                         testobj );

  oyProfile_Release( &p_in );
  oyProfile_Release( &p_out );

  #define OY_ERR if(l_error != 0) error = l_error;

  s = oyConversion_CreateBasicPixels( input,output, 0, testobj );
  // create a basic job ticket for faster repeats of oyConversion_RunPixels()
  out = oyConversion_GetNode( s, OY_OUTPUT );
  if(s && out)
    plug = oyFilterNode_GetPlug( out, 0 );
  else
    error = 1;
  pixel_access = oyPixelAccess_Create( 0,0, plug,
                                       oyPIXEL_ACCESS_IMAGE, testobj );
  oyFilterPlug_Release( &plug );
  oyFilterNode_Release( &out );

  oy_debug_image_read_array_count = 0;
  clck = oyClock();
  for(i = 0; i < n*bign/10; ++i)
    if(error <= 0)
      error  = oyConversion_RunPixels( s, pixel_access );
  clck = oyClock() - clck;

  if( !error )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Pixel",
    "oyConversion_RunPixels( oyPixelAcce." );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyConversion_RunPixels()" );
  }

  oy_debug_image_read_array_count = 0;
  oyArray2d_s * pdata = (oyArray2d_s*) oyImage_GetPixelData( output ); // or use oyConversion_GetImage()
  oyImage_s * output_image = oyPixelAccess_GetOutputImage( pixel_access );
  if(output_image != output)
    fprintf( zout, "Can not optimise last output copy\n" );
  oyArray2d_s * array = oyPixelAccess_GetArray( pixel_access );
  if(array != pdata) // should be typical the case
    oyPixelAccess_SetArray( pixel_access, (oyArray2d_s*)pdata, 0 );
  clck = oyClock();
  for(i = 0; i < n*bign/10; ++i)
  if(error <= 0)
    error  = oyConversion_RunPixels( s, pixel_access );
  clck = oyClock() - clck;
  oyArray2d_Release( &pdata );
  oyArray2d_Release( &array );

  if( !oy_debug_image_read_array_count )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Pixel",
    "oyConversion_RunPixels( array fast )" );
  } else
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_FAIL, i,clck/(double)CLOCKS_PER_SEC, "Pixel",
    "oyConversion_RunPixels( array fast )" );
  }

  oyImage_Release( &output_image );
  oyConversion_Release ( &s );
  oyPixelAccess_Release( &pixel_access );


  s = oyConversion_CreateBasicPixels( input,output, options, testobj );
  oyImage_Release( &input );
  oyImage_Release( &output );
  clck = oyClock();
  for(i = 0; i < n; ++i)
  if(error <= 0)
  {
    error  = oyConversion_RunPixels( s, 0 );
    //if(!(i%10)) fprintf(zout, "." ); fflush(zout);
  }
  //printf(zout, "\n" );
  clck = oyClock() - clck;
  oyConversion_Release ( &s );

  if( !error )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Pixel",
    "oyConversion_RunPixels()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyConversion_RunPixels()" );
  }
  oyOptions_Release( &options );


  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "objects convs" )


  prof = oyProfile_FromStd( oyEDITING_XYZ, icc_profile_flags, testobj );
  c = oyNamedColor_Create( NULL, NULL,0, prof, testobj );
  oyProfile_Release( &prof );
  if( c )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Obj.",
    "oyNamedColor_Create( )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyNamedColor_Create( )" );
  }


  clck = oyClock();

  for(i = 0; i < n && error <= 0; ++i)
  {
    l_error = oyNamedColor_SetColorStd ( c, oyASSUMED_WEB,
                                           (oyPointer)d, oyDOUBLE, 0, options );
    if(error <= 0)
      error = l_error;
  }
  clck = oyClock() - clck;
  oyNamedColor_Release( &c );

  if( !error )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Pixel",
    "oyNamedColor_SetColorStd()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyNamedColor_SetColorStd() oyASSUMED_WEB" );
  }

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "objects nm++" )

  p_in = oyProfile_FromStd ( oyASSUMED_WEB, icc_profile_flags, testobj );
  p_out = oyProfile_FromStd ( oyEDITING_XYZ, icc_profile_flags, testobj );

  clck = oyClock();
  for(i = 0; i < n; ++i)
  {

  oyImage_s * in  = NULL,
            * out = NULL;
  oyConversion_s * conv = NULL;

  in    = oyImage_Create( 1,1, 
                         buf_in ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         0 );
  out   = oyImage_Create( 1,1, 
                         buf_out ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                          oyDataType_m(buf_type_out),
                         p_out,
                         0 );

  conv   = oyConversion_CreateBasicPixels( in,out, options, testobj );
  if(!error)
  error  = oyConversion_RunPixels( conv, 0 );

  oyConversion_Release( &conv );
  oyImage_Release( &in );
  oyImage_Release( &out );
  }
  clck = oyClock() - clck;

  oyProfile_Release( &p_in );
  oyProfile_Release( &p_out );

  if( !error )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Pixel",
    "oyColorConvert_()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyColorConvert_()" );
  }

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "objects cc" )


  p_in = oyProfile_FromStd ( oyASSUMED_WEB, icc_profile_flags, testobj );
  p_out = oyProfile_FromStd ( oyEDITING_XYZ, icc_profile_flags, testobj );
  input  = oyImage_Create( 1,1, 
                         buf_in ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         testobj );
  output = oyImage_Create( 1,1, 
                         buf_out ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                          oyDataType_m(buf_type_out),
                         p_out,
                         testobj );
  oyProfile_Release( &p_in );
  oyProfile_Release( &p_out );

  conv = oyConversion_CreateBasicPixels( input,output, 0,testobj );
  oyImage_Release( &input );
  oyImage_Release( &output );

  out = oyConversion_GetNode( conv, OY_OUTPUT );

  /* conversion->out_ has to be linear, so we access only the first plug */
  if(conv)
    plug = oyFilterNode_GetPlug( out, 0 );
  oyFilterNode_Release (&out );

  /* create a very simple pixel iterator as job ticket */
  if(plug)
    pixel_access = oyPixelAccess_Create( 0,0, plug,
                                           oyPIXEL_ACCESS_IMAGE, testobj );
  oyFilterPlug_Release( &plug );
  error  = oyConversion_RunPixels( conv, pixel_access );

  clck = oyClock();
  if(pixel_access)
  for(i = 0; i < n*bign/10; ++i)
  {
    if(!error)
      error  = oyConversion_RunPixels( conv, pixel_access );
  }
  clck = oyClock() - clck;



  if( !error )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Pixel",
    "oyColorConvert_() with oyPixelAcce." );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyColorConvert_() with oyPixelAccess_Create()" );
  }

  clck = oyClock();
  d[0] = d[1] = d[2] = 1.0;
  d[3] = d[4] = d[5] = 0.0;
  if(pixel_access)
  for(i = 0; i < n*bign/10; ++i)
  {
    if(!error)
      error = oyConversion_GetOnePixel( conv, 0,0, pixel_access );
  }
  clck = oyClock() - clck;

  if( !error  && d[3] != 0.0 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Pixel",
    "oyConversion_GetOnePixel()%.02g %.02g %.02g",
                           d[3], d[4], d[5]);
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyConversion_GetOnePixel( oyPix. )  %s %.02g %.02g %.02g" ,
                          oyjlProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"), d[3], d[4], d[5]);
  }
  oyPixelAccess_Release( &pixel_access );
  oyConversion_Release( &conv );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "objects conv" )


  conv = oyConversion_New( testobj );
  oyFilterNode_s * in_node = oyFilterNode_NewWith( "//" OY_TYPE_STD "/root", 0, testobj );
  oyConversion_Set( conv, in_node, 0 );
  p_in = oyProfile_FromStd ( oyASSUMED_WEB, icc_profile_flags, testobj );
  input  = oyImage_Create( 1,1, 
                         buf_in ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         testobj );
  oyProfile_Release( &p_in );
  oyFilterNode_SetData( in_node, (oyStruct_s*)input, 0, 0 );
  oyImage_Release( &input );
  oyFilterNode_s * out_node = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", 0, testobj );
  /*oyFilterNode_SetData( out_node, (oyStruct_s*)output, 0, 0 );*/
  error = oyFilterNode_Connect( in_node, "//" OY_TYPE_STD "/data",
                                out_node, "//" OY_TYPE_STD "/data", 0 );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyFilterNode_Connect() error: %d", error )
  oyConversion_Set( conv, 0, out_node );
  //oyConversion_GetNode( conv, OY_OUTPUT );
  plug = oyFilterNode_GetPlug( out_node, 0 );
  //oyFilterNode_Release( &out_node );

  /* create a very simple pixel iterator as job ticket */
  if(plug)
    pixel_access = oyPixelAccess_Create( 0,0, plug,
                                         oyPIXEL_ACCESS_IMAGE, testobj );
  oyFilterPlug_Release( &plug );
  error  = oyConversion_RunPixels( conv, pixel_access );

  clck = oyClock();
  for(i = 0; i < n*bign/10; ++i)
  {
    if(!error)
      error = oyConversion_RunPixels( conv, pixel_access );
  }
  clck = oyClock() - clck;


  oyPixelAccess_Release( &pixel_access );
  oyConversion_Release( &conv );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "objects 2pix" )



  if( !error )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Pixel",
    "oyConversion_RunPixels (2 nodes)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyConversion_RunPixels (2 nodes)" );
  }

  p_in = oyProfile_FromStd ( oyASSUMED_WEB, icc_profile_flags, testobj );
  p_out = oyProfile_FromStd ( oyEDITING_XYZ, icc_profile_flags, testobj );
  conv = oyConversion_CreateBasicPixelsFromBuffers(
                                        p_in, buf_in, oyDataType_m(buf_type_in),
                                        p_out, buf_out, oyDataType_m(buf_type_out),
                                        0, 1 );
  oyConversion_Release( &conv );



  oyProfile_s * p_cmyk = oyProfile_FromStd( oyEDITING_CMYK, icc_profile_flags, testobj );
  oyNamedColor_s * ncl = 0;
  oyNamedColors_s * colors = oyNamedColors_New(testobj);

  oyNamedColors_SetPrefix( colors, "test" );
  oyNamedColors_SetSuffix( colors, "color" );

  if(p_cmyk)
  {
    clck = oyClock();
    for( i = 0; i < bign/10; ++i )
    {
      char name[12];
      double lab[3], device[4] = {0.2,0.2,0.1,0.5};

      lab[0] = i*0.001;
      lab[1] = lab[2] = 0.5;
      sprintf( name, "%d", i );

      ncl = oyNamedColor_CreateWithName( name, name, name, NULL, NULL, NULL, 0, p_cmyk, testobj );

      if(!error)
        error = oyNamedColor_SetColorStd( ncl, oyEDITING_LAB, lab, oyDOUBLE, 0, NULL );
      if(!error)
        oyNamedColor_SetChannels( ncl, device, 0 );

      oyNamedColors_MoveIn( colors, &ncl, i );
    }
    clck = oyClock() - clck;

    if( !error )
    { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Ncl",
      "oyNamedColor_CreateWithName()" );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "oyNamedColor_CreateWithName()" );
    }
  }

  oyNamedColors_Release( &colors );
  oyProfile_Release( &p_in );
  oyProfile_Release( &p_out );
  oyProfile_Release( &p_cmyk );
  oyOptions_Release( &options );
  //oyFilterNode_Release( &in_node );


  
  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

#include "oyRectangle_s_.h"

oyjlTESTRESULT_e testImagePixel()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP
  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyProfile_s * p_lab = oyProfile_FromStd( oyEDITING_LAB, icc_profile_flags, testobj );
  oyProfile_s * p_web = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  oyProfile_s /** p_cmyk = oyProfile_FromStd( oyEDITING_CMYK, NULL ),*/
              * p_in, * p_out;
  int error = 0,
      i,n = 1, bign = 1000;
  uint16_t buf_16in2x2[12] = {
  20000,20000,20000, 10000,10000,10000,
  0,0,0,             65535,65535,65535
  };
  uint16_t buf_16out2x2[12];
  oyDATATYPE_e buf_type_in = oyUINT16,
               buf_type_out = oyUINT16;
  oyImage_s *input, *output;

  fprintf(stdout, "\n" );

  p_in = p_web;
  p_out = p_lab;
  input =oyImage_Create( 2,2, 
                         buf_16in2x2,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         testobj );
  output=oyImage_Create( 2,2, 
                         buf_16out2x2,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                          oyDataType_m(buf_type_out),
                         p_out,
                         testobj );

  oyFilterPlug_s * plug = 0;
  oyPixelAccess_s * pixel_access = 0;
  oyConversion_s * cc;
  memset( buf_16out2x2, 0, sizeof(uint16_t)*12 );
  cc = oyConversion_CreateBasicPixels( input,output, 0, testobj );
  oyFilterNode_s * out = oyConversion_GetNode( cc, OY_OUTPUT );
  if(cc && out)
    plug = oyFilterNode_GetPlug( out, 0 );
  else
    error = 1;
  oyFilterNode_Release( &out );
  pixel_access = oyPixelAccess_Create( 0,0, plug,
                                           oyPIXEL_ACCESS_IMAGE, testobj );
  oyFilterPlug_Release( &plug );

  double clck1 = oyClock(), clck;
  error  = oyConversion_RunPixels( cc, pixel_access );
  clck1 = oyClock() - clck1;
  clck = oyClock();
  for(i = 0; i < n*bign; ++i)
  if(error <= 0)
  {
    error  = oyConversion_RunPixels( cc, pixel_access );
  }
  clck = oyClock() - clck;
  if(clck1/2.0 > clck/i)
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"X",
    "oyConversion_RunPixels() caches %s",
                          oyjlProfilingToString(1,clck1/(double)CLOCKS_PER_SEC, "1th"));
  } else
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_FAIL, i,clck/(double)CLOCKS_PER_SEC,"X",
    "oyConversion_RunPixels() caches %s",
                          oyjlProfilingToString(1,clck1/(double)CLOCKS_PER_SEC, "1th"));
  }

  if( !error &&
      /* check unchanged input buffer */
      buf_16in2x2[0]==20000 && buf_16in2x2[1]==20000 && buf_16in2x2[2]==20000 &&
      buf_16in2x2[3]==10000 && buf_16in2x2[4]==10000 && buf_16in2x2[5]==10000 &&
      buf_16in2x2[6]==0 && buf_16in2x2[7]==0 && buf_16in2x2[8]==0 &&
      buf_16in2x2[9]==65535 && buf_16in2x2[10]==65535 &&buf_16in2x2[11]==65535&&
      /* check black and white in lower row with typical *ab of 32896 */
      buf_16out2x2[6]<5000 && buf_16out2x2[7]>20000 && buf_16out2x2[7]<40000 &&
      buf_16out2x2[9]>65000 && buf_16out2x2[10]>20000 && buf_16out2x2[10]<40000
      )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, 4*i,clck/(double)CLOCKS_PER_SEC,"Pixel",
    "Plain Image" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "Plain Image" );
  }

  if(verbose) fprintf(zout, "input:  %d,%d,%d %d,%d,%d\n        %d,%d,%d %d,%d,%d\n",
                  buf_16in2x2[0], buf_16in2x2[1], buf_16in2x2[2],
                  buf_16in2x2[3], buf_16in2x2[4], buf_16in2x2[5],
                  buf_16in2x2[6], buf_16in2x2[7], buf_16in2x2[8],
                  buf_16in2x2[9], buf_16in2x2[10], buf_16in2x2[11] );
  if(verbose) fprintf(zout, "output: %d,%d,%d %d,%d,%d\n        %d,%d,%d %d,%d,%d\n",
                  buf_16out2x2[0], buf_16out2x2[1], buf_16out2x2[2],
                  buf_16out2x2[3], buf_16out2x2[4], buf_16out2x2[5],
                  buf_16out2x2[6], buf_16out2x2[7], buf_16out2x2[8],
                  buf_16out2x2[9], buf_16out2x2[10], buf_16out2x2[11] );



  buf_16in2x2[0]=buf_16in2x2[1]=buf_16in2x2[2]=20000;
  buf_16in2x2[3]=buf_16in2x2[4]=buf_16in2x2[5]=10000;
  buf_16in2x2[6]=buf_16in2x2[7]=buf_16in2x2[8]=0;
  buf_16in2x2[9]=buf_16in2x2[10]=buf_16in2x2[11]=65535;
  memset( buf_16out2x2, 0, sizeof(uint16_t)*12 );
  /* use the lower left source pixel */
  if(pixel_access)
  {
    oyRectangle_s * r = oyPixelAccess_GetArrayROI( pixel_access );
    (*oyRectangle_SetGeo1(r,2)) *= 0.5;
    (*oyRectangle_SetGeo1(r,3)) *= 0.5;
    oyPixelAccess_ChangeRectangle( pixel_access, 0.5,0.5, r );
    oyRectangle_Release( &r );
  }
  clck = oyClock();
  for(i = 0; i < n*bign; ++i)
  if(error <= 0)
  {
    error  = oyConversion_RunPixels( cc, pixel_access );
  }
  clck = oyClock() - clck;

  if( !error &&
      /* input should not change */
      buf_16in2x2[0]==20000 && buf_16in2x2[1]==20000 && buf_16in2x2[2]==20000 &&
      buf_16in2x2[3]==10000 && buf_16in2x2[4]==10000 && buf_16in2x2[5]==10000 &&
      buf_16in2x2[6]==0 && buf_16in2x2[7]==0 && buf_16in2x2[8]==0 &&
      buf_16in2x2[9]==65535 && buf_16in2x2[10]==65535 &&buf_16in2x2[11]==65535&&
      /* the result shall appear in the upper left corner / first pixel */
      buf_16out2x2[0]>65000 && buf_16out2x2[1]>20000 && buf_16out2x2[2]<40000&&
      /* all other buffer pixels shall remain untouched */
      buf_16out2x2[3]==0 && buf_16out2x2[4]==0 && buf_16out2x2[5]==0 &&
      buf_16out2x2[6]==0 && buf_16out2x2[7]==0 && buf_16out2x2[8]==0 &&
      buf_16out2x2[9]==0 && buf_16out2x2[10]==0 && buf_16out2x2[11]==0
      )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Pixel",
    "lower right source pixel in 1 pixel RoI" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "lower right source pixel in 1 pixel RoI" );
  }

  if(verbose) fprintf(zout, "input:  %d,%d,%d %d,%d,%d\n        %d,%d,%d %d,%d,%d\n",
                  buf_16in2x2[0], buf_16in2x2[1], buf_16in2x2[2],
                  buf_16in2x2[3], buf_16in2x2[4], buf_16in2x2[5],
                  buf_16in2x2[6], buf_16in2x2[7], buf_16in2x2[8],
                  buf_16in2x2[9], buf_16in2x2[10], buf_16in2x2[11] );
  if(verbose) fprintf(zout, "output: %d,%d,%d %d,%d,%d\n        %d,%d,%d %d,%d,%d\n",
                  buf_16out2x2[0], buf_16out2x2[1], buf_16out2x2[2],
                  buf_16out2x2[3], buf_16out2x2[4], buf_16out2x2[5],
                  buf_16out2x2[6], buf_16out2x2[7], buf_16out2x2[8],
                  buf_16out2x2[9], buf_16out2x2[10], buf_16out2x2[11] );


  buf_16in2x2[0]=buf_16in2x2[1]=buf_16in2x2[2]=20000;
  buf_16in2x2[3]=buf_16in2x2[4]=buf_16in2x2[5]=10000;
  buf_16in2x2[6]=buf_16in2x2[7]=buf_16in2x2[8]=0;
  buf_16in2x2[9]=buf_16in2x2[10]=buf_16in2x2[11]=65535;
  memset( buf_16out2x2, 0, sizeof(buf_16out2x2) );
  if(pixel_access)
  {
    oyRectangle_s * r = oyPixelAccess_GetArrayROI( pixel_access );
    oyRectangle_SetGeo(r, 0.5,0.5, 0.5,0.5);
    oyPixelAccess_ChangeRectangle( pixel_access, 0.5,0.5, r );
    oyRectangle_Release( &r );
  }
  clck = oyClock();
  for(i = 0; i < n*bign; ++i)
  if(error <= 0)
  {
    error  = oyConversion_RunPixels( cc, pixel_access );
  }
  clck = oyClock() - clck;

  if( !error &&
      /* input should not change */
      buf_16in2x2[0]==20000 && buf_16in2x2[1]==20000 && buf_16in2x2[2]==20000 &&
      buf_16in2x2[3]==10000 && buf_16in2x2[4]==10000 && buf_16in2x2[5]==10000 &&
      buf_16in2x2[6]==0 && buf_16in2x2[7]==0 && buf_16in2x2[8]==0 &&
      buf_16in2x2[9]==65535 && buf_16in2x2[10]==65535 &&buf_16in2x2[11]==65535&&
      /* the result shall appear in the lower right corner / last pixel */
      buf_16out2x2[9]>65000 && buf_16out2x2[10]>20000&&buf_16out2x2[11]<40000&&
      /* all other buffer pixels shall remain untouched */
      buf_16out2x2[0]==0 && buf_16out2x2[1]==0 && buf_16out2x2[2]==0 &&
      buf_16out2x2[3]==0 && buf_16out2x2[4]==0 && buf_16out2x2[5]==0 &&
      buf_16out2x2[6]==0 && buf_16out2x2[7]==0 && buf_16out2x2[8]==0
      )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"Pixel",
    "lower right source in lower right output" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "lower right source in lower right output" );
  }

  if(verbose) fprintf(zout, "input:  %d,%d,%d %d,%d,%d\n        %d,%d,%d %d,%d,%d\n",
                  buf_16in2x2[0], buf_16in2x2[1], buf_16in2x2[2],
                  buf_16in2x2[3], buf_16in2x2[4], buf_16in2x2[5],
                  buf_16in2x2[6], buf_16in2x2[7], buf_16in2x2[8],
                  buf_16in2x2[9], buf_16in2x2[10], buf_16in2x2[11] );
  if(verbose) fprintf(zout, "output: %d,%d,%d %d,%d,%d\n        %d,%d,%d %d,%d,%d\n",
                  buf_16out2x2[0], buf_16out2x2[1], buf_16out2x2[2],
                  buf_16out2x2[3], buf_16out2x2[4], buf_16out2x2[5],
                  buf_16out2x2[6], buf_16out2x2[7], buf_16out2x2[8],
                  buf_16out2x2[9], buf_16out2x2[10], buf_16out2x2[11] );


  oyConversion_Release ( &cc );
  oyPixelAccess_Release( &pixel_access );
  oyProfile_Release( &p_lab );
  oyProfile_Release( &p_web );

  oyPixel_t pixel_layout = OY_TYPE_123_16;
  oyPointer channels = 0;
  oyRectangle_s_ roi_ = {oyOBJECT_RECTANGLE_S, 0,0,0,0,0,0,0};
  oyRectangle_s_ a_roi_ = {oyOBJECT_RECTANGLE_S, 0,0,0,0,0,0,0};
  oyRectangle_s * roi = (oyRectangle_s*)&roi_,
                * a_roi = (oyRectangle_s*)&a_roi_;

  oyArray2d_s * a = oyArray2d_Create( channels,
                                      2 * oyToChannels_m(pixel_layout),
                                      2,
                                      oyToDataType_m(pixel_layout),
                                      testobj );

  oyRectangle_SetGeo( roi, 0.5,0.5,0.5,0.5 );
  oyRectangle_SetGeo( a_roi, 0.5,0.5,0.5,0.5 );
  buf_16out2x2[9] = buf_16out2x2[10] = buf_16out2x2[11] = 65535;
  error = oyImage_FillArray( output, roi, 0,
                             &a,
                             a_roi, 0 );

  uint16_t ** rows_u16 = (uint16_t**)oyArray2d_GetData(a);
  uint16_t * output_u16 = rows_u16[0];

  if(!error &&
     rows_u16[0] != buf_16out2x2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyImage_FillArray() keep allocation" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyImage_FillArray() keep alloc 0x%lo 0x%lo",
    (unsigned long)rows_u16[0], (unsigned long)buf_16out2x2 );
  }

  if(!error &&
     output_u16[9] == 65535 && output_u16[10] == 65535 && output_u16[11] == 65535)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyImage_FillArray() place array data" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyImage_FillArray() place array data" );
  }

  if(verbose) fprintf(zout, "output: %d,%d,%d %d,%d,%d\n        %d,%d,%d %d,%d,%d\n",
                  buf_16out2x2[0], buf_16out2x2[1], buf_16out2x2[2],
                  buf_16out2x2[3], buf_16out2x2[4], buf_16out2x2[5],
                  buf_16out2x2[6], buf_16out2x2[7], buf_16out2x2[8],
                  buf_16out2x2[9], buf_16out2x2[10], buf_16out2x2[11] );

  /* set lower right pixel */
  output_u16[0] = output_u16[1] = output_u16[2] = 2;

  oyRectangle_SetGeo( a_roi, 0.0,0.0,0.5,0.5 );
  error = oyImage_ReadArray( output, roi,
                             a, a_roi );


  rows_u16 = (uint16_t**)oyArray2d_GetData(a);
  if(!error &&
     rows_u16[0] != &buf_16out2x2[9] &&
     buf_16out2x2[9]==2&& buf_16out2x2[10]==2&& buf_16out2x2[11]==2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyImage_ReadArray( array_roi )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyImage_ReadArray( array_roi )" );
  }

  /* set lower right pixel */
  output_u16[0] = output_u16[1] = output_u16[2] = 3;
  error = oyImage_ReadArray( output, roi,
                             a, 0 );


  rows_u16 = (uint16_t**)oyArray2d_GetData(a);
  if(!error &&
     rows_u16[0] != &buf_16out2x2[9] &&
     buf_16out2x2[9]==3&& buf_16out2x2[10]==3&& buf_16out2x2[11]==3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyImage_ReadArray()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyImage_ReadArray()" );
  }

  if(verbose) fprintf(zout, "output: %d,%d,%d %d,%d,%d\n        %d,%d,%d %d,%d,%d\n",
                  buf_16out2x2[0], buf_16out2x2[1], buf_16out2x2[2],
                  buf_16out2x2[3], buf_16out2x2[4], buf_16out2x2[5],
                  buf_16out2x2[6], buf_16out2x2[7], buf_16out2x2[8],
                  buf_16out2x2[9], buf_16out2x2[10], buf_16out2x2[11] );

  /* move the focus to the lower left */
  oyRectangle_s_ array_roi_pix = {oyOBJECT_RECTANGLE_S,0,0,0, 0,1,3,1};
  oyArray2d_SetFocus( a, (oyRectangle_s*)&array_roi_pix );


  rows_u16 = (uint16_t**)oyArray2d_GetData(a);
  if(!error &&
     rows_u16[0] != output_u16)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyArray2d_SetFocus() change array" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyArray2d_SetFocus() change 0x%lo 0x%lo",
    (unsigned long)rows_u16[0], (unsigned long)output_u16 );
  }


  output_u16 = (uint16_t*)rows_u16[0];

  /* set lower right pixel */
  output_u16[0] = output_u16[1] = output_u16[2] = 4;
  error = oyImage_ReadArray( output, roi,
                             a, 0 );
  rows_u16 = (uint16_t**)oyArray2d_GetData(a);
  if(!error &&
     rows_u16[0] != &buf_16out2x2[9] &&
     buf_16out2x2[9]==4&& buf_16out2x2[10]==4&& buf_16out2x2[11]==4)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyArray2d_SetFocus()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyArray2d_SetFocus()" );
  }

  oyArray2d_Release( &a );

  oyRectangle_SetGeo( a_roi, 0.5,0.5,0.5,0.5 );
  error = oyImage_FillArray( output, roi, 0,
                             &a, a_roi, 0 );

  rows_u16 = (uint16_t**)oyArray2d_GetData(a);
  if(!error &&
     rows_u16[0] == &buf_16out2x2[0])
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyImage_FillArray() assigment" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyImage_FillArray() assigment 0x%lo 0x%lo",
    (unsigned long)rows_u16[0], (unsigned long)&buf_16out2x2[9] );
  }

  oyArray2d_Release( &a );
  oyImage_Release( &input );
  oyImage_Release( &output );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

oyjlTESTRESULT_e testRectangles()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP
  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyProfile_s * p_web = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  int error = 0;
  uint16_t buf_16_3x3[27] = {
  20000,20000,20000, 10000,10000,10000, 5000,5000,5000,
  0,0,0,             65535,65535,65535, 32768,32768,32768,
  1,1,1,             100,100,100,       1000,1000,1000
  };
  oyDATATYPE_e buf_type = oyUINT16;
  oyImage_s *image;
  oyRectangle_s * sample_rectangle = oyRectangle_New(testobj),
                * pixel_rectangle = oyRectangle_New(testobj),
                * roi = oyRectangle_New(testobj);

  fprintf(stdout, "\n" );

  image =oyImage_Create( 3,3, 
                         buf_16_3x3,
                         oyChannels_m(oyProfile_GetChannelsCount(p_web)) |
                          oyDataType_m(buf_type),
                         p_web,
                         testobj );
  error = (image == NULL);
  oyProfile_Release( &p_web );


  oyImage_RoiToSamples( image, NULL, &sample_rectangle );
  double x,y,w,h;
  oyRectangle_GetGeo( sample_rectangle, &x,&y,&w,&h );

  if(!error &&
     x==0 && y == 0 && w == 9 && h == 3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyImage_RoiToSamples( NULL )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyImage_RoiToSamples( NULL )" );
  }


  oyImage_SamplesToRoi( image, NULL, &roi );
  oyRectangle_GetGeo( roi, &x,&y,&w,&h );

  if(!error &&
     x==0 && y == 0 && w == 1.0 && h == 1.0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyImage_SamplesToRoi( NULL )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyImage_SamplesToRoi( NULL )" );
  }


  oyRectangle_SetGeo( roi, 1.0/3.0, 1.0/3.0, 1.0/3.0, 2.0/3.0 );
  oyImage_RoiToSamples( image, roi, &sample_rectangle );
  oyRectangle_GetGeo( sample_rectangle, &x,&y,&w,&h );

  if(!error &&
     x==3 && y == 1 && w == 3 && h == 2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyImage_RoiToSamples( roi )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyImage_RoiToSamples( roi )" );
  }


  oyRectangle_SetGeo( sample_rectangle, 6,2, 3,1 );
  oyImage_SamplesToRoi( image, sample_rectangle, &roi );
  oyRectangle_GetGeo( roi, &x,&y,&w,&h );

  if(!error &&
     x==2.0/3.0 && y == 2.0/3.0 && w == 1.0/3.0 && h == 1.0/3.0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyImage_SamplesToRoi( sample_rectangle )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyImage_SamplesToRoi( sample_rectangle )" );
  }


  oyRectangle_SetGeo( sample_rectangle, 3, 1, 3, 2 );
  oyImage_SamplesToPixels( image, sample_rectangle, pixel_rectangle );
  oyRectangle_GetGeo( pixel_rectangle, &x,&y,&w,&h );

  if(!error &&
     x==1 && y == 1 && w == 1 && h == 2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyImage_SamplesToPixels( )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyImage_SamplesToPixels( )" );
  }

  oyRectangle_SetGeo( pixel_rectangle, 1, 1, 2, 2 );
  oyImage_PixelsToSamples( image, pixel_rectangle, sample_rectangle );
  oyRectangle_GetGeo( sample_rectangle, &x,&y,&w,&h );

  if(!error &&
     x==3 && y == 1 && w == 6 && h == 2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyImage_PixelsToSamples( )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyImage_PixelsToSamples( )" );
  }


  oyImage_Release( &image );
  oyRectangle_Release( &sample_rectangle );
  oyRectangle_Release( &pixel_rectangle );
  oyRectangle_Release( &roi );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

int myNodeSignal( oyObserver_s *observer, oySIGNAL_e signal_type, oyStruct_s *signal_data )
{
  int count = oyOption_GetValueInt( (oyOption_s*) signal_data, -1 );
  if(signal_data && signal_data->type_ == oyOBJECT_OPTION_S)
  {
    ++count;
    oyOption_SetFromInt( (oyOption_s*) signal_data, count, 0, 0 );
  }

  if(verbose || oy_debug_signals)
  {
    const char * desc = oyStruct_GetText(signal_data, oyNAME_NAME, 0);
    fprintf( zout, "%s - %s\n", oySignalToString(signal_type),
                                oyNoEmptyString_m_(desc) );
  }
  return 0;
}

#include "../examples/image_display/oyranos_display_helpers.h"
oyjlTESTRESULT_e testDAG2()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP
  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyProfile_s * p_in = NULL, * p_out = NULL;
  int error = 0,
      i = 2,n = 10;
  const int src_width  = 4 * 1024,
      src_height =     512,
      dst_width  = 2 * 1024,
      dst_height =     256,
      channels = 4;
  int x,y;
  uint16_t * buf_16in  = (uint16_t*) calloc(sizeof(uint16_t), src_width * src_height * channels);
  uint16_t * buf_16out = (uint16_t*) calloc(sizeof(uint16_t), dst_width * dst_height * channels);
  oyDATATYPE_e buf_type_in = oyUINT16,
               data_type_request = oyUINT16;
  oyImage_s *input = NULL, *output = NULL;
  oyConversion_s * cc = NULL;
  oyFilterNode_s * icc = NULL, * rectangles = NULL, * node = NULL, * input_node = NULL;
  oyFilterSocket_s * socket = NULL;
  oyOption_s * option = NULL;

  fprintf(stdout, "\n" );

  for(y = 0; y < src_height; ++y)
    for(x = 0; x < src_width; ++x)
    {
      buf_16in[y*src_width*channels + x*channels + 0] = (y / (double)(src_height-1)) * 65535;
      buf_16in[y*src_width*channels + x*channels + 1] = (1.0 - (y / (double)(src_height-1))) * 65535;
      buf_16in[y*src_width*channels + x*channels + 2] = y;
      buf_16in[y*src_width*channels + x*channels + 3] = x;
    }

  p_in = oyProfile_FromStd( oyEDITING_RGB, icc_profile_flags, testobj );
  p_out = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  input =oyImage_Create( src_width,src_height, 
                         buf_16in,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)+1) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         testobj );
  output=oyImage_Create( dst_width,dst_height, 
                         buf_16out,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)+1) |
                          oyDataType_m(data_type_request),
                         p_out,
                         testobj );

  cc = oyConversion_FromImageForDisplay( input, output,
                                         &icc, oyOPTIONATTRIBUTE_ADVANCED,
                                         oyUINT16, NULL, testobj );
  if(verbose)
    oyObjectTreePrint( 0x01 | 0x02 | 0x04 | 0x08, "display graph" );

  node = oyFilterNode_Copy( icc, 0 );
  input_node = oyFilterNode_GetPlugNode( node, 0 );

  /* insert a "rectangles" filter to handle multiple monitors */
  rectangles = oyFilterNode_NewWith( "//" OY_TYPE_STD "/rectangles", 0, 0 );
  /* insert "rectangles" between "display" and its input_node */
  fprintf( zout, "%s -> %s\n", oyFilterNode_GetRegistration( input_node ), oyFilterNode_GetRegistration( node ) );
  oyFilterNode_Disconnect( node, 0 );
  error = oyFilterNode_Connect( input_node, "//" OY_TYPE_STD "/data",
                                rectangles, "//" OY_TYPE_STD "/data",0 );
  error = oyFilterNode_Connect( rectangles, "//" OY_TYPE_STD "/data",
                                node, "//" OY_TYPE_STD "/data",0 );
  oyFilterNode_Release( &node );
  oyFilterNode_Release( &input_node );
  if(verbose)
    oyObjectTreePrint( 0x01 | 0x02 | 0x04 | 0x08, "rectangles connected" );

  if( !error )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "insert node" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "insert node" );
  }


  oyProfile_Release( &p_in );
  oyProfile_Release( &p_out );
  oyImage_Release( &input );
  oyImage_Release( &output );
  oyFilterNode_Release( &icc );
  oyConversion_Release ( &cc );


  p_in = oyProfile_FromStd( oyEDITING_RGB, icc_profile_flags, testobj );
  p_out = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  input =oyImage_Create( src_width,src_height, 
                         buf_16in,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)+1) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         testobj );
  output=oyImage_Create( dst_width,dst_height, 
                         buf_16out,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)+1) |
                          oyDataType_m(data_type_request),
                         p_out,
                         testobj );

  cc = oyConversion_FromImageForDisplay( input, output,
                                         &icc, oyOPTIONATTRIBUTE_ADVANCED,
                                         oyUINT16, NULL, testobj );
  const char * hash = oyFilterNode_GetText( icc, oyNAME_NAME );

  if( hash )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "hashed node" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "hashed node" );
  }

  oyProfile_Release( &p_in );
  oyProfile_Release( &p_out );
  oyImage_Release( &input );
  oyImage_Release( &output );
  oyFilterNode_Release( &icc );
  oyConversion_Release ( &cc );
  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "objects hash" )


  p_in = oyProfile_FromStd( oyEDITING_RGB, icc_profile_flags, testobj );
  p_out = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  input =oyImage_Create( src_width,src_height, 
                         buf_16in,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)+1) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         testobj );
  output=oyImage_Create( dst_width,dst_height, 
                         buf_16out,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)+1) |
                          oyDataType_m(data_type_request),
                         p_out,
                         testobj );

  cc = oyConversion_FromImageForDisplay( input, output,
                                         &icc, oyOPTIONATTRIBUTE_ADVANCED,
                                         oyUINT16, NULL, testobj );
  oyProfile_Release( &p_in );
  oyProfile_Release( &p_out );
  oyImage_Release( &input );
  oyImage_Release( &output );
  oyFilterNode_Release( &icc );

  input_node = oyConversion_GetNode( cc, OY_INPUT );
  node = oyConversion_GetNode( cc, OY_OUTPUT );
  option = oyOption_FromRegistration( OY_STD "/node/count", testobj );
  error = oyStruct_ObserverAdd( (oyStruct_s*)node, (oyStruct_s*)node,
                                (oyStruct_s*)option, myNodeSignal );

  //{ char * text = oyBT(-1); oyObjectTreePrint( 0x01 | 0x02 | 0x04 | 0x08, text ? text : __func__ ); oyFree_m_( text ) }
#if 0
  /* graph broadcast */
  socket = oyFilterNode_GetSocket( input_node, 0 );
  oyFilterSocket_SignalToGraph( socket, oyCONNECTOR_EVENT_DATA_CHANGED );
#else
  /* let oyFilterNode_Run() emit a signal on filter call and observe the ticket */
  oyConversion_RunPixels( cc, NULL );
#endif
  //{ char * text = oyBT(-1); oyObjectTreePrint( 0x01 | 0x02 | 0x04 | 0x08, text ? text : __func__ ); oyFree_m_( text ) }

  n = oyOption_GetValueInt( option, -1 );
  if( n > 0 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyConversion_s input is connected to output %d", n );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyConversion_s input is connected to output %d", n );
  }
  oyFilterNode_Release( &node );
  oyFilterNode_Release( &input_node );
  oyFilterSocket_Release( &socket );
  oyOption_Release( &option );

  oyConversion_Release ( &cc );
  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "objects run" )


  p_in = oyProfile_FromStd( oyEDITING_RGB, icc_profile_flags, testobj );
  p_out = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  input =oyImage_Create( src_width,src_height, 
                         buf_16in,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)+1) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         testobj );
  output=oyImage_Create( dst_width,dst_height, 
                         buf_16out,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)+1) |
                          oyDataType_m(data_type_request),
                         p_out,
                         testobj );

  cc = oyConversion_FromImageForDisplay( input, output,
                                         &icc, oyOPTIONATTRIBUTE_ADVANCED,
                                         oyUINT16, NULL, testobj );
  oyProfile_Release( &p_in );
  oyProfile_Release( &p_out );
  oyImage_Release( &input );
  oyImage_Release( &output );

  socket = oyFilterNode_GetSocket( icc, 0 );
  oyFilterSocket_Release( &socket );
  oyFilterNode_Release( &icc );

  input_node = oyConversion_GetNode( cc, OY_INPUT );
  socket = oyFilterNode_GetSocket( input_node, 0 );
  oyFilterSocket_Release( &socket );
  oyFilterNode_Release( &input_node );
  oyConversion_Release ( &cc );

  oyFree_m_(buf_16in);
  oyFree_m_(buf_16out);
  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "objects release" )


  oyImage_s * image = NULL;
  error = oyImage_FromFile( OY_SOURCEDIR "not_existing.png", 0, &image, 0 );
  if( !image )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyImage_FromFile( \"not_existing.png\" )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyImage_FromFile( \"not_existing.png\" )" );
  }
  oyImage_Release( &image );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "objects noimg" )


  error = oyImage_FromFile( OY_SOURCEDIR "/extras/icons/oyranos.png", 0, &image, 0 );
  if( image )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyImage_FromFile( \"oyranos.png\" )  = %dx%d", oyImage_GetWidth(image ), oyImage_GetHeight( image ) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyImage_FromFile()" );
  }
  error = oyImage_WritePPM( image, OY_SOURCEDIR "/extras/icons/oyranos.pam", "plain copy of oyranos.png" );
  if( error == 0 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyImage_WritePPM( \"oyranos.pam\"" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, error,
    "oyImage_WritePPM( \"oyranos.pam\"" );
  }
  oyImage_Release( &image );

  system( "pngtopnm " OY_SOURCEDIR "/extras/icons/oyranos.png > " OY_SOURCEDIR "/extras/icons/oyranos.ppm" );
  error = oyImage_FromFile( OY_SOURCEDIR "/extras/icons/oyranos.ppm", 0, &image, 0 );
  if( image )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyImage_FromFile( \"oyranos.ppm\" )  = %dx%d", oyImage_GetWidth(image ), oyImage_GetHeight( image ) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyImage_FromFile()" );
  }
  oyImage_Release( &image );

  system( "pnmtojpeg " OY_SOURCEDIR "/extras/icons/oyranos.ppm > " OY_SOURCEDIR "/extras/icons/oyranos.jpg" );
  error = oyImage_FromFile( OY_SOURCEDIR "/extras/icons/oyranos.jpg", 0, &image, 0 );
  if( image )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyImage_FromFile( \"oyranos.jpg\" )  = %dx%d", oyImage_GetWidth(image ), oyImage_GetHeight( image ) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyImage_FromFile()" );
  }
  oyImage_Release( &image );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

oyjlTESTRESULT_e testScreenPixel()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP
  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyProfile_s * p_rgb = oyProfile_FromStd( oyEDITING_RGB, icc_profile_flags, testobj );
  oyProfile_s * p_web = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  oyProfile_s * p_in = NULL, * p_out = NULL;
  int error = 0,
      i = 2,n = 10;
  const int src_width  = 4 * 1024,
      src_height =     512,
      dst_width  = 2 * 1024,
      dst_height =     256,
      channels = 4;
  int x,y;
  uint16_t * buf_16in  = (uint16_t*) calloc(sizeof(uint16_t), src_width * src_height * channels);
  uint16_t * buf_16out = (uint16_t*) calloc(sizeof(uint16_t), dst_width * dst_height * channels);
  oyDATATYPE_e buf_type_in = oyUINT16,
               data_type_request = oyUINT16;
  oyImage_s *input = NULL, *output = NULL;

  fprintf(stdout, "\n" );

  double clck = oyClock();
  for(y = 0; y < src_height; ++y)
    for(x = 0; x < src_width; ++x)
    {
      buf_16in[y*src_width*channels + x*channels + 0] = (y / (double)(src_height-1)) * 65535;
      buf_16in[y*src_width*channels + x*channels + 1] = (1.0 - (y / (double)(src_height-1))) * 65535;
      buf_16in[y*src_width*channels + x*channels + 2] = y;
      buf_16in[y*src_width*channels + x*channels + 3] = x;
    }

  p_in = p_web;
  p_out = p_rgb;
  input =oyImage_Create( src_width,src_height, 
                         buf_16in,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)+1) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         testobj );
  output=oyImage_Create( dst_width,dst_height, 
                         buf_16out,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)+1) |
                          oyDataType_m(data_type_request),
                         p_out,
                         testobj );
  clck = oyClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling( -1, 1,clck/(double)CLOCKS_PER_SEC,"in+out-images",
      "Preparation finished") );

  if(getenv("OY_DEBUG_WRITE"))
  {
    const char * fn = "test2-oyDrawScreenImage-src.ppm";
    fprintf( zout, "wrote PPM               %s\n", fn );
    oyImage_WritePPM( input, fn, "test2::screen source image" );
  }

  oyFilterPlug_s * plug = NULL;
  oyPixelAccess_s * pixel_access = NULL;
  oyConversion_s * cc = NULL;
  oyFilterNode_s * icc = NULL;
  memset( buf_16out, 0, sizeof(*buf_16out) );
  clck = oyClock();
  cc = oyConversion_FromImageForDisplay( input, output,
                                         &icc, oyOPTIONATTRIBUTE_ADVANCED,
                                         oyUINT16, NULL, testobj );
  clck = oyClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling( -1, 1,clck/(double)CLOCKS_PER_SEC,"context",
      "Preparation finished") );
  oyFilterNode_Release( &icc );

  oyFilterNode_s * out = oyConversion_GetNode( cc, OY_OUTPUT );
  if(cc && out)
    plug = oyFilterNode_GetPlug( out, 0 );
  else
    error = 1;
  oyFilterNode_Release( &out );
  pixel_access = oyPixelAccess_Create( 0,0, plug,
                                           oyPIXEL_ACCESS_IMAGE, testobj );
  oyFilterPlug_Release( &plug );

  oyRectangle_s_ display_rectangle_ = {oyOBJECT_RECTANGLE_S,0,0,0, 0,0,dst_width,dst_height};
  oyRectangle_s_ old_display_rectangle_ = {oyOBJECT_RECTANGLE_S,0,0,0, 0,0,0,0};
  oyRectangle_s * display_rectangle = (oyRectangle_s *) &display_rectangle_;
  oyRectangle_s * old_display_rectangle = (oyRectangle_s *) &old_display_rectangle_;
  int dirty = 1;
  clck = oyClock();
  dirty = oyDrawScreenImage( cc, pixel_access, display_rectangle,
                                old_display_rectangle,
                                NULL, "oy-test",
                                data_type_request,
                                NULL, NULL, dirty,
                                output );
  clck = oyClock() - clck;

  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling( -1, 1,clck/(double)CLOCKS_PER_SEC,"draw",
      "First draw finished") );

  if(getenv("OY_DEBUG_WRITE"))
  {
    const char * fn = "test2-oyDrawScreenImage-00.ppm";
    fprintf( zout, "wrote PPM               %s\n", fn );
    oyImage_WritePPM( input, fn, "test2::screen 00 image" );
  }
  clck = oyClock();
  for(i = 1; i <= n; ++i)
  {
    int err = 0;
    oy_debug_write_id = 100*i;
    // clear the output buffer to make changes visible
    memset( buf_16out, 0, sizeof(*buf_16out) );
    display_rectangle_.x = i*256;
    dirty = oyDrawScreenImage( cc, pixel_access, display_rectangle,
                                old_display_rectangle,
                                NULL, "oy-test",
                                data_type_request,
                                NULL, NULL, dirty,
                                output );
    if(getenv("OY_DEBUG_WRITE"))
    {
      char fn[256], num[256];
      sprintf( num, "%04d image[%d] display: %s", oy_debug_write_id,
               oyStruct_GetId((oyStruct_s*)output),
               oyRectangle_Show(display_rectangle) );
      sprintf( fn, "test2-oyDrawScreenImage-%04d.ppm", oy_debug_write_id );
      fprintf( zout, "wrote PPM               %s\n", fn );
      oyImage_WritePPM( output, fn, num );
    }

    y = 128;
    for(x = 0; x < dst_width; ++x)
    {
      if( buf_16out[y*dst_width*channels + x*channels + 3] &&
          buf_16out[y*dst_width*channels + x*channels + 3] != x )
      {
        err = buf_16out[y*dst_width*channels + x*channels + 3];
        break;
      }
    }
    
    if( err == 0 )
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyDrawScreenImage   fills correct positions" );
    } else
    { PRINT_SUB( oyjlTESTRESULT_XFAIL,
      "oyDrawScreenImage   fills correct positions  %d/%d", x,err );
    }


    if(oy_debug)
    fprintf( zout, "\nDONE [%d] ##################################### %s\n\n", i, oyRectangle_Show(display_rectangle) );
  }
  clck = oyClock() - clck;

  if( !error &&
      dirty <= 0
    )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, n,clck/(double)CLOCKS_PER_SEC,"draws",
    "oyDrawScreenImage" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyDrawScreenImage" );
  }



  oyPixelAccess_Release( &pixel_access );
  oyConversion_Release ( &cc );
  oyProfile_Release( &p_rgb );
  oyProfile_Release( &p_web );
  oyImage_Release( &input );
  oyImage_Release( &output );
  oyFree_m_(buf_16in);
  oyFree_m_(buf_16out);

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_XFAIL, 1, 0, NULL )

  return result;
}

oyjlTESTRESULT_e testFilterNodeCMM( oyjlTESTRESULT_e result_,
                                  const char * reg_pattern )
{
  oyjlTESTRESULT_e result = result_;
  OBJECT_COUNT_SETUP
  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyProfile_s * p_lab = oyProfile_FromFile( "compatibleWithAdobeRGB1998.icc", icc_profile_flags, testobj );
  oyProfile_s * p_web = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  oyProfile_s /** p_cmyk = oyProfile_FromStd( oyEDITING_CMYK, NULL ),*/
              * p_in, * p_out;
  uint16_t buf_16in2x2[12] = {
  20000,20000,20000, 10000,10000,10000,
  0,0,0,             65535,65535,65535
  };
  uint16_t buf_16out2x2[12];
  oyDATATYPE_e buf_type_in = oyUINT16,
               buf_type_out = oyUINT16;
  int i;


  p_in = p_web;
  p_out = p_lab;

  oyOptions_s * options = NULL;
  oyOptions_SetFromString( &options, "////rendering_intent", "3", OY_CREATE_NEW );
  oyOptions_SetFromString( &options, "////context", reg_pattern, OY_CREATE_NEW );

  oyConversion_s * cc = oyConversion_CreateBasicPixelsFromBuffers(
                              p_in, buf_16in2x2, oyDataType_m(buf_type_in),
                              p_out, buf_16out2x2, oyDataType_m(buf_type_out),
                                                    options, 4 );

  oyFilterGraph_s * cc_graph = oyConversion_GetGraph( cc );
  oyFilterNode_s * icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
  oyFilterGraph_Release( &cc_graph );
  oyOptions_s * node_opts = oyFilterNode_GetOptions( icc, oyOPTIONATTRIBUTE_ADVANCED );
  int count = oyOptions_Count( node_opts );
  oyOption_s * o = oyOptions_Find( node_opts, "rendering_intent", oyNAME_PATTERN );
  oyOptions_Release( &node_opts );
  if(o)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyOptions_Find( node_opts )%s/%d", oyOption_GetText(o, oyNAME_NICK), count );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_Find( node_opts )  %s(%d)", oyOption_GetText(o, oyNAME_NICK), count );
  }

  oyBlob_s * blob = oyFilterNode_ToBlob( icc, testobj );
  oyProfile_s * p = oyProfile_FromMem( oyBlob_GetSize( blob ),
                                       oyBlob_GetPointer( blob ), 0,testobj );
  oyBlob_Release( &blob );
  char sig[] = "Info";
  oyProfileTag_s * tag = oyProfile_GetTagById( p, oyValueTagSig((icTagSignature)*(uint32_t*)sig) );
  if(!tag)
  {
    memcpy(sig, "Hash", 4);
    tag = oyProfile_GetTagById( p, oyValueTagSig((icTagSignature)*(uint32_t*)sig) );
  }
  int32_t tag_size = 0,
          texts_n = 0;
  char ** texts = oyProfileTag_GetText( tag, &texts_n, NULL, NULL,
                                        &tag_size, oyAllocateFunc_ );
  if(texts_n == 1)
  {
    int n = 0;
    char ** ts = oyStringSplit( texts[0], '\n', &n, oyAllocateFunc_ );
    oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
    texts_n = n;
    texts = ts;
  }

  for(i = 0; i < texts_n; ++i)
    if(strstr(texts[i], "rendering_intent") != NULL &&
       strstr(texts[i], "rendering_intent_proof") == NULL)
    {
      fprintf( zout, "found: %s\n", texts[i] );

      char * t = NULL;

      if(strcmp(sig,"Info") == 0)
      {
        const char * r = texts[i];
        oyStringAddPrintf( &t, 0,0, "<org><freedesktop><openicc><behaviour>%s</behaviour></openicc></freedesktop></org>",
                           r );
        oyOptions_s * opts = oyOptions_FromText( t, 0, 0 );
        oyFree_m_(t);

        oyOption_s * o_ = oyOptions_Find( opts, "rendering_intent", oyNAME_PATTERN );

        if(o_ == NULL)
        { PRINT_SUB( oyjlTESTRESULT_SYSERROR,
          "Could not obtain \"%s\"", "rendering_intent" );
          return result;
        }

        t = oyStringCopy( strrchr( oyOption_GetText( o_, oyNAME_NICK ), OY_SLASH_C ) + 1, oyAllocateFunc_ );

        oyOption_Release( &o_ );
        oyOptions_Release( &opts );

      } else
      if(strcmp(sig, "Hash") == 0)
      {

        if(strrchr(texts[i], OY_SLASH_C) != NULL)
          t = oyStringCopy( strrchr( texts[i], OY_SLASH_C ) + 1, oyAllocateFunc_ );
        else
          t = oyStringCopy( texts[i], oyAllocateFunc_ );

        int pos = -1, wt = 0;
        while(t[++pos])
        {
          if(t[pos] == ' ')
            ++wt;
          t[pos] = t[pos+wt];
        }

      }

      const char * ot = oyOption_GetText(o, oyNAME_NICK);
      if(ot)
        ot = strrchr( ot, OY_SLASH_C ) + 1;

      if(t && ot)
      {
        if(strcmp(t,ot) == 0)
        { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
          "DL's\"%s\" equals node %s %s", sig, t, ot );
        } else
        { PRINT_SUB( oyjlTESTRESULT_FAIL,
          "DL's\"%s\" equals node %s %s", sig, t, ot );
        }
      }
    }

  oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
  oyProfileTag_Release( &tag );
  oyProfile_Release( &p );
  oyOption_Release( &o );

  node_opts = oyFilterNode_GetOptions( icc, 0 );
  o = oyOptions_Find( node_opts, "rendering_intent", oyNAME_PATTERN );
  if(o)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyOptions_Find( node_opts ) after DL %s", oyOption_GetText(o, oyNAME_NICK) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_Find( node_opts ) after DL %s", oyOption_GetText(o, oyNAME_NICK) );
  }
  oyOption_Release( &o );

  count = oyOptions_Count( node_opts );
  int n = 0;
  for(int i = 0; i < count; ++i)
  {
    o = oyOptions_Get( node_opts, i );
    const char * reg = oyOption_GetRegistration( o );
    if(strstr(reg, "rendering_intent") != NULL &&
       strstr(reg, "rendering_intent_proof") == NULL)
    {
      fprintf( zout, "found: %d[%d] %s\n", i, count, reg );
      ++n;
    }
    oyOption_Release( &o );
  }
  if(n == 1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyOptions_Find( node_opts ) unique rendering_intent" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_Find( node_opts ) unique rendering_intent" );
  }

  o = oyOptions_Find( node_opts, "rendering_bpc", oyNAME_PATTERN );
  if(o)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyOptions_Find( node_opts ) other default %s", oyOption_GetText(o, oyNAME_NICK) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_Find( node_opts ) other default %s", oyOption_GetText(o, oyNAME_NICK) );
  }
  oyOption_Release( &o );

  oyOptions_Release( &options );
  oyOptions_Release( &node_opts );
  oyProfile_Release( &p_lab );
  oyProfile_Release( &p_web );
  oyFilterNode_Release( &icc );
  oyConversion_Release( &cc );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_XFAIL, 1, 0, NULL )

  return result;
}

oyjlTESTRESULT_e testFilterNode()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  char ** list = oyGetCMMs( oyCMM_CONTEXT, oyNAME_NAME, 0, malloc );
  int i = 0;

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 0, 1, "oyGetCMMs" )

  while(list && list[i])
  {
    char * reg = oyCMMNameToRegistration( list[i], oyCMM_CONTEXT, oyNAME_NAME, 0, malloc );
    char * reg_pattern = oyCMMRegistrationToName( reg, oyCMM_CONTEXT, oyNAME_PATTERN, 0, malloc );
    fprintf(zout, "Testing ICC CMM[%d]: \"%s\"\n", i, list[i] );

    oyjlTESTRESULT_e result_ = testFilterNodeCMM( result, reg_pattern );
    if(result_ < result)
      result = result_;

    fprintf(stdout, "\n" );
    ++i;
  }


  return result;
}

int oyHasLcms()
{
  char ** list = oyGetCMMs( oyCMM_CONTEXT, oyNAME_NICK, 0, malloc );
  int count = 0, has_lcms = 0;

  while(list && list[count])
  {
    if(strcmp(list[count],"lcms") == 0)
      has_lcms = 1;
    free( list[count] );
    ++count;
  }
  if(list) free(list);
  fprintf(stdout, "found CMMs: %d has_lcms=%d\n", count, has_lcms );

  return has_lcms;
}

oyjlTESTRESULT_e testConversion()
{
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyProfile_s * p_in = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  oyProfile_s * p_out = oyProfile_FromFile( "compatibleWithAdobeRGB1998.icc", icc_profile_flags, testobj );
  uint16_t buf_16in2x2[12] = {
  20000,20000,20000, 10000,10000,10000,
  0,0,0,             65535,65535,65535
  };
  uint16_t buf_16out2x2[12];
  oyDATATYPE_e buf_type_in = oyUINT16,
               buf_type_out = oyUINT16;
  oyImage_s *input, *output;

#ifndef COMPILE_STATIC
  int has_lcms = oyHasLcms();
#endif

  input =oyImage_Create( 2,2, 
                         buf_16in2x2,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         testobj );
  output=oyImage_Create( 2,2, 
                         buf_16out2x2,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                          oyDataType_m(buf_type_out),
                         p_out,
                         testobj );
  oyProfile_Release( &p_in );
  oyProfile_Release( &p_out );

  oyOptions_s * options = NULL;

  oyOptions_SetFromString( &options, "////cached", "1", OY_CREATE_NEW );
  oyConversion_s * cc = oyConversion_CreateBasicPixels( input,output, options, testobj );
  oyFilterGraph_s * cc_graph = oyConversion_GetGraph( cc );
  oyFilterNode_s * icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
  char * config_cmm = oyGetPersistentString( OY_DEFAULT_CMM_CONTEXT, 0, oySCOPE_USER_SYS, 0 );
  const char * reg = oyFilterNode_GetRegistration( icc );
  oyOptions_s * node_opts = oyFilterNode_GetOptions( icc, oyOPTIONATTRIBUTE_ADVANCED );
  oyOption_s * ct = oyOptions_Find( node_opts, "////context", oyNAME_PATTERN );
  oyOptions_Release( &node_opts );
  fprintf( zout, "context global: %s\ncontext node: %s\n", config_cmm, reg?strrchr( reg, '/')+1:"----" );
  reg = oyOption_GetValueString( ct, 0 );
  if(reg && (reg = strrchr( reg, '/')) != NULL) ++reg;
  fprintf( zout, "context option: %s\n", oyNoEmptyString_m_(reg) );

  if(ct)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyOptions_Find( node_opts, \"////context\" )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_Find( node_opts, \"////context\" )" );
  }
  oyOption_Release( &ct );

  oyBlob_s * blob = oyFilterNode_ToBlob( icc, testobj );
  if(blob)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyConversion_CreateBasicPixels( \"cached\"=\"1\" )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyConversion_CreateBasicPixels( \"cached\"=\"1\" )" );
  }
  oyBlob_Release( &blob );

  node_opts = oyFilterNode_GetOptions( icc, 0 );
  ct = oyOptions_Find( node_opts, "////context", oyNAME_PATTERN );
  oyOptions_Release( &node_opts );
  if(ct)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyOptions_Find( node_opts, \"////context\" )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_Find( node_opts, \"////context\" )" );
  }
  oyOption_Release( &ct );

  oyOptions_Release( &options );
  oyConversion_Release( &cc );
  oyFilterGraph_Release( &cc_graph );
  oyFilterNode_Release( &icc );

  oyOptions_SetFromString( &options, "////context", "lcm2", OY_CREATE_NEW );
  cc = oyConversion_CreateBasicPixels( input,output, options, testobj );
  cc_graph = oyConversion_GetGraph( cc );
  icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
  reg = oyFilterNode_GetRegistration( icc );
  const char * node_reg = reg;

  node_opts = oyFilterNode_GetOptions( icc, 0 );
  ct = oyOptions_Find( node_opts, "////context", oyNAME_PATTERN );
  reg = oyOption_GetValueString( ct, 0 );
  oyOption_Release( &ct );
  oyOptions_Release( &node_opts );
  int match = 0;
  if(reg && strrchr( reg, '/')) reg = strrchr( reg, '/') + 1;
  if(node_reg && strrchr( node_reg, '/')) node_reg = strrchr( node_reg, '/') + 1;
  if( reg &&
      oyFilterRegistrationMatch( node_reg,
                                 reg, oyOBJECT_NONE ))
    match = 1;
  if(match)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "node = option : %s ~ %s", node_reg, oyNoEmptyString_m_(reg) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "node = option : %s ~ %s", node_reg, oyNoEmptyString_m_(reg) );
  }
  
  reg = oyFilterNode_GetRegistration( icc );
  if(reg && strstr(reg, "lcm2"))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyConversion_CreateBasicPixels( \"context\"=\"lcm2\" )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyConversion_CreateBasicPixels( \"context\"=\"lcm2\" ) %s", oyNoEmptyString_m_(reg) );
  }

  blob = oyFilterNode_ToBlob( icc, testobj );
  if(blob)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyFilterNode_ToBlob( \"lcm2\" )           %d", (int)oyBlob_GetSize(blob) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyFilterNode_ToBlob( \"lcm2\" )           %d", (int)oyBlob_GetSize(blob) );
  }

  oyBlob_Release( &blob );
  oyOptions_Release( &options );
  oyConversion_Release( &cc );
  oyFilterGraph_Release( &cc_graph );
  oyFilterNode_Release( &icc );

  oyOptions_SetFromString( &options, "////renderer", "lcms", OY_CREATE_NEW );
  oyOptions_SetFromString( &options, "////context",  "lcm2", OY_CREATE_NEW );
  cc = oyConversion_CreateBasicPixels( input,output, options, testobj );
  cc_graph = oyConversion_GetGraph( cc );
  icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
  reg = oyFilterNode_GetRendererRegistration( icc );

  if(reg && strstr(reg, "lcms"))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyConversion_CreateBasicPixels( \"renderer\"=\"lcms\" )" );
#ifndef COMPILE_STATIC
  } else if(has_lcms)
  { PRINT_SUB( has_lcms ? oyjlTESTRESULT_FAIL : oyjlTESTRESULT_XFAIL,
    "oyConversion_CreateBasicPixels( \"renderer\"=\"lcms\" ) %s", oyNoEmptyString_m_(reg) );
    fprintf( zout, "\tnode reg = %s\n", oyFilterNode_GetRegistration( icc ));
#endif
  }

  oyOptions_Release( &options );

  options = oyFilterNode_GetOptions( icc, oyOPTIONATTRIBUTE_ADVANCED );
  oyOptions_SetFromString( &options, "////renderer", "lcm2", OY_CREATE_NEW );
  oyOptions_SetFromString( &options, "////context", "lcms", OY_CREATE_NEW );
  blob = oyFilterNode_ToBlob( icc, NULL );

  reg = oyFilterNode_GetRegistration( icc );
  if(reg && strstr(reg, "lcms"))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyFilterNode_SetContext_( \"context\"=\"lcms\" ) %d", (int)oyBlob_GetSize(blob) );
#ifndef COMPILE_STATIC
  } else if(has_lcms)
  { PRINT_SUB( has_lcms ? oyjlTESTRESULT_FAIL : oyjlTESTRESULT_XFAIL,
    "oyFilterNode_SetContext_( \"context\"=\"lcms\" ) %s  ", oyNoEmptyString_m_(reg) );
#endif
  }
  oyBlob_Release( &blob );

  reg = oyFilterNode_GetRendererRegistration( icc );
  if(reg && strstr(reg, "lcm2"))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyFilterNode_SetContext_( \"renderer\"=\"lcm2\" )" );
#ifndef COMPILE_STATIC
  } else
  { PRINT_SUB( has_lcms ? oyjlTESTRESULT_FAIL : oyjlTESTRESULT_XFAIL,
    "oyFilterNode_SetContext_( \"renderer\"=\"lcm2\" ) %s", oyNoEmptyString_m_(reg) );
#endif
  }

  oyOptions_Release( &options );
  oyConversion_Release( &cc );
  oyFilterGraph_Release( &cc_graph );
  oyFilterNode_Release( &icc );


  config_cmm = oyGetPersistentString( OY_DEFAULT_CMM_CONTEXT, 0, oySCOPE_USER_SYS, 0 );
  int error = oySetPersistentString( OY_DEFAULT_CMM_CONTEXT, oySCOPE_USER,
                                     "///icc_color.notX", "non existent CMM" );
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oySetPersistentString() error: %d", error )
  char * test_config_cmm = oyGetPersistentString( OY_DEFAULT_CMM_CONTEXT, 0, oySCOPE_USER_SYS, 0 );
  if(test_config_cmm && strcmp(test_config_cmm,"///icc_color.notX") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "set intermediate global context = %s", oyNoEmptyString_m_(test_config_cmm) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "set intermediate global context = %s", oyNoEmptyString_m_(test_config_cmm) );
  }
  cc = oyConversion_CreateBasicPixels( input,output, options, testobj );
  cc_graph = oyConversion_GetGraph( cc );
  icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
  reg = oyFilterNode_GetRegistration( icc );
  blob = oyFilterNode_ToBlob( icc, NULL );
  if(blob)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "f: %s", oyNoEmptyString_m_(reg) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "f: %s", oyNoEmptyString_m_(reg) );
  }
  error  = oyConversion_RunPixels( cc, NULL );
  if(error == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "fallback rendering with blob %d", (int)oyBlob_GetSize(blob) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "fallback rendering with blob %d", (int)oyBlob_GetSize(blob) );
  }

  oyBlob_Release( &blob );
  oyConversion_Release( &cc );
  oyFilterGraph_Release( &cc_graph );
  oyFilterNode_Release( &icc );

  int i;
  for(i = 0; i < 12; ++i) buf_16out2x2[i] = 0;
  cc = oyConversion_CreateBasicPixels( input,output, options, testobj );
  cc_graph = oyConversion_GetGraph( cc );
  icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
  node_opts = oyFilterNode_GetOptions( icc, oyOPTIONATTRIBUTE_ADVANCED );
  const char * context = oyOptions_FindString( node_opts, "////context", NULL );
  error  = oyConversion_RunPixels( cc, NULL );
  reg = oyFilterNode_GetRegistration( icc );
  if(!error)
    if(buf_16out2x2[0] == 0 && buf_16out2x2[3] == 0)
      error = 1;
  if(error == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "fallback rendering %s %s", oyNoEmptyString_m_(context), reg?strrchr(reg,'/')+1:"----" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "fallback rendering %s %s", oyNoEmptyString_m_(context), reg?strrchr(reg,'/')+1:"----" );
  }

  ct = oyOptions_Find( node_opts, "////context", oyNAME_PATTERN );
  reg = oyOption_GetValueString( ct, 0 );
  if(reg && (reg = strrchr( reg, '/')) != NULL) ++reg;
  if(ct)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyOptions_Find( node_opts, \"////context\" ) %s", oyNoEmptyString_m_(reg) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_Find( node_opts, \"////context\" )    " );
  }

  if((oyOption_GetFlags( ct ) & oyOPTIONATTRIBUTE_EDIT) == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "\"////context\" is not touched oyOPTIONATTRIBUTE_EDIT" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "\"////context\" is not touched oyOPTIONATTRIBUTE_EDIT" );
  }
  oyOption_Release( &ct );
  oyConversion_Release( &cc );
  oyFilterGraph_Release( &cc_graph );
  oyFilterNode_Release( &icc );
  oyOptions_Release( &node_opts );

  oyOptions_SetFromString( &options, "////context", test_config_cmm, OY_CREATE_NEW );
  cc = oyConversion_CreateBasicPixels( input,output, options, 0 );
  cc_graph = oyConversion_GetGraph( cc );
  icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
  node_opts = oyFilterNode_GetOptions( icc, oyOPTIONATTRIBUTE_ADVANCED );
  ct = oyOptions_Find( node_opts, "////context", oyNAME_PATTERN );
  if(!icc || (oyOption_GetFlags( ct ) & oyOPTIONATTRIBUTE_EDIT) != 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "\"////context\" is touched oyOPTIONATTRIBUTE_EDIT %s", oyNoEmptyString_m_(reg) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "\"////context\" is touched oyOPTIONATTRIBUTE_EDIT %s", oyNoEmptyString_m_(reg) );
  }
  oyOption_Release( &ct );
  oyOptions_Release( &options );

  for(i = 0; i < 12; ++i) buf_16out2x2[i] = 0;
  int cc_error = oyConversion_RunPixels( cc, NULL );
  context = oyOptions_FindString( node_opts, "////context", NULL );
  error = 0;
  if(buf_16out2x2[0] != 0 || buf_16out2x2[3] != 0)
    error = 1;
  reg = oyFilterNode_GetRegistration( icc );
  if(reg && (reg = strrchr( reg, '/')) != NULL) ++reg;
  if(error == 0 &&
     cc_error > 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "explicite no fallback rendering %s %s", oyNoEmptyString_m_(context), oyNoEmptyString_m_(reg) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "explicite no fallback rendering %s %s", oyNoEmptyString_m_(context), oyNoEmptyString_m_(reg) );
  }

  ct = oyOptions_Find( node_opts, "////context", oyNAME_PATTERN );
  reg = oyOption_GetValueString( ct, 0 );
  if(reg && (reg = strrchr( reg, '/')) != NULL) ++reg;
  if(!reg || (oyOption_GetFlags( ct ) & oyOPTIONATTRIBUTE_EDIT) != 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "\"////context\" is touched oyOPTIONATTRIBUTE_EDIT %s", oyNoEmptyString_m_(reg) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "\"////context\" is touched oyOPTIONATTRIBUTE_EDIT %s", oyNoEmptyString_m_(reg) );
  }
  oyOption_Release( &ct );

  oyConversion_Release( &cc );
  oyFilterGraph_Release( &cc_graph );
  oyFilterNode_Release( &icc );
  oyOptions_Release( &node_opts );

  oyOptions_SetFromString( &options, "////renderer", test_config_cmm, OY_CREATE_NEW );
  cc = oyConversion_CreateBasicPixels( input,output, options, 0 );
  cc_graph = oyConversion_GetGraph( cc );
  icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
  node_opts = oyFilterNode_GetOptions( icc, oyOPTIONATTRIBUTE_ADVANCED );
  ct = oyOptions_Find( node_opts, "////renderer", oyNAME_PATTERN );
  if(!icc || (oyOption_GetFlags( ct ) & oyOPTIONATTRIBUTE_EDIT) != 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "\"////renderer\" is touched oyOPTIONATTRIBUTE_EDIT %s", oyNoEmptyString_m_(reg) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "\"////renderer\" is touched oyOPTIONATTRIBUTE_EDIT %s", oyNoEmptyString_m_(reg) );
  }
  oyOption_Release( &ct );
  oyOptions_Release( &options );

  for(i = 0; i < 12; ++i) buf_16out2x2[i] = 0;
  cc_error = oyConversion_RunPixels( cc, NULL );
  const char * renderer = oyOptions_FindString( node_opts, "////renderer", NULL );
  error = 0;
  if(buf_16out2x2[0] != 0 || buf_16out2x2[3] != 0)
    error = 1;
  reg = oyFilterNode_GetRegistration( icc );
  if(reg && (reg = strrchr( reg, '/')) != NULL) ++reg;
  if(error == 0 &&
     cc_error > 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "explicite no fallback rendering %s %s", oyNoEmptyString_m_(renderer), oyNoEmptyString_m_(reg) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "explicite no fallback rendering %s %s", oyNoEmptyString_m_(renderer), oyNoEmptyString_m_(reg) );
  }

  ct = oyOptions_Find( node_opts, "////renderer", oyNAME_PATTERN );
  reg = oyOption_GetValueString( ct, 0 );
  if(reg && (reg = strrchr( reg, '/')) != NULL) ++reg;
  if(!reg || (oyOption_GetFlags( ct ) & oyOPTIONATTRIBUTE_EDIT) != 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "\"////renderer\" is touched oyOPTIONATTRIBUTE_EDIT %s", oyNoEmptyString_m_(reg) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "\"////renderer\" is touched oyOPTIONATTRIBUTE_EDIT %s", oyNoEmptyString_m_(reg) );
  }
  oyOption_Release( &ct );

  oyConversion_Release( &cc );
  oyFilterGraph_Release( &cc_graph );
  oyFilterNode_Release( &icc );
  oyOptions_Release( &node_opts );

  if(config_cmm)
  {
    error = oySetPersistentString( OY_DEFAULT_CMM_CONTEXT, oySCOPE_USER,
                                   config_cmm && config_cmm[0] ? config_cmm : NULL, NULL );
    if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oySetPersistentString() error: %d", error )
  }
  else
  {
    error = oySetPersistentString( OY_DEFAULT_CMM_CONTEXT, oySCOPE_USER,
                                   NULL, NULL );
    if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oySetPersistentString() error: %d", error )
  }
  oyFree_m_( config_cmm );
  oyFree_m_( test_config_cmm );

  oyImage_Release( &input );
  oyImage_Release( &output );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_XFAIL, 1, 0, NULL )

  return result;
}

oyjlTESTRESULT_e testCMMlists()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  char ** list = oyGetCMMs( oyCMM_CONTEXT, oyNAME_NAME, 0, malloc );
  int i = 0;

  if(verbose)
  while(list && list[i])
  {
    fprintf(zout, "  %d: \"%s\" (%s)\n", i, list[i], oyCMMNameToRegistration( list[i], oyCMM_CONTEXT, oyNAME_NAME, 0, malloc ) );
    ++i;
  }

  if(list)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyGetCMMs(oyCMM_CONTEXT, oyNAME_NAME,) fine %d", i );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyGetCMMs(oyCMM_CONTEXT, oyNAME_NAME,) failed   " );
  }

  list = oyGetCMMs( oyCMM_CONTEXT, oyNAME_REGISTRATION, 0, malloc );
  i = 0;

  if(verbose)
  while(list && list[i])
  {
    fprintf(zout, "  %d: \"%s\" (%s)\n", i, list[i], oyCMMRegistrationToName(list[i], oyCMM_CONTEXT, oyNAME_NAME, 0, malloc) );
    ++i;
  }

  if(list)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyGetCMMs(oyCMM_CONTEXT, oyNAME_REGISTRATION,)   %d", i );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyGetCMMs(oyCMM_CONTEXT, oyNAME_REGISTRATION,) failed " );
  }

  list = oyGetCMMs( oyCMM_CONTEXT, oyNAME_NICK, 0, malloc );
  i = 0;

  if(verbose)
  while(list && list[i])
  {
    fprintf(zout, "  %d: \"%s\" (%s)\n", i, list[i], oyCMMRegistrationToName(list[i], oyCMM_CONTEXT, oyNAME_PATTERN, 0, malloc) );
    ++i;
  }

  if(list)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyGetCMMs(oyCMM_CONTEXT, oyNAME_NICK,) fine %d", i );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyGetCMMs(oyCMM_CONTEXT, oyNAME_NICK,) failed" );
  }

  char * default_cmm = oyGetCMMPattern( oyCMM_CONTEXT, oySOURCE_DATA, malloc );
  if(default_cmm && default_cmm[0])
  { fprintf( zout,
    "oyGetCMMPattern( oySOURCE_DATA ) == %s         \n", default_cmm );
  } else
  { fprintf( zout,
    "oyGetCMMPattern( oySOURCE_DATA ) not set              \n" );
  }

  if(default_cmm && default_cmm[0])
  {
    int current = -1;
    oyOptionChoicesGet2( oyWIDGET_CMM_CONTEXT, 0, oyNAME_NAME, &i,
                         (const char ***)&list, &current );

    if(current != -1)
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      " oyOptionChoicesGet2( 0, current == %s [%d])", list[current],
                                                              current );
    } else
    { PRINT_SUB( oyjlTESTRESULT_XFAIL,
      "oyOptionChoicesGet2( current == ???? ) missed" );
    }

    oyOptionChoicesFree( oyWIDGET_CMM_CONTEXT, (const char ***)&list, i );
  }


  if(default_cmm) free(default_cmm);


  default_cmm = oyGetCMMPattern( oyCMM_CONTEXT, oySOURCE_FILTER, malloc );

  if(default_cmm && default_cmm[0])
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyGetCMMPattern( oySOURCE_FILTER )==%s", default_cmm );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyGetCMMPattern( oySOURCE_FILTER ) failed             " );
  }
  if(default_cmm) free(default_cmm);


  default_cmm = oyGetCMMPattern( oyCMM_CONTEXT, 0, malloc );

  if(default_cmm && default_cmm[0])
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyGetCMMPattern( ) == %s", default_cmm );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyGetCMMPattern( ) failed" );
  }
  if(default_cmm) free(default_cmm);

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_XFAIL, 1, 0, NULL )

  return result;
}

#define u16EQUALx(a,b,x) (a-x <= b && b <= a+x)
double u16Equal(uint16_t a, uint16_t b)
{
  uint16_t delta = 0;
  while(!u16EQUALx(a,b,delta))
    ++delta;
  return (double)delta/65535.0;
}

#define u16TripleEQUALx(a,b,c,x) (a-x <= b && b <= a+x && a-x <= c && c <= a+x)
double u16TripleEqual(uint16_t a, uint16_t b, uint16_t c)
{
  uint16_t delta = 0;
  while(!u16TripleEQUALx(a,b,c,delta))
    ++delta;
  return (double)delta/65535.0;
}

/* i >= 0 -> write DL to file */
oyBlob_s * getDL( oyConversion_s * cc, const char * reg_nick, int i )
{
  oyFilterGraph_s * cc_graph = oyConversion_GetGraph( cc );
  oyFilterNode_s * icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
  if(verbose)
  {
    oyOptions_s * node_opts = oyFilterNode_GetTags( icc );
    oyOptions_SetFromString( &node_opts, "////verbose", "true", OY_CREATE_NEW );
    oyOptions_Release( &node_opts );
  }
  oyBlob_s * blob = oyFilterNode_ToBlob( icc, NULL );
  char * name = NULL;
  oyStringAddPrintf( &name, 0,0,
                         "test-dl-%s-%d-id-%d.icc", reg_nick, i, oyObject_GetId(icc->oy_) );
  if(i >= 0)
  {
    oyWriteMemToFile_( name, oyBlob_GetPointer( blob ), oyBlob_GetSize( blob) );
    fprintf( zout, "wrote Device Link for inspection to %s\n", name );
  }
  oyFilterGraph_Release( &cc_graph );
  oyFilterNode_Release( &icc );

  return blob;
}

int      oyConversionColors          ( oyProfile_s       * p_in,
                                       oyPointer           buf_in,
                                       oyDATATYPE_e        buf_in_type,
                                       oyProfile_s       * p_out,
                                       oyPointer           buf_out,
                                       oyDATATYPE_e        buf_out_type,
                                       oyOptions_s       * options,
                                       int                 count,
                                       int                 correct_flags )
{
  int error = 0;
  oyConversion_s * cc = oyConversion_CreateBasicPixelsFromBuffers (
                              p_in,  buf_in,  oyDataType_m(buf_in_type),
                              p_out, buf_out, oyDataType_m(buf_out_type),
                              options, count );
  if(!cc) error = 1;
  else
  {
    if(correct_flags)
      error = oyConversion_Correct( cc, "//" OY_TYPE_STD "/icc_color", correct_flags == 1 ? oyOPTIONATTRIBUTE_BASIC : correct_flags, NULL);
    if(!error)
      error = oyConversion_RunPixels( cc, NULL );
  }
  oyConversion_Release( &cc );
  return error;
}

oyConversion_s * oy_xyz_srgb = NULL;
int oyXYZ2sRGB ( double * rgb )
{
  int error = 0;
  int icc_profile_flags = 0, i;
  static double rgb_[3];

  if(!oy_xyz_srgb)
  {
    oyProfile_s * pXYZ = oyProfile_FromStd( oyASSUMED_XYZ, icc_profile_flags, 0 ),
                * sRGB = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, 0 );
    oyOptions_s * options = NULL;
    oyOptions_SetFromString( &options, OY_DEFAULT_RENDERING_INTENT, "1", OY_CREATE_NEW );
    oy_xyz_srgb = oyConversion_CreateBasicPixelsFromBuffers (
                                       pXYZ, rgb_, oyDataType_m(oyDOUBLE),
                                       sRGB, rgb_, oyDataType_m(oyDOUBLE),
                                       options, 1 );
    oyProfile_Release( &sRGB );
    oyProfile_Release( &pXYZ );
    oyOptions_Release( &options );
  }
  error = !oy_xyz_srgb;

  for(i = 0; i < 3; ++i) rgb_[i] = rgb[i];
  oyConversion_RunPixels( oy_xyz_srgb, 0 );
  for(i = 0; i < 3; ++i) rgb[i] = rgb_[i];
  rgb[3] = 1.0;

  return error;
}
int  oyIsInRange                     ( const double        i,
                                       const double        reference,
                                       const double        delta )
{ 
  if(fabs(reference - i) < delta) return 1;
  return 0;
}
int  oyColorIsProofingMarker         ( const double        i[] )
{ 
  if( oyIsInRange(i[0], 0.5, 0.01) &&
      oyIsInRange(i[1], 0.5, 0.01) &&
      oyIsInRange(i[2], 0.5, 0.01) )
    return 1;
  return 0;
}
int      oyLabGamutCheck             ( double            * lab,
                                       int                 count,
                                       oyProfiles_s      * proofing,
                                       int               * is_in_gamut,
                                       double            * lab_tested )
{
  int error = -1;
  int icc_profile_flags = 0, i;
  oyProfile_s * pLab = oyProfile_FromStd( oyASSUMED_LAB, icc_profile_flags, 0 );
  double * tmp = lab_tested ? NULL : (double*)calloc( 3*count, sizeof(double) );
  oyOptions_s * module_options = NULL;
  oyProfiles_s * profs = oyProfiles_Copy( proofing, NULL );
  if(!lab_tested) lab_tested = tmp;
  if(!lab_tested) return error;
  if(proofing)
  {
    oyOptions_MoveInStruct( &module_options,
                                       OY_PROFILES_SIMULATION,
                                       (oyStruct_s**) &profs,
                                       OY_CREATE_NEW );
  }
  oyOptions_SetFromString( &module_options, OY_DEFAULT_PROOF_SOFT, "1", OY_CREATE_NEW );
  oyOptions_SetFromString( &module_options, OY_DEFAULT_RENDERING_INTENT, "1", OY_CREATE_NEW );
  oyOptions_SetFromString( &module_options, OY_DEFAULT_RENDERING_INTENT_PROOF, "1", OY_CREATE_NEW );
  oyOptions_SetFromString( &module_options, OY_DEFAULT_RENDERING_BPC, "0", OY_CREATE_NEW );
  oyOptions_SetFromString( &module_options, OY_DEFAULT_RENDERING_GAMUT_WARNING, "1", OY_CREATE_NEW );
  oyConversion_s * cc = oyConversion_CreateBasicPixelsFromBuffers (
                                       pLab, lab, oyDataType_m(oyDOUBLE),
                                       pLab, lab_tested, oyDataType_m(oyDOUBLE),
                                       module_options, count );
  error = !cc;
  oyConversion_RunPixels( cc, 0 );

  for(i = 0; i < count; ++i)
    if(oyColorIsProofingMarker(&lab[i*3]))
      is_in_gamut[i] = 1;
    else
      is_in_gamut[i] = !oyColorIsProofingMarker(&lab_tested[i*3]);

  oyProfile_Release( &pLab );
  oyOptions_Release( &module_options );
  oyConversion_Release( &cc );
  if(tmp)
    free(tmp);

  return error;
}


oyjlTESTRESULT_e testICCsCheck()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  char ** list = oyGetCMMs( oyCMM_CONTEXT, oyNAME_NAME, 0, malloc );
  int i = 0, j;

  while(list && list[i])
  {
    char * reg = oyCMMNameToRegistration( list[i], oyCMM_CONTEXT, oyNAME_NAME, 0, malloc );
    char * reg_pattern = oyCMMRegistrationToName( reg, oyCMM_CONTEXT, oyNAME_PATTERN, 0, malloc );
    fprintf(zout, "\nTesting ICC CMM[%d]: \"%s\"\n", i, list[i] );

    if(reg)
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "oyGetCMMs(oyCMM_CONTEXT) fine %s", reg_pattern );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "oyGetCMMs(oyCMM_CONTEXT) failed   " );
    }

    oyDATATYPE_e buf_type_in = oyDOUBLE,
                 buf_type_out = oyDOUBLE;
    oyOptions_s * options = NULL;
    oyProfile_s * p_in, * p_out;
    oyProfiles_s * proofing;
    uint32_t icc_profile_flags = 0;
    oyConversion_s * cc = NULL, * cc_xyz_rgb = NULL; 
    int show_details = verbose;
    int error = 0;
    double delta = 0.001;



    double buf_f64in2x2[12], 
           buf_f64out2x2[12] = {0,0,0, 0,0,0, 0,0,0, 0,0,0};

#define USE_XYZsRGB_FUNC 1
#if !defined(USE_XYZsRGB_FUNC)
    buf_f64in2x2[0] = 0.5; buf_f64in2x2[1]  = buf_f64in2x2[2]  = 0.5;
    buf_f64in2x2[3] = 0.2; buf_f64in2x2[4]  = buf_f64in2x2[5]  = 0.5;
    buf_f64in2x2[6] = 0.0; buf_f64in2x2[7]  = buf_f64in2x2[8]  = 0.5;
    buf_f64in2x2[9] = 1.0; buf_f64in2x2[10] = buf_f64in2x2[11] = 0.5;
    for(j = 0; j < 4; ++j)
    {
      oyIcc2CIELab( &buf_f64in2x2[j*3], &buf_f64in2x2[j*3], NULL );
      oyLab2XYZ(    &buf_f64in2x2[j*3], &buf_f64in2x2[j*3] );
    }

    p_in = oyProfile_FromStd( oyASSUMED_XYZ, icc_profile_flags, testobj );
    p_out = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );

    oyOptions_SetFromString( &options, OY_DEFAULT_CMM_CONTEXT, reg_pattern, OY_CREATE_NEW );
    cc_xyz_rgb = oyConversion_CreateBasicPixelsFromBuffers(
                              p_in, buf_f64in2x2, oyDataType_m(buf_type_in),
                              p_out, buf_f64out2x2, oyDataType_m(buf_type_out),
                                                    options, 4 );
    oyOptions_Release( &options );
    oyProfile_Release( &p_in );
    oyProfile_Release( &p_out );

    error = oyConversion_RunPixels( cc_xyz_rgb, 0 );
    //oyConversion_Release( &cc_xyz_rgb );
    if(!error &&
        fabs(1.0 - buf_f64out2x2[9]) < delta &&
        fabs(1.0 - buf_f64out2x2[10]) < delta &&
        fabs(1.0 - buf_f64out2x2[11]) < delta
      )
#else
    double lightness = 50.0; // percent Lab
    double Lab[3] = { lightness, 0.0, 0.0 }, rgb[4];
    oyLab2XYZ( Lab, rgb );
    if(verbose) fprintf(stderr, "Background Lab: %.2f %.2f %.2f XYZ: %.2f %.2f %.2f\n", Lab[0], Lab[1], Lab[2], rgb[0], rgb[1], rgb[2] );
    oyXYZ2sRGB( rgb );

    if(!error &&
        fabs(0.4664 - rgb[0]) < delta &&
        fabs(0.4664 - rgb[1]) < delta &&
        fabs(0.4664 - rgb[2]) < delta
      )
#endif
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "check XYZ -> Web" );
    } else
    { PRINT_SUB( oyjlTESTRESULT_XFAIL,
      "check XYZ -> Web                                    " );
      show_details = 1;
    }
    if(show_details)
    {
#if !defined(USE_XYZsRGB_FUNC)
      for(j = 0; j < 4; ++j)
      {
        fprintf( zout, "buf_f64in  %g %g %g -> ",
                 buf_f64in2x2[j*3+0], buf_f64in2x2[j*3+1], buf_f64in2x2[j*3+2]);
        fprintf( zout, "buf_f64_out %g %g %g\n",
                 buf_f64out2x2[j*3+0], buf_f64out2x2[j*3+1], buf_f64out2x2[j*3+2]);
      }
#else
      fprintf(stderr, "RGB: %.5f %.5f %.5f\n", rgb[0], rgb[1], rgb[2] );
#endif
    } show_details = verbose;



    p_in = oyProfile_FromStd( oyASSUMED_LAB, icc_profile_flags, testobj );
    p_out = oyProfile_FromStd( oyASSUMED_LAB, icc_profile_flags, testobj );

    buf_f64in2x2[0] = 0.5; buf_f64in2x2[1] = buf_f64in2x2[2] = 0.5;
    buf_f64in2x2[3] = 0.2; buf_f64in2x2[4] = buf_f64in2x2[5] = 0.5;
    buf_f64in2x2[6] = 0.0; buf_f64in2x2[7] = buf_f64in2x2[8] = 0.5;
    buf_f64in2x2[9] = 1.0; buf_f64in2x2[10] = buf_f64in2x2[11] = 0.5;
#if !defined(USE_XYZsRGB_FUNC)
    oyOptions_SetFromString( &options, OY_DEFAULT_CMM_CONTEXT, reg_pattern, OY_CREATE_NEW );
    cc = oyConversion_CreateBasicPixelsFromBuffers(
                              p_in, buf_f64in2x2, oyDataType_m(buf_type_in),
                              p_out, buf_f64out2x2, oyDataType_m(buf_type_out),
                                                    options, 4 );
    error = oyConversion_RunPixels( cc, 0 );
    if(!error &&
        fabs(buf_f64in2x2[0] - buf_f64out2x2[0]) < delta &&
        fabs(buf_f64in2x2[1] - buf_f64out2x2[1]) < delta &&
        fabs(buf_f64in2x2[2] - buf_f64out2x2[2]) < delta
      )
#else
    int inside[4] = {0,0,0,0};
    proofing = oyProfiles_New(NULL);
    error = oyLabGamutCheck( buf_f64in2x2, 4, proofing, inside, buf_f64out2x2 );
    if(!error &&
        ( inside[0] == 1 && inside[1] == 1 && inside[2] == 1 && inside[3] == 1 )
      )
#endif
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "check Lab -> Lab" );
    } else
    { PRINT_SUB( oyjlTESTRESULT_XFAIL,
      "check Lab -> Lab" );
      show_details = 1;
    }
    if(show_details)
    {
      for(j = 0; j < 4; ++j)
      {
        fprintf( zout, "buf_f64in  %g %g %g -> ",
                 buf_f64in2x2[j*3+0],  buf_f64in2x2[j*3+1],  buf_f64in2x2[j*3+2]);
        fprintf( zout, "buf_f64_out %g %g %g\n",
                 buf_f64out2x2[j*3+0], buf_f64out2x2[j*3+1], buf_f64out2x2[j*3+2]);
      }
    } show_details = verbose;

    oyProfiles_Release( &proofing );
    oyOptions_Release( &options );
    oyConversion_Release( &cc );
    oyProfile_Release( &p_in );
    oyProfile_Release( &p_out );


#if !defined(USE_XYZsRGB_FUNC)
    buf_f64in2x2[0] = 0.5; buf_f64in2x2[1] = buf_f64in2x2[2] = 0.5;
    buf_f64in2x2[3] = 0.2; buf_f64in2x2[4] = buf_f64in2x2[5] = 0.5;
    buf_f64in2x2[6] = 0.0; buf_f64in2x2[7] = buf_f64in2x2[8] = 0.5;
    buf_f64in2x2[9] = 1.0; buf_f64in2x2[10] = buf_f64in2x2[11] = 0.5;
    for(j = 0; j < 4; ++j)
    {
      oyIcc2CIELab( &buf_f64in2x2[j*3], &buf_f64in2x2[j*3], NULL );
      oyLab2XYZ(    &buf_f64in2x2[j*3], &buf_f64in2x2[j*3] );
    }
    error = oyConversion_RunPixels( cc_xyz_rgb, 0 );
    if(!error &&
        fabs(1.0 - buf_f64out2x2[9]) < delta &&
        fabs(1.0 - buf_f64out2x2[10]) < delta &&
        fabs(1.0 - buf_f64out2x2[11]) < delta
      )
#else
    double rgb4[4];
    Lab[0] = 0.67; Lab[1] = Lab[2] = 0.5;
    oyIcc2CIELab( Lab, Lab, NULL );
    oyLab2XYZ( Lab, rgb4 );
    if(verbose) fprintf(stderr, "Background Lab: %.2f %.2f %.2f XYZ: %.2f %.2f %.2f\n", Lab[0], Lab[1], Lab[2], rgb4[0], rgb4[1], rgb4[2] );
    oyXYZ2sRGB( rgb4 );

    if(!error &&
        fabs(0.6393 - rgb4[0]) < delta &&
        fabs(0.6393 - rgb4[1]) < delta &&
        fabs(0.6393 - rgb4[2]) < delta
      )
#endif
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "check XYZ -> Web" );
    } else
    { PRINT_SUB( oyjlTESTRESULT_XFAIL,
      "check XYZ -> Web" );
      show_details = 1;
    }
    if(show_details)
    {
#if !defined(USE_XYZsRGB_FUNC)
      for(j = 0; j < 4; ++j)
      {
        fprintf( zout, "buf_f64in  %g %g %g -> ",
                 buf_f64in2x2[j*3+0], buf_f64in2x2[j*3+1], buf_f64in2x2[j*3+2]);
        fprintf( zout, "buf_f64_out %g %g %g\n",
                 buf_f64out2x2[j*3+0], buf_f64out2x2[j*3+1], buf_f64out2x2[j*3+2]);
      }
#else
      fprintf(stderr, "RGB: %.5f %.5f %.5f\n", rgb4[0], rgb4[1], rgb4[2] );
#endif
    } show_details = verbose;
    oyConversion_Release( &cc_xyz_rgb );
    oyConversion_Release( &oy_xyz_srgb );


    icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", options, 0 );
    oyOptions_SetFromString( &options, OY_DEFAULT_CMM_CONTEXT, reg_pattern, OY_CREATE_NEW );
    p_in = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
    p_out = oyProfile_FromFile( "compatibleWithAdobeRGB1998.icc", icc_profile_flags, testobj );
    uint16_t buf_16in2x2[12] = {
    32767,32767,32767, 10000,10000,10000,
    0,0,0,             65535,65535,65535
    };
    uint16_t buf_16out2x2[12];
    oyImage_s *input, *output;
    buf_type_in = oyUINT16;
    buf_type_out = oyUINT16;

    //fprintf(stdout, "\n" );

    input =oyImage_Create( 2,2, 
                         buf_16in2x2,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         testobj );
    output=oyImage_Create( 2,2, 
                         buf_16out2x2,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                          oyDataType_m(buf_type_out),
                         p_out,
                         testobj );
    oyProfile_Release( &p_in );
    oyOptions_Release( &options );
    oyOptions_SetFromString( &options, OY_DEFAULT_CMM_CONTEXT, reg_pattern, OY_CREATE_NEW );
    oyOptions_SetFromString( &options, OY_DEFAULT_RENDERING_INTENT, "1", OY_CREATE_NEW );
    cc = oyConversion_CreateBasicPixels( input,output, options, testobj );
    oyFilterGraph_s * cc_graph = oyConversion_GetGraph( cc );
    oyFilterNode_s * icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
    const char * node_reg = oyFilterNode_GetRegistration( icc );
    char * reg_nick = oyCMMRegistrationToName( reg, oyCMM_CONTEXT, oyNAME_NICK, 0, malloc );

    if(node_reg && strstr(node_reg, reg_nick))
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "\"context\"=\"%s\"", oyNoEmptyString_m_(reg_nick) );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "\"context\"=\"%s\"", oyNoEmptyString_m_(node_reg) );
    }

    error = oyConversion_RunPixels( cc, NULL );
    double da = u16TripleEqual(buf_16out2x2[0], buf_16out2x2[1], buf_16out2x2[2]),
           db = u16TripleEqual(buf_16out2x2[3], buf_16out2x2[4], buf_16out2x2[5]),
           dc = u16TripleEqual(buf_16out2x2[6], buf_16out2x2[7], buf_16out2x2[8]),
           dd = u16TripleEqual(buf_16out2x2[9], buf_16out2x2[10], buf_16out2x2[11]);
    if(!error &&
       /* assuming that a proper working space gives equal results along the gray axis */
       da < delta && db < delta && dc < delta && dd < delta )
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "relative colorimetric intent, equal channels      %3.5f[%g]", OY_MAX(da,OY_MAX(db,OY_MAX(dc,dd)))*100.0, delta*100.0 );
    } else
    { PRINT_SUB( oyjlTESTRESULT_XFAIL,
      "relative colorimetric intent, equal channels      %3.5f[%g] %%", OY_MAX(da,OY_MAX(db,OY_MAX(dc,dd)))*100.0, delta*100.0 );
      show_details = 1;
    }
    if(show_details)
    {
      fprintf( zout, "%d %d %d   %d %d %d\n%d %d %d   %d %d %d\n",
               buf_16out2x2[0], buf_16out2x2[1], buf_16out2x2[2],
               buf_16out2x2[3], buf_16out2x2[4], buf_16out2x2[5],
               buf_16out2x2[6], buf_16out2x2[7], buf_16out2x2[8],
               buf_16out2x2[9], buf_16out2x2[10], buf_16out2x2[11]);
    } show_details = verbose;


    oyImage_Release( &output );
    oyConversion_Release( &cc );

    float buf_f32in2x2[12],
          buf_f32out2x2[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
    int j;
    for(j = 0; j < 3; ++j) buf_f32in2x2[j] = 0.5f;
    for(j = 3; j < 12; ++j) buf_f32in2x2[j] = buf_16in2x2[j]/65535.0f;
    buf_type_in = oyFLOAT;
    buf_type_out = oyFLOAT;
    p_in = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
    cc = oyConversion_CreateBasicPixelsFromBuffers(
                              p_in, buf_f32in2x2, oyDataType_m(buf_type_in),
                              p_out, buf_f32out2x2, oyDataType_m(buf_type_out),
                                                    options, 4 );
    oyProfile_Release( &p_in );
    error = oyConversion_RunPixels( cc, NULL );
    double equal = 0, max = 0;
    for(j = 0; j < 12; ++j)
      if((equal = u16Equal((int)(buf_f32out2x2[j]*65535.0f), buf_16out2x2[j])) > max)
        max = equal;
    /* Is the float conversion ~ equal to the integer math? */
    if(!error && (max <= delta))
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "relative colorimetric intent, integer equal float %3.5f[%g]", max*100.0, delta*100.0 );
    } else
    { PRINT_SUB( oyjlTESTRESULT_XFAIL,
      "relative colorimetric intent, integer equal float %3.5f[%g] %%", max*100.0, delta*100.0 );
      show_details = 1;
    }

    if(show_details)
    {
      fprintf( zout, "buf_f32in  %g %g %g   %g %g %g\n           %g %g %g   %g %g %g\n",
               buf_f32in2x2[0], buf_f32in2x2[1], buf_f32in2x2[2],
               buf_f32in2x2[3], buf_f32in2x2[4], buf_f32in2x2[5],
               buf_f32in2x2[6], buf_f32in2x2[7], buf_f32in2x2[8],
               buf_f32in2x2[9], buf_f32in2x2[10], buf_f32in2x2[11]);
      fprintf( zout, "buf_f32out %g %g %g   %g %g %g\n           %g %g %g   %g %g %g\n",
               buf_f32out2x2[0], buf_f32out2x2[1], buf_f32out2x2[2],
               buf_f32out2x2[3], buf_f32out2x2[4], buf_f32out2x2[5],
               buf_f32out2x2[6], buf_f32out2x2[7], buf_f32out2x2[8],
               buf_f32out2x2[9], buf_f32out2x2[10], buf_f32out2x2[11]);

      oyBlob_s * b = getDL( cc, reg_nick, i );
      oyBlob_Release( &b );

      fprintf( zout, "options where: %s\n", oyOptions_GetText( options, oyNAME_NICK ) );
    } show_details = verbose;

    buf_f32in2x2[0] = 0.0;
    buf_f32in2x2[1] = 0.0;
    buf_f32in2x2[2] = 1.0;
    error = oyConversion_RunPixels( cc, NULL );
    if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyConversion_RunPixels() error: %d", error )
    float blue[3] = { buf_f32out2x2[0], buf_f32out2x2[1], buf_f32out2x2[2] };
    oyOptions_Release( &options );
    oyConversion_Release( &cc );


    oyOptions_SetFromString( &options, OY_DEFAULT_CMM_CONTEXT, reg_pattern, OY_CREATE_NEW );
    oyOptions_SetFromString( &options, OY_DEFAULT_RENDERING_INTENT, "1", OY_CREATE_NEW );
    oyOptions_SetFromString( &options, OY_DEFAULT_PROOF_SOFT, "1", OY_CREATE_NEW );
    p_in = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
    cc = oyConversion_CreateBasicPixelsFromBuffers(
                              p_in, buf_f32in2x2, oyDataType_m(buf_type_in),
                              p_out, buf_f32out2x2, oyDataType_m(buf_type_out),
                                                    options, 4 );
    oyProfile_Release( &p_in );
    error = oyOptions_SetFromString( &options,
                                     "//" OY_TYPE_STD "/config/display_mode", "1",
                                     OY_CREATE_NEW );
    if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyOptions_SetFromString() error: %d", error )
    /* activate policy */
    error = oyConversion_Correct( cc, "//" OY_TYPE_STD "/icc_color", oyOPTIONATTRIBUTE_ADVANCED, options);
    if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyConversion_Correct() error: %d", error )
    error = oyConversion_RunPixels( cc, NULL );
    /* Is the float conversion ~ equal to the integer math? */
    if(!error && (blue[0] != buf_f32out2x2[0] ||
                  blue[1] != buf_f32out2x2[1] ||
                  blue[2] != buf_f32out2x2[2]    ))
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "relative colorimetric intent, simulation blue  %g %g %g -> %g %g %g", blue[0], blue[1], blue[2], buf_f32out2x2[0], buf_f32out2x2[1], buf_f32out2x2[2] );
    } else
    { PRINT_SUB( oyjlTESTRESULT_XFAIL,
      "relative colorimetric intent, simulation blue  %g %g", blue[0], buf_f32out2x2[0] );
      show_details = 1;
    }

    if(show_details)
    {
      fprintf( zout, "buf_f32in  %g %g %g\n",
               buf_f32in2x2[0], buf_f32in2x2[1], buf_f32in2x2[2]);
      fprintf( zout, "previous   %g %g %g\n",
               blue[0], blue[1], blue[2]);
      fprintf( zout, "simulation %g %g %g\n",
               buf_f32out2x2[0], buf_f32out2x2[1], buf_f32out2x2[2]);
    } show_details = verbose;
    oyOptions_Release( &options );
    oyConversion_Release( &cc );


    /* test multi profile transforms */
    oyOptions_SetFromString( &options, OY_DEFAULT_CMM_CONTEXT, reg_pattern, OY_CREATE_NEW );
    oyOptions_SetFromString( &options, OY_DEFAULT_RENDERING_INTENT, "0", OY_CREATE_NEW );
    oyOptions_SetFromString( &options, OY_DEFAULT_PROOF_SOFT, "1", OY_CREATE_NEW );
    oyProfile_s * p_cmyk = oyProfile_FromStd( oyEDITING_CMYK, icc_profile_flags, NULL );
    proofing = oyProfiles_New(NULL);
    oyProfiles_MoveIn( proofing, &p_cmyk, -1 );
    error = oyOptions_MoveInStruct ( &options,
                                     OY_PROFILES_SIMULATION,
                                     (oyStruct_s**) &proofing,
                                     OY_CREATE_NEW );
    cc = oyConversion_CreateFromImage (
                                input, options,
                                p_out, oyUINT16, 0, 0 );
    oyImage_Release( &input );
    oyBlob_s * blob = getDL( cc, reg_nick, show_details?i:-1 );
    if(!show_details)
      oyWriteMemToFile_( "test-getDL-non-verbose.icc", oyBlob_GetPointer( blob ), oyBlob_GetSize( blob) );

    oyProfile_s * dl;
    dl = oyProfile_FromMem( oyBlob_GetSize( blob ),
                            oyBlob_GetPointer( blob ), 0,0 );
    const char * fn;
    j = 0;
    if(verbose)
    {
      while((fn = oyProfile_GetFileName( dl, j )) != NULL)
        fprintf( zout, " -> \"%s\"[%d]", oyNoEmptyString_m_(fn), j++ );
      fprintf( zout, "\n" );
    } else
    {
      oyProfileTag_s * psid = oyProfile_GetTagById( dl, (icTagSignature)icSigProfileSequenceIdentifierTag );
      int texts_n = 0;
      char ** texts = oyProfileTag_GetText( psid, &texts_n, 0,0,0,0);
      if(texts_n && texts[0])
      {
        long l = 0;
        if(oyjlStringToLong( texts[0], &l, 0 ) <= 0)
          j = l;
      }
      oyjlStringListRelease( &texts, texts_n, oyDeAllocateFunc_ );
      if((texts_n -1) / 5 != j)
      { PRINT_SUB( oyjlTESTRESULT_XFAIL,
        "dl psid texts = %d (1+5*nProfiles) but announced psid profiles: %d", texts_n, j );
      }
      oyProfileTag_Release( &psid );
    }
    oyBlob_Release( &blob );
    oyProfile_Release( &dl );

    if(!error && j == 3)
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "device link contains 2 profiles + simulation == %d", j );
    } else
    { PRINT_SUB( oyjlTESTRESULT_XFAIL,
      "device link contains 2 profiles + simulation == %d", j );
    }

    oyOptions_s * node_opts = oyFilterNode_GetOptions( icc, oyOPTIONATTRIBUTE_ADVANCED );
    char * ui_text = NULL;
    char ** namespaces = NULL;
    oyFilterNode_GetUi( icc, oyNAME_JSON, &ui_text, &namespaces, malloc );
    const char * data = NULL;
    int text_type = oyjlDataFormat(ui_text);
    if(text_type != oyNAME_JSON)
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
      "text type of oyFilterNode_GetUi(%s) not JSON: 7|%d", reg_nick, text_type );
    } else
    {
      data = oyOptions_GetText( node_opts, (oyNAME_e) oyNAME_JSON );
      char * text = NULL;
      text = oyJsonFromModelAndUi( data, ui_text, malloc );
      if(oyjlDataFormat(text) == oyNAME_JSON)
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "oyJsonFromModelAndUi()                   len=%d", (int)strlen(text) );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyJsonFromModelAndUi()                  type=%d", oyjlDataFormat(text) );
      }
      oyFree_m_( text );
    }
    oyOptions_Release( &node_opts );
    oyFree_m_( ui_text );

    oyOptions_Release( &options );
    oyConversion_Release( &cc );

    oyProfile_Release( &p_in );
    oyProfile_Release( &p_out );


    ++i;

    oyFree_m_( reg_pattern );
    oyFree_m_( reg_nick );
    oyFree_m_( reg );
    oyFilterNode_Release( &icc );
    oyFilterGraph_Release( &cc_graph );
  }

  i = 0;
  if(list)
  {
    while(list[i])
      free(list[i++]);
    free(list);
  }

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_XFAIL, 1, 0, NULL )

  return result;
}

oyjlTESTRESULT_e testCCorrectFlags( )
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyProfile_s * p_lab = oyProfile_FromFile( "compatibleWithAdobeRGB1998.icc", icc_profile_flags, testobj );
  oyProfile_s * p_web = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, testobj );
  oyProfile_s /** p_cmyk = oyProfile_FromStd( oyEDITING_CMYK, NULL ),*/
              * p_in, * p_out;
  uint16_t buf_16in2x2[12] = {
  20000,20000,20000, 10000,10000,10000,
  0,0,0,             65535,65535,65535
  };
  uint16_t buf_16out2x2[12];
  oyDATATYPE_e buf_type_in = oyUINT16,
               buf_type_out = oyUINT16;
  int i;


  p_in = p_web;
  p_out = p_lab;

  oyOptions_s * options = NULL;
  oyOptions_SetFromString( &options, "////rendering_intent", "3", OY_CREATE_NEW );
  oyOptions_SetFromString( &options, "////context", "lcm2", OY_CREATE_NEW );

  oyConversion_s * cc = oyConversion_CreateBasicPixelsFromBuffers(
                              p_in, buf_16in2x2, oyDataType_m(buf_type_in),
                              p_out, buf_16out2x2, oyDataType_m(buf_type_out),
                                                    options, 4 );

  oyFilterGraph_s * cc_graph = oyConversion_GetGraph( cc );
  oyFilterNode_s * icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
  oyOptions_s * node_opts = oyFilterNode_GetOptions( icc, oyOPTIONATTRIBUTE_ADVANCED );
  int count = oyOptions_Count( node_opts );
  oyOption_s * o = oyOptions_Find( node_opts, "rendering_intent", oyNAME_PATTERN );
  if(o)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyOptions_Find( node_opts )  %s(%d)", oyOption_GetText(o, oyNAME_NICK), count );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyOptions_Find( node_opts )  %s(%d)", oyOption_GetText(o, oyNAME_NICK), count );
  }
  oyOption_Release( &o );

  int error = oyConversion_Correct( cc, "//" OY_TYPE_STD "/icc_color", oyOPTIONATTRIBUTE_ADVANCED, NULL);
  if(error) PRINT_SUB( oyjlTESTRESULT_XFAIL, "oyConversion_Correct() error: %d", error )

  int ri_count = 0, ri_touched = 0, bpc_touched = 0;
  for(i = 0 ; i < count; ++i)
  {
    oyOption_s * opt = oyOptions_Get( node_opts, i );
    int flags = oyOption_GetFlags( opt );
    const char * reg = oyOption_GetRegistration( opt );
    const char * val = oyOption_GetValueString( opt, 0 );
    char * v = oyGetPersistentString( reg, 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
    int tested_key = 0;
    if(reg && oyFilterRegistrationMatch( reg, "rendering_intent", oyOBJECT_NONE ))
    {
      ++ri_count;
      if(flags & oyOPTIONATTRIBUTE_EDIT)
        ri_touched = 1;
      tested_key = 1;
    }
    if( reg &&
        oyFilterRegistrationMatch( reg, "rendering_bpc", oyOBJECT_NONE ) &&
        flags & oyOPTIONATTRIBUTE_EDIT )
    {
      bpc_touched = 1;
      tested_key = 1;
    }
    if(verbose)
    fprintf( zout, "%s:%s %s%s %d\n", reg, val,
             flags & oyOPTIONATTRIBUTE_EDIT ? " touched":"",
             flags & oyOPTIONATTRIBUTE_AUTOMATIC ? " auto":"",
             flags );
    if(!tested_key && !(flags & oyOPTIONATTRIBUTE_EDIT) &&
        val && strstr(reg,"advanced") == NULL && ((v && strcmp(val,v)) || !v))
    {
      PRINT_SUB( oyjlTESTRESULT_FAIL, "oyConversion_Correct() DB missmatch: %s %s/%s", verbose?"":reg, val, v?v:"----" );
    }
    oyOption_Release( &opt );
    if(v) oyFree_m_(v);
  }
  if(ri_count == 1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "ri: exact count for manipulated option" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "ri: exact count for manipulated option" );
  }
  if(ri_touched == 1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "ri: oyOPTIONATTRIBUTE_EDIT flag found correctly" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "ri: oyOPTIONATTRIBUTE_EDIT flag found correctly " );
  }
  if(bpc_touched == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "bpc: oyOPTIONATTRIBUTE_EDIT flag missed correctly" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "bpc: oyOPTIONATTRIBUTE_EDIT flag missed correctly" );
  }

  oyOptions_Release( &options );
  oyOptions_Release( &node_opts );
  oyConversion_Release( &cc );
  oyFilterGraph_Release( &cc_graph );
  oyFilterNode_Release( &icc );
  oyProfile_Release( &p_lab );
  oyProfile_Release( &p_web );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

#include "oyranos_generic_internal.h"
oyHash_s *   oyTestCacheListGetEntry_ ( const char        * hash_text)
{
  if(!oy_test_cache_)
    oy_test_cache_ = oyStructList_Create( oyOBJECT_NONE, "oy_test_cache_", testobj );

  return oyCacheListGetEntry_(oy_test_cache_, 0, hash_text);
}

oyjlTESTRESULT_e testCache()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  char * text = oyAlphaPrint_( verbose );
  if(text)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(text),
    "Cache content:" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyAlphaPrint_( ) failed" );
  }
  if(verbose && text)
    fprintf( zout, "Cache:\n%s\n", text);

  const char * hash_texts[] = {
    "A", "B", "AB", "ABC", "abc", "ABCDEF",
    "org/freedesktop/openicc/foo/bar/long/item",
    "org/freedesktop/openicc/foo/bar/sjsjsjsjsjsjsjsjsjsjsjsjsjsjsjsjsjsjsj------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------",
    "///not/so/long", "//an/other/not/so/long"
  };

  oyTestCacheListClear_();

  int repeat = 20;
  int count = 1000;
  if(verbose)
  fprintf(zout, "%d:\n", count );
  double clck = oyClock();
  int i;
  for(int j = 0; j < repeat; ++j)
  for(i = 0; i < count; ++i)
  {
    char * hash_text = NULL;
    oyStringAddPrintf_( &hash_text, 0,0,//oyAllocateFunc_, oyDeAllocateFunc_,
                        "%s%d", hash_texts[6], i );
    oyHash_s * hash = oyTestCacheListGetEntry_( hash_text );
    oyHash_Release( &hash );
    oyFree_m_(hash_text);
  }
  clck = oyClock() - clck;

  if( i == count )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i*repeat,clck/(double)CLOCKS_PER_SEC,"entries",
    "oyCacheListGetEntry_(unique long entry) count: %d", count*repeat );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyCacheListGetEntry_(unique long entry)" );
  }

  oyTestCacheListClear_();
  count = 100;
  if(verbose)
  fprintf(zout, "%d:\n", count );
  clck = oyClock();
  for(int j = 0; j < repeat; ++j)
  for(i = 0; i < count; ++i)
  {
    char * hash_text = NULL;
    oyStringAddPrintf_( &hash_text, 0,0,//oyAllocateFunc_, oyDeAllocateFunc_,
                        "%s%d", hash_texts[6], i );
    oyHash_s * hash = oyTestCacheListGetEntry_( hash_text );
    oyHash_Release( &hash );
    oyFree_m_(hash_text);
  }
  clck = oyClock() - clck;

  if( i == count )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i*repeat,clck/(double)CLOCKS_PER_SEC,"entries",
    "oyCacheListGetEntry_(unique long entry) count: %d", count*repeat );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyCacheListGetEntry_(unique long entry)  " );
  }


  oyTestCacheListClear_();
  count = 1000;
  if(verbose)
  fprintf(zout, "%d:\n", count );
  clck = oyClock();
  for(int j = 0; j < repeat; ++j)
  for(i = 0; i < count; ++i)
  {
    char * hash_text = NULL;
    oyStringAddPrintf_( &hash_text, 0,0,//oyAllocateFunc_, oyDeAllocateFunc_,
                        "%s%d", hash_texts[7], i );
    oyHash_s * hash = oyTestCacheListGetEntry_( hash_text );
    oyHash_Release( &hash );
    oyFree_m_(hash_text);
  }
  clck = oyClock() - clck;

  if( i == count )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i*repeat,clck/(double)CLOCKS_PER_SEC,"entries",
    "oyCacheListGetEntry_(unique vlong entry) count: %d", count*repeat );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyCacheListGetEntry_(unique vlong entry)" );
  }

  oyTestCacheListClear_();
  count = 100;
  if(verbose)
  fprintf(zout, "%d:\n", count );
  clck = oyClock();
  for(int j = 0; j < repeat; ++j)
  for(i = 0; i < count; ++i)
  {
    char * hash_text = NULL;
    oyStringAddPrintf_( &hash_text, 0,0,//oyAllocateFunc_, oyDeAllocateFunc_,
                        "%s%d", hash_texts[7], i );
    oyHash_s * hash = oyTestCacheListGetEntry_( hash_text );
    oyHash_Release( &hash );
    oyFree_m_(hash_text);
  }
  clck = oyClock() - clck;

  if( i == count )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i*repeat,clck/(double)CLOCKS_PER_SEC,"entries",
    "oyCacheListGetEntry_(unique vlong entry) count: %d", count*repeat );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyCacheListGetEntry_(unique vlong entry)" );
  }


  oyTestCacheListClear_();
  count = 1000;
  if(verbose)
  fprintf(zout, "%d:\n", count );
  clck = oyClock();
  for(int j = 0; j < repeat; ++j)
  for(i = 0; i < count; ++i)
  {
    char * hash_text = NULL;
    oyStringAddPrintf_( &hash_text, 0,0,//oyAllocateFunc_, oyDeAllocateFunc_,
                        "%s%d", hash_texts[3], i );
    oyHash_s * hash = oyTestCacheListGetEntry_( hash_text );
    oyHash_Release( &hash );
    oyFree_m_(hash_text);
  }
  clck = oyClock() - clck;

  if( i == count )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i*repeat,clck/(double)CLOCKS_PER_SEC,"entries",
    "oyCacheListGetEntry_(unique short entry) count: %d", count*repeat );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyCacheListGetEntry_(unique short entry)" );
  }

  oyTestCacheListClear_();
  count = 100;
  if(verbose)
  fprintf(zout, "%d:\n", count );
  clck = oyClock();
  for(int j = 0; j < repeat; ++j)
  for(i = 0; i < count; ++i)
  {
    char * hash_text = NULL;
    oyStringAddPrintf_( &hash_text, 0,0,//oyAllocateFunc_, oyDeAllocateFunc_,
                        "%s%d", hash_texts[3], i );
    oyHash_s * hash = oyTestCacheListGetEntry_( hash_text );
    oyHash_Release( &hash );
    oyFree_m_(hash_text);
  }
  clck = oyClock() - clck;

  if( i == count )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i*repeat,clck/(double)CLOCKS_PER_SEC,"entries",
    "oyCacheListGetEntry_(unique short entry) count: %d", count*repeat );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyCacheListGetEntry_(unique short entry) " );
  }

  oyTestCacheListClear_();
  
  unsigned char md5_return[OY_HASH_SIZE*2];
  clck = oyClock();
  for(int j = 0; j < repeat; ++j)
  for(i = 0; i < count; ++i)
  {
    oyMiscBlobGetHash_(hash_texts[7], strlen(hash_texts[7]), oyHASH_L3, md5_return);
  }
  clck = oyClock() - clck;
  uint32_t hash32 = oyMiscBlobL3_( hash_texts[7], strlen(hash_texts[7]) );
  if( strcmp( (const char*) md5_return, "51124909" ) == 0 &&
      hash32 == 51124909 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, repeat,clck/(double)CLOCKS_PER_SEC,"entries",
    "oyMiscBlobGetHash_(oyHASH_L3) = %s", md5_return );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyMiscBlobGetHash_(oyHASH_L3) = %s", md5_return );
  }

  uint32_t id[8]={0,0,0,0,0,0,0,0};
  clck = oyClock();
  for(int j = 0; j < repeat; ++j)
  for(i = 0; i < count; ++i)
    oyMiscBlobGetHash_(hash_texts[7], strlen(hash_texts[7]), oyHASH_MD5, (unsigned char*)id);
  clck = oyClock() - clck;
  char * t = NULL;
  oyjlStringAdd( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                          "%08x%08x%08x%08x",
                          id[0],id[1],id[2],id[3] );
  if( strcmp( t, "99dff16d86c143ad68ff2a390fad6233" ) == 0 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, repeat,clck/(double)CLOCKS_PER_SEC,"entries",
    "oyMiscBlobGetHash_(oyHASH_MD5) = %s", t );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyMiscBlobGetHash_(oyHASH_MD5) = %s", t );
  }

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

oyjlTESTRESULT_e testPaths()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

  const char * type_names[] = {
    "oyPATH_NONE", "oyPATH_ICC", "oyPATH_POLICY", "oyPATH_MODULE", "oyPATH_SCRIPT", "oyPATH_CACHE", "oyPATH_LOGO"
  };
  oyPATH_TYPE_e types[] = {
    oyPATH_NONE, oyPATH_ICC, oyPATH_POLICY, oyPATH_MODULE, oyPATH_SCRIPT, oyPATH_CACHE, oyPATH_LOGO
  };
  const char * scope_names[] = {
    "oySCOPE_USER_SYS", "oySCOPE_USER", "oySCOPE_SYSTEM", "oySCOPE_OYRANOS", "oySCOPE_MACHINE"
  };
  oySCOPE_e scopes[] = {
    oySCOPE_USER_SYS, oySCOPE_USER, oySCOPE_SYSTEM, (oySCOPE_e)oySCOPE_OYRANOS, (oySCOPE_e)oySCOPE_MACHINE
  };

  for(int i = 1; i <= 6; ++i)
  for(int j = 1; j <= 4; ++j)
  {
  char * text = oyGetInstallPath( types[i], scopes[j], oyAllocateFunc_ );
  if(text)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyGetInstallPath( %s, %s ): %s", type_names[i],scope_names[j],
                                                oyNoEmptyString_m_(text) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyGetInstallPath( %s, %s ): %s", type_names[i],scope_names[j],
                                                oyNoEmptyString_m_(text) );
  }
  }


  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}


oyjlTESTRESULT_e testConfDomain ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  int error = 0;
  oyConfDomain_s * a = 0, * b = 0;
  const char ** texts = 0;
  char       ** domains = 0;
  int i,j,n;
  uint32_t count = 0,
         * rank_list = 0;
  OBJECT_COUNT_SETUP

  fprintf(stdout, "\n" );

#ifdef __APPLE__
  a = oyConfDomain_FromReg( OY_STD"/device/config.icc_profile.monitor.qarz", 0 );
#else
  a = oyConfDomain_FromReg( OY_STD"/device/config.icc_profile.monitor.oyX1", 0 );
#endif
  error = !a;

  if(!error)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyConfDomain_FromReg() good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyConfDomain_FromReg() failed" );
  }

  if(!testobj)
    testobj = oyObject_NewWithAllocators( myAllocFunc, myDeAllocFunc, "testobj" );
  b = oyConfDomain_Copy( a, testobj );

  if(!error && b && b != a)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyConfDomain_Copy good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyConfDomain_Copy failed" );
  }

  error = oyConfDomain_Release( &b );

  b = oyConfDomain_Copy( a, 0 );

  if(!error && b && a == b )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyConfDomain_Copy() good" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyConfDomain_Copy() failed" );
  }

  oyConfDomain_Release( &a );
  oyConfDomain_Release( &b );

  error = oyConfigDomainList( "//" OY_TYPE_STD, &domains, &count, &rank_list,
                              malloc );
  if( count && domains)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyConfigDomainList \"%s\": %d", "//" OY_TYPE_STD "",
                                                    (int)count );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyConfigDomainList \"%s\": %d", "//" OY_TYPE_STD "",
                                                    (int)count );
  }
  if(verbose)
  for( i = 0; i < (int)count; ++i)
    fprintf( zout, "%d: %s\n", i, oyNoEmptyString_m_(domains[i]) );
  if(verbose) fprintf( zout, "\n");

  for(i = 0; i < (int)count; ++i)
  {
    int text_missed = 0;
    const char * t[3] = {0,0,0};
    const char * nick = oyNoEmptyString_m_(domains[i]);

    if(strchr(nick, '/'))
      nick = strrchr(nick, '/') + 1;

    a = oyConfDomain_FromReg( oyNoEmptyString_m_(domains[i]), 0 );
    texts = oyConfDomain_GetTexts( a );
    n = j = 0;
    if(texts)
      while(texts[j]) ++j;

    n = j;
    if(verbose)
    for(j = 0; j < n; ++j)
    {
      t[oyNAME_NICK] = oyConfDomain_GetText( a, texts[j], oyNAME_NICK );
      t[oyNAME_NAME] = oyConfDomain_GetText( a, texts[j], oyNAME_NAME );
      t[oyNAME_DESCRIPTION] = oyConfDomain_GetText( a, texts[j],
                                                          oyNAME_DESCRIPTION );

      if(!t[oyNAME_NICK])
      {
        fprintf(stdout, "  %d %s:\n    \"%s\" oyNAME_NICK is missed\n", j,
                        nick, texts[j]);
        text_missed = 1;
      }
      if(!t[oyNAME_NAME])
      {
        fprintf(stdout, "  %d %s:\n    \"%s\" oyNAME_NAME is missed\n", j,
                        nick, texts[j]);
        text_missed = 1;
      }
      if(!t[oyNAME_DESCRIPTION])
      {
        fprintf(stdout, "  %d %s:\n    \"%s\" oyNAME_DESCRIPTION is missed\n",j,
                        nick, texts[j]);
        text_missed = 1;
      }
      if(strcmp(texts[j], "name") == 0)
        fprintf(zout,"\"%s\": \"%s\" \"%s\" \"%s\"\n", texts[j],
                        t[oyNAME_NICK], t[oyNAME_NAME], t[oyNAME_DESCRIPTION]);
    }

    if(!error && n && !text_missed)
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
      "oyConfDomain_GetTexts() \"%s\" %d good  ", nick, n );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "oyConfDomain_GetTexts() \"%s\" %d failed ", nick, n );
    }

    oyConfDomain_Release( &a );
    if(verbose) fprintf( zout, "----------\n");
  }
  oyStringListRelease_( &domains, count, free );
  oyFree_m_( rank_list );

  OBJECT_COUNT_PRINT( oyjlTESTRESULT_FAIL, 1, 0, NULL )

  return result;
}

oyjlTESTRESULT_e testIO()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  size_t size = 0;
  char * text = oyReadUrlToMemf_( &size, "r", oyAllocateFunc_,
                                  "https://location.services.mozilla.com/v1/geolocate?key=test" );
  if(text)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyReadUrlToMemf_( ): %d", (int)strlen(text) );
    oyDeAllocateFunc_(text);
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyReadUrlToMemf_( )" );
  }

  const char * dir = "not_here";
  int r = oyMakeDir_ (dir);
  if(r == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyMakeDir_(\"%s\"): %d", dir, r );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyMakeDir_(\"%s\"): %d", dir, r );
  }
  char * out_name = NULL;
  r = oyWriteMemToFile2_( "test-file",
                          "Test", strlen("Test"), OY_FILE_TEMP_DIR,
                          &out_name, oyAllocateFunc_ );
  if(r == 0 && out_name)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyWriteMemToFile2_(\"%s\"): %d", out_name, r );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyWriteMemToFile2_(\"%s\"): %d", out_name, r );
  }

  r = oyRemoveFile_(out_name);
  if(r == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyRemoveFile_(\"%s\"): %d", out_name, r );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyRemoveFile_(\"%s\"): %d", out_name, r );
  }
  oyFree_m_(out_name);

  int files_n = 0;
  char ** files = oyLibFilesGet_( &files_n, OY_CMMSUBPATH, oySCOPE_USER_SYS,
                                  0, OY_MODULE_NAME, 0, oyAllocateFunc_ );
  if(files_n > 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyLibFilesGet_(): %d", files_n );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyLibFilesGet_(): %d", files_n );
  }
  oyjlStringListRelease( &files, files_n, oyDeAllocateFunc_ );

  files = oyGetFiles_( ".", &files_n );
  if(files_n > 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyGetFiles_(): %d", files_n );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyGetFiles_(): %d", files_n );
  }
  oyjlStringListRelease( &files, files_n, oyDeAllocateFunc_ );

  return result;
}

/* --- end actual tests --- */

