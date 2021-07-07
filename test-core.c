/** @file test.c
 *
 *  Oyranos is an open source Color Management System
 *
 *  Copyright (C) 2004-2019  Kai-Uwe Behrmann
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
  TEST_RUN( testString, "String handling", 1 ); \
  TEST_RUN( testArgs, "Options handling", 1 ); \
  TEST_RUN( testTree, "Tree handling", 1 ); \
  TEST_RUN( testIO, "File handling", 1 );

void oyjlLibRelease();
#define OYJL_TEST_MAIN_SETUP  printf("\n    OyjlCore Test Program\n");
#define OYJL_TEST_MAIN_FINISH printf("\n    OyjlCore Test Program finished\n\n"); oyjlLibRelease();
#define OYJL_TEST_NAME "test-core"
#include "oyjl_test_main.h"
#include "oyjl.h"
#include "oyjl_version.h"
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>
#endif
#include "oyjl_i18n.h"
#include "oyjl_internal.h"
#include "oyjl_tree_internal.h"

#define oyjlNoEmpty(x) ((x)?(x):"---")
char *     oyjlTreePrint             ( oyjl_val            v );

/* --- actual tests --- */

oyjlTESTRESULT_e testVersion()
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

  clang = setlocale(LC_ALL,"");

  if(clang && (strstr(clang, "de_DE") != 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "Language initialised good %s            ", clang?clang:"---" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "Language initialised failed %s          ", clang?clang:"---" );
  }

  char * language = oyjlLanguage( clang );
  if(language && (strstr(language, "de") != 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlLanguage() good %s                             ", language?language:"---" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlLanguage() good %s                             ", language?language:"---" );
  }
  if(language) free(language);

  char * country = oyjlCountry( clang );
  if(country && (strstr(country, "DE") != 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlCountry() good %s                              ", country?country:"---" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlCountry() good %s                              ", country?country:"---" );
  }
  if(country) free(country);

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

