/** @file test.c
 *
 *  Oyranos is an open source Color Management System
 *
 *  Copyright (C) 2004-2019  Kai-Uwe Behrmann
 *
 *  @brief    Oyranos test suite
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2008/12/04
 */

#define TESTS_RUN \
  TEST_RUN( testVersion, "Version matching", 1 ); \
  TEST_RUN( testI18N, "Internationalisation", 1 ); \
  TEST_RUN( testDataFormat, "Data Format Detection", 1 ); \
  TEST_RUN( testJson, "JSON handling", 1 ); \
  TEST_RUN( testFromJson, "Data Writers", 1 ); \
  TEST_RUN( testJsonRoundtrip, "Data Readers", 1 ); \
  TEST_RUN( testUiRoundtrip, "Ui Export", 1 ); \
  TEST_RUN( testUiTranslation, "Ui Translation", 1 ); \
  TEST_RUN( testToolOyjl, "Tool oyjl", 1 ); \
  TEST_RUN( testToolOyjlTranslation, "Tool oyjl-translation", 1 );

void oyjlLibRelease();
#define OYJL_TEST_MAIN_SETUP  printf("\n    Oyjl Test Program\n");

#include "oyjl_version.h"
#include "oyjl.h"
#ifdef OYJL_HAVE_LIBXML2
#include <libxml/parser.h>
# define OYJL_TEST_MAIN_FINISH printf("\n    Oyjl Test Program finished\n\n"); oyjlLibRelease(); if(oyjl_print_sub) free(oyjl_print_sub); xmlCleanupParser();
#else
# define OYJL_TEST_MAIN_FINISH printf("\n    Oyjl Test Program finished\n\n"); oyjlLibRelease(); if(oyjl_print_sub) free(oyjl_print_sub);
#endif
#define OYJL_TEST_NAME "test"
#include "oyjl_test_main.h"
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>
#endif
#include "oyjl_i18n.h"
#include "oyjl_internal.h"
#include "oyjl_macros.h"      /* OYJL_CREATE_VA_STRING */
#include "liboyjl.i18n.h"

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

char *   testTranslateJson           ( const char        * json,
                                       oyjlTr_s          * tr_context,
                                       const char        * key_list,
                                       int                 count,
                                       double            * clock )
{
  char * txt = 0;
  int i;
  oyjl_val array[count];
  char error_buffer[128] = {0};
  for(i = 0; i < count; ++i)
    array[i] = oyjlTreeParse( json, error_buffer,128 );
  if(error_buffer[0])
  {
    char * t = oyjlBT(0);
    fprintf( stderr, "%s%s\n", t, error_buffer );
    free(t);
  }
  double clck = oyjlClock();
  for( i = 0; i < count; ++i )
    oyjlTranslateJson( array[i], tr_context, key_list );
  clck = oyjlClock() - clck;
  *clock = clck;

  i = 0;
  oyjlTreeToJson( array[0], &i, &txt );

  for(i=0;i<count;++i) oyjlTreeFree( array[i] );

  return txt;
}

extern oyjlMessage_f oyjlMessage_p;
int          oyjlMessageFunc         ( int/*oyjlMSG_e*/    error_code,
                                       const void        * context_object OYJL_UNUSED,
                                       const char        * format,
                                       ... );
oyjlTESTRESULT_e testI18N()
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
    "Language uninitialised failed" );
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

  const char * loc = setlocale(LC_ALL,"de_DE.UTF8");
  int use_gettext = 0;
#ifdef OYJL_USE_GETTEXT
  use_gettext = 1;
#endif
  static int my_debug = 0;
  oyjlTr_s * trc = NULL;
  my_debug = verbose;
  if(loc)
  {
    int size = sizeof(liboyjl_i18n_oiJS);
    oyjl_val oyjl_catalog = (oyjl_val) oyjlStringAppendN( NULL, (const char*) liboyjl_i18n_oiJS, size, malloc );
    if(*oyjl_debug)
      oyjlMessage_p( oyjlMSG_INFO, 0, "loc: \"%s\" domain: \"%s\" catalog-size: %d", loc, OYJL_DOMAIN, size );
    trc = oyjlTr_New( loc, OYJL_DOMAIN, &oyjl_catalog, 0,0,0,0 );
  }
  oyjlInitLanguageDebug( "Oyjl", "OYJL_DEBUG", &my_debug, use_gettext, "OYJL_LOCALEDIR", OYJL_LOCALEDIR, &trc, oyjlMessageFunc );
  oyjlTr_Release( &trc );

  loc = setlocale(LC_ALL, "");
  if(loc && (strcmp(loc, "C") != 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "setlocale() initialised good %s", loc );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "setlocale() initialised failed %s", loc );
  }

  const char * text = _("Example");
  if(strcmp(text,"Beispiel") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "dgettext() good \"%s\"", text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "dgettext() failed \"%s\"", text );
  }

  oyjl_val catalog = NULL;
  int size;
  size = sizeof(liboyjl_i18n_oiJS);
  catalog = (oyjl_val) oyjlStringAppendN( NULL, (const char*) liboyjl_i18n_oiJS, size, malloc );
  loc = setlocale(LC_ALL,"de_DE.UTF8");
  trc = oyjlTr_New( loc, OYJL_DOMAIN, &catalog, 0,0,0,0 );
  oyjlTr_Set( &trc );
  oyjlLang(loc);


  const char * json = "{\n\
  \"org\": {\n\
    \"freedesktop\": {\n\
      \"oyjl\": {\n\
        \"translations\": {\n\
          \"de_DE.UTF8\": {\n\
            \"Example\": \"Beispiel\"\n\
          },\n\
          \"de_DE\": {\n\
            \"Example2\": \"Beispiel2\"\n\
          },\n\
          \"de_AT\": {\n\
            \"Nonsense\": \"Schmarrn\"\n\
          },\n\
          \"de\": {\n\
            \"Color\": \"Farbe\",\n\
            \"2. Color\": \"2. Farbe\",\n\
            \"Color [3]\": \"Farbe [3]\",\n\
            \"Color \\\"Rose\\\"\": \"Farbe \\\"Rosa\\\"\",\n\
            \"Use [A-Z,a-z] underscore '_'.\": \"Benutze [A-Z,a-z] Understrich '_'.\",\n\
            \"Mode(1)\": \"Modus(1)\",\n\
            \"prog -i test.json ///my/path/\": \"prog -i test.json ///mein/Pfad/\"\n\
          }\n\
        }\n\
      }\n\
    }\n\
  }\n\
}";
  oyjlWriteFile( "i18n.json", json, strlen(json) );
  int flags = 0;
  catalog = oyjlTreeParse( json, NULL, 0 );
  trc = oyjlTr_New( loc, OYJL_DOMAIN, &catalog, NULL,NULL,NULL, flags );
  oyjlTr_Set( &trc );
  text = _("");
  if(strcmp(text,"") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de_DE.UTF8\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlTranslate(%s) \"de_DE.UTF8\":\"%s\"", loc, text );
  }

  text = _("Example");
  if(strcmp(text,"Beispiel") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de_DE.UTF8\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de_DE.UTF8\":\"%s\"", loc, text );
  }

  text = _("Example2");
  if(strcmp(text,"Beispiel2") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de_DE\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de_DE\":\"%s\"", loc, text );
  }

  text = _("Color");
  if(strcmp(text,"Farbe") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  }

  text = _("2. Color");
  if(strcmp(text,"2. Farbe") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  }

  text = _("Color [3]");
  if(strcmp(text,"Farbe [3]") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  }

  text = _("Color \"Rose\"");
  if(strcmp(text,"Farbe \"Rosa\"") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  }

  flags = verbose ? OYJL_OBSERVE : 0;
  trc = oyjlTr_Get(OYJL_DOMAIN);
  oyjlTr_SetFlags( trc, flags );
  text = _("Use [A-Z,a-z] underscore '_'.");
  if(strcmp(text,"Benutze [A-Z,a-z] Understrich '_'.") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  }

  flags = verbose ? OYJL_OBSERVE : 0;
  text = _("Mode(1)");
  if(strcmp(text,"Modus(1)") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  }

  flags = verbose ? OYJL_OBSERVE : 0;
  text = _("prog -i test.json ///my/path/");
  if(strcmp(text,"prog -i test.json ///mein/Pfad/") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  }

  flags = 0;
  oyjlTr_SetFlags( trc, flags );
  text = _("Nonsense");
  if(strcmp(text,"Schmarrn") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de_AT\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de_AT\":\"%s\"", loc, text );
  }

  loc = "de_CH.UTF-8";
  oyjlLang(loc);
  text = _("Nonsense");
  if(strcmp(text,"Schmarrn") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de_AT\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de_AT\":\"%s\"", loc, text );
  }

  loc = "de_DE";
  oyjlLang(loc);
  text = _("Example");
  if(strcmp(text,"Beispiel") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de_DE.UTF8\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de_DE.UTF8\":\"%s\"", loc, text );
  }

  text = _("Color");
  if(strcmp(text,"Farbe") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  }

  loc = "de";
  oyjlLang(loc);
  text = _("Example");
  if(strcmp(text,"Beispiel") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de_DE.UTF8\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de_DE.UTF8\":\"%s\"", loc, text );
  }

  loc = "de_CH";
  oyjlLang(loc);
  text = _("Color");
  if(strcmp(text,"Farbe") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  }

  setlocale(LC_ALL,"");
  oyjlLang("C");

  size = sizeof(liboyjl_i18n_oiJS);
  catalog = (oyjl_val) oyjlStringAppendN( NULL, (const char*) liboyjl_i18n_oiJS, size, malloc );
  loc = setlocale(LC_ALL,"de_DE.UTF8");
  loc = oyjlLang(loc);

  char * oyjl_export, * txt;
  size = 0;
  oyjl_export = txt = oyjlReadCommandF( &size, "r", malloc, "LANG=C PATH=%s:$PATH %s --export export", OYJL_BUILDDIR, "oyjl" );
  if(!txt || strlen(txt) != 22471)
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(txt),
    "LANG=C oyjl --export export" );
  }

  const char * key_list = "name,description,help,label,value_name";

  int count, i;
  double clck = oyjlClock();
  char ** paths = oyjlTreeToPaths( catalog, 10000000, NULL, OYJL_KEY, &count );
  clck = oyjlClock() - clck;
  count = 0; while(paths && paths[count]) ++count;
  if( count == 347 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS,1,clck/(double)CLOCKS_PER_SEC,"wr",
    "oyjlTreeToPaths(catalog) = %d", count );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
    "oyjlTreeToPaths(catalog)" );
  }

  int langs_n = 0,
     *lang_positions_start = NULL;
  char ** langs;
  clck = oyjlClock();
  for( i = 0; i < 500; ++i )
  {
    langs = oyjlCatalogGetLangs_( paths, count,
                                  &langs_n, &lang_positions_start );
    if(langs && langs_n && i < 500-1)
    {
      oyjlStringListRelease( &langs, langs_n, free );
      free( lang_positions_start );
      lang_positions_start = NULL;
    }
  }
  clck = oyjlClock() - clck;
  int j;
  txt = NULL;
  for(j = 0; j < langs_n; ++j)
    oyjlStringAdd( &txt, 0,0, " %s:%d", langs[j], lang_positions_start[j] );
  fprintf( zout, "%s\n",
    oyjlPrintSubProfiling( -1, i, clck/(double)CLOCKS_PER_SEC,"ck",
    "oyjlCatalogGetLangs_(%d)%s", count, txt ) );
  oyjlStringListRelease( &langs, langs_n, free );
  free( lang_positions_start );
  lang_positions_start = NULL;
  myDeAllocFunc( txt ); txt = NULL;
  oyjlStringListRelease( &paths, count, free );

  const char * name = "catalog";
  loc = "de_DE.UTF8";
  trc = oyjlTr_New( loc, OYJL_DOMAIN, &catalog, NULL,NULL,NULL, !verbose?0:OYJL_OBSERVE );
  clck = oyjlClock();
  for( i = 0; i < 100; ++i )
    text = oyjlTranslate( trc, "render" );
  clck = oyjlClock() - clck;
  if( strcmp(text,"Darstellung") == 0 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"tr",
    "oyjlTranslate(\"%s\",%s,\"render\") %s", loc, name, oyjlTr_GetLang( trc ) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTranslate(\"%s\",%s,\"render\")", loc, name );
  }

  loc = "de_DE";
  oyjlLang( loc );
  clck = oyjlClock();
  for( i = 0; i < 100; ++i )
    text = oyjlTranslate( trc, "render" );
  clck = oyjlClock() - clck;
  if( strcmp(text,"Darstellung") == 0 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"tr",
    "oyjlTranslate(\"%s\",%s,\"render\") %s", loc, name, oyjlTr_GetLang( trc ) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTranslate(\"%s\",%s,\"render\")", loc, name );
  }

  loc = "de";
  oyjlLang( loc );
  clck = oyjlClock();
  for( i = 0; i < 100; ++i )
    text = oyjlTranslate( trc, "render" );
  clck = oyjlClock() - clck;
  if( strcmp(text,"Darstellung") == 0 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"tr",
    "oyjlTranslate(\"%s\",%s,\"render\") %s", loc, name, oyjlTr_GetLang( trc ) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTranslate(\"%s\",%s,\"render\")", loc, name );
  }

  loc = "cs";
  oyjlTr_SetLocale( trc, loc );
  clck = oyjlClock();
  for( i = 0; i < 10000; ++i )
    text = oyjlTranslate( trc, "Color" );
  clck = oyjlClock() - clck;
  if( strcmp(text,"Barva") == 0 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"tr",
    "oyjlTranslate(\"%s\",%s,\"Color\")", loc, name );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTranslate(\"%s\",%s,\"Color\")", loc, name );
  }

  int n = 1;
  loc = "de_DE";
  oyjlTr_SetLocale( trc, loc );
  txt = testTranslateJson( oyjl_export, trc, key_list, n, &clck );
  i = 0;
  if( txt && strlen( txt ) == 22846 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS,n,clck/(double)CLOCKS_PER_SEC,"JS",
    "oyjlTranslateJson(\"%s\",%s) %s", loc, name, oyjlTr_GetLang( trc ) );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(txt),
    "oyjlTranslateJson(\"%s\",%s) %s", loc, name, oyjlTr_GetLang( trc ) );
  }
  OYJL_TEST_WRITE_RESULT( txt, strlen(txt), "oyjlTranslateJson", "txt" )
  if(verbose && txt)
    fprintf( zout, "%s\n", txt );
  myDeAllocFunc( txt ); txt = NULL;

  oyjlTr_Release( &trc );

  //return result;

