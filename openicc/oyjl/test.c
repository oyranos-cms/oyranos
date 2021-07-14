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
  TEST_RUN( testUiTranslation, "Ui Translation", 1 );

void oyjlLibRelease();
#define OYJL_TEST_MAIN_SETUP  printf("\n    Oyjl Test Program\n");

#include "oyjl_version.h"
#ifdef OYJL_HAVE_LIBXML2
#include <libxml/parser.h>
# define OYJL_TEST_MAIN_FINISH printf("\n    Oyjl Test Program finished\n\n"); oyjlLibRelease(); xmlCleanupParser();
#else
# define OYJL_TEST_MAIN_FINISH printf("\n    Oyjl Test Program finished\n\n"); oyjlLibRelease();
#endif
#define OYJL_TEST_NAME "test"
#include "oyjl_test_main.h"
#include "oyjl.h"
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>
#endif
#include "oyjl_i18n.h"
#include "oyjl_internal.h"


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

#undef _
#define _(x) oyjlTranslate( lang, catalog, x )
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
            \"Color\": \"Farbe\"\n\
          }\n\
        }\n\
      }\n\
    }\n\
  }\n\
}";
  oyjl_val catalog = oyjlTreeParse( json, NULL, 0 );
  text = _("");
  if(strcmp(text,"") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de_DE.UTF8\":\"%s\"          ", lang, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlTranslate(%s) \"de_DE.UTF8\":\"%s\"          ", lang, text );
  }

  text = _("Example");
  if(strcmp(text,"Beispiel") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de_DE.UTF8\":\"%s\"  ", lang, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de_DE.UTF8\":\"%s\"  ", lang, text );
  }

  text = _("Example2");
  if(strcmp(text,"Beispiel2") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de_DE\":\"%s\"      ", lang, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de_DE\":\"%s\"      ", lang, text );
  }

  text = _("Color");
  if(strcmp(text,"Farbe") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de\":\"%s\"         ", lang, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de\":\"%s\"         ", lang, text );
  }

  text = _("Nonsense");
  if(strcmp(text,"Schmarrn") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de_AT\":\"%s\"      ", lang, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de_AT\":\"%s\"      ", lang, text );
  }

  lang = "de_CH.UTF-8";
  text = _("Nonsense");
  if(strcmp(text,"Schmarrn") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de_AT\":\"%s\"      ", lang, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de_AT\":\"%s\"      ", lang, text );
  }

  lang = "de_DE";
  text = _("Example");
  if(strcmp(text,"Beispiel") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de_DE.UTF8\":\"%s\"  ", lang, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de_DE.UTF8\":\"%s\"  ", lang, text );
  }

  text = _("Color");
  if(strcmp(text,"Farbe") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de\":\"%s\"              ", lang, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de\":\"%s\"              ", lang, text );
  }

  lang = "de";
  text = _("Example");
  if(strcmp(text,"Beispiel") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de_DE.UTF8\":\"%s\"      ", lang, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de_DE.UTF8\":\"%s\"      ", lang, text );
  }

  lang = "de_CH";
  text = _("Color");
  if(strcmp(text,"Farbe") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTranslate(%s) \"de\":\"%s\"              ", lang, text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "oyjlTranslate(%s) \"de\":\"%s\"              ", lang, text );
  }

