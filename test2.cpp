/** @file test_oyranos.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2004-2009  Kai-Uwe Behrmann
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

#include <lcms.h>

#include "oyranos.h"


/* C++ includes and definitions */
#ifdef __cplusplus
void* myAllocFunc(size_t size) { return new char [size]; }
#include <fstream>
#include <iostream>
using namespace oyranos;
#else
void* myAllocFunc(size_t size) { return calloc(size,1); }
#endif

/* --- general test routines --- */

typedef enum {
  oyTESTRESULT_SUCCESS,
  oyTESTRESULT_FAIL,
  oyTESTRESULT_XFAIL,
  oyTESTRESULT_SYSERROR,
  oyTESTRESULT_UNKNOWN
} oyTESTRESULT_e;

int results[oyTESTRESULT_UNKNOWN+1];

const char * oyTestResultToString    ( oyTESTRESULT_e      error )
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

  const char * test = "sw/oyranos.org/imaging/display.oydi/display_name";
  
  char * erg = oyStrnchr_( (char*) test, OY_SLASH_C, oyStrlen_(test) );
  int test_n = oyStringSegmentsN_( test, oyStrlen_(test), OY_SLASH_C );
  test_n = oyStringSegments_( test, OY_SLASH_C );
  char * test_out = (char*) malloc(strlen(test));
  char * test_sub = 0;
  int test_end;

  error = 0;
  if(test_n != 5) error = 1;
  for(i = 0; i < test_n; ++i)
  {
    int test_end = 0;
    char * test_sub = oyStringSegment_( (char*) test, OY_SLASH_C, i,
                                           &test_end );
    int test_sub_n = oyStringSegmentsN_( test_sub, test_end, '.' );

    switch(i) {
      case 0: if(test_sub_n != 1) error = 1; break;
      case 1: if(test_sub_n != 2) error = 1; break;
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
      fprintf(stdout, "%d%c%d%c \"%s\"\n", i, j?' ':'/',j, j ? '.': ' ',
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


  double clck = clock();
  for(i = 0; i < 1000000; ++i)
    test_sub = oyFilterRegistrationToSTextField( test, oyFILTER_REG_OPTION,
                                                 &test_end );
  clck = clock() - clck;

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyFilterRegistrationToSTextField()          %d %.03f", i,
                                       (double)clck/(double)CLOCKS_PER_SEC );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyFilterRegistrationToSTextField()                 " );
  }


  clck = clock();
  for(i = 0; i < 1000000; ++i)
    test_sub = oyFilterRegistrationToText( test, oyFILTER_REG_OPTION, 0 );
  clck = clock() - clck;

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyFilterRegistrationToText()                %d %.03f", i,
                                       (double)clck/(double)CLOCKS_PER_SEC );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyFilterRegistrationToText())                      " );
  }



  test = "//imaging/display.oydi/";
  erg = oyStrnchr_( (char*) test, OY_SLASH_C, oyStrlen_(test) );
  test_n = oyStringSegmentsN_( test, oyStrlen_(test), OY_SLASH_C );
  test_n = oyStringSegments_( test, OY_SLASH_C );

  error = 0;
  if(test_n != 5) error = 1;
  for(i = 0; i < test_n; ++i)
  {
    int test_end = 0;
    char * test_sub = oyStringSegment_( (char*) test, OY_SLASH_C, i,
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
      fprintf(stdout, "%d%c%d%c \"%s\"\n", i, j?' ':'/', j, j ? '.': ' ',
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

oyTESTRESULT_e testOptionInt ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  int error = 0;
  oyOption_s * o = 0;
  int32_t erg[4] = { -1,-1,-1,-1 };

  oyExportReset_(EXPORT_SETTING);

  fprintf(stdout, "\n" );

  o = oyOption_New( "//" OY_TYPE_STD "/filter/x", 0 );

  error = oyOption_SetFromInt( o, 0, 0, 0 );
  if(!error && o->value &&
     o->value->int32 == 0 &&
     o->value_type == oyVAL_INT)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() good                        " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromInt() failed                      " );
  }

  error = oyOption_SetFromInt( o, 58293, 0, 0 );
  if(!error && o->value &&
     o->value->int32 == 58293 &&
     o->value_type == oyVAL_INT)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() single int32_t good         " );
    erg[0] = oyOption_GetValueInt( o, 0 );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromInt() single int32_t failed       " );
  }

  error = oyOption_SetFromInt( o, 58293, 1, 0 );
  if(!error && o->value &&
     o->value->int32_list[0] == 2 &&
     o->value->int32_list[1] == 58293 &&
     o->value->int32_list[2] == 58293 &&
     o->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() add int32_t list good     " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromInt() add int32_t list failed   " );
  }

  error = oyOption_SetFromInt( o, 58293, 2, 0 );
  if(!error && o->value &&
     o->value->int32_list[0] == 3 &&
     o->value->int32_list[1] == 58293 &&
     o->value->int32_list[2] == 58293 &&
     o->value->int32_list[3] == 58293 &&
     o->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() add int32_t list good     " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromInt() add int32_t list failed   " );
  }

  error = oyOption_SetFromInt( o, 58293, 1, 1 );
  if(!error && o->value &&
     o->value->int32_list[0] == 2 &&
     o->value->int32_list[1] == 0 &&
     o->value->int32_list[2] == 58293 &&
     o->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() new int32_t list good     " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromInt() new int32_t list failed   " );
  }

  error = oyOption_SetFromInt( o, 58293, 0, 0 );
  if(!error && o->value &&
     o->value->int32_list[0] == 2 &&
     o->value->int32_list[1] == 58293 &&
     o->value->int32_list[2] == 58293 &&
     o->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() set int32_t list good     " );
    erg[1] = oyOption_GetValueInt( o, 0 );
    erg[2] = oyOption_GetValueInt( o, 1 );
    erg[3] = oyOption_GetValueInt( o, 3 );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromInt() set int32_t list failed   " );
  }

  if(!error && erg[0] == 58293 && erg[1] == 58293 && erg[2] == 58293 &&
               erg[3] == 0)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_GetValueInt() good                     " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_GetValueInt() failed                   " );
  }

  oyOption_Release( &o );

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
                "sw/oyranos.org/imaging/lcms.colour.icc/rendering_bpc.advanced",
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
                                 "//" OY_TYPE_STD "/imaging/C", "true",
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
                            "sw/oyranos.org/imaging/image", setA  );

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

  if(!error && b && b->ptr && b->size && b->ptr == static_ptr)
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

  if(!error && b && b->ptr && !b->size && b->ptr == static_ptr)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyBlob_Copy( static 0) good                     " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyBlob_Copy( static 0) failed                   " );
  }

  ptr = malloc(1024);
  error = oyBlob_SetFromData( a, ptr, 1024, type );

  if(!error && a->ptr && a->size == 1024 && a->ptr != ptr)
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

  if(!error && b && a != b && b->ptr && a->size == b->size && a->ptr != b->ptr )
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyBlob_Copy() good                              " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyBlob_Copy() failed                            " );
  }

  oyBlob_Release( &a );
  oyBlob_Release( &b );
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

  fprintf(stdout, "\n" );

  /* we check for out standard CMM */
  opts = oyOptions_ForFilter( "//" OY_TYPE_STD, "lcms",
                                            oyOPTIONATTRIBUTE_ADVANCED /* |
                                            oyOPTIONATTRIBUTE_FRONT |
                                            OY_SELECT_COMMON */, 0 );

  oyOptions_SetFromText( &opts, "cmyk_cmyk_black_preservation", "1", 0 );

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
    printf("%d: \"%s\": \"%s\" %s %d\n", i, 
           oyOption_GetText( o, oyNAME_DESCRIPTION ), o->value->string,
           oyFilterRegistrationToText( oyOption_GetText( o, oyNAME_DESCRIPTION),
                                       oyFILTER_REG_OPTION, 0 ),
           o->flags );

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

  opts = oyOptions_ForFilter( "//" OY_TYPE_STD, "lcms",
                                            oyOPTIONATTRIBUTE_ADVANCED  |
                                            oyOPTIONATTRIBUTE_FRONT |
                                            OY_SELECT_COMMON, 0 );
  printf("Show advanced common front end options:\n");
  countB = oyOptions_Count( opts );
  for( i = 0; i < countB; ++i)
  {
    o = oyOptions_Get( opts, i );
    printf("%d: \"%s\": \"%s\" %s %d\n", i, 
           oyOption_GetText( o, oyNAME_DESCRIPTION ), o->value->string,
           oyFilterRegistrationToText( oyOption_GetText( o, oyNAME_DESCRIPTION),
                                       oyFILTER_REG_OPTION, 0 ),
           o->flags );

    oyOption_Release( &o );
  }
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

