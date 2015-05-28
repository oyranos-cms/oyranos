/** @file test_oyranos.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2004-2015  Kai-Uwe Behrmann
 *
 *  @brief    Oyranos test suite
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
#include "oyranos_i18n.h"
#include "oyranos_string.h"
#include "oyranos_xml.h"
#include "oyranos_config_internal.h"

/* C++ includes and definitions */
#ifdef __cplusplus
#include <fstream>
#include <iostream>
#define USE_NEW
#endif

#ifdef USE_NEW
void* myAllocFunc(size_t size) { return new char [size]; }
#else
void* myAllocFunc(size_t size) { return calloc(size,1); }
#endif

#include <cmath>


/* --- general test routines --- */

typedef enum {
  oyTESTRESULT_SYSERROR,
  oyTESTRESULT_FAIL,
  oyTESTRESULT_XFAIL,
  oyTESTRESULT_SUCCESS,
  oyTESTRESULT_UNKNOWN
} oyTESTRESULT_e;


const char * oyTestResultToString    ( oyTESTRESULT_e      error )
{
  const char * text = "";
  switch(error)
  {
    case oyTESTRESULT_SYSERROR:text = "SYSERROR"; break;
    case oyTESTRESULT_FAIL:    text = "FAIL"; break;
    case oyTESTRESULT_XFAIL:   text = "XFAIL"; break;
    case oyTESTRESULT_SUCCESS: text = "SUCCESS"; break;
    case oyTESTRESULT_UNKNOWN: text = "UNKNOWN"; break;
    default:                   text = "Huuch, what's that?"; break;
  }
  return text;
}

const char  *  oyIntToString         ( int                 integer )
{
  static char texts[3][255];
  static int a = 0;
  int i;

  if(a >= 3) a = 0;

  for(i = 0; i < 8-log10(integer); ++i)
    sprintf( &texts[a][i], " " );

  sprintf( &texts[a][i], "%d", integer );

  return texts[a++];
}

const char  *  oyProfilingToString   ( int                 integer,
                                       double              duration,
                                       const char        * term )
{
  static char texts[3][255];
  static int a = 0;
  int i, len;

  if(a >= 3) a = 0;

  if(integer/duration >= 1000000.0)
    sprintf( &texts[a][0], "%.02f M%s/s", integer/duration/1000000.0, term );
  else
    sprintf( &texts[a][0], "%.00f %s/s", integer/duration, term );

  len = strlen(&texts[a][0]);

  for(i = 0; i < 16-len; ++i)
    sprintf( &texts[a][i], " " );

  if(integer/duration >= 1000000.0)
    sprintf( &texts[a][i], "%.02f M%s/s", integer/duration/1000000.0, term );
  else
    sprintf( &texts[a][i], "%.00f %s/s", integer/duration, term );

  return texts[a++];
}

FILE * zout = stdout;  /* printed inbetween results */

int oy_test_sub_count = 0;
#define PRINT_SUB( result_, ... ) { \
  if(result_ < result) \
    result = result_; \
  fprintf(stdout, ## __VA_ARGS__ ); \
  fprintf(stdout, " ..\t%s", oyTestResultToString(result_)); \
  if(result_ <= oyTESTRESULT_FAIL) \
    fprintf(stdout, " !!! ERROR !!!" ); \
  fprintf(stdout, "\n" ); \
  ++oy_test_sub_count; \
}


/* --- actual tests --- */

oyTESTRESULT_e testVersion()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );
  fprintf(zout, "compiled version:     %d\n", OYRANOS_VERSION );
  fprintf(zout, " runtime version:     %d\n", oyVersion(0) );

  if(OYRANOS_VERSION == oyVersion(0))
    result = oyTESTRESULT_SUCCESS;
  else
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
  if((lang && (strcmp(lang, "C") == 0)) || !lang)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyLanguage() uninitialised good %s                ", lang );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyLanguage() uninitialised failed                 " );
  }

  setlocale(LC_ALL,"");
  oyI18Nreset();

  lang = oyLanguage();
  if(lang && (strcmp(lang, "C") != 0))
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyLanguage() initialised good %s                  ", lang );
  } else
  { PRINT_SUB( oyTESTRESULT_XFAIL, 
    "oyLanguage() initialised failed %s                ", lang );
  }

  return result;
}

#define TEST_DOMAIN "sw/Oyranos/Tests"
#define TEST_KEY "/test_key"

#include "oyranos_elektra.h"
oyTESTRESULT_e testElektra()
{
  int error = 0;
  char * value = 0,
       * start = 0;
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  oyExportReset_(EXPORT_SETTING);

  fprintf(stdout, "\n" );

  error = oySetPersistentString( TEST_DOMAIN TEST_KEY, oySCOPE_USER,
                                 "NULLTestValue", "NULLTestComment" );
  if(error)
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "oySetPersistentString(%s)", TEST_DOMAIN TEST_KEY );
  } else
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oySetPersistentString(%s)", TEST_DOMAIN TEST_KEY );
  }

  start = oyGetPersistentString(TEST_DOMAIN TEST_KEY, 0, oySCOPE_USER_SYS, 0);
  if(start && start[0])
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyGetPersistentString(%s)", TEST_DOMAIN TEST_KEY );
  } else
  {
    PRINT_SUB( oyTESTRESULT_XFAIL,
    "oyGetPersistentString(%s)", TEST_DOMAIN TEST_KEY );
  }

  printf ("start is %s\n", oyNoEmptyString_m_(start));
  if(!start)
  {
    oyExportStart_(EXPORT_CHECK_NO);
    oyExportEnd_();
    error = oySetPersistentString(TEST_DOMAIN TEST_KEY, oySCOPE_USER,
                                 "NULLTestValue", "NULLTestComment" );
    start = oyGetPersistentString(TEST_DOMAIN TEST_KEY, 0, oySCOPE_USER_SYS, 0);
    printf ("start is %s\n", start);
    
    PRINT_SUB( start?oyTESTRESULT_SUCCESS:oyTESTRESULT_XFAIL,
    "Elektra not initialised? try oyExportStart_(EXPORT_CHECK_NO)" );
  }
  if(!start)
  {
    oyExportStart_(EXPORT_SETTING);
    oyExportEnd_();
    error = oySetPersistentString(TEST_DOMAIN TEST_KEY, oySCOPE_USER,
                                 "NULLTestValue", "NULLTestComment" );
    start = oyGetPersistentString(TEST_DOMAIN TEST_KEY, 0, oySCOPE_USER_SYS, 0);
    PRINT_SUB( start?oyTESTRESULT_SUCCESS:oyTESTRESULT_XFAIL, 
    "Elektra not initialised? try oyExportStart_(EXPORT_SETTING)" );
  }
  if(start)
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
    PRINT_SUB( oyTESTRESULT_SYSERROR, 
    "Elektra error: %d", error );
  } else
  /* we want "start" to be different from "value" */
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
  oyFree_m_( start );
  oyFree_m_( value );

  error = oyDBEraseKey_( TEST_DOMAIN TEST_KEY, oySCOPE_USER );
  if(error)
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyDBEraseKey_(%s)", TEST_DOMAIN TEST_KEY );
  } else
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyDBEraseKey_(%s)", TEST_DOMAIN TEST_KEY );
  }
  oyDB_s * db = oyDB_newFrom( TEST_DOMAIN, oySCOPE_USER_SYS, oyAllocateFunc_ );
  value = oyDB_getString(db, TEST_DOMAIN TEST_KEY);
  oyDB_release( &db );
  if(value && strlen(value))
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "Elektra key not erased                  " );
  } else
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "Elektra key erased                      " );
  }
  oyFree_m_(value);


  error = oySetPersistentString( OY_STD "/device" TEST_KEY "/#0/key-01", oySCOPE_USER,
                                 "SomeValue", "SomeComment" );
  error = oySetPersistentString( OY_STD "/device" TEST_KEY "/#0/key-02", oySCOPE_USER,
                                 "SomeValue", "SomeComment" );
  error = oySetPersistentString( OY_STD "/device" TEST_KEY "/#1/key-01", oySCOPE_USER,
                                 "SomeValue", "SomeComment" );
  error = oySetPersistentString( OY_STD "/device" TEST_KEY "/#1/key-02", oySCOPE_USER,
                                 "SomeValue", "SomeComment" );
  value = oyDBSearchEmptyKeyname_(OY_STD "/device" TEST_KEY, oySCOPE_USER);
  if(value && strcmp( "user/" OY_STD "/device" TEST_KEY "/#2",value) == 0 )
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyDBSearchEmptyKeyname_()=%s", value );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oyDBSearchEmptyKeyname_(%s)", OY_STD "/device" TEST_KEY );
  }
  if(value)
    oyFree_m_( value );

  error = oyDBEraseKey_( OY_STD "/device" TEST_KEY, oySCOPE_USER );
  value = oyDBSearchEmptyKeyname_(OY_STD "/device" TEST_KEY, oySCOPE_USER);
  if(value && strcmp( "user/" OY_STD "/device" TEST_KEY "/#0",value) == 0 )
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyDBSearchEmptyKeyname_()=%s", value );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oyDBSearchEmptyKeyname_(%s)", OY_STD "/device" TEST_KEY );
  }
  oyFree_m_( value );

  error = oySetPersistentString( TEST_DOMAIN "/device" TEST_KEY "/#0/key-01", oySCOPE_USER,
                                 "SomeValue", "SomeComment" );
  error = oySetPersistentString( TEST_DOMAIN "/device" TEST_KEY "/#0/key-02", oySCOPE_USER,
                                 "SomeValue", "SomeComment" );
  error = oySetPersistentString( TEST_DOMAIN "/device" TEST_KEY "/#1/key-01", oySCOPE_USER,
                                 "SomeValue", "SomeComment" );
  error = oySetPersistentString( TEST_DOMAIN "/device" TEST_KEY "/#1/key-02", oySCOPE_USER,
                                 "SomeValue", "SomeComment" );
  value = oyDBSearchEmptyKeyname_(TEST_DOMAIN "/device" TEST_KEY, oySCOPE_USER);
  if(value && strcmp( "user/" TEST_DOMAIN "/device" TEST_KEY "/#2",value) == 0 )
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyDBSearchEmptyKeyname_()=%s", value );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oyDBSearchEmptyKeyname_()=%s", value );
  }
  oyFree_m_( value );

  error = oyDBEraseKey_( TEST_DOMAIN "/device" TEST_KEY, oySCOPE_USER );
  value = oyDBSearchEmptyKeyname_(TEST_DOMAIN "/device" TEST_KEY, oySCOPE_USER);
  if(value && strcmp( "user/" TEST_DOMAIN "/device" TEST_KEY "/#0",value) == 0 )
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyDBSearchEmptyKeyname_()=%s", value );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oyDBSearchEmptyKeyname_()=%s", TEST_DOMAIN "/device" TEST_KEY );
  }

  char * key = 0;
  oyStringAddPrintf( &key, oyAllocateFunc_, oyDeAllocateFunc_,
                     "%s/array_key", value );
  error = oySetPersistentString( key, oySCOPE_USER_SYS,
                                 "ArrayValue", "ArrayComment" );
  oyFree_m_( value );
  value = oyGetPersistentString(strchr(key,'/')+1, 0, oySCOPE_USER_SYS, 0);
  if(value && strcmp(value, "ArrayValue") == 0)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oySetPersistentString(%s, oySCOPE_USER_SYS)", key );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oySetPersistentString(%s, oySCOPE_USER_SYS)", key );
  }
  oyFree_m_( key );
  oyFree_m_( value );

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

oyTESTRESULT_e testStringRun ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int error = 0,
      i;

  const char * test = OY_INTERNAL "/display.oydi/display_name";
  
  int test_n = oyStringSegmentsN_( test, oyStrlen_(test), OY_SLASH_C );
  test_n = oyStringSegments_( test, OY_SLASH_C );
  char * test_out = (char*) malloc(strlen(test));
  char * test_sub = 0;
  int test_end;

  fprintf(zout, "\"%s\"\n", test );

  error = 0;
  if(test_n != 5) error = 1;
  for(i = 0; i < test_n; ++i)
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
      fprintf(zout, "%d%c%d%c \"%s\"\n", i, j?' ':'/',j, j ? '.': ' ',
                      test_out);
    }
  }

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyStringSegmentxxx()...                            " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyStringSegmentxxx()...                            " );
  }


  double clck = oyClock();
  for(i = 0; i < 1000000; ++i)
    test_sub = oyFilterRegistrationToSTextField( test, oyFILTER_REG_OPTION,
                                                 &test_end );
  clck = oyClock() - clck;

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyFilterRegistrationToSTextField()          %d %.03f", i,
                                       (double)clck/(double)CLOCKS_PER_SEC );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyFilterRegistrationToSTextField()                 " );
  }


  clck = oyClock();
  for(i = 0; i < 1000000; ++i)
  {
    test_sub = oyFilterRegistrationToText( test, oyFILTER_REG_OPTION, 0 );
    oyFree_m_(test_sub);
  }
  clck = oyClock() - clck;

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyFilterRegistrationToText()                %d %.03f", i,
                                       (double)clck/(double)CLOCKS_PER_SEC );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyFilterRegistrationToText())                      " );
  }



  test = "//" OY_TYPE_STD "/display.oydi/";
  fprintf(zout, "\"%s\"\n", test );
  test_n = oyStringSegmentsN_( test, oyStrlen_(test), OY_SLASH_C );
  test_n = oyStringSegments_( test, OY_SLASH_C );

  error = 0;
  if(test_n != 5) error = 1;
  for(i = 0; i < test_n; ++i)
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
      fprintf(zout, "%d%c%d%c \"%s\"\n", i, j?' ':'/', j, j ? '.': ' ',
                      test_out);
    }
  }
  free(test_out);

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyStringSegmentxxx()...                            " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyStringSegmentxxx()...                            " );
  }

  return result;
}


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

  o = oyOption_FromRegistration( 0, 0 );
  if(o)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_New() good                               " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_New() failed                             " );
  }

  error = oyOption_FromRegistration( "blabla", 0 ) != 0;
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
  oyFree_m_(ptr);

  return result;
}

#include "oyOption_s_.h"

oyTESTRESULT_e testOptionInt ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  int error = 0;
  oyOption_s * o = 0;
  oyOption_s_ * oi = 0;
  int32_t erg[4] = { -1,-1,-1,-1 };

  oyExportReset_(EXPORT_SETTING);

  fprintf(stdout, "\n" );

  o = oyOption_FromRegistration( "//" OY_TYPE_STD "/filter/x", 0 );
  oi = (oyOption_s_*) o;

  error = oyOption_SetFromInt( o, 0, 0, 0 );
  if(!error && oi->value &&
     oi->value->int32 == 0 &&
     oi->value_type == oyVAL_INT)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() good                        " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromInt() failed                      " );
  }

  error = oyOption_SetFromInt( o, 58293, 0, 0 );
  if(!error && oi->value &&
     oi->value->int32 == 58293 &&
     oi->value_type == oyVAL_INT)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() single int32_t good         " );
    erg[0] = oyOption_GetValueInt( o, 0 );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromInt() single int32_t failed       " );
  }

  error = oyOption_SetFromInt( o, 58293, 1, 0 );
  if(!error && oi->value &&
     oi->value->int32_list[0] == 2 &&
     oi->value->int32_list[1] == 58293 &&
     oi->value->int32_list[2] == 58293 &&
     oi->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() add int32_t list good     " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromInt() add int32_t list failed   " );
  }

  error = oyOption_SetFromInt( o, 58293, 2, 0 );
  if(!error && oi->value &&
     oi->value->int32_list[0] == 3 &&
     oi->value->int32_list[1] == 58293 &&
     oi->value->int32_list[2] == 58293 &&
     oi->value->int32_list[3] == 58293 &&
     oi->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() add int32_t list good     " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromInt() add int32_t list failed   " );
  }

  error = oyOption_SetFromInt( o, 58293, 1, 0 );
  if(!error && oi->value &&
     oi->value->int32_list[0] == 3 &&
     oi->value->int32_list[1] == 58293 &&
     oi->value->int32_list[2] == 58293 &&
     oi->value->int32_list[3] == 58293 &&
     oi->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() new int32_t list good     " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromInt() new int32_t list failed   " );
  }

  error = oyOption_SetFromInt( o, 293, 1, 0 );
  if(!error && oi->value &&
     oi->value->int32_list[0] == 3 &&
     oi->value->int32_list[1] == 58293 &&
     oi->value->int32_list[2] == 293 &&
     oi->value->int32_list[3] == 58293 &&
     oi->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() modify int32_t list good  " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromInt() modify int32_t list failed" );
  }

  error = oyOption_SetFromInt( o, 58293, 0, 0 );
  if(!error && oi->value &&
     oi->value->int32_list[0] == 3 &&
     oi->value->int32_list[1] == 58293 &&
     oi->value->int32_list[2] == 293 &&
     oi->value->int32_list[3] == 58293 &&
     oi->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() set int32_t list good     " );
    erg[0] = oyOption_GetValueInt( o, 0 );
    erg[1] = oyOption_GetValueInt( o, 1 );
    erg[2] = oyOption_GetValueInt( o, 2 );
    erg[3] = oyOption_GetValueInt( o, 3 );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromInt() set int32_t list failed   " );
  }

  if(!error && erg[0] == 58293 && erg[1] == 293 && erg[2] == 58293 &&
               erg[3] == 0)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_GetValueInt() good                     " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_GetValueInt() failed                   " );
  }

  oyOption_Release( &o );


  o = oyOption_FromRegistration( "//" OY_TYPE_STD "/filter/y", 0 );
  oi = (oyOption_s_*) o;
  /*o->value_type = oyVAL_INT_LIST;*/
  error = oyOption_SetFromInt( o, 58293, 2, 0 );
  if(!error && oi->value &&
     oi->value->int32_list[0] == 3 &&
     oi->value->int32_list[1] == 0 &&
     oi->value->int32_list[2] == 0 &&
     oi->value->int32_list[3] == 58293 &&
     oi->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() explicite int32_t list good" );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromInt() explicite int32_t list failed" );
  }

  oyOption_Release( &o );

  return result;
}

