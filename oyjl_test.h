/** @file oyjl_test.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2004-2018  Kai-Uwe Behrmann
 *
 *  @brief    Oyranos test suite
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2008/12/04
 */

#ifndef OYJL_TEST_H
#define OYJL_TEST_H

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* C++ includes and definitions */
#ifdef __cplusplus
#include <fstream>
#include <iostream>
#include <cmath>
#define USE_NEW
#else
#include <math.h>
#endif

/** \addtogroup oyjl
 *  @{ *//* oyjl */
/** \addtogroup oyjl_test Testing
 *  @brief API testing for prototyping and regression checking in CI
 *
 *  The API is designed to be easily useable without much boilerplate.
 *  The implementation is declared and defined in header only. No
 *  extra linking is needed, except of libm and libc where required.
 *
 *  Define somewhere in your test.c file a TESTS_RUN
 *  with your test functions like:
 *  @code
#define TESTS_RUN \
  TEST_RUN( testVersion, "Version matching", 1 ); \
  TEST_RUN( testJson, "JSON handling", 1 ); \
  TEST_RUN( testFromJson, "Data Writers", 1 ); \
  TEST_RUN( testJsonRoundtrip, "Data Readers", 1 );
    @endcode
 *  Then include simply the oyjl_test_main.h header and it defines
 *  a main() function for you to handle command line parsing, statistics
 *  and summary printing after test program finish.
 *
 *  @{ *//* oyjl_test */


#ifdef USE_NEW
void* myAllocFunc(size_t size) { return new char [size]; }
void  myDeAllocFunc( void * ptr ) { delete [] (char*)ptr; }
#else
void* myAllocFunc(size_t size) { return calloc(size,1); }
void  myDeAllocFunc( void * ptr ) { free( ptr ); }
#endif


/* --- general test routines --- */

/** test result categories */
typedef enum {
  oyjlTESTRESULT_SYSERROR,           /**< system error */
  oyjlTESTRESULT_FAIL,               /**< test failed and will error */
  oyjlTESTRESULT_XFAIL,              /**< non critical failure */
  oyjlTESTRESULT_SUCCESS,            /**< tested and verified feature */
  oyjlTESTRESULT_UNKNOWN             /**< unknown error */
} oyjlTESTRESULT_e;

/* true color codes */
#define OYJL_RED_TC "\033[38;2;240;0;0m"
#define OYJL_GREEN_TC "\033[38;2;0;250;100m"
#define OYJL_BLUE_TC "\033[38;2;0;150;255m"
/* basic color codes */
#define OYJL_RED_B "\033[0;31m"
#define OYJL_GREEN_B "\033[0;32m"
#define OYJL_BLUE_B "\033[0;34m"
/* switch back */
#define OYJL_CTEND "\033[0m"
typedef enum {
  oyjlRED,
  oyjlGREEN,
  oyjlBLUE
} oyjlCOLORTERM_e;
const char * colorterm = NULL;
static const char * oyjlTermColor_( oyjlCOLORTERM_e rgb, const char * text) {
  int len = strlen(text);
  static char t[256];
  int truecolor = colorterm && strcmp(colorterm,"truecolor") == 0;
  int color = colorterm != NULL ? 1 : 0;
  if(len < 200)
  {
    switch(rgb)
    {
      case oyjlRED: sprintf( t, "%s%s%s", truecolor ? OYJL_RED_TC : color ? OYJL_RED_B : "", text, OYJL_CTEND ); break;
      case oyjlGREEN: sprintf( t, "%s%s%s", truecolor ? OYJL_GREEN_TC : color ? OYJL_GREEN_B : "", text, OYJL_CTEND ); break;
      case oyjlBLUE: sprintf( t, "%s%s%s", truecolor ? OYJL_BLUE_TC : color ? OYJL_BLUE_B : "", text, OYJL_CTEND ); break;
    }
    return t;
  } else
    return text;
}

const char * oyjlTestResultToString  ( oyjlTESTRESULT_e      error )
{
  const char * text = "";
  switch(error)
  {
    case oyjlTESTRESULT_SYSERROR:text = oyjlTermColor_(oyjlRED,"SYSERROR"); break;
    case oyjlTESTRESULT_FAIL:    text = oyjlTermColor_(oyjlRED,"FAIL"); break;
    case oyjlTESTRESULT_XFAIL:   text = oyjlTermColor_(oyjlBLUE,"XFAIL"); break;
    case oyjlTESTRESULT_SUCCESS: text = oyjlTermColor_(oyjlGREEN,"SUCCESS"); break;
    case oyjlTESTRESULT_UNKNOWN: text = oyjlTermColor_(oyjlBLUE,"UNKNOWN"); break;
    default:                   text = oyjlTermColor_(oyjlBLUE,"Huuch, what's that?"); break;
  }
  return text;
}

/** FILE descriptor for printed inbetween results
 *
 *  A good default might be stdout for a CLI program.
 */
FILE * zout;
static int test_number = 0;
const char * oyjl_test_file = NULL;
int oyjl_test_file_line = -1;
void oyjlSetDbgPosition( const char * file, int line )
{
  oyjl_test_file = file;
  oyjl_test_file_line = line;
}
#define OYJL_TEST_START oyjlSetDbgPosition(__FILE__,__LINE__-1);
/** macro to register a test
 *  @see TESTS_RUN
 *  @param         prog                test function: oyjlTESTRESULT_e  (*test)(void)
 *  @param         text                name of the test
 *  @param         do_it               enable the test - usually 1
 */
