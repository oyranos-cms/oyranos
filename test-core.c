/** @file test-core.c
 *
 *  Oyranos is an open source Color Management System
 *
 *  Copyright (C) 2004-2023  Kai-Uwe Behrmann
 *
 *  @brief    Oyranos test suite
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2008/12/04
 */

#ifdef OYJL_TEST_NAME
#define TEST_CORE_SKIP_MAIN 1
#endif

#ifdef progNAME
#undef progNAME
#endif
#ifndef TEST_CORE_SKIP_MAIN
#define TESTS_RUN \
  TEST_RUN( testVersion, "Version matching", 1 ); \
  TEST_RUN( testI18N, "Internationalisation", 1 ); \
  TEST_RUN( testString, "String handling", 1 ); \
  TEST_RUN( testArgs, "Options handling", 1 ); \
  TEST_RUN( testTree, "Tree handling", 1 ); \
  TEST_RUN( testIO, "File handling", 1 ); \
  TEST_RUN( testFunction, "Functions", 1 );

void oyjlLibRelease();
#define OYJL_TEST_MAIN_SETUP  printf("\n    %s Program\n", oyjlTermColor(oyjlBOLD, "OyjlCore Test"));
#define OYJL_TEST_MAIN_FINISH printf("\n    %s Program finished\n\n", oyjlTermColor(oyjlBOLD, "OyjlCore Test")); oyjlLibRelease(); if(oyjl_print_sub) free(oyjl_print_sub);
#define OYJL_TEST_NAME "test-core"
#include "oyjl.h"
#include "oyjl_test_main.h"
#define progNAME( name ) name
#else
#define progNAME( name ) name##Core
#endif

#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>
#endif
#include "oyjl_i18n_internal.h"
#include "oyjl_internal.h"
#include "oyjl_tree_internal.h"

#define oyjlNoEmpty(x) ((x)?(x):"---")
char *     oyjlTreePrint             ( oyjl_val            v );

/* --- actual tests --- */

#ifndef TEST_CORE_SKIP_MAIN
oyjlTESTRESULT_e progNAME(testVersion) ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );
  fprintf(zout, "compiled version:     %d\n", OYJL_VERSION );
  fprintf(zout, " runtime version:     %d\n", oyjlVersion(0) );

  if(OYJL_VERSION == oyjlVersion(0))
    result = oyjlTESTRESULT_SUCCESS;
  else
    result = oyjlTESTRESULT_FAIL;

  return result;
}

extern oyjlMessage_f oyjlMessage_p;
int          oyjlMessageFunc         ( int/*oyjlMSG_e*/    error_code,
                                       const void        * context_object OYJL_UNUSED,
                                       const char        * format,
                                       ... );
oyjlTESTRESULT_e progNAME(testI18N) ()
{
  const char * clang;
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  clang = setlocale(LC_ALL, NULL);
  if((clang && (strcmp(clang, "C") == 0)) || !clang)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "Language uninitialised good \"C\"" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "Language uninitialised good \"C\"" );
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

  clang = setlocale(LC_ALL,"");

  if(clang && (strstr(clang, "de_DE") != 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "Language initialised good %s", clang?clang:"---" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "Language initialised failed %s", clang?clang:"---" );
  }

  char * language = oyjlLanguage( clang );
  if(language && (strstr(language, "de") != 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlLanguage() good %s", language?language:"---" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlLanguage() good %s", language?language:"---" );
  }
  if(language) free(language);

  char * country = oyjlCountry( clang );
  if(country && (strstr(country, "DE") != 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlCountry() good %s", country?country:"---" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlCountry() good %s", country?country:"---" );
  }
  if(country) free(country);

  oyjlTranslation_s * trc = NULL;
  const char * loc = NULL;
  const char * lang = getenv("LANG");
  int use_gettext = 0;
  static int my_debug = 0;
#ifdef OYJL_USE_GETTEXT
  use_gettext = 1;
#endif
#ifdef OYJL_HAVE_LOCALE_H
  loc = setlocale(LC_ALL,"de_DE.UTF8");
#endif
  if(!loc)
    fprintf( zout, "setlocale() not available or failed\n" );
  if(!loc && lang)
    loc = lang;
  if(loc)
  {
# include "liboyjl.i18n.h"
    int size = sizeof(liboyjl_i18n_oiJS);
    oyjl_val oyjl_catalog = (oyjl_val) oyjlStringAppendN( NULL, (const char*) liboyjl_i18n_oiJS, size, malloc );
    if(*oyjl_debug)
      oyjlMessage_p( oyjlMSG_INFO, 0, "loc: \"%s\" domain: \"%s\" catalog-size: %d", loc, OYJL_DOMAIN, size );
    trc = oyjlTranslation_New( loc, OYJL_DOMAIN, &oyjl_catalog, 0,0,0,0 );
  }
  oyjlInitLanguageDebug( "Oyjl", "OYJL_DEBUG", &my_debug, use_gettext, "OYJL_LOCALEDIR", OYJL_LOCALEDIR, &trc, oyjlMessageFunc );
  oyjlTranslation_Release( &trc );

  lang = setlocale(LC_ALL, NULL);
  if(lang && (strcmp(lang, "C") != 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "setlocale() initialised good %s", lang );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "setlocale() initialised failed %s", lang );
  }

  const char * text = _("Example");
  if(strcmp(text,"Beispiel") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "dgettext() good \"%s\"", text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "dgettext() failed \"%s\"", text );
  }

  setlocale(LC_ALL,"");


  return result;
}
#endif

#define TEST_DOMAIN "org/freedesktop/openicc/tests"
#define TEST_DOMAIN2 "org2/freedesktop2/openicc2/tests2"
#define TEST_KEY "/test_key"

static void replaceCb(const char * text OYJL_UNUSED, const char * start, const char * end, const char * search, const char ** replace, int * r_len, void * data)
{
  if(start < end)
  {
    const char * word = start;
    int * test = (int*) data;
    while(word && (word = strstr(word+1,"d")) != NULL && word < end)
      ++test[0];
    word = start;
    while(word && (word = strstr(word+1,"e")) != NULL && word < end)
      --test[0];
    if(test[0] < 0) test[0] = 0;
    word = start;

    if( test[0] )
    {
      *replace = search;
      *r_len = 1;
    }
    else
    {
      *replace = "\\/";
      *r_len = 2;
    }
  }
}

int        oyjlStr_Append            ( oyjl_str            string,
                                       const char        * append );