oyTESTRESULT_e testOptionsSet ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  int error = 0;
  oyOptions_s * setA = 0;
  const char * t = NULL;

  fprintf(stdout, "\n" );

  error = oyOptions_SetFromText( &setA,
                                 "org/test/" OY_TYPE_STD "/filter/gamma_A",
                                 "1", OY_CREATE_NEW );
  error = oyOptions_SetFromText( &setA,
                                 "org/test/" OY_TYPE_STD "/filter/gamma_A1",
                                 "1", OY_CREATE_NEW );
  error = oyOptions_SetFromText( &setA,
                                 "org/test/" OY_TYPE_STD "/filter/gamma_A12",
                                 "1", OY_CREATE_NEW );

  if(!error && oyOptions_Count( setA ) == 3)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOptions_SetFromText() similiar registration good  " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOptions_SetFromText() similiar registration failed" );
  }

  error = oyOptions_SetFromText( &setA,
                                 "org/oyranos/" OY_TYPE_STD "/filter/gamma_A2",
                                 "one\ntwo\nthree\nfour",
                                 OY_CREATE_NEW | OY_STRING_LIST);
  t = oyOptions_GetText( setA, oyNAME_NAME );
  if(t && t[0] && oyOptions_Count( setA ) == 4)
  {
    oyOption_s * opt;
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOptions_GetText()                             good" );
    fprintf( zout, "%s\n", t );
    opt = oyOptions_Get( setA, 3 );
    fprintf( zout, "fourth option\n" );
    char * t = oyOption_GetValueText(opt, malloc );
    fprintf( zout, "ValueText: %s\n", t );
    if(t) free(t);
    fprintf( zout, "NICK: %s\n", oyOption_GetText(opt, oyNAME_NICK) );
    fprintf( zout, "NAME: %s\n", oyOption_GetText(opt, oyNAME_NAME) );
    fprintf( zout, "DESCRIPTION: %s\n", oyOption_GetText(opt, oyNAME_DESCRIPTION) );
    oyOption_Release( &opt );

  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOptions_GetText()                           failed" );
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
    o = oyOption_FromRegistration( OY_STD "/filter", NULL );
  oyOption_SetFromText( o, "", 0 );
  if(found)
    oyOption_Release( &o );
  else
    oyOptions_MoveIn( setA, &o, -1 );

  found = 0;
  o = oyOptions_Find( setA, OY_STD "/filter/level_one", oyNAME_REGISTRATION );
  if(o)
    found = 1;
  else
    o = oyOption_FromRegistration( OY_STD "/filter/level_one", NULL );
  oyOption_SetFromText( o, "", 0 );
  if(found)
    oyOption_Release( &o );
  else
    oyOptions_MoveIn( setA, &o, -1 );

  found = 0;
  o = oyOptions_Find( setA, OY_STD "/filter/level_one/level_two", oyNAME_REGISTRATION );
  if(o)
    found = 1;
  else
    o = oyOption_FromRegistration( OY_STD "/filter/level_one/level_two", NULL );
  oyOption_SetFromText( o, "1", 0 );
  if(found)
    oyOption_Release( &o );
  else
    oyOptions_MoveIn( setA, &o, -1 );

  t = oyOptions_GetText( setA, oyNAME_NICK );
  if(t && t[0] && oyOptions_Count( setA ) == 7)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOptions_GetText() hierarchical                good" );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptions_GetText() hierarchical              failed" );
  }
  fprintf( zout, "%s\n", t );

  oyOptions_Release( &setA );

  return result;
}

oyTESTRESULT_e testOptionsCopy ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  int error = 0;
  oyOptions_s * setA = 0, * setB = 0, * setC = 0,
              * resultA = 0, * resultB = 0;
  int32_t count = 0;

  fprintf(stdout, "\n" );

  error = oyOptions_SetFromText( &setA,
                OY_INTERNAL "/lcm2.color.icc/rendering_bpc.advanced",
                                 "1", OY_CREATE_NEW );
  error = oyOptions_SetFromText( &setA,
                                 "//" OY_TYPE_STD "/image/A", "true",
                                 OY_CREATE_NEW );
  error = oyOptions_SetFromText( &setA,
                                 "//" OY_TYPE_STD "/image/A", "true",
                                 OY_CREATE_NEW );

  if(!error && oyOptions_Count( setA ) == 2)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOptions_SetFromText() good                    " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOptions_SetFromText() failed                  " );
  }

  error = oyOptions_SetFromText( &setB,
                                 "//" OY_TYPE_STD "/config/A", "true",
                                 OY_CREATE_NEW );
  error = oyOptions_SetFromText( &setB,
                                 "//" OY_TYPE_STD "/config/B", "true",
                                 OY_CREATE_NEW );
  error = oyOptions_SetFromText( &setB,
                                 "//" OY_TYPE_STD "/config/C", "true",
                                 OY_CREATE_NEW );

  error = oyOptions_SetFromText( &setC,
                                 "//" OY_TYPE_STD "/config/B", "true",
                                 OY_CREATE_NEW );
  error = oyOptions_SetFromText( &setC,
                                 "//" OY_TYPE_STD "/config/D", "true",
                                 OY_CREATE_NEW );
  error = oyOptions_SetFromText( &setC,
                                 "//" OY_TYPE_STD "/config/C", "true",
                                 OY_CREATE_NEW );

  error = oyOptions_CopyFrom( &resultA, setA, oyBOOLEAN_UNION,
                              oyFILTER_REG_NONE,0 );

  if(!error && oyOptions_Count( resultA ) == 2 &&
     oyOptions_FindString( resultA, "rendering_bpc", 0 ) &&
     oyOptions_FindString( resultA, "A", 0 ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOptions_CopyFrom() oyBOOLEAN_UNION good       " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOptions_CopyFrom() oyBOOLEAN_UNION failed     " );
  }

  error = oyOptions_CopyFrom( &resultA, setB, oyBOOLEAN_DIFFERENZ,
                              oyFILTER_REG_OPTION,0 );

  if(!error && oyOptions_Count( resultA ) == 3 &&
     oyOptions_FindString( resultA, "rendering_bpc", 0 ) &&
     oyOptions_FindString( resultA, "B", 0 ) &&
     oyOptions_FindString( resultA, "C", 0 ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOptions_CopyFrom() oyBOOLEAN_DIFFERENZ good   " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOptions_CopyFrom() oyBOOLEAN_DIFFERENZ failed " );
  }


  error = oyOptions_Filter( &resultB, &count, 0, oyBOOLEAN_INTERSECTION,
                  OY_INTERNAL "/image", setA  );

  if(!error && oyOptions_Count( resultB ) == 1 &&
     oyOptions_FindString( resultB, "A", 0 ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOptions_Filter() oyBOOLEAN_INTERSECTION good  " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOptions_Filter() oyBOOLEAN_INTERSECTION failed" );
  }

  oyOptions_Release( &setA );
  oyOptions_Release( &setB );
  oyOptions_Release( &setC );
  oyOptions_Release( &resultA );
  oyOptions_Release( &resultB );

  return result;
}

#include "oyBlob_s.h"

oyTESTRESULT_e testBlob ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  int error = 0;
  oyBlob_s * a = 0, * b = 0;
  oyPointer ptr = 0;
  const char static_ptr[16] = {0,1,0,1,0,1,0,1,  0,1,0,1,0,1,0,1};
  const char type[8] = "test";
  oyObject_s object = oyObject_New();

  fprintf(stdout, "\n" );

  a = oyBlob_New( 0 );
  ptr = (oyPointer) static_ptr;
  error = oyBlob_SetFromStatic( a, ptr, 16, type );

  if(!error)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyBlob_SetFromSatic() good                      " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyBlob_SetFromSatic() failed                    " );
  }

  b = oyBlob_Copy( a, object );

  if(!error && b && oyBlob_GetPointer(b) && oyBlob_GetSize(b) &&
     oyBlob_GetPointer( b ) == static_ptr)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyBlob_Copy( static ) good                      " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyBlob_Copy( static ) failed                    " );
  }

  error = oyBlob_Release( &b );

  error = oyBlob_SetFromStatic( a, ptr, 0, type );

  if(!error)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyBlob_SetFromSatic(0) good                     " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyBlob_SetFromSatic(0) failed                   " );
  }

  b = oyBlob_Copy( a, object );

  if(!error && b && oyBlob_GetPointer(b) && !oyBlob_GetSize(b) &&
     oyBlob_GetPointer(b) == static_ptr)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyBlob_Copy( static 0) good                     " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyBlob_Copy( static 0) failed                   " );
  }

  ptr = malloc(1024);
  error = oyBlob_SetFromData( a, ptr, 1024, type );

  if(!error && oyBlob_GetPointer(a) && oyBlob_GetSize(a) == 1024 &&
     oyBlob_GetPointer(a) != ptr)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyBlob_SetFromData() good                       " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyBlob_SetFromData() failed                     " );
  }
  
  error = oyBlob_Release( &b );

  if(!error && !b)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyBlob_Release() good                           " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyBlob_Release() failed                         " );
  }

  b = oyBlob_Copy( a, object );

  if(!error && b && a != b && oyBlob_GetPointer(b) &&
     oyBlob_GetSize(a) == oyBlob_GetSize(b) &&
     oyBlob_GetPointer(a) != oyBlob_GetPointer(b) )
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyBlob_Copy() good                              " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyBlob_Copy() failed                            " );
  }

  oyBlob_Release( &a );
  oyBlob_Release( &b );
  oyObject_Release( &object );
  free(ptr);

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
  xmlDocPtr doc = 0;

  oyExportReset_(EXPORT_SETTING);

  fprintf(zout, "\n" );

  /* we check for our standard CMM */
  opts = oyOptions_ForFilter( "//" OY_TYPE_STD "/lcm2",
                                            oyOPTIONATTRIBUTE_ADVANCED /* |
                                            oyOPTIONATTRIBUTE_FRONT |
                                            OY_SELECT_COMMON */, 0 );

  oyOptions_SetFromText( &opts, "cmyk_cmyk_black_preservation", "1", 0 );

  count = oyOptions_Count( opts );
  if(!count)
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
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
      PRINT_SUB( oyTESTRESULT_FAIL, 
      "oyOptions_FindString() could not find option" );
    } else
    if(size > 1)
    {
      PRINT_SUB( oyTESTRESULT_FAIL, 
      "oyOptions_FindString() returned doubled options %u",
                       (unsigned int)size );
    } else
      PRINT_SUB( oyTESTRESULT_SUCCESS, 
      "oyOptions_FindString() returned one option" );
  }

  /*ptr = xmlSaveToBuffer( buf, 0, 0 );*/


  text = oyStringCopy(oyOptions_GetText( opts, oyNAME_NAME ), oyAllocateFunc_);

  {
    if(!text || !strlen(text))
    {
      PRINT_SUB( oyTESTRESULT_FAIL, 
      "oyOptions_GetText() returned no text             " );
    } else
    {
      PRINT_SUB( oyTESTRESULT_SUCCESS, 
      "oyOptions_GetText() returned text               %d", (int)strlen(text) );
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
    char * t, *t2, *t3;
    o = oyOptions_Get( opts, i );
    t = oyStringCopy(oyOption_GetText(o, oyNAME_DESCRIPTION), oyAllocateFunc_);
    t2 = oyOption_GetValueText( o, malloc );
    t3 = oyFilterRegistrationToText( oyOption_GetText( o, oyNAME_DESCRIPTION),
                                     oyFILTER_REG_OPTION, 0 );
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

  doc = xmlParseMemory( text, oyStrlen_( text ) );
  error = !doc;
  {
    if(error)
    {
      PRINT_SUB( oyTESTRESULT_FAIL, 
      "libxml2::xmlParseMemory() returned could not parse the document" );
    } else
    {
      PRINT_SUB( oyTESTRESULT_SUCCESS, 
      "libxml2 returned document                        " );
    }
  }
  oyFree_m_( text );

  xmlDocDumpFormatMemory( doc, (xmlChar**)&text, &i, 1 );
  fprintf(zout,"xmlDocDump: %s\n", text);
  xmlFreeDoc( doc );
  free(text);
  /*xmlSaveDoc( ptr, doc );*/

  oyOptions_Release( &opts );

  opts = oyOptions_ForFilter( "//" OY_TYPE_STD "/lcm2",
                                            oyOPTIONATTRIBUTE_ADVANCED  |
                                            oyOPTIONATTRIBUTE_FRONT |
                                            OY_SELECT_COMMON, 0 );
  fprintf(zout,"Show advanced common front end options:\n");
  countB = oyOptions_Count( opts );
  for( i = 0; i < countB; ++i)
  {
    char * t, *t2, *t3;
    o = oyOptions_Get( opts, i );
    t = oyStringCopy(oyOption_GetText(o, oyNAME_DESCRIPTION), oyAllocateFunc_);
    t2 = oyOption_GetValueText( o, malloc );
    t3 = oyFilterRegistrationToText( oyOption_GetText( o, oyNAME_DESCRIPTION),
                                     oyFILTER_REG_OPTION, 0 ),
    fprintf(zout,"%d: \"%s\": \"%s\" %s %u\n", i, 
           t, t2, t3,
           (unsigned int)((oyOption_s_*)o)->flags );

    oyFree_m_( t );
    oyFree_m_( t2 );
    oyFree_m_( t3 );
    oyOption_Release( &o );
  }
  oyOptions_Release( &opts );


  return result;
}

#include "oyranos_helper.h"
oyTESTRESULT_e testInterpolation ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  uint16_t ramp[4] = { 0, 21845, 43690, 65535 };
  float pol;

  fprintf(stdout, "\n" );

  pol = oyLinInterpolateRampU16( ramp, 4, 0.499999 );
  if(pol < ramp[3]/2.0)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "Interpolation is fine %f                     ", pol );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "Interpolation failed: %f < %f                          ", pol, ramp[3]/2.0 );
  }

  pol = oyLinInterpolateRampU16( ramp, 4, 0.5 );
  if(pol == ramp[3]/2.0)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "Interpolation is fine.                                 " );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "Interpolation failed: %f == %f                         ", pol, ramp[3]/2.0 );
  }

  pol = oyLinInterpolateRampU16( ramp, 4, 2.0/3.0 );
  if(pol == ramp[2])
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "Interpolation is fine.                                 " );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "Interpolation failed: %f == %u                         ", pol, ramp[2] );
  }

  pol = oyLinInterpolateRampU16( ramp, 4, -1 );
  if(pol == 0.0)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "Interpolation is fine.                                 " );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "Interpolation failed: %f == 0.0                        ", pol );
  }

  pol = oyLinInterpolateRampU16( ramp, 4, 1.1 );
  if(pol == ramp[3])
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "Interpolation is fine.                                 " );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "Interpolation failed: %f == %u                         ", pol, ramp[3] );
  }
  return result;
}

#include "oyProfile_s.h"
#include "oyranos_icc.h"

oyTESTRESULT_e testProfile ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  size_t size = 0;
  oyPointer data;
  oyProfile_s * p_a,
              * p_b;
  uint32_t icc_profile_flags = OY_ICC_VERSION_2;

  oyExportReset_(EXPORT_SETTING);

  fprintf(stdout, "\n" );

  p_a = oyProfile_FromStd ( oyASSUMED_WEB, 0, NULL );
  if(!p_a)
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "No assumed WEB profile found                           " );
  } else
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "found oyASSUMED_WEB                                    " );
  }
  
  /* compare the usual conversion profiles with the total of profiles */
  data = oyProfile_GetMem( p_a, &size, 0, malloc );
  if(!data || ! size)
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "Could get memory from profile                          " );
  } else
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "Obtained memory block from oyASSUMED_WEB:        %d    ", (int)size );
  }

  p_b = oyProfile_FromMem( size, data, 0,0 );
  if(!p_b)
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "Could not load profile from memory.                    " );
  } else
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "Loaded profile from memory of oyASSUMED_WEB            " );
  }

  if(!oyProfile_Equal( p_a, p_b ))
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "Reload of oyASSUMED_WEB failed. Unexplained difference." );
  } else
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyASSUMED_WEB is equal to memory loaded oyProfile_s.   " );
  }

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

  oyOptions_s * opts = oyOptions_New(0),
              * r = 0;

  oyOptions_SetFromInt( &opts, "///icc_profile_flags", icc_profile_flags, 0, OY_CREATE_NEW );
  oyOptions_MoveIn( opts, &matrix, -1 );
  const char * reg = "//"OY_TYPE_STD"/create_profile.color_matrix.icc";
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
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOptions_Handle( \"create_profile.icc_profile.color_matrix\") failed" );
  } else
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOptions_Handle( \"create_profile.icc_profile.color_matrix\")       " );
  }

  icSignature vs = (icUInt32Number) oyValueUInt32( (icUInt32Number) oyProfile_GetSignature( p, oySIGNATURE_VERSION ) );
  char * v = (char*)&vs;
  if( v[0] == 2 )
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOptions_Handle( \"create_profile.icc_profile.color_matrix\") version: %d.%d.%d", (int)v[0], (int)v[1]/16, (int)v[1]%16 );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptions_Handle( \"create_profile.icc_profile.color_matrix\") version: %d.%d.%d", (int)v[0], (int)v[1]/16, (int)v[1]%16 );
  }

  FILE * fp = fopen( ICC_TEST_NAME".icc", "rb" );
  if(fp)
  {
    fclose(fp); fp = 0;
    remove( ICC_TEST_NAME".icc" );
  }
  
  data = oyProfile_GetMem( p, &size, 0, malloc );
  int error = oyWriteMemToFile_( ICC_TEST_NAME".icc", data, size );
  if(!error )
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyWriteMemToFile_( \"%s\")          ", ICC_TEST_NAME".icc" );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oyWriteMemToFile_( \"%s\")  failed  ", ICC_TEST_NAME".icc" );
  }

  if(size >= 128 &&
     oyCheckProfileMem( data, 128, 0 ) == 0)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyCheckProfileMem( \"%d\")          ", (int)size );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oyCheckProfileMem( \"%d\")          ", (int)size );
  }

  oyProfile_Release( &p );

  p = oyProfile_FromFile( ICC_TEST_NAME".icc", OY_SKIP_NON_DEFAULT_PATH, NULL );
  if(!p )
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyProfile_FromFile( OY_SKIP_NON_DEFAULT_PATH )       " );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oyProfile_FromFile( OY_SKIP_NON_DEFAULT_PATH ) failed" );
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

    if( p && v[0] == 2 )
    {
      PRINT_SUB( oyTESTRESULT_SUCCESS,
      "oyProfile_FromName( \"%s\") version: %d.%d.%d", names[i], (int)v[0], (int)v[1]/16, (int)v[1]%16 );
    } else
    {
      PRINT_SUB( oyTESTRESULT_FAIL,
      "oyProfile_FromName( \"%s\") version: %d.%d.%d", names[i], (int)v[0], (int)v[1]/16, (int)v[1]%16 );
    }

    oyProfile_Release( &p );
  }

  return result;
}

