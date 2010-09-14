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


#include "oyranos.h"
#include "config.h"

/* C++ includes and definitions */
#ifdef __cplusplus
void* myAllocFunc(size_t size) { return new char [size]; }
#include <fstream>
#include <iostream>
#else
void* myAllocFunc(size_t size) { return calloc(size,1); }
#endif

#          if defined(__GNUC__) || defined(LINUX) || defined(APPLE) || defined(SOLARIS)
# include <sys/time.h>
# define   ZEIT_TEILER 10000
#          else // WINDOWS TODO
# define   ZEIT_TEILER CLOCKS_PER_SEC;
#          endif

#ifndef WIN32
# include <unistd.h>
#endif

#include <ctime>
#include <cmath>


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

time_t             oyTime            ( )
{
           time_t zeit_;
           double teiler = ZEIT_TEILER;
#          if defined(__GNUC__) || defined(APPLE) || defined(SOLARIS) || defined(BSD)
           struct timeval tv;
           gettimeofday( &tv, NULL );
           double tmp_d;
           zeit_ = tv.tv_usec/(1000000/(time_t)teiler)
                   + (time_t)(modf( (double)tv.tv_sec / teiler,&tmp_d )
                     * teiler*teiler);
#          else // WINDOWS TODO
           zeit_ = clock();
#          endif
    return zeit_;
}
double             oySeconds         ( )
{
           time_t zeit_ = oyTime();
           double teiler = ZEIT_TEILER;
           double dzeit = zeit_ / teiler;
    return dzeit;
}
double             oyClock           ( )
{ return oySeconds()*1000000; }

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
    test_sub = oyFilterRegistrationToText( test, oyFILTER_REG_OPTION, 0 );
  clck = oyClock() - clck;

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

  error = oyOption_SetFromInt( o, 58293, 1, 0 );
  if(!error && o->value &&
     o->value->int32_list[0] == 3 &&
     o->value->int32_list[1] == 58293 &&
     o->value->int32_list[2] == 58293 &&
     o->value->int32_list[3] == 58293 &&
     o->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() new int32_t list good     " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromInt() new int32_t list failed   " );
  }

  error = oyOption_SetFromInt( o, 293, 1, 0 );
  if(!error && o->value &&
     o->value->int32_list[0] == 3 &&
     o->value->int32_list[1] == 58293 &&
     o->value->int32_list[2] == 293 &&
     o->value->int32_list[3] == 58293 &&
     o->value_type == oyVAL_INT_LIST)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOption_SetFromInt() modify int32_t list good  " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOption_SetFromInt() modify int32_t list failed" );
  }

  error = oyOption_SetFromInt( o, 58293, 0, 0 );
  if(!error && o->value &&
     o->value->int32_list[0] == 3 &&
     o->value->int32_list[1] == 58293 &&
     o->value->int32_list[2] == 293 &&
     o->value->int32_list[3] == 58293 &&
     o->value_type == oyVAL_INT_LIST)
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


  o = oyOption_New( "//" OY_TYPE_STD "/filter/y", 0 );
  o->value_type = oyVAL_INT_LIST;
  error = oyOption_SetFromInt( o, 58293, 2, 0 );
  if(!error && o->value &&
     o->value->int32_list[0] == 3 &&
     o->value->int32_list[1] == 0 &&
     o->value->int32_list[2] == 0 &&
     o->value->int32_list[3] == 58293 &&
     o->value_type == oyVAL_INT_LIST)
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

  fprintf(stdout, "\n" );

  error = oyOptions_SetFromText( &setA,
                                 "//" OY_TYPE_STD "/filter/gamma_A",
                                 "1", OY_CREATE_NEW );
  error = oyOptions_SetFromText( &setA,
                                 "//" OY_TYPE_STD "/filter/gamma_A1",
                                 "1", OY_CREATE_NEW );
  error = oyOptions_SetFromText( &setA,
                                 "//" OY_TYPE_STD "/filter/gamma_A12",
                                 "1", OY_CREATE_NEW );

  if(!error && oyOptions_Count( setA ) == 3)
  { PRINT_SUB( oyTESTRESULT_SUCCESS, 
    "oyOptions_SetFromText() similiar registration good  " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL, 
    "oyOptions_SetFromText() similiar registration failed" );
  }

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

  a = oyConfDomain_FromReg( "shared/freedesktop.org/imaging/config.device.icc_profile.monitor.oyX1", 0 );
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
    fprintf( stdout, "%d: %s\n", i, domains[i] );
  }
  fprintf( stdout, "\n");
  
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
        printf("\"%s\" =\n  \"%s\" \"%s\" \"%s\"\n", texts[j],
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
    fprintf( stdout, "----------\n");
  }
  oyStringListRelease_( &domains, count, free );


  return result;
}

