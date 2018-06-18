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
  TEST_RUN( testI18N, "Internationalisation", 1 ); \
  TEST_RUN( testStringRun, "String handling", 1 ); \
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

oyjlTESTRESULT_e testI18N()
{
  const char * clang;
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  clang = setlocale(LC_ALL, NULL);
  if((clang && (strcmp(clang, "C") == 0)) || !clang)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "Language uninitialised good \"C\"              " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "Language uninitialised failed                 " );
  }

  clang = getenv("LANG");
  if(!clang || strcmp(clang,"C") == 0)
  {
    char * tmp = oyjlStringCopy("LANG=de_DE.UTF-8", 0);
    putenv(tmp);
    clang = getenv("LANG");
    if(!clang || strcmp(clang,"de_DE.UTF-8") != 0)
      fprintf( stderr, "Could not modify LANG environment variable. Test will not be useful.\n" );
  }

  setlocale(LC_ALL,"");

  if(clang && (strstr(clang, "de_DE") != 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "Language initialised good %s            ", clang?clang:"---" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "Language initialised failed %s          ", clang?clang:"---" );
  }

  return result;
}

#define TEST_DOMAIN "org/freedesktop/openicc/tests"
#define TEST_DOMAIN2 "org2/freedesktop2/openicc2/tests2"
#define TEST_KEY "/test_key"

oyjlTESTRESULT_e testStringRun ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int i;

  const char * test = TEST_DOMAIN "/display.oydi/display_name_long";
  

  test = "//openicc/display.oydi/";
  fprintf(zout, "\"%s\"\n", test );

  test = TEST_DOMAIN "/display.oydi/display_name";
  char * test_out = oyjlStringReplace( test, TEST_DOMAIN, TEST_DOMAIN2, 0,0 );
  //fprintf(zout, "test %s \"%s\"\n", test, test_out);
  if( strstr(test_out, TEST_DOMAIN2 ) != NULL )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringReplace(start)                             " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(start)                             " );
  }
  myDeAllocFunc(test_out);

  test_out = oyjlStringReplace( test, "display.", "foo.", 0,0 );
  //fprintf(zout, "test %s \"%s\"\n", test, test_out);
  if( strstr(test_out, "foo" ) != NULL )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringReplace(middle)                            " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(middle)                            " );
  }
  myDeAllocFunc(test_out);

  test_out = oyjlStringReplace( test, "display_name", "bar", 0,0 );
  //fprintf(zout, "test %s \"%s\"\n", test, test_out);
  if( strstr(test_out, "bar" ) != NULL &&
      strstr(test_out, "barbar" ) == NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringReplace(end)                               " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(end)                               " );
  }
  myDeAllocFunc(test_out);

  int list_n = 0;
  char ** list = oyjlStringSplit( "org/domain/eins.lib;org/domain/zwei.txt;org/domain/drei.lib;net/welt/vier.lib;net/welt/vier.txt;/net/welt/fuenf;/net/welt/fuenf", ';', &list_n, myAllocFunc );

  oyjlStringListFreeDoubles( list, &list_n, myDeAllocFunc );
  if( list_n == 6 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringListFreeDoubles()                          " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringListFreeDoubles()                          " );
    for(i = 0; i < list_n; ++i)
      fprintf(zout, " list[%d] \"%s\"\n", i, list[i] );
  }
  oyjlStringListRelease( &list, list_n, myDeAllocFunc );

  double d = 0.0;
  int error = oyjlStringToDouble( "0.2", &d );
  if( !error &&
      d == 0.2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringToDouble(\"0.2\") = %g                      ", d );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringToDouble(\"0.2\") = %g  error = %d            ", d, error );
  }
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

  value = oyjlTreeGetValue( NULL, OYJL_CREATE_NEW, "not/existing" );
  if(!value)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeGetValue( NULL, flags=OYJL_CREATE_NEW, xpath ) == NULL" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeGetValue( NULL, flags=OYJL_CREATE_NEW, xpath ) == NULL" );
  }

  root = oyjlTreeNew( NULL );
  if(root)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeNew( NULL )                     " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeNew( NULL )                     " );
  }
  oyjlTreeFree( root );

  root = oyjlTreeNew( "new/tree/key" );
  char * rjson = NULL; i = 0;
  oyjlTreeToJson( root, &i, &rjson ); i = 0;
  size_t len = strlen(rjson);
  if(root)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeNew( \"new/tree/key\" )       %d", (int)len );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeNew( \"new/tree/key\" )       %d", (int)len );
  }
  fprintf( zout, "%s\n", rjson );
  myDeAllocFunc( rjson ); rjson = NULL;

  value = oyjlTreeGetValue( root, 0, "new/tree/key" );
#define VALUE "value-\"one\""
  oyjlValueSetString( value, VALUE );
  oyjlTreeToJson( root, &i, &rjson ); i = 0;
  if(len < strlen(rjson))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlValueSetString( %s )         %d", VALUE, (int)strlen(rjson) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlValueSetString( %s )         %d", VALUE, (int)strlen(rjson) );
  }
  fprintf( zout, "%s\n", rjson );
  len = strlen(rjson);
  myDeAllocFunc( rjson ); rjson = NULL;

  char * v = oyjlValueText(value, myAllocFunc);
  if(v && strlen(v) == strlen(VALUE))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlValueText( \"new/tree/key\" ) = %s", v );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlValueText( \"new/tree/key\" ) = %s", VALUE );
  }
  myDeAllocFunc(v);

  oyjlTreeClearValue( root,"new/tree/key" );
  oyjlTreeToJson( root, &i, &rjson ); i = 0;
  if(!rjson)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeClearValue( \"new/tree/key\" ) " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeClearValue( \"new/tree/key\" ) " );
  }
  oyjlTreeFree( root );

  return result;
}



/* --- end actual tests --- */

#include "oyjl_test_main.h"

