/** @file test.c
 *
 *  Oyranos is an open source Color Management System
 *
 *  Copyright (C) 2004-2024  Kai-Uwe Behrmann
 *
 *  @brief    Oyranos test suite
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2008/12/04
 */

#define OYJL_TEST_NAME "test"

#include "oyjl_version.h"

#ifdef USE_GCOV
#define TESTS_RUN \
  /* Oyjl */ \
  /* OyjlArgs */ \
  TEST_RUN( testArgsPrint, "Options print OyjlArgs", 1 ); \
  TEST_RUN( testArgsCheck, "Options checking OyjlArgs", 1 ); \
  TEST_RUN( testArgs, "Options handling OyjlArgs", 1 ); \
  TEST_RUN( testArgsValues, "Value converting OyjlArgs", 1 ); \
  /* Oyjl */ \
  TEST_RUN( testVersion, "Version matching", 1 ); \
  TEST_RUN( testI18NSetup, "Internationalisation Setup", 1 ); \
  TEST_RUN( testI18N, "Internationalisation", 1 ); \
  /* OyjlCore */ \
  TEST_RUN( testString, "String handling OyjlCore", 1 ); \
  TEST_RUN( testArgsCore, "Options handling OyjlCore", 1 ); \
  TEST_RUN( testTree, "Tree handling OyjlCore", 1 ); \
  TEST_RUN( testIO, "File handling OyjlCore", 1 ); \
  TEST_RUN( testFunction, "Funktions OyjlCore", 1 ); \
  /* Oyjl */ \
  /*TEST_RUN( testVersion, "Version matching", 1 ); \
  TEST_RUN( testI18N, "Internationalisation", 1 );*/ \
  TEST_RUN( testDataFormat, "Data Format Detection", 1 ); \
  TEST_RUN( testJson, "JSON handling", 1 ); \
  TEST_RUN( testJsonEscape, "JSON Escape handling", 1 ); \
  TEST_RUN( testFromJson, "Data Writers", 1 ); \
  TEST_RUN( testJsonRoundtrip, "Data Readers", 1 ); \
  TEST_RUN( testUiRoundtrip, "Ui Export", 1 ); \
  TEST_RUN( testUiTranslation, "Ui Translation", 1 ); \
  TEST_RUN( testToolOyjl, "Tool oyjl", 1 ); \
  TEST_RUN( testToolOyjlTranslation, "Tool oyjl-translation", 1 );
#include "oyjl.h"

#else
#define TESTS_RUN \
  TEST_RUN( testVersion, "Version matching", 1 ); \
  TEST_RUN( testI18NSetup, "Internationalisation Setup", 1 ); \
  TEST_RUN( testI18N, "Internationalisation", 1 ); \
  TEST_RUN( testDataFormat, "Data Format Detection", 1 ); \
  TEST_RUN( testJson, "JSON handling simple", 1 ); \
  TEST_RUN( testJsonEscape, "JSON Escape handling", 1 ); \
  TEST_RUN( testJson2, "JSON handling", 1 ); \
  TEST_RUN( testFromJson, "Data Writers", 1 ); \
  TEST_RUN( testJsonRoundtrip, "Data Readers", 1 ); \
  TEST_RUN( testUiRoundtrip, "Ui Export", 1 ); \
  TEST_RUN( testUiTranslation, "Ui Translation", 1 ); \
  TEST_RUN( testToolOyjl, "Tool oyjl", 1 ); \
  TEST_RUN( testToolOyjlTranslation, "Tool oyjl-translation", 1 );
#endif

void oyjlLibRelease();
#define OYJL_TEST_MAIN_SETUP  printf("\n    %s Program\n", oyjlTermColor(oyjlBOLD, "Oyjl Test"));

#include "oyjl.h"
#ifdef OYJL_HAVE_LIBXML2
#include <libxml/parser.h>
# define OYJL_TEST_MAIN_FINISH printf("\n    %s Program finished\n\n", oyjlTermColor(oyjlBOLD, "Oyjl Test")); oyjlLibRelease(); if(oyjl_print_sub) free(oyjl_print_sub); xmlCleanupParser();
#else
# define OYJL_TEST_MAIN_FINISH printf("\n    %s Program finished\n\n", oyjlTermColor(oyjlBOLD, "Oyjl Test")); oyjlLibRelease(); if(oyjl_print_sub) free(oyjl_print_sub);
#endif
#include "oyjl_test_main.h"

#ifdef USE_GCOV
# include "test-args.c"
# include "test-core.c"
#else
#endif
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>
#endif
#include "oyjl_i18n_internal.h"
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
                                       oyjlTranslation_s * tr_context,
                                       const char        * key_list,
                                       int                 count,
                                       double            * clock )
{
  char * txt = 0;
  int i;
  oyjl_val array[count];
  for(i = 0; i < count; ++i)
    array[i] = oyjlTreeParse2( json, 0, __func__, NULL );
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
oyjlTESTRESULT_e testI18NSetup()
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
    fprintf( zout, "LANG=%s LANGUAGE=%s\n", getenv("LANG"), getenv("LANGUAGE"));
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
#if defined(OYJL_USE_GETTEXT)
  use_gettext = 1;
#endif
  static int my_debug = 0;
  oyjlTranslation_s * trc = NULL;
  my_debug = verbose;
  if(loc)
  {
    int size = sizeof(liboyjl_i18n_oiJS);
    oyjl_val oyjl_catalog = (oyjl_val) oyjlStringAppendN( NULL, (const char*) liboyjl_i18n_oiJS, size, malloc );
    if(*oyjl_debug)
      oyjlMessage_p( oyjlMSG_INFO, 0, "loc: \"%s\" domain: \"%s\" catalog-size: %d", loc, OYJL_DOMAIN, size );
    trc = oyjlTranslation_New( loc, OYJL_DOMAIN, &oyjl_catalog, 0,0,0,0 );
  }
  oyjlInitLanguageDebug( "Oyjl", "OYJL_DEBUG", &my_debug, use_gettext, "OYJL_LOCALEDIR", OYJL_LOCALEDIR, &trc, oyjlMessageFunc );
  oyjlTranslation_Release( &trc );

  loc = setlocale(LC_ALL, "");
  if(loc && (strcmp(loc, "C") != 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "setlocale() initialised good %s", loc );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "setlocale() initialised failed %s", loc );
  }

  const char * text;
#if defined(OYJL_USE_GETTEXT)
  text = dgettext( OYJL_DOMAIN, "Example" );
  if(strcmp(text,"Beispiel") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "dgettext() good \"%s\"", text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "dgettext() failed \"%s\"", text );
    fprintf( zout, "LANG=%s LANGUAGE=%s\n", getenv("LANG"), getenv("LANGUAGE"));
  }
#endif

  return result;
}