static void replaceCb(const char * text OYJL_UNUSED, const char * start, const char * end, const char * search, const char ** replace, void * data)
{
  if(start < end)
  {
    const char * word = start;
    int * test = (int*) data;
    while(word && (word = strstr(word+1,"nd")) != NULL && word < end)
      ++test[0];
    word = start;
    while(word && (word = strstr(word+1,"or")) != NULL && word < end)
      --test[0];
    if(test[0] < 0) test[0] = 0;
    word = start;

    if( test[0] )
      *replace = search;
    else
      *replace = "\\/";
  }
}

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
    "oyjlStringReplace(start) %d 59 == %d                 ", n, (int)strlen(test_out) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(start) %d 59 == %d                 ", n, (int)strlen(test_out) );
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
    "oyjlStringReplace(middle) 51 == %d                  ", (int)strlen(test_out) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(middle) 51 == %d                  ", (int)strlen(test_out) );
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
    "oyjlStringReplace(end)    46 == %d                  ", (int)strlen(test_out) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(end)    46 == %d                  ", (int)strlen(test_out) );
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
    "oyjlStringReplace(end)      6 == %d                  ", (int)strlen(test_out) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(end)      6 == %d                  ", (int)strlen(test_out) );
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
    "oyjlStringReplace(none)                             " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(none)                             " );
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
    "oyjlStringReplace(shorter) 49 == %d                 ", (int)strlen(test_out) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(shorter) 49 == %d                 ", (int)strlen(test_out) );
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
    "oyjlStringReplace(longer) 61 == %d                  ", (int)strlen(test_out) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringReplace(longer) 61 == %d                  ", (int)strlen(test_out) );
  }
  myDeAllocFunc(test_out);

  int list_n = 0;
  int *index = NULL;
  const char * text = "org/domain/eins.lib,org/domain/zwei.txt;org/domain/drei.lib?net/welt/vier.lib:net/welt/vier.txt$/net/welt/fuenf;/net/welt/fuenf";
  char ** list = oyjlStringSplit2( text, ";:,?$&§", &list_n, &index, myAllocFunc );

  if( list_n == 7 &&
      (index && text[index[0]] == ',') &&
      (index && text[index[1]] == ';') &&
      (index && text[index[2]] == '?') &&
      (index && text[index[3]] == ':') &&
      (index && text[index[4]] == '$') &&
      (index && text[index[5]] == ';')
    )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringSplit2()                                  " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringSplit2()                                  " );
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
    "oyjlStringListFreeDoubles()                         " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringListFreeDoubles()                         " );
    for(i = 0; i < list_n; ++i)
      fprintf(zout, " list[%d] \"%s\"\n", i, list[i] );
  }
  oyjlStringListRelease( &list, list_n, myDeAllocFunc );

  list_n = 0;
  list = oyjlStringSplit( "a a b c\td\ne", 0, &list_n, myAllocFunc );
  if(list_n == 6)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringSplit(0)                                  " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringSplit(0)                                  " );
  }
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || verbose)
  {
    for(i = 0; i < list_n; ++i)
      fprintf(zout, " list[%d] \"%s\"\n", i, list[i] );
  }
  oyjlStringListRelease( &list, list_n, myDeAllocFunc );

  test = "element";
  const char * slist[] = { test, NULL };
  list_n = 0;
  oyjlStringListAddList( &list, &list_n, slist, 1, 0, 0 );
  oyjlStringListAddList( &list, &list_n, slist, 1, 0, 0 );
  char * t = NULL;
  for(i = 0; i < list_n; ++i)
    oyjlStringAdd( &t, 0,0, "%s%s", i?",":"", list[i] );
  if(strcmp(t, "element,element") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringListAddList(list, &string)                " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringListAddList(list, &string)                " );
  }
  free(t); t = NULL;
  oyjlStringListRelease( &list, list_n, 0 );
  list_n = 0;


  long l = 0;
  int error = oyjlStringToLong( "2", &l );
  if( !error &&
      l == 2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringToLong(\"2\") = %ld                          ", l );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringToLong(\"2\") = %ld                          ", l );
  }

  l = 0;
  error = oyjlStringToLong( "2\n", &l );
  if( !error &&
      l == 2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringToLong(\"2\n\") = %ld                         ", l );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringToLong(\"2\n\") = %ld                         ", l );
  }

  double d = 0.0;
  error = oyjlStringToDouble( "0.2", &d );
  if( !error &&
      d == 0.2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringToDouble(\"0.2\") = %g                     ", d );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringToDouble(\"0.2\") = %g  error = %d           ", d, error );
  }

  d = 0.0;
  error = oyjlStringToDouble( "0.2#", &d );
  if( !error &&
      d == 0.2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringToDouble(\"0.2#\") = %g                    ", d );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringToDouble(\"0.2#\") = %g  error = %d          ", d, error );
  }

  d = 0.0;
  error = oyjlStringToDouble( "val_a", &d );
  if( error )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringToDouble(\"val_a\")   error: %d              ", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringToDouble(\"val_a\") = %g  error = %d         ", d, error );
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
    "oyjlStringsToDoubles(\"0.2 1 3.5\") error = %d         ", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringsToDoubles(\"0.2 1 3.5\") error = %d         ", error );
  }
  if(doubles) { free(doubles); } doubles = NULL;

  error = oyjlStringsToDoubles( "0.2", " ", &count, malloc, &doubles );
  if( !error &&
      doubles[0] == 0.2 &&
      count == 1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringsToDoubles(\"0.2\") error = %d               ", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringsToDoubles(\"0.2\") error = %d               ", error );
  }
  if(doubles) { free(doubles); } doubles = NULL;

  error = oyjlStringsToDoubles( "0.2;1;3.5", ";", &count, malloc, &doubles );
  if( !error &&
      doubles[0] == 0.2 &&
      doubles[1] == 1.0 &&
      doubles[2] == 3.5 &&
      count == 3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringsToDoubles(\"0.2;1;3.5\") error = %d         ", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringsToDoubles(\"0.2;1;3.5\") error = %d         ", error );
  }
  if(doubles) { free(doubles); } doubles = NULL;

  error = oyjlStringsToDoubles( "0.2 ; 1; 3.5 cm", ";", &count, malloc, &doubles );
  if( doubles[0] == 0.2 &&
      doubles[1] == 1.0 &&
      doubles[2] == 3.5 &&
      count == 3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringsToDoubles(\"0.2 ; 1; 3.5 cm\") error = %d  ", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringsToDoubles(\"0.2 ; 1; 3.5 cm\") error = %d  ", error );
  }
  if(doubles) { free(doubles); } doubles = NULL;

  error = oyjlStringsToDoubles( "0.2,1,3.5", ",", &count, malloc, &doubles );
  if( !error &&
      doubles[0] == 0.2 &&
      doubles[1] == 1.0 &&
      doubles[2] == 3.5 &&
      count == 3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringsToDoubles(\"0.2,1,3.5\") error = %d         ", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringsToDoubles(\"0.2,1,3.5\") error = %d         ", error );
  }
  if(doubles) { free(doubles); } doubles = NULL;

  error = oyjlStringsToDoubles( "2\t1.2\t 27", 0, &count, malloc, &doubles );
  if( error == 0 &&
      doubles[0] == 2 &&
      doubles[1] == 1.2 &&
      doubles[2] == 27 &&
      count == 3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringsToDoubles(\"2\\t1.2\\t 27\") error = %d       ", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringsToDoubles(\"2\\t1.2\\t 27\") error = %d       ", error );
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

  double clck = oyjlClock();
  n = 10000;
  for(i = 0; i < n; ++i)
    oyjlStringAdd( &t, 0,0, "/%s/%s", "more", "and" );
  clck = oyjlClock() - clck;
  fprintf( zout, "oyjlStringAdd()\t%dx9  %d               \t\%s\n", n, (int)strlen(t),
                 oyjlProfilingToString(n,clck/(double)CLOCKS_PER_SEC,"ops"));
  if(t) { free(t); t = NULL; }

  clck = oyjlClock();
  for(i = 0; i < n; ++i)
    oyjlStringAddN( &t, "/more/and", 9, malloc,free );
  clck = oyjlClock() - clck;
  fprintf( zout, "oyjlStringAddN()\t%dx9  %d       \t\%s\n", n, (int)strlen(t),
                 oyjlProfilingToString(n,clck/(double)CLOCKS_PER_SEC,"ops"));

  n = 1000;
  clck = oyjlClock();
  int len OYJL_UNUSED;
  for(i = 0; i < n; ++i)
    len = strlen( t );
  clck = oyjlClock() - clck;
  fprintf( zout, "strlen()\t%dx  %d             \t\%s\n", n, (int)strlen(t),
                 oyjlProfilingToString(n,clck/(double)CLOCKS_PER_SEC,"ops"));

  if(t) { free(t); t = NULL; }

  n = 1000000;
  t = calloc(10*n,sizeof(char));
  clck = oyjlClock();
  for(i = 0; i < n; ++i)
    memcpy( &t[9*i], "/more/and", 9 );
  clck = oyjlClock() - clck;
  fprintf( zout, "memcpy()\t%dx9  %d           \t\%s\n", n, (int)strlen(t),
                 oyjlProfilingToString(n,clck/(double)CLOCKS_PER_SEC,"ops"));
  if(t) { free(t); t = NULL; }

  oyjl_str string = oyjlStrNew(10, 0,0);
  clck = oyjlClock();
  n = 1000000;
  for(i = 0; i < n; ++i)
    oyjlStrAppendN( string, "/more/and", 9 );
  clck = oyjlClock() - clck;
  fprintf( zout, "oyjlStrAppendN()\t%dx9  %d    \t\%s\n", n, (int)strlen(oyjlStr(string)),
                 oyjlProfilingToString(n,clck/(double)CLOCKS_PER_SEC,"ops"));
  oyjlStrRelease( &string );

  int inside = 0;
  string = oyjlStrNew(10, 0,0);
  for(i = 0; i < 10; ++i)
    oyjlStrAppendN( string, "/more/and", 9 );
  oyjlStrReplace( string, "/", "\\/", replaceCb, &inside );
  const char * tmp = oyjlStr(string);
  if(strstr(tmp, "/more\\/and/more"))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStrReplace(callback,user_data)                  " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStrReplace(callback,user_data)                  " );
  }
  oyjlStrRelease( &string );

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
  if(oyjlRegExpFind(txt, regexp) == NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "!oyjlRegExpFind( \"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "!oyjlRegExpFind( \"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  }
  regexp = "_string_";
  if((match = oyjlRegExpFind(txt, regexp)) != NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlRegExpFind( \"%s\", \"%s\" ) %s", txt, oyjlNoEmpty(regexp), match );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlRegExpFind( \"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  }

  regexp = "NONE";
  if((match = oyjlRegExpFind(txt, regexp)) == NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "!oyjlRegExpFind( \"%s\", \"%s\" ) ", txt, oyjlNoEmpty(regexp) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "!oyjlRegExpFind( \"%s\", \"%s\" ) ", txt, oyjlNoEmpty(regexp) );
  }

  regexp = "1";
  if((match = oyjlRegExpFind(txt, regexp)) != NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlRegExpFind( \"%s\", \"%s\" ) %s", txt, oyjlNoEmpty(regexp), match );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlRegExpFind( \"%s\", \"%s\" ) ", txt, oyjlNoEmpty(regexp) );
  }

  regexp = "string.1";
  if((match = oyjlRegExpFind(txt, regexp)) != NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlRegExpFind( \"%s\", \"%s\" ) %s", txt, oyjlNoEmpty(regexp), match );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlRegExpFind( \"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  }

  regexp = "str.*lon";
  if((match = oyjlRegExpFind(txt, regexp)) != NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlRegExpFind( \"%s\", \"%s\" ) %s", txt, oyjlNoEmpty(regexp), match );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlRegExpFind( \"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  }

  regexp = "st[.]*lo";
  if(oyjlRegExpFind(txt, regexp) == NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "!oyjlRegExpFind(\"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "!oyjlRegExpFind(\"%s\", \"%s\" )", txt, oyjlNoEmpty(regexp) );
  }
  free(txt); txt = NULL;

  char * pattern = NULL;
#define REGEX_ESCAPE( pre, regex_, escape ) \
  { \
    t = oyjlRegExpEscape( escape ); \
    oyjlStringAdd( &compare, 0,0, "%s%s", pre, escape ); \
    oyjlStringAdd( &pattern, 0,0, "%s%s", regex_, t ); \
    if(oyjlRegExpFind( compare, pattern ) != NULL) \
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS, \
      "oyjlRegExpEscape( \"%s\" )                 ", escape ); \
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
{
  char * c_source = oyjlUiJsonToCode( json, OYJL_SOURCE_CODE_C );
  size_t len = c_source ? strlen(c_source) : 0;
  char * name = NULL;
  char info[48];
  const char * lib_so = "libOyjl.so";
  int lib_so_size = oyjlIsFile( lib_so, "r", info, 48 );
  const char * lib_a = "liboyjl-static.a";
  int lib_a_size = oyjlIsFile( lib_a, "r", info, 48 ),
      size;
  char * command = NULL;
  char * t;

  fprintf( zout, "compiling and testing: %s\n", oyjlTermColor(oyjlBOLD, prog) );

  if(c_source && len == code_size)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlUiJsonToCode(OYJL_SOURCE_CODE_C)   %lu", len );
  } else
  { PRINT_SUB( fail,
    "oyjlUiJsonToCode(OYJL_SOURCE_CODE_C)   %lu", len );
  }
  OYJL_TEST_WRITE_RESULT( c_source, len, "oyjlUiJsonToCode", "txt" )
  if(verbose && c_source)
    fprintf( zout, "%s\n", c_source );
  oyjlStringAdd( &name, 0,0, "%s.c", prog );
  oyjlWriteFile( name, c_source, len );
  if(c_source) {free(c_source);} c_source = NULL;
  /* compile */
  if(lib_a_size)
    oyjlStringAdd( &command, 0,0, "c++ %s -g -O0 -I %s -I %s %s -L %s/oyjl-args-qml -loyjl-args-qml-static -lQt5DBus -lQt5Qml -lQt5Network -lQt5Widgets -lQt5Gui -lQt5Core -L %s -loyjl-static -loyjl-core-static `pkg-config -libs-only-L openicc` -lopenicc-static -lyaml -lyajl -lxml2 -o %s", verbose?"-Wall -Wextra":"-Wno-write-strings", OYJL_SOURCEDIR, OYJL_BUILDDIR, name, OYJL_BUILDDIR, OYJL_BUILDDIR, prog );
  else if(lib_so_size)
    oyjlStringAdd( &command, 0,0, "cc %s -g -O0 -I %s -I %s %s -L %s -lOyjl -lOyjlCore -o %s", verbose?"-Wall -Wextra":"", OYJL_SOURCEDIR, OYJL_BUILDDIR, name, OYJL_BUILDDIR, prog );
  if(command)
  {
    if(verbose)
      fprintf( stderr, "compiling: %s\n", oyjlTermColor( oyjlBOLD, command ) );
    system(command);
    if(command) {free(command); command = NULL;}
    int size = oyjlIsFile( prog, "r", info, 48 );
    if(!size || verbose)
    {
      fprintf(stderr, "%scompile: %s %s %d\n", size == 0?"Could not ":"", oyjlTermColor(oyjlBOLD,prog), info, size);
    }
  }
  if(name) {free(name);} name = NULL;

  c_source = oyjlUiJsonToCode( json, OYJL_COMPLETION_BASH );
  len = c_source ? strlen(c_source) : 0;
  if(len == bash_size)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlUiJsonToCode(OYJL_COMPLETION_BASH) %lu", len );
  } else
  { PRINT_SUB( fail,
    "oyjlUiJsonToCode(OYJL_COMPLETION_BASH) %lu", len );
  }
  OYJL_TEST_WRITE_RESULT( c_source, strlen(c_source), "oyjlUiJsonToCode-Completion", "txt" )
  if(verbose && c_source)
    fprintf( zout, "%s\n", c_source );
  if(c_source) {free(c_source);} c_source = NULL;

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s --help", prog );
  len = t ? strlen(t) : 0;
  if(len == help_size)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "%s --help                        %lu", prog, len );
  } else
  { PRINT_SUB( fail,
    "%s --help                        %lu", prog, len );
  }
  OYJL_TEST_WRITE_RESULT( t, len, prog, "txt" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X man > %s.1 && COLUMNS=%d man ./%s.1", prog, prog, 400, prog );
  len = t ? strlen(t) : 0;
  if(len == man_size)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "%s -X man && man %s.1          %lu", prog, prog, len );
  } else
  { PRINT_SUB( fail,
    "%s -X man && man %s.1          %lu", prog, prog, len );
  }
  OYJL_TEST_WRITE_RESULT( t, len, prog, "man" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X markdown", prog );
  len = t ? strlen(t) : 0;
  if(len == markdown_size)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "%s -X markdown                   %lu", prog, len );
  } else
  { PRINT_SUB( fail,
    "%s -X markdown                   %lu", prog, len );
  }
  OYJL_TEST_WRITE_RESULT( t, len, prog, "md" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X json | %s/oyjl json -i -", prog, OYJL_BUILDDIR );
  len = t ? strlen(t) : 0;
  if(len == json_size && oyjlDataFormat(t) == 7)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "%s -X json                       %lu", prog, len );
  } else
  { PRINT_SUB( fail,
    "%s -X json                       %lu %d", prog, len, oyjlDataFormat(t) );
  system("ls -l oyjl");
  }
  OYJL_TEST_WRITE_RESULT( t, len, prog, "json" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X json+command | %s/oyjl json -i -", prog, OYJL_BUILDDIR );
  len = t ? strlen(t) : 0;
  if(len == json_command_size && oyjlDataFormat(t) == 7)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "%s -X json+command               %lu", prog, len );
  } else
  { PRINT_SUB( fail,
    "%s -X json+command               %lu", prog, len );
  }
  OYJL_TEST_WRITE_RESULT( t, len, prog, "json" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X export | %s/oyjl json -i -", prog, OYJL_BUILDDIR );
  len = t ? strlen(t) : 0;
  if(len == export_size && oyjlDataFormat(t) == 7)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "%s -X export                    %lu", prog, len );
  } else
  { PRINT_SUB( fail,
    "%s -X export                    %lu", prog, len );
  }
  OYJL_TEST_WRITE_RESULT( t, len, prog, "json" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  return result;
}

oyjlTESTRESULT_e testArgs()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );
  setlocale(LC_ALL,"en_GB.UTF8");

  int output = 0;
  const char * file = NULL;
  int file_count = 0;
  int show_status = 0;
  int help = 0;
  int verbose_ = 0;
  int version = 0;
  int state = 0;
  int argc = 1;
  const char * argv[] = {"test","-vvvvvv","--input","file-name.json", "-z"};

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
                                    {"","","",""}};
  oyjlOptionChoice_s o_choices[] = {{"0", _("Print All"), _("All Device Classes"), ""},
                                    {"1", _("Print Camera"), _("JSON"), ""},
                                    {"2", _("Print None"), "", ""},
                                    {"","","",""}};

  /*  declare options - the core information; use previously declared choices;
   *  only declared options are parsed
   */
  oyjlOption_s oarray[] = {
  /* type,   flags, o,   option,    key,  name,         description,         help, value_name,    value_type,               values,                                                          variable_type, output variable */
    {"oiwi", 0,     "#", "",        NULL, _("status"),  _("Show Status"),    NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &show_status} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,"@","",NULL,_("input"),_("Set Input"),NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {}, oyjlINT, {.i = &file_count} },
    {"oiwi", 0,     "i", "input",   NULL, _("input"),   _("Set Input"),      NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)i_choices, sizeof(i_choices), malloc )}, oyjlSTRING, {.s = &file} },
    {"oiwi", 0,     "o", "output",  NULL, _("output"),  _("Control Output"), NULL, "0|1|2",       oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)o_choices, sizeof(o_choices), malloc )}, oyjlINT, {.i = &output} },
    /* default options -h, -v and -V */
    {"oiwi", 0,     "h", "help",    NULL, _("help"),    _("Help"),           NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &help} },
    {"oiwi", 0,     "v", "verbose", NULL, _("verbose"), _("verbose"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &verbose_} },
    {"oiwi", 0,     "V", "version", NULL, _("version"), _("Version"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &version} },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups_no_args[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Mode1"),_("Simple mode"),     NULL, "#",       "o,v",    "o" }, /* accepted even if none of the mandatory options is set */
    {"oiwg", OYJL_OPTION_FLAG_EDITABLE,_("Mode2"),_("Any arg mode"),NULL,"@","o,v","@,o"},/* accepted if anonymous arguments are set */
    {"oiwg", 0,     _("Mode3"),_("Actual mode"),     NULL, "i",       "o,v",    "i,o" },/* parsed and checked with -i option */
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "",        "",       "v,h" },/* just show in documentation */
    {"",0,0,0,0,0,0,0}
  };

  /* tell about the tool */
  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups_no_args, NULL );
  /* done with options handling */

  if(verbose)
    oyjlOptions_PrintHelp( ui->opts, ui, 1, "%s v%s - %s", argv[0],
                            "1.0", "Test Tool for testing" );
  if(ui)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui created - no args                           " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui created - no args                           " );
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
                           8501                      /*code_size*/,
                           1082                      /*help_size*/,
                           1967                      /*man_size*/,
                           3788                      /*markdown_size*/,
                           6779                      /*json_size*/,
                           6806                      /*json_command_size*/,
                           10581                     /*export_size*/,
                           3163                      /*bash_size*/,
                           result,
                           oyjlTESTRESULT_FAIL       /*fail*/ );

  oyjlTreeFree( json ); json = NULL;


  const char * argv_anonymous[] = {"test","-v","file-name.json","file-name2.json"};
  int argc_anonymous = 4;
  ui = oyjlUi_Create( argc_anonymous, argv_anonymous, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups_no_args, NULL );
  if(ui)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui created - anonymous args                    " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui created - anonymous args                    " );
  }
  int count = 0, i;
  /* detect all anonymous arguments */
  char ** results = oyjlOptions_ResultsToList( ui?ui->opts:NULL, "@", &count );
  if(count == 2 &&
     file_count == count &&
     strcmp(argv_anonymous[2],results[0]) == 0 &&
     strcmp(argv_anonymous[3],results[1]) == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "anonymous args correctly detected  %d          ", count );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "anonymous args correctly detected  %d          ", count );
  }
  if(verbose)
  for(i = 0; i < count; ++i)
    fprintf( zout, "%s\n", results[i] );
  oyjlUi_Release( &ui);
  oyjlStringListRelease( &results, count, 0 );

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Mode"), _("Actual mode"),     NULL, "i",       "o,v",    "i,o" },
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "h",       "",       "h,v" },
    {"",0,0,0,0,0,0,0}
  };

  argc = 2;
  ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups, NULL );

  if(!ui)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui not created - missing mandatory option      " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui not created - missing mandatory option      " );
  }

  oyjlUi_Release( &ui);

  argc = 3;
  ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups, &state );
  if(!ui)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui not created - missing arg %d                " , state >> oyjlUI_STATE_OPTION );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui not created - missing arg                   " );
  }
  oyjlUi_Release( &ui);

  argc = 4;
  ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups, NULL );
  if(ui && strcmp(file,"file-name.json") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui created - parse string                      " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui created - parse string                      " );
  }
  json = oyjlUi_ExportToJson_( ui, 0/*flags*/ );
  oyjlUi_Release( &ui);

  result = testCode( json, "oiCR"                    /*prog*/,
                           8246                      /*code_size*/,
                            615                      /*help_size*/,
                           1451                      /*man_size*/,
                           2300                      /*markdown_size*/,
                           4710                      /*json_size*/,
                           4737                      /*json_command_size*/,
                           10066                     /*export_size*/,
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
    "ui not created - wrong arg  %d                 ", state >> oyjlUI_STATE_OPTION );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui not created - wrong arg                     " );
  }
  oyjlUi_Release( &ui);


  argc = 4;
  ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyjl-config-read", "Oyjl Config Reader", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups, NULL );
  text = oyjlUi_ToJson( ui, 0 );
  if(text && strlen(text) == 4916)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ToJson() %lu                           ", strlen(text) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ToJson() 4903 == %lu                   ", strlen(text) );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToJson", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  text = oyjlUi_ToMan( ui, 0 );
  if(text && strlen(text) == 833)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ToMan() %lu                            ", strlen(text) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ToMan() 848 == %lu                     ", strlen(text) );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToMan", "txt" )
  if(verbose)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  text = oyjlOptions_ResultsToJson( ui->opts );
  if(text && strlen(text) == 63)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_ResultsToJson() %lu                 ", strlen(text) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_ResultsToJson() %lu                 ", strlen(text) );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlOptions_ResultsToJson", "txt" )
  if(verbose)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;
  setlocale(LC_ALL,"");

  text = oyjlOptions_ResultsToText( ui->opts );
  if(text && strlen(text) == 41)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_ResultsToText() %lu                 ", strlen(text) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_ResultsToText() %lu                 ", strlen(text) );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlOptions_ResultsToText", "txt" )
  if(verbose)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  if(verbose)
  {
    fprintf(stdout, "Help text -> stderr:\n" );
    oyjlOptions_PrintHelp( ui->opts, ui, 1, "%s v%s - %s", argv[0],
                            "1.0", "Test Tool for testing" );
  }

  text = oyjlUi_ExportToJson( ui, 0 );
  if(text && strlen(text) == 6644)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ExportToJson()       %lu                 ", strlen(text) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ExportToJson()       6644 == %lu         ", strlen(text) );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ExportToJson", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) { free(text); text = NULL; }

  oyjlUi_Release( &ui);
  char * wrong = "test";
  fprintf(stdout, "oyjlUi_Release(&\"test\") - should give a warning message:\n" );
  oyjlUi_Release( (oyjlUi_s **)&wrong);

  free(oarray[2].values.choices.list);
  free(oarray[3].values.choices.list);

  return result;
}


