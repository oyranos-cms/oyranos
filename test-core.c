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
  TEST_RUN( testTree, "Tree handling", 1 );

#include "oyjl_test.h"
#include "oyjl.h"
#include "oyjl_version.h"
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>
#endif
#include "oyjl_i18n.h"


/* --- actual tests --- */

oyjlTESTRESULT_e testVersion()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );
  fprintf(zout, "compiled version:     %d\n", OYJL_VERSION );
  fprintf(zout, " runtime version:     %d\n", oyjlVersion(0) );
  fprintf(zout, "sta yajl version:     %d\n", oyjlVersion(1) );

  if(OYJL_VERSION == oyjlVersion(0))
    result = oyjlTESTRESULT_SUCCESS;
  else
    result = oyjlTESTRESULT_FAIL;

  return result;
}

extern oyjlMessage_f oyjlMessage_p;
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

  setlocale(LC_ALL,"de_DE.UTF8");
  int use_gettext = 0;
#ifdef OYJL_USE_GETTEXT
  use_gettext = 1;
#endif
  int debug = 0;
  oyjlInitLanguageDebug( "Oyjl", "OYJL_DEBUG", &debug, use_gettext, "OYJL_LOCALEDIR", OYJL_LOCALEDIR, OYJL_DOMAIN, oyjlMessage_p );

  const char * lang = setlocale(LC_ALL, NULL);
  if(lang && (strcmp(lang, "C") != 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "setlocale() initialised good %s            ", lang );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "setlocale() initialised failed %s          ", lang );
  }

  const char * text = _("Example");
  if(strcmp(text,"Beispiel") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "dgettext() good \"%s\"                      ", text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "dgettext() failed \"%s\"                    ", text );
  }

  setlocale(LC_ALL,"");


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



oyjlTESTRESULT_e testTree ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int i;

  oyjl_val value = 0;
  oyjl_val root = 0;

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

  value = oyjlTreeGetValue( root, 0, "new/[0]" );
  int count = oyjlValueCount( value );
  if(count == 1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeGetValue(root, 0, \"new/[0]\") oyjlValueCount() %i", count );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeGetValue(root, 0, \"new/[0]\") oyjlValueCount() %i", count );
  }

  value = oyjlTreeGetValue( root, 0, "new/[]" );
  count = oyjlValueCount( value );
  if(count == 1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeGetValue(root, 0, \"new/[]\")  oyjlValueCount() %i", count );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeGetValue(root, 0, \"new/[]\")  oyjlValueCount() %i", count );
  }

  value = oyjlTreeGetValue( root, OYJL_CREATE_NEW, "new/[]" );
  count = oyjlValueCount( value );
  if(count == 1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeGetValue(root, OYJL_CREATE_NEW, \"new/[]\") oyjlValueCount() %i", count );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeGetValue(root, OYJL_CREATE_NEW, \"new/[]\") oyjlValueCount() %i", count );
  }


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

  value = oyjlTreeGetValue( root, 0, "new/tree/key" );
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