oyjlTESTRESULT_e testString ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int i;

  const char * test;
  

  test = TEST_DOMAIN "/display.oydi/display_name";
  fprintf(zout, "test \"%s\" %d\n", test, (int)strlen(test));
  char * test_out = oyjlStringCopy(test,malloc);
  int n = oyjlStringReplace( &test_out, TEST_DOMAIN, TEST_DOMAIN2, 0,0 );
  if(verbose)
    fprintf(zout, "test %s \"%s\"\n", test, test_out);
  if( strstr(test_out, TEST_DOMAIN2 ) != NULL &&
      strlen(test_out) == 59 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringReplace(start) %d 59 == %d", n, (int)strlen(test_out) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(start) %d 59 == %d", n, (int)strlen(test_out) );
  }
  myDeAllocFunc(test_out);

  test_out = oyjlStringCopy(test,malloc);
  n = oyjlStringReplace( &test_out, "display.", "foo.", 0,0 );
  if(verbose)
    fprintf(zout, "test %s \"%s\"\n", test, test_out);
  if( strstr(test_out, "foo" ) != NULL &&
      strlen(test_out) == 51 &&
      n == 1 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringReplace(middle) 51 == %d", (int)strlen(test_out) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(middle) 51 == %d", (int)strlen(test_out) );
  }
  myDeAllocFunc(test_out);

  test_out = oyjlStringCopy(test,malloc);
  n = oyjlStringReplace( &test_out, "display_name", "bar", 0,0 );
  if(verbose)
    fprintf(zout, "test %s \"%s\"\n", test, test_out);
  if( strstr(test_out, "bar" ) != NULL &&
      strstr(test_out, "barbar" ) == NULL &&
      strlen(test_out) == 46 &&
      n == 1 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringReplace(end)    46 == %d", (int)strlen(test_out) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(end)    46 == %d", (int)strlen(test_out) );
  }
  if(verbose)
    fprintf( stderr, "%s \"display_name\"->\"bar\" %s\n", test, test_out );
  myDeAllocFunc(test_out);

  test_out = oyjlStringCopy("abc]",malloc);
  n = oyjlStringReplace( &test_out, "]", " ] ", 0,0 );
  if(verbose)
    fprintf(zout, "test %s \"%s\"\n", test, test_out);
  if( strstr(test_out, " ] " ) != NULL &&
      strstr(test_out, "c]" ) == NULL &&
      strlen(test_out) == 6 &&
      n == 1 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringReplace(end)      6 == %d", (int)strlen(test_out) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(end)      6 == %d", (int)strlen(test_out) );
  }
  if(verbose)
    fprintf( stderr, "\"%s\" \"]\"->\" ] \" \"%s\"\n", "abc]", test_out );
  myDeAllocFunc(test_out);

  char * compare;
  compare = test_out = oyjlStringCopy(test,malloc);
  n = oyjlStringReplace( &test_out, "not_inside", "bar", 0,0 );
  if(verbose)
    fprintf(zout, "test %s \"%s\"\n", test, test_out);
  if( compare && compare == test_out &&
      n == 0 &&
      strcmp(compare,test) == 0 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringReplace(none)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(none)" );
  }
  myDeAllocFunc(test_out);
  compare = NULL;

  test_out = oyjlStringCopy(test,malloc);
  n = oyjlStringReplace( &test_out, "display", "moni", 0,0 );
  if(verbose)
    fprintf(zout, "test %s \"%s\"\n", test, test_out);
  if( strlen(test) > strlen(test_out) &&
      strlen(test_out) == 49 &&
      n == 2 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringReplace(shorter) 49 == %d", (int)strlen(test_out) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(shorter) 49 == %d", (int)strlen(test_out) );
  }
  myDeAllocFunc(test_out);

  test_out = oyjlStringCopy(test,malloc);
  n = oyjlStringReplace( &test_out, "display", "monitorXYZ", 0,0 );
  if(verbose)
    fprintf(zout, "test %s \"%s\"\n", test, test_out);
  if( strlen(test) < strlen(test_out) &&
      strlen(test_out) == 61 &&
      n == 2 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringReplace(longer) 61 == %d", (int)strlen(test_out) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(longer) 61 == %d", (int)strlen(test_out) );
  }
  myDeAllocFunc(test_out);

  test_out = oyjlStringCopy("some %s text",malloc);
  n = oyjlStringReplace( &test_out, "\%", "\%\%", 0,0 );
  if(verbose)
    fprintf(zout, "test \"%s\"\n", test_out);
  if( strlen(test_out) == 13 &&
      n == 1 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringReplace(\%) == %d", (int)strlen(test_out) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(\%) == %s %d", test_out, (int)strlen(test_out) );
  }
  myDeAllocFunc(test_out);

  if(oyjlStringFind("test", "test", OYJL_COMPARE_EXACT) != -1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringFind( OYJL_COMPARE_EXACT)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringFind(OYJL_COMPARE_EXACT)" );
  }
  if(oyjlStringFind("test", "none", OYJL_COMPARE_EXACT) != -1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "!oyjlStringFind(OYJL_COMPARE_EXACT)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "!oyjlStringFind(OYJL_COMPARE_EXACT)" );
  }

  if(oyjlStringFind("test", "Test", OYJL_COMPARE_CASE) != -1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringFind( OYJL_COMPARE_CASE)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringFind(OYJL_COMPARE_CASE)" );
  }
  if(oyjlStringFind("test", "None", OYJL_COMPARE_CASE) != -1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "!oyjlStringFind(OYJL_COMPARE_CASE)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "!oyjlStringFind(OYJL_COMPARE_CASE)" );
  }

  if(oyjlStringFind("test", "st", OYJL_COMPARE_LAZY) != -1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringFind( OYJL_COMPARE_LAZY)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringFind(OYJL_COMPARE_LAZY)" );
  }
  if(oyjlStringFind("test", "no", OYJL_COMPARE_LAZY) != -1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "!oyjlStringFind(OYJL_COMPARE_LAZY)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "!oyjlStringFind(OYJL_COMPARE_LAZY)" );
  }

  if(oyjlStringFind("test", "te", OYJL_COMPARE_STARTS_WITH) != -1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringFind( OYJL_COMPARE_STARTS_WITH)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringFind(OYJL_COMPARE_STARTS_WITH)" );
  }
  if(oyjlStringFind("test", "st", OYJL_COMPARE_STARTS_WITH) != -1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "!oyjlStringFind(OYJL_COMPARE_STARTS_WITH)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "!oyjlStringFind(OYJL_COMPARE_STARTS_WITH)" );
  }


  int list_n = 0;
  int *index = NULL;
  const char * text = "org/domain/eins.lib,org/domain/zwei.txt;org/domain/drei.lib?net/welt/vier.lib:net/welt/vier.txt$/net/welt/fuenf;/net/welt/fuenf";
  char ** list = oyjlStringSplit2( text, ";:,?$&§", 0, &list_n, &index, myAllocFunc );

  if( list_n == 7 &&
      (index && text[index[0]] == ',') &&
      (index && text[index[1]] == ';') &&
      (index && text[index[2]] == '?') &&
      (index && text[index[3]] == ':') &&
      (index && text[index[4]] == '$') &&
      (index && text[index[5]] == ';')
    )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringSplit2()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringSplit2()" );
  }
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || verbose)
  {
    fprintf( zout, " text: \"%s\"\n", text );
    for(i = 0; i < list_n; ++i)
      fprintf(zout, " list[%d] \"%s\" index in text of delimiter: %d  delimiter:\'%c\'\n", i, list[i], index ? index[i] : -1, index && index[i] ? text[index[i]] : '0' );
  }
  oyjlStringListRelease( &list, list_n, myDeAllocFunc );
  if(index) { myDeAllocFunc( index ); index = NULL; }

  list_n = 0;
  list = oyjlStringSplit( "org/domain/eins.lib;org/domain/zwei.txt;org/domain/drei.lib;net/welt/vier.lib;net/welt/vier.txt;/net/welt/fuenf;/net/welt/fuenf", ';', &list_n, myAllocFunc );

  oyjlStringListFreeDoubles( list, &list_n, myDeAllocFunc );
  if( list_n == 6 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringListFreeDoubles()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringListFreeDoubles()" );
    for(i = 0; i < list_n; ++i)
      fprintf(zout, " list[%d] \"%s\"\n", i, list[i] );
  }

  oyjlStringListFree( list, &list_n, 4, 8, myDeAllocFunc );
  if(list_n == 4 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringListFree(4,8) list_n; %d", list_n );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringListFree(4,8) list_n; %d", list_n );
    for(i = 0; i < list_n; ++i)
      fprintf(zout, " list[%d] \"%s\"\n", i, list[i] );
  }

  oyjlStringListFree( list, &list_n, 1, 1, myDeAllocFunc );
  if(list_n == 3 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringListFree(1,1) list_n: %d", list_n );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringListFree(1,1) list_n; %d", list_n );
    for(i = 0; i < list_n; ++i)
      fprintf(zout, " list[%d] \"%s\"\n", i, list[i] );
  }

  if(oyjlStringListFind( list, &list_n, "", 0,0 ) < 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringListFind(\"\") < 0" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringListFind(\"\") < 0" );
    for(i = 0; i < list_n; ++i)
      fprintf(zout, " list[%d] \"%s\"\n", i, list[i] );
  }

  if(oyjlStringListFind( list, &list_n, list[2], 0,0 ) == 2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringListFind(\"%s\") == [2/%d]", list[2], list_n );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringListFind(\"%s\") == [2/%d]", list[2], list_n );
    for(i = 0; i < list_n; ++i)
      fprintf(zout, " list[%d] \"%s\"\n", i, list[i] );
  }

  if(oyjlStringListFind( list, &list_n, list[1], OYJL_REMOVE, free ) == 1 && list_n == 2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringListFind(OYJL_REMOVE) list_n: %d", list_n );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringListFind(OYJL_REMOVE) list_n: %d", list_n );
    for(i = 0; i < list_n; ++i)
      fprintf(zout, " list[%d] \"%s\"\n", i, list[i] );
  }
  oyjlStringListRelease( &list, list_n, myDeAllocFunc );

  list_n = 0;
  list = oyjlStringSplit( "a a b c\td\ne", 0, &list_n, myAllocFunc );
  if(list_n == 6)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringSplit(0)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringSplit(0)" );
  }
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || verbose)
  {
    for(i = 0; i < list_n; ++i)
      fprintf(zout, " list[%d] \"%s\"\n", i, list[i] );
  }
  oyjlStringListRelease( &list, list_n, myDeAllocFunc );

  char * t = oyjlStringCopy( "word", 0 );;
  oyjlStringPrepend( &t, "pre-", 0,0 );
  if(strcmp(t, "pre-word") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringPrepend()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringPrepend()" );
  }
  oyjlStringPush( &t, "-end", 0,0 );
  if(strcmp(t, "pre-word-end") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringPush()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringPush()" );
  }
  free(t); t = NULL;

  test = "element";
  const char * slist[] = { test, NULL };
  list_n = 0;
  oyjlStringListAddList( &list, &list_n, slist, 1, 0, 0 );
  oyjlStringListAddList( &list, &list_n, slist, 1, 0, 0 );
  for(i = 0; i < list_n; ++i)
    oyjlStringAdd( &t, 0,0, "%s%s", i?",":"", list[i] );
  if(strcmp(t, "element,element") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringListAddList(list, &string)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringListAddList(list, &string)" );
  }
  free(t); t = NULL;
  oyjlStringListRelease( &list, list_n, 0 );
  list_n = 0;

  double clck = oyjlClock();
  for( i = 0; i < 10000; ++i )
    oyjlStringListPush( &list, &list_n, "text ", malloc,free );
  if(list && list_n)
    oyjlStringListRelease( &list, list_n, free );
  clck = oyjlClock() - clck;
  if( list_n == 10000 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"wr",
    "oyjlStringListPush()" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, list_n,
    "oyjlStringListPush()" );
  }

  clck = oyjlClock();
  list_n = 0;
  for( i = 0; i < 10000; ++i )
    oyjlStringListAdd( &list, &list_n, malloc,free, "%d ", i );
  if(list && list_n)
    oyjlStringListRelease( &list, list_n, free );
  clck = oyjlClock() - clck;
  if( list_n == 10000 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"wr",
    "oyjlStringListAdd()" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, list_n,
    "oyjlStringListAdd()" );
  }

  long l = 0;
  const char * end = NULL;
  int error = oyjlStringToLong( "2", &l, &end );
  if( !error &&
      l == 2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringToLong(\"2\") = %ld  error = %d", l, error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringToLong(\"2\") = %ld  error = %d", l, error );
  }

  l = 0;
  error = oyjlStringToLong( "2\n", &l, &end );
  if( error == -1 &&
      l == 2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringToLong(\"2\n\") = %ld  error = %d", l, error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringToLong(\"2\n\") = %ld", l );
  }

  l = -2;
  error = oyjlStringToLong( "", &l, &end );
  if( error == 1 &&
      l == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringToLong(\"\") = %ld  error = %d", l, error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringToLong(\"\") = %ld", l );
  }

  double d = 0.0;
  error = oyjlStringToDouble( "0.2", &d, &end, 0 );
  if( !error &&
      d == 0.2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringToDouble(\"0.2\") = %g  error = %d", d, error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringToDouble(\"0.2\") = %g  error = %d", d, error );
  }

  d = 0.0;
  error = oyjlStringToDouble( "0.2#", &d, &end, 0 );
  if( error == -1 &&
      d == 0.2 && end[0] == '#' )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringToDouble(\"0.2#\") = %g  error = %d  end = %s", d, error, end );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringToDouble(\"0.2#\") = %g  error = %d  end = %s", d, error, end );
  }

  d = 0.0;
  error = oyjlStringToDouble( "val_a", &d, &end, 0 );
  if( error )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringToDouble(\"val_a\")   error = %d", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringToDouble(\"val_a\") = %g  error = %d", d, error );
  }

  double * doubles = NULL;
  int count = 0;
  error = oyjlStringsToDoubles( "0.2 1 3.5", " ", &count, malloc, &doubles );
  if( !error &&
      doubles[0] == 0.2 &&
      doubles[1] == 1.0 &&
      doubles[2] == 3.5 &&
      count == 3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringsToDoubles(\"0.2 1 3.5\") error = %d", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringsToDoubles(\"0.2 1 3.5\") error = %d", error );
  }
  if(doubles) { free(doubles); } doubles = NULL;

  error = oyjlStringsToDoubles( "0.2", " ", &count, malloc, &doubles );
  if( !error &&
      doubles[0] == 0.2 &&
      count == 1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringsToDoubles(\"0.2\") error = %d", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringsToDoubles(\"0.2\") error = %d", error );
  }
  if(doubles) { free(doubles); } doubles = NULL;

  error = oyjlStringsToDoubles( "0.2;1;3.5", ";", &count, malloc, &doubles );
  if( !error &&
      doubles[0] == 0.2 &&
      doubles[1] == 1.0 &&
      doubles[2] == 3.5 &&
      count == 3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringsToDoubles(\"0.2;1;3.5\") error = %d", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringsToDoubles(\"0.2;1;3.5\") error = %d", error );
  }
  if(doubles) { free(doubles); } doubles = NULL;

  error = oyjlStringsToDoubles( "0.2 ; 1; 3.5 cm", ";", &count, malloc, &doubles );
  if( doubles[0] == 0.2 &&
      doubles[1] == 1.0 &&
      doubles[2] == 3.5 &&
      count == 3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringsToDoubles(\"0.2 ; 1; 3.5 cm\") error = %d", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringsToDoubles(\"0.2 ; 1; 3.5 cm\") error = %d", error );
  }
  if(doubles) { free(doubles); } doubles = NULL;

  error = oyjlStringsToDoubles( "0.2,1,3.5", ",", &count, malloc, &doubles );
  if( !error &&
      doubles[0] == 0.2 &&
      doubles[1] == 1.0 &&
      doubles[2] == 3.5 &&
      count == 3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringsToDoubles(\"0.2,1,3.5\") error = %d", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringsToDoubles(\"0.2,1,3.5\") error = %d", error );
  }
  if(doubles) { free(doubles); } doubles = NULL;

  error = oyjlStringsToDoubles( "2\t1.2\t 27", 0, &count, malloc, &doubles );
  if( error == 0 &&
      doubles[0] == 2 &&
      doubles[1] == 1.2 &&
      doubles[2] == 27 &&
      count == 3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringsToDoubles(\"2\\t1.2\\t 27\") error = %d", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringsToDoubles(\"2\\t1.2\\t 27\") error = %d", error );
  }
  if(verbose)
  {
    for(i = 0; i < count; ++i)
      fprintf( zout, "%d:%f ", i, doubles[i] );
    fprintf( zout, "\n" );
  }
  if(doubles) { free(doubles); } doubles = NULL;

  error = oyjlStringsToDoubles( "\n \t2\t 1.2\t 27\t\n", 0, &count, malloc, &doubles );
  if( error == 0 &&
      doubles[0] == 2 &&
      doubles[1] == 1.2 &&
      doubles[2] == 27 &&
      count == 3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringsToDoubles(\"\\n \\t2\\t 1.2\\t 27\\t\\n\") er = %d", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringsToDoubles(\"\\n \\t2\\t 1.2\\t 27\\t\\n\") er = %d", error );
  }
  if(verbose)
  {
    for(i = 0; i < count; ++i)
      fprintf( zout, "%d:%f ", i, doubles[i] );
    fprintf( zout, "\n" );
  }
  if(doubles) { free(doubles); } doubles = NULL;

  error = oyjlStringsToDoubles( "x=0.2,one dot five,", ",", &count, malloc, &doubles );
  if( error > 0 &&
      count == 3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringsToDoubles(\"x=0.2:,one dot five,\") err = %d", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringsToDoubles(\"x=0.2:,one dot five,\") err = %d", error );
  }
  if(doubles) { free(doubles); } doubles = NULL;



  int found = oyjlStringSplitFind( "gui:start=instant:repeat_s=2", ":", "repeat_s", OYJL_COMPARE_STARTS_WITH, &t, malloc, free );
  if( found >= 0 &&
      t && strcmp(strchr(t,'=')+1,"2") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringSplitFind(\"ui:start=instant:repeat_s=2\",\"repeat_s\",OYJL_COMPARE_STARTS_WITH) found = %d t = \"%s\"", found, t );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringSplitFind(\"ui:start=instant:repeat_s=2\",\"repeat_s\",OYJL_COMPARE_STARTS_WITH) found = %d t = \"%s\"", found, t );
  }
  if(t) { free(t); t = NULL; }

  found = oyjlStringSplitFind( "gui:start=instant:repeat_s=2", ":", "start=instant", 0, NULL, 0,0 );
  if( found == 1 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringSplitFind(\"ui:start=instant:repeat_s=2\",\"start=instant\") found = %d", found );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringSplitFind(\"ui:start=instant:repeat_s=2\",\"start=instant\") found = %d", found );
  }


  clck = oyjlClock();
  n = 1000;
  for(i = 0; i < n; ++i)
    oyjlStringAdd( &t, 0,0, "/%s/%s", "more", "and" );
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n", oyjlPrintSubProfiling( -1, n, clck/(double)CLOCKS_PER_SEC,"ops",
    "oyjlStringAdd()            %dx9        %d", n, (int)strlen(t)) );
  if(t) { free(t); t = NULL; }

  clck = oyjlClock();
  for(i = 0; i < n; ++i)
    oyjlStringAddN( &t, "/more/and", 9, malloc,free );
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n", oyjlPrintSubProfiling( -1, n, clck/(double)CLOCKS_PER_SEC,"ops",
    "oyjlStringAddN()           %dx9        %d", n, (int)strlen(t)) );

  n = 1000;
  clck = oyjlClock();
  int len OYJL_UNUSED;
  for(i = 0; i < n; ++i)
    len = strlen( t );
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n", oyjlPrintSubProfiling( -1, n, clck/(double)CLOCKS_PER_SEC,"ops",
    "strlen()                   %dx          %d", n, (int)strlen(t)) );

  if(t) { free(t); t = NULL; }

  n = 1000000;
  t = calloc(10*n,sizeof(char));
  clck = oyjlClock();
  for(i = 0; i < n; ++i)
    memcpy( &t[9*i], "/more/and", 9 );
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n", oyjlPrintSubProfiling( -1, n, clck/(double)CLOCKS_PER_SEC,"ops",
    "memcpy()                   %dx9    %d", n, (int)strlen(t)) );
  if(t) { free(t); t = NULL; }

  oyjl_str string = oyjlStr_New(10, 0,0);
  clck = oyjlClock();
  n = 1000000;
  for(i = 0; i < n; ++i)
    oyjlStr_AppendN( string, "/more/and", 9 );
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n", oyjlPrintSubProfiling( -1, n, clck/(double)CLOCKS_PER_SEC,"ops",
    "oyjlStr_AppendN() memcpy() %dx9    %d", n, (int)strlen(oyjlStr(string))) );
  oyjlStr_Clear( string );

  clck = oyjlClock();
  n = 1000000;
  for(i = 0; i < n; ++i)
    oyjlStr_Push( string, "/more/and" );
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n", oyjlPrintSubProfiling( -1, n, clck/(double)CLOCKS_PER_SEC,"ops",
    "oyjlStr_Push() memcpy()    %dx9    %d", n, (int)strlen(oyjlStr(string))) );
  oyjlStr_Clear( string );

  clck = oyjlClock();
  n = 5000;
  for(i = 0; i < n; ++i)
    oyjlStr_Append( string, "/more/and" );
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n", oyjlPrintSubProfiling( -1, n, clck/(double)CLOCKS_PER_SEC,"ops",
    "oyjlStr_Append() strcpy()  %dx9        %d", n, (int)strlen(oyjlStr(string))) );
  oyjlStr_Clear( string );
  oyjlStr_Release( &string );

  int inside = 0;
  string = oyjlStr_New(10, 0,0);
  n = 1000;
  const char * more = "more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/more/and/";
  len = strlen(more);
  clck = oyjlClock();
  for(i = 0; i < n; ++i)
  {
    oyjlStr_Append( string, more );
    oyjlStr_Append( string, "  " );
  }
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n", oyjlPrintSubProfiling( -1, n, clck/(double)CLOCKS_PER_SEC,"ops",
    "oyjlStr_Append() strcpy()  %dx250+2  %d", 2*n, (int)strlen(oyjlStr(string))) );
  n = 1;
  clck = oyjlClock();
  oyjlStr_Replace( string, "  ", "&nbsp;&nbsp;", NULL, &inside );
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n", oyjlPrintSubProfiling( -1, n, clck/(double)CLOCKS_PER_SEC,"ops",
    "oyjlStr_Replace(\"  \",\"&nbsp;&nbsp;\")   %dx     %d", n, (int)strlen(oyjlStr(string))) );
  oyjlStr_Clear( string );
  oyjlStr_Release( &string );

  string = oyjlStr_New(10, 0,0);
  oyjl_str tstr = oyjlStr_New(10, 0,0);
  n = 1000;
  for(i = 0; i < n; ++i)
  {
    oyjlStr_Append( string, more );
    oyjlStr_Append( string, "  " );
  }
  const char * start, * search = "  ", * replacement = "&nbsp;&nbsp;";
  {
    const char * end, * last;
    start = end = oyjlStr(string);
    int s_len = strlen( search );
    last = end + strlen(start);
    n = 0;
    clck = oyjlClock();
    while((end = strstr(start,search)) != 0)
    {
      oyjlStr_AppendN( tstr, start, end-start );
      oyjlStr_Push( tstr, replacement );
      if(end + (size_t)s_len < last)
        start = end + s_len;
      else
      {
        if(strstr(start,search) != 0)
          oyjlStr_Push( tstr, replacement );
        start = end = end + s_len;
        break;
      }
      ++n;
    }
    clck = oyjlClock() - clck;
  }
  fprintf( zout, "%s\n", oyjlPrintSubProfiling( -1, 2*n, clck/(double)CLOCKS_PER_SEC,"ops",
    "oyjlStr_Replace() strcpy() %dx(250+2)/%d  %d", 2*n, len, (int)strlen(oyjlStr(string))) );

  start = oyjlStr(string);
  n = 1000000;
  clck = oyjlClock();
  for(i = 0; i < n; ++i)
    t = strstr(start, search);
  clck = oyjlClock() - clck;
  if(!t) len = 0;
  fprintf( zout, "%s\n", oyjlPrintSubProfiling( -1, n, clck/(double)CLOCKS_PER_SEC,"ops",
    "strstr() %dx(250+2)/%d  %d %g", n, len, (int)strlen(oyjlStr(string)), clck) );

  n = 100;
  clck = oyjlClock();
  for(i = 0; i < n; ++i)
    len = strlen(start);
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n", oyjlPrintSubProfiling( -1, n, clck/(double)CLOCKS_PER_SEC,"ops",
    "strlen() %dx%d  %d %g", n, len, (int)strlen(oyjlStr(string)), clck) );
  oyjlStr_Clear( string );
  oyjlStr_Release( &string );
  oyjlStr_Clear( tstr );
  oyjlStr_Release( &tstr );


  string = oyjlStr_New(10, 0,0);
  for(i = 0; i < 10; ++i)
    oyjlStr_Append( string, "/more/and" );
  inside = 0;
  oyjlStr_Replace( string, "/", "\\/", replaceCb, &inside );
  const char * tmp = oyjlStr(string);
  if(strstr(tmp, "/more\\/nd/more"))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStr_Replace(callback,user_data) = %s", tmp );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStr_Replace(callback,user_data) = %s", tmp );
  }
  oyjlStr_Release( &string );

  const char * wstring = "反差";
  int wlen = oyjlStringSplitUTF8( wstring, NULL, 0 );
  if(wlen == 2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringSplitUTF8( %s ) = count                %d", wstring, wlen );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringSplitUTF8( %s ) = count                %d", wstring, wlen );
  }

  list = NULL; len = 0;
  wlen = oyjlStringSplitUTF8(wstring, &list, myAllocFunc);
  if(wlen && list)
    len = strlen(list[0]);
  if(wlen == 2 && len == 3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringSplitUTF8( \"%s\" , \"%s\" )               %d %d", list[0], list[1], (int)strlen(list[0]), (int)strlen(list[1]) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringSplitUTF8( )                              %d", len );
  }
  oyjlStringListRelease( &list, wlen, myDeAllocFunc );

  char * txt = oyjlStringCopy( "my_string_1_is_long", 0 );
  const char * regexp = "*string";
  const char * match;
  len = 0;
  if(oyjlRegExpFind(txt, regexp, &len) == NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "!oyjlRegExpFind( \"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "!oyjlRegExpFind( \"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  }
  regexp = "_string_";
  if((match = oyjlRegExpFind(txt, regexp, &len)) != NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlRegExpFind( \"%s\", \"%s\" ) %s", txt, oyjlNoEmpty(regexp), match );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlRegExpFind( \"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  }

  regexp = "NONE";
  if((match = oyjlRegExpFind(txt, regexp, &len)) == NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "!oyjlRegExpFind( \"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "!oyjlRegExpFind( \"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  }

  regexp = "1";
  if((match = oyjlRegExpFind(txt, regexp, &len)) != NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlRegExpFind( \"%s\", \"%s\" ) %s", txt, oyjlNoEmpty(regexp), match );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlRegExpFind( \"%s\", \"%s\" ) ", txt, oyjlNoEmpty(regexp) );
  }

  regexp = "string.1";
  if((match = oyjlRegExpFind(txt, regexp, &len)) != NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlRegExpFind( \"%s\", \"%s\" ) %s", txt, oyjlNoEmpty(regexp), match );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlRegExpFind( \"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  }

  regexp = "str.*lon";
  if((match = oyjlRegExpFind(txt, regexp, &len)) != NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlRegExpFind( \"%s\", \"%s\" ) %s", txt, oyjlNoEmpty(regexp), match );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlRegExpFind( \"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  }

  regexp = "st[.]*lo";
  if(oyjlRegExpFind(txt, regexp, &len) == NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "!oyjlRegExpFind(\"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "!oyjlRegExpFind(\"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  }

  regexp = "/de.*/The API's\\.";
  oyjlStringAdd( &txt, 0,0, "/de_DE.UTF8/The API's." );
  if((match = oyjlRegExpFind(txt, regexp, &len)) != NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlRegExpFind(\"%s\", \"%s\" ) %s", txt, oyjlNoEmpty(regexp), oyjlNoEmpty(match) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlRegExpFind(\"%s\", \"%s\" ) %s", txt, oyjlNoEmpty(regexp), oyjlNoEmpty(match) );
  }
  free(txt); txt = NULL;

  char * pattern = NULL;
#define REGEX_ESCAPE( pre, regex_, escape ) \
  { int len;\
    t = oyjlRegExpEscape( escape ); \
    oyjlStringAdd( &compare, 0,0, "%s%s", pre, escape ); \
    oyjlStringAdd( &pattern, 0,0, "%s%s", regex_, t ); \
    if(oyjlRegExpFind( compare, pattern, &len ) != NULL) \
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS, \
      "oyjlRegExpEscape( \"%s\" ) %d", escape, len ); \
    } else \
    { PRINT_SUB( oyjlTESTRESULT_FAIL, \
      "oyjlRegExpEscape( \"%s\" ) %s -> oyjlRegExpFind(\"%s\", \"%s\")", escape, t, compare, pattern ); \
    } \
    free(t); t = NULL; \
    free(compare); compare = NULL; \
    free(pattern); pattern = NULL; \
  }
  REGEX_ESCAPE( "/stri/n/g_", "str.*", "_string" )
  REGEX_ESCAPE( "/stri/n/g_", "str.*", " (string) " )
  REGEX_ESCAPE( "/stri/n/g_", "str.*", "!string" )
  REGEX_ESCAPE( "/stri/n/g_", "str.*", "^string" )
  REGEX_ESCAPE( "/stri/n/g_", "str.*", "string^" )
  REGEX_ESCAPE( "/stri/n/g_", "str.*", "?<=string" )
  REGEX_ESCAPE( "/stri/n/g_", "str.*", "<string>" )
  REGEX_ESCAPE( "/stri/n/g_", "str.*", "[:digit:]" )
  REGEX_ESCAPE( "/stri/n/g_", "str.*", "[s,S]ring" )
  REGEX_ESCAPE( "/stri/n/g_", "str.*", "$+string" )
  REGEX_ESCAPE( "/stri/n/g_", "str.*", "string.*" )
  REGEX_ESCAPE( "/stri/n/g_", "str.*", "string.+" )
  REGEX_ESCAPE( "/stri/n/g_", "str.*", "string{3,}" )
  REGEX_ESCAPE( "/stri/n/g_", "str.*", "(ring|Ring)" )

#if 0
  { char * text = oyjlStringCopy( "rotation", 0 ); \
    oyjlRegExpReplace( &text, "tat", "" ); \
    if(text && strcmp( text, "roion" ) == 0) \
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS, \
      "oyjlRegExpReplace( \"%s\" )", "tat" ); \
    } else \
    { PRINT_SUB( oyjlTESTRESULT_FAIL, \
      "oyjlRegExpReplace( \"%s\" )", "tat" ); \
    } \
    free(text); text = NULL; \
  }
#endif

  regexp = "tat";
  txt = "rotation";
  if((match = oyjlRegExpFind( txt, regexp, &len)) != NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlRegExpFind( \"%s\", \"%s\" ) = %s", txt, oyjlNoEmpty(regexp), match );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlRegExpFind( \"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  }
#define REGEX_REPLACE( string_, regex_, replacement, check ) \
  { char * print = NULL; \
    t = oyjlStringCopy( string_, 0 ); \
    oyjlRegExpReplace( &t, regex_, replacement ); \
    oyjlStringAdd( &print, 0,0, "oyjlRegExpReplace( \"%s\", \"%s\", \"%s\" ) = \"%s\"", string_, regex_, replacement, t ); \
    n = oyjlStringReplace( &print, "\%s", "\%\%\%\%s", 0,0 ); \
    if(verbose) \
    { fputs( print, stderr ); fputs( "\n", stderr ); } \
    if(text && strcmp( t, check ) == 0) \
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS, \
      print ); \
    } else \
    { PRINT_SUB( oyjlTESTRESULT_FAIL, \
      print ); \
    } \
    free(print); print = NULL; \
    if(t) { free(t); t = NULL; } \
  }
  REGEX_REPLACE( "rotation", "ot", "ehabilit", "rehabilitation" )
  REGEX_REPLACE( "\033[1mSomeText\033[0m \033[38;2;0;200;0mSomeMoreText\033[0m", "\033[[0-9;]*m", "", "SomeText SomeMoreText" )
  const char * rexexp = "((([a-z]+://)?[a-zA-Z0-9-]+\\.[a-zA-Z0-9-]+\\.[a-zA-Z0-9-]+)|([a-z]+://)+[a-zA-Z0-9-]+\\.[a-zA-Z0-9-]+)(:[0-9]{1,5})?([/a-zA-Z0-9+-.?=_*]*)?";
  replacement = "<a href=\"%s\">%s</a>";
  REGEX_REPLACE( "start www.url.org:8888/path?x=2 end.", rexexp, replacement, "start <a href=\"www.url.org:8888/path?x=2\">www.url.org:8888/path?x=2</a> end.")
  REGEX_REPLACE( "start.of.adress (http://www.url.org) just-a.dot ending.adress.org", rexexp, replacement, "<a href=\"start.of.adress\">start.of.adress</a> (<a href=\"http://www.url.org\">http://www.url.org</a>) just-a.dot <a href=\"ending.adress.org\">ending.adress.org</a>")
  REGEX_REPLACE( "start www.url.org", rexexp, replacement, "start <a href=\"www.url.org\">www.url.org</a>")
  REGEX_REPLACE( "www.url.org", rexexp, replacement, "<a href=\"www.url.org\">www.url.org</a>")
  REGEX_REPLACE( "url.org", rexexp, replacement, "url.org")
  REGEX_REPLACE( "http://www.url.org", rexexp, replacement, "<a href=\"http://www.url.org\">http://www.url.org</a>")
  REGEX_REPLACE( "http://url.org/one/two/three", rexexp, replacement, "<a href=\"http://url.org/one/two/three\">http://url.org/one/two/three</a>")
  REGEX_REPLACE( "More information about our great little big thing here www.our-adress.xy", rexexp, replacement, "More information about our great little big thing here <a href=\"www.our-adress.xy\">www.our-adress.xy</a>")

  int length = 0;
  regexp = "txt$";
  tmp = oyjlRegExpDelimiter( text, regexp, &length );
  if(tmp != text)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlRegExpDelimiter( \"%s\", \"%s\" ) = %s length=%d", text, oyjlNoEmpty(regexp), tmp, length );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlRegExpFind( \"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  }

  // text = "org/domain/eins.lib,org/domain/zwei.txt;org/domain/drei.lib?net/welt/vier.lib:net/welt/vier.txt$/net/welt/fuenf;/net/welt/fuenf";
  list = oyjlStringSplit2( text, "/welt/", oyjlRegExpDelimiter, &list_n, &index, myAllocFunc );

  if( list_n == 5 &&
      (index && list[1] && strcmp(list[1], "vier.lib:net") == 0) &&
      (index && strcmp(list[2], "vier.txt$/net") == 0) &&
      (index && text[index[2]] == '/') &&
      (index && text[index[2]] == '/')
    )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringSplit2(oyjlRegExpDelimiter)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringSplit2(oyjlRegExpDelimiter)" );
  }
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || verbose)
  {
    fprintf( zout, " text: \"%s\"\n", text );
    for(i = 0; i < list_n; ++i)
      fprintf(zout, " list[%d] \"%s\" index in text of delimiter: %d\n", i, list[i], index ? index[i] : -1 );
  }
  oyjlStringListRelease( &list, list_n, myDeAllocFunc );
  if(index) { myDeAllocFunc( index ); index = NULL; }

  return result;
}

oyjlTESTRESULT_e   testCode          ( oyjl_val            json,
                                       const char        * prog,
                                       size_t              code_size,
                                       size_t              help_size,
                                       size_t              man_size,
                                       size_t              markdown_size,
                                       size_t              json_size,
                                       size_t              json_command_size,
                                       size_t              export_size,
                                       size_t              bash_size,
                                       oyjlTESTRESULT_e    result,
                                       oyjlTESTRESULT_e    fail )
#ifdef TEST_CORE_SKIP_MAIN
;
#else
{
  char * c_source = oyjlUiExportToCode( json, OYJL_SOURCE_CODE_C );
  size_t len = c_source ? strlen(c_source) : 0;
  char * name = NULL;
  char info[48];
  const char * lib_so = "libOyjl.so";
  int lib_so_size = oyjlIsFile( lib_so, "r", OYJL_NO_CHECK, info, 48 );
  const char * lib_a = "liboyjl-static.a";
  int lib_a_size = oyjlIsFile( lib_a, "r", OYJL_NO_CHECK, info, 48 ),
      size;
  char * command = NULL;
  char * t = NULL;

  fprintf( zout, "compiling and testing: %s\n", oyjlTermColor(oyjlBOLD, prog) );

  if(c_source && len == code_size)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "oyjlUiExportToCode(OYJL_SOURCE_CODE_C)" );
  } else
  { PRINT_SUB_INT( fail, len,
    "oyjlUiExportToCode(OYJL_SOURCE_CODE_C)" );
  }
  OYJL_TEST_WRITE_RESULT( c_source, len, "oyjlUiExportToCode", "txt" )
  if(oy_test_last_result != oyjlTESTRESULT_SUCCESS || verbose)
    fprintf( zout, "%s\n", c_source );
  oyjlStringAdd( &name, 0,0, "%s.c", prog );
  oyjlWriteFile( name, c_source, len );
  if(c_source) {free(c_source);} c_source = NULL;
  /* compile */
  if(lib_a_size)
    oyjlStringAdd( &command, 0,0, "c++ %s -g -O0 -I %s -I %s %s -L %s/oyjl-args-qml -loyjl-args-qml-static -lQt5DBus -lQt5Qml -lQt5Network -lQt5Widgets -lQt5Gui -lQt5Core -L %s -L %s -loyjl-args-cli-static -loyjl-args-web-static-client -lmicrohttpd -loyjl-static -loyjl-core-static -lyaml -lyajl -lxml2 -o %s", verbose?"-Wall -Wextra":"-Wno-write-strings", OYJL_SOURCEDIR, OYJL_BUILDDIR, name, OYJL_BUILDDIR, OYJL_BUILDDIR, OYJL_INSTALL_LIBDIR, prog );
  else if(lib_so_size)
    oyjlStringAdd( &command, 0,0, "cc %s -g -O0 -I %s -I %s %s -L %s -lOyjl -lOyjlCore -o %s", verbose?"-Wall -Wextra":"", OYJL_SOURCEDIR, OYJL_BUILDDIR, name, OYJL_BUILDDIR, prog );
  if(t) { free(t); t = NULL; }
  if(command)
  {
    int r;
    if(verbose)
      fprintf( stderr, "compiling: %s\n", oyjlTermColor( oyjlBOLD, command ) );
    r = system(command);
    int size = oyjlIsFile( prog, "r", OYJL_NO_CHECK, info, 48 );
    if(!size || verbose)
    {
      fprintf(stderr, "%s\n", command );
      fprintf(stderr, "%scompile: %s %s %d returned: %d\n", size == 0?"Could not ":"", oyjlTermColor(oyjlBOLD,prog), info, size, r);
    }
    if(command) {free(command); command = NULL;}
  }
  if(name) {free(name);} name = NULL;

  c_source = oyjlUiExportToCode( json, OYJL_COMPLETION_BASH );
  len = c_source ? strlen(c_source) : 0;
  if(len == bash_size)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "oyjlUiExportToCode(OYJL_COMPLETION_BASH)" );
  } else
  { PRINT_SUB_INT( fail, len,
    "oyjlUiExportToCode(OYJL_COMPLETION_BASH)" );
  }
  OYJL_TEST_WRITE_RESULT( c_source, strlen(c_source), "oyjlUiExportToCode-Completion", "txt" )
  if(verbose && c_source)
    fprintf( zout, "%s\n", c_source );
  if(c_source) {free(c_source);} c_source = NULL;

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s --help", prog );
  const char * plain = oyjlTermColorToPlain(t, 0);
  len = plain ? strlen(plain) : 0;
  if(len == help_size)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "%s --help", prog );
  } else
  { PRINT_SUB_INT( fail, len,
    "%s --help", prog );
    if(len == 0)
      system("pwd; ls -l");
  }
  OYJL_TEST_WRITE_RESULT( t, size, prog, "txt" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X man > %s.1 && COLUMNS=%d man ./%s.1", prog, prog, 400, prog );
  len = t ? strlen(t) : 0;
  if(len == man_size)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "%s -X man && man %s.1", prog, prog );
  } else
  { PRINT_SUB_INT( fail, len,
    "%s -X man && man %s.1", prog, prog );
  }
  OYJL_TEST_WRITE_RESULT( t, len, prog, "man" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X markdown", prog );
  len = t ? strlen(t) : 0;
  if(len == markdown_size)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "%s -X markdown", prog );
  } else
  { PRINT_SUB_INT( fail, len,
    "%s -X markdown", prog );
  }
  OYJL_TEST_WRITE_RESULT( t, len, prog, "md" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X json | %s/oyjl json -i -", prog, OYJL_BUILDDIR );
  plain = oyjlTermColorToPlain(t, 0);
  len = t ? strlen(plain) : 0;
  if(len == json_size && oyjlDataFormat(plain) == 7)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "%s -X json", prog );
  } else
  { PRINT_SUB( fail,
    "%s -X json                       %lu/%lu %d", prog, json_size, len, oyjlDataFormat(plain) );
  system("ls -l oyjl");
  }
  OYJL_TEST_WRITE_RESULT( t, size, prog, "json" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X json+command | %s/oyjl json -i -", prog, OYJL_BUILDDIR );
  plain = oyjlTermColorToPlain(t, 0);
  len = t ? strlen(plain) : 0;
  if(len == json_command_size && oyjlDataFormat(t) == 7)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "%s -X json+command", prog );
  } else
  { PRINT_SUB_INT( fail, len,
    "%s -X json+command", prog );
  }
  OYJL_TEST_WRITE_RESULT( t, size, prog, "json" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X export | %s/oyjl json -i -", prog, OYJL_BUILDDIR );
  plain = oyjlTermColorToPlain(t, 0);
  len = t ? strlen(plain) : 0;
  if(len == export_size && oyjlDataFormat(t) == 7)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "%s -X export", prog );
  } else
  { PRINT_SUB_INT( fail, len,
    "%s -X export", prog );
  }
  OYJL_TEST_WRITE_RESULT( t, size, prog, "json" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  return result;
}
#endif