oyjlTESTRESULT_e testI18N()
{
  const char * plain;
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  const char * loc;
  const char * text;
  oyjlTranslation_s * trc = NULL;

  oyjl_val catalog = NULL;
  int size;
  size = sizeof(liboyjl_i18n_oiJS);
  catalog = (oyjl_val) oyjlStringAppendN( NULL, (const char*) liboyjl_i18n_oiJS, size, malloc );
  loc = setlocale(LC_ALL,"de_DE.UTF8");
  trc = oyjlTranslation_New( loc, OYJL_DOMAIN, &catalog, 0,0,0,0 );
  oyjlTranslation_Set( &trc );
  oyjlLang( "de_DE.UTF8");


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
  catalog = oyjlTreeParse2( json, OYJL_NO_MARKUP, __func__, NULL );
  trc = oyjlTranslation_New( loc, OYJL_DOMAIN, &catalog, NULL,NULL,NULL, flags );
  oyjlTranslation_Set( &trc );
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
  trc = oyjlTranslation_Get(OYJL_DOMAIN);
  oyjlTranslation_SetFlags( trc, flags );
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
  oyjlTranslation_SetFlags( trc, flags );
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
  loc = oyjlLang("de_DE.UTF8");

  char * oyjl_export, * txt;
  size = 0;
  oyjl_export = txt = oyjlReadCommandF( &size, "r", malloc, "LANG=C PATH=%s:$PATH %s --export export", OYJL_BUILDDIR, "oyjl" );
  plain = oyjlTermColorToPlain(txt, 0);
  if(!txt || strlen(plain) != 27151)
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(plain),
    "LANG=C oyjl --export export" );
  }

  const char * key_list = "name,description,help,label,value_name";

  int count, i;
  double clck = oyjlClock();
  char ** paths = oyjlTreeToPaths( catalog, 10000000, NULL, OYJL_KEY, &count );
  clck = oyjlClock() - clck;
  count = 0; while(paths && paths[count]) ++count;
  if( count == 651 || 641 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS,1,clck/(double)CLOCKS_PER_SEC,"wr",
    "oyjlTreeToPaths(catalog) = %d", count );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
    "oyjlTreeToPaths(catalog)" );
  }
  txt = NULL;
  for(i = 0; i < count; ++i)
    oyjlStringAdd( &txt, 0,0, "%s\n", paths[i] );
  OYJL_TEST_WRITE_RESULT( txt, strlen(txt), "oyjlTreeToPaths", "txt" )
  free(txt); txt = NULL;

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
  trc = oyjlTranslation_New( loc, OYJL_DOMAIN, &catalog, NULL,NULL,NULL, !verbose?0:OYJL_OBSERVE );
  clck = oyjlClock();
  for( i = 0; i < 100; ++i )
    text = oyjlTranslate( trc, "render" );
  clck = oyjlClock() - clck;
  if( strcmp(text,"Darstellung") == 0 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"tr",
    "oyjlTranslate(\"%s\",%s,\"render\") %s", loc, name, oyjlTranslation_GetLang( trc ) );
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
    "oyjlTranslate(\"%s\",%s,\"render\") %s", loc, name, oyjlTranslation_GetLang( trc ) );
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
    "oyjlTranslate(\"%s\",%s,\"render\") %s", loc, name, oyjlTranslation_GetLang( trc ) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTranslate(\"%s\",%s,\"render\")", loc, name );
  }

  loc = "cs";
  oyjlTranslation_SetLocale( trc, loc );
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
  size_t test_translate_json = 27573;
  loc = "de_DE";
  oyjlTranslation_SetLocale( trc, loc );
  txt = testTranslateJson( oyjl_export, trc, key_list, n, &clck );
  i = 0;
  plain = oyjlTermColorToPlain(txt, 0);
  if( txt && strlen( plain ) == test_translate_json )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS,n,clck/(double)CLOCKS_PER_SEC,"JS",
    "oyjlTranslateJson(\"%s\",%s) %s", loc, name, oyjlTranslation_GetLang( trc ) );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(plain),
    "oyjlTranslateJson(\"%s\",%s) %s", loc, name, oyjlTranslation_GetLang( trc ) );
  }
  OYJL_TEST_WRITE_RESULT( txt, strlen(txt), "oyjlTranslateJson", "txt" )
  if(verbose && txt)
    fprintf( zout, "%s\n", txt );
  myDeAllocFunc( txt ); txt = NULL;

  oyjlTranslation_Release( &trc );

  //return result;

#if defined(OYJL_USE_GETTEXT) && !defined(USE_GCOV)
  n = 100000;
  name = "gettext";
  loc = "de_DE.UTF8";
  setlocale( LC_ALL, loc );
  trc = oyjlTranslation_New( loc, OYJL_DOMAIN, NULL, NULL,NULL,NULL, OYJL_GETTEXT | (!verbose?0:OYJL_OBSERVE) );
  clck = oyjlClock();
  for( i = 0; i < n; ++i )
    text = oyjlTranslate( trc, "render" );
  clck = oyjlClock() - clck;
  if( strcmp(text,"Darstellung") == 0 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"tr",
    "oyjlTranslate(\"%s\",%s,\"render\")", loc, name );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyjlTranslate(\"%s\",%s,\"render\")", loc, name );
  }
  loc = "de_DE";
  oyjlTranslation_SetLocale( trc, loc );
  clck = oyjlClock();
  for( i = 0; i < n; ++i )
    text = oyjlTranslate( trc, "render" );
  clck = oyjlClock() - clck;
  if( strcmp(text,"Darstellung") == 0 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"tr",
    "oyjlTranslate(\"%s\",%s,\"render\")", loc, name );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyjlTranslate(\"%s\",%s,\"render\")", loc, name );
  }
  loc = "de";
  oyjlTranslation_SetLocale( trc, loc );
  clck = oyjlClock();
  for( i = 0; i < n; ++i )
    text = oyjlTranslate( trc, "render" );
  clck = oyjlClock() - clck;
  if( strcmp(text,"Darstellung") == 0 )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS, i,clck/(double)CLOCKS_PER_SEC,"tr",
    "oyjlTranslate(\"%s\",%s,\"render\")", loc, name );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "oyjlTranslate(\"%s\",%s,\"render\")", loc, name );
  }

  n = 1;
  loc = "de_DE";
  oyjlTranslation_SetLocale( trc, loc );
  txt = testTranslateJson( oyjl_export, trc, key_list, n, &clck );
  plain = oyjlTermColorToPlain(txt, 0);
  if( txt && strlen( plain ) == test_translate_json )
  { PRINT_SUB_PROFILING( oyjlTESTRESULT_SUCCESS,n,clck/(double)CLOCKS_PER_SEC,"JS",
    "oyjlTranslateJson(gettext)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_XFAIL, strlen(plain),
    "oyjlTranslateJson(gettext)" );
  }
  OYJL_TEST_WRITE_RESULT( txt, strlen(txt), "oyjlTranslateJson", "txt" )
  if(/*oy_test_last_result == oyjlTESTRESULT_FAIL || oy_test_last_result == oyjlTESTRESULT_XFAIL ||*/ verbose)
    fprintf( zout, "%s\n", txt );
  myDeAllocFunc( txt ); txt = NULL;
  oyjlTranslation_Release( &trc );
#endif

#define printtime oyjlPrintTime(OYJL_BRACKETS, oyjlNO_MARK)
#define timeformat "%s "
  fprintf( zout, timeformat "before oyjlTreeParse2(liboyjl_i18n_oiJS)\n", printtime);
  oyjl_val root = oyjlTreeParse2( oyjl_export, 0, __func__, NULL );
  fprintf( zout, timeformat "before while\n", printtime );
  size = sizeof(liboyjl_i18n_oiJS);
  catalog = (oyjl_val) oyjlStringAppendN( NULL, (const char*) liboyjl_i18n_oiJS, size, malloc );
  loc = "de_DE";
  trc = oyjlTranslation_New( loc, OYJL_DOMAIN, &catalog, NULL, NULL,NULL, flags );
  fprintf( zout, timeformat "after while; before oyjlTranslateJson\n", printtime );
  oyjlTranslateJson( root, trc, key_list );
  fprintf( zout, timeformat "after oyjlTranslateJson\n", printtime );
  oyjlTranslation_Release( &trc );
  i = 0;
  txt = NULL;
  oyjlTreeToJson( root, &i, &txt );
  plain = oyjlTermColorToPlain(txt, 0);
  if( txt && strlen( plain ) == test_translate_json )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(plain),
    "oyjlTranslateJson(oyjl)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(plain),
    "oyjlTranslateJson(oyjl)" );
  }
  OYJL_TEST_WRITE_RESULT( txt, strlen(txt), "oyjlTranslateJson", "txt" )
  if(verbose && txt)
    fprintf( zout, "%s\n", txt );
  myDeAllocFunc( txt ); txt = NULL;
  oyjlTreeFree( root );

  size = 0;
  flags = verbose ? OYJL_OBSERVE : 0;
  catalog = oyjlTreeParse2( json, OYJL_NO_MARKUP, __func__, NULL );
  oyjl_val static_catalog = oyjlTreeSerialise( catalog, flags, &size );
  if(size == 928 && oyjlStringStartsWith( (const char*)static_catalog, "oiJS", 0 ))
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
      strcmp(paths[j++],"org/freedesktop/oyjl/translations/de_DE.UTF8/Example") == 0 &&
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
  trc = oyjlTranslation_New( loc, OYJL_DOMAIN, &static_catalog, NULL, NULL,NULL, flags );
  oyjlTranslation_Set( &trc );
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
  oyjlTranslation_Release( &trc );

  size = sizeof(liboyjl_i18n_oiJS);
  catalog = (oyjl_val) oyjlStringAppendN( NULL, (const char*) liboyjl_i18n_oiJS, size, malloc );
  trc = oyjlTranslation_New( "C", OYJL_DOMAIN, &catalog, NULL,NULL,NULL, !verbose?0:OYJL_OBSERVE );
  oyjlTranslation_Set( &trc );
  loc = setlocale(LC_ALL,"de_DE.UTF8");
  loc = oyjlLang("de_DE.UTF8");
  root = oyjlTreeParse2( oyjl_export, 0, __func__, NULL );
  fprintf( zout, timeformat "before while\n", printtime );
  size = 0; while(size < 1000) ++size;
  fprintf( zout, timeformat "after while; before oyjlTranslateJson\n", printtime );
  oyjlTranslateJson( root, oyjlTranslation_Get(OYJL_DOMAIN), key_list );
  fprintf( zout, timeformat "after oyjlTranslateJson\n", printtime );
  i = 0;
  txt = NULL;
  oyjlTreeToJson( root, &i, &txt );
  plain = oyjlTermColorToPlain(txt, 0);
  if( txt && strlen( plain ) == test_translate_json )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(plain),
    "oyjlTranslateJson(oyjl, static_catalog)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(plain),
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
    "oyjlDataFormat (%s) = %d/\"%s\" guessed", data?data:"NULL", format, oyjlDataFormatToString(format) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (%s) = %d/\"%s\" guessed", data?data:"NULL", format, oyjlDataFormatToString(format) );
  }

  data = "";
  format = oyjlDataFormat(data);
  if(format == -2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  }

  data = "  \t ";
  format = oyjlDataFormat(data);
  if(format == -2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  }

  data = "\t XYZ";
  format = oyjlDataFormat(data);
  if(format == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  }

  data = " {\"a\":}";
  format = oyjlDataFormat(data);
  if(format == 7)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  }

  data = " [{\"a\":}]";
  format = oyjlDataFormat(data);
  if(format == 7)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  }

  data = "  <?xml attr=\"XX\"><p>";
  format = oyjlDataFormat(data);
  if(format == 8)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  }

  data = "  <p>XYZ";
  format = oyjlDataFormat(data);
  if(format == 8)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  }

  data = "  \n---\na:";
  format = oyjlDataFormat(data);
  if(format == 9)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  }

  data = "  \n#include \"oyjl.h\"\nint i;";
  format = oyjlDataFormat(data);
  if(format == 10)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  }

  data = "---a:";
  format = oyjlDataFormat(data);
  if(format == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  }

  data = "  ---\na:";
  format = oyjlDataFormat(data);
  if(format == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d/\"%s\" guessed", data, format, oyjlDataFormatToString(format) );
  }

  return result;
}