#undef _
#ifdef OYJL_USE_GETTEXT
# define _(text) dgettext( OYJL_DOMAIN, text )
#else
# define _(text) text
#endif

  setlocale(LC_ALL,"");

  oyjl_val root = oyjlTreeNew( "" );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "Example", "one/[%d]/name", 0 );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "Color", "one/[%d]/description", 0 );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "Example2", "one/[%d]/label", 0 );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "Nonsense", "one/[%d]/help", 0 );
  const char * key_list = "name,description,help,label";
  oyjlTranslateJson( root, "de_DE", catalog, key_list, NULL );
  int i = 0;
  char * txt = NULL;
  oyjlTreeToJson( root, &i, &txt );
  if( txt && strlen( txt ) == 131 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTranslateJson()                             %ld", strlen(txt) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTranslateJson()                             %ld", strlen(txt) );
  }
  OYJL_TEST_WRITE_RESULT( txt, strlen(txt), "oyjlTranslateJson", "txt" )
  if(verbose && txt)
    fprintf( zout, "%s\n", txt );
  myDeAllocFunc( txt ); txt = NULL;
  oyjlTreeFree( root );
  oyjlTreeFree( catalog );


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
    "oyjlDataFormat (%s) = %d guessed                ", data?data:"NULL", format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (%s) = %d guessed                ", data?data:"NULL", format );
  }

  data = "";
  format = oyjlDataFormat(data);
  if(format == -2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed                ", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed                ", data, format );
  }

  data = "  \t ";
  format = oyjlDataFormat(data);
  if(format == -2)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed           ", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed           ", data, format );
  }

  data = "\t XYZ";
  format = oyjlDataFormat(data);
  if(format == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed        ", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed        ", data, format );
  }

  data = " {\"a\":}";
  format = oyjlDataFormat(data);
  if(format == 7)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed             ", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed             ", data, format );
  }

  data = " [{\"a\":}]";
  format = oyjlDataFormat(data);
  if(format == 7)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed          ", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed          ", data, format );
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
    "oyjlDataFormat (\"%s\") = %d guessed         ", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed         ", data, format );
  }

  data = "  \n---\na:";
  format = oyjlDataFormat(data);
  if(format == 9)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed             ", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed             ", data, format );
  }

  data = "---a:";
  format = oyjlDataFormat(data);
  if(format == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed             ", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed             ", data, format );
  }

  data = "  ---\na:";
  format = oyjlDataFormat(data);
  if(format == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlDataFormat (\"%s\") = %d guessed             ", data, format );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlDataFormat (\"%s\") = %d guessed             ", data, format );
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
      char * rjson = 0;
      oyjlTreeToJson( root, &level, &rjson );
      if(json && json[0] && strlen(rjson) == 210 && strcmp(json,rjson) == 0)
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "oyjlTreeToJson()                     %lu", (unsigned long)strlen(rjson) );
        if(verbose) fprintf( zout, "%s\n", rjson );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlTreeToJson()                                " );
      }
      myDeAllocFunc(rjson);

      char ** paths = NULL;
      oyjlTreeToPaths( root, 10, NULL, 0, &paths );
      int count = 0; while(paths && paths[count]) ++count;
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
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "oyjlTreeToPaths()                     %d", count );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlTreeToPaths()                     %d", count );
        for(j = 0; j < count; ++j)
          fprintf( zout, "paths[%d]: %s\n", j, paths[j]);
      }
      if(verbose)
        for(j = 0; j < count; ++j)
          fprintf( zout, "%d: %s\n", j, paths[j] );
      if(paths && count)
        oyjlStringListRelease( &paths, count, free );

      oyjlTreeToPaths( root, 10, NULL, OYJL_KEY, &paths );
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
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "oyjlTreeToPaths( OYJL_KEY )           %d", count );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlTreeToPaths( OYJL_KEY )           %d", count );
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

      oyjlTreeToPaths( root, 10, NULL, OYJL_PATH, &paths );
      count = 0; while(paths && paths[count]) ++count;
      j = 0;
      if(count == 4 &&
          strcmp(paths[j++],"org") == 0 &&
          strcmp(paths[j++],"org/free") == 0 &&
          strcmp(paths[j++],"org/free/[0]") == 0 &&
          strcmp(paths[j++],"org/free/[1]") == 0
        )
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "oyjlTreeToPaths( OYJL_PATH )          %d", count );
      } else
      { PRINT_SUB( oyjlTESTRESULT_FAIL,
        "oyjlTreeToPaths( OYJL_PATH )          %d", count );
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
      { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
        "oyjlValueText(%s) %s", p,
                   oyjlProfilingToString(n,clck/(double)CLOCKS_PER_SEC,"key"));
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
        "oyjlTreeGetValue(flags=%d)            ", flags );
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
          "copy node                             " );
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

  return result;
}