oyTESTRESULT_e testProfile ()
{
  oyTESTRESULT_e result = oyTESTRESULT_UNKNOWN;

  size_t size = 0;
  oyPointer data;
  oyProfile_s * p_a,
              * p_b;

  oyExportReset_(EXPORT_SETTING);

  fprintf(stdout, "\n" );

  p_a = oyProfile_FromStd ( oyASSUMED_WEB, NULL );
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
    "oyPolicyToXML                         " );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyPolicyToXML                         " );
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
  oyConfigDomainList( "//"OY_TYPE_STD"/config.device.icc_profile",
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
    fprintf( stdout, "%d: %s\n", i, texts[i] );
  }
  fprintf(stdout, "\n" );

  oyConfigs_s * configs = 0;
  oyConfig_s * config = 0;
  oyOptions_s * options_list = 0;
  oyOption_s * o = 0;
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
  fprintf( stdout, "\n");


  /* add list call to module arguments */
  error = oyOptions_SetFromText( &options_list,
                                 "//" OY_TYPE_STD "/config/command", "list",
                                 OY_CREATE_NEW );
  error = oyOptions_SetFromText( &options_list,
                                 "//" OY_TYPE_STD "/config/icc_profile",
                                 "true", OY_CREATE_NEW );

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
      oyConfigs_s * dbs = 0,
                  * heap = 0;
      int precise_count = 0,
          serial_count = 0,
          mnft_count = 0,
          dev_name_count = 0;

      config = oyConfigs_Get( configs, j );

      fprintf(stdout, "--------------------------------------------------------------------------------\n\"%s\":\n", oyConfig_FindString( config, "device_name", 0 ) );
      {
        oyOptions_s * options = 0;
        const char * t = 0;
        oyOptions_SetFromText( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.net_color_region_target",
                         "yes", OY_CREATE_NEW );
        error = oyDeviceGetProfile( config, options, &p );
        oyOptions_Release( &options );
        t = oyProfile_GetText( p, oyNAME_DESCRIPTION);
        printf( "oyDeviceGetProfile(): \"%s\"\n", t ? t : "----" );
        oyProfile_Release( &p );
      }

      error = oyConfigs_FromDB( config->registration, &heap, 0 );

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


      printf( "\"%s\" has %d precise matches,\n"
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

        r = oyFilterRegistrationToText( o->registration,
                                        oyFILTER_REG_OPTION, 0 );
        if(r && strcmp(r,"supported_devices_info") == 0 &&
           o->value_type == oyVAL_STRING_LIST)
        {
          /* print first line special */
          while(o->value->string_list[++mnft])
          {
            mn = 0; pos = -1;
            while(o->value->string_list[mnft][++pos])
            {
              if(o->value->string_list[mnft][pos] == '\n')
              {
                if(mn && o->value->string_list[mnft][pos+1])
                  putc(',', stdout);
                else if(mn == 0)
                {
                  putc(':', stdout);
                  putc('\n', stdout);
                  putc(' ', stdout);
                  putc(' ', stdout);
                }
                ++mn;

              } else
                putc(o->value->string_list[mnft][pos], stdout);
            }
            putc('\n', stdout);
          }
        } else
        {
          val = oyOption_GetValueText( o, oyAllocateFunc_ );
          printf("  %d::%d::%d \"%s\": \"%s\"\n", i,j,k,
                 o->registration, val?val:"(nix)" );
        }

        if(r) oyDeAllocateFunc_(r); r = 0;
        if(val)
          oyDeAllocateFunc_( val ); val = 0;
        oyOption_Release( &o );
      }

      oyConfig_Release( &config );
    }

    oyConfigs_Release( &configs );
  }
  fprintf( stdout, "\n");
  oyOptions_Release( &options_list );

  fprintf( stdout, "\n");

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

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif

  /* get all configuration filters */
  oyConfigDomainList( "//"OY_TYPE_STD"/config.device.icc_profile",
                      &texts, &count, &rank_list ,0 );

  if( count )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyConfigDomainList Found CMM's %d     ", (int)count );
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyConfigDomainList Found CMM's %d     ", (int)count );
  }

  oyConfigs_s * configs = 0;
  oyConfig_s * config = 0,
             * device = 0;
  oyOptions_s * options = 0;
  oyOption_s * o = 0;
  int devices_n = 0;


  fprintf( stdout, "oyConfigs_FromDomain() \"properties\" call:\n" );
  for( i = 0; i < (int)count; ++i)
  {
    const char * registration_domain = texts[i];
    printf("%d[rank %d]: %s\n", i, rank_list[i], registration_domain);

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
      fprintf(stdout, "--------------------------------------------------------------------------------\n%s:\n", registration_domain );
      config = oyConfigs_Get( configs, l );

      k_n = oyConfig_Count( config );
      for( k = 0; k < k_n; ++k )
      {
        o = oyConfig_Get( config, k );

        val = oyOption_GetValueText( o, oyAllocateFunc_ );
        printf( "  %d::%d %s: \"%s\"\n", l,k, 
                  oyStrrchr_(o->registration,'/')+1, val );

        if(val) oyDeAllocateFunc_( val ); val = 0;
        oyOption_Release( &o );
      }

      o = oyConfig_Find( device, "icc_profile" );
      if(o)
      {
        val = oyOption_GetValueText( o, oyAllocateFunc_ );
        printf( "  %d %s: \"%s\"\n", l, 
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

  fprintf( stdout, "\n");

  if(texts && texts[0])
    config = oyConfig_New( texts[0], 0 );
  error = oyConfig_AddDBData( config, "k1", "bla1", OY_CREATE_NEW );
  error = oyConfig_AddDBData( config, "k2", "bla2", OY_CREATE_NEW );
  error = oyConfig_AddDBData( config, "k3", "bla3", OY_CREATE_NEW );

  if( !error  && config && oyOptions_Count( config->db ))
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
#include "oforms/oyranos_forms.h"

#define H(type,value) oyFormsAddHeadline( &t, type, value,\
                                           oyAllocateFunc_, oyDeAllocateFunc_ );
#define CHOICE(ref,label,help) oyFormsStartChoice( &t, ref, label, help,\
                                           oyAllocateFunc_, oyDeAllocateFunc_ );
#define ITEM(value,label) oyFormsAddItem( &t, value, label,\
                                           oyAllocateFunc_, oyDeAllocateFunc_ );