#ifdef OYJL_USE_GETTEXT
  n = 100000;
  name = "gettext";
  loc = "de_DE.UTF8";
  setlocale( LC_ALL, loc );
  trc = oyjlTr_New( loc, OYJL_DOMAIN, NULL, NULL,NULL,NULL, OYJL_GETTEXT | (!verbose?0:OYJL_OBSERVE) );
  clck = oyjlClock();
  for( i = 0; i < n; ++i )
    text = oyjlTranslate( trc, "render" );
  clck = oyjlClock() - clck;
  if( strcmp(text,"Darstellung") == 0 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"tr",
    "oyjlTranslate(\"%s\",%s,\"render\")", loc, name );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTranslate(\"%s\",%s,\"render\")", loc, name );
  }
  loc = "de_DE";
  oyjlTr_SetLocale( trc, loc );
  clck = oyjlClock();
  for( i = 0; i < n; ++i )
    text = oyjlTranslate( trc, "render" );
  clck = oyjlClock() - clck;
  if( strcmp(text,"Darstellung") == 0 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"tr",
    "oyjlTranslate(\"%s\",%s,\"render\")", loc, name );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTranslate(\"%s\",%s,\"render\")", loc, name );
  }
  loc = "de";
  oyjlTr_SetLocale( trc, loc );
  clck = oyjlClock();
  for( i = 0; i < n; ++i )
    text = oyjlTranslate( trc, "render" );
  clck = oyjlClock() - clck;
  if( strcmp(text,"Darstellung") == 0 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"tr",
    "oyjlTranslate(\"%s\",%s,\"render\")", loc, name );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTranslate(\"%s\",%s,\"render\")", loc, name );
  }

  n = 1;
  loc = "de_DE";
  oyjlTr_SetLocale( trc, loc );
  txt = testTranslateJson( oyjl_export, trc, key_list, n, &clck );
  if( txt && strlen( txt ) == 22846 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS,n,clck/(double)CLOCKS_PER_SEC,"JS",
    "oyjlTranslateJson(gettext)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(txt),
    "oyjlTranslateJson(gettext)" );
  }
  OYJL_TEST_WRITE_RESULT( txt, strlen(txt), "oyjlTranslateJson", "txt" )
  if(/*oy_test_last_result == oyjlTESTRESULT_FAIL || oy_test_last_result == oyjlTESTRESULT_XFAIL ||*/ verbose)
    fprintf( zout, "%s\n", txt );
  myDeAllocFunc( txt ); txt = NULL;
  oyjlTr_Release( &trc );
#endif

  double tmp_d;
#define printtime oyjlPrintTime(OYJL_TIME, oyjlNO_MARK), (int)(modf(oyjlSeconds(),&tmp_d)*1000)
#define timeformat "[%s.%04d] "
  fprintf( zout, timeformat "before oyjlTreeParse(liboyjl_i18n_oiJS)\n", printtime);
  oyjl_val root = oyjlTreeParse( oyjl_export, NULL, 0 );
  fprintf( zout, timeformat "before while\n", printtime );
  size = sizeof(liboyjl_i18n_oiJS);
  catalog = (oyjl_val) oyjlStringAppendN( NULL, (const char*) liboyjl_i18n_oiJS, size, malloc );
  loc = "de_DE";
  trc = oyjlTr_New( loc, OYJL_DOMAIN, &catalog, NULL, NULL,NULL, flags );
  fprintf( zout, timeformat "after while; before oyjlTranslateJson\n", printtime );
  oyjlTranslateJson( root, trc, key_list );
  fprintf( zout, timeformat "after oyjlTranslateJson\n", printtime );
  oyjlTr_Release( &trc );
  i = 0;
  txt = NULL;
  oyjlTreeToJson( root, &i, &txt );
  if( txt && strlen( txt ) == 22846 )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(txt),
    "oyjlTranslateJson(oyjl)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(txt),
    "oyjlTranslateJson(oyjl)" );
  }
  OYJL_TEST_WRITE_RESULT( txt, strlen(txt), "oyjlTranslateJson", "txt" )
  if(verbose && txt)
    fprintf( zout, "%s\n", txt );
  myDeAllocFunc( txt ); txt = NULL;
  oyjlTreeFree( root );

  size = 0;
  //flags = verbose ? OYJL_OBSERVE : 0;
  catalog = oyjlTreeParse( json, NULL, 0 );
  oyjl_val static_catalog = oyjlTreeSerialise( catalog, flags, &size );
  if(size == 854 && memcmp( static_catalog, "oiJS", 4 ) == 0)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, size,
    "oyjlTreeSerialise() oiJS" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, size,
    "oyjlTreeSerialise() oiJS" );
  }
  oyjlWriteFile( "oiJS.json", static_catalog, size );
char *     oyjlTreeSerialisedPrint_  ( oyjl_val            v,
                                       int                 flags OYJL_UNUSED );
  char * t = oyjlTreeSerialisedPrint_( static_catalog, 0 );
  if(verbose)
    fprintf( zout, "parsed: \"%s\"\n", t );
  free(t); t = NULL;
  oyjlTreeFree( catalog );

  j = 0;
  paths = oyjlTreeToPaths( static_catalog, 10000000, NULL, flags | OYJL_KEY,&count);
  if(count == 10 &&
      strcmp(paths[j++],"org/freedesktop/oyjl/translations/de_DE\\.UTF8/Example") == 0 &&
      strcmp(paths[j++],"org/freedesktop/oyjl/translations/de_DE/Example2") == 0 &&
      strcmp(paths[j++],"org/freedesktop/oyjl/translations/de_AT/Nonsense") == 0 &&
      strcmp(paths[j++],"org/freedesktop/oyjl/translations/de/Color") == 0
    )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
    "oyjlTreeToPaths( oiJS )" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
    "oyjlTreeToPaths( oiJS )" );
  }
  if(verbose)
    for(j = 0; j < count; ++j)
      fprintf( zout, "%d: %s\n", j, paths[j] );

  if(paths && count)
    oyjlStringListRelease( &paths, count, free );

  loc = "de_DE.UTF-8";
  trc = oyjlTr_New( loc, OYJL_DOMAIN, &static_catalog, NULL, NULL,NULL, flags );
  oyjlTr_Set( &trc );
  text = _("Color [3]");
  if(strcmp(text,"Farbe [3]") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  }

  loc = "de_CH";
  oyjlLang( loc );
  text = _("Color");
  if(strcmp(text,"Farbe") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de\":\"%s\"", loc, text );
  }
  oyjlTr_Release( &trc );

  size = sizeof(liboyjl_i18n_oiJS);
  catalog = (oyjl_val) oyjlStringAppendN( NULL, (const char*) liboyjl_i18n_oiJS, size, malloc );
  trc = oyjlTr_New( "C", OYJL_DOMAIN, &catalog, NULL,NULL,NULL, !verbose?0:OYJL_OBSERVE );
  oyjlTr_Set( &trc );
  loc = setlocale(LC_ALL,"de_DE.UTF8");
  loc = oyjlLang(loc);
  root = oyjlTreeParse( oyjl_export, NULL, 0 );
  fprintf( zout, timeformat "before while\n", printtime );
  size = 0; while(size < 1000) ++size;
  fprintf( zout, timeformat "after while; before oyjlTranslateJson\n", printtime );
  oyjlTranslateJson( root, oyjlTr_Get(OYJL_DOMAIN), key_list );
  fprintf( zout, timeformat "after oyjlTranslateJson\n", printtime );
  i = 0;
  txt = NULL;
  oyjlTreeToJson( root, &i, &txt );
  if( txt && strlen( txt ) == 22846 )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(txt),
    "oyjlTranslateJson(oyjl, static_catalog)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(txt),
    "oyjlTranslateJson(oyjl, static_catalog)" );
  }
  OYJL_TEST_WRITE_RESULT( txt, strlen(txt), "oyjlTranslateJson", "txt" )
  if(verbose && txt)
    fprintf( zout, "%s\n", txt );
  myDeAllocFunc( txt ); txt = NULL;
  myDeAllocFunc( oyjl_export ); oyjl_export = NULL;
  oyjlTreeFree( root );

  return result;
}