oyjlTESTRESULT_e progNAME(testArgs)()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );
  const char * loc = setlocale(LC_ALL,"en_GB.UTF8");
  oyjlLang("en_GB.UTF8");

  int output = 0;
  const char * file = NULL;
  int file_count = 0;
  int show_status = 0;
  int help = 0;
  int verbose_ = 0;
  int version = 0;
  int state = 0;
  int argc = 1;
  const char * argv[] = {"test","-o=0","--input","file-name.json", "-z"};

  /* handle options */
  /* Select from *version*, *manufacturer*, *copyright*, *license*, *url*,
   * *support*, *download*, *sources*, *oyjl_module_author* and
   * *documentation* what you see fit. Add new ones as needed. */
  oyjlUiHeaderSection_s sections[] = {
    /* type, nick,            label, name,                  description  */
    {"oihs", "version",       NULL,  "1.0",                 NULL},
    {"oihs", "documentation", NULL,  "",                    _("The example tool demontrates the usage of the libOyjl API's.")},
    {"oihs", "date",          NULL,  "2018-10-10T12:00:00", _("October 10, 2018")},
    {"",0,0,0,0}};

  /* declare some option choices */
  oyjlOptionChoice_s i_choices[] = {{"oyjl.json", _("oyjl.json"), "First file", ""},
                                    {"oyjl2.json", _("oyjl2.json"), "Second file", ""},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s o_choices[] = {{"0", _("Print All"), _("All Device Classes"), ""},
                                    {"1", _("Print Camera"), _("JSON"), ""},
                                    {"2", _("Print None"), "", ""},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s A_choices[] = {{"",_("Example"),_("prog --opt=arg"), NULL},
                                    {"",_("Example"),_("prog --opt2=arg2"), NULL},
                                    {NULL,NULL,NULL,NULL}};

  /*  declare options - the core information; use previously declared choices;
   *  only declared options are parsed
   */
  oyjlOption_s oarray[] = {
  /* type,   flags, o,   option,    key,  name,         description,         help, value_name,    value_type,               values,                                                          variable_type, output variable */
    {"oiwi", 0,     "#", "",        NULL, _("status"),  _("Show Status"),    NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &show_status}, NULL },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,"@","",NULL,_("input"),_("Set Input"),NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {}, oyjlINT, {.i = &file_count}, NULL },
    {"oiwi", 0,     "i", "input",   NULL, _("input"),   _("Set Input"),      NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)i_choices, sizeof(i_choices), malloc )}, oyjlSTRING, {.s = &file}, NULL },
    {"oiwi", 0,     "o", "output",  NULL, _("output"),  _("Control Output"), NULL, "0|1|2",       oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)o_choices, sizeof(o_choices), malloc )}, oyjlINT, {.i = &output}, NULL },
    /* default options -h, -v and -V */
    {"oiwi", 0,     "h", "help",    NULL, _("help"),    _("Help"),           NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &help}, NULL },
    {"oiwi", 0,     "v", "verbose", NULL, _("verbose"), _("verbose"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &verbose_}, NULL },
    {"oiwi", 0,     "V", "version", NULL, _("version"), _("Version"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &version}, NULL },
    {"oiwi", 0,     "A","man-examples",NULL,_("EXAMPLES"),NULL,              NULL, NULL,
          oyjlOPTIONTYPE_CHOICE,{.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)A_choices, sizeof(A_choices), malloc )}, oyjlNONE, {}, NULL},
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{},0}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups_no_args[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Mode1"),_("Simple mode"),     NULL, "#",       "o,v",    "o", NULL }, /* accepted even if none of the mandatory options is set */
    {"oiwg", OYJL_OPTION_FLAG_EDITABLE,_("Mode2"),_("Any arg mode"),NULL,"@","o,v","@,o", NULL},/* accepted if anonymous arguments are set */
    {"oiwg", 0,     _("Mode3"),_("Actual mode"),     NULL, "i",       "o,v",    "i,o", NULL },/* parsed and checked with -i option */
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "",        "",       "v,h", NULL },/* just show in documentation */
    {"",0,0,0,0,0,0,0,0}
  };

  /* tell about the tool */
  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups_no_args, NULL );
  /* done with options handling */

  if(verbose)
  {
    FILE * f = NULL;
    char * t = oyjlOptions_PrintHelp( ui->opts, ui, 1, &f, "%s v%s - %s", argv[0],
                            "1.0", "Test Tool for testing" );
    fputs( t, f ); free(t); t = NULL;
  }
  if(ui)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui created - no args" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui created - no args" );
  }
  char * text = oyjlUi_ToJson( ui, 0 );
  if(text)
    OYJL_TEST_WRITE_RESULT( text, strlen(text), "UiCreateNoArgs", "txt" )
  if(verbose)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  oyjl_val json = oyjlUi_ExportToJson_( ui, 0/*flags*/ );
  oyjlUi_Release( &ui);

  result = testCode( json, "oiCR"                    /*prog*/,
                           9983                      /*code_size*/,
                           1081                      /*help_size*/,
                           2082                      /*man_size*/,
                           4065                      /*markdown_size*/,
                           7382                      /*json_size*/,
                           7409                      /*json_command_size*/,
                           11725                     /*export_size*/,
                           3163                      /*bash_size*/,
                           result,
                           oyjlTESTRESULT_FAIL       /*fail*/ );

  oyjlTreeFree( json ); json = NULL;


  const char * argv_anonymous[] = {"test","-o=0","file-name.json","file-name2.json"};
  int argc_anonymous = 4;
  ui = oyjlUi_Create( argc_anonymous, argv_anonymous, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups_no_args, NULL );
  if(ui)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui created - anonymous args" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui created - anonymous args" );
  }
  int count = 0, i;
  /* detect all anonymous arguments */
  char ** results = oyjlOptions_ResultsToList( ui?ui->opts:NULL, "@", &count );
  if(count == 2 &&
     file_count == count &&
     strcmp(argv_anonymous[2],results[0]) == 0 &&
     strcmp(argv_anonymous[3],results[1]) == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "anonymous args correctly detected  %d", count );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "anonymous args correctly detected  %d", count );
  }
  if(verbose)
  for(i = 0; i < count; ++i)
    fprintf( zout, "%s\n", results[i] );
  oyjlUi_Release( &ui);
  oyjlStringListRelease( &results, count, 0 );

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Mode"), _("Actual mode"),     NULL, "i",       "o,v",    "i,o", NULL },
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "h",       "",       "h,v", NULL },
    {"",0,0,0,0,0,0,0,0}
  };

  argc = 2;
  ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups, NULL );

  if(!ui)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui not created - missing mandatory option" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui not created - missing mandatory option" );
  }

  oyjlUi_Release( &ui);

  argc = 3;
  ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups, &state );
  if(!ui)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui not created - missing arg %d" , state >> oyjlUI_STATE_OPTION );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui not created - missing arg" );
  }
  oyjlUi_Release( &ui);

  argc = 4;
  ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups, NULL );
  if(ui && strcmp(file,"file-name.json") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui created - parse string" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui created - parse string                      " );
  }
  json = oyjlUi_ExportToJson_( ui, 0/*flags*/ );
  oyjlUi_Release( &ui);

  result = testCode( json, "oiCR"                    /*prog*/,
                           9699                      /*code_size*/,
                            614                      /*help_size*/,
                           1566                      /*man_size*/,
                           2521                      /*markdown_size*/,
                           5119                      /*json_size*/,
                           5146                      /*json_command_size*/,
                           11210                     /*export_size*/,
                           3030                      /*bash_size*/,
                           result,
                           oyjlTESTRESULT_FAIL       /*fail*/ );

  oyjlTreeFree( json ); json = NULL;

  argc = 5;
  ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups, &state );
  if(!ui)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui not created - wrong arg  %d", state >> oyjlUI_STATE_OPTION );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui not created - wrong arg" );
  }
  oyjlUi_Release( &ui);


  argc = 4;
  ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyjl-config-read", "Oyjl Config Reader", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups, NULL );
  text = oyjlUi_ToJson( ui, 0 );
  const char * plain = oyjlTermColorToPlain(text, 0);
  int len = strlen(plain);
  if(text && len == 5312)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "oyjlUi_ToJson()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ToJson() 5312 == %lu", len );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToJson", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  text = oyjlUi_ToMan( ui, 0 );
  len = strlen(text);
  if(text && strlen(text) == 940)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "oyjlUi_ToMan()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ToMan() 940 == %lu", strlen(text) );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToMan", "txt" )
  if(verbose)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  text = oyjlOptions_ResultsToJson( ui->opts, OYJL_JSON );
  plain = oyjlTermColorToPlain(text, 0);
  len = strlen(plain);
  if(text && len == 43)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "oyjlOptions_ResultsToJson()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_ResultsToJson() %lu", len );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlOptions_ResultsToJson", "txt" )
  if(verbose)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;
  setlocale(LC_ALL,"");

  text = oyjlOptions_ResultsToText( ui->opts );
  len = strlen(text);
  if(text && len == 21)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "oyjlOptions_ResultsToText()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_ResultsToText() %lu", strlen(text) );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlOptions_ResultsToText", "txt" )
  if(verbose)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  if(verbose)
  {
    char * t = oyjlOptions_PrintHelp( ui->opts, ui, 1, NULL, "%s v%s - %s", argv[0],
                            "1.0", "Test Tool for testing" );
    fprintf(stdout, "Help text -> stderr:\n%s", t );
    free(t); t = NULL;
  }

  text = oyjlUi_ExportToJson( ui, 0 );
  plain = oyjlTermColorToPlain(text, 0);
  len = strlen(plain);
  if(text && len == 7680)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "oyjlUi_ExportToJson()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ExportToJson()       7680 == %lu", strlen(plain) );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ExportToJson", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) { free(text); text = NULL; }

  oyjl_val root = oyjlUi_ToJson_( ui );
  oyjlUi_Release( &ui);
  char * wrong = "test";
  fprintf(stdout, "oyjlUi_Release(&\"test\") - should give a warning message:\n" );
  oyjlUi_Release( (oyjlUi_s **)&wrong);

  free(oarray[2].values.choices.list);
  free(oarray[3].values.choices.list);
  free(oarray[7].values.choices.list);


  const char * option_string = "v:i=2:o=string";
  oyjl_val defaults = oyjlOptionStringToJson( option_string );
  text = oyjlTreeToText( defaults, OYJL_NO_MARKUP );
  len = strlen(text);
  if( len == 50 )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "oyjlOptionStringToJson(\"%s\")", option_string );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, len,
    "oyjlOptionStringToJson(\"%s\")", option_string );
  }
  if(verbose && text)
    fprintf(zout, "%s\n", text );
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlOptionStringToJson", "txt" )
  free( text ); text = NULL;

  oyjlUiJsonSetDefaults( root, defaults );
  text = oyjlTreeToText( root, OYJL_NO_MARKUP );
  len = strlen(text);
  if( len == 5304 )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "oyjlUiJsonSetDefaults(\"%s\")", option_string );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, len,
    "oyjlUiJsonSetDefaults(\"%s\")", option_string );
  }
  if((verbose || result != oyjlTESTRESULT_SUCCESS) && text)
    fprintf(zout, "%s\n", text );
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlOptionStringToJson", "txt" )
  oyjlTreeFree( root ); root = NULL;
  oyjlTreeFree( defaults ); defaults = NULL;
  free( text ); text = NULL;

  return result;
}