#ifdef __cplusplus
  std::cout << "Start: " << start_time << std::endl;
#else
  fprintf(stdout, "Start %.3f\n", start_time );
#endif

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
  for(i = 0; i < 1000; ++i)
  {
    uint32_t count = 0;
    char ** names = oyProfileListGet(0, &count, myAllocFunc);
    if(count != ref_count)
    {
      PRINT_SUB( oyTESTRESULT_FAIL, 
      ": wrong profile count: %d/%d", count, ref_count );
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
#ifdef __cplusplus
        delete [] names[j];
#else
        free(names[j]);
#endif
    }
#ifdef __cplusplus
    if( names ) delete [] names;
    std::cout << "." << std::flush;
#else
    if( names ) free( names );
    fprintf(stdout, "." ); fflush(stdout);
#endif

  }

  double end = DBG_UHR_;

#ifdef __cplusplus
  std::cout << std::endl;
  std::cout << "1000 + 1 calls to oyProfileListGet() took: "<< end - start_time
            << " seconds" << std::endl;
#else
  fprintf(stdout, "\n1000 + 1 calls to oyProfileListGet() took: %.03f seconds\n",
                  end - start_time );
#endif

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


  error = oyDevicesGet( OY_TYPE_STD, "monitor", 0, &devices );
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

  if( oyFilterRegistrationMatch("sw/oyranos.org/" OY_TYPE_STD "/icc.lcms",
                                "//" OY_TYPE_STD "/icc",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "simple CMM selection                  " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "simple CMM selection                  " );
  }

  if(!oyFilterRegistrationMatch("sw/oyranos.org/" OY_TYPE_STD "/icc.lcms",
                                "//" OY_TYPE_STD "/icc.octl",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "simple CMM selection no match         " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "simple CMM selection no match         " );
  }

  if( oyFilterRegistrationMatch("sw/oyranos.org/" OY_TYPE_STD "/icc.lcms",
                                "//" OY_TYPE_STD "/icc.4+lcms",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "special CMM selection                 " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "special CMM selection                 " );
  }

  if(!oyFilterRegistrationMatch("sw/oyranos.org/" OY_TYPE_STD "/icc.lcms",
                                "//" OY_TYPE_STD "/icc.4-lcms",
                                oyOBJECT_CMM_API4_S ))
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "special CMM avoiding                  " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "special CMM avoiding                  " );
  }

  if( oyFilterRegistrationMatch("sw/oyranos.org/" OY_TYPE_STD "/icc.lcms",
                                "//" OY_TYPE_STD "/icc.7-lcms",
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
    xml = oyPolicyToXML( oyGROUP_ALL, 1, myAllocFunc );

  if( xml && xml[0] )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyranos::oyPolicyToXML                " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyranos::oyPolicyToXML                " );
  }

  if(xml) {
    oyReadXMLPolicy(oyGROUP_ALL, xml);
    printf("xml text: \n%s", xml);

    data = oyPolicyToXML( oyGROUP_ALL, 1, myAllocFunc );

    if( strcmp( data, xml ) == 0 )
    { PRINT_SUB( oyTESTRESULT_SUCCESS,
      "Policy rereading                      " );
    } else
    { PRINT_SUB( oyTESTRESULT_FAIL,
      "Policy rereading                      " );
    }

#ifdef __cplusplus
    delete [] xml;
    delete [] data;
#else
    free(xml);
    free(data);
#endif
  }


  return result;
}