#include "oyProfiles_s.h"

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
  profs = oyProfiles_ForStd( oyDEFAULT_PROFILE_START, 0, &current, 0 );
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
    "profiles found for oyProfileListGet:        %u",(unsigned int) size );
  }
  oyStringListRelease_( &texts, size, oyDeAllocateFunc_ );

  if((int)size < count)
  {
    PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyProfileListGet() returned less than oyDEFAULT_PROFILE_START %u|%d", (unsigned int)size, count );
  } else if(count)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyProfileListGet and oyDEFAULT_PROFILE_START ok %u|%d", (unsigned int)size, count );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oyProfileListGet and/or oyDEFAULT_PROFILE_START are wrong %u|%d", (unsigned int)size, count );
  }

  /* compare the default profile spaces with the total of profiles */
  countB = 0;
  /* thread testing needs initialisation through oyThreadLockingSet()
#pragma omp parallel for private(current,count,p,tmp,profs) */
  for(i = oyEDITING_XYZ; i <= oyEDITING_GRAY; ++i)
  {
    profs = oyProfiles_ForStd( (oyPROFILE_e)i, 0, &current, 0 );

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
    "oyProfileListGet() returned less than oyPROFILE_e %u|%d", (unsigned int)size, count );
  } else
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyProfileListGet and oyPROFILE_e ok %u|%d", (unsigned int)size, countB );
  }


  return result;
}


oyTESTRESULT_e testProfileLists ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;
#ifdef _WIN32
  int repeat = 100;
#else
  int repeat = 1000;
#endif

  fprintf(stdout, "\n" );

  double clck = oyClock();

  uint32_t ref_count = 0;
  char ** reference = oyProfileListGet(0, &ref_count, myAllocFunc);

  if((int)ref_count)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyProfileListGet() returned profiles %d", (int)ref_count );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oyProfileListGet() zero" );
  }

  int i,j;
  for(i = 0; i < repeat; ++i)
  {
    uint32_t count = 0;
    char ** names = oyProfileListGet(0, &count, myAllocFunc);
    if(count != ref_count)
    {
      PRINT_SUB( oyTESTRESULT_FAIL, 
      ": wrong profile count: %u/%u", (unsigned int)count, (unsigned int)ref_count );
    }
    for(j = 0; j < (int)count; ++j)
    {
      if(!(names[j] && strlen(names[j])) ||
         strcmp( names[j], reference[j] ) != 0 )
      {
        PRINT_SUB( oyTESTRESULT_FAIL, 
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

#ifdef __cplusplus
  std::cout << std::endl;
  std::cout << repeat << " + 1 calls to oyProfileListGet() "
            << oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "calls")
            << std::endl;
#else
  fprintf(zout, "\n%d + 1 calls to oyProfileListGet() %s\n",
                repeat,  oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "calls") );
#endif

  return result;
}

#include "oyProfile_s_.h"           /* oyProfile_ToFile_ */

oyTESTRESULT_e testProofingEffect ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;
  oyOptions_s * opts = oyOptions_New(0),
              * result_opts = 0;
  const char * text = 0;
  oyProfile_s * prof = oyProfile_FromStd( oyEDITING_CMYK, 0, NULL ),
              * abstract;
  int error;

  fprintf(stdout, "\n" );

  error = oyOptions_MoveInStruct( &opts, "//"OY_TYPE_STD"/icc_profile.proofing_profile",
                                  (oyStruct_s**) &prof, OY_CREATE_NEW );
  error = oyOptions_Handle( "//"OY_TYPE_STD"/create_profile.proofing_effect",
                            opts,"create_profile.proofing_effect",
                            &result_opts );
  abstract = (oyProfile_s*)oyOptions_GetType( result_opts, -1, "icc_profile",
                                              oyOBJECT_PROFILE_S );
  oyOptions_Release( &result_opts );
  oyOptions_Release( &opts );

  text = oyProfile_GetText( abstract, oyNAME_DESCRIPTION );

  if(abstract)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOptions_Handle(\"create_profile\"): %s", text );
  } else if(error == -1)
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptions_Handle(\"create_profile\") no" );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptions_Handle(\"create_profile\") zero" );
  }

  oyProfile_ToFile_( (oyProfile_s_*)abstract, "test_proof_effect.icc" );
  oyProfile_Release( &abstract );

  return result;
}

#include "oyImage_s.h"
#include "oyConversion_s.h"

oyTESTRESULT_e testDeviceLinkProfile ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  double buf[24];
  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyProfile_s * prof = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, 0 ), *dl = 0;
  oyImage_s * in = oyImage_Create( 2, 2, buf, OY_TYPE_123_DBL, prof, 0 );
  oyImage_s * out = oyImage_CreateForDisplay( 2, 2, buf, OY_TYPE_123_DBL, 0,
                                              0,0, 12,12,
                                              icc_profile_flags, 0 );
  oyOptions_s * options = NULL;
  oyOptions_SetFromText( &options, OY_CMM_STD"/context", "lcm2", OY_CREATE_NEW );
  oyConversion_s *cc = oyConversion_CreateBasicPixels( in, out, options, 0 );
  oyFilterGraph_s * graph = NULL;
  oyFilterNode_s * node = NULL;
  oyBlob_s * blob = NULL;
  int error = 0;
  const char * fn = NULL,
             * prof_fn = oyProfile_GetFileName( prof, -1 );
  int i,n=0, len;

  fprintf(stdout, "\n" );

  fprintf(stdout, "creating DL from sRGB to CIE*XYZ\n" );

  memset( buf, 0, sizeof(double)*24);

  /*oyConversion_RunPixels( cc, 0 );*/

  if(cc)
    graph = oyConversion_GetGraph( cc );
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

  fn = oyProfile_GetFileName( dl, 0 );
  if(fn && prof_fn && strcmp(fn,prof_fn) == 0)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyProfile_GetFileName(dl, 0): %s", fn );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oyProfile_GetFileName(dl, 0): %s  %s", oyNoEmptyString_m_(fn), oyNoEmptyString_m_(prof_fn) );
  }

  fn = oyProfile_GetFileName( dl, 1 );
  if(fn)
  {
    PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyProfile_GetFileName(dl, 1): %s", fn );
  } else
  {
    PRINT_SUB( oyTESTRESULT_FAIL,
    "oyProfile_GetFileName(dl, 1): %s", oyNoEmptyString_m_(fn) );
  }

  error = oyConversion_Release( &cc );
  error = oyImage_Release( &in );
  error = oyImage_Release( &out );
  error = oyProfile_Release( &prof );
  error = oyProfile_Release( &dl );


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
    oy_test_cache_ = oyStructList_New( 0 );

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

static void    setupColourTable      ( PrivColorContext  * ccontext,
                                       int                 advanced )
{
  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyConversion_s * cc;
  int error = 0;
  oyProfile_s * dst_profile = ccontext->dst_profile, * web = 0;

    if(!ccontext->dst_profile)
      dst_profile = web = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, 0 );


    {
      int flags = 0;
      int ** ptr;

      oyProfile_s * src_profile = ccontext->src_profile;
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
        src_profile = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, 0 );

      if(!src_profile)
        oyCompLogMessage(NULL, "compicc", CompLogLevelWarn,
             DBG_STRING "Output %s: no oyASSUMED_WEB src_profile",
             DBG_ARGS, ccontext->output_name );

      /* optionally set advanced options from Oyranos */
      if(advanced)
        flags = oyOPTIONATTRIBUTE_ADVANCED;

      oyCompLogMessage( NULL, "compicc", CompLogLevelDebug,
                      DBG_STRING "oyConversion_Correct(///icc_color,%d,0) %s %s",
                      DBG_ARGS, flags, ccontext->output_name,
                      advanced?"advanced":"");
      oyImage_s * image_in = oyImage_Create( GRIDPOINTS,GRIDPOINTS*GRIDPOINTS,
                                             ccontext->clut,
                                             pixel_layout, src_profile, 0 );
      oyImage_s * image_out= oyImage_Create( GRIDPOINTS,GRIDPOINTS*GRIDPOINTS,
                                             ccontext->clut,
                                             pixel_layout, dst_profile, 0 );

      oyProfile_Release( &src_profile );

      oyOptions_SetFromText( &options, "//cmm/any/cached", "1", OY_CREATE_NEW );
      cc = oyConversion_CreateBasicPixels( image_in, image_out, options, 0 );
      if (cc == NULL)
      {
        oyCompLogMessage( NULL, "compicc", CompLogLevelWarn,
                      DBG_STRING "no conversion created for %s",
                      DBG_ARGS, ccontext->output_name);
        goto clean_setupColourTable;
      }
      oyOptions_Release( &options );

      error = oyOptions_SetFromText( &options,
                                     "//"OY_TYPE_STD"/config/display_mode", "1",
                                     OY_CREATE_NEW );
      error = oyConversion_Correct(cc, "//" OY_TYPE_STD "/icc_color", flags, options);
      if(error)
      {
        oyCompLogMessage( NULL, "compicc", CompLogLevelWarn,
                      DBG_STRING "oyConversion_Correct(///icc_color,%d,0) failed %s",
                      DBG_ARGS, flags, ccontext->output_name);
        goto clean_setupColourTable;
      }
      oyOptions_Release( &options );

      oyFilterGraph_s * cc_graph = oyConversion_GetGraph( cc );
      oyFilterNode_s * icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
      oyBlob_s * blob = oyFilterNode_ToBlob( icc, NULL );

      if(!blob)
      {
        oyConversion_Release( &cc );
        oyFilterNode_Release( &icc );

        oyOptions_SetFromText( &options, OY_DEFAULT_CMM_CONTEXT, "lcm2", OY_CREATE_NEW );
        cc = oyConversion_CreateBasicPixels( image_in, image_out, options, 0 );
        if (cc == NULL)
        {
          oyCompLogMessage( NULL, "compicc", CompLogLevelWarn,
                      DBG_STRING "no conversion created for %s",
                      DBG_ARGS, ccontext->output_name);
          goto clean_setupColourTable;
        }
        oyOptions_Release( &options );
        error = oyOptions_SetFromText( &options,
                                     "//"OY_TYPE_STD"/config/display_mode", "1",
                                     OY_CREATE_NEW );
        error = oyConversion_Correct(cc, "//" OY_TYPE_STD "/icc_color", flags, options);
      }

      uint32_t exact_hash_size = 0;
      char * hash_text = 0;
      const char * t = 0;
      {
        t = oyFilterNode_GetText( icc, oyNAME_NAME );
        if(t)
          hash_text = strdup(t);
      }
      oyHash_s * entry;
      oyArray2d_s * clut = NULL;
      oyStructList_s * cache = pluginGetPrivatesCache();
      entry = oyStructList_GetHash( cache, exact_hash_size, hash_text );
      clut = (oyArray2d_s*) oyHash_GetPointer( entry, oyOBJECT_ARRAY2D_S);
      oyFilterNode_Release( &icc );
      oyFilterGraph_Release( &cc_graph );

      oyCompLogMessage( NULL, "compicc", CompLogLevelDebug,
                      DBG_STRING "clut from cache %s %s",
                      DBG_ARGS, clut?"obtained":"", oyNoEmptyString_m_(hash_text) );
      if(clut)
      {
        ptr = (int**)oyArray2d_GetData(clut);
        memcpy( ccontext->clut, ptr[0], 
                sizeof(GLushort) * GRIDPOINTS*GRIDPOINTS*GRIDPOINTS * 3 );
      } else
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

        clut = oyArray2d_Create( NULL, GRIDPOINTS*3, GRIDPOINTS*GRIDPOINTS,
                                 oyUINT16, NULL );

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

      if(hash_text)
      {
        cicc_free(hash_text); hash_text = 0;
      }


      oyOptions_Release( &options );
      oyImage_Release( &image_in );
      oyImage_Release( &image_out );
      oyConversion_Release( &cc );

      cdCreateTexture( ccontext );

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
}

oyTESTRESULT_e testClut ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  oyTestCacheListClear_();

  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  PrivColorContext pc = {
    oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, NULL ), /* the data profile or device link */
    oyProfile_FromFile( "compatibleWithAdobeRGB1998.icc", icc_profile_flags, NULL ), /* the monitor profile or none */
    oyStringCopy( "*TEST*", oyAllocateFunc_ ), /* the intented output device */
  };
  double clck = oyClock();
  setupColourTable( &pc, 0 );
  clck = oyClock() - clck;
  uint16_t c[3] = {pc.clut[12][12][12][0],pc.clut[12][12][12][1],pc.clut[12][12][12][2]};
  fprintf(zout, "compatibleWithAdobeRGB1998.icc %d,%d,%d\n", pc.clut[12][12][12][0],pc.clut[12][12][12][1],pc.clut[12][12][12][2]);

  int count = oyStructList_Count( oy_test_cache_ );
  if( count )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "clut cached            %d %s", count,
                   oyProfilingToString(1,clck/(double)CLOCKS_PER_SEC,"Tex"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "clut cache empty                      " );
  }
  oyProfile_Release( &pc.dst_profile );


  pc.dst_profile = oyProfile_FromFile( "LStar-RGB.icc", icc_profile_flags, NULL );
  setupColourTable( &pc, 0 );
  fprintf(zout, "LStar-RGB.icc %d,%d,%d\n", pc.clut[12][12][12][0],pc.clut[12][12][12][1],pc.clut[12][12][12][2]);
  count = oyStructList_Count( oy_test_cache_ );
  if( !(c[0] == pc.clut[12][12][12][0] || c[1] == pc.clut[12][12][12][1] || c[2] == pc.clut[12][12][12][2]) &&
      count > 1)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "cache difference                    %d", count );
  } else
  { PRINT_SUB( oyTESTRESULT_XFAIL,
    "cache difference                    %d", count );
  }

  return result;
}


oyTESTRESULT_e testRegistrationMatch ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  if( oyFilterRegistrationMatch(OY_INTERNAL "/icc_color.lcms",
                                "//" OY_TYPE_STD "/icc_color",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "simple CMM selection                  " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "simple CMM selection                  " );
  }

  if(!oyFilterRegistrationMatch(OY_INTERNAL "/icc_color.lcms",
                                "//" OY_TYPE_STD "/icc_color.octl",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "simple CMM selection no match         " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "simple CMM selection no match         " );
  }

  if( oyFilterRegistrationMatch(OY_INTERNAL "/icc_color.lcms",
                                "//" OY_TYPE_STD "/icc_color.4+lcms",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "special CMM selection                 " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "special CMM selection                 " );
  }

  if(!oyFilterRegistrationMatch(OY_INTERNAL "/icc_color.lcms",
                                "//" OY_TYPE_STD "/icc_color.4-lcms",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "special CMM avoiding                  " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "special CMM avoiding                  " );
  }

  if( oyFilterRegistrationMatch(OY_INTERNAL "/icc_color.lcms",
                                "//" OY_TYPE_STD "/icc_color.7-lcms",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "special CMM avoiding, other API       " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "special CMM avoiding, other API       " );
  }

  if( oyFilterRegistrationMatch("org/freedesktop/openicc/device/monitor/manufacturer",
                                "org/freedesktop/openicc/device/monitor/model",
                                oyOBJECT_NONE ) == 0)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "device key mismatch                   " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "device key mismatch                   " );
  }

  if( oyFilterRegistrationMatch("org/freedesktop/openicc/device/monitor/manufacturer",
                                "org/freedesktop/openicc/device/monitor/manufacturer",
                                oyOBJECT_NONE ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "device key match                      " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "device key match                      " );
  }

  if( oyFilterRegistrationMatch("org/freedesktop/openicc/device/monitor/manufacturer/short",
                                "org/freedesktop/openicc/device/monitor/manufacturer/full",
                                oyOBJECT_NONE ) == 0)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "long device key mismatch              " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "long device key mismatch              " );
  }

  if( oyFilterRegistrationMatch("org/freedesktop/openicc/device/monitor/manufacturer/short",
                                "org/freedesktop/openicc/device/monitor/manufacturer/short",
                                oyOBJECT_NONE ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "long device key match                 " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "long device key match                 " );
  }

  return result;
}

extern "C" {
int oyTextIccDictMatch( const char *, const char *, double delta ); }

oyTESTRESULT_e test_oyTextIccDictMatch ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  if( oyTextIccDictMatch("ABC",
                         "ABC", 0))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "simple text matching                  " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "simple text matching                  " );
  }

  if(!oyTextIccDictMatch("ABC",
                         "ABCD", 0))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "simple text mismatching               " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "simple text mismatching               " );
  }

  if( oyTextIccDictMatch("abcd,ABC,efgh",
                         "abcdef,12345,ABC", 0))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "multiple text matching                " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "multiple text matching                " );
  }

  if( oyTextIccDictMatch("abcd,ABC,efgh,12345",
                         "abcdef,12345,ABCD", 0.0005))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "multiple integer matching             " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "multiple integer matching             " );
  }

  if(!oyTextIccDictMatch("abcd,ABC,efgh,12345",
                         "abcdef,12345ABCD", 0.0005))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "multiple integer mismatching          " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "multiple integer mismatching          " );
  }

  if( oyTextIccDictMatch("abcd,ABC,efgh,123.45001",
                         "abcdef,123.45,ABCD", 0.0005))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "multiple float matching               " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "multiple float matching               " );
  }

  if(!oyTextIccDictMatch("abcd,ABC,efgh,123.45",
                         "abcdef,123", 0.0005))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "multiple float mismatching            " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "multiple float mismatching            " );
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyPolicyToXML                         " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyPolicyToXML                         " );
  }

  if(xml) {
    oyReadXMLPolicy(oyGROUP_ALL, xml);
    fprintf(zout,"xml text: \n%s", xml);

    data = oyPolicyToXML( oyGROUP_ALL, 1, myAllocFunc );

    if( strcmp( data, xml ) == 0 )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "Policy rereading                      " );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
      "Policy rereading                      " );
    }

