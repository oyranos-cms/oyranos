/** @file test.c
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

#define TESTS_RUN \
  TEST_RUN( testVersion, "Version matching", 1 ); \
  TEST_RUN( testJson, "JSON handling", 1 );

#include "oyjl_test.h"
#include "oyjl.h"
#include "oyjl_version.h"
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif


/* --- actual tests --- */

#include <yajl/yajl_version.h>
oyjlTESTRESULT_e testVersion()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );
  fprintf(zout, "compiled version:     %d\n", OYJL_VERSION );
  fprintf(zout, " runtime version:     %d\n", oyjlVersion(0) );
  fprintf(zout, "sta yajl version:     %d\n", oyjlVersion(1) );
#define STR(x) #x
#define zOUT(x) fprintf(zout, "cnf yajl version:     %s\n", STR(x) );
  zOUT( OYJL_YAJL_VERSION )
  fprintf(zout, "dyn yajl version:     %d\n", yajl_version() );

  if(OYJL_VERSION == oyjlVersion(0))
    result = oyjlTESTRESULT_SUCCESS;
  else
    result = oyjlTESTRESULT_FAIL;

  return result;
}


oyjlTESTRESULT_e testJson ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int i;
  const char * json = "{\"org\":{\"free\":[{\"s1key_a\":\"val_a\",\"s1key_b\":\"val_b\"},{\"s2key_c\":\"val_c\",\"s2key_d\":\"val_d\"}],\"key_e\":\"val_e_yyy\",\"key_f\":\"val_f\"}}";

  fprintf( zout, "%s\n", json );

  oyjl_val value = 0;
  oyjl_val root = 0;
  for(i = 0; i < 5; ++i)
  {
    int level = 0;
    const char * xpath = NULL;
    char error_buffer[128];
    int flags = 0;

    root = oyjlTreeParse( json, error_buffer, 128 );

    switch(i) {
    case 1: xpath = "org/free/[1]"; break;
    case 2: xpath = "org/free/[1]/"; break;
    case 3: xpath = "/org/free/[1]"; break;
    case 4: xpath = "org/objects/new_one";
	    flags = OYJL_CREATE_NEW;
	    break;
    default: xpath = NULL; break;
    }

    if(!xpath)
    {
      char * json = 0;
      oyjlTreeToJson( root, &level, &json );
      if(json && json[0] && strlen(json) == 210)
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "oyjlTreeToJson()                     %lu", (unsigned long)strlen(json) );
        fprintf( zout, "%s\n", json );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlTreeToJson()                                " );
      }
      myDeAllocFunc(json);

      char ** paths = NULL;
      oyjlTreeToPaths( root, 10, NULL, 0, &paths );
      int count = 0; while(paths && paths[count]) ++count;
      if(count == 10)
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "oyjlTreeToPaths()                     %d", count );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlTreeToPaths()                     %d", count );
      }
      if(paths && count)
        oyjlStringListRelease( &paths, count, free );

      oyjlTreeToPaths( root, 10, NULL, OYJL_KEY, &paths );
      count = 0; while(paths && paths[count]) ++count;
      if(count == 6)
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "oyjlTreeToPaths( OYJL_KEY )           %d", count );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlTreeToPaths( OYJL_KEY )           %d", count );
      }
      const char * match = NULL;
      const char * xpath = "org///s2key_d";
      for(int j = 0; j < count; ++j)
      {
        if(oyjlPathMatch( paths[j], xpath, 0 ))
          match = paths[j];
        fprintf( zout, "%d: %s\n", j, paths[j] );
      }
      if(match && strcmp(match,"org/free/[1]/s2key_d") == 0)
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "oyjlPathMatch(%s, %s)", match, xpath );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlPathMatch(%s, %s)", match, xpath );
      }
      if(paths && count)
        oyjlStringListRelease( &paths, count, free );

      oyjlTreeToPaths( root, 10, NULL, OYJL_PATH, &paths );
      count = 0; while(paths && paths[count]) ++count;
      if(count == 4)
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "oyjlTreeToPaths( OYJL_PATH )          %d", count );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlTreeToPaths( OYJL_PATH )          %d", count );
      }
      for(int j = 0; j < count; ++j)
        fprintf( zout, "%d: %s\n", j, paths[j] );

      double clck = oyjlClock();
      const char * p = "org/free/[0]/s1key_b";
      int k = 0, n = 500;
      for(k = 0; k < n; ++k)
      {
        value = oyjlTreeGetValue( root, flags, p );
        char * t = oyjlValueText(value, myAllocFunc);
        if(!t) break;
        myDeAllocFunc(t);
      }
      clck = oyjlClock() - clck;
      if( k == n )
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "oyjlValueText(%s) %s", p,
                   oyProfilingToString(n,clck/(double)CLOCKS_PER_SEC,"key"));
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlValueText(%s) (%d) (%s)", p, k, value?"oyjlTreeGetValue() good":"oyjlTreeGetValue() failed" );
      }

      if(paths && count)
        oyjlStringListRelease( &paths, count, free );
    }
    if(xpath)
    {
      int success = 0;
      char * rjson = NULL;
      value = oyjlTreeGetValue( root, flags, xpath );
      if( value  )
      {
        if(i == 4)
        {
          oyjlTreeToJson( root, &level, &rjson );
        } else
          oyjlTreeToJson( value, &level, &rjson );
      }
      if(rjson && rjson[0])
        success = 1;
      else if(i == 2 || i == 3)
        success = 1;
      if(success)
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "oyjlTreeGetValue(flags=%d)            ", flags );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlTreeGetValue(flags=%d)            ", flags );
      }
      fprintf( zout, "%s xpath \"%s\" %s\n", value?"found":"found not", xpath, success?"ok":"" );
      if(rjson && rjson[0])
      {
        success = 1;
        fprintf( zout, "%s\n", rjson );
      }
      if(rjson) myDeAllocFunc(rjson);
      rjson = NULL;
      if(!root) oyjlTreeFree( value );
    }

    oyjlTreeFree( root );
  }

  return result;
}



/* --- end actual tests --- */

#include "oyjl_test_main.h"