void oyjlTreeToJson2_(oyjl_val v, int * level, char ** json);



oyjlTESTRESULT_e testTreePath        ( oyjl_val            root,
                                       const char        * key,
                                       const char        * name,
                                       oyjlTESTRESULT_e    result,
                                       oyjlTESTRESULT_e    fail )
{
  oyjl_val value = oyjlTreeGetValueF( root, 0, key );
  char * path = oyjlTreeGetPath( root, value );
  if( path && strcmp(path, name) == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeToPath() \"%s\"", path );
  } else
  { PRINT_SUB( fail,
    "oyjlTreeToPath() \"%s\"", path );
  }
  if(path) {free(path); path = NULL;}

  return result;
}

oyjlTESTRESULT_e testTreeToPaths     ( oyjl_val            root,
                                       int                 flags,
                                       int                 count_,
                                       const char        * name,
                                       oyjlTESTRESULT_e    result,
                                       oyjlTESTRESULT_e    fail )
{
  int count, i;
  char ** paths = oyjlTreeToPaths( root, 10000000, NULL, flags, &count );

  if( count == count_ )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
    "oyjlTreeToPaths(\"%s\" %s %s)                  %s", name?name:"NULL", flags&OYJL_KEY ? "k":"", flags&OYJL_PATH ? "p":"", name&&!name[0]?"   ":"" );
  } else
  { PRINT_SUB_INT( fail, count,
    "oyjlTreeToPaths(\"%s\" %s %s)", name?name:"NULL", flags&OYJL_KEY ? "k":"", flags&OYJL_PATH ? "p":"" );
  }
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || oy_test_last_result == oyjlTESTRESULT_XFAIL || verbose)
  {
    char * text = NULL; i = 0;
    oyjlTreeToJson( root, &i, &text ); i = 0;
    fprintf( zout, "%s\n", text );
    free(text);
    for(i = 0; i < count; ++i)
    {
      char * path = paths[i];
      fprintf( zout, "paths[%d]: %s\n", i, path );
    }
  }

  if(paths && count)
    oyjlStringListRelease( &paths, count, free );

  return result;
}