oyjlTESTRESULT_e testDataFormat ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  const char * data = NULL;
  int format = oyjlDataFormat(data);
  if(format == -1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (%s) = %d guessed", data?data:"NULL", format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (%s) = %d guessed", data?data:"NULL", format );
  }

  data = "";
  format = oyjlDataFormat(data);
  if(format == -2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  }

  data = "  \t ";
  format = oyjlDataFormat(data);
  if(format == -2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  }

  data = "\t XYZ";
  format = oyjlDataFormat(data);
  if(format == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  }

  data = " {\"a\":}";
  format = oyjlDataFormat(data);
  if(format == 7)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  }

  data = " [{\"a\":}]";
  format = oyjlDataFormat(data);
  if(format == 7)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  }

  data = "  <?xml attr=\"XX\"><p>";
  format = oyjlDataFormat(data);
  if(format == 8)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  }

  data = "  <p>XYZ";
  format = oyjlDataFormat(data);
  if(format == 8)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  }

  data = "  \n---\na:";
  format = oyjlDataFormat(data);
  if(format == 9)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  }

  data = "---a:";
  format = oyjlDataFormat(data);
  if(format == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  }

  data = "  ---\na:";
  format = oyjlDataFormat(data);
  if(format == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed", data, format );
  }

  return result;
}

#define end_example

oyjlTESTRESULT_e testEscapeJson      ( const char        * text_,
                                       const char *        escaped,
                                       int                 flags,
                                       size_t              tree_size,
                                       oyjlTESTRESULT_e    result,
                                       oyjlTESTRESULT_e    fail )
{
  char * key = oyjlJsonEscape( text_, flags ), * text = 0;
  int i = 0;

  if( strcmp( key, escaped ) == 0 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlJsonEscape(%s, [%s,%s,%s]) \"%s\"", oyjlTermColor(oyjlBOLD,text_),
    flags&OYJL_NO_BACKSLASH?"/":"", flags&OYJL_QUOTE?"\"":"", flags&OYJL_REGEXP?"r":flags&OYJL_NO_INDEX?"\\[":"",
    key  );
  } else
  { PRINT_SUB( fail,
    "oyjlJsonEscape(%s, [%s,%s,%s]) \"%s\" %s", oyjlTermColor(oyjlBOLD,text_),
    flags&OYJL_NO_BACKSLASH?"/":"", flags&OYJL_QUOTE?"\"":"", flags&OYJL_REGEXP?"r":flags&OYJL_NO_INDEX?"\\[":"",
    key, escaped );
  }

  oyjl_val root = oyjlTreeNew("");
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "value", "data/key-%s", key );
  oyjlTreeToJson( root, &i, &text ); i = 0;
  char error_buffer[128];
  oyjl_val rroot = oyjlTreeParse( text, error_buffer, 128 );
  if( text && strlen( text ) == tree_size && rroot )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(text),
    "set key  [%s,%s,%s] \"%s\"",
    flags&OYJL_NO_BACKSLASH?"/":"", flags&OYJL_QUOTE?"\"":"", flags&OYJL_NO_INDEX?"\\[":"",
    oyjlTermColor(oyjlBOLD,key) );
  } else
  { PRINT_SUB_INT( fail, strlen(text),
    "set key  [%s,%s,%s] \"%s\"",
    flags&OYJL_NO_BACKSLASH?"/":"", flags&OYJL_QUOTE?"\"":"", flags&OYJL_NO_INDEX?"\\[":"",
    oyjlTermColor(oyjlBOLD,key) );
  }
  oyjlTreeFree( rroot );
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeSetStringF", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  myDeAllocFunc( text ); text = NULL;

  oyjl_val value = oyjlTreeGetValueF( root, 0, "data/key-%s", key );
  if( value )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "get key                      \"%s\"", key );
  } else
  { PRINT_SUB( fail,
    "get key                             \"%s\"", key );
  }

  char * path = oyjlTreeGetPath( root, value );
  char * expect = NULL;
  oyjlStringAdd( &expect, 0,0, "data/key-%s", key );
  if( path && expect && ((expect && strchr(expect,'[') && !strstr(expect,"\\[")) || strcmp(path,expect) == 0) )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "get path                     \"%s\"", path );
  } else
  { PRINT_SUB( fail,
    "get path \"%s\"/\"%s\"", expect, path );
  }
  myDeAllocFunc( path ); path = NULL;
  myDeAllocFunc( expect ); expect = NULL;
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || oy_test_last_result == oyjlTESTRESULT_XFAIL || verbose)
  {
    int count, i;
    char ** paths = oyjlTreeToPaths( root, 10000000, NULL, OYJL_KEY, &count );

    for(i = 0; i < count; ++i)
    {
      path = paths[i];
      fprintf( zout, "paths[%d]: %s\n", i, path );
    }

    if(paths && count)
      oyjlStringListRelease( &paths, count, free );
  }
  oyjlTreeFree( root );
  myDeAllocFunc( text ); text = NULL;
  myDeAllocFunc( key ); key = NULL;

  return result;
}

oyjlTESTRESULT_e testEscapeJsonVal   ( const char        * text_,
                                       const char *        escaped,
                                       int                 flags,
                                       size_t              tree_size,
                                       oyjlTESTRESULT_e    result,
                                       oyjlTESTRESULT_e    fail )
{
  char * value = oyjlJsonEscape( text_, flags ), * text = 0;
  int i = 0;

  if( strcmp( value, escaped ) == 0 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlJsonEscape(%s, [%s,%s,%s])     \"%s\"", oyjlTermColor(oyjlBOLD,text_),
    flags&OYJL_NO_BACKSLASH?"/":"", flags&OYJL_QUOTE?"\"":"", flags&OYJL_REGEXP?"r":flags&OYJL_NO_INDEX?"\\[":"",
    value );
  } else
  { PRINT_SUB( fail,
    "oyjlJsonEscape(%s, [%s,%s,%s]) \"%s\" %s", oyjlTermColor(oyjlBOLD,text_),
    flags&OYJL_NO_BACKSLASH?"/":"", flags&OYJL_QUOTE?"\"":"", flags&OYJL_REGEXP?"r":flags&OYJL_NO_INDEX?"\\[":"",
    value, escaped );
  }

  oyjl_val root = oyjlTreeNew("");
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, value, "key" );
  oyjlTreeToJson( root, &i, &text ); i = 0;
  char error_buffer[128];
  oyjl_val rroot = oyjlTreeParse( text, error_buffer, 128 );
  if( text && strlen( text ) == tree_size && rroot )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(text), 
    "set value [%s,%s,%s] \"%s\"",
    flags&OYJL_NO_BACKSLASH?"/":"", flags&OYJL_QUOTE?"\"":"", flags&OYJL_NO_INDEX?"\\[":"",
    oyjlTermColor(oyjlBOLD,value), strlen(text) );
  } else
  { PRINT_SUB_INT( fail, strlen(text),
    "set value [%s,%s,%s] \"%s\"",
    flags&OYJL_NO_BACKSLASH?"/":"", flags&OYJL_QUOTE?"\"":"", flags&OYJL_NO_INDEX?"\\[":"",
    oyjlTermColor(oyjlBOLD,value), strlen(text) );
  }
  oyjlTreeFree( rroot );
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "value", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );

  oyjlTreeFree( root );
  myDeAllocFunc( text ); text = NULL;
  myDeAllocFunc( value ); value = NULL;

  return result;
}