oyjlTESTRESULT_e testFromJson ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  const char * json = "{\"org\":{\"free\":[{\"s1key_a\":\"val_a\",\"s1key_b\":\"val_b\"},{\"@s2attribute_c\":\"val_c\",\"@text\":\"inner string d\"}],\"key_e\":\"val_e\",\"@attribute_f\":\"val_f\",\"dc.node\":{\"@text\":\"inner string g\",\"@attr_h\":\"val_h\"},\"val_i\":1234,\"val_j\":true,\"val_k\":false,\"val_l\":12.34,\"val_m\":[1,2,3,4.5]}}";
  char error_buffer[128];
  char * text = 0;
  int level = 0;
  int format;

  oyjl_val root = 0;

  root = oyjlTreeParse( json, error_buffer, 128 );

  oyjlTreeToJson( root, &level, &text );
  format = oyjlDataFormat(text);
  if(text && text[0] && strlen(text) > 20 && format == 7 && strlen(text) == 406)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeToJson()                     %lu", (unsigned long)strlen(text) );
    if(verbose) fprintf( zout, "%s\n", text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeToJson()                                " );
  }
  myDeAllocFunc(text);

  text = NULL;
  oyjlTreeToYaml( root, &level, &text );
  format = oyjlDataFormat(text);
  if(text && text[0] && strlen(text) > 20 && format == 9 && strlen(text) == 305)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeToYaml()                     %lu", (unsigned long)strlen(text) );
    if(verbose) fprintf( zout, "%s\n", text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeToYaml()                                " );
  }
  myDeAllocFunc(text);

  text = NULL;
  oyjlTreeToXml( root, &level, &text );
  format = oyjlDataFormat(text);
  if(text && text[0] && strlen(text) > 20 && format == 8 && strlen(text) == 436)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeToXml ()                     %lu", (unsigned long)strlen(text) );
    if(verbose) fprintf( zout, "%s\n", text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeToXml ()                                " );
  }
  myDeAllocFunc(text);

  oyjlTreeFree( root );

  return result;
}