oyjlTESTRESULT_e testTree ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int i;

  double clck = oyjlClock();
  for( i = 0; i < 10000; ++i )
    oyjlPathMatch( "my/path/to/a/longer/and/more/detailed/key", "my/path/to/a/longer/and/more/detailed/key", 0 );
  clck = oyjlClock() - clck;
  if( i == 10000 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"match",
    "oyjlPathMatch( )" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, i,
    "oyjlPathMatch( )" );
  }

  clck = oyjlClock();
  for( i = 0; i < 100000; ++i )
    oyjlPathMatch( "my/path/to/a/longer/and/more/detailed/key", "other/path/to/a/longer/and/more/detailed/key", OYJL_PATH_MATCH_LAST_ITEMS );
  clck = oyjlClock() - clck;
  if( i == 100000 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"match",
    "oyjlPathMatch( OYJL_PATH_MATCH_LAST_ITEMS )" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, i,
    "oyjlPathMatch( OYJL_PATH_MATCH_LAST_ITEMS )" );
  }

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

  char * text = NULL; i = 0;
  root = oyjlTreeNew( NULL );
  oyjlTreeToJson( root, &i, &text ); i = 0;
  if(root && text && (strcmp(text,"null") == 0 || strcmp(text,oyjlTermColor(oyjlUNDERLINE,"null")) == 0 ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeNew( NULL )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeNew( NULL )" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeNew_NULL", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  oyjlTreeFree( root );
  if(text) { free(text); text = NULL; }

  root = oyjlTreeNew( "" );
  oyjlTreeToJson( root, &i, &text ); i = 0;
  if(root && text && (strcmp(text,"null") == 0 || strcmp(text,oyjlTermColor(oyjlUNDERLINE,"null")) == 0 ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeNew( \"\" )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeNew( \"\" )" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeNew_", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  oyjlTreeFree( root );
  if(text) { free(text); text = NULL; }

  root = oyjlTreeNew( "new/tree/key" );
  i = 0;
  oyjlTreeToJson( root, &i, &text ); i = 0;
  size_t len = strlen(text);
  if(root && (len == 56 || len == 88))
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "oyjlTreeNew( \"new/tree/key\" )" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, len,
    "oyjlTreeNew( \"new/tree/key\" )" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeNew_new_tree_key", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) { free(text); text = NULL; }

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
  clck = oyjlClock();
  int n;
  for(n = 0; n < 100000; ++n)
    oyjlValueSetString( value, VALUE );
  clck = oyjlClock() - clck;
  oyjlTreeToJson( root, &i, &text ); i = 0;
  if(len < strlen(text))
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS,n,clck/(double)CLOCKS_PER_SEC,"set",
    "oyjlValueSetString( %s )", VALUE );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(text),
    "oyjlValueSetString( %s )", VALUE );
  }
  if(verbose)
    fprintf( zout, "%s\n", text );
  len = strlen(text);
  myDeAllocFunc( text ); text = NULL;

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

  v = oyjlTreeGetPath( root, value );
  if(v && strcmp(v,"new/tree/key") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeGetPath(value) resolved to \"new/tree/key\"" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeGetPath(value) resolved to \"new/tree/key\"" );
  }
  myDeAllocFunc(v);

  oyjlTreeClearValue( root,"new/tree/key" );
  oyjlTreeToJson( root, &i, &text ); i = 0;
  if(!text || (strcmp(text, "null") == 0 || strcmp(text,oyjlTermColor(oyjlUNDERLINE,"null")) == 0 ))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeClearValue( \"new/tree/key\" ) " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeClearValue( \"new/tree/key\" ) " );
  }
  oyjlTreeFree( root );
  myDeAllocFunc( text ); text = NULL;

  root = oyjlTreeNew( NULL );
  value = oyjlTreeGetValue( root, OYJL_CREATE_NEW, "arr/[3]" );
  value = oyjlTreeGetValue( root, 0, "arr" );
  count = oyjlValueCount( value );
  text = oyjlTreePrint(root);
  len = strlen(text);
  if(count == 4 && (len == 34 || len == 74))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeGetValue(OYJL_CREATE_NEW, \"arr/[3]\") oyjlValueCount() %i", count );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeGetValue(OYJL_CREATE_NEW, \"arr/[3]\") oyjlValueCount() %i %i", count, len );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeGetValue-array-4", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "value", "arr/[1]/key" );
  text = oyjlTreePrint(root);
  len = strlen(text);
  if(len == 58 || len == 106)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeSetStringF(OYJL_CREATE_NEW,\"arr/[1]/key\") %lu", strlen(text) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeSetStringF(OYJL_CREATE_NEW, \"arr/[1]/key\") %lu", strlen(text) );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeGetValue-array-4", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;
  oyjlTreeFree( root );

  root = oyjlTreeNew( "one" );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "value", "one/[%d]/key1", 0 );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "arr1a", "one/[%d]/data/[0]", 0 );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "arr1b", "one/[%d]/data/[1]", 0 );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "value", "two/[%d]/key2", 0 );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "arr2a", "two/[%d]/data/[0]", 0 );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "arr2b", "two/[%d]/data/[1]", 0 );
  const char * txt = "value\nafter_line_break";
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, txt, "one/[%d]/key3", 1 );
  if(verbose)
    fprintf( zout, "oyjlTreeSetStringF( value: %s )\n", txt );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "%33[1marr3a%33[0m", "one/[%d]/data/[0]", 1 );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "\033[1marr3b\033[0m", "one/[%d]/data/[1]", 1 );
  oyjlTreeSetDoubleF( root, OYJL_CREATE_NEW, 1.4,     "one/[%d]/key4", 1 );
  oyjlTreeSetIntF( root, OYJL_CREATE_NEW, 0x800000,   "one/[%d]/data/[2]", 1 );
  oyjlTreeToJson( root, &i, &text ); i = 0;
  len = strlen(text);
  if( text && (len == 284 || len == 470) )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(text),
    "add array" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, text?strlen(text):0,
    "add array" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeSetStringF", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  myDeAllocFunc( text ); text = NULL;

  int size = 0;
  int flags = verbose ? OYJL_OBSERVE : 0;
  oyjl_val catalog = oyjlTreeNew( "" );
  oyjlTreeSetStringF( catalog, OYJL_CREATE_NEW, "Beispiel", "org/freedesktop/oyjl/translations/de_DE.UTF8/Example" );
  oyjlTreeSetStringF( catalog, OYJL_CREATE_NEW, "Beispiel2", "org/freedesktop/oyjl/translations/de_DE/Example2" );
  oyjlTreeSetStringF( catalog, OYJL_CREATE_NEW, "Schmarrn", "org/freedesktop/oyjl/translations/de_AT/Nonsense" );
  oyjlTreeSetStringF( catalog, OYJL_CREATE_NEW, "Farbe", "org/freedesktop/oyjl/translations/de/Color" );
  oyjlTreeSetStringF( catalog, OYJL_CREATE_NEW, "2. Farbe", "org/freedesktop/oyjl/translations/de/2. Color" );
  oyjlTreeSetStringF( catalog, OYJL_CREATE_NEW, "Farbe [3]", "org/freedesktop/oyjl/translations/de/Color\\[3]" );
  oyjlTreeSetStringF( catalog, OYJL_CREATE_NEW, "Farbe \"Rosa\"", "org/freedesktop/oyjl/translations/de/Color \"Rose\"" );
  oyjlTreeSetStringF( catalog, OYJL_CREATE_NEW, "Benutze [A-Z,a-z] Understrich '_'.", "org/freedesktop/oyjl/translations/de/Use \\[A-Z,a-z] underscore '_'." );
  oyjlTreeSetStringF( catalog, OYJL_CREATE_NEW, "Modus(1)", "org/freedesktop/oyjl/translations/de/Mode(1)" );
  oyjlTreeSetStringF( catalog, OYJL_CREATE_NEW, "prog -i test.json ///mein/Pfad/", "org/freedesktop/oyjl/translations/de/prog -i test.json %%37%%37%%37my%%37path%%37" );

  oyjl_val static_catalog = oyjlTreeSerialise( catalog, flags, &size );
  if( static_catalog && size == 944 )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, size,
    "oyjlTreeSerialise() oiJS" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, size,
    "oyjlTreeSerialise() oiJS" );
  }
  OYJL_TEST_WRITE_RESULT( static_catalog, size, "oyjlTreeSerialise", "oiJS" )
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || oy_test_last_result == oyjlTESTRESULT_XFAIL || verbose)
  {
char *     oyjlTreeSerialisedPrint_  ( oyjl_val            v,
                                       int                 flags OYJL_UNUSED );
    char * t = oyjlTreeSerialisedPrint_( static_catalog, 0 );
    fprintf( zout, "parsed: %s\n", t );
    free(t); t = NULL;
    i = 0;
    oyjlTreeToJson( catalog, &i, &t ); i = 0;
    fprintf( zout, "%s\n", t );
    free(t); t = NULL;
  }

  oyjlTreeFree( static_catalog );
  oyjlTreeFree( catalog );
  oyjlTreeFree( root );

  clck = oyjlClock();
  root = oyjlTreeNew("");
  n = 2000;
  for(i = 0; i < n; ++i)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "value", "data/key-%d", i );
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling( -1, n, clck/(double)CLOCKS_PER_SEC,"node",
    "oyjlTreeSetStringF()  %dx", n) );
  i = 0;

  clck = oyjlClock();
  oyjlTreeToJson2_( root, &i, &text ); i = 0;
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling( -1, 1, clck/(double)CLOCKS_PER_SEC,"dump",
    "oyjlTreeToJson2_()                  1x %d", (int)strlen(text)) );
  myDeAllocFunc( text ); text = NULL;

  clck = oyjlClock();
  oyjlTreeToJson( root, &i, &text ); i = 0;
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling( -1, 1, clck/(double)CLOCKS_PER_SEC,"dump",
    "oyjlTreeToJson()                    1x %d", (int)strlen(text)) );

  clck = oyjlClock();
  txt = oyjlTermColorToPlain( text, OYJL_REGEXP );
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling( -1, 1, clck/(double)CLOCKS_PER_SEC,"dump",
    "oyjlTermColorToPlain( OYJL_REGEXP ) 1x %d -> %d", (int)strlen(text), (int)strlen(txt)) );

  clck = oyjlClock();
  txt = oyjlTermColorToPlain( text, 0 );
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling( -1, 1, clck/(double)CLOCKS_PER_SEC,"dump",
    "oyjlTermColorToPlain( 0 )           1x %d -> %d", (int)strlen(text), (int)strlen(txt)) );
  myDeAllocFunc( text ); text = NULL;

  clck = oyjlClock();
  text = oyjlTreeToText( root, OYJL_NO_MARKUP );
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling( -1, 1, clck/(double)CLOCKS_PER_SEC,"dump",
    "oyjlTreeToText( OYJL_NO_MARKUP )    1x %d", (int)strlen(text)) );
  myDeAllocFunc( text ); text = NULL;

  clck = oyjlClock();
  text = oyjlTreeToText( root, OYJL_JSON );
  clck = oyjlClock() - clck;
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling( -1, 1, clck/(double)CLOCKS_PER_SEC,"dump",
    "oyjlTreeToText( OYJL_JSON )         1x %d", (int)strlen(text)) );
  myDeAllocFunc( text ); text = NULL;
  oyjlTreeFree( root );


