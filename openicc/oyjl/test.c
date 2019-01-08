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
  TEST_RUN( testJson, "JSON handling", 1 ); \
  TEST_RUN( testFromJson, "Data Writers", 1 ); \
  TEST_RUN( testJsonRoundtrip, "Data Readers", 1 );

#include "oyjl_test_main.h"
#include "oyjl_version.h"
#include "oyjl.h"
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

oyjlTESTRESULT_e testFromJson ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  const char * json = "{\"org\":{\"free\":[{\"s1key_a\":\"val_a\",\"s1key_b\":\"val_b\"},{\"@s2attribute_c\":\"val_c\",\"@text\":\"inner string d\"}],\"key_e\":\"val_e\",\"@attribute_f\":\"val_f\",\"dc.node\":{\"@text\":\"inner string g\",\"@attr_h\":\"val_h\"},\"val_i\":1234,\"val_j\":true,\"val_k\":false,\"val_l\":12.34,\"val_m\":[1,2,3,4.5]}}";
  char error_buffer[128];
  char * text = 0;
  int level = 0;

  oyjl_val root = 0;

  root = oyjlTreeParse( json, error_buffer, 128 );

  oyjlTreeToJson( root, &level, &text );
  if(text && text[0] && strlen(text) > 20)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeToJson()                     %lu", (unsigned long)strlen(text) );
    fprintf( zout, "%s\n", text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeToJson()                                " );
  }
  myDeAllocFunc(text);

  text = NULL;
  oyjlTreeToYaml( root, &level, &text );
  if(text && text[0] && strlen(text) > 20)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeToYaml()                     %lu", (unsigned long)strlen(text) );
    fprintf( zout, "%s\n", text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlTreeToYaml()                                " );
  }
  myDeAllocFunc(text);

  text = NULL;
  oyjlTreeToXml( root, &level, &text );
  if(text && text[0] && strlen(text) > 20)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlTreeToXml ()                     %lu", (unsigned long)strlen(text) );
    fprintf( zout, "%s\n", text );
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


/* --- end actual tests --- */


