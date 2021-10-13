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
#define OYJL_TEST_MAIN_FINISH printf("\n    Test Program finished\n\n"); if(oyjl_print_sub) free(oyjl_print_sub);
#include "oyjl_test_main.h"

int local_debug = 0;
int * oyjl_debug = &local_debug; // satisfy oyjl_test_main.h without linking libOyjlCore

/* --- actual tests --- */

oyjlTESTRESULT_e testA()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  if(__LINE__ == 33)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, __LINE__-2,
    "line" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "line %d", (__LINE__-5) );
  }

  return result;
}

oyjlTESTRESULT_e testB()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  OYJL_TEST_START

  fprintf(stdout, "\n" );

  if(__LINE__ == -1)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, __LINE__-2, 
    "line" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_XFAIL, __LINE__-5, 
    "line" );
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
  if(__LINE__ == 70)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    oyjlPrintSubProfiling( -1, n, clck/(double)CLOCKS_PER_SEC,"alloc",
    "line %d", (__LINE__-1) ) );
  } else
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_XFAIL,
    n,clck/(double)CLOCKS_PER_SEC,"alloc", 
    "line %d", (__LINE__-5) );
  }

  return result;
}



/* --- end actual tests --- */