#define end_example

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
  const char * plain;
  for(i = 0; i < 5; ++i)
  {
    int level = 0;
    const char * xpath = NULL;
    int flags = 0;

    root = oyjlTreeParse2( json, OYJL_NO_MARKUP, __func__, NULL );

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
      plain = oyjlTermColorToPlain(rjson, 0);
      if(json && json[0] && strlen(plain) == 210 && strcmp(json,plain) == 0)
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
        plain = oyjlTermColorToPlain(rjson, 0);
      }
      if(rjson && rjson[0])
        success = 1;
      else if(i == 2 || i == 3)
        success = 1;
      if( success &&
          ((i == 1 && strcmp(plain, xjson) == 0) ||
           (i == 2 && strcmp(plain, "\"val_c\"") == 0) ||
           (i == 3 && !rjson) ||
           (i == 4 && strlen(plain) == 256)
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
        oyjl_val new_sub = oyjlTreeParse2( new_tree, OYJL_NO_MARKUP, __func__, NULL );
        oyjl_val rv = oyjlTreeGetValue( root, OYJL_CREATE_NEW, xpath );
        oyjl_val nv = oyjlTreeGetValue( new_sub, 0, "root" );
        size_t size = sizeof( * rv );
        memcpy( rv, nv, size );
        memset( nv, 0, size );
        oyjlTreeToJson( root, &level, &rjson );
        plain = oyjlTermColorToPlain(rjson, 0);
        if( strlen( plain ) == 291 )
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

  return result;
}

void testEscapeJsonPrintFlags        ( int                 flags,
                                       FILE              * fp,
                                       const char        * func,
                                       int                 line )
{
  char * t = NULL, * t2 = NULL, * t3 = NULL, * t4 = NULL, * t5 = NULL, * t6 = NULL, * t7 = NULL;

  fprintf( fp, "%s flags: %s%s%s%s%s%s%s%s%s\n", oyjlFunctionPrint(func, strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__, line), oyjlTermColorF( oyjlBLUE, "%d", flags),
      flags & OYJL_JSON   ? oyjlTermColorFPtr(oyjlNO_MARK, &t, " OYJL_JSON:%d",     OYJL_JSON ) : "",
      flags & OYJL_KEY    ? oyjlTermColorFPtr(oyjlNO_MARK, &t, " OYJL_KEY:%d",     OYJL_KEY ) : "",
      flags & OYJL_NO_INDEX    ? oyjlTermColorFPtr(oyjlNO_MARK, &t2, " OYJL_NO_INDEX:%d",     OYJL_NO_INDEX ) : "",
      flags & OYJL_QUOTE    ? oyjlTermColorFPtr(oyjlNO_MARK, &t3, " OYJL_QUOTE:%d",     OYJL_QUOTE ) : "",
      flags & OYJL_NO_BACKSLASH    ? oyjlTermColorFPtr(oyjlNO_MARK, &t4, " OYJL_NO_BACKSLASH:%d",     OYJL_NO_BACKSLASH ) : "",
      flags & OYJL_REGEXP ? oyjlTermColorFPtr(oyjlNO_MARK, &t5," OYJL_REGEXP:%d",  OYJL_REGEXP ) : "",
      flags & OYJL_JSON_VALUE? oyjlTermColorFPtr(oyjlNO_MARK, &t6, " OYJL_JSON_VALUE:%d", OYJL_JSON_VALUE ) : "",
      flags & OYJL_REVERSE? oyjlTermColorFPtr(oyjlNO_MARK, &t7, " OYJL_REVERSE:%d", OYJL_REVERSE ) : "" );
  free(t); free(t2); free(t3); free(t4); free(t5); free(t6); free(t7); t = t2 = t3 = t4 = t5 = t6 = t7 = NULL;
}

oyjlTESTRESULT_e testEscapeJson      ( const char        * text_,
                                       const char *        escaped,
                                       int                 flags,
                                       size_t              tree_size,
                                       oyjlTESTRESULT_e    result,
                                       oyjlTESTRESULT_e    fail )
{
  char * key, * text = NULL;
  int i = 0;

  testEscapeJsonPrintFlags( flags, zout, __func__, __LINE__ );

  key = oyjlStringEscape( text_, flags, 0 );
  if( strcmp( key, escaped ) == 0 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlJsonEscape(%s)     \"%s\"", oyjlTermColor(oyjlBOLD,text_),
    key  );
  } else
  { PRINT_SUB( fail,
    "oyjlJsonEscape(%s)     \"%s\" %s", oyjlTermColor(oyjlBOLD,text_),
    key, escaped );
  }

  const char * val = oyjlJsonEscape( key, flags | OYJL_REVERSE );
  if( strcmp( val, text_ ) == 0 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlJsonEscape(%s, %s) \"%s\"", oyjlTermColor(oyjlBOLD,text_),
    oyjlTermColorF(oyjlBLUE, "<-"), val );
  } else
  { PRINT_SUB( fail,
    "oyjlJsonEscape(%s, %s) \"%s\" %s", oyjlTermColor(oyjlBOLD,text_),
    oyjlTermColorF(oyjlBLUE, "<-"), val, escaped );
  }

  oyjl_val root = oyjlTreeNew("");
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "value", "data/key-%s", key );
  oyjlTreeToJson( root, &i, &text ); i = 0;
  int status = 0;
  oyjl_val rroot = oyjlTreeParse2( text, 0, __func__, &status );
  if( text && strlen( oyjlTermColorToPlain(text, 0) ) == tree_size && rroot && status == oyjlPARSE_STATE_NONE )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(text),
    "set key       \"%s\"",
    oyjlTermColor(oyjlBOLD,key) );
  } else
  { PRINT_SUB_INT( fail, strlen(text),
    "set key       \"%s\" %d",
    oyjlTermColor(oyjlBOLD,key), strlen( oyjlTermColorToPlain(text, 0) ) );
  }
  oyjlTreeFree( rroot );
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeSetStringF", "txt" )
  if(text && (oy_test_last_result != oyjlTESTRESULT_SUCCESS || verbose))
    fprintf( zout, "%s\n", text );

  oyjl_val value = oyjlTreeGetValueF( root, 0, "data/key-%s", key );
  if( value )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "get key       \"%s\"", key );
  } else
  { PRINT_SUB( fail,
    "get key       \"%s\"", key );
  }

  char * path = oyjlTreeGetPath( root, value );
  char * expect = NULL;
  oyjlStringAdd( &expect, 0,0, "data/key-%s", key );
  if( path && expect && ((expect && strchr(expect,'[') && !strstr(expect,"\\[")) || strcmp(path,expect) == 0) )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "get path      \"%s\"", path );
  } else
  { PRINT_SUB( fail,
    "get path oyjlTreeGetPath() ecpect:\"%s\"", expect );
  }
  myDeAllocFunc( expect ); expect = NULL;
  myDeAllocFunc( path ); path = NULL;
  int count;
  char ** paths = oyjlTreeToPaths( root, 10000000, NULL, OYJL_KEY, &count );
  if(oy_test_last_result != oyjlTESTRESULT_SUCCESS || verbose)
  {
    for(i = 0; i < count; ++i)
    {
      path = paths[i];
      fprintf( zout, "paths[%d]: \"%s\" / \"%s\"\n", i, oyjlTermColor(oyjlITALIC,path), oyjlJsonEscape( path, OYJL_KEY ) );
    }
  }
  path = oyjlStringCopy( paths[0], 0 );
  if(paths && count)
    oyjlStringListRelease( &paths, count, free );
  oyjlTreeFree( root );
  myDeAllocFunc( key ); key = NULL;

  root = oyjlTreeNew("");
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "value", "%s", path );
  char * roundtrip_json = oyjlTreeToText( root, OYJL_JSON );
  if( roundtrip_json && text && strlen( oyjlTermColorToPlain(text, 0) ) == strlen( oyjlTermColorToPlain(roundtrip_json, 0) ) )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "roundtrip path: %s", oyjlTermColor(oyjlBOLD,path)  );
  } else
  { PRINT_SUB( fail,
    "roundtrip path: %s", oyjlTermColor(oyjlBOLD,path)  );
  }
  oyjlTreeFree( root );
  OYJL_TEST_WRITE_RESULT( roundtrip_json, strlen(roundtrip_json), "roundtrip_json", "txt" )
  if(text && (oy_test_last_result != oyjlTESTRESULT_SUCCESS || verbose))
    fprintf( zout, "original: %s\nroundtrip_json: %s\n", text, roundtrip_json );

  myDeAllocFunc( path ); path = NULL;
  myDeAllocFunc( text ); text = NULL;
  myDeAllocFunc( roundtrip_json ); roundtrip_json = NULL;

  return result;
}