void oyjlTreeToJson2_(oyjl_val v, int * level, char ** json);

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

  char * text = NULL; i = 0;
  root = oyjlTreeNew( NULL );
  oyjlTreeToJson( root, &i, &text ); i = 0;
  if(root && text && strcmp(text,"null") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeNew( NULL )                     " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeNew( NULL )                     " );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeNew_NULL", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  oyjlTreeFree( root );
  if(text) { free(text); text = NULL; }

  root = oyjlTreeNew( "" );
  oyjlTreeToJson( root, &i, &text ); i = 0;
  if(root && text && strcmp(text,"null") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeNew( \"\" )                       " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeNew( \"\" )                       " );
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
  if(root && len == 56)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeNew( \"new/tree/key\" )       %d", (int)len );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeNew( \"new/tree/key\" )       %d", (int)len );
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
  oyjlValueSetString( value, VALUE );
  oyjlTreeToJson( root, &i, &text ); i = 0;
  if(len < strlen(text))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlValueSetString( %s )         %d", VALUE, (int)strlen(text) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlValueSetString( %s )         %d", VALUE, (int)strlen(text) );
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
  if(!text || strcmp(text, "null") == 0)
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
  if(count == 4 && strlen(text) == 34)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeGetValue(OYJL_CREATE_NEW, \"arr/[3]\") oyjlValueCount() %i", count );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeGetValue(OYJL_CREATE_NEW, \"arr/[3]\") oyjlValueCount() %i", count );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeGetValue-array-4", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "value", "arr/[1]/key" );
  text = oyjlTreePrint(root);
  if(strlen(text) == 58)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeSetStringF(OYJL_CREATE_NEW, \"arr/[1]/key\") %lu", strlen(text) );
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
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "value", "one/[%d]/key3", 1 );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "arr3a", "one/[%d]/data/[0]", 1 );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "arr3b", "one/[%d]/data/[1]", 1 );
  oyjlTreeSetDoubleF( root, OYJL_CREATE_NEW, 1.4,     "one/[%d]/key4", 1 );
  oyjlTreeSetDoubleF( root, OYJL_CREATE_NEW, 1.2,     "one/[%d]/data/[2]", 1 );
  oyjlTreeToJson( root, &i, &text ); i = 0;
  if( text && strlen( text ) == 238 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "add array                                      %ld", strlen(text) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "add array                                      %ld", strlen(text) );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeSetStringF", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  myDeAllocFunc( text ); text = NULL;
  oyjlTreeFree( root );

  double clck = oyjlClock();
  root = oyjlTreeNew("");
  int n = 2000;
  for(i = 0; i < n; ++i)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "value", "data/key-%d", i );
  clck = oyjlClock() - clck;
  fprintf( zout, "oyjlTreeSetStringF()\t%dx              \t\%s\n", n,
                 oyjlProfilingToString(n,clck/(double)CLOCKS_PER_SEC,"node"));
  i = 0;

  clck = oyjlClock();
  oyjlTreeToJson2_( root, &i, &text ); i = 0;
  clck = oyjlClock() - clck;
  fprintf( zout, "oyjlTreeToJson2_()      \t1x %d            \t\%s\n", (int)strlen(text),
                 oyjlProfilingToString(1,clck/(double)CLOCKS_PER_SEC,"dump"));
  myDeAllocFunc( text ); text = NULL;

  clck = oyjlClock();
  oyjlTreeToJson( root, &i, &text ); i = 0;
  clck = oyjlClock() - clck;
  fprintf( zout, "oyjlTreeToJson()        \t1x %d            \t\%s\n", (int)strlen(text),
                 oyjlProfilingToString(1,clck/(double)CLOCKS_PER_SEC,"dump"));
  myDeAllocFunc( text ); text = NULL;
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
  int r = oyjlIsFile( "test.txt", "r", info, len );
  int size2 = oyjlWriteFile( "test2.txt", "test2", 6 );
  char info2[len];
  int r2 = oyjlIsFile( "test2.txt", "r", info2, len );

  if(r && r2 && size == 5 && size2 == 6)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlWriteFile()                         " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlWriteFile()                         " );
  }

  if(strlen(info) && strlen(info2))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlIsFile()                            " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlIsFile()                            " );
  }

  char * text = oyjlReadFile( "test2.txt", &size );
  if(text && strcmp(text, "test2") == 0 && size == 6)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlReadFile()                          " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlReadFile()                          " );
  }
  if(text) { free(text); text = NULL; }

  text = oyjlReadCommandF( &size, "r", malloc, "echo ABC" );
  if(text && strcmp(text, "ABC\n") == 0 && size == 4)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlReadCommandF()                      " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlReadCommandF() %s %d                ", text, size );
  }
  if(text) { free(text); text = NULL; }

  if( verbose )
    fprintf( zout, "info: %s %s\n", info, info2 );

  return result;
}

/* --- end actual tests --- */