oyjlTESTRESULT_e testJson ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int i;
  const char * json = "{\n\
  \"org\": {\n\
    \"free\": [{\n\
        \"s1key_a\": \"val_a\",\n\
        \"s1key_b\": \"val_b\"\n\
      },{\n\
        \"s2key_c\": \"val_c\",\n\
        \"s2key_d\": \"val_d\"\n\
      }],\n\
    \"key_e\": \"val_e_yyy\",\n\
    \"key_f\": \"val_f\"\n\
  }\n\
}";

  if(verbose) fprintf( zout, "%s\n", json );

  oyjl_val value = 0;
  oyjl_val root = 0;
  char error_buffer[128];
  for(i = 0; i < 5; ++i)
  {
    int level = 0;
    const char * xpath = NULL;
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
      char * rjson = 0;
      oyjlTreeToJson( root, &level, &rjson );
      if(json && json[0] && strlen(rjson) == 210 && strcmp(json,rjson) == 0)
      { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(rjson),
        "oyjlTreeToJson()" );
        if(verbose) fprintf( zout, "%s\n", rjson );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlTreeToJson()" );
      }
      myDeAllocFunc(rjson);

      int count;
      char ** paths = oyjlTreeToPaths( root, 10, NULL, 0, &count );
      int j = 0;
      if( count == 10 &&
          strcmp(paths[j++],"org") == 0 &&
          strcmp(paths[j++],"org/free") == 0 &&
          strcmp(paths[j++],"org/free/[0]") == 0 &&
          strcmp(paths[j++],"org/free/[0]/s1key_a") == 0 &&
          strcmp(paths[j++],"org/free/[0]/s1key_b") == 0 &&
          strcmp(paths[j++],"org/free/[1]") == 0 &&
          strcmp(paths[j++],"org/free/[1]/s2key_c") == 0 &&
          strcmp(paths[j++],"org/free/[1]/s2key_d") == 0 &&
          strcmp(paths[j++],"org/key_e") == 0 &&
          strcmp(paths[j++],"org/key_f") == 0
        )
      { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
        "oyjlTreeToPaths()" );
      } else
      { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
        "oyjlTreeToPaths()" );
        for(j = 0; j < count; ++j)
          fprintf( zout, "paths[%d]: %s\n", j, paths[j]);
      }
      if(verbose)
        for(j = 0; j < count; ++j)
          fprintf( zout, "%d: %s\n", j, paths[j] );
      if(paths && count)
        oyjlStringListRelease( &paths, count, free );

      paths = oyjlTreeToPaths( root, 10, NULL, OYJL_KEY, &count );
      count = 0; while(paths && paths[count]) ++count;
      j = 0;
      if( count == 6 &&
          strcmp(paths[j++],"org/free/[0]/s1key_a") == 0 &&
          strcmp(paths[j++],"org/free/[0]/s1key_b") == 0 &&
          strcmp(paths[j++],"org/free/[1]/s2key_c") == 0 &&
          strcmp(paths[j++],"org/free/[1]/s2key_d") == 0 &&
          strcmp(paths[j++],"org/key_e") == 0 &&
          strcmp(paths[j++],"org/key_f") == 0
        )
      { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
        "oyjlTreeToPaths( OYJL_KEY )" );
      } else
      { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
        "oyjlTreeToPaths( OYJL_KEY )" );
      }
      const char * match = NULL;
      const char * xpath = "org///s2key_d";
      for(j = 0; j < count; ++j)
      {
        if(oyjlPathMatch( paths[j], xpath, 0 ))
          match = paths[j];
        if(verbose) fprintf( zout, "%d: %s\n", j, paths[j] );
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

      paths = oyjlTreeToPaths( root, 10, NULL, OYJL_PATH, &count );
      j = 0;
      if(count == 4 &&
          strcmp(paths[j++],"org") == 0 &&
          strcmp(paths[j++],"org/free") == 0 &&
          strcmp(paths[j++],"org/free/[0]") == 0 &&
          strcmp(paths[j++],"org/free/[1]") == 0
        )
      { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
        "oyjlTreeToPaths( OYJL_PATH )" );
      } else
      { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
        "oyjlTreeToPaths( OYJL_PATH )" );
      }
      if(verbose)
      for(j = 0; j < count; ++j)
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
      { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS,n, clck/(double)CLOCKS_PER_SEC,"key",
        "oyjlValueText(%s)", p );
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
      const char * xjson =
"{\n\
  \"s2key_c\": \"val_c\",\n\
  \"s2key_d\": \"val_d\"\n\
}";
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
      if( success &&
          ((i == 1 && strcmp(rjson, xjson) == 0) ||
           (i == 2 && strcmp(rjson, "\"val_c\"") == 0) ||
           (i == 3 && !rjson) ||
           (i == 4 && strlen(rjson) == 256)
          )
        )
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "oyjlTreeGetValue(flags=%d)", flags );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlTreeGetValue(flags=%d) %d %lu", flags, i, rjson?(unsigned long)strlen(rjson):0 );
      }
      if(verbose)
      fprintf( zout, "%s xpath \"%s\" %s\n", value?"found":"found not", xpath, success?"ok":"" );
      if(verbose)
        fprintf( zout, "%d %s\n", i, rjson?rjson:"---" );
      if(rjson) myDeAllocFunc(rjson);
      rjson = NULL;
      if(i == 4)
      {
        const char * new_tree = "{ \"root\": {\"embedded_key\": \"val\" } }";
        oyjl_val new_sub = oyjlTreeParse( new_tree, error_buffer, 128 );
        oyjl_val rv = oyjlTreeGetValue( root, OYJL_CREATE_NEW, xpath );
        oyjl_val nv = oyjlTreeGetValue( new_sub, 0, "root" );
        size_t size = sizeof( * rv );
        memcpy( rv, nv, size );
        memset( nv, 0, size );
        oyjlTreeToJson( root, &level, &rjson );
        if( strlen( rjson ) == 291 )
        { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
          "copy node" );
        } else
        { PRINT_SUB( oyjlTESTRESULT_FAIL,
          "copy node                          %lu", rjson?(unsigned long)strlen(rjson):0 );
        }
        if(verbose)
        fprintf( zout, "%s xpath \"%s\" %s\n", value?"found":"found not", xpath, success?"ok":"" );
        if(verbose)
          fprintf( zout, "%d %s\n", i, rjson?rjson:"---" );
        if(rjson) myDeAllocFunc(rjson);
        rjson = NULL;
        oyjlTreeFree(new_sub);
      }
      if(!root) oyjlTreeFree( value );
    }

    oyjlTreeFree( root );
  }

  char * non_json = NULL;
  oyjlStringAdd( &non_json, 0,0, "%s,(nonsense}]", json );
  root = oyjlTreeParse( non_json, error_buffer, 128 );
  if( !root )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeParse(non_json)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyjlTreeParse(non_json)" );
  }
  if(verbose)
    fprintf( zout, "%s\n", non_json );
  free(non_json); non_json = NULL;
  oyjlTreeFree( root );
  root = NULL;

  result = testEscapeJson( "Color", "Color", OYJL_NO_BACKSLASH, 44, result, oyjlTESTRESULT_FAIL );
  result = testEscapeJson( "Color \"Rose\"", "Color \"Rose\"", OYJL_NO_BACKSLASH, 53, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJson( "Color \"Rose\"", "Color \"Rose\"", 0, 53, result, oyjlTESTRESULT_XFAIL );

  result = testEscapeJson( "2. Color", "2\\. Color", OYJL_NO_BACKSLASH | OYJL_REGEXP, 47, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJson( "2. Color", "2\\. Color", OYJL_REGEXP, 47, result, oyjlTESTRESULT_XFAIL );

  result = testEscapeJson( "Color [3]", "Color \\\\[3]", OYJL_NO_BACKSLASH | OYJL_NO_INDEX, 48, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJson( "Color [3]", "Color [3]", 0, 38, result, oyjlTESTRESULT_XFAIL );

  result = testEscapeJson( "Color(1)", "Color\\(1\\)", OYJL_REGEXP, 47, result, oyjlTESTRESULT_XFAIL );

  result = testEscapeJsonVal( "my.value", "my\\.value", OYJL_REGEXP, 25, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJsonVal( "my.value", "my.value", 0, 23, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJsonVal( "my\\.value", "my\\\\.value", OYJL_QUOTE | OYJL_NO_BACKSLASH, 27, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJsonVal( "my/value", "my%37value", OYJL_KEY, 25, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJsonVal( "value\nafter_line_break", "value\\nafter_line_break", 0, 39, result, oyjlTESTRESULT_XFAIL );

  const char * json2 = "{\n\
  \"org\": {\n\
    \"free\": [{\n\
        \"s1key_a\": null,\n\
        \"s1key_b\": \"matrix.from\"\n\
      },{\n\
        \"s2key_c\": 1.0,\n\
        \"s2key_d\": \"1.0\"\n\
      }],\n\
    \"key_e\": true,\n\
    \"key_f\": false\n\
  }\n\
}";
  root = oyjlTreeParse( json2, error_buffer, 128 );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "value\\.property", "data/key.property" );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "value.property", "data/key.property2" );
  char * text = NULL;
  int level = 0;
  oyjlTreeToJson( root, &level, &text );
  if(text && strlen( text ) == 293)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(text),
    "escape roundtrip" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(text),
    "escape roundtrip" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "key.property", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  oyjlTreeFree( root );
  root = oyjlTreeParse( text, error_buffer, 128 );
  char * tree_text = text; text = NULL;

  int size = 0,
      flags = verbose ? OYJL_OBSERVE : 0;
  value = oyjlTreeSerialise( root, flags, &size );
  oyjlTreeFree( root );
  if(size > 10 && memcmp( value, "oiJS", 4 ) == 0)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, size,
    "oyjlTreeSerialise() oiJS" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, size,
    "oyjlTreeSerialise() oiJS" );
  }
  oyjlWriteFile( "oiJS.json", value, size );
char *     oyjlTreeSerialisedPrint_  ( oyjl_val            v,
                                       int                 flags OYJL_UNUSED );
  text = oyjlTreeSerialisedPrint_( value, 0 );
  if(verbose)
    fprintf( zout, "parsed: \"%s\"\n", text );
  free(text); text = NULL;

  int count;
  char ** paths = oyjlTreeToPaths( value, 10000000, NULL, flags | OYJL_KEY, &count );
  int j = 0;
  if(count == 8 &&
      strcmp(paths[j++],"org/free/[0]/s1key_a") == 0 &&
      strcmp(paths[j++],"org/free/[0]/s1key_b") == 0 &&
      strcmp(paths[j++],"org/free/[1]/s2key_c") == 0 &&
      strcmp(paths[j++],"org/free/[1]/s2key_d") == 0
    )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
    "oyjlTreeToPaths( oiJS )" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
    "oyjlTreeToPaths( oiJS )" );
  }
  if(verbose)
    for(j = 0; j < count; ++j)
      fprintf( zout, "%d: %s\n", j, paths[j] );

const char * oyjlTreeGetString_      ( oyjl_val            v,
                                       int                 flags OYJL_UNUSED,
                                       const char        * path );
  j = 0;
  if( count == 8 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"null") == 0 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"matrix.from") == 0 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"1.0") == 0 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"1.0") == 0 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"true") == 0 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"false") == 0 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"value\\.property") == 0 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"value.property") == 0
    )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeGetString_( )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeGetString_( )" );
  }
  if(verbose)
    for(j = 0; j < count; ++j)
      fprintf( zout, "%s:%s\n", paths[j], oyjlTreeGetString_(value, 0, paths[j]) );

 if(paths && count)
    oyjlStringListRelease( &paths, count, free );

  root = oyjlTreeDeSerialise( value, flags, size );
  free(value);
  value = root; root = NULL;
  if(value->type == oyjl_t_object)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, size,
    "oyjlTreeDeSerialise( oiJS )" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, size,
    "oyjlTreeDeSerialise( oiJS )" );
  }

  paths = oyjlTreeToPaths( value, 10000000, NULL, flags | OYJL_KEY, &count );
  j = 0;
  if(count == 8 &&
      strcmp(paths[j++],"org/free/[0]/s1key_a") == 0 &&
      strcmp(paths[j++],"org/free/[0]/s1key_b") == 0 &&
      strcmp(paths[j++],"org/free/[1]/s2key_c") == 0 &&
      strcmp(paths[j++],"org/free/[1]/s2key_d") == 0
    )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
    "oyjlTreeToPaths( DeSerialised oiJS )" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
    "oyjlTreeToPaths( DeSerialised oiJS )" );
  }
  if(verbose)
    for(j = 0; j < count; ++j)
      fprintf( zout, "%d: %s\n", j, paths[j] );

  j = 0;
  if( count == 8 &&
      oyjlTreeGetValue( value, 0, paths[j++] )->type == oyjl_t_null &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"matrix.from") == 0 &&
      oyjlTreeGetValue( value, 0, paths[j++] )->type == oyjl_t_number &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"1.0") == 0 &&
      oyjlTreeGetValue( value, 0, paths[j++] )->type == oyjl_t_true &&
      oyjlTreeGetValue( value, 0, paths[j++] )->type == oyjl_t_false &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"value\\.property") == 0 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"value.property") == 0
    )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeGetString_( DeSerialised oiJS )" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeGetString_( DeSerialised oiJS )" );
  }
  if(verbose)
    for(j = 0; j < count; ++j)
      fprintf( zout, "%s:%s\n", paths[j], oyjlTreeGetString_(value, 0, paths[j]) );

 if(paths && count)
    oyjlStringListRelease( &paths, count, free );

  level = 0;
  oyjlTreeToJson( value, &level, &text );
  oyjlTreeFree( value );
  value = NULL;

  if(text && strlen( text ) == 293 && strcmp(tree_text,text) == 0)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(text),
    "oyjlTreeDeSerialise()" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(text),
    "oyjlTreeDeSerialise()" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeDeSerialise", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text); text = NULL;}
  if(tree_text) {free(tree_text); tree_text = NULL;}

  return result;
}

