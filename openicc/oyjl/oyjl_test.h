/** @file oyjl_test.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2004-2021  Kai-Uwe Behrmann
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

#ifndef OYJL_H
#define OYJL_H_NOT
# ifndef OYJL_CTEND
typedef enum {
  oyjlNO_MARK,
  oyjlRED,
  oyjlGREEN,
  oyjlBLUE,
  oyjlBOLD,
  oyjlITALIC,
  oyjlUNDERLINE
} oyjlTEXTMARK_e;
# endif
#endif
#ifndef OYJL_CREATE_VA_STRING
#define OYJL_CREATE_VA_STRING(format_, text_, alloc_, error_action) \
if(format_ && strchr(format_,'%') != NULL) { \
  va_list list; \
  size_t sz = 0; \
  int len = 0; \
  void*(* allocate_)(size_t size) = alloc_; \
  if(!allocate_) allocate_ = malloc; \
\
  text_ = NULL; \
  \
  va_start( list, format_); \
  len = vsnprintf( text_, sz, format_, list); \
  va_end  ( list ); \
\
  { \
    text_ = (char*) allocate_( sizeof(char) * len + 2 ); \
    if(!text_) \
    { \
      fprintf( stderr, "could not allocate memory" ); \
      error_action; \
    } \
    va_start( list, format_); \
    len = vsnprintf( text, len+1, format_, list); \
    va_end  ( list ); \
  } \
} else if(format_) \
{ \
  text_ = strdup( format_ );\
} \
else \
{ \
  fprintf( stderr, "%s format argument is missing", oyjlTermColor_(oyjlRED,"Program Error:") ); \
  error_action; \
}
#endif

extern int * oyjl_debug;
extern int verbose;

/** \addtogroup oyjl
 *  @{ *//* oyjl */
/** \addtogroup oyjl_test OyjlTest Code and Tools Testing
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
 *  Include oyjl.h before oyjl_test_main.h, in case you need it.
 *
 *  A complete test file is the self test. It includes the
 *  oyjl_test_main.h and implicitely other oyjl_test.h headers and
 *  compiles without additional dependencies:
 *  @include test-test.c
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
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
static int oyjlTermColorCheck__()
{
  struct stat sout, serr;
  int color_term = 0;

  if( fstat( fileno(stdout), &sout ) == -1 )
    return 0;

  if( fstat( fileno(stderr), &serr ) == -1 )
    return color_term;

  if( S_ISCHR( sout.st_mode ) &&
      S_ISCHR( serr.st_mode ) )
    color_term = 1;

  return color_term;
}
int oyjlTermColorCheck()
{
  int color_env = 0;
  static int colorterm_init = 0;
  static const char * oyjl_colorterm = NULL;
  static int truecolor = 0,
             color = 0;
  if(!colorterm_init)
  {
    colorterm_init = 1;
    oyjl_colorterm = getenv("COLORTERM");
    color = oyjl_colorterm != NULL ? 1 : 0;
    if(!oyjl_colorterm) oyjl_colorterm = getenv("TERM");
    truecolor = oyjl_colorterm && strcmp(oyjl_colorterm,"truecolor") == 0;
    if(!oyjlTermColorCheck__())
      truecolor = color = 0;
    if( getenv("FORCE_COLORTERM") )
      truecolor = color = 1;
    if( getenv("FORCE_NO_COLORTERM") )
      truecolor = color = 0;
    if(verbose)
      fprintf(stdout, "color: %d truecolor: %d oyjl_colorterm: %s\n", color, truecolor, oyjl_colorterm );
  }
  color_env = (color ? 0x01 : 0x00) | (truecolor ? 0x02 : 0x00);
  return color_env;
}
const char * oyjlTermColor_( oyjlTEXTMARK_e rgb, const char * text) {
  int len = strlen(text);
  static char t[256];
  int color_env = oyjlTermColorCheck(),
      color = color_env & 0x01,
      truecolor = color_env & 0x02;
  if(len < 200)
  {
    switch(rgb)
    {
      case oyjlNO_MARK: sprintf( t, "%s", text ); break;
      case oyjlRED: sprintf( t, "%s%s%s", truecolor ? OYJL_RED_TC : color ? OYJL_RED_B : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
      case oyjlGREEN: sprintf( t, "%s%s%s", truecolor ? OYJL_GREEN_TC : color ? OYJL_GREEN_B : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
      case oyjlBLUE: sprintf( t, "%s%s%s", truecolor ? OYJL_BLUE_TC : color ? OYJL_BLUE_B : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
      case oyjlBOLD: sprintf( t, "%s%s%s", truecolor || color ? OYJL_BOLD : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
      case oyjlITALIC: sprintf( t, "%s%s%s", truecolor || color ? OYJL_ITALIC : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
      case oyjlUNDERLINE: sprintf( t, "%s%s%s", truecolor || color ? OYJL_UNDERLINE : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
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
oyjlTESTRESULT_e prog(void); \
{ \
  if(argc > argpos && do_it) { \
      for(i = argpos; i < argc; ++i) \
        if(strstr(text, argv[i]) != 0 || \
           atoi(argv[i]) == oyjl_test_number ) \
          oyjlTestRun( prog, text, oyjl_test_number ); \
  } else if(list) \
    printf( "[%d] %s\n", oyjl_test_number, text); \
  else if(do_it) \
    oyjlTestRun( prog, text, oyjl_test_number ); \
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
/** Macro to override inbuild default of 64 maximum array length for storing test results.
 */