#ifdef USE_NEW
    delete [] xml;
    delete [] data;
#else
    free(xml);
    free(data);
#endif
  }


  return result;
}

oyTESTRESULT_e testWidgets ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  const char * name = NULL,
             * tooltip = NULL;
  int          flags = 0;
  const oyGROUP_e * categories = NULL;

  oyWIDGET_e option = oyWIDGET_RENDERING_INTENT;
  oyWIDGET_TYPE_e type = oyWidgetTitleGet( option, 
                                           &categories, &name, &tooltip,
                                           &flags );

  if( name && tooltip )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyWidgetTitleGet \"%s\" %d\n\t%s %d", name, type, tooltip, flags );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyWidgetTitleGet                      " );
  }

  int choices = 0;
  const char ** choices_string_list = NULL;
  int current = -1;
  int error = oyOptionChoicesGet2( option, 0, oyNAME_NAME,
                                   &choices, &choices_string_list,
                                   &current );

  if( !error && choices && choices_string_list && current != -1 )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOptionChoicesGet2 %d [%d]: %s", choices, current, choices_string_list[current] );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptionChoicesGet2                   " );
  }
  const char * description = NULL;
  int choice = current;
  type = oyWidgetDescriptionGet( option, &description, choice );
  if( type == oyWIDGETTYPE_BEHAVIOUR && description )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyWidgetDescriptionGet  %s", description );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyWidgetDescriptionGet                " );
  }

  oyOptionChoicesFree( option, &choices_string_list, choices );

  option = oyWIDGET_ASSUMED_WEB;
  type = oyWidgetTitleGet( option, &categories, &name, &tooltip, &flags );
  if( name && tooltip && (flags & OY_LAYOUT_NO_CHOICES) &&
      type == oyWIDGETTYPE_DEFAULT_PROFILE )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyWidgetTitleGet \"%s\"\n\t%s %d", name, tooltip, flags );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyWidgetTitleGet                      " );
  }

  fprintf( zout, "\n");

  return result;
}

/* forward declaration */
#ifdef __cplusplus
extern "C" {
#endif
char ** oyCMMsGetLibNames_           ( uint32_t          * n );
#ifdef __cplusplus
}
#endif

#include "oyranos_devices.h"

oyTESTRESULT_e testCMMDevicesListing ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

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
  oyConfigDomainList( "//"OY_TYPE_STD"/device/config.icc_profile",
                      &texts, &count, &rank_list ,0 );

  if( count )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfigDomainList Found CMM's %d     ", (int)count );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfigDomainList Found CMM's %d     ", (int)count );
  }
  for( i = 0; i < (int)count; ++i)
  {
    fprintf( zout, "%d: %s\n", i, texts[i] );
  }
  fprintf(zout, "\n" );

  oyConfigs_s * configs = 0;
  oyConfig_s * config = 0;
  oyOptions_s * options_list = 0;
  oyOption_s * o = 0;
  oyOption_s_ * oi = 0;
  oyProfile_s * p = 0;

  /* send a empty query to one module to obtain instructions in a message */
  if(count)
  error = oyConfigs_FromDomain( texts[0], 0, &configs, 0 );
  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfigs_FromDomain \"%s\" help text ", texts ? 
                                              oyNoEmptyString_m_(texts[0]) :"----");
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfigs_FromDomain \"%s\" help text ", texts[0] );
  }
  fprintf( zout, "\n");


  /* add list call to module arguments */
  error = oyOptions_SetFromText( &options_list,
                                 "//" OY_TYPE_STD "/config/command", "list",
                                 OY_CREATE_NEW );
  error = oyOptions_SetFromText( &options_list,
                                 "//" OY_TYPE_STD "/config/icc_profile",
                                 "true", OY_CREATE_NEW );

  fprintf( zout, "oyConfigs_FromDomain() \"list\" call:\n" );
  for( i = 0; i < (int)count; ++i)
  {
    const char * registration_domain = texts[i];
    fprintf(zout,"%d[rank %u]: %s\n", i, (unsigned int)rank_list[i], registration_domain);

    error = oyConfigs_FromDomain( registration_domain,
                                  options_list, &configs, 0 );
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

      fprintf(zout, "--------------------------------------------------------------------------------\n\"%s\":\n", oyConfig_FindString( config, "device_name", 0 ) );
      {
        oyOptions_s * options = 0;
        const char * t = 0;
        oyOptions_SetFromText( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
                         "yes", OY_CREATE_NEW );
        error = oyDeviceGetProfile( config, options, &p );
        oyOptions_Release( &options );
        t = oyProfile_GetText( p, oyNAME_DESCRIPTION);
        fprintf(zout, "oyDeviceGetProfile(): \"%s\"\n", t ? t : "----" );
        oyProfile_Release( &p );
      }

      error = oyConfigs_FromDB( oyConfig_GetRegistration( config ), NULL, &heap, 0 );

      error = oyDeviceSelectSimiliar( config, heap, 0, &dbs );
      precise_count = oyConfigs_Count( dbs );
      oyConfigs_Release( &dbs );

      error = oyDeviceSelectSimiliar( config, heap, 1, &dbs );
      serial_count = oyConfigs_Count( dbs );
      oyConfigs_Release( &dbs );

      error = oyDeviceSelectSimiliar( config, heap, 2, &dbs );
      mnft_count = oyConfigs_Count( dbs );
      oyConfigs_Release( &dbs );

      error = oyDeviceSelectSimiliar( config, heap, 4, &dbs );
      dev_name_count = oyConfigs_Count( dbs );
      oyConfigs_Release( &dbs );

      oyConfigs_Release( &heap );


      fprintf(zout, "\"%s\" has %d precise matches,\n"
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
          fprintf(zout,"  %d::%d::%d \"%s\": \"%s\"\n", i,j,k,
                 oyOption_GetRegistration(o), val?val:"(nix)" );
        }

        if(r) oyDeAllocateFunc_(r); r = 0;
        if(val)
          oyDeAllocateFunc_( val ); val = 0;
        oyOption_Release( &o ); oi = 0;
      }

      oyConfig_Release( &config );
    }

    oyConfigs_Release( &configs );
  }
  fprintf( zout, "\n");
  oyOptions_Release( &options_list );

  fprintf( zout, "\n");

  return result;
}

oyTESTRESULT_e testCMMDevicesDetails ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

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
  oyConfigDomainList( "//"OY_TYPE_STD"/device/config.icc_profile",
                      &texts, &count, &rank_list ,0 );

  if( count )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfigDomainList Found CMM's %d     ", (int)count );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfigDomainList Found CMM's %d     ", (int)count );
  }


  fprintf( zout, "oyConfigs_FromDomain() \"properties\" call:\n" );
  for( i = 0; i < (int)count; ++i)
  {
    const char * registration_domain = texts[i];
    fprintf(zout,"%d[rank %u]: %s\n", i, (unsigned int)rank_list[i], registration_domain);

    /* set a general request */
    error = oyOptions_SetFromText( &options,
                                     "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );
    /* send the query to a module */
    error = oyConfigs_FromDomain( registration_domain,
                                  options, &configs, 0 );
    devices_n = oyConfigs_Count( configs );
    for( l = 0; l < devices_n; ++l )
    {
      /* display results */
      fprintf(zout, "--------------------------------------------------------------------------------\n%s:\n", registration_domain );
      config = oyConfigs_Get( configs, l );

      k_n = oyConfig_Count( config );
      for( k = 0; k < k_n; ++k )
      {
        o = oyConfig_Get( config, k );

        val = oyOption_GetValueText( o, oyAllocateFunc_ );
        fprintf(zout, "  %d::%d %s: \"%s\"\n", l,k, 
                  oyStrrchr_(oyOption_GetRegistration(o),'/')+1, val );

        if(val) oyDeAllocateFunc_( val ); val = 0;
        oyOption_Release( &o );
      }

      o = oyConfig_Find( config, "icc_profile" );
      if(o)
      {
        val = oyOption_GetValueText( o, oyAllocateFunc_ );
        fprintf(zout, "  %d %s: \"%s\"\n", l, 
                oyStrrchr_(oyOption_GetRegistration(o),'/')+1, val );

        if(val) oyDeAllocateFunc_( val ); val = 0;
        oyOption_Release( &o );
      }

        //error = oyConfig_SaveToDB( config );
      oyConfig_Release( &config );
    }

    oyConfigs_Release( &configs );
    oyOptions_Release( &options );
  }

  fprintf( zout, "\n");


  if(texts && texts[0])
  {
    oyConfDomain_s * domain = oyConfDomain_FromReg( texts[0], 0 );
    const char * device_class = oyConfDomain_GetText( domain, "device_class", oyNAME_NICK );
    char * class_key = NULL;
    oyStringAddPrintf( &class_key, oyAllocateFunc_, oyDeAllocateFunc_,
                       OY_STD"/device/%s", device_class );
    config = oyConfig_FromRegistration( class_key, 0 );
    oyFree_m_( class_key );
    oyConfDomain_Release( &domain );
  }
  error = oyConfig_AddDBData( config, "k1", "bla1", OY_CREATE_NEW );
  error = oyConfig_AddDBData( config, "k2", "bla2", OY_CREATE_NEW );
  error = oyConfig_AddDBData( config, "k3", "bla3", OY_CREATE_NEW );

  if( !error  && config && oyOptions_Count(*oyConfig_GetOptions( config,"db") ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfig_AddDBData                    " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfig_AddDBData                    " );
  }

  char * registration = 0;
  if(config)
    registration = oyStringCopy( oyConfig_GetRegistration( config ),
                                  oyAllocateFunc_ );
  error = oyConfig_SaveToDB( config, oySCOPE_USER );

  error = oyConfigs_FromDB( registration, NULL, &configs, 0 );
  count = oyConfigs_Count( configs );
  oyConfigs_Release( &configs );

  if( count > 0 )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfigs_FromDB() %d                  ", (int)count );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfigs_FromDB() failed %d           ", (int)count );
  }


  int32_t rank = 0;
  error = oyConfig_GetDB( config, NULL, &rank );
  const char * key_set_name = oyConfig_FindString( config, "key_set_name", 0 );
  char * key = 0;
  STRING_ADD( key, key_set_name );
  STRING_ADD( key, OY_SLASH );
  STRING_ADD( key, "k1" );
  o = oyConfig_Get( config, 0 );
  oyOption_SetRegistration( o, key );
  oyOption_Release( &o );
  if(key)
    oyDeAllocateFunc_( key ); key = 0;
  error = oyConfig_EraseFromDB( config, oySCOPE_USER );
  oyConfig_Release( &config );

  error = oyConfigs_FromDB( registration, NULL, &configs, 0 );
  i = oyConfigs_Count( configs );
  oyConfigs_Release( &configs );

  if( count - i == 1 )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfig_EraseFromDB() %d/%d            ", (int)count,i );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfig_EraseFromDB() failed %d/%d     ", (int)count,i );
  }

  if(registration)
    oyDeAllocateFunc_( registration ); registration = 0;

  fprintf( zout, "\n");

  return result;
}

oyTESTRESULT_e testCMMRankMap ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif

  int error = 0;
  oyConfig_s * device;
  oyOptions_s * options = 0;
  oyConfigs_s * devices = 0;

  oyOptions_SetFromText( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
                         "yes", OY_CREATE_NEW );
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                   "properties", OY_CREATE_NEW );  
  error = oyDevicesGet( OY_TYPE_STD, "monitor", options, &devices );
  oyOptions_Release( &options );

  int count = oyConfigs_Count( devices ),
      i;

  if( count )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "monitor(s) found               %d     ", (int)count );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "no monitor found               %d     ", (int)count );
  }

  for(i = 0; i < count; ++i)
  {
    char * json_text = 0;
    device = oyConfigs_Get( devices, i );
    oyDeviceToJSON( device, 0, &json_text, malloc );

    if( strlen(json_text) )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "json from device [%d]         %d    ", i, (int)strlen(json_text) );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
    "json from device failed for [%d]      ", i );
    }

    const oyRankMap * map = oyConfig_GetRankMap( device );
    if( map )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "Map from device  [%d]                 ", i );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
    "Map from device failed for [%d]       ", i );
    }

    char * rank_map_text = 0;
    error = oyRankMapToJSON( map, options, &rank_map_text, malloc );
    if( rank_map_text )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "JSON from Map    [%d]          %lu    ", i, (long unsigned int)strlen(rank_map_text) );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
    "JSON from Map failed  [%d]            ", i );
    }

    oyRankMap * rank_map2 = 0;
    error = oyRankMapFromJSON( rank_map_text, options, &rank_map2, malloc );
    if( rank_map2 )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "Map from JSON    [%d]                 ", i );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
    "Map from JSON failed [%d]             ", i );
    }

    oyConfig_Release( &device );


    oyDeviceFromJSON( json_text, 0, &device );
    if(json_text) free( json_text ); json_text = 0;
    oyDeviceToJSON( device, 0, &json_text, malloc );

    if( json_text && strlen(json_text) )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "json from device [%d]         %d    ", i, (int)strlen(json_text) );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
    "json from device failed for %d        ", i );
    }

    oyConfig_Release( &device );
    fprintf( zout, "\n");
  }

  fprintf( zout, "\n");

  return result;
}

oyTESTRESULT_e testCMMMonitorJSON ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

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
  error = oyOptions_SetFromText( &options,
                                     "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );
  error = oyDevicesGet( 0, "monitor", options, &configs );
  if( error <= 0 && configs )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyDeviceGet() \"monitor\"          " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyDeviceGet() \"monitor\"          " );
  }

  clck = oyClock() - clck;
  devices_n = oyConfigs_Count( configs );
  for( i = 0; i < devices_n; ++i )
  {
    char * json_text = 0;
    config = oyConfigs_Get( configs, i );
    error = oyDeviceToJSON( config, 0, &json_text, malloc );
    fprintf(zout, "  %d oyDeviceToJSON():\n%s\n", i,
            json_text?json_text:"---" );

    oyConfig_Release( &config );
    if( !error && json_text )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "oyDeviceToJSON() \"monitor\"       " );
      if(i == 0)
        first_json = strdup(json_text);
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
      "oyDeviceToJSON() \"monitor\"      %d", error );
    }

    if(i == 1)
    {
      if(strcmp(json_text,first_json) != 0)
      { PRINT_SUB( oyTESTRESULT_SUCCESS,
        "found second unique monitor        " );
      } else
      { PRINT_SUB( oyTESTRESULT_FAIL,
        "first and second monitor are equal " );
      }
    }

    error = oyDeviceFromJSON( json_text, 0, &config );
    if( !error && config )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "oyDeviceFromJSON() %d             ", oyConfig_Count(config) );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
      "oyDeviceFromJSON() %d %d           ", oyConfig_Count(config), error );
    }

 
#   ifdef HAVE_X11
    if(i == 0)
      system("xprop -remove _ICC_PROFILE -root; xprop -remove _ICC_DEVICE_PROFILE -root");
#   endif
    oyProfile_s * p = NULL;
    oyOptions_SetFromText( &options,
                  "//" OY_TYPE_STD "/config/command",
                           "list", OY_CREATE_NEW );
    oyOptions_SetFromText( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
                          "yes", OY_CREATE_NEW );
    error = oyDeviceGetProfile( config, options, &p );
    if( p )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "oyDeviceGetProfile(\"list\") \"%s\"", oyProfile_GetText(p,oyNAME_DESCRIPTION) );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
      "oyDeviceGetProfile(\"list\")  %d", error );
    }
    oyProfile_s * list_profile = p; p = NULL;

#   ifdef HAVE_X11
    if(i == 0)
      system("xprop -root -len 4 | grep _ICC");
#   endif

    oyOptions_SetFromText( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.fallback",
                          "yes", OY_CREATE_NEW );
    error = oyDeviceGetProfile( config, options, &p );
    if( p )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "oyDeviceGetProfile(\"fallback\") \"%s\"", oyProfile_GetText(p,oyNAME_DESCRIPTION) );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
      "oyDeviceGetProfile(\"fallback\")    " );
    }
    oyProfile_Release( &p );
    oyConfig_Release( &config );
    oyOptions_Release( &options );



    error = oyDeviceFromJSON( json_text, 0, &config );
    oyOptions_SetFromText( &options,
                  "//" OY_TYPE_STD "/config/command",
                           "properties", OY_CREATE_NEW );
    oyOptions_SetFromText( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
                          "yes", OY_CREATE_NEW );
    error = oyDeviceGetProfile( config, options, &p );
    if( !error && p )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "oyDeviceGetProfile(\"properties\") \"%s\"", oyProfile_GetText(p,oyNAME_DESCRIPTION) );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
      "oyDeviceGetProfile(\"properties\")  %d", error );
    }


    if(oyProfile_Equal(p,list_profile))
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "\"list\" == \"properties\" profile" );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
      "\"list\" == \"properties\" profile" );
    }
    oyProfile_Release( &p );
    oyProfile_Release( &list_profile );


    oyOptions_SetFromText( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.fallback",
                          "yes", OY_CREATE_NEW );
    error = oyDeviceGetProfile( config, options, &p );
    if( p )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "oyDeviceGetProfile(\"fallback\") \"%s\"", oyProfile_GetText(p,oyNAME_DESCRIPTION) );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
      "oyDeviceGetProfile(\"fallback\")    " );
    }


    oyProfileTag_s * tag = oyProfile_GetTagById( p, (icTagSignature)icSigMetaDataTag );
    if( tag )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "oyProfile_GetTagById(icSigMetaDataTag)" );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
      "oyProfile_GetTagById(icSigMetaDataTag)" );
    }

    int32_t texts_n = 0, tag_size = 0;
    char ** texts = oyProfileTag_GetText( tag, &texts_n, NULL, NULL,
                                          &tag_size, oyAllocateFunc_ );
    if( texts_n > 2 && tag_size )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "oyProfileTag_GetText(meta) texts: %d tag size: %d", texts_n, tag_size );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
      "oyProfileTag_GetText(meta) texts: %d tag size: %d", texts_n, tag_size );
    }

    int edid_mnft_count = 0;
    for(int j = 0; j < texts_n; ++j)
    {
      //fprintf( zout, "%s\n", texts[j] );
      if(strstr(texts[j],"EDID_serial") != NULL)
        ++edid_mnft_count;
    }
    // one single key is needed
    // otherwise the monitor is defect or
    // there is key duplication
    if( edid_mnft_count == 1 )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "Found EDID_serial keys in meta tag: %d", edid_mnft_count );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
      "Found EDID_serial keys in meta tag: %d", edid_mnft_count );
    }

    oyConfig_Release( &config );
    oyOptions_Release( &options );
    oyFree_m_( json_text );
  }

  oyConfigs_Release( &configs );
  fprintf( zout, "\n");

  return result;
}