/* forward declaration for oyranos_alpha.c */
#ifdef __cplusplus
extern "C" {
#endif
char ** oyCMMsGetLibNames_           ( uint32_t          * n,
                                       const char        * required_cmm );
#ifdef __cplusplus
}
#endif

oyTESTRESULT_e testCMMDevicesListing ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int i, j, k, l, j_n, k_n;
  uint32_t count = 0,
         * rank_list = 0;
  int error = 0;
  char ** texts = 0,
        * val = 0;

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

  error = oyConfigDomainList( "//" OY_TYPE_STD, &texts, &count, &rank_list, 0 );
  if( count )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfigDomainList \"%s\": %d         ", "//" OY_TYPE_STD "", (int)count );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfigDomainList \"%s\": %d         ", "//" OY_TYPE_STD "", (int)count );
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
  error = oyOptions_SetFromText( &options_list,
                                 "//" OY_TYPE_STD "/config/command", "list",
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

        val = oyOption_GetValueText( o, oyAllocateFunc_ );
        /* collect the device_name's into a set of options for later */
        error = oyOptions_SetFromText( &options_devices, o->registration,
                                       val,
                                       OY_CREATE_NEW | OY_ADD_ALWAYS );
        printf("  %d::%d::%d %s %s\n", i,j,k,
               o->registration, val );

        if(val)
          oyDeAllocateFunc_( val ); val = 0;
        oyOption_Release( &o );
      }

      oyConfig_Release( &config );
    }

    oyConfigs_Release( &configs );
  }
  fprintf( stdout, "\n");

  fprintf( stdout, "oyConfigs_FromDomain() \"properties\" call:\n" );
  devices_n = oyOptions_Count( options_devices );
  {
    char * classe = (char*) malloc(1024);

    for( l = 0; l < devices_n; ++l )
    {
      /* set a general request */
      error = oyOptions_SetFromText( &options,
                                     "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );

      /* set the device_name */
      o = oyOptions_Get( options_devices, l );
      val = oyOption_GetValueText( o, oyAllocateFunc_ );
      error = oyOptions_SetFromText( &options, o->registration,
                                     val, OY_CREATE_NEW );
      sprintf(classe, "%s", o->registration);
      (strrchr(classe, '/'))[0] = 0;

      if(val) oyDeAllocateFunc_( val ); val = 0;
      oyOption_Release( &o );

      /* send the query to a backend */
      error = oyConfigs_FromDomain( classe, options, &configs, 0 );
      /* display results */
      j_n = oyConfigs_Count( configs );
      if(j_n)
        fprintf(stdout, "--------------------------------------------------------------------------------\n%s:\n", classe );
      for( j = 0; j < j_n; ++j )
      {
        config = oyConfigs_Get( configs, j );

        if(l == 0 && j == 0)
          device = oyConfig_Copy( config, 0 );

        k_n = oyConfig_Count( config );
        for( k = 0; k < k_n; ++k )
        {
          o = oyConfig_Get( config, k );

          val = oyOption_GetValueText( o, oyAllocateFunc_ );
          printf( "  %d::%d::%d %s: \"%s\"\n", l,j,k, 
                  oyStrrchr_(o->registration,'/')+1, val );

          if(val) oyDeAllocateFunc_( val ); val = 0;
          oyOption_Release( &o );
        }

        //error = oyConfig_SaveToDB( config );
        oyConfig_Release( &config );
      }

      oyConfigs_Release( &configs );
      oyOptions_Release( &options );
    }
  }

  oyOptions_Release( &options_list );
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
  char * text = 0,
       * val = 0;

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

  error = oyDeviceGet( 0, "monitor", ":0.0", 0, &config );
  k_n = oyConfig_Count( config );
  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyDeviceGet(..\"monitor\" \":0.0\"..) %d     ", k_n );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyDeviceGet(..\"monitor\" \":0.0\"..) %d     ", k_n );
  }
    for( k = 0; k < k_n; ++k )
    {
      o = oyConfig_Get( config, k );
      val = oyOption_GetValueText( o, oyAllocateFunc_ );

      printf( "  %d %s: \"%s\"\n", k, o->registration, val );

      if(val) oyDeAllocateFunc_( val ); val = 0;
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
  char * val = 0;

  error = oyConfigs_FromDB( "//" OY_TYPE_STD "", &configs, 0 );
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
      printf( "  %d::%d %s %s\n", j,k, o->registration, val );

      if(val) oyDeAllocateFunc_( val ); val = 0;
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
  char * val = 0;

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

      val = oyOption_GetValueText( o, oyAllocateFunc_ );
      printf( "  d::%d %s: \"%s\"\n", k,
      strchr(strchr(strchr(strchr(o->registration,'/')+1,'/')+1,'/')+1,'/')+1,
              val );

      if(val) oyDeAllocateFunc_( val ); val = 0;
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

  int i, j, k, l;
  uint32_t count = 0;
  char ** texts = 0,
        * text = 0,
        * text_tmp = (char*)oyAllocateFunc_(65535);
  oyCMMInfo_s * cmm_info = 0;
  oyCMMapi4_s * cmm_api4 = 0;
  oyCMMapi6_s * cmm_api6 = 0;
  oyCMMapi7_s * cmm_api7 = 0;
  oyCMMapi8_s * cmm_api8 = 0;
  oyCMMapi9_s * cmm_api9 = 0;
  oyCMMapi_s * tmp = 0;
  oyCMMapiFilter_s * cmm_filter = 0;


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

              for(j = oyOBJECT_CMM_API4_S; j <= (int)oyOBJECT_CMM_API9_S; j++)
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

                  if(api->type == oyOBJECT_CMM_API7_S)
                  {
                    cmm_api7 = (oyCMMapi7_s*) api;
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
                            cmm_api7->plugs[l]->connector_type,
                            cmm_api7->plugs[l]->name.nick,
                            cmm_api7->plugs[l]->name.name,
                            cmm_api7->plugs[l]->name.description
                            );
                      STRING_ADD( text, text_tmp );
                    }
                    for(l = 0; l < (int)cmm_api7->sockets_n; ++l)
                    {
                      snprintf( text_tmp, 65535,
                            "        sock[%d]: type:\"%s\" id:\"%s\" \"%s\" \"%s\"\n", l,
                            cmm_api7->sockets[l]->connector_type,
                            cmm_api7->sockets[l]->name.nick,
                            cmm_api7->sockets[l]->name.name,
                            cmm_api7->sockets[l]->name.description
                            );
                      STRING_ADD( text, text_tmp );
                    }
                  }

                  if(api->type == oyOBJECT_CMM_API8_S)
                  {
                    /* a non useable filter definition */
                    oyRankPad rank_map[2] = {{(char*)"key",1,2,3},
                                             {(char*)"other",4,5,6}};
                    oyCMMapi8_s cpp_api8 = { oyOBJECT_CMM_API8_S,0,0,0,
                                             0, /* next */
                                             0,0,0, /* oyCMMapi_s stuff */
                                             (char*)"invalid/registration",
                                             {0,0,0}, /* version */
                                             0,0, /* Oyranos stuff */
                                             0, /* oyConfigs_FromPattern */
                                             0, /* oyConfig_Rank */
                                             (oyRankPad*)rank_map
                                           };

                    l = 0;
                    cmm_api8 = (oyCMMapi8_s*) api;
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

                  if(api->type == oyOBJECT_CMM_API9_S)
                  {
                    cmm_api9 = (oyCMMapi9_s*) api;
                    snprintf( text_tmp, 65535,
                            "        \"%s\"\n"
                            "        supported pattern: \"%s\"\n",
                            cmm_api9->options,
                            cmm_api9->pattern );
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

#include <kdb.h>
#ifdef __cplusplus
extern "C" {
#endif
oyOptions_s *  oyOptions_ForFilter_  ( oyFilterCore_s    * filter,
                                       uint32_t            flags,
                                       oyObject_s          object );
int      oyKeyIsString_              ( const char        * full_key_name );
int  oyColourConvert_ ( oyProfile_s       * p_in,
                        oyProfile_s       * p_out,
                        oyPointer           buf_in,
                        oyPointer           buf_out,
                        oyDATATYPE_e        buf_type_in,
                        oyDATATYPE_e        buf_type_out );
#ifdef __cplusplus
}
#define ckdb ckdb::
#else
#define ckdb
#endif
extern ckdb KDB * oy_handle_;

double d[6] = {0.5,0.5,0.5,0,0,0};

oyTESTRESULT_e testCMMnmRun ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;
  oyNamedColour_s * c = 0;
  oyProfile_s * prof = oyProfile_FromStd( oyEDITING_XYZ, NULL );
  int error = 0, l_error = 0,
      i,n = 10;

  fprintf(stdout, "\n" );
#if 1
  double clck = clock();
  for(i = 0; i < n*10000; ++i)
  {
    c = oyNamedColour_Create( NULL, NULL,0, prof, 0 );
    oyNamedColour_Release( &c );
  }
  clck = clock() - clck;

  c = oyNamedColour_Create( NULL, NULL,0, prof, 0 );
  if( c )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyNamedColour_Create( )             %s",
                   oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Obj."));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyNamedColour_Create( )                            " );
  }


  const char * key_name = "shared/freedesktop.org/imaging/behaviour/rendering_bpc";
  oyAlloc_f allocate_func = oyAllocateFunc_;

  char* name = 0;
  char* full_key_name = 0;
  name = (char*) oyAllocateWrapFunc_( MAX_PATH, allocate_func );
  full_key_name = (char*) oyAllocateFunc_ (MAX_PATH);
  clck = clock();

  for(i = 0; i < n*3*17; ++i)
  {
  int rc = 0;
  ckdb Key * key = 0;
  int success = 0;

  sprintf( full_key_name, "%s%s", OY_USER, key_name );

  /** check if the key is a binary one */
  key = ckdb keyNew( full_key_name, KEY_END );
  rc=ckdb kdbGetKey( oy_handle_, key );
  success = ckdb keyIsString(key);

  if(success)
    rc = ckdb keyGetString ( key, name, MAX_PATH );
  ckdb keyDel( key ); key = 0;

  if( rc || !strlen( name ))
  {
    sprintf( full_key_name, "%s%s", OY_SYS, key_name );
    key = ckdb keyNew( full_key_name, KEY_END );
    if(success)
      rc = ckdb keyGetString( key, name, MAX_PATH );
    ckdb keyDel( key ); key = 0;
  }

  }
  oyDeAllocateFunc_( full_key_name );
  oyDeAllocateFunc_( name );
  clck = clock() - clck;

  if( i )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyGetKeyString_()                   %s",
                  oyProfilingToString(n,clck/(double)CLOCKS_PER_SEC, "Filter"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyGetKeyString_()                                  " );
  }



  clck = clock();
  oyOption_s * option = oyOption_New("shared/freedesktop.org/imaging/behaviour/rendering_bpc", 0);

  for(i = 0; i < n*3*17; ++i)
  {
  int error = !option || !option->registration;
  char * text = 0;
  oyPointer ptr = 0;
  size_t size = 0;

  oyExportStart_(EXPORT_SETTING);

  if(error <= 0)
    text = oyGetKeyString_( option->registration, oyAllocateFunc_ );

  if(error <= 0)
  {
    if(text)
      oyOption_SetFromText( option, text, 0 );
    else
    {
      ptr = oyGetKeyBinary_( option->registration, &size, oyAllocateFunc_ );
      if(ptr && size)
      {
        oyOption_SetFromData( option, ptr, size );
        oyFree_m_( ptr );
      }
    }
  }

  if(text)
    oyFree_m_( text );

  oyExportEnd_();
  }
  clck = clock() - clck;

  if( i )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOption_SetValueFromDB()           %s",
                  oyProfilingToString(n,clck/(double)CLOCKS_PER_SEC, "Filter"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOption_SetValueFromDB()                          " );
  }

  clck = clock();
  for(i = 0; i < n*3; ++i)
  {
  oyFilterCore_s * filter = oyFilterCore_New_( 0 );
  oyCMMapi4_s * api4 = 0;
  oyObject_s object = 0;

  if(error <= 0)
  {
    api4 = (oyCMMapi4_s*) oyCMMsGetFilterApi_( 0, 0,
                                "//" OY_TYPE_STD "/root", oyOBJECT_CMM_API4_S );
    error = !api4;
  }

  if(error <= 0)
    error = oyFilterCore_SetCMMapi4_( filter, api4 );

  oyOptions_s * s = 0,
              * opts_tmp = 0,
              * opts_tmp2 = 0;
  oyOption_s * o = 0;
  char * type_txt = oyFilterRegistrationToText( filter->registration_,
                                                oyFILTER_REG_TYPE, 0 );
  oyCMMapi5_s * api5 = 0;
  int i,n, flags = 0;
  int error = !filter || !filter->api4_;

  /* by default we parse both sources */
  if(!(flags & OY_SELECT_FILTER) && !(flags & OY_SELECT_COMMON))
    flags |= OY_SELECT_FILTER | OY_SELECT_COMMON;

  if(!error)
  {
    /*
        Programm:
        1. get filter and its type
        2. get implementation for filter type
        3. parse static common options from meta backend
        4. parse static options from filter 
        5. merge both
        6. get stored values from disk
     */

    /*  1. get filter */

    /*  2. get implementation for filter type */
    api5 = filter->api4_->api5_;

    /*  3. parse static common options from meta backend */
    if(api5 && flags & OY_SELECT_COMMON)
      ; /*opts_tmp = oyOptions_FromText( api5->options, 0, object );*/
    /* requires step 2 */

    /*  4. parse static options from filter */
    if(flags & OY_SELECT_FILTER)
      opts_tmp2 = oyOptions_FromText( filter->api4_->options, 0, object );

    /*  5. merge */
    s = oyOptions_FromBoolean( opts_tmp, opts_tmp2, oyBOOLEAN_UNION, object );

    oyOptions_Release( &opts_tmp );
    oyOptions_Release( &opts_tmp2 );

    /*  6. get stored values */
    n = oyOptions_Count( s );
    for(i = 0; i < n && error <= 0; ++i)
    {
      o = oyOptions_Get( s, i );
      o->source = oyOPTIONSOURCE_FILTER;
      /* ask Elektra */
      error = oyOption_SetValueFromDB( o );
      oyOption_Release( &o );
    }
    error = oyOptions_DoFilter ( s, flags, type_txt );
  }

  if(type_txt)
    oyDeAllocateFunc_( type_txt );

  }
  clck = clock() - clck;

  if( i )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOptions_ForFilter_()              %s",
                  oyProfilingToString(n,clck/(double)CLOCKS_PER_SEC, "Filter"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptions_ForFilter_()                             " );
  }

  clck = clock();
  {
  oyFilterCore_s * s = oyFilterCore_New_( 0 );
  int error = !s;
  uint32_t ret = 0;
  oyOptions_s * opts_tmp = 0, * options = 0;
  oyCMMapi4_s * api4 = 0;
  oyCMMapiQueries_s * queries = 0;

  if(error <= 0)
  {
    api4 = (oyCMMapi4_s*) oyCMMsGetFilterApi_( 0, queries,
                                "//" OY_TYPE_STD "/root", oyOBJECT_CMM_API4_S );
    error = !api4;
  }

  if(error <= 0)
    error = oyFilterCore_SetCMMapi4_( s, api4 );

  if(error <= 0)
  {
    for(i = 0; i < n*3; ++i)
      opts_tmp = oyOptions_ForFilter_( s, 0, s->oy_);
#if 0
    s->options_ = api4->oyCMMFilter_ValidateOptions( s, options, 0, &ret );
#endif
    error = ret;
    
    /* @todo test oyBOOLEAN_SUBSTRACTION for correctness */
    s->options_ = oyOptions_FromBoolean( opts_tmp, options,
                                         oyBOOLEAN_SUBSTRACTION, s->oy_ );
    oyOptions_Release( &opts_tmp );
  }

    oyFilterCore_Release( &s );
  }
  clck = clock() - clck;

  if( i )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOptions_ForFilter_()              %s",
                  oyProfilingToString(n,clck/(double)CLOCKS_PER_SEC, "Filter"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptions_ForFilter_()                             " );
  }



  oyOptions_s * options = oyOptions_New(0);
  clck = clock();
  for(i = 0; i < n*3*10000; ++i)
  {
    oyFilterCore_s * core = oyFilterCore_New( "//" OY_TYPE_STD "/root",
                                              options,0 );

    oyFilterCore_Release( &core );
  }
  clck = clock() - clck;

  if( i )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyFilterCore_New()                  %s",
                 oyProfilingToString(i/3,clck/(double)CLOCKS_PER_SEC, "Cores"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyFilterCore_New()                                 " );
  }


  clck = clock();
  const char * registration = "//" OY_TYPE_STD "/root";
  for(i = 0; i < n*3*10000; ++i)
  {
    oyCMMapi4_s * api4 = 0;
    api4 = (oyCMMapi4_s*) oyCMMsGetFilterApi_( 0, 0,
                                            registration, oyOBJECT_CMM_API4_S );
    error = !api4;
    if(!(i%30000)) fprintf(stdout, "." ); fflush(stdout);
  }
  fprintf(stdout,"\n");
  clck = clock() - clck;

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyFilterCore_New() oyCMMapi4_s      %s",
                 oyProfilingToString(i/3,clck/(double)CLOCKS_PER_SEC, "Cores"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyFilterCore_New() oyCMMapi4_s                     " );
  }


  clck = clock();
  oyConversion_s * s = 0;
  oyFilterNode_s * in = 0, * out = 0;
  oyImage_s * input  = NULL,
            * output = NULL;
  double * buf_in = &d[0],
         * buf_out = &d[3];
  oyDATATYPE_e buf_type_in = oyDOUBLE,
               buf_type_out = oyDOUBLE;
  oyProfile_s * p_in = prof,
              * p_out = oyProfile_FromStd( oyASSUMED_WEB, 0 );

  for(i = 0; i < n*100; ++i)
  if(error <= 0)
  {
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


    s = oyConversion_New ( 0 );
    error = !s;    

    if(error <= 0)
      in = oyFilterNode_NewWith( "//" OY_TYPE_STD "/root", options, 0 );
    if(error <= 0)
      error = oyConversion_Set( s, in, 0 );
    if(error <= 0)
      error = oyFilterNode_DataSet( in, (oyStruct_s*)input, 0, 0 );

    if(error <= 0)
      out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/icc", options, 0 );
    if(error <= 0)
      error = oyFilterNode_DataSet( out, (oyStruct_s*)output, 0, 0 );
    if(error <= 0)
      error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                    out, "//" OY_TYPE_STD "/data", 0 );

    in = out; out = 0;

    if(error <= 0)
      out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", options, 0 );
    if(error <= 0)
    {
      error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                    out, "//" OY_TYPE_STD "/data", 0 );
      if(error)
        WARNc1_S( "could not add  filter: %s\n", "//" OY_TYPE_STD "/output" );
    }
    if(error <= 0)
      error = oyConversion_Set( s, 0, out );
    oyConversion_Release( &s );
    oyImage_Release( &input );
    oyImage_Release( &output );
    if(!(i%1000)) fprintf(stdout, "." ); fflush(stdout);
  }
  fprintf(stdout,"\n");
  clck = clock() - clck;

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConversion_CreateBasicPixels()    %s",
                    oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Obj."));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConversion_CreateBasicPixels()                   " );
  }