oyjlTESTRESULT_e testJsonRoundtrip ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  const char * json = "{\"org\":{\"free\":[{\"s1key_a\":\"val_a\",\"s1key_b\":\"val_b\"},{\"@s2attribute_c\":\"val_c\",\"@text\":\"inner string d\"}],\"key_e\":\"val_e\",\"@xmlns:xsi\":\"http://www.w3.org/2001/XMLSchema-instance\",\"@attribute_f\":\"val_f\",\"xsi:node\":{\"@text\":\"inner string g\",\"@attr_h\":\"val_h\"},\"val_i\":1234,\"val_j\":true,\"val_k\":false,\"val_l\":12.34,\"val_m\":[0.0,1,2,3,4.5]}}";
  char error_buffer[128] = {0};
  char * text = NULL,
       * text_to_xml = NULL,
       * text_from_xml = NULL;
  int level = 0;

  oyjl_val root = 0;

  root = oyjlTreeParse( json, error_buffer, 128 );

  oyjlTreeToJson( root, &level, &text );
  if(text && text[0] && strlen(text) > 20)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeToJson()                     %lu", (unsigned long)strlen(text) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeToJson()                                " );
  }

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
    "oyjlTreeToXml () <-> oyjlTreeParseXml ()        " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeToXml () <-> oyjlTreeParseXml ()        " );
    fprintf( zout, "%s\n", text );
    fprintf( zout, "%s\n", text_from_xml );
  }
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
    "oyjlTreeParseXml () <-> oyjlTreeToXml ()        " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeParseXml () <-> oyjlTreeToXml ()        " );
    fprintf( zout, "%s\n", xml );
    fprintf( zout, "%s\n", text_from_xml );
    fprintf( zout, "%s\n", text_to_xml );
  }
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

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X json", prog );
  len = t ? strlen(t) : 0;
  if(len == json_size)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "%s -X json                       %lu", prog, len );
  } else
  { PRINT_SUB( fail,
    "%s -X json                       %lu", prog, len );
  }
  OYJL_TEST_WRITE_RESULT( t, len, prog, "json" )
  if(verbose && len)
    fprintf( zout, "%s\n", t );
  if(t) {free(t);}

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X json+command", prog );
  len = t ? strlen(t) : 0;
  if(len == json_command_size)
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

  t = oyjlReadCommandF( &size, "r", malloc, "LANG=C ./%s -X export", prog );
  len = t ? strlen(t) : 0;
  if(len == export_size)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "%s -X export                    %lu", prog, len );
  } else
  { PRINT_SUB( fail,
    "%s -X export                    %lu", prog, len );
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
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlUi_ImportFromJson()                %lu", len );
  } else
  { PRINT_SUB( fail,
    "oyjlUi_ImportFromJson()                %lu", len );
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
    "%s roundtrip  of reimported JSON   %s", prog, t );
  } else
  { PRINT_SUB( fail,
    "%s roundtrip  of reimported JSON   %s", prog, t );
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

  setlocale(LC_ALL,"en_GB.UTF8");

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
                                    {"","","",""}};
  oyjlOptionChoice_s o_choices[] = {{"0", _("Print All"), _("Print All"), ""},
                                    {"1", _("Print Camera"), _("Print Camera JSON"), ""},
                                    {"2", _("Print None"), _("Print None"), ""},
                                    {"","","",""}};

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
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ExportToJson()                  %lu", text?strlen(text):0 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ExportToJson()                  %lu", text?strlen(text):0 );
  }
  oyjlOptions_SetAttributes( &opts, NULL );
  oyjlUi_Release( &ui);
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ExportToJson", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );

  oyjl_val json = oyjlTreeParse( text, error_buffer, 128 );
  if(text) {free(text);} text = NULL;

  result = testCode( json, "oiCR"                    /*prog*/,
                           9353                      /*code_size*/,
                           1284                      /*help_size*/,
                           2174                      /*man_size*/,
                           4316                      /*markdown_size*/,
                           7528                      /*json_size*/,
                           7555                      /*json_command_size*/,
                           11682                     /*export_size*/,
                           4144                      /*bash_size*/,
                           result,
                           oyjlTESTRESULT_FAIL       /*fail*/ );


  oyjlTreeFree( json ); json = NULL;

  free(oarray[2].values.choices.list);
  free(oarray[3].values.choices.list);

  return result;
}