oyTESTRESULT_e testCMMMonitorListing ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyDevicesGet() \"monitor\": %d                      %s", devices_n,
                   oyProfilingToString(1,clck/(double)CLOCKS_PER_SEC,"Obj."));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyDevicesGet() \"monitor\": %d     ", devices_n );
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
    clck = oyClock() - clck;
    if(text)
      fprintf( zout, "  %d oyDeviceProfileFromDB(): %s %s\n", i, text,
                   oyProfilingToString(1,clck/(double)CLOCKS_PER_SEC,"Obj."));
    else
      fprintf( zout, "  %d oyDeviceProfileFromDB(): ---\n", i );

    clck = oyClock();
    error = oyDeviceGetInfo( config, oyNAME_NICK, 0, &text, 0 );
    clck = oyClock() - clck;
    fprintf( zout, "  %d oyDeviceGetInfo)(..oyNAME_NICK..): \"%s\" %s\n",
             i, text? text:"???",
                   oyProfilingToString(1,clck/(double)CLOCKS_PER_SEC,"Obj."));
    clck = oyClock();
    error = oyDeviceGetInfo( config, oyNAME_NAME, 0, &text, 0 );
    clck = oyClock() - clck;
    fprintf( zout, "  %d oyDeviceGetInfo)(..oyNAME_NAME..): \"%s\" %s\n",
             i, text? text:"???",
                   oyProfilingToString(1,clck/(double)CLOCKS_PER_SEC,"Obj."));

    oyConfig_Release( &config );
  }
  oyConfigs_Release( &configs );
  fprintf( zout, "\n");

  error = oyDeviceGet( 0, "monitor", device_name, 0, &config );
  k_n = oyConfig_Count( config );
  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyDeviceGet(..\"monitor\" \"%s\"..) %d     ", device_name, k_n );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyDeviceGet(..\"monitor\" \"%s\"..) %d     ", device_name, k_n );
  }
    for( k = 0; k < k_n; ++k )
    {
      o = oyConfig_Get( config, k );
      val = oyOption_GetValueText( o, oyAllocateFunc_ );

      fprintf(zout, "  %d %s: \"%s\"\n", k, oyOption_GetRegistration(o), val );

      if(val) oyDeAllocateFunc_( val ); val = 0;
      oyOption_Release( &o );
    }
  oyConfig_Release( &config );
  oyConfigs_Release( &configs );
  fprintf( zout, "\n");


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
  char * val = 0;

  error = oyConfigs_FromDB( "//" OY_TYPE_STD "", NULL, &configs, 0 );
  j_n = oyConfigs_Count( configs );
  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfigs_FromDB( \"//" OY_TYPE_STD "\" ) count: %d     ", j_n );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfigs_FromDB( \"//" OY_TYPE_STD "\" ) count: %d     ", j_n );
  }
  for( j = 0; j < j_n; ++j )
  {
    config = oyConfigs_Get( configs, j );

    k_n = oyConfig_Count( config );
    for( k = 0; k < k_n; ++k )
    {
      o = oyConfig_Get( config, k );

      val = oyOption_GetValueText( o, oyAllocateFunc_ );
      fprintf(zout, "  %d::%d %s %s\n", j,k, oyOption_GetRegistration(o), val );

      if(val) oyDeAllocateFunc_( val ); val = 0;
      oyOption_Release( &o );
    }

    oyConfig_Release( &config );
  }

  return result;
}

oyTESTRESULT_e testCMMMonitorModule ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;
  oyConfigs_s * devices = 0;
  oyOptions_s * options = 0;
  const char * t = ":0.100";
  int error = 0;

  fprintf(stdout, "\n" );

  /* non existing display */
  error = oyOptions_SetFromText( &options,
                                 "//" OY_TYPE_STD "/config/device_name",
                                 t, OY_CREATE_NEW );
  /* clean up */
  error = oyOptions_SetFromText( &options,
                                 "//"OY_TYPE_STD"/config/command",
                                 "unset", OY_CREATE_NEW );
  error = oyDevicesGet( OY_TYPE_STD, "monitor", options, &devices );
  oyConfigs_Release( &devices );

  if( error == -1
#ifdef __APPLE__
      ||  error == 0
#endif
    )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyDevicesGet( \"//" OY_TYPE_STD "\", unset, ... ) = %d", error );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyDevicesGet( \"//" OY_TYPE_STD "\", unset, ... ) = %d", error );
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

  fprintf( zout, "load a device ...\n");
  clck = oyClock();
  error = oyDeviceGet( 0, "monitor", device_name, 0, &device );
  clck = oyClock() - clck;
  k_n = oyConfig_Count( device );
  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyDeviceGet(..\"monitor\" \"%s\".. &device) %d %s", device_name, k_n,
                   oyProfilingToString(1,clck/(double)CLOCKS_PER_SEC,"Obj."));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyDeviceGet(..\"monitor\" \"%s\".. &device) %d", device_name, k_n );
  }

  fprintf( zout, "... and search for the devices DB entry ...\n");
  clck = oyClock();
  error = oyConfig_GetDB( device, NULL, &rank );
  clck = oyClock() - clck;
  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfig_GetDB( device )                  %d %s", (int)rank,
                   oyProfilingToString(1,clck/(double)CLOCKS_PER_SEC,"Obj."));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfig_GetDB( device )                    %s",
                   oyProfilingToString(1,clck/(double)CLOCKS_PER_SEC,"Obj."));
  }
  if(device && rank > 0)
  {
    fprintf(zout,"rank: %d\n", (int)rank);
    k_n = oyConfig_Count( device );
    for( k = 0; k < k_n; ++k )
    {
      o = oyConfig_Get( device, k );

      val = oyOption_GetValueText( o, oyAllocateFunc_ );
      fprintf(zout, "  d::%d %s: \"%s\"\n", k,
      strchr(strchr(strchr(strchr(oyOption_GetRegistration(o),'/')+1,'/')+1,'/')+1,'/')+1,
              val );

      if(val) oyDeAllocateFunc_( val ); val = 0;
      oyOption_Release( &o );
    }
    //error = oyConfig_EraseFromDB( config );
  }

  oySetPersistentString( OY_STD "/device/test/#0/system_port", oySCOPE_USER, "TEST-port", "TESTcomment" );
  oySetPersistentString( OY_STD "/device/test/#0/model", oySCOPE_USER, "TEST-model", "TESTcomment" );
  oySetPersistentString( OY_STD "/device/test/#1/system_port", oySCOPE_USER, "TEST-port2", "TESTcomment2" );
  oySetPersistentString( OY_STD "/device/test/#1/model", oySCOPE_USER, "TEST-model2", "TESTcomment2" );
  fprintf(zout, "creating DB device class: \"%s\"\n", OY_STD "/device/test/#[0,1]/system_port: TEST-port/1" );

  const char * reg = OY_STD "/device/test";
  oyConfigs_s * configs = NULL;
  error = oyConfigs_FromDB( reg, NULL, &configs, 0 );
  int count = oyConfigs_Count( configs );
  oyConfig_Release( &device );

  if(count == 2)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfigs_FromDB( %s ) %d", reg, count);
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfigs_FromDB( %s )  ", reg);
  }

  for(k = 0; k < count; ++k)
  {
    oyOptions_s * db;
    device = oyConfigs_Get( configs, k );
    db = *oyConfig_GetOptions(device,"db");
    fprintf(zout, "  d::%d %d: \"%s\"\n", k, oyConfig_Count( device ),
                  oyOptions_GetText( db, oyNAME_NICK ));
    int k_n = oyOptions_Count(db);
    if(k_n == 2)
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "oyConfig_s[%d] = %d                                           ", k, k_n);
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
      "oyConfig_s[%d] = %d                                           ", k, k_n);
    }
  }

  error = oyDBEraseKey_( reg, oySCOPE_USER );

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

oyTESTRESULT_e testCMMsShow ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

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


  fprintf(zout, "\n" );

  texts = oyCMMsGetLibNames_( &count );

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
    cmm_info = (oyCMMinfo_s_*)oyCMMinfoFromLibName_( texts[i] );
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
                for(k = 0; k < (int)apis_n; ++k)
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
                    while(cmm_api8->rank_map[l].key)
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
                  //oyCMMapiFilter_Release( &api );
                }
                oyCMMapiFilters_Release( &apis );
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

    fprintf(zout,"%d: \"%s\": %s\n\n", i, texts[i], text );

  }
  oyStringListRelease_( &texts, count, free );

  if( count )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyCMMsGetLibNames_( ) found %u                     ", (unsigned int)count );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyCMMsGetLibNames_( ) found %u                     ", (unsigned int)count );
  }

  oyDeAllocateFunc_( text_tmp );

  STRING_ADD( t, "     </xf:group>\n</body>\n</html>\n" );
  remove("test2_CMMs.xhtml");
  oyWriteMemToFile2_( "test2_CMMs.xhtml", t, strlen(t),0/*OY_FILE_NAME_SEARCH*/,
                      &rfile, malloc );
  fprintf(zout, "Wrote %s\n", rfile?rfile:"test2_CMMs.xhtml" );
  free( rfile );

  return result;
}

#include <kdb.h>
#ifndef KDB_VERSION_MAJOR
#define KDB_VERSION_MAJOR 0
#endif
#ifndef KDB_VERSION_MINOR
#define KDB_VERSION_MINOR 0
#endif
#define KDB_VERSION_NUM (KDB_VERSION_MAJOR*10000 + KDB_VERSION_MINOR*100)
#ifdef __cplusplus
#define ckdb ckdb::
#else
#define ckdb
#endif
extern ckdb KDB * oy_handle_;

double d[6] = {0.5,0.5,0.5,0,0,0};

#if KDB_VERSION_NUM >= 800
extern "C" {int oyGetKey(ckdb Key*);}
#define dbGetKey(a,b) oyGetKey(b)
#else
#define dbGetKey(a,b) ckdb kdbGetKey(a,b)
#endif

#include "oyFilterCore_s_.h"
#include "oyNamedColor_s.h"
#include "oyNamedColors_s.h"
#include "oyranos_alpha_internal.h"

oyTESTRESULT_e testCMMnmRun ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;
  oyNamedColor_s * c = 0;
  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyProfile_s * prof = oyProfile_FromStd( oyEDITING_XYZ, icc_profile_flags, NULL );
  int error = 0, l_error = 0,
      i,n = 10;

  fprintf(stdout, "\n" );

  fprintf(zout, "clearing caches\n" );
  oyAlphaFinish_(0);

  double clck = oyClock();
  for(i = 0; i < n*10000; ++i)
  {
    c = oyNamedColor_Create( NULL, NULL,0, prof, 0 );
    oyNamedColor_Release( &c );
  }
  clck = oyClock() - clck;

  c = oyNamedColor_Create( NULL, NULL,0, prof, 0 );
  if( c )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyNamedColor_Create( )             %s",
                   oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Obj."));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyNamedColor_Create( )                            " );
  }


  const char * key_name = OY_STD"/behaviour/rendering_bpc";
  clck = oyClock();
  for(i = 0; i < n*3; ++i)
  {
    oyDB_s * db = oyDB_newFrom( key_name, oySCOPE_USER_SYS, oyAllocateFunc_ );
    char * value = oyDB_getString(db, key_name);
    if(!value)
      break;
    oyFree_m_(value);
    oyDB_release( &db );
  }
  clck = oyClock() - clck;

  if( i )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyDB_getString()                   %s",
                  oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "key"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyDB_getString_(%s)", key_name );
  }


  clck = oyClock();
  oyDB_s * db = oyDB_newFrom( key_name, oySCOPE_USER_SYS, oyAllocateFunc_ );
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyDB_getString() shared            %s",
                  oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "key"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyDB_getString_(%s)", key_name );
  }


  char * value = oyGetPersistentString( key_name, 0, oySCOPE_USER_SYS,0 );
  if( (!value && !value_db) ||
      (value && value_db && strcmp(value,value_db) == 0))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "DB and cached values are equal        ");
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyGetPersistentString() cached     %s",
                  oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "key"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyGetPersistentString(%s)", key_name );
  }


  clck = oyClock();
  oyOption_s * option = oyOption_FromRegistration(OY_STD"/behaviour/rendering_bpc", 0);
  oyOption_SetFromText(option, "-1", 0);

  for(i = 0; i < n*3*17; ++i)
  {
    error = oyOption_SetValueFromDB( option );
    if(error > 0) break;
  }
  clck = oyClock() - clck;

  if( i > 1 && error <= 0 )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOption_SetValueFromDB()           %s",
                  oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Opt."));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOption_SetValueFromDB(%s)", oyOption_GetText(option, oyNAME_NICK) );
  }

  clck = oyClock();
  for(i = 0; i < 1; ++i)
  {
    oyOptions_s * options = oyOptions_ForFilter( "//" OY_TYPE_STD "/lcm2",
                                            oyOPTIONATTRIBUTE_ADVANCED  |
                                            oyOPTIONATTRIBUTE_FRONT |
                                            OY_SELECT_COMMON, 0 );
    oyOptions_Release( &options );
  }
  clck = oyClock() - clck;

  if( i )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOptions_ForFilter() first        %s",
                  oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Filter"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptions_ForFilter() first                       " );
  }

  clck = oyClock();
  for(i = 0; i < n; ++i)
  {
    oyOptions_s * options = oyOptions_ForFilter( "//" OY_TYPE_STD "/lcm2",
                                            oyOPTIONATTRIBUTE_ADVANCED  |
                                            oyOPTIONATTRIBUTE_FRONT |
                                            OY_SELECT_COMMON, 0 );
    oyOptions_Release( &options );
  }
  clck = oyClock() - clck;

  if( i )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOptions_ForFilter()              %s",
                  oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Filter"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptions_ForFilter()                             " );
  }



  oyOptions_s * options = oyOptions_New(0);
  clck = oyClock();
  for(i = 0; i < n*10000; ++i)
  {
    oyFilterCore_s * core = oyFilterCore_NewWith( "//" OY_TYPE_STD "/root",
                                                  options,0 );
    if(!core) break;
    oyFilterCore_Release( &core );
  }
  clck = oyClock() - clck;

  if( i )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyFilterCore_New()                  %s",
                 oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Cores"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyFilterCore_New()                                 " );
  }


  clck = oyClock();
  const char * registration = "//" OY_TYPE_STD "/root";
  for(i = 0; i < n*10000; ++i)
  {
    oyCMMapi4_s_ * api4 = 0;
    api4 = (oyCMMapi4_s_*) oyCMMsGetFilterApi_(
                                            registration, oyOBJECT_CMM_API4_S );
    error = !api4;
    if(!(i%30000)) fprintf(zout, "." ); fflush(zout);
  }
  fprintf(zout,"\n");
  clck = oyClock() - clck;

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyFilterCore_New() oyCMMapi4_s      %s",
                 oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Cores"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyFilterCore_New() oyCMMapi4_s                     " );
  }


  oyConversion_s * s = 0;
  oyFilterNode_s * out = 0;
  oyImage_s * input  = NULL,
            * output = NULL;
  double * buf_in = &d[0],
         * buf_out = &d[3];
  oyDATATYPE_e buf_type_in = oyDOUBLE,
               buf_type_out = oyDOUBLE;
  oyProfile_s * p_in = prof,
              * p_out = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, 0 );

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


  clck = oyClock();
  for(i = 0; i < n; ++i)
  if(error <= 0)
  {
    s = oyConversion_CreateBasicPixels( input, output, options, NULL );
    oyConversion_Release( &s );
    //if(!(i%10)) fprintf(zout, "." ); fflush(stdout);
  }
  clck = oyClock() - clck;

  //fprintf(zout,"\n");
  oyImage_Release( &input );
  oyImage_Release( &output );

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConversion_CreateBasicPixels()    %s",
                    oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Obj."));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConversion_CreateBasicPixels()                   " );
  }

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

  #define OY_ERR if(l_error != 0) error = l_error;

  oyFilterPlug_s * plug = 0;
  oyPixelAccess_s * pixel_access = 0;
  s = oyConversion_CreateBasicPixels( input,output, 0, 0 );
  out = oyConversion_GetNode( s, OY_OUTPUT );
  if(s && out)
    plug = oyFilterNode_GetPlug( out, 0 );
  else
    error = 1;
  pixel_access = oyPixelAccess_Create( 0,0, plug,
                                           oyPIXEL_ACCESS_IMAGE, 0 );
  oyFilterPlug_Release( &plug );

  clck = oyClock();
  for(i = 0; i < n*1000; ++i)
  if(error <= 0)
    error  = oyConversion_RunPixels( s, pixel_access );
  clck = oyClock() - clck;

  oyConversion_Release ( &s );
  oyPixelAccess_Release( &pixel_access );

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConversion_RunPixels( oyPixelAcce.%s",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConversion_RunPixels()                           " );
  }


  s = oyConversion_CreateBasicPixels( input,output, options, 0 );
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConversion_RunPixels()          %s",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConversion_RunPixels()                         " );
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

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyNamedColor_SetColorStd()        %s",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyNamedColor_SetColorStd() oyASSUMED_WEB         " );
  }

  p_in = oyProfile_FromStd ( oyASSUMED_WEB, icc_profile_flags, NULL );
  p_out = oyProfile_FromStd ( oyEDITING_XYZ, icc_profile_flags, NULL );

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

  conv   = oyConversion_CreateBasicPixels( in,out, options, 0 );
  if(!error)
  error  = oyConversion_RunPixels( conv, 0 );

  oyConversion_Release( &conv );
  oyImage_Release( &in );
  oyImage_Release( &out );
  }
  clck = oyClock() - clck;


  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyColorConvert_()                  %s",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyColorConvert_()                                 " );
  }


  input  = oyImage_Create( 1,1, 
                         buf_in ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         0 );
  output = oyImage_Create( 1,1, 
                         buf_out ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                          oyDataType_m(buf_type_out),
                         p_out,
                         0 );
  oyConversion_s * conv   = oyConversion_CreateBasicPixels( input,output, 0,0 );

  out = oyConversion_GetNode( conv, OY_OUTPUT );

  /* conversion->out_ has to be linear, so we access only the first plug */
  if(conv)
    plug = oyFilterNode_GetPlug( out, 0 );
  oyFilterNode_Release (&out );

  /* create a very simple pixel iterator as job ticket */
  if(plug)
    pixel_access = oyPixelAccess_Create( 0,0, plug,
                                           oyPIXEL_ACCESS_IMAGE, 0 );
  error  = oyConversion_RunPixels( conv, pixel_access );

  clck = oyClock();
  if(pixel_access)
  for(i = 0; i < n*1000; ++i)
  {
    if(!error)
      error  = oyConversion_RunPixels( conv, pixel_access );
  }
  clck = oyClock() - clck;



  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyColorConvert_() with oyPixelAcce.%s",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyColorConvert_() with oyPixelAccess_Create()     " );
  }

  clck = oyClock();
  d[0] = d[1] = d[2] = 1.0;
  d[3] = d[4] = d[5] = 0.0;
  if(pixel_access)
  for(i = 0; i < n*1000; ++i)
  {
    if(!error)
      error = oyConversion_GetOnePixel( conv, 0,0, pixel_access );
  }
  clck = oyClock() - clck;
  oyConversion_Release( &conv );
  oyPixelAccess_Release( &pixel_access );

  if( !error  && d[3] != 0.0 )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConversion_GetOnePixel( oyPix. )  %s %.02g %.02g %.02g",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"), d[3], d[4], d[5]);
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConversion_GetOnePixel( oyPix. )  %s %.02g %.02g %.02g" ,
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"), d[3], d[4], d[5]);
  }


  conv = oyConversion_New( 0 );
  oyFilterNode_s * in_node = oyFilterNode_NewWith( "//" OY_TYPE_STD "/root", 0, 0 );
  oyConversion_Set( conv, in_node, 0 );
  oyFilterNode_SetData( in_node, (oyStruct_s*)input, 0, 0 );
  oyFilterNode_s * out_node = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", 0, 0 );
  /*oyFilterNode_SetData( out_node, (oyStruct_s*)output, 0, 0 );*/
  error = oyFilterNode_Connect( in_node, "//" OY_TYPE_STD "/data",
                                out_node, "//" OY_TYPE_STD "/data", 0 );
  oyConversion_Set( conv, 0, out_node );
  oyConversion_GetNode( conv, OY_OUTPUT );
  plug = oyFilterNode_GetPlug( out, 0 );
  oyFilterNode_Release (&out );

  /* create a very simple pixel iterator as job ticket */
  if(plug)
    pixel_access = oyPixelAccess_Create( 0,0, plug,
                                           oyPIXEL_ACCESS_IMAGE, 0 );
  error  = oyConversion_RunPixels( conv, pixel_access );

  clck = oyClock();
  for(i = 0; i < n*1000; ++i)
  {
    if(!error)
      error = oyConversion_RunPixels( conv, pixel_access );
  }
  clck = oyClock() - clck;

  oyImage_Release( &input );
  oyImage_Release( &output );

  oyPixelAccess_Release( &pixel_access );
  oyConversion_Release( &conv );


  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConversion_RunPixels (2 nodes)    %s",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConversion_RunPixels (2 nodes)                   " );
  }


  conv = oyConversion_CreateBasicPixelsFromBuffers(
                                        p_in, buf_in, oyDataType_m(buf_type_in),
                                        p_out, buf_out, oyDataType_m(buf_type_out),
                                        0, 1 );
  oyConversion_Release( &conv );



  oyProfile_s * p_cmyk = oyProfile_FromStd( oyEDITING_CMYK, icc_profile_flags, NULL );
  oyNamedColor_s * ncl = 0;
  oyNamedColors_s * colors = oyNamedColors_New(0);

  oyNamedColors_SetPrefix( colors, "test" );
  oyNamedColors_SetSuffix( colors, "color" );

  clck = oyClock();
  for( i = 0; i < 10*100; ++i )
  {
    char name[12];
    double lab[3], device[4] = {0.2,0.2,0.1,0.5};

    lab[0] = i*0.001;
    lab[1] = lab[2] = 0.5;
    sprintf( name, "%d", i );

    ncl = oyNamedColor_CreateWithName( name, name, name, NULL, NULL, NULL, 0, p_cmyk, NULL );

    if(!error)
      error = oyNamedColor_SetColorStd( ncl, oyEDITING_LAB, lab, oyDOUBLE, 0, NULL );
    if(!error)
      oyNamedColor_SetChannels( ncl, device, 0 );

    oyNamedColors_MoveIn( colors, &ncl, i );
  }
  clck = oyClock() - clck;

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyNamedColor_CreateWithName()     %s",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Ncl"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyNamedColor_CreateWithName()            " );
  }

  
  return result;
}