#define OYJL_TEST_MAX_COUNT 64
#endif
char * tests_failed[OYJL_TEST_MAX_COUNT];
char * tests_xfailed[OYJL_TEST_MAX_COUNT];

#ifndef MAX_PATH
/* maximal path lenght, if not allready defined elsewhere */
#define MAX_PATH 1024
#endif

#if defined(__linux__)
#include <sys/ioctl.h>
#include <unistd.h>
int oyjlTestTermColumns()
{
    struct winsize w;
    int columns = 0;
    int gitlab = 0;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    columns = w.ws_col;
    if( getenv("GITLAB_CI") )
    {
      columns = 80;
      gitlab = 1;
    }
    if(verbose)
      fprintf(stdout, "oyjlTestTermColumns(): %d/%d %s\n", columns,w.ws_col, gitlab?"GITLAB_CI":"" );
    return columns;
}
#else
int oyjlTestTermColumns() {return 0;}
#endif

#ifndef OYJL_PRINT_SUB_LENGTH
/** Macro to override te name of the OYJL_PRINT_SUB_LENGTH environment variable.
 *  The environment variable will be used to detect the desired print width
 *  inside sub tests. The ::oyjl_print_sub_length is set accordingly.
 */
#define OYJL_PRINT_SUB_LENGTH "OYJL_PRINT_SUB_LENGTH"
#endif

int oy_test_current_sub_count;
/** @brief default for printed columns
 *  @see OYJL_PRINT_SUB_LENGTH */
int oyjl_print_sub_length = 51;
/** run a test and print results on end
 *  @param         test                test function
 *  @param         test_name           short string for status line
 *  @param         number              internal test number
 */
oyjlTESTRESULT_e oyjlTestRun         ( oyjlTESTRESULT_e  (*test)(void),
                                       const char        * test_name,
                                       int                 number )
{
  oyjlTESTRESULT_e error = oyjlTESTRESULT_UNKNOWN;
  char * text = NULL;
  /** Handle columns width by checking ::OYJL_PRINT_SUB_LENGTH envar or
   *  using terminal window size by asking linux ioctl or
   *  use inbuild default. */
  int i = 0, columns;
  const char * oyjl_print_sub_length_env = getenv(OYJL_PRINT_SUB_LENGTH);

  puts( "\n" );
  columns = oyjlTestTermColumns();
  if(oyjl_print_sub_length_env)
    oyjl_print_sub_length = atoi(oyjl_print_sub_length_env);
  else if(columns)
  {
    oyjl_print_sub_length = columns;
    if(oyjl_print_sub_length >= 12)
      oyjl_print_sub_length -= 4 + 9 + oyjl_print_sub_length%8;
  }

  if(oyjl_print_sub_length < 56)
    oyjl_print_sub_length = 56;

  oyjl_test_file = NULL;
  oyjl_test_file_line = -1;
  oy_test_current_sub_count = 0;

  /** Print header line and title. */
  text = (char*) malloc(80 + oyjl_print_sub_length);
  text[0] = '\000';
  i = oyjl_print_sub_length + 12;
  while(i--) sprintf( &text[strlen(text)], "_" );
  sprintf( &text[strlen(text)], "\n" );
  fprintf( stdout, text );
  free(text);
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

  /** Print sumarisation of sub test. */
  if(error <= oyjlTESTRESULT_FAIL)
    fprintf(stdout, " !!! ERROR !!!" );
  fprintf(stdout, "\n" );

  return error;
}