const char * oyjlTreeGetString_      ( oyjl_val            v,
                                       int                 flags OYJL_UNUSED,
                                       const char        * path );
oyjlTESTRESULT_e testEscapeJsonVal   ( const char        * text_,
                                       const char *        escaped,
                                       int                 flags,
                                       size_t              tree_size,
                                       oyjlTESTRESULT_e    result,
                                       oyjlTESTRESULT_e    fail )
{
  char * value, * text = 0;
  int i = 0;

  testEscapeJsonPrintFlags( flags, zout, __func__, __LINE__ );

  value = oyjlStringEscape( text_, flags, 0 );
  if( strcmp( value, escaped ) == 0 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringEscape(\"%s\")     = \"%s\"", oyjlTermColor(oyjlBOLD,text_),
    value );
  } else
  { PRINT_SUB( fail,
    "oyjlStringEscape(\"%s\")     = \"%s\" expected_escaped: \"%s\"", oyjlTermColor(oyjlBOLD,text_),
    value, escaped );
  }

  const char * val = oyjlJsonEscape( value, flags | OYJL_REVERSE );
  if( strcmp( val, text_ ) == 0 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlJsonEscape(  \"%s\", %s) = \"%s\"", oyjlTermColor(oyjlBOLD,text_),
    oyjlTermColorF(oyjlBLUE, "<-"), val );
  } else
  { PRINT_SUB( fail,
    "oyjlJsonEscape(  \"%s\", %s) = \"%s\" \"%s\"", oyjlTermColor(oyjlBOLD,text_),
    oyjlTermColorF(oyjlBLUE, "<-"), val, escaped );
  }

  oyjl_val root = oyjlTreeNew("");
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, text_, "key" );
  oyjlTreeToJson( root, &i, &text ); i = 0;
  int status = 0;
  oyjl_val rroot = oyjlTreeParse2( text, 0, __func__, &status );
  if( text && strlen( oyjlTermColorToPlain(text, 0) ) == tree_size && rroot && status == oyjlPARSE_STATE_NONE )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(text), 
    "set value        \"%s\"",
    oyjlTermColor(oyjlBOLD,text_), strlen(text) );
  } else
  { PRINT_SUB_INT( fail, strlen(text),
    "set value        \"%s\" %d/%d",
    oyjlTermColor(oyjlBOLD,text_), strlen( oyjlTermColorToPlain(text, 0) ), tree_size );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "value", "txt" )
  if(text && (oy_test_last_result != oyjlTESTRESULT_SUCCESS || verbose))
    fprintf( zout, "%s\n", text );

  const char * t = oyjlTreeGetString_( rroot, flags, "key" ),
             * unescaped = oyjlJsonEscape( t, flags | OYJL_REVERSE );
  if(unescaped && strcmp(unescaped, text_) == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "get value        \"%s\"",
    oyjlTermColor(oyjlBOLD,unescaped) );
  } else
  { PRINT_SUB( fail,
    "get value        \"%s\"",
    oyjlTermColor(oyjlBOLD,unescaped) );
  }
  oyjlTreeFree( rroot );

  oyjlTreeFree( root );
  myDeAllocFunc( text ); text = NULL;
  myDeAllocFunc( value ); value = NULL;

  return result;
}