#if 1
  root = oyjlTreeNew("");
  const char * key = "Color";
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "value", "data/key-%s", key );
  value = oyjlTreeGetValueF( root, 0, "data/key-%s", key );
  flags = 0;
  char ** paths = oyjlTreeToPaths( root, 10000000, NULL, flags, &count );
  char * path = oyjlTreeGetPath( root, value );
  oyjlTreeFree( root );
  if( count == 2 && path)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
    "oyjlTreeToPaths(\"%s\")", key );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
    "oyjlTreeToPaths(\"%s\")", key );
  }
  if(path) {free(path);} path = NULL;
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || oy_test_last_result == oyjlTESTRESULT_XFAIL || verbose)
  for(i = 0; i < count; ++i)
  {
    char * path = paths[i];
    fprintf( zout, "paths[%d]: %s\n", i, path );
  }
  if(paths && count)
    oyjlStringListRelease( &paths, count, free );
#endif

  fprintf( zout, "\n       creation key    k(OYJL_KEY) p(OYJL_PATH) count \n" );
  const char * json = NULL;
  root = oyjlTreeNew(json);
  result = testTreeToPaths( root, 0, 0, json, result, oyjlTESTRESULT_FAIL );
  oyjlTreeFree( root );

  json = "";
  root = oyjlTreeNew(json);
  result = testTreeToPaths( root, 0, 0, json, result, oyjlTESTRESULT_FAIL );
  oyjlTreeFree( root );

  json = "data";
  root = oyjlTreeNew(json);
  result = testTreeToPaths( root, 0, 1, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_KEY|OYJL_PATH, 1, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_KEY, 1, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_PATH, 0, json, result, oyjlTESTRESULT_FAIL );
  oyjlTreeFree( root );

  json = "d/key";
  root = oyjlTreeNew(json);
  result = testTreeToPaths( root, 0, 2, json, result, oyjlTESTRESULT_FAIL );
  oyjlTreeFree( root );

  json = "d/e/k";
  root = oyjlTreeNew(json);
  result = testTreeToPaths( root, 0, 3, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_KEY|OYJL_PATH, 3, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_KEY, 1, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_PATH, 2, json, result, oyjlTESTRESULT_FAIL );
  oyjlTreeFree( root );

  json = "d/1. key";
  root = oyjlTreeNew(json);
  result = testTreeToPaths( root, 0, 2, json, result, oyjlTESTRESULT_XFAIL );
  result = testTreeToPaths( root, OYJL_KEY|OYJL_PATH, 2, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_KEY, 1, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_PATH, 1, json, result, oyjlTESTRESULT_FAIL );
  oyjlTreeFree( root );

  json = "d/k\\\\[3]";
  root = oyjlTreeNew(json);
  result = testTreeToPaths( root, 0, 2, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_KEY|OYJL_PATH, 2, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_KEY, 1, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_PATH, 1, json, result, oyjlTESTRESULT_FAIL );
  result = testTreePath( root, json, json, result, oyjlTESTRESULT_FAIL );
  oyjlTreeFree( root );

  json = "d/[3]/k";
  root = oyjlTreeNew(json);
  result = testTreeToPaths( root, 0, 6, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_KEY|OYJL_PATH, 6, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_KEY, 1, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_PATH, 5, json, result, oyjlTESTRESULT_FAIL );
  oyjlTreeFree( root );

  json = "d/[3]";
  root = oyjlTreeNew(json);
  result = testTreeToPaths( root, 0, 5, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_KEY|OYJL_PATH, 5, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_KEY, 0, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_PATH, 5, json, result, oyjlTESTRESULT_FAIL );
  oyjlTreeFree( root );

  json = "d/e.f/k";
  root = oyjlTreeNew(json);
  result = testTreeToPaths( root, 0, 3, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_KEY|OYJL_PATH, 3, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_KEY, 1, json, result, oyjlTESTRESULT_FAIL );
  result = testTreeToPaths( root, OYJL_PATH, 2, json, result, oyjlTESTRESULT_FAIL );
  oyjlTreeFree( root );

  return result;
}