#include "oyRectangle_s_.h"

oyTESTRESULT_e testImagePixel()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;
  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyProfile_s * p_lab = oyProfile_FromStd( oyEDITING_LAB, icc_profile_flags, NULL );
  oyProfile_s * p_web = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, NULL );
  oyProfile_s /** p_cmyk = oyProfile_FromStd( oyEDITING_CMYK, NULL ),*/
              * p_in, * p_out;
  int error = 0,
      i,n = 10;
  uint16_t buf_16in2x2[12] = {
  20000,20000,20000, 10000,10000,10000,
  0,0,0,             65535,65535,65535
  };
  uint16_t buf_16out2x2[12];
  oyDATATYPE_e buf_type_in = oyUINT16,
               buf_type_out = oyUINT16;
  oyImage_s *input, *output;

  fprintf(stdout, "\n" );

  double clck = oyClock();
  p_in = p_web;
  p_out = p_lab;
  input =oyImage_Create( 2,2, 
                         buf_16in2x2,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         0 );
  output=oyImage_Create( 2,2, 
                         buf_16out2x2,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                          oyDataType_m(buf_type_out),
                         p_out,
                         0 );

  oyFilterPlug_s * plug = 0;
  oyPixelAccess_s * pixel_access = 0;
  oyConversion_s * cc;
  memset( buf_16out2x2, 0, sizeof(uint16_t)*12 );
  cc = oyConversion_CreateBasicPixels( input,output, 0, 0 );
  oyFilterNode_s * out = oyConversion_GetNode( cc, OY_OUTPUT );
  if(cc && out)
    plug = oyFilterNode_GetPlug( oyConversion_GetNode( cc, OY_OUTPUT), 0 );
  else
    error = 1;
  pixel_access = oyPixelAccess_Create( 0,0, plug,
                                           oyPIXEL_ACCESS_IMAGE, 0 );
  oyFilterPlug_Release( &plug );

  error  = oyConversion_RunPixels( cc, pixel_access );
  clck = oyClock();
  for(i = 0; i < n*1000; ++i)
  if(error <= 0)
  {
    error  = oyConversion_RunPixels( cc, pixel_access );
  }
  clck = oyClock() - clck;

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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "Plain Image                              %s",
                          oyProfilingToString(4*i,clck/(double)CLOCKS_PER_SEC, "Pixel"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "Plain Image                                        " );
  }

  fprintf(zout, "input:  %d,%d,%d %d,%d,%d\n        %d,%d,%d %d,%d,%d\n",
                  buf_16in2x2[0], buf_16in2x2[1], buf_16in2x2[2],
                  buf_16in2x2[3], buf_16in2x2[4], buf_16in2x2[5],
                  buf_16in2x2[6], buf_16in2x2[7], buf_16in2x2[8],
                  buf_16in2x2[9], buf_16in2x2[10], buf_16in2x2[11] );
  fprintf(zout, "output: %d,%d,%d %d,%d,%d\n        %d,%d,%d %d,%d,%d\n",
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
    oyRectangle_s * r = oyPixelAccess_GetOutputROI( pixel_access );
    (*oyRectangle_SetGeo1(r,2)) *= 0.5;
    (*oyRectangle_SetGeo1(r,3)) *= 0.5;
    oyPixelAccess_ChangeRectangle( pixel_access, 0.5,0.5, r );
  }
  clck = oyClock();
  for(i = 0; i < n*1000; ++i)
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "lower right source pixel in 1 pixel RoI  %s",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "lower right source pixel in 1 pixel RoI            " );
  }

  fprintf(zout, "input:  %d,%d,%d %d,%d,%d\n        %d,%d,%d %d,%d,%d\n",
                  buf_16in2x2[0], buf_16in2x2[1], buf_16in2x2[2],
                  buf_16in2x2[3], buf_16in2x2[4], buf_16in2x2[5],
                  buf_16in2x2[6], buf_16in2x2[7], buf_16in2x2[8],
                  buf_16in2x2[9], buf_16in2x2[10], buf_16in2x2[11] );
  fprintf(zout, "output: %d,%d,%d %d,%d,%d\n        %d,%d,%d %d,%d,%d\n",
                  buf_16out2x2[0], buf_16out2x2[1], buf_16out2x2[2],
                  buf_16out2x2[3], buf_16out2x2[4], buf_16out2x2[5],
                  buf_16out2x2[6], buf_16out2x2[7], buf_16out2x2[8],
                  buf_16out2x2[9], buf_16out2x2[10], buf_16out2x2[11] );


  buf_16in2x2[0]=buf_16in2x2[1]=buf_16in2x2[2]=20000;
  buf_16in2x2[3]=buf_16in2x2[4]=buf_16in2x2[5]=10000;
  buf_16in2x2[6]=buf_16in2x2[7]=buf_16in2x2[8]=0;
  buf_16in2x2[9]=buf_16in2x2[10]=buf_16in2x2[11]=65535;
  memset( buf_16out2x2, 0, sizeof(uint16_t)*12 );
  if(pixel_access)
  {
    oyRectangle_s * r = oyPixelAccess_GetOutputROI( pixel_access );
    oyRectangle_SetGeo(r, 0.5,0.5, 0.5,0.5);
    oyPixelAccess_ChangeRectangle( pixel_access, 0.5,0.5, r );
  }
  clck = oyClock();
  for(i = 0; i < n*1000; ++i)
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "lower right source in lower right output %s",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "lower right source in lower right output           " );
  }

  fprintf(zout, "input:  %d,%d,%d %d,%d,%d\n        %d,%d,%d %d,%d,%d\n",
                  buf_16in2x2[0], buf_16in2x2[1], buf_16in2x2[2],
                  buf_16in2x2[3], buf_16in2x2[4], buf_16in2x2[5],
                  buf_16in2x2[6], buf_16in2x2[7], buf_16in2x2[8],
                  buf_16in2x2[9], buf_16in2x2[10], buf_16in2x2[11] );
  fprintf(zout, "output: %d,%d,%d %d,%d,%d\n        %d,%d,%d %d,%d,%d\n",
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
                                      0 );

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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyImage_FillArray() keep allocation                 " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyImage_FillArray() keep alloc 0x%lo 0x%lo",
    (unsigned long)rows_u16[0], (unsigned long)buf_16out2x2 );
  }

  if(!error &&
     output_u16[0] == 65535 && output_u16[1] == 65535 && output_u16[2] == 65535)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyImage_FillArray() place array data                " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyImage_FillArray() place array data                " );
  }

  /* set lower right pixel */
  output_u16[0] = output_u16[1] = output_u16[2] = 2;

  fprintf(zout, "output: %d,%d,%d %d,%d,%d\n        %d,%d,%d %d,%d,%d\n",
                  buf_16out2x2[0], buf_16out2x2[1], buf_16out2x2[2],
                  buf_16out2x2[3], buf_16out2x2[4], buf_16out2x2[5],
                  buf_16out2x2[6], buf_16out2x2[7], buf_16out2x2[8],
                  buf_16out2x2[9], buf_16out2x2[10], buf_16out2x2[11] );

  oyRectangle_SetGeo( a_roi, 0.0,0.0,0.5,0.5 );
  error = oyImage_ReadArray( output, roi,
                             a, a_roi );


  rows_u16 = (uint16_t**)oyArray2d_GetData(a);
  if(!error &&
     rows_u16[0] != &buf_16out2x2[9] &&
     buf_16out2x2[9]==2&& buf_16out2x2[10]==2&& buf_16out2x2[11]==2)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyImage_ReadArray( array_roi )                      " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyImage_ReadArray( array_roi )                      " );
  }

  /* set lower right pixel */
  output_u16[0] = output_u16[1] = output_u16[2] = 3;
  error = oyImage_ReadArray( output, roi,
                             a, 0 );


  rows_u16 = (uint16_t**)oyArray2d_GetData(a);
  if(!error &&
     rows_u16[0] != &buf_16out2x2[9] &&
     buf_16out2x2[9]==3&& buf_16out2x2[10]==3&& buf_16out2x2[11]==3)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyImage_ReadArray()                                 " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyImage_ReadArray()                                 " );
  }

  fprintf(zout, "output: %d,%d,%d %d,%d,%d\n        %d,%d,%d %d,%d,%d\n",
                  buf_16out2x2[0], buf_16out2x2[1], buf_16out2x2[2],
                  buf_16out2x2[3], buf_16out2x2[4], buf_16out2x2[5],
                  buf_16out2x2[6], buf_16out2x2[7], buf_16out2x2[8],
                  buf_16out2x2[9], buf_16out2x2[10], buf_16out2x2[11] );

  /* move the focus to the top left */
  oyRectangle_s_ array_roi_pix = {oyOBJECT_RECTANGLE_S,0,0,0, 0,0,3,1};
  oyArray2d_SetFocus( a, (oyRectangle_s*)&array_roi_pix );


  rows_u16 = (uint16_t**)oyArray2d_GetData(a);
  if(!error &&
     rows_u16[0] != output_u16)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyArray2d_SetFocus() change array                   " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyArray2d_SetFocus()                                " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyArray2d_SetFocus()                                " );
  }

  oyArray2d_Release( &a );

  oyRectangle_SetGeo( a_roi, 0.5,0.5,0.5,0.5 );
  error = oyImage_FillArray( output, roi, 0,
                             &a, a_roi, 0 );

  rows_u16 = (uint16_t**)oyArray2d_GetData(a);
  if(!error &&
     rows_u16[0] == &buf_16out2x2[9])
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyImage_FillArray() assigment                       " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyImage_FillArray() assigment 0x%lo 0x%lo",
    (unsigned long)rows_u16[0], (unsigned long)&buf_16out2x2[9] );
  }

  oyArray2d_Release( &a );
  oyImage_Release( &input );
  oyImage_Release( &output );

  return result;
}

oyTESTRESULT_e testFilterNodeCMM( oyTESTRESULT_e result_,
                                  const char * reg_pattern )
{
  oyTESTRESULT_e result = result_;
  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyProfile_s * p_lab = oyProfile_FromFile( "compatibleWithAdobeRGB1998.icc", icc_profile_flags, NULL );
  oyProfile_s * p_web = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, NULL );
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
  oyOptions_SetFromText( &options, "////rendering_intent", "3", OY_CREATE_NEW );
  oyOptions_SetFromText( &options, "////context", reg_pattern, OY_CREATE_NEW );

  oyConversion_s * cc = oyConversion_CreateBasicPixelsFromBuffers(
                              p_in, buf_16in2x2, oyDataType_m(buf_type_in),
                              p_out, buf_16out2x2, oyDataType_m(buf_type_out),
                                                    options, 4 );

  oyFilterGraph_s * cc_graph = oyConversion_GetGraph( cc );
  oyFilterNode_s * icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
  oyOptions_s * node_opts = oyFilterNode_GetOptions( icc, oyOPTIONATTRIBUTE_ADVANCED );
  int count = oyOptions_Count( node_opts );
  oyOption_s * o = oyOptions_Find( node_opts, "rendering_intent", oyNAME_PATTERN );
  oyOptions_Release( &node_opts );
  if(o)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOptions_Find( node_opts )  %s(%d)", oyOption_GetText(o, oyNAME_NICK), count );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptions_Find( node_opts )  %s(%d)", oyOption_GetText(o, oyNAME_NICK), count );
  }

  oyBlob_s * blob = oyFilterNode_ToBlob( icc, NULL );
  oyProfile_s * p = oyProfile_FromMem( oyBlob_GetSize( blob ),
                                       oyBlob_GetPointer( blob ), 0,0 );
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
    char ** ts = oyStringSplit_( texts[0], '\n', &n, oyAllocateFunc_ );
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
        { PRINT_SUB( oyTESTRESULT_SYSERROR,
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
        { PRINT_SUB( oyTESTRESULT_SUCCESS,
          "DL's\"%s\" equals node %s %s", sig, t, ot );
        } else
        { PRINT_SUB( oyTESTRESULT_FAIL,
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOptions_Find( node_opts ) after DL %s", oyOption_GetText(o, oyNAME_NICK) );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOptions_Find( node_opts ) unique rendering_intent" );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptions_Find( node_opts ) unique rendering_intent" );
  }

  o = oyOptions_Find( node_opts, "rendering_bpc", oyNAME_PATTERN );
  if(o)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOptions_Find( node_opts ) other default %s", oyOption_GetText(o, oyNAME_NICK) );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptions_Find( node_opts ) other default %s", oyOption_GetText(o, oyNAME_NICK) );
  }
  oyOption_Release( &o );

  oyOptions_Release( &options );
  oyOptions_Release( &node_opts );
  oyConversion_Release( &cc );

  return result;
}

oyTESTRESULT_e testFilterNode()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  char ** list = oyGetCMMs( oyCMM_CONTEXT, oyNAME_NAME, 0, malloc );
  int i = 0;

  while(list && list[i])
  {
    char * reg = oyCMMNameToRegistration( list[i], oyCMM_CONTEXT, oyNAME_NAME, 0, malloc );
    char * reg_pattern = oyCMMRegistrationToName( reg, oyCMM_CONTEXT, oyNAME_PATTERN, 0, malloc );
    fprintf(zout, "Testing ICC CMM[%d]: \"%s\"\n", i, list[i] );

    oyTESTRESULT_e result_ = testFilterNodeCMM( result, reg_pattern );
    if(result_ < result)
      result = result_;

    fprintf(stdout, "\n" );
    ++i;
  }

  return result;
}

