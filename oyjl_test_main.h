/** @file oyjl_test_main.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2004-2018  Kai-Uwe Behrmann
 *
 *  @brief    Oyjl test suite
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *  new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2008/12/04
 */

#ifndef OYJL_TEST_MAIN_H
#define OYJL_TEST_MAIN_H

#include "oyjl_test.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup oyjl
 *  @{ *//* oyjl */

/** \addtogroup oyjl_test
 *  @{ *//* oyjl_test */

/** @brief print more results, when the -v argument is passed to the test program. */
int verbose = 0;


#ifndef OYJL_TEST_MAIN_SETUP
/** @brief setup Oyjl to your needs by defining this macro
 *
 *  The macro is called as first entry inside oyjl_test_main.h defined main().
 *  @code
    #include <oyjl.h> // declare oyjlDebugVariableSet()
    int my_debug_variable = 0;

    #define OYJL_TEST_MAIN_SETUP oyjlDebugVariableSet( &my_debug_variable ); printf("\n    My Test Program\n");
    #include <oyjl_test_main.h> // inject code from OYJL_TEST_MAIN_SETUP into main() start
    @endcode
 *
 *  The above example let your code set your own debug variable. So you can
 *  see if the -v -v option was set, as that increases *oyjl_debug += 1.
 *  Or you can e.g. redefine zout to default to a FILE pointer instead of
 *  stdout.
 */
#define OYJL_TEST_MAIN_SETUP
#endif

#ifndef OYJL_TEST_MAIN_FINISH
/** @brief end your test program as you need by defining this macro
 *
 *  Place this macro in front of your \#include oyjl_test_main.h.
 *  The macro is called as last entry inside oyjl_test_main.h defined main().
 *  @code
    #define OYJL_TEST_MAIN_FINISH printf("\n    My Test Program finished\n\n");
    @endcode
 */
#define OYJL_TEST_MAIN_FINISH
#endif

#undef TEST_RUN
#define TEST_RUN( prog, text, do_it ) oyjlTESTRESULT_e prog(void);
TESTS_RUN /* declare test functions outside main() for C++ and almost reset the TEST_RUN macro */
#undef TEST_RUN

/** @brief simple start function for testing program */
int main(int argc, char** argv)
{
  int i, error = 0,
      argpos = 1,
      list = 0;

  /** The ::OYJL_TEST_MAIN_SETUP macro can be used to do something initially. */
  OYJL_TEST_MAIN_SETUP

  zout = stdout;  /* printed inbetween results */

  /* init */
  for(i = 0; i <= oyjlTESTRESULT_UNKNOWN; ++i)
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

  /** Use the verbose variable in your test code to enable additional result printing.
   *  The test command argument '-v' will set the verbose variable to 1, default is 0.
   */
  i = 1; while(i < argc) if( strcmp(argv[i++],"-v") == 0 )
  { ++argpos;
    if(verbose)
      *oyjl_debug += 1;
    verbose = 1;
  }

  i = 1; while(i < argc) if( strcmp(argv[i++],"-h") == 0 )
  { ++argpos;
    fprintf( stdout, "    Hint: the '-l' option will list all test names\n" );
    fprintf( stdout, "    Hint: the '-v' option enables the 'verbose' variable\n" );
    fprintf( stdout, "    Hint: the '--silent' option sends all zout printing to stderr\n" );
    fprintf( stdout, "    Hint: the '-h' option prints this help text\n" );
    return 0;
  }

  memset(tests_xfailed, 0, sizeof(char*) * OYJL_TEST_MAX_COUNT);
  memset(tests_failed, 0, sizeof(char*) * OYJL_TEST_MAX_COUNT);
  /* do tests */

#undef TEST_RUN
#define TEST_RUN( prog, text, do_it ) ++oyjl_test_count;
TESTS_RUN /* count */
#undef TEST_RUN
#define TEST_RUN( prog, text, do_it ) \
{ \
  if(argc > argpos && do_it) { \
      for(i = argpos; i < argc; ++i) \
        if(strstr(text, argv[i]) != 0 || \
           atoi(argv[i]) == oyjl_test_number ) \
          oyjlTestRun( prog, text, oyjl_test_number, oyjl_test_count ); \
  } else if(list) \
    printf( "[%d] %s\n", oyjl_test_number, text); \
  else if(do_it) \
    oyjlTestRun( prog, text, oyjl_test_number, oyjl_test_count ); \
  ++oyjl_test_number; \
}
  TESTS_RUN

  /* give a summary */
  if(!list)
  {
    int i = oyjl_print_sub_length + 12;
    fputs( "\n", stdout );
    while(i--) fputs( "#", stdout );
    fputs( "\n", stdout );
    fputs( "#", stdout );
    i = oyjl_print_sub_length + 10;
    while(i--) fputs( " ", stdout );
    fputs( "#\n", stdout );
    fprintf( stdout, "#                     " );
    fprintf( stdout, "%s", oyjlTermColor_(oyjlBOLD, "Results" ));
    i = oyjl_print_sub_length - 18;
    while(i--) fputs( " ", stdout );
    fputs( "#\n", stdout );
    fprintf( stdout, "    Total of Sub Tests:         %d\n", oy_test_sub_count );
    for(i = 0; i <= oyjlTESTRESULT_UNKNOWN; ++i)
    {
      fprintf( stdout, "    Tests with status %s:\t%d\n",
                       oyjlTestResultToString( (oyjlTESTRESULT_e)i, results[i] ), results[i] );
    }

    error = (results[oyjlTESTRESULT_FAIL] ||
             results[oyjlTESTRESULT_SYSERROR] ||
             results[oyjlTESTRESULT_UNKNOWN]
            );

    for(i = 0; i < OYJL_TEST_MAX_COUNT; ++i)
      if(tests_xfailed[i])
      {
        fprintf( stdout, "    %s: [%d] \"%s\"\n",
                 oyjlTestResultToString( oyjlTESTRESULT_XFAIL, 1 ), i, tests_xfailed[i] );
        free(tests_xfailed[i]);
        tests_xfailed[i] = NULL;
      }
    for(i = 0; i < OYJL_TEST_MAX_COUNT; ++i)
      if(tests_failed[i])
      {
        fprintf( stdout, "    %s: [%d] \"%s\"\n",
                 oyjlTestResultToString( oyjlTESTRESULT_FAIL, 1 ), i, tests_failed[i] );
        free(tests_failed[i]);
        tests_failed[i] = NULL;
      }

    if(error)
      fprintf( stdout, "    Tests %s\n", oyjlTermColor_( oyjlRED, "FAILED" ) );
    else
      fprintf( stdout, "    Tests %s\n", oyjlTermColor_( oyjlGREEN, "SUCCEEDED" ) );

    fprintf( stdout, "\n    Hint: the '-l' option will list all test names\n" );
  }

  /** The ::OYJL_TEST_MAIN_FINISH macro can be used to do something after all tests. */
  OYJL_TEST_MAIN_FINISH

  return error;
}
/** @} *//* oyjl_test */
/** @} *//* oyjl */

#ifdef __cplusplus
}
#endif

#endif /* OYJL_TEST_MAIN_H */