oyjlTESTRESULT_e testFromJson ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  const char * json = "{\"org\":{\"free\":[{\"s1key_a\":\"val_a\",\"s1key_b\":\"val.b\"},{\"@s2attribute_c\":\"val_c\",\"@text\":\"inner string d\"}],\"key_e\":\"val_e\",\"@attribute_f\":\"val_f\",\"dc.node\":{\"@text\":\"inner string g\",\"@attr_h\":\"val_h\"},\"val_i\":1234,\"val_j\":true,\"val_k\":false,\"val_l\":12.34,\"val_m\":[1,2,3,4.5]}}";
  char error_buffer[128];
  char * text = 0;
  int level = 0;
  int format;

  oyjl_val root = 0;

  root = oyjlTreeParse( json, error_buffer, 128 );

  oyjlTreeToJson( root, &level, &text );
  format = oyjlDataFormat(text);
  if(text && text[0] && strlen(text) > 20 && format == 7 && strlen(text) == 413)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(text),
    "oyjlTreeToJson()" );
    if(verbose) fprintf( zout, "%s\n", text );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(text),
    "oyjlTreeToJson()" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeToJson", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  myDeAllocFunc(text);

  text = NULL;
  oyjlTreeToYaml( root, &level, &text );
  format = oyjlDataFormat(text);
  if(text && text[0] && strlen(text) > 20 && format == 9 && strlen(text) == 312)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(text),
    "oyjlTreeToYaml()" );
    if(verbose) fprintf( zout, "%s\n", text );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(text),
    "oyjlTreeToYaml()" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeToYaml", "txt" )
  myDeAllocFunc(text);

  text = NULL;
  oyjlTreeToXml( root, &level, &text );
  format = oyjlDataFormat(text);
  if(text && text[0] && strlen(text) > 20 && format == 8 && strlen(text) == 443)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(text),
    "oyjlTreeToXml()" );
    if(verbose) fprintf( zout, "%s\n", text );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(text),
    "oyjlTreeToXml()" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeToXml", "txt" )
  myDeAllocFunc(text);

  oyjlTreeFree( root );

  return result;
}

oyjlTESTRESULT_e testJsonRoundtrip () /* Data Readers */
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  const char * json = "{ \"org\":{\"val\":true}}";
  json = "{\"org\":{\"free\":[{\"s1key_a\":\"val_a\",\"s1key_b\":\"val.b\"},{\"@s2attribute_c\":\"val_c\",\"@text\":\"inner string d\"}],\"key_e\":\"val_e\",\"@xmlns:xsi\":\"http://www.w3.org/2001/XMLSchema-instance\",\"@attribute_f\":\"val_f\",\"xsi:node\":{\"@text\":\"inner string g\",\"@attr_h\":\"val_h\"},\"val_i\":1234,\"val_j\":true,\"val_k\":false,\"val_l\":12.34,\"val_m\":[0.0,1,2,3,4.5]}}";
  char error_buffer[128] = {0};
  char * text = NULL,
       * text_to_xml = NULL,
       * text_from_xml = NULL;
  int level = 0;

  oyjl_val root = 0;

  oyjlWriteFile( "testJsonRoundtrip.json", json, strlen(json) );
  root = oyjlTreeParse( json, error_buffer, 128 );

  oyjlTreeToJson( root, &level, &text );
  if(text && text[0] && strlen(text) > 20)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(text),
    "oyjlTreeToJson()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeToJson()" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeParse", "txt" )

  oyjlTreeToXml( root, &level, &text_to_xml );
  oyjlTreeFree( root ); root = NULL;


#if defined(OYJL_HAVE_LIBXML2)
  root = oyjlTreeParseXml( text_to_xml, OYJL_NUMBER_DETECTION, error_buffer, 128 );
  level = 0;
  oyjlTreeToJson( root, &level, &text_from_xml );
  int text_from_xml_size = strlen(text_from_xml),
      text_size = strlen(text);
  if(text && text_from_xml && text_from_xml_size == text_size)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeToXml () <-> oyjlTreeParseXml () %ul<->%ul", text_from_xml_size, text_size );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeToXml () <-> oyjlTreeParseXml ()" );
    fprintf( zout, "%s\n", text );
    fprintf( zout, "%s\n", text_from_xml );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeParseXml", "txt" )
  oyjlTreeFree( root ); root = NULL;
  myDeAllocFunc(text_from_xml); text_from_xml = NULL;
  myDeAllocFunc(text_to_xml); text_to_xml = NULL;
  myDeAllocFunc(text); text = NULL;

  char * xml = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n\
<org xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" attribute_f=\"val_f\">\n\
  <free>\n\
    <s1key_a>val_a</s1key_a>\n\
    <s1key_b>val_b</s1key_b>\n\
  </free>\n\
  <free s2attribute_c=\"val_c\">inner string d</free>\n\
  <key_e>val_e</key_e>\n\
  <xsi:node attr_h=\"val_h\">inner string g</xsi:node>\n\
  <val_i>1234</val_i>\n\
  <val_j>1</val_j>\n\
  <val_k>0</val_k>\n\
  <val_l>12.34</val_l>\n\
  <val_m>0.0</val_m>\n\
  <val_m>1</val_m>\n\
  <val_m>2</val_m>\n\
  <val_m>3</val_m>\n\
  <val_m>4.5</val_m>\n\
</org>";
  oyjlWriteFile( "testJsonRoundtrip.xml", xml, strlen(xml) );
  root = oyjlTreeParseXml( xml, OYJL_NUMBER_DETECTION, error_buffer, 128 );
  level = 0;
  oyjlTreeToJson( root, &level, &text_from_xml );
  oyjlTreeFree( root ); root = NULL;
  root = oyjlTreeParse( text_from_xml, error_buffer, 128 );
  level = 0;
  oyjlTreeToXml( root, &level, &text_to_xml );
  oyjlTreeFree( root ); root = NULL;
  if(xml && text_to_xml && strlen(text_to_xml) == strlen(xml))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeParseXml () <-> oyjlTreeToXml ()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeParseXml () <-> oyjlTreeToXml ()" );
    fprintf( zout, "%s\n", xml );
    fprintf( zout, "%s\n", text_from_xml );
    fprintf( zout, "%s\n", text_to_xml );
  }
  OYJL_TEST_WRITE_RESULT( text_to_xml, strlen(text_to_xml), "oyjlTreeToXml", "txt" )