oyTESTRESULT_e testConversion()
{
  fprintf(stdout, "\n" );

  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;
  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyProfile_s * p_in = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, NULL );
  oyProfile_s * p_out = oyProfile_FromFile( "compatibleWithAdobeRGB1998.icc", icc_profile_flags, NULL );
  uint16_t buf_16in2x2[12] = {
  20000,20000,20000, 10000,10000,10000,
  0,0,0,             65535,65535,65535
  };
  uint16_t buf_16out2x2[12];
  oyDATATYPE_e buf_type_in = oyUINT16,
               buf_type_out = oyUINT16;
  oyImage_s *input, *output;


  input =oyImage_Create( 2,2, 
                         buf_16in2x2,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         0 );
  output=oyImage_Create( 2,2, 
                         buf_16out2x2,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                          oyDataType_m(buf_type_out),
                         p_out,
                         0 );
  oyProfile_Release( &p_in );
  oyProfile_Release( &p_out );

  oyOptions_s * options = NULL;

  oyOptions_SetFromText( &options, "////cached", "1", OY_CREATE_NEW );
  oyConversion_s * cc = oyConversion_CreateBasicPixels( input,output, options, 0 );
  oyFilterGraph_s * cc_graph = oyConversion_GetGraph( cc );
  oyFilterNode_s * icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
  char * config_cmm = oyGetPersistentString( OY_DEFAULT_CMM_CONTEXT, 0, oySCOPE_USER_SYS, 0 );
  const char * reg = oyFilterNode_GetRegistration( icc );
  oyOptions_s * node_opts = oyFilterNode_GetOptions( icc, oyOPTIONATTRIBUTE_ADVANCED );
  oyOption_s * ct = oyOptions_Find( node_opts, "////context", oyNAME_PATTERN );
  oyOptions_Release( &node_opts );
  fprintf( zout, "context global: %s\ncontext node: %s\n", config_cmm, strrchr( reg, '/')+1 );
  reg = oyOption_GetValueString( ct, 0 );
  if((reg = strrchr( reg, '/')) != NULL) ++reg;
  fprintf( zout, "context option: %s\n", oyNoEmptyString_m_(reg) );

  if(ct)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOptions_Find( node_opts, \"////context\" )    " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptions_Find( node_opts, \"////context\" )    " );
  }
  oyOption_Release( &ct );

  oyBlob_s * blob = oyFilterNode_ToBlob( icc, NULL );
  if(blob)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConversion_CreateBasicPixels( \"cached\"=\"1\" )" );
  } else
  { PRINT_SUB( oyTESTRESULT_XFAIL,
    "oyConversion_CreateBasicPixels( \"cached\"=\"1\" )" );
  }
  oyBlob_Release( &blob );

  node_opts = oyFilterNode_GetOptions( icc, 0 );
  ct = oyOptions_Find( node_opts, "////context", oyNAME_PATTERN );
  oyOptions_Release( &node_opts );
  if(ct)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOptions_Find( node_opts, \"////context\" )    " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptions_Find( node_opts, \"////context\" )    " );
  }
  oyOption_Release( &ct );

  oyOptions_Release( &options );
  oyConversion_Release( &cc );
  oyFilterGraph_Release( &cc_graph );
  oyFilterNode_Release( &icc );

  oyOptions_SetFromText( &options, "////context", "lcm2", OY_CREATE_NEW );
  cc = oyConversion_CreateBasicPixels( input,output, options, 0 );
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "node = option : %s ~ %s", node_reg, oyNoEmptyString_m_(reg) );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "node = option : %s ~ %s", node_reg, oyNoEmptyString_m_(reg) );
  }
  
  reg = oyFilterNode_GetRegistration( icc );
  if(reg && strstr(reg, "lcm2"))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConversion_CreateBasicPixels( \"context\"=\"lcm2\" )" );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConversion_CreateBasicPixels( \"context\"=\"lcm2\" ) %s", oyNoEmptyString_m_(reg) );
  }

  blob = oyFilterNode_ToBlob( icc, NULL );
  if(blob)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyFilterNode_ToBlob( \"lcm2\" )           %d  ", (int)oyBlob_GetSize(blob) );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyFilterNode_ToBlob( \"lcm2\" )           %d  ", (int)oyBlob_GetSize(blob) );
  }

  oyBlob_Release( &blob );
  oyOptions_Release( &options );
  oyConversion_Release( &cc );
  oyFilterGraph_Release( &cc_graph );
  oyFilterNode_Release( &icc );

  oyOptions_SetFromText( &options, "////renderer", "lcms", OY_CREATE_NEW );
  oyOptions_SetFromText( &options, "////context",  "lcm2", OY_CREATE_NEW );
  cc = oyConversion_CreateBasicPixels( input,output, options, 0 );
  cc_graph = oyConversion_GetGraph( cc );
  icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
  reg = oyFilterNode_GetRendererRegistration( icc );

  if(reg && strstr(reg, "lcms"))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConversion_CreateBasicPixels( \"renderer\"=\"lcms\" )" );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConversion_CreateBasicPixels( \"renderer\"=\"lcms\" ) %s", oyNoEmptyString_m_(reg) );
    fprintf( zout, "\tnode reg = %s\n", oyFilterNode_GetRegistration( icc ));
  }

  oyOptions_Release( &options );

  options = oyFilterNode_GetOptions( icc, oyOPTIONATTRIBUTE_ADVANCED );
  oyOptions_SetFromText( &options, "////renderer", "lcm2", OY_CREATE_NEW );
  oyOptions_SetFromText( &options, "////context", "lcms", OY_CREATE_NEW );
  blob = oyFilterNode_ToBlob( icc, NULL );

  reg = oyFilterNode_GetRegistration( icc );
  if(reg && strstr(reg, "lcms"))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyFilterNode_SetContext_( \"context\"=\"lcms\" ) %d", (int)oyBlob_GetSize(blob) );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyFilterNode_SetContext_( \"context\"=\"lcms\" ) %s  ", oyNoEmptyString_m_(reg) );
  }
  oyBlob_Release( &blob );

  reg = oyFilterNode_GetRendererRegistration( icc );
  if(reg && strstr(reg, "lcm2"))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyFilterNode_SetContext_( \"renderer\"=\"lcm2\" )" );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyFilterNode_SetContext_( \"renderer\"=\"lcm2\" ) %s", oyNoEmptyString_m_(reg) );
  }

  oyOptions_Release( &options );
  oyConversion_Release( &cc );
  oyFilterGraph_Release( &cc_graph );
  oyFilterNode_Release( &icc );


  config_cmm = oyGetPersistentString( OY_DEFAULT_CMM_CONTEXT, 0, oySCOPE_USER_SYS, 0 );
  int error = oySetPersistentString( OY_DEFAULT_CMM_CONTEXT, oySCOPE_USER,
                                     "///icc_color.notX", "non existent CMM" );
  char * test_config_cmm = oyGetPersistentString( OY_DEFAULT_CMM_CONTEXT, 0, oySCOPE_USER_SYS, 0 );
  if(strcmp(test_config_cmm,"///icc_color.notX") == 0)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "set intermediate global context = %s", oyNoEmptyString_m_(test_config_cmm) );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "set intermediate global context = %s", oyNoEmptyString_m_(test_config_cmm) );
  }
  cc = oyConversion_CreateBasicPixels( input,output, options, 0 );
  cc_graph = oyConversion_GetGraph( cc );
  icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
  reg = oyFilterNode_GetRegistration( icc );
  blob = oyFilterNode_ToBlob( icc, NULL );
  if(blob)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "f: %s", oyNoEmptyString_m_(reg) );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "f: %s", oyNoEmptyString_m_(reg) );
  }
  error  = oyConversion_RunPixels( cc, NULL );
  if(error == 0)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "fallback rendering with blob %d               ", (int)oyBlob_GetSize(blob) );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "fallback rendering with blob %d               ", (int)oyBlob_GetSize(blob) );
  }

  oyBlob_Release( &blob );
  oyConversion_Release( &cc );
  oyFilterGraph_Release( &cc_graph );
  oyFilterNode_Release( &icc );

  int i;
  for(i = 0; i < 12; ++i) buf_16out2x2[i] = 0;
  cc = oyConversion_CreateBasicPixels( input,output, options, 0 );
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "fallback rendering %s %s", context, strrchr(reg,'/')+1 );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "fallback rendering %s %s", context, strrchr(reg,'/')+1 );
  }

  ct = oyOptions_Find( node_opts, "////context", oyNAME_PATTERN );
  reg = oyOption_GetValueString( ct, 0 );
  if((reg = strrchr( reg, '/')) != NULL) ++reg;
  if(ct)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOptions_Find( node_opts, \"////context\" ) %s", oyNoEmptyString_m_(reg) );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptions_Find( node_opts, \"////context\" )    " );
  }

  if((oyOption_GetFlags( ct ) & oyOPTIONATTRIBUTE_EDIT) == 0)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "\"////context\" is not touched oyOPTIONATTRIBUTE_EDIT" );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "\"////context\" is not touched oyOPTIONATTRIBUTE_EDIT" );
  }
  oyOption_Release( &ct );
  oyConversion_Release( &cc );
  oyFilterGraph_Release( &cc_graph );
  oyFilterNode_Release( &icc );
  oyOptions_Release( &node_opts );

  oyOptions_SetFromText( &options, "////context", test_config_cmm, OY_CREATE_NEW );
  cc = oyConversion_CreateBasicPixels( input,output, options, 0 );
  cc_graph = oyConversion_GetGraph( cc );
  icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
  node_opts = oyFilterNode_GetOptions( icc, oyOPTIONATTRIBUTE_ADVANCED );
  ct = oyOptions_Find( node_opts, "////context", oyNAME_PATTERN );
  if(!icc || (oyOption_GetFlags( ct ) & oyOPTIONATTRIBUTE_EDIT) != 0)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "\"////context\" is touched oyOPTIONATTRIBUTE_EDIT %s", reg );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "\"////context\" is touched oyOPTIONATTRIBUTE_EDIT %s", reg );
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "explicite no fallback rendering %s %s", oyNoEmptyString_m_(context), oyNoEmptyString_m_(reg) );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "explicite no fallback rendering %s %s", oyNoEmptyString_m_(context), oyNoEmptyString_m_(reg) );
  }

  ct = oyOptions_Find( node_opts, "////context", oyNAME_PATTERN );
  reg = oyOption_GetValueString( ct, 0 );
  if(reg && (reg = strrchr( reg, '/')) != NULL) ++reg;
  if(!reg || (oyOption_GetFlags( ct ) & oyOPTIONATTRIBUTE_EDIT) != 0)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "\"////context\" is touched oyOPTIONATTRIBUTE_EDIT %s", oyNoEmptyString_m_(reg) );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "\"////context\" is touched oyOPTIONATTRIBUTE_EDIT %s", oyNoEmptyString_m_(reg) );
  }
  oyOption_Release( &ct );

  oyConversion_Release( &cc );
  oyFilterGraph_Release( &cc_graph );
  oyFilterNode_Release( &icc );
  oyOptions_Release( &node_opts );

  oyOptions_SetFromText( &options, "////renderer", test_config_cmm, OY_CREATE_NEW );
  cc = oyConversion_CreateBasicPixels( input,output, options, 0 );
  cc_graph = oyConversion_GetGraph( cc );
  icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
  node_opts = oyFilterNode_GetOptions( icc, oyOPTIONATTRIBUTE_ADVANCED );
  ct = oyOptions_Find( node_opts, "////renderer", oyNAME_PATTERN );
  if(!icc || (oyOption_GetFlags( ct ) & oyOPTIONATTRIBUTE_EDIT) != 0)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "\"////renderer\" is touched oyOPTIONATTRIBUTE_EDIT %s", reg );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "\"////renderer\" is touched oyOPTIONATTRIBUTE_EDIT %s", reg );
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "explicite no fallback rendering %s %s", oyNoEmptyString_m_(renderer), oyNoEmptyString_m_(reg) );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "explicite no fallback rendering %s %s", oyNoEmptyString_m_(renderer), oyNoEmptyString_m_(reg) );
  }

  ct = oyOptions_Find( node_opts, "////renderer", oyNAME_PATTERN );
  reg = oyOption_GetValueString( ct, 0 );
  if(reg && (reg = strrchr( reg, '/')) != NULL) ++reg;
  if(!reg || (oyOption_GetFlags( ct ) & oyOPTIONATTRIBUTE_EDIT) != 0)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "\"////renderer\" is touched oyOPTIONATTRIBUTE_EDIT %s", oyNoEmptyString_m_(reg) );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "\"////renderer\" is touched oyOPTIONATTRIBUTE_EDIT %s", oyNoEmptyString_m_(reg) );
  }
  oyOption_Release( &ct );

  oyConversion_Release( &cc );
  oyFilterGraph_Release( &cc_graph );
  oyFilterNode_Release( &icc );
  oyOptions_Release( &node_opts );

  if(config_cmm)
    error = oySetPersistentString( OY_DEFAULT_CMM_CONTEXT, oySCOPE_USER,
                                   config_cmm, NULL );
  else
    error = oySetPersistentString( OY_DEFAULT_CMM_CONTEXT, oySCOPE_USER,
                                   NULL, NULL );
  oyFree_m_( config_cmm );
  oyFree_m_( test_config_cmm );

  oyImage_Release( &input );
  oyImage_Release( &output );

  return result;
}

oyTESTRESULT_e testCMMlists()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  char ** list = oyGetCMMs( oyCMM_CONTEXT, oyNAME_NAME, 0, malloc );
  int i = 0;

  while(list && list[i])
  {
    fprintf(zout, "  %d: \"%s\" (%s)\n", i, list[i], oyCMMNameToRegistration( list[i], oyCMM_CONTEXT, oyNAME_NAME, 0, malloc ) );
    ++i;
  }

  if(list)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyGetCMMs(oyCMM_CONTEXT, oyNAME_NAME,) fine %d  ", i );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyGetCMMs(oyCMM_CONTEXT, oyNAME_NAME,) failed   " );
  }

  list = oyGetCMMs( oyCMM_CONTEXT, oyNAME_REGISTRATION, 0, malloc );
  i = 0;

  while(list && list[i])
  {
    fprintf(zout, "  %d: \"%s\" (%s)\n", i, list[i], oyCMMRegistrationToName(list[i], oyCMM_CONTEXT, oyNAME_NAME, 0, malloc) );
    ++i;
  }

  if(list)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyGetCMMs(oyCMM_CONTEXT, oyNAME_REGISTRATION,) fine %d", i );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyGetCMMs(oyCMM_CONTEXT, oyNAME_REGISTRATION,) failed " );
  }

  list = oyGetCMMs( oyCMM_CONTEXT, oyNAME_NICK, 0, malloc );
  i = 0;

  while(list && list[i])
  {
    fprintf(zout, "  %d: \"%s\" (%s)\n", i, list[i], oyCMMRegistrationToName(list[i], oyCMM_CONTEXT, oyNAME_PATTERN, 0, malloc) );
    ++i;
  }

  if(list)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyGetCMMs(oyCMM_CONTEXT, oyNAME_NICK,) fine %d        ", i );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyGetCMMs(oyCMM_CONTEXT, oyNAME_NICK,) failed         " );
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
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      " oyOptionChoicesGet2( 0, current == %s [%d])      ", list[current],
                                                              current );
    } else
    { PRINT_SUB( oyTESTRESULT_XFAIL,
      "oyOptionChoicesGet2( current == ???? ) missed         " );
    }

    oyOptionChoicesFree( oyWIDGET_CMM_CONTEXT, (const char ***)&list, i );
  }


  if(default_cmm) free(default_cmm);


  default_cmm = oyGetCMMPattern( oyCMM_CONTEXT, oySOURCE_FILTER, malloc );

  if(default_cmm && default_cmm[0])
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyGetCMMPattern( oySOURCE_FILTER )==%s", default_cmm );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyGetCMMPattern( oySOURCE_FILTER ) failed             " );
  }
  if(default_cmm) free(default_cmm);


  default_cmm = oyGetCMMPattern( oyCMM_CONTEXT, 0, malloc );

  if(default_cmm && default_cmm[0])
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyGetCMMPattern( ) == %s             ", default_cmm );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyGetCMMPattern( ) failed                             " );
  }
  if(default_cmm) free(default_cmm);

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

