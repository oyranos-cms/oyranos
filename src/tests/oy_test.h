/** @file oy_test.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2004-2017  Kai-Uwe Behrmann
 *
 *  @brief    Oyranos test suite
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
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

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

/* C++ includes and definitions */
#ifdef __cplusplus
#include <fstream>
#include <iostream>
#define USE_NEW
#endif

oyObject_s testobj = NULL;

#ifdef USE_NEW
void* myAllocFunc(size_t size) { return new char [size]; }
void  myDeAllocFunc( void * ptr ) { delete [] (char*)ptr; }
#else
void* myAllocFunc(size_t size) { return calloc(size,1); }
void  myDeAllocFunc( void * ptr ) { free( ptr ); }
#endif


/* --- general test routines --- */

typedef enum {
  oyTESTRESULT_SYSERROR,
  oyTESTRESULT_FAIL,
  oyTESTRESULT_XFAIL,
  oyTESTRESULT_SUCCESS,
  oyTESTRESULT_UNKNOWN
} oyTESTRESULT_e;

/* true color codes */
#define RED_TC "\033[38;2;240;0;0m"
#define GREEN_TC "\033[38;2;0;250;100m"
#define BLUE_TC "\033[38;2;0;150;255m"
/* basic color codes */
#define RED_B "\033[0;31m"
#define GREEN_B "\033[0;32m"
#define BLUE_B "\033[0;34m"
/* switch back */
#define CTEND "\033[0m"
typedef enum {
  oyRED,
  oyGREEN,
  oyBLUE
} oyCOLORTERM_e;
const char * colorterm = NULL;
static const char * oyTermColor_( oyCOLORTERM_e rgb, const char * text) {
  int len = strlen(text);
  static char t[256];
  int truecolor = colorterm && strcmp(colorterm,"truecolor") == 0;
  int color = colorterm != NULL ? 1 : 0;
  if(len < 200)
  {
    switch(rgb)
    {
      case oyRED: sprintf( t, "%s%s%s", truecolor ? RED_TC : color ? RED_B : "", text, CTEND ); break;
      case oyGREEN: sprintf( t, "%s%s%s", truecolor ? GREEN_TC : color ? GREEN_B : "", text, CTEND ); break;
      case oyBLUE: sprintf( t, "%s%s%s", truecolor ? BLUE_TC : color ? BLUE_B : "", text, CTEND ); break;
    }
    return t;
  } else
    return text;
}

const char * oyTestResultToString    ( oyTESTRESULT_e      error )
{
  const char * text = "";
  switch(error)
  {
    case oyTESTRESULT_SYSERROR:text = oyTermColor_(oyRED,"SYSERROR"); break;
    case oyTESTRESULT_FAIL:    text = oyTermColor_(oyRED,"FAIL"); break;
    case oyTESTRESULT_XFAIL:   text = oyTermColor_(oyBLUE,"XFAIL"); break;
    case oyTESTRESULT_SUCCESS: text = oyTermColor_(oyGREEN,"SUCCESS"); break;
    case oyTESTRESULT_UNKNOWN: text = oyTermColor_(oyBLUE,"UNKNOWN"); break;
    default:                   text = oyTermColor_(oyBLUE,"Huuch, what's that?"); break;
  }
  return text;
}

FILE * zout;  /* printed inbetween results */
static int test_number = 0;
#define TEST_RUN( prog, text, do_it ) { \
  if(argc > argpos && do_it) { \
      for(i = argpos; i < argc; ++i) \
        if(strstr(text, argv[i]) != 0 || \
           atoi(argv[i]) == test_number ) \
          oyTestRun( prog, text, test_number ); \
  } else if(list) \
    printf( "[%d] %s\n", test_number, text); \
  else if(do_it) \
    oyTestRun( prog, text, test_number ); \
  ++test_number; \
}

int results[oyTESTRESULT_UNKNOWN+1];
#define tn 64
char * tests_failed[tn];
char * tests_xfailed[tn];

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
    tests_failed[number] = (char*)test_name;
  if(error == oyTESTRESULT_XFAIL)
    tests_xfailed[number] = (char*)test_name;
  results[error] += 1;

  /* print */
  if(error <= oyTESTRESULT_FAIL)
    fprintf(stdout, " !!! ERROR !!!" );
  fprintf(stdout, "\n" );

  return error;
}


int oy_test_sub_count = 0;
#define PRINT_SUB( result_, ... ) { \
  if((result_) < result) \
    result = result_; \
  fprintf(stdout, ## __VA_ARGS__ ); \
  fprintf(stdout, " ..\t%s", oyTestResultToString(result_)); \
  if((result_) <= oyTESTRESULT_FAIL) \
    fprintf(stdout, " !!! ERROR !!!" ); \
  fprintf(stdout, "\n" ); \
  ++oy_test_sub_count; \
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

oyTESTRESULT_e displayFail()
{
  oyTESTRESULT_e fail_type = oyTESTRESULT_XFAIL;

#if !defined(__APPLE__)
  const char * disp_env = getenv("DISPLAY");
  if(disp_env && disp_env[0])
    fail_type = oyTESTRESULT_FAIL;
#endif

  return fail_type;
}