#undef _
# define _(text) oyjlTranslate( oyjl_lang_, oyjl_catalog_, text )
oyjlTESTRESULT_e testUiTranslation ()
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
                                    {"","","",""}};
  oyjlOptionChoice_s o_choices[] = {{"0", _("Print All"), _("Print All"), ""},
                                    {"1", _("Print Camera"), _("Print Camera JSON"), ""},
                                    {"2", _("Print None"), _("Print None"), ""},
                                    {"","","",""}};

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
  if(text && strlen(text) == 7511)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ToJson(en_GB)                 %lu", text?strlen(text):0 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ToJson(en_GB)                 %lu", text?strlen(text):0 );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToJson-en_GB", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  text = oyjlUi_ExportToJson( ui, 0 );
  if(text && strlen(text) == 6689)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ExportToJson(en_GB)           %lu", text?strlen(text):0 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ExportToJson(en_GB)           %lu", text?strlen(text):0 );
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
                           8409                      /*code_size*/,
                           1187                      /*help_size*/,
                           2075                      /*man_size*/,
                           3874                      /*markdown_size*/,
                           6961                      /*json_size*/,
                           6993                      /*json_command_size*/,
                           10606                     /*export_size*/,
                           3150                      /*bash_size*/,
                           result,
                           oyjlTESTRESULT_FAIL       /*fail*/ );

  oyjlTreeFree( json ); json = NULL;

  text = oyjlUi_ExportToJson( ui_en, 0 );
  if(text && strlen(text) == 6689)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ExportToJson(copy)            %lu", text?strlen(text):0 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ExportToJson(copy)            %lu", text?strlen(text):0 );
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
  oyjlCatalog( &catalog );

  oyjlLang( "de_DE" );

  ui = oyjlUi_Create( argc_anonymous, argv_anonymous, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups_no_args, NULL );
  text = oyjlUi_ToJson( ui, 0 );
  if(text && strlen(text) == 7517)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ToJson(de)                    %lu", text?strlen(text):0 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ToJson(de)                    %lu", text?strlen(text):0 );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToJson-de", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  text = oyjlUi_ExportToJson( ui, 0 );
  if(text && strlen(text) == 6695)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ExportToJson(de)              %lu", text?strlen(text):0 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ExportToJson(de)              %lu", text?strlen(text):0 );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ExportToJson-de", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );

  error_buffer[0] = 0;
  json = oyjlTreeParse( text, error_buffer, 128 );
  if(text) {free(text);} text = NULL;

  char * c_source = oyjlUiJsonToCode( json, OYJL_SOURCE_CODE_C );
  if(c_source && strlen(c_source) == 8415)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUiJsonToCode(de)                 %lu", c_source?strlen(c_source):0 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUiJsonToCode(de)                 %lu", c_source?strlen(c_source):0 );
  }
  OYJL_TEST_WRITE_RESULT( c_source, strlen(c_source), "oyjlUiJsonToCode-de", "txt" )
  if(verbose && c_source)
    fprintf( zout, "%s\n", c_source );
  if(c_source) {free(c_source);} c_source = NULL;

  oyjlTreeFree( json ); json = NULL;

  catalog = NULL;
  oyjlTranslate_f tr = oyjlTranslate;
  oyjlUi_Translate( ui, "de_DE", oyjlCatalog(&catalog), tr );
  text = oyjlUi_ToJson( ui, 0 );
  if(text && strlen(text) == 7584)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ToJson(de+)                   %lu", text?strlen(text):0 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ToJson(de+)                   %lu", text?strlen(text):0 );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToJson-de", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  oyjlUi_Translate( ui, "back", oyjlCatalog(&catalog), tr );
  text = oyjlUi_ToJson( ui, 0 );
  if(text && strlen(text) == 7511)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ToJson(back)                  %lu", text?strlen(text):0 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ToJson(back)                  %lu", text?strlen(text):0 );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToJson-back", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  text = oyjlUi_ExportToJson( ui, 0 );
  if(text && strlen(text) == 6689)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ExportToJson(back)            %lu", text?strlen(text):0 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ExportToJson(back)            %lu", text?strlen(text):0 );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ExportToJson-back", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  oyjlUi_Translate( ui, "cs_CZ", oyjlCatalog(&catalog), tr );
  text = oyjlUi_ToJson( ui, 0 );
  if(text && strlen(text) == 7612)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ToJson(cs)                    %lu", text?strlen(text):0 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ToJson(cs)                    %lu", text?strlen(text):0 );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToJson-cs", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  text = oyjlUi_ExportToJson( ui, 0 );
  if(text && strlen(text) == 6752)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ExportToJson(cs)              %lu", text?strlen(text):0 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ExportToJson(cs)              %lu", text?strlen(text):0 );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ExportToJson-cs", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  catalog = oyjlCatalog(&catalog);
  int level = 0;
  oyjlTreeToJson( catalog, &level, &text );
  //puts(text);
  if(text && strlen(text) == 2683 && strlen(catalog_json) == 1797)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "catalog: %lu catalog_json: %lu        ", strlen(text), strlen(catalog_json) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "catalog: %lu catalog_json: %lu        ", strlen(text), strlen(catalog_json) );
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

  free(oarray[2].values.choices.list);
  free(oarray[3].values.choices.list);

  return result;
}
/* --- end actual tests --- */