oyTESTRESULT_e testICCsCheck()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  char ** list = oyGetCMMs( oyCMM_CONTEXT, oyNAME_NAME, 0, malloc );
  int i = 0;

  while(list && list[i])
  {
    char * reg = oyCMMNameToRegistration( list[i], oyCMM_CONTEXT, oyNAME_NAME, 0, malloc );
    char * reg_pattern = oyCMMRegistrationToName( reg, oyCMM_CONTEXT, oyNAME_PATTERN, 0, malloc );
    fprintf(zout, "Testing ICC CMM[%d]: \"%s\"\n", i, list[i] );

    if(reg)
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "oyGetCMMs(oyCMM_CONTEXT) fine %s", reg_pattern );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
      "oyGetCMMs(oyCMM_CONTEXT) failed   " );
    }


    uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
    oyProfile_s /** p_cmyk = oyProfile_FromStd( oyEDITING_CMYK, NULL ),*/
                * p_in = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, NULL ),
                * p_out = oyProfile_FromFile( "compatibleWithAdobeRGB1998.icc", icc_profile_flags, NULL );
    uint16_t buf_16in2x2[12] = {
    32767,32767,32767, 10000,10000,10000,
    0,0,0,             65535,65535,65535
    };
    uint16_t buf_16out2x2[12];
    oyDATATYPE_e buf_type_in = oyUINT16,
                 buf_type_out = oyUINT16;
    oyImage_s *input, *output;

    //fprintf(stdout, "\n" );

    input =oyImage_Create( 2,2, 
                         buf_16in2x2,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         0 );
    output=oyImage_Create( 2,2, 
                         buf_16out2x2,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                          oyDataType_m(buf_type_out),
                         p_out,
                         0 );
    oyOptions_s * options = NULL;
    oyOptions_SetFromText( &options, "////context", reg_pattern, OY_CREATE_NEW );
    oyOptions_SetFromText( &options, "////rendering_intent", "1", OY_CREATE_NEW );
    oyConversion_s * cc = oyConversion_CreateBasicPixels( input,output, options, 0 );
    oyFilterGraph_s * cc_graph = oyConversion_GetGraph( cc );
    oyFilterNode_s * icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
    const char * node_reg = oyFilterNode_GetRegistration( icc );
    char * reg_nick = oyCMMRegistrationToName( reg, oyCMM_CONTEXT, oyNAME_NICK, 0, malloc );

    if(node_reg && strstr(node_reg, reg_nick))
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "\"context\"=\"%s\"                                   ", oyNoEmptyString_m_(reg_nick) );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
      "\"context\"=\"%s\"                                   ", oyNoEmptyString_m_(node_reg) );
    }

    int error = oyConversion_RunPixels( cc, NULL );
    double delta = 0.001,
           da = u16TripleEqual(buf_16out2x2[0], buf_16out2x2[1], buf_16out2x2[2]),
           db = u16TripleEqual(buf_16out2x2[3], buf_16out2x2[4], buf_16out2x2[5]),
           dc = u16TripleEqual(buf_16out2x2[6], buf_16out2x2[7], buf_16out2x2[8]),
           dd = u16TripleEqual(buf_16out2x2[9], buf_16out2x2[10], buf_16out2x2[11]);
    if(!error &&
       /* assuming that a proper working space gives equal results along the gray axis */
       da < delta && db < delta && dc < delta && dd < delta )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "relative colorimetric intent, equal channels      %3.5f[%g] %%", OY_MAX(da,OY_MAX(db,OY_MAX(dc,dd)))*100.0, delta*100.0 );
    } else
    { PRINT_SUB( oyTESTRESULT_XFAIL,
      "relative colorimetric intent, equal channels      %3.5f[%g] %%", OY_MAX(da,OY_MAX(db,OY_MAX(dc,dd)))*100.0, delta*100.0 );
      fprintf( zout, "%d %d %d   %d %d %d\n%d %d %d   %d %d %d\n",
               buf_16out2x2[0], buf_16out2x2[1], buf_16out2x2[2],
               buf_16out2x2[3], buf_16out2x2[4], buf_16out2x2[5],
               buf_16out2x2[6], buf_16out2x2[7], buf_16out2x2[8],
               buf_16out2x2[9], buf_16out2x2[10], buf_16out2x2[11]);
    }


    oyImage_Release( &input );
    oyImage_Release( &output );
    oyConversion_Release( &cc );

    float buf_f32in2x2[12],
          buf_f32out2x2[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
    int j;
    for(j = 0; j < 3; ++j) buf_f32in2x2[j] = 0.5f;
    for(j = 3; j < 12; ++j) buf_f32in2x2[j] = buf_16in2x2[j]/65535.0f;
    buf_type_in = oyFLOAT;
    buf_type_out = oyFLOAT;
    cc = oyConversion_CreateBasicPixelsFromBuffers(
                              p_in, buf_f32in2x2, oyDataType_m(buf_type_in),
                              p_out, buf_f32out2x2, oyDataType_m(buf_type_out),
                                                    options, 4 );
    error = oyConversion_RunPixels( cc, NULL );
    double equal = 0, max = 0;
    for(j = 0; j < 12; ++j)
      if((equal = u16Equal((int)(buf_f32out2x2[j]*65535.0f), buf_16out2x2[j])) > max)
        max = equal;
    /* Is the float conversion ~ equal to the integer math? */
    if(!error && (max <= delta))
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "relative colorimetric intent, integer equal float %3.5f[%g] %%", max*100.0, delta*100.0 );
    } else
    { PRINT_SUB( oyTESTRESULT_XFAIL,
      "relative colorimetric intent, integer equal float %3.5f[%g] %%", max*100.0, delta*100.0 );
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

      oyFilterGraph_s * cc_graph = oyConversion_GetGraph( cc );
      oyFilterNode_s * icc = oyFilterGraph_GetNode( cc_graph, -1, "///icc_color", 0 );
      oyBlob_s * blob = oyFilterNode_ToBlob( icc, NULL );
      char * name = NULL;
      oyStringAddPrintf( &name, 0,0,
                         "test-dl-%s-%d-id-%d.icc", reg_nick, i, oyObject_GetId(icc->oy_) );
      oyWriteMemToFile_( name, oyBlob_GetPointer( blob ), oyBlob_GetSize( blob) );
      fprintf( zout, "wrote Device Link for inspection to %s\n", name );
      oyFilterGraph_Release( &cc_graph );
      oyFilterNode_Release( &icc );
      oyBlob_Release( &blob );
      fprintf( zout, "options where: %s\n", oyOptions_GetText( options, oyNAME_NICK ) );
    }

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

  return result;
}


#include "oyranos_generic_internal.h"
oyHash_s *   oyTestCacheListGetEntry_ ( const char        * hash_text)
{
  if(!oy_test_cache_)
    oy_test_cache_ = oyStructList_New( 0 );

  return oyCacheListGetEntry_(oy_test_cache_, 0, hash_text);
}

oyTESTRESULT_e testCache()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int verbose = 0;
  char * text = oyAlphaPrint_( verbose );
  if(text)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "Cache content: %s             ", text );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyAlphaPrint_( ) failed                               " );
  }

  const char * hash_texts[] = {
    "A", "B", "AB", "ABC", "abc", "ABCDEF",
    "org/freedesktop/openicc/foo/bar/long/item",
    "org/freedesktop/openicc/foo/bar/sjsjsjsjsjsjsjsjsjsjsjsjsjsjsjsjsjsjsj------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------",
    "///not/so/long", "//an/other/not/so/long"
  };

  oyTestCacheListClear_();

  int repeat = 20;
  int count = 1000;
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyCacheListGetEntry_(unique long entry) %s",
                          oyProfilingToString(i*repeat,clck/(double)CLOCKS_PER_SEC, "entries"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyCacheListGetEntry_(unique long entry)  " );
  }

  oyTestCacheListClear_();
  count = 100;
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyCacheListGetEntry_(unique long entry) %s",
                          oyProfilingToString(i*repeat,clck/(double)CLOCKS_PER_SEC, "entries"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyCacheListGetEntry_(unique long entry)  " );
  }


  oyTestCacheListClear_();
  count = 1000;
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyCacheListGetEntry_(unique vlong entry) %s",
                          oyProfilingToString(i*repeat,clck/(double)CLOCKS_PER_SEC, "entries"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyCacheListGetEntry_(unique vlong entry)  " );
  }

  oyTestCacheListClear_();
  count = 100;
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyCacheListGetEntry_(unique vlong entry) %s",
                          oyProfilingToString(i*repeat,clck/(double)CLOCKS_PER_SEC, "entries"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyCacheListGetEntry_(unique vlong entry)  " );
  }


  oyTestCacheListClear_();
  count = 1000;
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyCacheListGetEntry_(unique short entry) %s",
                          oyProfilingToString(i*repeat,clck/(double)CLOCKS_PER_SEC, "entries"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyCacheListGetEntry_(unique short entry) " );
  }

  oyTestCacheListClear_();
  count = 100;
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
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyCacheListGetEntry_(unique short entry) %s",
                          oyProfilingToString(i*repeat,clck/(double)CLOCKS_PER_SEC, "entries"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyCacheListGetEntry_(unique short entry) " );
  }

  oyTestCacheListClear_();

  return result;
}

oyTESTRESULT_e testPaths()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  const char * type_names[] = {
    "oyPATH_NONE", "oyPATH_ICC", "oyPATH_POLICY", "oyPATH_MODULE", "oyPATH_SCRIPT", "oyPATH_CACHE"
  };
  oyPATH_TYPE_e types[] = {
    oyPATH_NONE, oyPATH_ICC, oyPATH_POLICY, oyPATH_MODULE, oyPATH_SCRIPT, oyPATH_CACHE
  };
  const char * scope_names[] = {
    "oySCOPE_USER_SYS", "oySCOPE_USER", "oySCOPE_SYSTEM", "oySCOPE_OYRANOS", "oySCOPE_MACHINE"
  };
  oySCOPE_e scopes[] = {
    oySCOPE_USER_SYS, oySCOPE_USER, oySCOPE_SYSTEM, (oySCOPE_e)oySCOPE_OYRANOS, (oySCOPE_e)oySCOPE_MACHINE
  };

  for(int i = 1; i <= 5; ++i)
  for(int j = 1; j <= 4; ++j)
  {
  char * text = oyGetInstallPath( types[i], scopes[j], oyAllocateFunc_ );
  if(text)
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyGetInstallPath( %s, %s ): %s", type_names[i],scope_names[j],
                                                oyNoEmptyString_m_(text) );
  } else
  { PRINT_SUB( oyTESTRESULT_XFAIL,
    "oyGetInstallPath( %s, %s ): %s", type_names[i],scope_names[j],
                                                oyNoEmptyString_m_(text) );
  }
  }


  return result;
}


oyTESTRESULT_e testConfDomain ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  int error = 0;
  oyConfDomain_s * a = 0, * b = 0;
  oyObject_s object = oyObject_New();
  const char ** texts = 0;
  char       ** domains = 0;
  int i,j,n;
  uint32_t count = 0,
         * rank_list = 0;

  fprintf(stdout, "\n" );

#ifdef __APPLE__
  a = oyConfDomain_FromReg( OY_STD"/device/config.icc_profile.monitor.qarz", 0 );
#else
  a = oyConfDomain_FromReg( OY_STD"/device/config.icc_profile.monitor.oyX1", 0 );
#endif
  error = !a;

  if(!error)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyConfDomain_FromReg() good                     " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyConfDomain_FromReg() failed                   " );
  }

  b = oyConfDomain_Copy( a, object );

  if(!error && b && b != a)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyConfDomain_Copy good                          " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyConfDomain_Copy failed                        " );
  }

  error = oyConfDomain_Release( &b );

  b = oyConfDomain_Copy( a, 0 );

  if(!error && b && a == b )
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyConfDomain_Copy() good                        " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyConfDomain_Copy() failed                      " );
  }

  oyConfDomain_Release( &a );
  oyConfDomain_Release( &b );

  error = oyConfigDomainList( "//" OY_TYPE_STD, &domains, &count, &rank_list,
                              malloc );
  if( count )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfigDomainList \"%s\": %d               ", "//" OY_TYPE_STD "",
                                                    (int)count );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfigDomainList \"%s\": %d               ", "//" OY_TYPE_STD "",
                                                    (int)count );
  }
  for( i = 0; i < (int)count; ++i)
  {
    fprintf( zout, "%d: %s\n", i, domains[i] );
  }
  fprintf( zout, "\n");
  
  for(i = 0; i < (int)count; ++i)
  {
    int text_missed = 0;
    const char * t[3] = {0,0,0};
    const char * nick = domains[i];

    if(strchr(nick, '/'))
      nick = strrchr(nick, '/') + 1;

    a = oyConfDomain_FromReg( domains[i], 0 );
    texts = oyConfDomain_GetTexts( a );
    n = j = 0;
    if(texts)
      while(texts[j]) ++j;

    n = j;
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
        fprintf(zout,"\"%s\" =\n  \"%s\" \"%s\" \"%s\"\n", texts[j],
                        t[oyNAME_NICK], t[oyNAME_NAME], t[oyNAME_DESCRIPTION]);
    }

    if(!error && n && !text_missed)
    { PRINT_SUB( oyTESTRESULT_SUCCESS, 
      "oyConfDomain_GetTexts() \"%s\" %d good  ", nick, n );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL, 
      "oyConfDomain_GetTexts() \"%s\" %d failed ", nick, n );
    }

    oyConfDomain_Release( &a );
    fprintf( zout, "----------\n");
  }
  oyStringListRelease_( &domains, count, free );


  return result;
}

static int test_number = 0;
#define TEST_RUN( prog, text ) { \
  if(argc > argpos) { \
      for(i = argpos; i < argc; ++i) \
        if(strstr(text, argv[i]) != 0 || \
           atoi(argv[i]) == test_number ) \
          oyTestRun( prog, text, test_number ); \
  } else if(list) \
    printf( "[%d] %s\n", test_number, text); \
  else \
    oyTestRun( prog, text, test_number ); \
  ++test_number; \
}

int results[oyTESTRESULT_UNKNOWN+1];
char * tests_failed[64];
char * tests_xfailed[64];

oyTESTRESULT_e oyTestRun             ( oyTESTRESULT_e    (*test)(void),
                                       const char        * test_name,
                                       int                 number )
{
  oyTESTRESULT_e error = oyTESTRESULT_UNKNOWN;

  fprintf( stdout, "\n________________________________________________________________\n" );
  fprintf(stdout, "Test[%d]: %s ... ", test_number, test_name );

  error = test();

  fprintf(stdout, "\t%s", oyTestResultToString(error));

  if(error == oyTESTRESULT_FAIL)
    tests_failed[results[error]] = (char*)test_name;
  if(error == oyTESTRESULT_XFAIL)
    tests_xfailed[results[error]] = (char*)test_name;
  results[error] += 1;

  /* print */
  if(error <= oyTESTRESULT_FAIL)
    fprintf(stdout, " !!! ERROR !!!" );
  fprintf(stdout, "\n" );

  return error;
}

/*  main */
int main(int argc, char** argv)
{
  int i, error = 0,
      argpos = 1,
      list = 0;

  if(getenv("OY_DEBUG"))
  {
    int value = atoi(getenv("OY_DEBUG"));
    if(value > 0)
      oy_debug += value;
  }

  /* init */
  for(i = 0; i <= oyTESTRESULT_UNKNOWN; ++i)
    results[i] = 0;

  i = 1; while(i < argc) if( strcmp(argv[i++],"-l") == 0 )
  { ++argpos;
    zout = stderr;
    list = 1;
  }

  i = 1; while(i < argc) if( strcmp(argv[i++],"--silent") == 0 )
  { ++argpos;
    zout = stderr;
  }

  fprintf( zout, "\nOyranos Tests v" OYRANOS_VERSION_NAME
           "  developed: " OYRANOS_DATE
           "\n\n" );

  /* do tests */

  TEST_RUN( testVersion, "Version matching" );
  TEST_RUN( testI18N, "Internationalisation" );
  TEST_RUN( testElektra, "Elektra" );
  TEST_RUN( testStringRun, "String handling" );
  TEST_RUN( testOption, "basic oyOption_s" );
  TEST_RUN( testOptionInt,  "oyOption_s integers" );
  TEST_RUN( testOptionsSet,  "Set oyOptions_s" );
  TEST_RUN( testOptionsCopy,  "Copy oyOptions_s" );
  TEST_RUN( testBlob, "oyBlob_s" );
  TEST_RUN( testSettings, "default oyOptions_s settings" );
  TEST_RUN( testConfDomain, "oyConfDomain_s");
  TEST_RUN( testInterpolation, "Interpolation oyLinInterpolateRampU16" );
  TEST_RUN( testProfile, "Profile handling" );
  TEST_RUN( testProfiles, "Profiles reading" );
  TEST_RUN( testProfileLists, "Profile lists" );
  TEST_RUN( testProofingEffect, "proofing_effect" );
  TEST_RUN( testDeviceLinkProfile, "CMM deviceLink" );
  TEST_RUN( testClut, "CMM clut" );
  //TEST_RUN( testMonitor,  "Monitor profiles" );
  //TEST_RUN( testDevices,  "Devices listing" );
  TEST_RUN( testRegistrationMatch,  "Registration matching" );
  TEST_RUN( test_oyTextIccDictMatch,  "IccDict matching" );
  TEST_RUN( testPolicy, "Policy handling" );
  TEST_RUN( testWidgets, "Widgets" );
  TEST_RUN( testCMMDevicesListing, "CMM devices listing" );
  TEST_RUN( testCMMDevicesDetails, "CMM devices details" );
  TEST_RUN( testCMMRankMap, "rank map handling" );
  TEST_RUN( testCMMMonitorJSON, "monitor JSON" );
  TEST_RUN( testCMMMonitorListing, "CMM monitor listing" );
  TEST_RUN( testCMMMonitorModule, "CMM monitor module" );
  TEST_RUN( testCMMDBListing, "CMM DB listing" );
  TEST_RUN( testCMMmonitorDBmatch, "CMM monitor DB match" );
  TEST_RUN( testCMMsShow, "CMMs show" );
  TEST_RUN( testCMMnmRun, "CMM named color run" );
  TEST_RUN( testImagePixel, "CMM Image Pixel run" );
  TEST_RUN( testFilterNode, "FilterNode Options" );
  TEST_RUN( testConversion, "CMM selection" );
  TEST_RUN( testCMMlists, "CMMs listing" );
  TEST_RUN( testICCsCheck, "CMMs ICC conversion check" );
  TEST_RUN( testCache, "Cache" );
  TEST_RUN( testPaths, "Paths" );

  /* give a summary */
  if(!list)
  {

    fprintf( stdout, "\n################################################################\n" );
    fprintf( stdout, "#                                                              #\n" );
    fprintf( stdout, "#                     Results                                  #\n" );
    fprintf( stdout, "    Total of Sub Tests:         %d\n", oy_test_sub_count );
    for(i = 0; i <= oyTESTRESULT_UNKNOWN; ++i)
      fprintf( stdout, "    Tests with status %s:\t%d\n",
                       oyTestResultToString( (oyTESTRESULT_e)i ), results[i] );

    error = (results[oyTESTRESULT_FAIL] ||
             results[oyTESTRESULT_SYSERROR] ||
             results[oyTESTRESULT_UNKNOWN]
            );

    for(i = 0; i < results[oyTESTRESULT_XFAIL]; ++i)
      fprintf( stdout, "    %s: \"%s\"\n",
               oyTestResultToString( oyTESTRESULT_XFAIL), tests_xfailed[i] );
    for(i = 0; i < results[oyTESTRESULT_FAIL]; ++i)
      fprintf( stdout, "    %s: \"%s\"\n",
               oyTestResultToString( oyTESTRESULT_FAIL), tests_failed[i] );

    if(error)
      fprintf( stdout, "    Tests FAILED\n" );
    else
      fprintf( stdout, "    Tests SUCCEEDED\n" );

    fprintf( stdout, "\n    Hint: the '-l' option will list all test names\n" );

  }

  return error;
}