oyjlTESTRESULT_e testIO ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int len = 48;
  int size = oyjlWriteFile( "test.txt", "test", 5 );
  char info[len];
  int r = oyjlIsFile( "test.txt", "r", OYJL_NO_CHECK, info, len );
  int size2 = oyjlWriteFile( "test2.txt", "test2", 6 );
  char info2[len];
  int r2 = oyjlIsFile( "test2.txt", "r", OYJL_NO_CHECK, info2, len );

  if(r && r2 && size == 5 && size2 == 6)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlWriteFile()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlWriteFile()" );
  }

  if(strlen(info) && strlen(info2))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlIsFile()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlIsFile()" );
  }

  char * text = oyjlReadFile( "test2.txt", 0, &size );
  if(text && strcmp(text, "test2") == 0 && size == 6)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlReadFile()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlReadFile()" );
  }
  if(text) { free(text); text = NULL; }

  text = oyjlReadCommandF( &size, "r", malloc, "echo ABC" );
  if(text && strcmp(text, "ABC\n") == 0 && size == 4)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlReadCommandF()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlReadCommandF() %s %d", text, size );
  }
  if(text) { free(text); text = NULL; }

  oyjlTermColorInit( OYJL_RESET_COLORTERM | OYJL_FORCE_COLORTERM );
  const char * t = oyjlTermColor( oyjlBOLD, "test" );
  len = strlen(t);
  if(t && len == 12)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlColorTerm() = %s %d", t, len );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlColorTerm() = %s %d", t, len );
  }
  fputs(t,stderr);
  fputs(" plain text",stderr);
  fputs("\n",stderr);
  oyjlStringAdd( &text, 0,0, "%s", t );
  fputs(text,stderr);
  fputs(" plain text",stderr);
  fputs("\n",stderr);
  if(text) { free(text); text = NULL; }
  size = oyjlWriteFile( "test.txt", t, strlen(t) );
  fprintf( zout, "oyjlWriteFile(%s) = %d\n", t, size );
  text = oyjlReadFile( "test.txt", 0, &size );
  fprintf( zout, "oyjlReadFile() = \"%s\" %d\n", text, size );
  if(text) { free(text); text = NULL; }

  t = "../";
  if(oyjlIsDir(t))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlIsDir(%s)", t );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlIsDir(%s)", t );
  }

  t = "~";
  text = oyjlResolveDirFile( t );
  if(oyjlIsDir(text))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlResolveDirFile(%s) = %s", t, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlResolveDirFile(%s) = %s", t, text );
  }
  free(text);

  t = "~/";
  text = oyjlResolveDirFile( t );
  if(oyjlIsDir(text))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlResolveDirFile(%s) = %s", t, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlResolveDirFile(%s) = %s", t, text );
  }
  free(text);

  t = "~/Downloads";
  text = oyjlResolveDirFile( t );
  if(oyjlIsDir(text))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlResolveDirFile(%s) = %s", t, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, /* depends on a DE, fails on servers or dockers */
    "oyjlResolveDirFile(%s) = %s", t, text );
  }
  free(text);

  t = ".";
  text = oyjlResolveDirFile( t );
  if(oyjlIsDir(text))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlResolveDirFile(%s) = %s", t, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlResolveDirFile(%s) = %s", t, text );
  }
  free(text);

  t = "./";
  text = oyjlResolveDirFile( t );
  if(oyjlIsDir(text))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlResolveDirFile(%s) = %s", t, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlResolveDirFile(%s) = %s", t, text );
  }
  free(text);

  t = "../docs";
  text = oyjlResolveDirFile( t );
  if(oyjlIsDir(text))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlResolveDirFile(%s) = %s", t, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, /* depends on build path */
    "oyjlResolveDirFile(%s) = %s", t, text );
  }
  free(text);

  t = "../docs/";
  text = oyjlResolveDirFile( t );
  if(oyjlIsDir(text))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlResolveDirFile(%s) = %s", t, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, /* depends on build path */
    "oyjlResolveDirFile(%s) = %s", t, text );
  }
  free(text);

  if( verbose )
    fprintf( zout, "info: %s %s\n", info, info2 );

  return result;
}