oyjlTESTRESULT_e testJsonEscape ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  result = testEscapeJson( "Color", "Color", OYJL_NO_BACKSLASH, 44, result, oyjlTESTRESULT_FAIL );
  result = testEscapeJson( "Color \"Rose\"", "Color \"Rose\"", OYJL_NO_BACKSLASH, 53, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJson( "Color \"Rose\"", "Color \"Rose\"", 0, 53, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJson( "Color \"Rose\"", "Color \"Rose\"", OYJL_KEY, 53, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJson( "Color \"Rose\"", "Color \\\"Rose\\\"", OYJL_JSON, 57, result, oyjlTESTRESULT_XFAIL );

  result = testEscapeJson( "2. Color", "2. Color", OYJL_KEY, 47, result, oyjlTESTRESULT_XFAIL );

  result = testEscapeJson( "Color [3]", "Color \\\\[3]", OYJL_NO_BACKSLASH | OYJL_NO_INDEX, 48, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJson( "Color [3]", "Color [3]", 0, 38, result, oyjlTESTRESULT_XFAIL );

  result = testEscapeJsonVal( "my.value", "my.value", OYJL_JSON_VALUE, 23, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJsonVal( "my.value", "my.value", 0, 23, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJsonVal( "my\\.value", "my\\\\.value", OYJL_QUOTE | OYJL_NO_BACKSLASH | OYJL_JSON_VALUE, 25, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJsonVal( "my\\.value", "my\\.value", OYJL_NO_BACKSLASH, 25, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJsonVal( "my/key", "my%37key", OYJL_KEY, 21, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJsonVal( "my/key.attribute", "my%37key.attribute", OYJL_KEY, 31, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJsonVal( "value\nafter_line_break", "value\\nafter_line_break", 0, 38, result, oyjlTESTRESULT_XFAIL );

  const char * prepend = "[]/",
             * sequence = "\\.property/[1]-?%+;![o]&more$2";
  char * key = NULL, * val = NULL;
  oyjlStringAdd( &key, 0,0, "%s\"key\"%s", prepend, sequence );
  oyjlStringAdd( &val, 0,0, "%s\"value\"%s", prepend, sequence );
  result = testEscapeJson(    key, "\\\\[]%37\"key\"\\.property%37\\\\[1]-?%+;!\\\\[o]&more$2", OYJL_KEY, 80, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJsonVal( val, "[]/\"value\"\\\\.property/[1]-?%+;![o]&more$2", 0, 58, result, oyjlTESTRESULT_XFAIL );
  free(key); key = NULL;
  free(val); val = NULL;

  sequence = ".property/[1]-?%+;&more";
  oyjlStringAdd( &key, 0,0, "%s\"key\"%s", prepend, sequence );
  oyjlStringAdd( &val, 0,0, "%s\"value\"%s", prepend, sequence );
  result = testEscapeJson(    key, "\\\\[]%37\"key\".property%37\\\\[1]-?%+;&more", OYJL_KEY, 72, result, oyjlTESTRESULT_XFAIL );
  result = testEscapeJsonVal( val, "[]/\"value\".property/[1]-?%+;&more", 0, 50, result, oyjlTESTRESULT_XFAIL );
  free(key); key = NULL;
  free(val); val = NULL;

  return result;
}

oyjlTESTRESULT_e testJson2 ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

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
  char error_buffer[128];
  oyjl_val root = oyjlTreeParse( json2, error_buffer, 128 );
  /* now some stressing with to be escaped letters */
  const char * prepend = "[]/",
             * sequence = "\\.property/[1]-\"?%+\";&more";
  char * key = NULL, * val = NULL;
  char * key_escaped = NULL;
  oyjlStringAdd( &key, 0,0, "%skey%s", prepend, sequence );
  key_escaped = oyjlStringEscape( key, OYJL_KEY, 0 );
  oyjlStringAdd( &val, 0,0, "%svalue%s", prepend, sequence );
  if(key_escaped && strcmp(key_escaped, "\\\\[]%37key\\.property%37\\\\[1]-\"?%+\";&more") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringEscape( \"%s\", OYJL_KEY ) = \"%s\"", oyjlTermColor(oyjlITALIC,key), oyjlTermColorF(oyjlBOLD, "%s",key_escaped) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringEscape( \"%s\", OYJL_KEY ) = \"%s\"", oyjlTermColor(oyjlITALIC,key), oyjlTermColorF(oyjlBOLD, "%s",key_escaped) );
  }
  if(verbose)
    fprintf( zout, "oyjlTreeSetStringF( val:\"%s\", path:data/\"s\"(orig:%s ) %s\n", val, key_escaped, key );
  free(key); key = NULL;
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, val, "data/%s", key_escaped );
  free(key_escaped); key_escaped = NULL;
  free(val); val = NULL;

  sequence = ".property/[1]-?%+;&more";
  oyjlStringAdd( &key, 0,0, "%skey%s", prepend, sequence );
  key_escaped = oyjlStringEscape( key, OYJL_KEY, 0 );
  oyjlStringAdd( &val, 0,0, "%svalue%s", prepend, sequence );
  if(key_escaped && strcmp(key_escaped, "\\\\[]%37key.property%37\\\\[1]-?%+;&more") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlJsonEscape(   \"%s\", OYJL_KEY ) = \"%s\"", oyjlTermColor(oyjlITALIC,key), oyjlTermColorF(oyjlBOLD, "%s",key_escaped) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlJsonEscape(   \"%s\", OYJL_KEY ) = \"%s\"", oyjlTermColor(oyjlITALIC,key), oyjlTermColorF(oyjlBOLD, "%s",key_escaped) );
  }
  if(verbose)
    fprintf( zout, "oyjlTreeSetStringF( val:\"%s\", path:data/\"s\"(orig:%s ) %s\n", val, key_escaped, key );
  free(key); key = NULL;
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, val, "data/%s", key_escaped );
  free(val); val = NULL;

  char * text = NULL;
  int level = 0;
  oyjlTreeToJson( root, &level, &text );
  int len = strlen( oyjlTermColorToPlain(text, 0) );
  if(text && len == 370)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "escape roundtrip" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, len,
    "escape roundtrip" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "key.property", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );

  int count;
  char ** paths = oyjlTreeToPaths( root, 10, NULL, 0, &count ),
       * p12 = paths[12];
  int j = 0;
  if( count == 13 &&
      strcmp(paths[j++],"org") == 0 &&
      strcmp(paths[j++],"org/free") == 0 &&
      strcmp(paths[j++],"org/free/[0]") == 0 &&
      strstr(p12,key_escaped) != NULL
    )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, count,
    "oyjlTreeToPaths( OYJL_PATH ) %s", key_escaped );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, count,
    "oyjlTreeToPaths( OYJL_PATH ) %s", key_escaped );
  }
  if(verbose)
  for(j = 0; j < count; ++j)
    fprintf( zout, "%d: %s\n", j, paths[j] );

  int flags = verbose ? OYJL_OBSERVE : 0;
  j = 11;
  const char * value_text = oyjlTreeGetString_( root, flags, paths[j] );
  if(strcmp(value_text,"[]/value\\.property/[1]-\"?%+\";&more") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeGetString_( %s ) %s", verbose?oyjlTermColorF(oyjlITALIC,"[%d] %s", j, paths[j]):"", verbose?oyjlTermColor(oyjlBOLD,"value\\.property"):"" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeGetString_( %s ) %s", oyjlTermColorF(oyjlITALIC,"[%d] %s", j, paths[j]), oyjlTermColor(oyjlBOLD,value_text) );
  }
  ++j;
  value_text = oyjlTreeGetString_( root, flags, paths[j] );
  if(strcmp(value_text,"[]/value.property/[1]-?%+;&more") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeGetString_( %s ) %s", verbose?oyjlTermColorF(oyjlITALIC,"[%d] %s", j, paths[j]):"", verbose?oyjlTermColor(oyjlBOLD,"value.property"):"" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeGetString_( %s ) %s", oyjlTermColorF(oyjlITALIC,"[%d] %s", j, paths[j]), oyjlTermColor(oyjlBOLD,value_text) );
  }
  if(paths && count)
    oyjlStringListRelease( &paths, count, free );
  oyjlTreeFree( root );
  free(key_escaped); key_escaped = NULL;


  root = oyjlTreeParse2( text, 0, __func__, NULL );
  char * tree_text = text; text = NULL;

  int size = 0;
  oyjl_val value = oyjlTreeSerialise( root, flags, &size );
  oyjlTreeFree( root );
  if(size > 10 && oyjlStringStartsWith( (const char*)value, "oiJS", 0 ))
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

  paths = oyjlTreeToPaths( value, 10000000, NULL, flags | OYJL_KEY, &count );
  j = 0;
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
      fprintf( zout, OYJL_DBG_FORMAT "%d: %s\n", OYJL_DBG_ARGS, j, paths[j] );

  j = 0;
  if( count == 8 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"null") == 0 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"matrix.from") == 0 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"1.0") == 0 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"1.0") == 0 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"true") == 0 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"false") == 0 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"[]/value\\.property/[1]-\"?%+\";&more") == 0 &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"[]/value.property/[1]-?%+;&more") == 0
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
      fprintf( zout, OYJL_DBG_FORMAT "%d: %s\n", OYJL_DBG_ARGS, j, paths[j] );

  j = 0;
  if( count == 8 &&
      oyjlTreeGetValue( value, 0, paths[j++] )->type == oyjl_t_null &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"matrix.from") == 0 &&
      oyjlTreeGetValue( value, 0, paths[j++] )->type == oyjl_t_number &&
      strcmp(oyjlTreeGetString_( value, 0, paths[j++] ),"1.0") == 0 &&
      oyjlTreeGetValue( value, 0, paths[j++] )->type == oyjl_t_true &&
      oyjlTreeGetValue( value, 0, paths[j++] )->type == oyjl_t_false &&
      strcmp(oyjlTreeGetString_( value, flags, paths[j++] ),"[]/value\\.property/[1]-\"?%+\";&more") == 0 &&
      strcmp(oyjlTreeGetString_( value, flags, paths[j++] ),"[]/value.property/[1]-?%+;&more") == 0
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

  const char * ctext = oyjlTermColorToPlain(text, 0);
  if(ctext && strlen( ctext ) == 370)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, strlen(ctext),
    "oyjlTreeDeSerialise()" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, strlen(ctext),
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
  int len;

  oyjl_val root = 0;

  root = oyjlTreeParse( json, error_buffer, 128 );

  oyjlTreeToJson( root, &level, &text );
  format = oyjlDataFormat(text);
  len = strlen(text);
  if(text && text[0] && strlen(text) > 20 && format == 7 && (len == 413 || len == 771))
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
  len = strlen(text);
  if(text && text[0] && strlen(text) > 20 && format == 9 && (len == 320 || len == 678))
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
  len = strlen(text);
  if(text && text[0] && strlen(text) > 20 && format == 8 && (len == 443 || len == 889))
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
  char * plain, * plain2;
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
  int status = 0;
  root = oyjlTreeParse2( text_to_xml, OYJL_NUMBER_DETECTION, __func__, &status );
  level = 0;
  oyjlTreeToJson( root, &level, &text_from_xml );
  plain = oyjlStringCopy( oyjlTermColorToPlain( text, 0 ), 0 );
  plain2 = oyjlStringCopy( oyjlTermColorToPlain( text_from_xml, 0 ), 0 );
  int text_from_xml_size = strlen(plain2),
      text_size = strlen(plain);
  if(text && text_from_xml && text_from_xml_size == text_size && status == oyjlPARSE_STATE_NONE)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeToXml () <-> oyjlTreeParseXml () %i<->%i", text_from_xml_size, text_size );
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
  if(plain) {free(plain); plain = NULL;}
  if(plain2) {free(plain2); plain2 = NULL;}

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
  root = oyjlTreeParse2( text_from_xml, 0, __func__, NULL );
  level = 0;
  oyjlTreeToXml( root, &level, &text_to_xml );
  oyjlTreeFree( root ); root = NULL;
  if(xml && text_to_xml && strlen(oyjlTermColorToPlain( text_to_xml, 0 )) == strlen(xml))
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

