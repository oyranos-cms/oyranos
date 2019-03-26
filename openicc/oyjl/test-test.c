/** @file test-test.c
 *
 *  Oyranos is an open source Color Management System
 *
 *  Copyright (C) 2019  Kai-Uwe Behrmann
 *
 *  @brief    Oyjl test suite
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2019/02/24
 */

#define TESTS_RUN \
  TEST_RUN( testA, "A Test", 1 ); \
  TEST_RUN( testB, "B Test", 1 );

#define OYJL_TEST_MAIN_SETUP  printf("\n    Test Program\n");
#define OYJL_TEST_MAIN_FINISH printf("\n    Test Program finished\n\n");
#include "oyjl_test_main.h"

int local_debug = 0;
int * oyjl_debug = &local_debug; // satisfy oyjl_test_main.h without linking libOyjlCore

/* --- actual tests --- */

oyjlTESTRESULT_e testA()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  if(__LINE__ == 33)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "line %d                                        ", (__LINE__-2) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "line %d                                        ", (__LINE__-5) );
  }

  return result;
}

oyjlTESTRESULT_e testB()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OYJL_TEST_START

  fprintf(stdout, "\n" );

  if(__LINE__ == 34)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "line %d                                        ", (__LINE__-2) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "line %d                                        ", (__LINE__-5) );
  }

  int i, n = 100;
  double clck = oyjlClock();
  {
    for(i = 0; i < n; ++i)
    { // take time
      int * mem = (int*)malloc(1000000000);
      mem[i] = i;
      free(mem);
    }
  }
  clck = oyjlClock() - clck;
  if(__LINE__ == 71)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "line %d    %s                     ", (__LINE__-2), oyjlProfilingToString(n,clck/(double)CLOCKS_PER_SEC,"alloc") );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "line %d    %s                     ", (__LINE__-2), oyjlProfilingToString(n,clck/(double)CLOCKS_PER_SEC,"alloc") );
  }

  return result;
}



/* --- end actual tests --- */