#include "oyjl_macros.h" /* OYJL_DBG_FORMAT */

oyjlTESTRESULT_e testFunction ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  char * bt = oyjlBT(0);

  if(bt)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlBT() = %s", bt );
    free(bt); bt = NULL;
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlBT() = %s", bt );
  }

  const char * html = "<strong>Bold</strong> Normal <em>italic</em>";
  const char * ansi = oyjlTermColorFromHtml( html, 0 );
  if(ansi && strlen(ansi) != strlen(html))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTermColorFromHtml() = %s", ansi );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTermColorFromHtml() %d / %d", strlen(ansi), strlen(html) );
  }


  ansi = "\033[1mbold\033[0m \033[3mitalic\033[0m \033[4munderline\033[0m \033[0;31mred\033[0m \033[0;32mgreen\033[0m \033[0;34mblue\033[0m";
  oyjl_str string = oyjlStr_New(10, 0,0);
  int n = 1000000, i;
  for(i = 0; i < n; ++i)
    oyjlStr_AppendN( string, ansi, 9 );
  const char * str = oyjlStr(string);
  int len = strlen(str);
  double clck = oyjlClock();
  html = oyjlTermColorToHtml( str, 0 );
  clck = oyjlClock() - clck;
  char * text = malloc(20);
  snprintf( text, 20, "%s", html );
  if(html && len != (int)strlen(html))
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS,1,clck/(double)CLOCKS_PER_SEC,"ops",
    "oyjlTermColorToHtml(%s) len: %d...", ansi, len );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTermColorToHtml() %d / %d", strlen(ansi), strlen(html) );
  }
  free(text);
  oyjlStr_Clear( string );
  oyjlStr_Release( &string );

  int          oyjlTermColor256GetIndex( const char        * term_color );
  const char * ansi_256_color = "\033[38;5;28mcolor\033[0m\033[38;5;284mnope\033[0m\033[1;38;5;217mred\033[0m";
  int index = oyjlTermColor256GetIndex( ansi_256_color );
  if(index == 28)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTermColor256GetIndex(%s) = %d", ansi_256_color, index );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTermColor256GetIndex(%s) = %d", ansi_256_color, index );
  }

  html = oyjlTermColorToHtml( ansi_256_color, 0 );
  if(html && strstr(html, ">color<") != 0 && strstr(html, ">nope<") != 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTermColorToHtml(%s) = %s ...", ansi_256_color, html );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTermColorToHtml() %d / %d", strlen(ansi_256_color), strlen(html) );
  }

  oyjlMessage_p( oyjlMSG_INFO, NULL, OYJL_DBG_FORMAT "Test Info message", OYJL_DBG_ARGS );
  oyjlMessage_p( oyjlMSG_CLIENT_CANCELED, NULL, OYJL_DBG_FORMAT "Test ClientCanceled message", OYJL_DBG_ARGS );
  oyjlMessage_p( oyjlMSG_INSUFFICIENT_DATA, NULL, OYJL_DBG_FORMAT "Test InsufficientData message", OYJL_DBG_ARGS );
  oyjlMessage_p( oyjlMSG_ERROR, NULL, OYJL_DBG_FORMAT "Test Error message", OYJL_DBG_ARGS );
  oyjlMessage_p( oyjlMSG_PROGRAM_ERROR, NULL, OYJL_DBG_FORMAT "Test ProgramError message", OYJL_DBG_ARGS );
  oyjlMessage_p( oyjlMSG_SECURITY_ALERT, NULL, OYJL_DBG_FORMAT "Test SecurityAlert message", OYJL_DBG_ARGS );

  return result;
}

/* --- end actual tests --- */


