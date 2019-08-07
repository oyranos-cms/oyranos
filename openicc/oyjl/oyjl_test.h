/** @file oyjl_test.h
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

#ifndef OYJL_TEST_H
#define OYJL_TEST_H

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


/* C++ includes and definitions */
#ifdef __cplusplus
#include <fstream>
#include <iostream>
#include <cmath>
#define USE_NEW
#else
#include <math.h>
#endif

extern int * oyjl_debug;

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
#define OYJL_BOLD "\033[1m"
#define OYJL_ITALIC "\033[3m"
#define OYJL_UNDERLINE "\033[4m"
#define OYJL_RED_B "\033[0;31m"
#define OYJL_GREEN_B "\033[0;32m"
#define OYJL_BLUE_B "\033[0;34m"
/* switch back */
#define OYJL_CTEND "\033[0m"
typedef enum {
  oyjlRED,
  oyjlGREEN,
  oyjlBLUE,
  oyjlBOLD,
  oyjlITALIC,
  oyjlUNDERLINE
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
      default: sprintf( t, "%s", text ); break;
    }
    return t;
  } else
    return text;
}

const char * oyjlTestResultToString  ( oyjlTESTRESULT_e      error, int use_color )
{
  const char * text = "";
#define S( color, string ) use_color ? oyjlTermColor_(color,string) : string
  switch(error)
  {
    case oyjlTESTRESULT_SYSERROR:text = S(oyjlRED,"SYSERROR"); break;
    case oyjlTESTRESULT_FAIL:    text = S(oyjlRED,"FAIL"); break;
    case oyjlTESTRESULT_XFAIL:   text = S(oyjlBLUE,"XFAIL"); break;
    case oyjlTESTRESULT_SUCCESS: text = S(oyjlGREEN,"SUCCESS"); break;
    case oyjlTESTRESULT_UNKNOWN: text = S(oyjlBLUE,"UNKNOWN"); break;
    default:                   text = S(oyjlBLUE,"Huuch, what's that?"); break;
  }
#undef S
  return text;
}

/** FILE descriptor for printed inbetween results
 *
 *  A good default might be stdout for a CLI program.
 */
FILE * zout;
int oyjl_test_number = 0;
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
           atoi(argv[i]) == oyjl_test_number ) \
          oyTestRun( prog, text, oyjl_test_number ); \
  } else if(list) \
    printf( "[%d] %s\n", oyjl_test_number, text); \
  else if(do_it) \
    oyTestRun( prog, text, oyjl_test_number ); \
  ++oyjl_test_number; \
}

#ifndef OYJL_TEST_NAME
/** Macro to set the global test name. It will be used as file name component.
 *  in ::OYJL_TEST_WRITE_RESULT.
 */
#define OYJL_TEST_NAME argv[0]
#endif /* OYJL_TEST_NAME */

int results[oyjlTESTRESULT_UNKNOWN+1];
#ifndef OYJL_TEST_MAX_COUNT
#define OYJL_TEST_MAX_COUNT 64
#endif
char * tests_failed[OYJL_TEST_MAX_COUNT];
char * tests_xfailed[OYJL_TEST_MAX_COUNT];

#ifndef MAX_PATH
/* maximal path lenght, if not allready defined elsewhere */
#define MAX_PATH 1024
#endif

int oy_test_current_sub_count;
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
  char * text = NULL;

  oyjl_test_file = NULL;
  oyjl_test_file_line = -1;
  oy_test_current_sub_count = 0;

  fprintf( stdout, "\n________________________________________________________________\n" );
  fprintf(stdout, "Test[%d]: %s ... ", number, test_name );

  error = test();

  fprintf(stdout, "\t%s", oyjlTestResultToString(error,1));

  if(error == oyjlTESTRESULT_FAIL || error == oyjlTESTRESULT_XFAIL)
  {
    text = (char*) malloc(strlen(test_name) + (MAX_PATH) + 80);

    if(oyjl_test_file && oyjl_test_file_line)
      sprintf( text, "%s (%s:%d)", test_name, strchr(oyjl_test_file,'/')?strrchr(oyjl_test_file,'/') + 1 : oyjl_test_file, oyjl_test_file_line );
    else
      sprintf( text, "%s", test_name );
    if(error == oyjlTESTRESULT_FAIL)
      tests_failed[number] = text;
    if(error == oyjlTESTRESULT_XFAIL)
      tests_xfailed[number] = text;
  }

  results[error] += 1;

  /* print */
  if(error <= oyjlTESTRESULT_FAIL)
    fprintf(stdout, " !!! ERROR !!!" );
  fprintf(stdout, "\n" );

  return error;
}