#else
  oyConversion_s * s = 0;
  oyFilterNode_s * in = 0, * out = 0;
  oyImage_s * input  = NULL,
            * output = NULL;
  double * buf_in = &d[0],
         * buf_out = &d[3];
  oyDATATYPE_e buf_type_in = oyDOUBLE,
               buf_type_out = oyDOUBLE;
  oyProfile_s * p_in = prof,
              * p_out = oyProfile_FromStd( oyASSUMED_WEB, 0 );
  double clck;
#endif

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
  if(s->out_)
    plug = oyFilterNode_GetPlug( s->out_, 0 );
  else
    error = 1;
  pixel_access = oyPixelAccess_Create( 0,0, plug,
                                           oyPIXEL_ACCESS_IMAGE, 0 );
  oyFilterPlug_Release( &plug );

  clck = clock();
  for(i = 0; i < n*10000; ++i)
  if(error <= 0)
  {
    pixel_access->start_xy[0] = pixel_access->start_xy[1] = 0;

#if 1
    error  = oyConversion_RunPixels( s, pixel_access );
#else

  oyConversion_s * conversion = s;
  oyFilterPlug_s * plug = 0;
  oyFilterCore_s * filter = 0;
  oyImage_s * image = 0, * image_input = 0;
  int error = 0, result, l_error = 0, i,n, dirty = 0, tmp_ticket = 0;

  if(!conversion->out_ || !conversion->out_->plugs ||
     !conversion->out_->plugs[0])
  {
    WARNc1_S("graph incomplete [%d]", s ? oyObject_GetId( s->oy_ ) : -1)
    break;
  }                                    
                                       
  /* conversion->out_ has to be linear, so we access only the first plug */
  plug = oyFilterNode_GetPlug( conversion->out_, 0 );

  if(!pixel_access)
  {
    /* create a very simple pixel iterator as job ticket */
    if(plug)
      pixel_access = oyPixelAccess_Create( 0,0, plug,
                                           oyPIXEL_ACCESS_IMAGE, 0 );
    tmp_ticket = 1;
  } 

  /* should be the same as conversion->out_->filter */
  filter = conversion->out_->core;
  image = oyConversion_GetImage( conversion, OY_OUTPUT );

  result = oyImage_FillArray( image, pixel_access->output_image_roi, 0,
                              &pixel_access->array, 0, 0 );
  error = ( result != 0 );

  if(error <= 0)
    error = conversion->out_->api7_->oyCMMFilterPlug_Run( plug, pixel_access );

  if(error != 0)
  {
    dirty = oyOptions_FindString( pixel_access->graph->options, "dirty", "true")
            ? 1 : 0;

    /* refresh the graph representation */
    oyFilterGraph_SetFromNode( pixel_access->graph, conversion->input, 0, 0 );

    /* resolve missing data */
    image_input = oyFilterPlug_ResolveImage( plug, plug->remote_socket_,
                                             pixel_access );
    oyImage_Release( &image_input );

    n = oyFilterNodes_Count( pixel_access->graph->nodes );
    for(i = 0; i < n; ++i)
    {
      l_error = oyArray2d_Release( &pixel_access->array ); OY_ERR
      l_error = oyImage_FillArray( image, pixel_access->output_image_roi, 0,
                                   &pixel_access->array, 0, 0 ); OY_ERR

      if(error != 0 &&
         dirty)
      {
        if(pixel_access->start_xy[0] != pixel_access->start_xy_old[0] ||
           pixel_access->start_xy[1] != pixel_access->start_xy_old[1])
        {
          /* set back to previous values, at least for the simplest case */
          pixel_access->start_xy[0] = pixel_access->start_xy_old[0];
          pixel_access->start_xy[1] = pixel_access->start_xy_old[1];
        }

        oyFilterGraph_PrepareContexts( pixel_access->graph, 1 );
        error = conversion->out_->api7_->oyCMMFilterPlug_Run( plug,
                                                              pixel_access);
      }

      if(error == 0)
        break;
    }
  }

  if(tmp_ticket)
  {
    /* write the data to the output image */
    if(image != pixel_access->output_image)
      result = oyImage_ReadArray( image, pixel_access->output_image_roi,
                                         pixel_access->array, 0 );
    oyPixelAccess_Release( &pixel_access );
  }

  oyImage_Release( &image );

#endif
  }
  clck = clock() - clck;

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


  cmsHPROFILE lp_in = cmsOpenProfileFromMem( p_in->block_, p_in->size_ ),
              lp_out = cmsOpenProfileFromMem( p_out->block_, p_out->size_ );
  cmsHTRANSFORM xform = cmsCreateTransform( lp_in, TYPE_XYZ_DBL,
                                            lp_out, TYPE_RGB_DBL, 1, 0 );
  clck = clock();
  for(i = 0; i < 1000000; ++i)
  {
    cmsDoTransform( xform, &d[0], &d[3], 1 );
  }
  clck = clock() - clck;

  cmsCloseProfile( lp_in );
  cmsCloseProfile( lp_out );
  cmsDeleteTransform( xform );

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "cmsDoTransform() lcms               %s",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "cmsDoTransform() lcms                              " );
  }


  lp_in = cmsOpenProfileFromMem( p_in->block_, p_in->size_ );
  lp_out = cmsOpenProfileFromMem( p_out->block_, p_out->size_ );
  clck = clock();
  for(i = 0; i < n; ++i)
  {
    xform = cmsCreateTransform( lp_in, TYPE_XYZ_DBL,
                                              lp_out, TYPE_RGB_DBL, 1, 0 );
    cmsDoTransform( xform, &d[0], &d[3], 1 );
    cmsDeleteTransform( xform );
    fprintf(stdout, "." ); fflush(stdout);
  }
  clck = clock() - clck;
  cmsCloseProfile( lp_in );
  cmsCloseProfile( lp_out );
  fprintf(stdout, "\n" );

  oyProfile_Release( &p_in );
  oyProfile_Release( &p_out );

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "cmsCreateTransform() lcms           %s",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "xform"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "cmsCreateTransform() lcms                          " );
  }


  clck = clock();
  for(i = 0; i < 100*n; ++i)
  if(error <= 0)
  {
    s = oyConversion_CreateBasicPixels( input,output, options, 0 );
    error  = oyConversion_RunPixels( s, 0 );
    oyConversion_Release ( &s );
    if(!(i%100)) fprintf(stdout, "." ); fflush(stdout);
  }
  fprintf(stdout, "\n" );

  clck = clock() - clck;
  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "+ oyConversion_RunPixels()          %s",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "+ oyConversion_RunPixels()                         " );
  }



  clck = clock();

  for(i = 0; i < n*100; ++i)
  {
    l_error = oyNamedColour_SetColourStd ( c, oyASSUMED_WEB,
                                           (oyPointer)d, oyDOUBLE, 0, options );
    if(error <= 0)
      error = l_error;
  }
  clck = clock() - clck;

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyNamedColour_SetColourStd()        %s",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyNamedColour_SetColourStd() oyASSUMED_WEB         " );
  }

  p_in = oyProfile_FromStd ( oyASSUMED_WEB, NULL );
  p_out = oyProfile_FromStd ( oyEDITING_XYZ, NULL );

  clck = clock();
  for(i = 0; i < n*100; ++i)
  {

  oyImage_s * in  = NULL,
            * out = NULL;
  oyConversion_s * conv = NULL;
  int error = 0;

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
  error  = oyConversion_RunPixels( conv, 0 );

  oyConversion_Release( &conv );
  oyImage_Release( &in );
  oyImage_Release( &out );
  }
  clck = clock() - clck;


  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyColourConvert_()                  %s",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyColourConvert_()                                 " );
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
  //oyFilterPlug_s * plug = 0;
  //oyPixelAccess_s   * pixel_access = 0;
  oyConversion_s * conv   = oyConversion_CreateBasicPixels( input,output, 0,0 );

  /* conversion->out_ has to be linear, so we access only the first plug */
  plug = oyFilterNode_GetPlug( conv->out_, 0 );

  /* create a very simple pixel iterator as job ticket */
  if(plug)
    pixel_access = oyPixelAccess_Create( 0,0, plug,
                                           oyPIXEL_ACCESS_IMAGE, 0 );
  error  = oyConversion_RunPixels( conv, pixel_access );

  clck = clock();
  for(i = 0; i < n*10000; ++i)
  {
    int error = 0;
    pixel_access->start_xy[0] = pixel_access->start_xy[1] = 0;
    error  = oyConversion_RunPixels( conv, pixel_access );
  }
  clck = clock() - clck;

  oyPixelAccess_Release( &pixel_access );
  oyConversion_Release( &conv );


  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyColourConvert_() sans oyPixelAcce.%s",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyColourConvert_() sans oyPixelAccess_Create()     " );
  }


  conv = oyConversion_New( 0 );
  oyFilterNode_s * in_node = oyFilterNode_NewWith( "//" OY_TYPE_STD "/root", 0, 0 );
  oyConversion_Set( conv, in_node, 0 );
  oyFilterNode_DataSet( in_node, (oyStruct_s*)input, 0, 0 );
  oyFilterNode_s * out_node = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", 0, 0 );
  oyFilterNode_DataSet( out_node, (oyStruct_s*)output, 0, 0 );
  error = oyFilterNode_Connect( in_node, "//" OY_TYPE_STD "/data",
                                out_node, "//" OY_TYPE_STD "/data", 0 );
  oyConversion_Set( conv, 0, out_node );
  plug = oyFilterNode_GetPlug( conv->out_, 0 );

  /* create a very simple pixel iterator as job ticket */
  if(plug)
    pixel_access = oyPixelAccess_Create( 0,0, plug,
                                           oyPIXEL_ACCESS_IMAGE, 0 );
  error  = oyConversion_RunPixels( conv, pixel_access );

  clck = clock();
  for(i = 0; i < n*10000; ++i)
  {
    pixel_access->start_xy[0] = pixel_access->start_xy[1] = 0;
    oyConversion_RunPixels( conv, pixel_access );
  }
  clck = clock() - clck;

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
  TEST_RUN( testStringRun, "String handling" );
  TEST_RUN( testOption, "basic oyOption_s" );
  TEST_RUN( testOptionInt,  "oyOption_s integers" );
  TEST_RUN( testOptionsCopy,  "Copy oyOptions_s" );
  TEST_RUN( testBlob, "oyBlob_s" );
  TEST_RUN( testSettings, "default oyOptions_s settings" );
  TEST_RUN( testProfiles, "Profiles reading" );
  TEST_RUN( testProfileLists, "Profile lists" );
  TEST_RUN( testMonitor,  "Monitor profiles" );
  //TEST_RUN( testDevices,  "Devices listing" );
  TEST_RUN( testRegistrationMatch,  "Registration matching" );
  TEST_RUN( testPolicy, "Policy handling" );
  TEST_RUN( testCMMDevicesListing, "CMM devices listing" );
  TEST_RUN( testCMMMonitorListing, "CMM monitor listing" );
  TEST_RUN( testCMMDBListing, "CMM DB listing" );
  TEST_RUN( testCMMmonitorDBmatch, "CMM monitor DB match" );
  TEST_RUN( testCMMsShow, "CMMs show" );
  TEST_RUN( testCMMnmRun, "CMM named colour run" );

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