#define CHOICE_END STRING_ADD( t, "      </xf:choices>\n     </xf:select1>\n" );

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
  oyCMMInfo_s * cmm_info = 0;
  oyCMMapi4_s * cmm_api4 = 0;
  oyCMMapi6_s * cmm_api6 = 0;
  oyCMMapi7_s * cmm_api7 = 0;
  oyCMMapi8_s * cmm_api8 = 0;
  oyCMMapi9_s * cmm_api9 = 0;
  oyCMMapi10_s * cmm_api10 = 0;
  oyCMMapi_s * tmp = 0;
  oyCMMapiFilter_s * cmm_filter = 0;


  fprintf(stdout, "\n" );

  texts = oyCMMsGetLibNames_( &count, 0 );

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
    cmm_info = oyCMMInfoFromLibName_( texts[i] );
    text = oyCMMInfoPrint_( cmm_info );
    tmp = cmm_info->api;

        while(tmp)
        {
          oyOBJECT_e type = oyOBJECT_NONE;
          char num[48],
               * api_reg = 0;

          /* oforms */
          CHOICE( "shared/dummy", oyStructTypeToText(tmp->type), text )
          ITEM( "0", cmm_info->cmm )
          CHOICE_END

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
              const char * nick = cmm_info->cmm;

              classe = oyFilterRegistrationToText( cmm_filter->registration,
                                                   oyFILTER_REG_TYPE, 0 );
              api_reg = oyStringCopy_("//", oyAllocateFunc_ );
              STRING_ADD( api_reg, classe );
              if(classe)
              oyFree_m_( classe );


              H( 4, "API(s) load from Meta module" )
              STRING_ADD( text, "    API(s) load from Meta module:\n" );

              for(j = oyOBJECT_CMM_API4_S; j <= (int)oyOBJECT_CMM_API10_S; j++)
              {
                apis = oyCMMsGetFilterApis_( nick, 0, api_reg, (oyOBJECT_e)j,
                                             oyFILTER_REG_MODE_NONE,
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
                  /* oforms */
                  CHOICE( "shared/dummy", oyStructTypeToText(api->type), text_tmp )
                  ITEM( "0", api->registration )
                  CHOICE_END

                  if(api->type == oyOBJECT_CMM_API4_S)
                  {
                    cmm_api4 = (oyCMMapi4_s*) api;
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
                                             0,0, /* oyCMMapi_s stuff */
                                             (char*)"invalid/registration",
                                             {0,0,0}, /* version */
                                             {0,0,0}, /* module_api */
                                             0,0, /* Oyranos stuff */
                                             0, /* oyConfigs_FromPattern */
                                             0, /* oyConfigs_Modify */
                                             0, /* oyConfig_Rank */
                                             0, /* oyCMMui_s */
                                             0, /* oyIcon_s */
                                             (oyRankPad*)rank_map
                                           };

                    cpp_api8.version[2] = 1;

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

                  if(api->type == oyOBJECT_CMM_API10_S)
                  {
                    cmm_api10 = (oyCMMapi10_s*) api;
                    for(l = 0; l < 3; ++l)
                    {
                      if(cmm_api10->texts[l])
                      {
                        snprintf( text_tmp, 65535,
                            "        \"%s\":\n"
                            "        \"%s\"\n",
                            cmm_api10->texts[l],
                            cmm_api10->getText( cmm_api10->texts[l],
                                                oyNAME_DESCRIPTION) );
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

  STRING_ADD( t, "     </xf:group>\n</body>\n</html>\n" );
  remove("test2_CMMs.xhtml");
  oyWriteMemToFile2_( "test2_CMMs.xhtml", t, strlen(t),0/*OY_FILE_NAME_SEARCH*/,
                      &rfile, malloc );
  printf( "Wrote %s\n", rfile?rfile:"test2_CMMs.xhtml" );
  free( rfile );

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
  double clck = oyClock(), clck2;
  for(i = 0; i < n*10000; ++i)
  {
    c = oyNamedColour_Create( NULL, NULL,0, prof, 0 );
    oyNamedColour_Release( &c );
  }
  clck = oyClock() - clck;

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
  clck = oyClock();

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
  clck = oyClock() - clck;

  if( i )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyGetKeyString_()                   %s",
                  oyProfilingToString(n,clck/(double)CLOCKS_PER_SEC, "Filter"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyGetKeyString_()                                  " );
  }



  clck = oyClock();
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
  clck = oyClock() - clck;

  if( i )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOption_SetValueFromDB()           %s",
                  oyProfilingToString(n,clck/(double)CLOCKS_PER_SEC, "Filter"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOption_SetValueFromDB()                          " );
  }

  clck = oyClock();
  for(i = 0; i < n*3; ++i)
  {
  oyFilterCore_s * filter = oyFilterCore_New_( 0 );
  oyCMMapi4_s * api4 = 0;
  oyObject_s object = 0;

  if(error <= 0)
  {
    api4 = (oyCMMapi4_s*) oyCMMsGetFilterApi_( 0,
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
        3. parse static common options from meta module
        4. parse static options from filter 
        5. merge both
        6. get stored values from disk
     */

    /*  1. get filter */

    /*  2. get implementation for filter type */
    api5 = filter->api4_->api5_;

    /*  3. parse static common options from meta module */
    if(api5 && flags & OY_SELECT_COMMON)
    {
      oyCMMapiFilters_s * apis;
      int apis_n = 0;
      uint32_t         * rank_list = 0;
      oyCMMapi9_s * cmm_api9 = 0;
      char * klass, * api_reg;

      klass = oyFilterRegistrationToText( filter->registration_,
                                          oyFILTER_REG_TYPE, 0 );
      api_reg = oyStringCopy_("//", oyAllocateFunc_ );
      STRING_ADD( api_reg, klass );
      oyFree_m_( klass );

      s = oyOptions_New( 0 );

      apis = oyCMMsGetFilterApis_( 0,0, api_reg,
                                   oyOBJECT_CMM_API9_S,
                                   oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                   &rank_list, 0);
      apis_n = oyCMMapiFilters_Count( apis );
      for(i = 0; i < apis_n; ++i)
      {
        cmm_api9 = (oyCMMapi9_s*) oyCMMapiFilters_Get( apis, i );
        if(oyFilterRegistrationMatch( filter->registration_, cmm_api9->pattern,
                                      oyOBJECT_NONE ))
        {
          opts_tmp = oyOptions_FromText( cmm_api9->options, 0, object );
          oyOptions_AppendOpts( s, opts_tmp );
          oyOptions_Release( &opts_tmp );
        }
        if(cmm_api9->release)
          cmm_api9->release( (oyStruct_s**)&cmm_api9 );
      }
      oyCMMapiFilters_Release( &apis );
      oyFree_m_( api_reg );
      opts_tmp = s; s = 0;
    }
    /* requires step 2 */

    /*  4. parse static options from filter */
    if(flags & OY_SELECT_FILTER)
      opts_tmp2 = oyOptions_FromText( filter->api4_->ui->options, 0, object );

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
      if(!(flags & oyOPTIONSOURCE_FILTER))
        error = oyOption_SetValueFromDB( o );
      oyOption_Release( &o );
    }
    error = oyOptions_DoFilter ( s, flags, type_txt );
  }

  if(type_txt)
    oyDeAllocateFunc_( type_txt );

  }
  clck = oyClock() - clck;

  if( i )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOptions_ForFilter_()              %s",
                  oyProfilingToString(n,clck/(double)CLOCKS_PER_SEC, "Filter"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptions_ForFilter_()                             " );
  }

  clck = oyClock();
  {
  oyFilterCore_s * s = oyFilterCore_New_( 0 );
  int error = !s;
  uint32_t ret = 0;
  oyOptions_s * opts_tmp = 0, * options = 0;
  oyCMMapi4_s * api4 = 0;

  if(error <= 0)
  {
    api4 = (oyCMMapi4_s*) oyCMMsGetFilterApi_( 0,
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
  clck = oyClock() - clck;

  if( i )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyOptions_ForFilter_()              %s",
                  oyProfilingToString(n,clck/(double)CLOCKS_PER_SEC, "Filter"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyOptions_ForFilter_()                             " );
  }



  oyOptions_s * options = oyOptions_New(0);
  clck = oyClock();
  for(i = 0; i < n*3*10000; ++i)
  {
    oyFilterCore_s * core = oyFilterCore_New( "//" OY_TYPE_STD "/root",
                                              options,0 );
    if(!core) break;
    oyFilterCore_Release( &core );
  }
  clck = oyClock() - clck;

  if( i )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyFilterCore_New()                  %s",
                 oyProfilingToString(i/3,clck/(double)CLOCKS_PER_SEC, "Cores"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyFilterCore_New()                                 " );
  }


  clck = oyClock();
  const char * registration = "//" OY_TYPE_STD "/root";
  for(i = 0; i < n*3*10000; ++i)
  {
    oyCMMapi4_s * api4 = 0;
    api4 = (oyCMMapi4_s*) oyCMMsGetFilterApi_( 0,
                                            registration, oyOBJECT_CMM_API4_S );
    error = !api4;
    if(!(i%30000)) fprintf(stdout, "." ); fflush(stdout);
  }
  fprintf(stdout,"\n");
  clck = oyClock() - clck;

  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "oyFilterCore_New() oyCMMapi4_s      %s",
                 oyProfilingToString(i/3,clck/(double)CLOCKS_PER_SEC, "Cores"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "oyFilterCore_New() oyCMMapi4_s                     " );
  }


  clck = oyClock();
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
  clck = oyClock() - clck;

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
  if(s && s->out_)
    plug = oyFilterNode_GetPlug( s->out_, 0 );
  else
    error = 1;
  pixel_access = oyPixelAccess_Create( 0,0, plug,
                                           oyPIXEL_ACCESS_IMAGE, 0 );
  oyFilterPlug_Release( &plug );

  clck = oyClock();
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


  clck = oyClock();
  for(i = 0; i < 100*n; ++i)
  if(error <= 0)
  {
    s = oyConversion_CreateBasicPixels( input,output, options, 0 );
    error  = oyConversion_RunPixels( s, 0 );
    oyConversion_Release ( &s );
    if(!(i%100)) fprintf(stdout, "." ); fflush(stdout);
  }
  fprintf(stdout, "\n" );

  clck = oyClock() - clck;
  if( !error )
  { PRINT_SUB( oyTESTRESULT_SUCCESS,
    "+ oyConversion_RunPixels()          %s",
                          oyProfilingToString(i,clck/(double)CLOCKS_PER_SEC, "Pixel"));
  } else
  { PRINT_SUB( oyTESTRESULT_FAIL,
    "+ oyConversion_RunPixels()                         " );
  }



  clck = oyClock();

  for(i = 0; i < n*100 && error <= 0; ++i)
  {
    l_error = oyNamedColour_SetColourStd ( c, oyASSUMED_WEB,
                                           (oyPointer)d, oyDOUBLE, 0, options );
    if(error <= 0)
      error = l_error;
  }
  clck = oyClock() - clck;

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

  clck = oyClock();
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
  clck = oyClock() - clck;


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
  if(conv)
    plug = oyFilterNode_GetPlug( conv->out_, 0 );

  /* create a very simple pixel iterator as job ticket */
  if(plug)
    pixel_access = oyPixelAccess_Create( 0,0, plug,
                                           oyPIXEL_ACCESS_IMAGE, 0 );
  error  = oyConversion_RunPixels( conv, pixel_access );

  clck = oyClock();
  if(pixel_access)
  for(i = 0; i < n*10000; ++i)
  {
    int error = 0;
    pixel_access->start_xy[0] = pixel_access->start_xy[1] = 0;
    error  = oyConversion_RunPixels( conv, pixel_access );
  }
  clck = oyClock() - clck;

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

  clck = oyClock();
  for(i = 0; i < n*10000; ++i)
  {
    pixel_access->start_xy[0] = pixel_access->start_xy[1] = 0;
    oyConversion_RunPixels( conv, pixel_access );
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
  TEST_RUN( testOptionsSet,  "Set oyOptions_s" );
  TEST_RUN( testOptionsCopy,  "Copy oyOptions_s" );
  TEST_RUN( testBlob, "oyBlob_s" );
  TEST_RUN( testSettings, "default oyOptions_s settings" );
  TEST_RUN( testConfDomain, "oyConfDomain_s");
  TEST_RUN( testProfile, "Profile handling" );
  TEST_RUN( testProfiles, "Profiles reading" );
  TEST_RUN( testProfileLists, "Profile lists" );
  //TEST_RUN( testMonitor,  "Monitor profiles" );
  //TEST_RUN( testDevices,  "Devices listing" );
  TEST_RUN( testRegistrationMatch,  "Registration matching" );
  TEST_RUN( testPolicy, "Policy handling" );
  TEST_RUN( testCMMDevicesListing, "CMM devices listing" );
  TEST_RUN( testCMMDevicesDetails, "CMM devices details" );
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