int oy_test_sub_count = 0;
oyjlTESTRESULT_e oy_test_last_result = oyjlTESTRESULT_UNKNOWN;
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
  oy_test_last_result = result_; \
  if((result_) < oyjlTESTRESULT_SUCCESS && (result_) < result) OYJL_TEST_START \
  if((result_) < result) \
    result = result_; \
  fprintf(stdout, ## __VA_ARGS__ ); \
  fprintf(stdout, " ..\t%s", oyjlTestResultToString(result_,1)); \
  if((result_) <= oyjlTESTRESULT_FAIL) \
    fprintf(stdout, " !!! ERROR !!!" ); \
  fprintf(stdout, "\n" ); \
  ++oy_test_sub_count; \
  ++oy_test_current_sub_count; \
}
int  oyjlWriteTestFile               ( const char        * filename,
                                       const void        * mem,
                                       int                 size )
{
  FILE *fp = 0;
  int r = !filename;
  int written_n = 0;

  if(!r)
  {
    fp = fopen(filename, "wb");
    if ((fp != 0)
     && mem
     && size)
    {
      written_n = fwrite( mem, 1, size, fp );
      if(written_n != size)
        r = errno;
    } else 
      if(mem && size)
        r = errno;
      else
        fprintf( zout, "no data to write into: \"%s\"", filename );

    if(r && *oyjl_debug > 1)
    {
      switch (errno)
      {
        case EACCES:       fprintf( zout, "Permission denied: %s", filename); break;
        case EIO:          fprintf( zout, "EIO : %s", filename); break;
        case ENAMETOOLONG: fprintf( zout, "ENAMETOOLONG : %s", filename); break;
        case ENOENT:       fprintf( zout, "A component of the path/file_name does not exist, or the file_name is an empty string: \"%s\"", filename); break;
        case ENOTDIR:      fprintf( zout, "ENOTDIR : %s", filename); break;
#ifdef HAVE_POSIX
        case ELOOP:        fprintf( zout, "Too many symbolic links encountered while traversing the path: %s", filename); break;
        case EOVERFLOW:    fprintf( zout, "EOVERFLOW : %s", filename); break;
#endif
        default:           fprintf( zout, "%s : %s", strerror(errno), filename);break;
      }
    }

    if (fp) fclose (fp);
  }

  return written_n;
}

/** @brief Store test data
 *
 *  Write test data to a file for easy error comparision. Use the macro after
 *  the PRINT_SUB() of the belonging sub test. The macro will generate the
 *  file name containing test related infos, like test number and sub test
 *  number, status and possibly your provided hint. That way you can easily
 *  find your test data in the writeable test directory. In case one test
 *  was successful followed by a failed test and the suffix is "txt", then
 *  the diff of the successful and the failed file will be shown.
 *
 *  @code
    char * text = myApiTesting(); // shall return "good result"
    if(strcmp(text,"good result") == 0) // good
      PRINT_SUB( oyjlTESTRESULT_SUCCESS, "myApiTesting()" );
    else // fail
      PRINT_SUB( oyjlTESTRESULT_FAIL, "myApiTesting()" );
    OYJL_TEST_WRITE_RESULT( data, strlen(text), "myApiTesting", "txt" )
    @endcode
 *
 *  @param         mem                 memory pointer
 *  @param         size                size of mem to write to file
 *  @param         hint                optional text to place inside the file name
 *  @param         suffix              file name suffix
 */
#define OYJL_TEST_WRITE_RESULT( mem, size, hint, suffix ) if(mem) { \
  char * fn = malloc(64); \
  sprintf( fn, "%s-%d-%d-%s-%s.%s", OYJL_TEST_NAME, oyjl_test_number, oy_test_current_sub_count, hint?hint:"", oyjlTestResultToString(oy_test_last_result,0), suffix ); \
  oyjlWriteTestFile( fn, mem, size ); \
  if(verbose) fprintf(zout, "%s\n", fn); \
  if(oy_test_last_result == oyjlTESTRESULT_FAIL) { \
    char * fns = malloc(64); \
    sprintf( fns, "%s-%d-%d-%s-%s.%s", OYJL_TEST_NAME, oyjl_test_number, oy_test_current_sub_count, hint?hint:"", oyjlTestResultToString(oyjlTESTRESULT_SUCCESS,0), suffix ); \
    FILE * fp = fopen(fns, "r"); \
    if(fp && strcmp(suffix, "txt") == 0) { \
      char * diff = malloc(128); \
      sprintf( diff, "diff -aur %s %s", fns, fn ); \
      system(diff); \
      free(diff); \
      fclose(fp); \
    } \
  } \
  free(fn); \
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