#ifdef OYJL_HAVE_YAML
  const char * yaml = "\
---\n\
org:\n\
  free:\n\
    -\n\
      s1key_a: val_a\n\
      s1key_b: val.b\n\
    -\n\
      xml@s2attribute_c: val_c\n\
      xml@text: inner string d\n\
  key_e: val_e\n\
  xml@xmlns:xsi: http://www.w3.org/2001/XMLSchema-instance\n\
  xml@attribute_f: val_f\n\
  xsi:node:\n\
    xml@text: inner string g\n\
    xml@attr_h: val_h\n\
  val_i: 1234\n\
  val_j: true\n\
  val_k: false\n\
  val_l: 12.34\n\
  val_m:\n\
    - 0.0\n\
    - 1\n\
    - 2\n\
    - 3\n\
    - 4.5";
  root = oyjlTreeParseYaml( yaml, 0, error_buffer, 128 );
  text = oyjlTreeToText( root, OYJL_YAML );
  int size_to = strlen( oyjlTermColorToPlain( yaml, 0 ) );
  int size_from = strlen( oyjlTermColorToPlain( text, 0 ) );
  if(root && size_to == size_from)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeParseYaml() <-> oyjlTreeToYaml()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeParseYaml() <-> oyjlTreeToYaml()" );
  }
  oyjlTreeFree( root ); root = NULL;
#endif
  myDeAllocFunc(text); text = NULL;

  const char * csv = "\
header;B;C;D;E;F\n\
A1;B1;C1;D1;E1;F1\n\
2;B2;1234;true;false;12.34";
  root = oyjlTreeParseCsv( csv, ";", OYJL_NUMBER_DETECTION, error_buffer, 128 );
  text = oyjlTreeToText( root, OYJL_CSV | OYJL_DELIMITER_SEMICOLON );
  int size_to_csv = strlen( oyjlTermColorToPlain( csv, 0 ) );
  int size_from_csv = strlen( oyjlTermColorToPlain( text, 0 ) );
  if(root && size_to_csv == size_from_csv)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeParseCsv() <-> oyjlTreeToCsv()" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeParseCsv() <-> oyjlTreeToCsv() %d/%d", size_to_csv, size_from_csv );
    fprintf( zout, "%s\n", csv );
    fprintf( zout, "%s\n", text );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlTreeToCsv", "txt" )
  myDeAllocFunc(text); text = NULL;

  if(OYJL_GET_ARRAY_2D_INTEGER(root,2,2) == 1234)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "OYJL_GET_ARRAY_2D_INTEGER(csv,2,2)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "OYJL_GET_ARRAY_2D_INTEGER(csv,2,2) %d", OYJL_GET_ARRAY_2D_INTEGER(root,2,2) );
  }

  if(OYJL_GET_ARRAY_2D_DOUBLE(root,2,5) == 12.34)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "OYJL_GET_ARRAY_2D_DOUBLE(csv,2,5)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "OYJL_GET_ARRAY_2D_DOUBLE(csv,2,5) %s", oyjlValueText(OYJL_GET_ARRAY_2D_POS(root,2,5),0) );
  }

  const char * t = OYJL_GET_ARRAY_2D_STRING(root,0,0);
  if(t && strcmp( t, "header") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "OYJL_GET_ARRAY_2D_STRING(csv,0,0)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "OYJL_GET_ARRAY_2D_STRING(csv,0,0) %s", t );
  }
  oyjlTreeFree( root ); root = NULL;

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
  char * t = oyjlReadCommandF( &size, "r", malloc, "pkg-config -libs-only-L openicc" );
  const char * gcov_flags = "";
  if(t && t[0] && t[strlen(t)-1] == '\n') t[strlen(t)-1] = '\000';

  fprintf( zout, "compiling and testing: %s\n", oyjlTermColor(oyjlBOLD, prog) );
#ifdef USE_GCOV
  gcov_flags = " -g -O0 -fprofile-arcs -ftest-coverage ";
  fprintf( zout, "Compiling with gcov: %s\n", gcov_flags );
#endif

  if(c_source && len == code_size)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "oyjlUiExportToCode(OYJL_SOURCE_CODE_C)" );
  } else
  { PRINT_SUB_INT( fail, len,
    "oyjlUiExportToCode(OYJL_SOURCE_CODE_C)" );
  }
  OYJL_TEST_WRITE_RESULT( c_source, len, "oyjlUiExportToCode", "txt" )
  if(verbose && c_source)
    fprintf( zout, "%s\n", c_source );
  oyjlStringAdd( &name, 0,0, "%s.c", prog );
  oyjlWriteFile( name, c_source, len );
  if(c_source) {free(c_source);} c_source = NULL;
  /* compile */
  if(!(t && t[0]))
    fprintf( zout, "Compiling without OpenICC\n" );
  if(lib_a_size)
    oyjlStringAdd( &command, 0,0, "c++ %s -g -O0 -I %s -I %s %s -L %s/oyjl-args-qml -loyjl-args-qml-static -lQt5DBus -lQt5Qml -lQt5Network -lQt5Widgets -lQt5Gui -lQt5Core -L %s -L %s -loyjl-args-cli-static -loyjl-args-web-static-client -lmicrohttpd -loyjl-static -loyjl-core-static %s %s -lyaml -lyajl -lxml2 %s -o %s", verbose?"-Wall -Wextra":"-Wno-write-strings", OYJL_SOURCEDIR, OYJL_BUILDDIR, name, OYJL_BUILDDIR, OYJL_BUILDDIR, OYJL_INSTALL_LIBDIR, t&&t[0]?t:"", t&&t[0]?"-lopenicc-static":"", gcov_flags, prog );
  else if(lib_so_size)
    oyjlStringAdd( &command, 0,0, "cc %s -g -O0 -I %s -I %s %s -L %s -lOyjl -lOyjlCore -o %s", verbose?"-Wall -Wextra":"", OYJL_SOURCEDIR, OYJL_BUILDDIR, name, OYJL_BUILDDIR, prog );
  if(command)
  {
    if(verbose)
      fprintf( stderr, "compiling: %s\n", oyjlTermColor( oyjlBOLD, command ) );
    int r = system(command);
    if(command) {free(command); command = NULL;}
    int size = oyjlIsFile( prog, "r", OYJL_NO_CHECK, info, 48 );
    if(!size || verbose)
    {
      fprintf(stderr, "%scompile: %s %s %d %d\n", size == 0?"Could not ":"", oyjlTermColor(oyjlBOLD,prog), info, size, r);
    }
  }
  if(name) {free(name); name = NULL;}
  if(t) {free(t); t = NULL;}
  if(!command) return result;

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
  len = t ? strlen(plain) : 0;
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
  plain = oyjlTermColorToPlain(t, 0);
  len = t ? strlen(plain) : 0;
  if(abs((int)len - (int)json_size) <= 1 && oyjlDataFormat(plain) == 7)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "%s -X json", prog );
  } else
  { PRINT_SUB_INT( fail, len,
    "%s -X json                       %lu/%lu %d", prog, json_size, len, oyjlDataFormat(plain) );
  }
  OYJL_TEST_WRITE_RESULT( t, len, prog, "json" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X json+command", prog );
  plain = oyjlTermColorToPlain(t, 0);
  len = t ? strlen(plain) : 0;
  if(abs((int)len - (int)json_command_size) <= 1 && oyjlDataFormat(plain) == 7)
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
  plain = oyjlTermColorToPlain(t, 0);
  len = t ? strlen(plain) : 0;
  if(abs((int)len - (int)export_size) <= 1 && oyjlDataFormat(plain) == 7)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "%s -X export", prog );
  } else
  { PRINT_SUB_INT( fail, len,
    "%s -X export", prog );
  }
  OYJL_TEST_WRITE_RESULT( t, len, prog, "json" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );

  char * export_text = t; t = NULL;
  size_t export_len = len;
  oyjl_val root = oyjlTreeParse2( export_text, 0, __func__, NULL );

  oyjlUi_s * ui = oyjlUi_ImportFromJson( root, 0 );
  t = oyjlUi_ExportToJson( ui, 0 );
  len = t ? strlen(oyjlTermColorToPlain( t, 0 )) : 0;
  if(abs((int)len - (int)export_len) <= 1 )
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "oyjlUi_ImportFromJson()" );
  } else
  { PRINT_SUB_INT( fail, len,
    "oyjlUi_ImportFromJson()" );
  }
  if(ui) oyjlOptions_SetAttributes( ui->opts, NULL );
  oyjlUi_Release( &ui);
  OYJL_TEST_WRITE_RESULT( t, strlen(t), "oyjlUi_ImportFromJson", "txt" )
  if(verbose && t)
    fprintf( zout, "%s\n", t );

  oyjlStringAdd( &name, 0,0, "%s-import.json", prog );
  oyjlWriteFile( name, t, strlen(t) );
  if(name) {free(name);} name = NULL;
  oyjlStringAdd( &name, 0,0, "%s-export.json", prog );
  oyjlWriteFile( name, t, strlen(t) );
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

