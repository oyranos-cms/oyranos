/** @file testi-args.c
 *
 *  Oyranos is an open source Color Management System
 *
 *  Copyright (C) 2019  Kai-Uwe Behrmann
 *
 *  @brief    Oyranos test suite
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2019/06/24
 */

#define TESTS_RUN \
  TEST_RUN( testArgs, "Options handling", 1 );

void oyjlLibRelease();
#define OYJL_TEST_MAIN_SETUP  printf("\n    Oyjl Args Test Program\n");
#define OYJL_TEST_MAIN_FINISH printf("\n    Oyjl Args Test Program finished\n\n"); oyjlLibRelease();
#define OYJL_TEST_NAME "test-args"
#include "oyjl_test_main.h"
#include "oyjl.h"
#include "oyjl_version.h"
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>
#endif
#include "oyjl_i18n.h"

#include "oyjl_args.c"

/* --- actual tests --- */


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
  int state = 0;
  int argc = 1;
  const char * argv[] = {"test-args","-v","--input","file-name.json", "-z"};

  /* handle options */
  /* Select from *version*, *manufacturer*, *copyright*, *license*, *url*,
   * *support*, *download*, *sources*, *oyjl_modules_author* and
   * *documentation* what you see fit. Add new ones as needed. */
  oyjlUiHeaderSection_s sections[] = {
    /* type, nick,            label, name,                  description  */
    {"oihs", "version",       NULL,  "1.0",                 NULL},
    {"oihs", "documentation", NULL,  "",                    _("The example tool demontrates the usage of the libOyjl API's.")},
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
    {"oiwi", 0,     "b", NULL,      NULL, "blabla",     "BlaBla",            NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &output} },
    {"oiwi", 0,     NULL,"candle",  NULL, "candle",     "Candle",            NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &output} },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups_no_args[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Mode1"),_("Simple mode"),     NULL, "#",       "o,v",    "o" }, /* accepted even if none of the mandatory options is set */
    {"oiwg", 0,     _("Mode2"),_("Simple mode"),     NULL, "#",       "o,v",    "#,o" }, /* accepted even if none of the mandatory options is set */
    {"oiwg", 0,     _("Mode3"),_("Simple mode"),     NULL, "#",       "",       "#" }, /* accepted even if none of the mandatory options is set */
    {"oiwg", 0,     _("Mode4"),_("Simple mode"),     NULL, "#",       "",       "" }, /* accepted even if none of the mandatory options is set */
    {"oiwg", OYJL_OPTION_FLAG_EDITABLE,_("Mode5"),_("Any arg mode"),NULL,"@","o,v","@,o"},/* accepted if anonymous arguments are set */
    {"oiwg", 0,     _("Mode6"),_("Actual mode"),     NULL, "i",       "o,v",    "i,o" },/* parsed and checked with -i option */
    {"oiwg", 0,     _("Mode7"),_("Alternate"),       NULL, "i|o",     "h|v",    "i,o,h,v" },
    {"oiwg", 0,     _("Mode8"),_("Long"),            NULL, "b",       "candle,v","b,candle,v" },
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

  char * syn = oyjlOptions_PrintHelpSynopsis( ui->opts, &ui->opts->groups[0], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN );
  if(strcmp(syn,"**test-args** | [<strong>-o</strong>=<em>0|1|2</em>] [<strong>-v</strong>]") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "SynopsisMode1  #       o,v    o                " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "SynopsisMode1  #       o,v    o                " );
  }
  OYJL_TEST_WRITE_RESULT( syn, strlen(syn), "SynopsisMode1", "txt" )
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || verbose)
    fprintf( zout, "SynopsisMode1: %s\n", syn );
  free(syn);

  syn = oyjlOptions_PrintHelpSynopsis( ui->opts, &ui->opts->groups[1], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN );
  if(strcmp(syn,"**test-args** | [<strong>-o</strong>=<em>0|1|2</em>] [<strong>-v</strong>]") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "SynopsisMode2  #       o,v    #,o              " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "SynopsisMode2  #       o,v    #,o              " );
  }
  OYJL_TEST_WRITE_RESULT( syn, strlen(syn), "SynopsisMode2", "txt" )
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || verbose)
    fprintf( zout, "SynopsisMode2: %s\n", syn );
  free(syn);

  syn = oyjlOptions_PrintHelpSynopsis( ui->opts, &ui->opts->groups[2], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN );
  if(strcmp(syn,"**test-args**") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "SynopsisMode3  #              #                " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "SynopsisMode3  #              #                " );
  }
  OYJL_TEST_WRITE_RESULT( syn, strlen(syn), "SynopsisMode3", "txt" )
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || verbose)
    fprintf( zout, "SynopsisMode3: %s\n", syn );
  free(syn);

  syn = oyjlOptions_PrintHelpSynopsis( ui->opts, &ui->opts->groups[3], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN );
  if(strcmp(syn,"**test-args**") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "SynopsisMode4  #                               " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "SynopsisMode4  #                               " );
  }
  OYJL_TEST_WRITE_RESULT( syn, strlen(syn), "SynopsisMode4", "txt" )
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || verbose)
    fprintf( zout, "SynopsisMode4: %s\n", syn );
  free(syn);

  syn = oyjlOptions_PrintHelpSynopsis( ui->opts, &ui->opts->groups[4], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN );
  if(strcmp(syn,"**test-args** [<strong>-o</strong>=<em>0|1|2</em>] [<strong>-v</strong>] FILENAME") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "SynopsisMode5  @       o,v    @,o              " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "SynopsisMode5  @       o,v    @,o              " );
  }
  OYJL_TEST_WRITE_RESULT( syn, strlen(syn), "SynopsisMode5", "txt" )
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || verbose)
    fprintf( zout, "SynopsisMode5: %s\n", syn );
  free(syn);

  syn = oyjlOptions_PrintHelpSynopsis( ui->opts, &ui->opts->groups[5], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN );
  if(strcmp(syn,"**test-args** <strong>-i</strong>=<em>FILENAME</em> [<strong>-o</strong>=<em>0|1|2</em>] [<strong>-v</strong>]") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "SynopsisMode6  i       o,v    i,o              " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "SynopsisMode6  i       o,v    i,o              " );
  }
  OYJL_TEST_WRITE_RESULT( syn, strlen(syn), "SynopsisMode6", "txt" )
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || verbose)
    fprintf( zout, "SynopsisMode6: %s\n", syn );
  free(syn);

  syn = oyjlOptions_PrintHelpSynopsis( ui->opts, &ui->opts->groups[6], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN );
  if(strcmp(syn,"**test-args** <strong>-i</strong>=<em>FILENAME</em> | <strong>-o</strong>=<em>0|1|2</em> [<strong>-h</strong>|<strong>-v</strong>]") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "SynopsisMode7  i|o     h|v    i,o,h,v          " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "SynopsisMode7  i|o     h|v    i,o,h,v          " );
  }
  OYJL_TEST_WRITE_RESULT( syn, strlen(syn), "SynopsisMode7", "txt" )
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || verbose)
    fprintf( zout, "SynopsisMode7: %s\n", syn );
  free(syn);

  syn = oyjlOptions_PrintHelpSynopsis( ui->opts, &ui->opts->groups[7], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN );
  if(strcmp(syn,"**test-args** <strong>-b</strong> [<strong>--candle</strong>] [<strong>-v</strong>]") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "SynopsisMode8  b,      candle, b,candle,v      " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "SynopsisMode8  b,      candle, b,candle,v      " );
  }
  OYJL_TEST_WRITE_RESULT( syn, strlen(syn), "SynopsisMode8", "txt" )
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || verbose)
    fprintf( zout, "SynopsisMode8: %s\n", syn );
  free(syn);

  oyjlUi_Release( &ui);


  char * text;
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
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || verbose)
  for(i = 0; i < count; ++i)
    fprintf( zout, "%s\n", results[i] );
  oyjlUi_Release( &ui);
  oyjlStringListRelease( &results, count, 0 );

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Mode"), _("Actual mode"),     NULL, "i",       "o,v",    "i,o" },
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "",        "",       "v,h" },
    {"",0,0,0,0,0,0,0}
  };

  argc = 2;
  ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups, NULL );

  if(ui)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui created - correct args                      " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui created - correct args                      " );
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
  if(ui && file && strcmp(file,"file-name.json") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui created - parse string                      " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui created - parse string                      " );
  }
  oyjlUi_Release( &ui);

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
                                       sections, oarray, groups_no_args, NULL );

  text = oyjlUi_ToMan( ui, 0 );
  if( text && strlen(text) == 2183 &&
      strstr(text, "\n\\-\\-candle\tCandle"))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ToMan() %lu                            ", strlen(text) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ToMan() 2183 == %lu                    ", text ? strlen(text) : 0 );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToMan", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  text = oyjlUi_ToMarkdown( ui, 0 );
  if( text && strlen(text) == 6880 &&
      strstr(text, "><strong>--candle</strong><") )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ToMarkdown() %lu                       ", strlen(text) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ToMarkdown() %lu                       ", text?strlen(text):0 );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToMarkdown", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text); text = NULL;}

  if(ui)
    text = oyjlOptions_ResultsToText( ui->opts );
  if(text && strlen(text) == 21)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_ResultsToText() %lu                 ", strlen(text) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_ResultsToText() %lu                 ", text?strlen(text):0 );
  }
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  if(verbose)
  {
    fprintf(stdout, "Help text -> stderr:\n" );
    oyjlOptions_PrintHelp( ui->opts, ui, 1, "%s v%s - %s", argv[0],
                            "1.0", "Test Tool for testing" );
  }

  oyjlUi_Release( &ui);
  char * wrong = "test";
  fprintf(stdout, "oyjlUi_Release(&\"test\") - should give a warning message:\n" );
  oyjlUi_Release( (oyjlUi_s **)&wrong);

  free(oarray[2].values.choices.list);
  free(oarray[3].values.choices.list);

  return result;
}


/* --- end actual tests --- */