int oy_test_sub_count = 0;
/** @brief Result of last sub test */
oyjlTESTRESULT_e oy_test_last_result = oyjlTESTRESULT_UNKNOWN;
/** @brief Register status and print info of sub test
 *
 *  Print a custom line to stdout followed by the status. Register state.
 *
 *  The PRINT_SUB_BASIC macro remembers the first file position of similar strongly
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
      PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, i,
      "i =" );
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
 *
 *  @see PRINT_SUB PRINT_SUB_INT PRINT_SUB_PROFILING
 */
#define PRINT_SUB_BASIC( result_, ... ) \
{ oy_test_last_result = result_; \
  if((result_) < oyjlTESTRESULT_SUCCESS && (result_) < result) OYJL_TEST_START \
  if((result_) < result) \
    result = result_; \
  fprintf(stdout, ## __VA_ARGS__ ); \
  fprintf(stdout, " .. %s", oyjlTestResultToString(result_,1)); \
  if((result_) <= oyjlTESTRESULT_FAIL) \
    fprintf(stdout, " !!! ERROR !!!" ); \
  fprintf(stdout, "\n" ); \
  ++oy_test_sub_count; \
  ++oy_test_current_sub_count; \
}

/** Like ::PRINT_SUB_BASIC but with variable columns size. */
#define PRINT_SUB( result_, ... ) \
{ PRINT_SUB_BASIC( result_, \
    oyjlPrintSub(-1,-1, ## __VA_ARGS__ ) ); \
}

/** Like ::PRINT_SUB_BASIC but with variable columns size and right side integer print. */
#define PRINT_SUB_INT( result_, count_, ... ) \
{ PRINT_SUB_BASIC( result_, \
    oyjlPrintSub(-1, count_, ## __VA_ARGS__ ) ); \
}

/** Like ::PRINT_SUB_BASIC but with variable columns size and right side profiling print. */
#define PRINT_SUB_PROFILING( result_, integer_, duration_, term_, ... ) \
{ PRINT_SUB_BASIC( result_, \
    oyjlPrintSubProfiling(-1, integer_, duration_, term_, ## __VA_ARGS__ ) ); \
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
      PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(text), "myApiTesting()" );
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
  char * fn = (char*)malloc((hint?strlen(hint):0)+64); \
  sprintf( fn, "%s-%d-%d-%s-%s.%s", OYJL_TEST_NAME, oyjl_test_number, oy_test_current_sub_count, hint?hint:"", oyjlTestResultToString(oy_test_last_result,0), suffix ); \
  oyjlWriteTestFile( fn, mem, size ); \
  if(verbose) fprintf(zout, "%s\n", fn); \
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || oy_test_last_result == oyjlTESTRESULT_XFAIL) { \
    char * fns = (char*)malloc((hint?strlen(hint):0)+64); \
    sprintf( fns, "%s-%d-%d-%s-%s.%s", OYJL_TEST_NAME, oyjl_test_number, oy_test_current_sub_count, hint?hint:"", oyjlTestResultToString(oyjlTESTRESULT_SUCCESS,0), suffix ); \
    FILE * fp = fopen(fns, "r"); \
    if(fp && strcmp(suffix, suffix) == 0) { \
      char * diff = (char*)malloc((hint?strlen(hint)*2:0)+128); \
      sprintf( diff, "diff -aur %s %s", fns, fn ); \
      system(diff); \
      free(diff); \
      fclose(fp); \
    } \
    free(fns); \
  } \
  free(fn); \
}

/** helper to print a number */
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

/** helper to print tempo */
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
    sprintf( &texts[a][0], "%.00f  %s/s", integer/duration, term );

  len = strlen(&texts[a][0]);

  for(i = 0; i < 16-len; ++i)
    sprintf( &texts[a][i], " " );

  if(integer/duration >= 1000000.0)
    sprintf( &texts[a][i], "%.02f M%s/s", integer/duration/1000000.0, term );
  else
    sprintf( &texts[a][i], "%.00f  %s/s", integer/duration, term );

  return texts[a++];
}

/** Storage location of last return from oyjlPrintSub() and inside the
 *  PRINT_SUB() family respectively. */
char * oyjl_print_sub = 0;
/** @brief print test results in a canonical way
 *
 *  The funktion is not reentrant. You need to copy the result if
 *  calling repeatedly.
 *
 *  @param[in]     space               number line width: use -1 for default of 51
 *  @param[in]     right               number for right alignment: use -1 to omit
 *  @param[in]     format              printf format
 *  @param[in]     ...                 variable argument list for format
 *  @return                            return static string in oyjl_print_sub
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/09/29
 *  @since   2021/09/29 (Oyjl: 1.0.0)
 */
const char * oyjlPrintSub            ( int                 space,
                                       int                 right,
                                       const char        * format,
                                                           ... )
{
  char * text = NULL, * num = NULL, * visual, * tmp;
  int len, vlen, i;

  if(oyjl_print_sub)
  { free(oyjl_print_sub); oyjl_print_sub = NULL; }

  OYJL_CREATE_VA_STRING(format, text, malloc, return NULL)

  visual = strdup( text );
#ifndef OYJL_ARGS_C
# ifndef OYJL_H_NOT
  oyjlRegExpReplace( &visual, "\033[[0-9;]*[a-zA-Z]", "" );
# endif
#endif
  vlen = strlen(visual);
  len = strlen(text);
  if(space == -1)
    space = oyjl_print_sub_length;

  if(vlen-1 < space)
  {
    tmp = (char*) realloc( text, (len<space?space:len) + space - vlen + 2 );
    text = tmp;
    for( i = vlen - 1; i < space; ++i ) sprintf( &text[strlen(text)], " " );
  }
  free(visual); visual = NULL;

  if(right != -1)
  {
    num = (char*) malloc(64);
    sprintf( num, "%d", right );
    i = strlen(num);
    len = strlen(text);
    tmp = (char*) realloc( text, len + strlen(num) + 2 );
    text = tmp;
    if(vlen + i -1 < space)
      text[len-i-1] = '\000';
    sprintf( &text[strlen(text)], " %d", right );
    free(num);
  }

  oyjl_print_sub = text;

  return text;
}

/** @brief print test results with profiling in a canonical way
 *
 *  The funktion is not reentrant. You need to copy the result if
 *  calling repeatedly.
 *
 *  @param[in]     space               number line width: use -1 for default
 *                                     columns or 51
 *  @param[in]     integer             arg like for oyjlProfilingToString()
 *  @param[in]     duration            arg like for oyjlProfilingToString()
 *  @param[in]     term                arg like for oyjlProfilingToString()
 *  @param[in]     format              printf format
 *  @param[in]     ...                 variable argument list for format
 *  @return                            return static string in oyjl_print_sub
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/08
 *  @since   2021/10/08 (Oyjl: 1.0.0)
 */
const char * oyjlPrintSubProfiling   ( int                 space,
                                       int                 integer,
                                       double              duration,
                                       const char        * term,
                                       const char        * format,
                                                           ... )
{
  char * text = NULL, * visual, * tmp, * prof = NULL;
  int len, vlen, i;
  static int even = 1;

  if(oyjl_print_sub)
  { free(oyjl_print_sub); oyjl_print_sub = NULL; }

  OYJL_CREATE_VA_STRING(format, text, malloc, return NULL)

  prof = (char*) malloc( 256 );
  if(!prof) { free(text); return NULL; }
  if(integer/duration >= 1000000.0)
    sprintf( prof, " %.02f M%s/s", integer/duration/1000000.0, term );
  else if(integer/duration < 10.0)
    sprintf( prof, " %.04f  %s/s", integer/duration, term );
  else
    sprintf( prof, " %.00f  %s/s", integer/duration, term );

  visual = strdup( text );
#ifndef OYJL_ARGS_C
# ifndef OYJL_H_NOT
  oyjlRegExpReplace( &visual, "\033[[0-9;]*[a-zA-Z]", "" );
# endif
#endif
  vlen = strlen(visual);
  len = strlen(text);
  if(space == -1)
    space = oyjl_print_sub_length;

  if(vlen-1 < space)
  {
    const char * mark = ".";
    int odd = 1;
    even = !even;
    tmp = (char*) realloc( text, (len<space?space:len) + space - len + 20 );
    text = tmp;
    for( i = vlen - 1; i < space; ++i ) sprintf( &text[strlen(text)], i < space - 16&&i>vlen+5 ? even?((odd = !odd) == 0)?mark:" ":mark : " " );
  }
  free(visual); visual = NULL;

  i = strlen(prof);
  if(integer/duration < 10.0)
  {
    const char * t;
    sprintf( prof, " %.04f", integer/duration );
    t = oyjlTermColor_(oyjlITALIC,prof);
    if(t == prof)
      snprintf( &prof[strlen(prof)], 256-strlen(prof), "  %s/s", term );
    else
      snprintf( prof, 256, "%s  %s/s", t, term );
  }
  len = strlen(text);
  tmp = (char*) realloc( text, len + strlen(prof) + 2 );
  text = tmp;
  if(vlen + i < space)
    text[len-i] = '\000';
  else
    text[vlen + 1] = '\000';
  sprintf( &text[strlen(text)], "%s", prof );
  free(prof);

  oyjl_print_sub = text;

  return text;
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