oyjlTESTRESULT_e testUiRoundtrip ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  setlocale(LC_ALL,"en_GB.UTF8");
  oyjlLang( "en_GB.UTF8" );

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
    {"oiwi", 0,     "#", "",        NULL, _("status"),  _("Show Status"),    NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &show_status}, NULL },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,"@","",NULL,_("input"),_("Set Input"),NULL, _("FILENAME"), oyjlOPTIONTYPE_FUNCTION, {}, oyjlINT, {.i = &file_count}, NULL },
    {"oiwi", 0,     "i", "input",   NULL, _("input"),   _("Set Input"),      NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)i_choices, sizeof(i_choices), malloc )}, oyjlSTRING, {.s = &file}, NULL },
    {"oiwi", 0,     "o", "output",  NULL, _("output"),  _("Control Output"), NULL, "0|1|2",       oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)o_choices, sizeof(o_choices), malloc )}, oyjlINT, {.i = &output}, NULL },
    {"oiwi", 0,     "f", "function",NULL, _("function"),_("Function"),       NULL, _("FUNCTION"), oyjlOPTIONTYPE_FUNCTION, {}, oyjlSTRING, {.s = &func}, NULL },
    {"oiwi", 0,     "n", "number",  NULL, _("number"),  _("Number"),         NULL, _("NUMBER"),   oyjlOPTIONTYPE_DOUBLE, {.dbl.start = -1.0, .dbl.end = 1.0, .dbl.tick = 0.1, .dbl.d = number}, oyjlDOUBLE, {.d = &number}, NULL },
    {"oiwi", 0,     "h", "help",    NULL, _("help"),    _("Help"),           NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &help}, NULL },
    {"oiwi", 0,     "v", "verbose", NULL, _("verbose"), _("verbose"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &verbose_}, NULL },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{},0}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups_no_args[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Mode1"),_("Simple mode"),     NULL, "#",       "o,v",    "o", NULL }, /* accepted even if none of the mandatory options is set */
    {"oiwg", OYJL_OPTION_FLAG_EDITABLE,_("Mode2"),_("Any arg mode"),NULL,"@","o,v","@,o", NULL},/* accepted if anonymous arguments are set */
    {"oiwg", 0,     _("Mode3"),_("Actual mode"),     NULL, "i",       "o,f,n,v",    "i,o,f,n", NULL },/* parsed and checked with -i option */
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "",        "",       "v,h", NULL },/* just show in documentation */
    {"",0,0,0,0,0,0,0,0}
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
  int len = strlen(text);
  if(text && (len == 8689 || len == 12179))
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

  oyjl_val json = oyjlTreeParse2( text, 0, __func__, NULL );
  if(text) {free(text);} text = NULL;

  result = testCode( json, "oiCR"                    /*prog*/,
                           9864                      /*code_size*/,
                           1283                      /*help_size*/,
                           2182                      /*man_size*/,
                           4476                      /*markdown_size*/,
                           8023                      /*json_size*/,
                           8050                      /*json_command_size*/,
                           11790                     /*export_size*/,
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

  oyjlTranslation_s * trc = NULL; oyjlTranslation_Unset( OYJL_DOMAIN );
  setlocale(LC_ALL,"en_GB.UTF8");
  oyjlLang("en_GB.UTF8");

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
    {"oiwi", 0,     "#", "",        NULL, _("status"),  _("Show Status"),    NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &show_status}, NULL },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,"@","",NULL,_("input"),_("Set Input"),NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {}, oyjlINT, {.i = &file_count}, NULL },
    {"oiwi", 0,     "i", "input",   NULL, _("input"),   _("Set Input"),      NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)i_choices, sizeof(i_choices), malloc )}, oyjlSTRING, {.s = &file}, NULL },
    {"oiwi", 0,     "o", "output",  NULL, _("output"),  _("Control Output"), NULL, "0|1|2",       oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)o_choices, sizeof(o_choices), malloc )}, oyjlINT, {.i = &output}, NULL },
    {"oiwi", 0,     "h", "help",    NULL, _("help"),    _("Help"),           NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &help}, NULL },
    {"oiwi", 0,     "v", "verbose", NULL, _("verbose"), _("verbose"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &verbose_}, NULL },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{},0}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups_no_args[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Mode(1)"),_("Simple mode \"(1)\""), NULL, "#", "o,v",    "o", NULL }, /* accepted even if none of the mandatory options is set */
    {"oiwg", OYJL_OPTION_FLAG_EDITABLE,_("Mode2"),_("Any arg mode"),NULL,"@","o,v","@,o", NULL},/* accepted if anonymous arguments are set */
    {"oiwg", 0,     _("Mode3"),_("Actual mode"),     NULL, "i",       "o,v",    "i,o", NULL },/* parsed and checked with -i option */
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "",        "h",      "h,v", NULL },/* just show in documentation */
    {"",0,0,0,0,0,0,0,0}
  };

  const char * argv_anonymous[] = {"test","-v","file-name.json","file-name2.json"};
  int argc_anonymous = 4;
  oyjlUi_s * ui = oyjlUi_Create( argc_anonymous, argv_anonymous, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups_no_args, NULL );
  char * text = oyjlUi_ToJson( ui, 0 );
  int len;
  len = strlen(text);
  if(text && (len == 7971 || len == 10467))
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
  len = strlen(text);
  if(text && (len == 6689 || len == 9426))
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

  oyjl_val json = oyjlTreeParse2( text, 0, __func__, NULL );
  if(text) {free(text);} text = NULL;

  result = testCode( json, "oiCR_enGB"               /*prog*/,
                           9334                      /*code_size*/,
                           1186                      /*help_size*/,
                           2081                      /*man_size*/,
                           4034                      /*markdown_size*/,
                           7441                      /*json_size*/,
                           7473                      /*json_command_size*/,
                           10714                     /*export_size*/,
                           3150                      /*bash_size*/,
                           result,
                           oyjlTESTRESULT_FAIL       /*fail*/ );

  oyjlTreeFree( json ); json = NULL;

  text = oyjlUi_ExportToJson( ui_en, 0 );
  len = strlen(text);
  if(text && (len == 6689 || len == 9426))
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
  trc = oyjlTranslation_New( "C", OYJL_DOMAIN, &catalog, NULL,NULL,NULL, !verbose?0:OYJL_OBSERVE );
  oyjlTranslation_Set( &trc );
  oyjlLang( "de_DE" );

  ui = oyjlUi_Create( argc_anonymous, argv_anonymous, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups_no_args, NULL );
  text = oyjlUi_ToJson( ui, 0 );
  len = strlen(text);
  if(text && (len == 7977 || len == 10473))
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
  len = strlen(text);
  if(text && (len == 6695 || len == 9432))
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, text?strlen(text):0,
    "oyjlUi_ExportToJson(de)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, text?strlen(text):0,
    "oyjlUi_ExportToJson(de)" );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ExportToJson-de", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );

  json = oyjlTreeParse2( text, 0, __func__, NULL );
  if(text) {free(text);} text = NULL;

  char * c_source = oyjlUiExportToCode( json, OYJL_SOURCE_CODE_C );
  if(c_source && strlen(c_source) == 9340)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, c_source?strlen(c_source):0,
    "oyjlUiExportToCode(de)" );
  } else
  { PRINT_SUB_INT( oyjlTESTRESULT_FAIL, c_source?strlen(c_source):0,
    "oyjlUiExportToCode(de)" );
  }
  OYJL_TEST_WRITE_RESULT( c_source, strlen(c_source), "oyjlUiExportToCode-de", "txt" )
  if(verbose && c_source)
    fprintf( zout, "%s\n", c_source );
  if(c_source) {free(c_source);} c_source = NULL;

  oyjlTreeFree( json ); json = NULL;

  catalog = oyjlTreeParse( catalog_json, NULL, 0 );
  trc = oyjlTranslation_New( "de_DE", OYJL_DOMAIN, &catalog, NULL,NULL,NULL, !verbose?0:OYJL_OBSERVE );
  oyjlUi_Translate( ui, trc );
  text = oyjlUi_ToJson( ui, 0 );
  len = strlen(text);
  if(text && (len == 8050 || len == 10546))
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

  oyjlTranslation_SetLocale( trc, "back" );
  oyjlUi_Translate( ui, trc );
  text = oyjlUi_ToJson( ui, 0 );
  len = strlen(text);
  if(text && (len == 7971 || len == 10467))
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
  len = strlen(text);
  if(text && (len == 6689 || len == 9426))
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

  oyjlTranslation_SetLocale( trc, "cs_CZ" );
  oyjlUi_Translate( ui, trc );
  text = oyjlUi_ToJson( ui, 0 );
  len = strlen(text);
  if(text && (len == 8082 || len == 10578))
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
  len = strlen(text);
  if(text && (len == 6752 || len == 9489))
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
  catalog = oyjlTranslation_GetCatalog( oyjlTranslation_Get(OYJL_DOMAIN) );
  oyjlTreeToJson( catalog, &level, &text );
  //puts(text);
  if(text && (strlen(text) == 1797 || strlen(text) == 2397) && strlen(catalog_json) == 1797)
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
  oyjlTranslation_Release( &trc );

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
  const char * plain;

  fprintf( zout, "testing: %s  %d tests\n", oyjlTermColorF(oyjlBOLD, prog), count );

  char * command = NULL, *t;
  oyjlStringAdd( &command, 0,0, "%s/%s", OYJL_BUILDDIR, prog );
  if(command)
  {
    if(verbose)
      fprintf( stderr, "detecting: %s\n", oyjlTermColor( oyjlBOLD, command ) );
    size = oyjlIsFile( command, "r", OYJL_NO_CHECK, info, 48 );
    if(!size || verbose)
    {
      fprintf(stderr, "%sread: %s %s %d\n", size == 0?"Could not ":"", oyjlTermColor(oyjlBOLD,prog), info, size);
    }
    free(command);
    command = NULL;
  }

  t = oyjlReadCommandF( &size, "r", malloc, "%s%s --help", strstr(prog,  "LANG=")?"":"LANG=C ./", prog );
  plain = oyjlTermColorToPlain(t, 0);
  len = t ? strlen(plain) : 0;
  if(len == help_size)
  { PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
    "%s --help", prog );
  } else
  { PRINT_SUB_INT( fail, len,
    "%s --help", prog );
  }
  OYJL_TEST_WRITE_RESULT( t, size, prog, "txt" )
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
    plain = oyjlTermColorToPlain(t, 0);
    len = t ? strlen(plain) : 0;
    if(len && t[len-1] == '\n' && result_string && result_string[0] && result_string[strlen(result_string)-1] != '\n')
      oyjlStringAdd( &result_string, 0,0, "\n" );
    if( (len == result_size || result_size == 0) &&
        (!result_string ||
         (result_string && t && strcmp(t, result_string) == 0)) )
    {
      if(result_string && len && result_string[len-1] == '\n') result_string[len-1] = '\000';
      PRINT_SUB_INT( oyjlTESTRESULT_SUCCESS, len,
        "%s%s%s%s%s", print?print:cmd, result_string&&t?" = ":"", result_string&&t?"\"":"", result_string&&t?result_string:"", result_string&&t?"\"":"" );
    } else
    { PRINT_SUB( fail,
      "%s  %lu(%lu) %s", print?print:cmd, len,result_size, result_string&&t?result_string:"" );
    }
    OYJL_TEST_WRITE_RESULT( t, size, prog, "txt" )
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
    { "json -i i18n_de_DE.c -i i18n_cs_CZ.json -w oiJS", 2474,  NULL, NULL },
    { "json -i i18n_de_DE.c -i i18n_cs_CZ.json -w oiJS | ./oyjl json -i=-", 432, NULL,NULL },
    { "yaml -i i18n_de_DE.c -i i18n_cs_CZ.json",    289,  NULL,       NULL },
    { "xml -i i18n_de_DE.c -i i18n_cs_CZ.json",     482,  NULL,       NULL },
    { "json -i i18n_de_DE.c -x '/////[1]' --set 'LongValue'",344,NULL,     NULL }, /* change a value */
    { "json -i i18n_de_DE.c > i18n.de_DE.json; cat i18n.de_DE.json",    337,  NULL, "convert from C to JSON" },
    { "json -i i18n.de_DE.json -w C --wrap-name oyjl > i18n.de_DE.c; cat i18n.de_DE.c",    410, NULL, "convert back to C" },
    { "count -i i18n_de_DE.c",                      2,    "1",        NULL },
    { "count -i i18n_de_DE.c -x '////'",            2,    "5",        NULL },
    { "key -i i18n_de_DE.c -x '////'",              6,    "de_DE",    NULL },
    { "type -i i18n_de_DE.c -x '/////'",            7,    "string",   NULL },
    { "paths -i i18n_de_DE.c -x '////'",            291,  NULL,       NULL },
    { "format -i i18n_cs_CZ.json",                  5,    "JSON",     NULL },
    { "format -i none.file",                        9,    "no input", NULL },
    { "-X man > oyjl.1 && cat oyjl.1",              8222, NULL,       NULL },
    { "-R cli > oyjl.txt && cat oyjl.txt",          0,    NULL,       NULL }
  };
  int count = 17;
  result = testTool( "oyjl", 5539/*help size*/, commands_oyjl, count, result, oyjlTESTRESULT_FAIL );

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
  const char * loc = setlocale(LC_ALL,"en_GB.UTF8");
  if(verbose)
  {
    fprintf( zout, "setlocale(LC_ALL,\"en_GB.UTF8\") = %s\n", loc );
    int err = setenv("OUTPUT_CHARSET", "UTF-8", 1); if(err || verbose) fprintf( zout, "setenv(\"OUTPUT_CHARSET\", \"UTF-8\", 1) == %d\n", err );
  }

  oyjl_command_test_s commands_oyjl_translate[] = {
    { "-X export > oyjl-translate-ui.json && cat oyjl-translate-ui.json", 27980,  NULL,       NULL },
    { "-e -i oyjl-translate-ui.json -o i18n.c -f '_(\"%s\");\n' -k name,description,help && cat i18n.c", 4833,  NULL,       NULL },
#ifdef OYJL_USE_GETTEXT
    { "-a -i oyjl-translate-ui.json -o oyjl-translate-ui-i18n.json -k name,description,help -d oyjl -p locale -l=de_DE.UTF-8,cs_CZ.UTF-8 && cat oyjl-translate-ui-i18n.json", loc==NULL?0:37619, NULL,       NULL },
#endif
    { "-V; xgettext --add-comments --keyword=gettext --flag=gettext:1:pass-c-format --keyword=_ --flag=_:1:pass-c-format --keyword=N_ --flag=N_:1:pass-c-format  --copyright-holder='Kai-Uwe Behrmann'  --msgid-bugs-address='ku.b@gmx.de' --from-code=utf-8 --package-name=i18n --package-version=1.0.0 -o i18n.pot i18n.c && cat i18n.pot", 8779,  NULL,       "xgettext ... i18n.c -> i18n.pot; hand translate -> de.po(prepared example)" },
    { "-c -i de.po --locale=de_DE -o i18n-de_DE.json && cat i18n-de_DE.json", 320, NULL,       NULL }
  };
  int count = 4;
#ifdef OYJL_USE_GETTEXT
  ++count;
#endif
  result = testTool( "oyjl-translate", 4639/*help size*/, commands_oyjl_translate, count, result, oyjlTESTRESULT_FAIL );

  return result;
}/* --- end actual tests --- */


