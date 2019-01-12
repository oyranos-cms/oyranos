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

/** \addtogroup oyjl_test
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
 *  @example test.c
 *  @{ *//* oyjl */
/** @brief simple start function for testing program */
int main(int argc, char** argv)
{
  int i, error = 0,
      argpos = 1,
      list = 0;

  zout = stdout;  /* printed inbetween results */

  /* init */
  for(i = 0; i <= oyjlTESTRESULT_UNKNOWN; ++i)
    results[i] = 0;

  i = 1; while(i < argc) if( strcmp(argv[i++],"-l") == 0 )
  { ++argpos;
    zout = stderr;
    list = 1;
  }

  colorterm = getenv("COLORTERM");

  i = 1; while(i < argc) if( strcmp(argv[i++],"--silent") == 0 )
  { ++argpos;
    zout = stderr;
  }

  fprintf( zout, "\nTests"
           "\n\n" );

  memset(tests_xfailed, 0, sizeof(char*) * tn);
  memset(tests_failed, 0, sizeof(char*) * tn);
  /* do tests */

  TESTS_RUN

  /* give a summary */
  if(!list)
  {
    const char * colorterm_ = colorterm;

    fprintf( stdout, "\n################################################################\n" );
    fprintf( stdout, "#                                                              #\n" );
    fprintf( stdout, "#                     Results                                  #\n" );
    fprintf( stdout, "    Total of Sub Tests:         %d\n", oy_test_sub_count );
    for(i = 0; i <= oyjlTESTRESULT_UNKNOWN; ++i)
    {
      if(!results[i]) colorterm = NULL;
      fprintf( stdout, "    Tests with status %s:\t%d\n",
                       oyTestResultToString( (oyjlTESTRESULT_e)i ), results[i] );
      colorterm = colorterm_;
    }

    error = (results[oyjlTESTRESULT_FAIL] ||
             results[oyjlTESTRESULT_SYSERROR] ||
             results[oyjlTESTRESULT_UNKNOWN]
            );

    for(i = 0; i < tn; ++i)
      if(tests_xfailed[i])
        fprintf( stdout, "    %s: [%d] \"%s\"\n",
                 oyTestResultToString( oyjlTESTRESULT_XFAIL), i, tests_xfailed[i] );
    for(i = 0; i < tn; ++i)
      if(tests_failed[i])
        fprintf( stdout, "    %s: [%d] \"%s\"\n",
                 oyTestResultToString( oyjlTESTRESULT_FAIL), i, tests_failed[i] );

    if(error)
      fprintf( stdout, "    Tests %s\n", oyjlTermColor_( oyRED, "FAILED" ) );
    else
      fprintf( stdout, "    Tests %s\n", oyjlTermColor_( oyGREEN, "SUCCEEDED" ) );

    fprintf( stdout, "\n    Hint: the '-l' option will list all test names\n" );
  }

  return error;
}
/*  @} *//* oyjl */

#endif /* OYJL_TEST_MAIN_H */