#define TEST_RUN( prog, text, do_it ) \
oyjlTESTRESULT_e prog(); \
{ \
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

int results[oyjlTESTRESULT_UNKNOWN+1];
#define tn 64
char * tests_failed[tn];
char * tests_xfailed[tn];

#ifndef MAX_PATH
/* maximal path lenght, if not allready defined elsewhere */
#define MAX_PATH 1024
#endif

/** run a test and print results on end
 *  @param         test                test function
 *  @param         test_name           short string for status line
 *  @param         number              internal test number
 */
oyjlTESTRESULT_e oyTestRun           ( oyjlTESTRESULT_e  (*test)(void),
                                       const char        * test_name,
                                       int                 number )
{
  oyjlTESTRESULT_e error = oyjlTESTRESULT_UNKNOWN;
  char * text = (char*) malloc(strlen(test_name) + (MAX_PATH) + 80);

  oyjl_test_file = NULL;
  oyjl_test_file_line = -1;

  fprintf( stdout, "\n________________________________________________________________\n" );
  fprintf(stdout, "Test[%d]: %s ... ", test_number, test_name );

  error = test();

  fprintf(stdout, "\t%s", oyjlTestResultToString(error));

  if(oyjl_test_file && oyjl_test_file_line)
    sprintf( text, "%s (%s:%d)", test_name, strchr(oyjl_test_file,'/')?strrchr(oyjl_test_file,'/') + 1 : oyjl_test_file, oyjl_test_file_line );
  else
    sprintf( text, "%s", test_name );
  if(error == oyjlTESTRESULT_FAIL)
    tests_failed[number] = text;
  if(error == oyjlTESTRESULT_XFAIL)
    tests_xfailed[number] = text;
  results[error] += 1;

  /* print */
  if(error <= oyjlTESTRESULT_FAIL)
    fprintf(stdout, " !!! ERROR !!!" );
  fprintf(stdout, "\n" );

  return error;
}


int oy_test_sub_count = 0;
/** @brief register status and print info of sub test
 *
 *  Print a custom line to stdout followed by the status. Register state.
 *
 *  The PRINT_SUB macro remembers the first file position of similar strongly
 *  failed sub tests. As macros count the last closing brace ')', the
 *  line number is set to (\_\_LINE\_\_ - 1). So it is suggested to place
 *  the status macro in one line to let the position fall in front or use
 *  a two line macro with falling the debug position in the start of the
 *  macro. Here two examples:
 *  @code
    int i = 4; // debugging hint will show this line
    if(i != 2) PRINT_SUB(oyjlTESTRESULT_FAIL, "i = %d", i);

    if(i == 2)
    {
      PRINT_SUB( oyjlTESTRESULT_SUCCESS,
      "i = %d", i );
    }
    else
    {
      // debugging hint will point to line below
      PRINT_SUB( oyjlTESTRESULT_XFAIL,
      "i = %d", i );
    }
    @endcode
 *
 *  @param         result_             use oyjlTESTRESULT_e for
 *  @param         ...                 the argument list to fprint(stdout, ...)
 */
#define PRINT_SUB( result_, ... ) { \
  if((result_) < oyjlTESTRESULT_SUCCESS && (result_) < result) OYJL_TEST_START \
  if((result_) < result) \
    result = result_; \
  fprintf(stdout, ## __VA_ARGS__ ); \
  fprintf(stdout, " ..\t%s", oyjlTestResultToString(result_)); \
  if((result_) <= oyjlTESTRESULT_FAIL) \
    fprintf(stdout, " !!! ERROR !!!" ); \
  fprintf(stdout, "\n" ); \
  ++oy_test_sub_count; \
}

/** helper to print a number inside ::PRINT_SUB(...) */
const char  *  oyjlIntToString      ( int                 integer )
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

/** helper to print tempo in ::PRINT_SUB(...) */
const char  *  oyjlProfilingToString ( int                 integer,
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

/** helper to check for availablity of display environment for test status */
oyjlTESTRESULT_e displayFail()
{
  oyjlTESTRESULT_e fail_type = oyjlTESTRESULT_XFAIL;

#if !defined(__APPLE__)
  const char * disp_env = getenv("DISPLAY");
  if(disp_env && disp_env[0])
    fail_type = oyjlTESTRESULT_FAIL;
#endif

  return fail_type;
}

#if defined(_WIN32) && !defined(__GNU__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h> // portable: uint64_t   MSVC: __int64 

// MSVC defines this in winsock2.h!?
typedef struct timeval {
    long tv_sec;
    long tv_usec;
} timeval;

int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}
# define   TIME_DIVIDER CLOCKS_PER_SEC
#else
# include <time.h>
# include <sys/time.h>
# define   TIME_DIVIDER 10000
# include <unistd.h>
#endif

time_t             oyjlTime          ( )
{
  time_t time_;
  double divider = TIME_DIVIDER;
  struct timeval tv;
  double tmp_d;
  gettimeofday( &tv, NULL );
  time_ = tv.tv_usec/(1000000/(time_t)divider)
                   + (time_t)(modf( (double)tv.tv_sec / divider,&tmp_d )
                     * divider*divider);
  return time_;
}
double             oyjlSeconds       ( )
{
           time_t zeit_ = oyjlTime();
           double teiler = TIME_DIVIDER;
           double dzeit = zeit_ / teiler;
    return dzeit;
}
double             oyjlClock         ( )
{ return oyjlSeconds()*1000000; }

/** @} *//* oyjl_test */
/** @} *//* oyjl */

#endif /* OYJL_TEST_H */