#endif
  myDeAllocFunc(text_from_xml); text_from_xml = NULL;
  myDeAllocFunc(text_to_xml); text_to_xml = NULL;
  myDeAllocFunc(text); text = NULL;

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
  char * t = oyjlReadCommandF( &size, "r", malloc, "pkg-config -libs-only-L openicc" );

  fprintf( zout, "compiling and testing: %s\n", oyjlTermColor(oyjlBOLD, prog) );

  if(c_source && len == code_size)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "oyjlUiJsonToCode(OYJL_SOURCE_CODE_C)" );
  } else
  { PRINT_SUB_INT( fail, len,
    "oyjlUiJsonToCode(OYJL_SOURCE_CODE_C)" );
  }
  OYJL_TEST_WRITE_RESULT( c_source, len, "oyjlUiJsonToCode", "txt" )
  if(verbose && c_source)
    fprintf( zout, "%s\n", c_source );
  oyjlStringAdd( &name, 0,0, "%s.c", prog );
  oyjlWriteFile( name, c_source, len );
  if(c_source) {free(c_source);} c_source = NULL;
  /* compile */
  if(!(t && t[0]))
    fprintf( zout, "Compiling without OpenICC\n" );
  if(lib_a_size)
    oyjlStringAdd( &command, 0,0, "c++ %s -g -O0 -I %s -I %s %s -L %s/oyjl-args-qml -loyjl-args-qml-static -lQt5DBus -lQt5Qml -lQt5Network -lQt5Widgets -lQt5Gui -lQt5Core -L %s -loyjl-static -loyjl-core-static %s %s -lyaml -lyajl -lxml2 -o %s", verbose?"-Wall -Wextra":"-Wno-write-strings", OYJL_SOURCEDIR, OYJL_BUILDDIR, name, OYJL_BUILDDIR, OYJL_BUILDDIR, t&&t[0]?t:"", t&&t[0]?"-lopenicc-static":"", prog );
  else if(lib_so_size)
    oyjlStringAdd( &command, 0,0, "cc %s -g -O0 -I %s -I %s %s -L %s -lOyjl -lOyjlCore -o %s", verbose?"-Wall -Wextra":"", OYJL_SOURCEDIR, OYJL_BUILDDIR, name, OYJL_BUILDDIR, prog );
  if(command)
  {
    if(verbose)
      fprintf( stderr, "compiling: %s\n", oyjlTermColor( oyjlBOLD, command ) );
    int r = system(command);
    if(command) {free(command); command = NULL;}
    int size = oyjlIsFile( prog, "r", info, 48 );
    if(!size || verbose)
    {
      fprintf(stderr, "%scompile: %s %s %d %d\n", size == 0?"Could not ":"", oyjlTermColor(oyjlBOLD,prog), info, size, r);
    }
  }
  if(name) {free(name); name = NULL;}
  if(t) {free(t); t = NULL;}

  c_source = oyjlUiJsonToCode( json, OYJL_COMPLETION_BASH );
  len = c_source ? strlen(c_source) : 0;
  if(len == bash_size)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "oyjlUiJsonToCode(OYJL_COMPLETION_BASH)" );
  } else
  { PRINT_SUB_INT( fail, len,
    "oyjlUiJsonToCode(OYJL_COMPLETION_BASH)" );
  }
  OYJL_TEST_WRITE_RESULT( c_source, strlen(c_source), "oyjlUiJsonToCode-Completion", "txt" )
  if(verbose && c_source)
    fprintf( zout, "%s\n", c_source );
  if(c_source) {free(c_source);} c_source = NULL;

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s --help", prog );
  len = t ? strlen(t) : 0;
  if(len == help_size)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "%s --help", prog );
  } else
  { PRINT_SUB_INT( fail, len,
    "%s --help", prog );
  }
  OYJL_TEST_WRITE_RESULT( t, len, prog, "txt" )
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

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X json", prog );
  len = t ? strlen(t) : 0;
  if(len == json_size)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "%s -X json", prog );
  } else
  { PRINT_SUB_INT( fail, len,
    "%s -X json", prog );
  }
  OYJL_TEST_WRITE_RESULT( t, len, prog, "json" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X json+command", prog );
  len = t ? strlen(t) : 0;
  if(len == json_command_size)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "%s -X json+command", prog );
  } else
  { PRINT_SUB_INT( fail, len,
    "%s -X json+command", prog );
  }
  OYJL_TEST_WRITE_RESULT( t, len, prog, "json" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X export", prog );
  len = t ? strlen(t) : 0;
  if(len == export_size)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "%s -X export", prog );
  } else
  { PRINT_SUB_INT( fail, len,
    "%s -X export", prog );
  }
  OYJL_TEST_WRITE_RESULT( t, len, prog, "json" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );

  char error_buffer[128] = {0};
  char * export_text = t; t = NULL;
  size_t export_len = len;
  oyjl_val root = oyjlTreeParse( export_text, error_buffer, 128 );

  oyjlUi_s * ui = oyjlUi_ImportFromJson( root, 0 );
  t = oyjlUi_ExportToJson( ui, 0 );
  len = t ? strlen(t) : 0;
  if(abs((int)len - (int)export_len) <= 1)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "oyjlUi_ImportFromJson()" );
  } else
  { PRINT_SUB_INT( fail, len,
    "oyjlUi_ImportFromJson()" );
  }
  if(ui) oyjlOptions_SetAttributes( ui->opts, NULL );
  oyjlUi_Release( &ui);
  OYJL_TEST_WRITE_RESULT( t, len, "oyjlUi_ImportFromJson", "txt" )
  if(verbose && t)
    fprintf( zout, "%s\n", t );

  oyjlStringAdd( &name, 0,0, "%s-import.json", prog );
  oyjlWriteFile( name, t, len );
  if(name) {free(name);} name = NULL;
  oyjlStringAdd( &name, 0,0, "%s-export.json", prog );
  oyjlWriteFile( name, t, len );
  if(name) {free(name);} name = NULL;
  if(t) {free(t);} t = NULL;
  t = oyjlReadCommandF( &size, "r", malloc, "diff -aur %s-export.json %s-import.json", prog, prog );
  if(verbose)
    fprintf( zout, "Checking diff: \"%s\"\n", t );
  if(!t || (t && strlen(t) == 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "%s roundtrip  of reimported JSON  %s", prog, t );
  } else
  { PRINT_SUB( fail,
    "%s roundtrip  of reimported JSON  %s", prog, t );
  }

  if(t) {free(t);} t = NULL;
  oyjlTreeFree( root ); root = NULL;
  if(export_text) {free(export_text);}


  return result;
}

#include "oyjl_i18n.h"
oyjlTESTRESULT_e testUiRoundtrip ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  const char * loc = setlocale(LC_ALL,"en_GB.UTF8");
  oyjlLang( loc );

  int output = 0;
  const char * file = NULL;
  const char * func = NULL;
  double number = 0.0;
  int file_count = 0;
  int show_status = 0;
  int help = 0;
  int verbose_ = 0;

  /* handle options */
  /* Select from *version*, *manufacturer*, *copyright*, *license*, *url*,
   * *support*, *download*, *sources*, *oyjl_module_author* and
   * *documentation* what you see fit. Add new ones as needed. */
  oyjlUiHeaderSection_s sections[] = {
    /* type, nick,            label, name,                  description  */
    {"oihs", "version",       NULL,  "1.0",                 NULL},
    {"oihs", "documentation", NULL,  "",                    _("The example tool demonstrates the usage of the libOyjl API's.")},
    {"oihs", "date",          NULL,  "2018-10-10T12:00:00", _("October 10, 2018")},
    {"",0,0,0,0}};

  /* declare some option choices */
  oyjlOptionChoice_s i_choices[] = {{"oyjl.json", _("oyjl.json"), _("oyjl.json"), ""},
                                    {"oyjl2.json", _("oyjl2.json"), _("oyjl2.json"), ""},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s o_choices[] = {{"0", _("Print All"), _("Print All"), ""},
                                    {"1", _("Print Camera"), _("Print Camera JSON"), ""},
                                    {"2", _("Print None"), _("Print None"), ""},
                                    {NULL,NULL,NULL,NULL}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags, o,   option,    key,  name,         description,         help, value_name,    value_type,               values,                                                          variable_type, output variable */
    {"oiwi", 0,     "#", "",        NULL, _("status"),  _("Show Status"),    NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &show_status} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,"@","",NULL,_("input"),_("Set Input"),NULL, _("FILENAME"), oyjlOPTIONTYPE_FUNCTION, {}, oyjlINT, {.i = &file_count} },
    {"oiwi", 0,     "i", "input",   NULL, _("input"),   _("Set Input"),      NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)i_choices, sizeof(i_choices), malloc )}, oyjlSTRING, {.s = &file} },
    {"oiwi", 0,     "o", "output",  NULL, _("output"),  _("Control Output"), NULL, "0|1|2",       oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)o_choices, sizeof(o_choices), malloc )}, oyjlINT, {.i = &output} },
    {"oiwi", 0,     "f", "function",NULL, _("function"),_("Function"),       NULL, _("FUNCTION"), oyjlOPTIONTYPE_FUNCTION, {}, oyjlSTRING, {.s = &func} },
    {"oiwi", 0,     "n", "number",  NULL, _("number"),  _("Number"),         NULL, _("NUMBER"),   oyjlOPTIONTYPE_DOUBLE, {.dbl.start = -1.0, .dbl.end = 1.0, .dbl.tick = 0.1, .dbl.d = number}, oyjlDOUBLE, {.d = &number} },
    {"oiwi", 0,     "h", "help",    NULL, _("help"),    _("Help"),           NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &help} },
    {"oiwi", 0,     "v", "verbose", NULL, _("verbose"), _("verbose"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &verbose_} },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups_no_args[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Mode1"),_("Simple mode"),     NULL, "#",       "o,v",    "o" }, /* accepted even if none of the mandatory options is set */
    {"oiwg", OYJL_OPTION_FLAG_EDITABLE,_("Mode2"),_("Any arg mode"),NULL,"@","o,v","@,o"},/* accepted if anonymous arguments are set */
    {"oiwg", 0,     _("Mode3"),_("Actual mode"),     NULL, "i",       "o,f,n,v",    "i,o,f,n" },/* parsed and checked with -i option */
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "",        "",       "v,h" },/* just show in documentation */
    {"",0,0,0,0,0,0,0}
  };

  const char * attr = "{\n\
  \"org\": {\n\
    \"freedesktop\": {\n\
      \"oyjl\": {\n\
        \"ui\": {\n\
          \"options\": {\n\
            \"type\": \"oiws\",\n\
            \"array\": [{},{\n\
                \"type\": \"oiwi\",\n\
                \"o\": \"@\",\n\
                \"values\": {\n\
                  \"getChoicesCompletionBash\": \"ls -1\"\n\
                }\n\
              },{},{},{\n\
                \"type\": \"oiwi\",\n\
                \"o\": \"f\",\n\
                \"values\": {\n\
                  \"getChoicesCompletionBash\": \"pwd\"\n\
                }\n\
              }]\n\
            }\n\
          }\n\
        }\n\
      }\n\
    }\n\
  }";
  char error_buffer[128] = {0};
  oyjl_val root = oyjlTreeParse( attr, error_buffer, 128 );

  const char * argv_anonymous[] = {"test","-v","file-name.json","file-name2.json"};
  int argc_anonymous = 4;
  /* declare options    type,   array,  groups,         user_data, argc,           argv,           private_data */
  oyjlOptions_s opts = {"oiws", oarray, groups_no_args, NULL,      argc_anonymous, argv_anonymous, NULL };
  {
    oyjlOptions_SetAttributes( &opts, &root );
  }
  oyjlUi_s * ui = oyjlUi_FromOptions( "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                  sections, &opts, NULL );
  char * text = oyjlUi_ExportToJson( ui, 0 );
  if(text && strlen(text) == 8689)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, text?strlen(text):0,
    "oyjlUi_ExportToJson()" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, text?strlen(text):0,
    "oyjlUi_ExportToJson()" );
  }
  oyjlOptions_SetAttributes( &opts, NULL );
  oyjlUi_Release( &ui);
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ExportToJson", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );

  oyjl_val json = oyjlTreeParse( text, error_buffer, 128 );
  if(text) {free(text);} text = NULL;

  result = testCode( json, "oiCR"                    /*prog*/,
                           9996                      /*code_size*/,
                           1284                      /*help_size*/,
                           2174                      /*man_size*/,
                           4316                      /*markdown_size*/,
                           8023                      /*json_size*/,
                           8050                      /*json_command_size*/,
                           11682                     /*export_size*/,
                           4144                      /*bash_size*/,
                           result,
                           oyjlTESTRESULT_FAIL       /*fail*/ );


  oyjlTreeFree( json ); json = NULL;

  free(oarray[2].values.choices.list);
  free(oarray[3].values.choices.list);

  return result;
}

