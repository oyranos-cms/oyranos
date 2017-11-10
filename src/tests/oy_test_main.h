/** @file oy_test_main.h
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


/*  main */
int main(int argc, char** argv)
{
  int i, error = 0,
      argpos = 1,
      list = 0;

  zout = stdout;  /* printed inbetween results */

  oyExportStart_(EXPORT_CHECK_NO);

  /* init */
  for(i = 0; i <= oyTESTRESULT_UNKNOWN; ++i)
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

  fprintf( zout, "\nOyranos Tests v" OYRANOS_VERSION_NAME
           "  developed: " OYRANOS_DATE
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
    for(i = 0; i <= oyTESTRESULT_UNKNOWN; ++i)
    {
      if(!results[i]) colorterm = NULL;
      fprintf( stdout, "    Tests with status %s:\t%d\n",
                       oyTestResultToString( (oyTESTRESULT_e)i ), results[i] );
      colorterm = colorterm_;
    }

    error = (results[oyTESTRESULT_FAIL] ||
             results[oyTESTRESULT_SYSERROR] ||
             results[oyTESTRESULT_UNKNOWN]
            );

    for(i = 0; i < tn; ++i)
      if(tests_xfailed[i])
        fprintf( stdout, "    %s: [%d] \"%s\"\n",
                 oyTestResultToString( oyTESTRESULT_XFAIL), i, tests_xfailed[i] );
    for(i = 0; i < tn; ++i)
      if(tests_failed[i])
        fprintf( stdout, "    %s: [%d] \"%s\"\n",
                 oyTestResultToString( oyTESTRESULT_FAIL), i, tests_failed[i] );

    if(error)
    {
      if(CHECK_COLOR)
        fprintf( stdout, "    Tests " RED "FAILED" CTEND "\n" );
      else
        fprintf( stdout, "    Tests FAILED\n" );
    }
    else
    {
      if(CHECK_COLOR)
        fprintf( stdout, "    Tests " GREEN "SUCCEEDED" CTEND "\n" );
      else
        fprintf( stdout, "    Tests SUCCEEDED\n" );
    }

    fprintf( stdout, "\n    Hint: the '-l' option will list all test names\n" );

    oyFinish_( 0 );
  }

  return error;
}

