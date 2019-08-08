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
  TEST_RUN( testDataFormat, "Data Format Detection", 1 ); \
  TEST_RUN( testJson, "JSON handling", 1 ); \
  TEST_RUN( testFromJson, "Data Writers", 1 ); \
  TEST_RUN( testJsonRoundtrip, "Data Readers", 1 ); \
  TEST_RUN( testUiRoundtrip, "Ui Export", 1 );

#define OYJL_TEST_MAIN_SETUP  printf("\n    Oyjl Test Program\n");
#define OYJL_TEST_MAIN_FINISH printf("\n    Oyjl Test Program finished\n\n");
#define OYJL_TEST_NAME "test"
#include "oyjl_test_main.h"
#include "oyjl_version.h"
#include "oyjl.h"
#ifdef OYJL_HAVE_LOCALE_H
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
      for(int j = 0; j < count; ++j)
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
  if(text && text_from_xml && strlen(text_from_xml) == strlen(text))
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

#include "oyjl_i18n.h"
oyjlTESTRESULT_e testUiRoundtrip ()
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

  /* handle options */
  /* Select from *version*, *manufacturer*, *copyright*, *license*, *url*,
   * *support*, *download*, *sources*, *oyjl_modules_author* and
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
    {"oiwg", 0,     _("Mode1"),_("Simple mode"),     NULL, "#",       "o,v",    "o" }, /* accepted even if none of the mandatory options is set */
    {"oiwg", OYJL_OPTION_FLAG_EDITABLE,_("Mode2"),_("Any arg mode"),NULL,"@","o,v","@,o"},/* accepted if anonymous arguments are set */
    {"oiwg", 0,     _("Mode3"),_("Actual mode"),     NULL, "i",       "o,v",    "i,o" },/* parsed and checked with -i option */
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "",        "",       "v,h" },/* just show in documentation */
    {"",0,0,0,0,0,0,0}
  };

  const char * argv_anonymous[] = {"test","-v","file-name.json","file-name2.json"};
  int argc_anonymous = 4;
  oyjlUi_s * ui = oyjlUi_Create( argc_anonymous, argv_anonymous, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups_no_args, NULL );
  char * text = oyjlUi_ExportToJson( ui, 0 );
  if(text && strlen(text) == 6686)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ExportToJson()                %lu", text?strlen(text):0 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ExportToJson()                %lu", text?strlen(text):0 );
  }
  oyjlUi_Release( &ui);
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ExportToJson", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );

  char error_buffer[128] = {0};
  oyjl_val json = oyjlTreeParse( text, error_buffer, 128 );
  if(text) {free(text);} text = NULL;

  char * c_source = oyjlUiJsonToCode( json, OYJL_SOURCE_CODE_C );
  if(c_source && strlen(c_source) == 5988)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUiJsonToCode()                   %lu", c_source?strlen(c_source):0 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUiJsonToCode()                   %lu", c_source?strlen(c_source):0 );
  }
  OYJL_TEST_WRITE_RESULT( c_source, strlen(c_source), "oyjlUi_ExportToCode", "txt" )
  if(verbose && c_source)
    fprintf( zout, "%s\n", c_source );
  if(c_source) {free(c_source);} c_source = NULL;


  return result;
}

/* --- end actual tests --- */