oyjlTESTRESULT_e testUiTranslation ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  oyjlTr_s * trc = NULL; oyjlTr_Unset( OYJL_DOMAIN );
  const char * loc = setlocale(LC_ALL,"en_GB.UTF8");
  oyjlLang( loc );

  int output = 0;
  const char * file = NULL;
  int file_count = 0;
  int show_status = 0;
  int help = 0;
  int verbose_ = 0;
  const char * catalog_json = NULL;

  /* handle options */
  /* Select from *version*, *manufacturer*, *copyright*, *license*, *url*,
   * *support*, *download*, *sources*, *oyjl_module_author* and
   * *documentation* what you see fit. Add new ones as needed. */
  oyjlUiHeaderSection_s sections[] = {
    /* type, nick,            label, name,                  description  */
    {"oihs", "version",       NULL,  "1.0",                 NULL},
    {"oihs", "documentation", NULL,  "",                    _("The example tool demonstrates the usage of the libOyjl API's.")},
    {"oihs", "date",          NULL,  "2018-10-10T12:00:00", _("October 10, 2018")},
    {"",0,0,0,0}};

  /* declare some option choices */
  oyjlOptionChoice_s i_choices[] = {{"oyjl.json", _("oyjl.json"), _("oyjl.json"), ""},
                                    {"oyjl2.json", _("oyjl2.json"), _("oyjl2.json"), ""},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s o_choices[] = {{"0", _("Print All"), _("Print All"), ""},
                                    {"1", _("Print Camera"), _("Print Camera JSON"), ""},
                                    {"2", _("Print None"), _("Print None"), ""},
                                    {NULL,NULL,NULL,NULL}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags, o,   option,    key,  name,         description,         help, value_name,    value_type,               values,                                                          variable_type, output variable */
    {"oiwi", 0,     "#", "",        NULL, _("status"),  _("Show Status"),    NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &show_status} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,"@","",NULL,_("input"),_("Set Input"),NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {}, oyjlINT, {.i = &file_count} },
    {"oiwi", 0,     "i", "input",   NULL, _("input"),   _("Set Input"),      NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)i_choices, sizeof(i_choices), malloc )}, oyjlSTRING, {.s = &file} },
    {"oiwi", 0,     "o", "output",  NULL, _("output"),  _("Control Output"), NULL, "0|1|2",       oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)o_choices, sizeof(o_choices), malloc )}, oyjlINT, {.i = &output} },
    {"oiwi", 0,     "h", "help",    NULL, _("help"),    _("Help"),           NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &help} },
    {"oiwi", 0,     "v", "verbose", NULL, _("verbose"), _("verbose"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &verbose_} },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups_no_args[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Mode(1)"),_("Simple mode \"(1)\""), NULL, "#", "o,v",    "o" }, /* accepted even if none of the mandatory options is set */
    {"oiwg", OYJL_OPTION_FLAG_EDITABLE,_("Mode2"),_("Any arg mode"),NULL,"@","o,v","@,o"},/* accepted if anonymous arguments are set */
    {"oiwg", 0,     _("Mode3"),_("Actual mode"),     NULL, "i",       "o,v",    "i,o" },/* parsed and checked with -i option */
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "",        "h",      "h,v" },/* just show in documentation */
    {"",0,0,0,0,0,0,0}
  };

  const char * argv_anonymous[] = {"test","-v","file-name.json","file-name2.json"};
  int argc_anonymous = 4;
  oyjlUi_s * ui = oyjlUi_Create( argc_anonymous, argv_anonymous, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups_no_args, NULL );
  char * text = oyjlUi_ToJson( ui, 0 );
  if(text && strlen(text) == 7971)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, text?strlen(text):0,
    "oyjlUi_ToJson(en_GB)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, text?strlen(text):0,
    "oyjlUi_ToJson(en_GB)" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToJson-en_GB", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  text = oyjlUi_ExportToJson( ui, 0 );
  if(text && strlen(text) == 6689)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, text?strlen(text):0,
    "oyjlUi_ExportToJson(en_GB)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, text?strlen(text):0,
    "oyjlUi_ExportToJson(en_GB)" );
  }
  oyjlUi_s * ui_en = oyjlUi_Copy( ui );
  oyjlUi_Release( &ui);
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ExportToJson-en_GB", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );

  char error_buffer[128] = {0};
  oyjl_val json = oyjlTreeParse( text, error_buffer, 128 );
  if(text) {free(text);} text = NULL;

  result = testCode( json, "oiCR_enGB"               /*prog*/,
                           9052                      /*code_size*/,
                           1187                      /*help_size*/,
                           2075                      /*man_size*/,
                           3874                      /*markdown_size*/,
                           7441                      /*json_size*/,
                           7473                      /*json_command_size*/,
                           10606                     /*export_size*/,
                           3150                      /*bash_size*/,
                           result,
                           oyjlTESTRESULT_FAIL       /*fail*/ );

  oyjlTreeFree( json ); json = NULL;

  text = oyjlUi_ExportToJson( ui_en, 0 );
  if(text && strlen(text) == 6689)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, text?strlen(text):0,
    "oyjlUi_ExportToJson(copy)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, text?strlen(text):0,
    "oyjlUi_ExportToJson(copy)" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ExportToJson-copy", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;
  oyjlUi_Release( &ui_en );

  catalog_json = "{\n\
  \"org\": {\n\
    \"freedesktop\": {\n\
      \"oyjl\": {\n\
        \"translations\": {\n\
          \"cs_CZ.UTF8\": {\n\
            \"Help\": \"Nápověda\",\n\
            \"Example\": \"Příklad\",\n\
            \"FILENAME\": \"JMENO_SOUBORU\",\n\
            \"Mode(1)\": \"Režimu(1)\",\n\
            \"Mode3\": \"Režimu3\",\n\
            \"General options\": \"Obecné volby\",\n\
            \"Print All\": \"Tisknout vše\",\n\
            \"Print Camera JSON\": \"Tisk fotoaparátu JSON\",\n\
            \"Print None\": \"Tisk žádný\",\n\
            \"verbose\": \"upovídaný výstup\"\n\
          },\n\
          \"de_DE.UTF8\": {\n\
            \"Example\": \"Beispiel\",\n\
            \"The example tool demonstrates the usage of the libOyjl API's.\": \"Das Beispielwerkzeug zeigt die Benutzung der libOyjl APIs.\",\n\
            \"help\": \"hilfe\",\n\
            \"increase verbosity\": \"mehr Infos\",\n\
            \"Help\": \"Hilfe\",\n\
            \"Actual mode\": \"Aktueller Modus\",\n\
            \"Control Output\": \"Ausgabeneinstellungen\",\n\
            \"FILENAME\": \"DATEINAME\",\n\
            \"General options\": \"Allgemeine Optionen\",\n\
            \"Misc\": \"Verschiedenes\",\n\
            \"Mode(1)\": \"Modus(1)\",\n\
            \"Mode3\": \"Modus3\",\n\
            \"October 10, 2018\": \"10. Oktober 2018\",\n\
            \"Print All\": \"Zeige Alle\",\n\
            \"Print Camera JSON\": \"Zeige Kamera JSON\",\n\
            \"Print None\": \"Zeige Nichts\",\n\
            \"Set Input\": \"Setze Eingabe\",\n\
            \"Short example tool using libOyjl\": \"Einfaches Beispielwerkzeug mit libOyjl\",\n\
            \"Show Status\": \"Zeige Status\",\n\
            \"Simple mode \\\"(1)\\\"\": \"Einfacher Modus \\\"(1)\\\"\"\n\
          },\n\
          \"de_DE\": {\n\
            \"Example2\": \"Beispiel2\"\n\
          },\n\
          \"de_AT\": {\n\
            \"Nonsense\": \"Schmarrn\"\n\
          },\n\
          \"de\": {\n\
            \"Color\": \"Farbe\"\n\
          }\n\
        }\n\
      }\n\
    }\n\
  }\n\
}";
  oyjl_val catalog = oyjlTreeParse( catalog_json, NULL, 0 );
  trc = oyjlTr_New( "C", OYJL_DOMAIN, &catalog, NULL,NULL,NULL, !verbose?0:OYJL_OBSERVE );
  oyjlTr_Set( &trc );
  oyjlLang( "de_DE" );

  ui = oyjlUi_Create( argc_anonymous, argv_anonymous, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups_no_args, NULL );
  text = oyjlUi_ToJson( ui, 0 );
  if(text && strlen(text) == 7977)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, text?strlen(text):0,
    "oyjlUi_ToJson(de)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, text?strlen(text):0,
    "oyjlUi_ToJson(de)" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToJson-de", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  text = oyjlUi_ExportToJson( ui, 0 );
  if(text && strlen(text) == 6695)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, text?strlen(text):0,
    "oyjlUi_ExportToJson(de)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, text?strlen(text):0,
    "oyjlUi_ExportToJson(de)" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ExportToJson-de", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );

  error_buffer[0] = 0;
  json = oyjlTreeParse( text, error_buffer, 128 );
  if(text) {free(text);} text = NULL;

  char * c_source = oyjlUiJsonToCode( json, OYJL_SOURCE_CODE_C );
  if(c_source && strlen(c_source) == 9058)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, c_source?strlen(c_source):0,
    "oyjlUiJsonToCode(de)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, c_source?strlen(c_source):0,
    "oyjlUiJsonToCode(de)" );
  }
  OYJL_TEST_WRITE_RESULT( c_source, strlen(c_source), "oyjlUiJsonToCode-de", "txt" )
  if(verbose && c_source)
    fprintf( zout, "%s\n", c_source );
  if(c_source) {free(c_source);} c_source = NULL;

  oyjlTreeFree( json ); json = NULL;

  catalog = oyjlTreeParse( catalog_json, NULL, 0 );
  trc = oyjlTr_New( "de_DE", OYJL_DOMAIN, &catalog, NULL,NULL,NULL, !verbose?0:OYJL_OBSERVE );
  oyjlUi_Translate( ui, trc );
  text = oyjlUi_ToJson( ui, 0 );
  if(text && strlen(text) == 8050)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, text?strlen(text):0,
    "oyjlUi_ToJson(de+)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, text?strlen(text):0,
    "oyjlUi_ToJson(de+)" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToJson-de", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  oyjlTr_SetLocale( trc, "back" );
  oyjlUi_Translate( ui, trc );
  text = oyjlUi_ToJson( ui, 0 );
  if(text && strlen(text) == 7971)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, text?strlen(text):0,
    "oyjlUi_ToJson(back)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, text?strlen(text):0,
    "oyjlUi_ToJson(back)" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToJson-back", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  text = oyjlUi_ExportToJson( ui, 0 );
  if(text && strlen(text) == 6689)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, text?strlen(text):0,
    "oyjlUi_ExportToJson(back)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, text?strlen(text):0,
    "oyjlUi_ExportToJson(back)" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ExportToJson-back", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  oyjlTr_SetLocale( trc, "cs_CZ" );
  oyjlUi_Translate( ui, trc );
  text = oyjlUi_ToJson( ui, 0 );
  if(text && strlen(text) == 8082)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, text?strlen(text):0,
    "oyjlUi_ToJson(cs)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, text?strlen(text):0,
    "oyjlUi_ToJson(cs)" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToJson-cs", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  text = oyjlUi_ExportToJson( ui, 0 );
  if(text && strlen(text) == 6752)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, text?strlen(text):0,
    "oyjlUi_ExportToJson(cs)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, text?strlen(text):0,
    "oyjlUi_ExportToJson(cs)" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ExportToJson-cs", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  int level = 0;
  catalog = oyjlTr_GetCatalog( oyjlTr_Get(OYJL_DOMAIN) );
  oyjlTreeToJson( catalog, &level, &text );
  //puts(text);
  if(text && strlen(text) == 1797 && strlen(catalog_json) == 1797)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "catalog: %lu catalog_json: %lu", strlen(text), strlen(catalog_json)  );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "catalog: %lu catalog_json: %lu", strlen(text), strlen(catalog_json) );
  }
  if(oy_test_last_result != oyjlTESTRESULT_SUCCESS || verbose)
  {
    char * name = NULL, * t = NULL;
    int size = 0;
    oyjlStringAdd( &name, 0,0, "catalog_parsed.json" );
    oyjlWriteFile( name, text, strlen(text) );
    if(name) {free(name);} name = NULL;
    oyjlStringAdd( &name, 0,0, "catalog_json.json" );
    oyjlWriteFile( name, catalog_json, strlen(catalog_json) );
    if(name) {free(name);} name = NULL;
    if(t) {free(t);} t = NULL;
    t = oyjlReadCommandF( &size, "r", malloc, "diff -aur catalog_json.json catalog_parsed.json" );
    fprintf( zout, "%s\n", t );
    if(t) {free(t);} t = NULL;
  }
  if(text) {free(text);} text = NULL;

  oyjlUi_Release( &ui);
  oyjlTr_Release( &trc );

  free(oarray[2].values.choices.list);
  free(oarray[3].values.choices.list);

  return result;
}

typedef struct {
  const char * command; /* command line to exercise and read back in with oyjlReadCommandF() */
  size_t       result;  /* size of the expected result of command */
  const char * string;  /* content of the expected result of command */
  const char * print;   /* print instead of showing plain command line */
} oyjl_command_test_s;

oyjlTESTRESULT_e   testTool          ( const char        * prog,
                                       size_t              help_size,
                                       oyjl_command_test_s*commands,
                                       int                 count,
                                       oyjlTESTRESULT_e    result,
                                       oyjlTESTRESULT_e    fail )
{
  size_t len;
  int size;
  char info[48];

  fprintf( zout, "testing: %s  %d tests\n", oyjlTermColor(oyjlBOLD, prog), count );

  char * command = NULL, *t;
  oyjlStringAdd( &command, 0,0, "%s/%s", OYJL_BUILDDIR, prog );
  if(command)
  {
    if(verbose)
      fprintf( stderr, "detecting: %s\n", oyjlTermColor( oyjlBOLD, command ) );
    size = oyjlIsFile( command, "r", info, 48 );
    if(!size || verbose)
    {
      fprintf(stderr, "%sread: %s %s %d\n", size == 0?"Could not ":"", oyjlTermColor(oyjlBOLD,prog), info, size);
    }
    free(command);
    command = NULL;
  }

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C PATH=%s %s --help", OYJL_BUILDDIR, prog );
  len = t ? strlen(t) : 0;
  if(len == help_size)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "%s --help", prog );
  } else
  { PRINT_SUB_INT( fail, len,
    "%s --help", prog );
  }
  OYJL_TEST_WRITE_RESULT( t, len, prog, "txt" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  int i;
  for(i = 0; i < count; ++i)
  {
    char * cmd = NULL;
    oyjl_command_test_s * command = &commands[i];
    const char * args = command->command;
    size_t result_size = command->result;
    char * result_string = oyjlStringCopy( command->string, 0 );
    const char * print = command->print;

    oyjlStringAdd( &cmd, 0,0, "%s %s", prog, args );

    if(verbose)
      fprintf( stderr, "cmd: %s : %ld\n", cmd, result_size );

    t = oyjlReadCommandF( &size, "r", malloc, "LANG=C PATH=%s:$PATH %s", OYJL_BUILDDIR, cmd );
    len = t ? strlen(t) : 0;
    if(len && t[len-1] == '\n' && result_string && result_string[0] && result_string[strlen(result_string)-1] != '\n')
      oyjlStringAdd( &result_string, 0,0, "\n" );
    if( len == result_size &&
        (!result_string ||
         (result_string && t && strcmp(t, result_string) == 0)) )
    {
      if(result_string && len && result_string[len-1] == '\n') result_string[len-1] = '\000';
      PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
        "%s%s%s%s%s", print?print:cmd, result_string&&t?" = ":"", result_string&&t?"\"":"", result_string&&t?result_string:"", result_string&&t?"\"":"" );
    } else
    { PRINT_SUB( fail,
      "%s  %lu %s", print?print:cmd, len, result_string&&t?result_string:"" );
    }
    OYJL_TEST_WRITE_RESULT( t, len, prog, "txt" )
    if(verbose && len)
      fprintf( zout, "%s\n", t );
    if(t) {free(t);}

    if(cmd) {free(cmd); cmd = NULL;}
    if(result_string) {free(result_string); result_string = NULL;}
  }

  return result;
}

oyjlTESTRESULT_e testToolOyjl ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  const char * json = "#define oyjl_json \"{\\\n\
  \\\"org\\\": {\\\n\
    \\\"freedesktop\\\": {\\\n\
      \\\"oyjl\\\": {\\\n\
        \\\"translations\\\": {\\\n\
          \\\"de_DE\\\": {\\\n\
            \\\"Help\\\": \\\"Hilfe\\\",\\\n\
            \\\"Action\\\": null,\\\n\
            \\\"[1] Square Brackets\\\": \\\"[1] Eckige Klammern\\\",\\\n\
            \\\".Dot\\\": \\\".Punkt\\\",\\\n\
            \\\"\\\\\\\"Apostroph\\\\\\\"\\\": \\\"\\\\\\\"Anführungszeichen\\\\\\\"\\\"\\\n\
          }\\\n\
        }\\\n\
      }\\\n\
    }\\\n\
  }\\\n\
}\"\n";
  oyjlWriteFile( "i18n_de_DE.c", json, strlen(json) );

  json = "{\n\
  \"org\": {\n\
    \"freedesktop\": {\n\
      \"oyjl\": {\n\
        \"translations\": {\n\
          \"cs_CZ\": {\n\
            \"Help\": \"Nápověda\",\n\
            \"Action\": null\n\
          }\n\
        }\n\
      }\n\
    }\n\
  }\n\
}";
  oyjlWriteFile( "i18n_cs_CZ.json", json, strlen(json) );


  oyjl_command_test_s commands_oyjl[] = {
    { "json -i i18n_de_DE.c -i i18n_cs_CZ.json",    432,  NULL,       NULL },
    { "yaml -i i18n_de_DE.c -i i18n_cs_CZ.json",    289,  NULL,       NULL },
    { "xml -i i18n_de_DE.c -i i18n_cs_CZ.json",     482,  NULL,       NULL },
    { "json -i i18n_de_DE.c -x '/////[1]' --set 'LongValue'",344,NULL,     NULL }, /* change a value */
    { "json -i i18n_de_DE.c > i18n.de_DE.json; cat i18n.de_DE.json",    337,  NULL, "convert from C to JSON" },
    { "json -i i18n.de_DE.json -w C --wrap-name oyjl > i18n.de_DE.c; cat i18n.de_DE.c",    410, NULL, "convert back to C" },
    { "count -i i18n_de_DE.c",                      2,    "1",        NULL },
    { "count -i i18n_de_DE.c -x '////'",            2,    "5",        NULL },
    { "key -i i18n_de_DE.c -x '////'",              6,    "de_DE",    NULL },
    { "type -i i18n_de_DE.c -x '/////'",            7,    "string",   NULL },
    { "paths -i i18n_de_DE.c -x '////'",            292,  NULL,       NULL },
    { "format -i i18n_cs_CZ.json",                  5,    "JSON",     NULL },
    { "format -i none.file",                        9,    "no input", NULL },
    { "-X man > oyjl.1 && cat oyjl.1",              5758, NULL,       NULL }
  };
  int count = 14;
  result = testTool( "oyjl", 3751/*help size*/, commands_oyjl, count, result, oyjlTESTRESULT_FAIL );

  return result;
}


oyjlTESTRESULT_e testToolOyjlTranslation ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  const char * po = "\
#: liboyranos_core/oyranos_core.c:505 tools/oyranos-device.c:671\n\
#: tools/oyranos-device.c:687 tools/oyranos-device.c:702\n\
#: tools/oyranos-device.c:730 tools/oyranos-device.c:772\n\
#: tools/oyranos-device.c:1155 tools/oyranos-device.c:1276\n\
#: tools/oyranos-device.c:1512 tools/oyranos-monitor-white-point.c:739\n\
#: tools/qcmsevents/qcmsevents.cpp:338\n\
#, c-format\n\
msgid \"!!! Error\"\n\
msgstr \"!!! Fehler\"\n\
\n\
#: i18n.c:1\n\
msgid \"Oyjl Translation\"\n\
msgstr \"Oyjl Übersetzung\"\n\
\n\
msgid \"Help\"\n\
msgstr \"Hilfe\"\n\
\n\
#: modules/color/modules/lcms2/oyranos_cmm_lcm2.c:4931\n\
msgid \"\"\n\
\"[0 - tricky with index], \"\n\
msgstr \"\"\n\
\"[0 - Indextrick].\"";

  oyjlWriteFile( "de.po", po, strlen(po) );
  if(verbose)
    fprintf( zout, "de.po:\n%s\n", po );

  oyjl_command_test_s commands_oyjl_translate[] = {
    { "-X export > oyjl-translate-ui.json && cat oyjl-translate-ui.json", 22517,  NULL,       NULL },
    { "-e -i oyjl-translate-ui.json -o i18n.c -f '_(\"%s\");\n' -k name,description,help && cat i18n.c", 3714,  NULL,       NULL },
#ifdef OYJL_USE_GETTEXT
    { "-a -i oyjl-translate-ui.json -o oyjl-translate-ui-i18n.json -k name,description,help -d oyjl -p locale -l=de_DE,cs_CZ && cat oyjl-translate-ui-i18n.json", 29697, NULL,       NULL },
#endif
    { "-V; xgettext --add-comments --keyword=gettext --flag=gettext:1:pass-c-format --keyword=_ --flag=_:1:pass-c-format --keyword=N_ --flag=N_:1:pass-c-format  --copyright-holder='Kai-Uwe Behrmann'  --msgid-bugs-address='ku.b@gmx.de' --from-code=utf-8 --package-name=i18n --package-version=1.0.0 -o i18n.pot i18n.c && cat i18n.pot", 6986,  NULL,       "xgettext ... i18n.c -> i18n.pot; hand translate -> de.po(prepared example)" },
    { "-c -i de.po --locale=de_DE -o i18n-de_DE.json && cat i18n-de_DE.json", 320, NULL,       NULL }
  };
  int count = 4;
#ifdef OYJL_USE_GETTEXT
  ++count;
#endif
  result = testTool( "oyjl-translate", 3631/*help size*/, commands_oyjl_translate, count, result, oyjlTESTRESULT_FAIL );

  return result;
}/* --- end actual tests --- */


