/** @file testi-args.c
 *
 *  Oyranos is an open source Color Management System
 *
 *  Copyright (C) 2019-2021  Kai-Uwe Behrmann
 *
 *  @brief    Oyranos test suite
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2019/06/24
 */

#define TESTS_RUN \
  TEST_RUN( testArgsPrint, "Options print", 1 ); \
  TEST_RUN( testArgsCheck, "Options checking", 1 ); \
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

char *    oyjlReadFile( const char * file_name,
                        int        * size_ptr );
/* --- actual tests --- */

oyjlTESTRESULT_e testArgsPrintOne( oyjlOption_s * o,
                                   int style,
                                   const char * res,
                                   const char * res_man,
                                   const char * res_mark,
                                   const char * res_o,
                                   const char * res_o_man,
                                   const char * res_o_mark,
                                   const char * res_opt,
                                   const char * res_opt_man,
                                   const char * res_opt_mark,
                                   const char * res_oopt,
                                   const char * res_oopt_man,
                                   const char * res_oopt_mark,
                                   oyjlTESTRESULT_e result,
                                   oyjlTESTRESULT_e fail )
{
  char * txt;

  txt = oyjlOption_PrintArg( o, style );
  if((txt && res && strcmp(txt,res) == 0) || (!txt && !res))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOption_PrintArg() \"%s\"            ", txt?txt:"(none)" );
  } else
  { PRINT_SUB( fail, 
    "oyjlOption_PrintArg() \"%s\"            ", txt?txt:"(none)" );
  }
  if(txt) {free(txt); txt = NULL;}

  txt = oyjlOption_PrintArg( o, style | oyjlOPTIONSTYLE_MAN );
  if((txt && res_man && strcmp(txt,res_man) == 0) || (!txt && !res_man))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOption_PrintArg() \"%s\"", txt?txt:"(none)" );
  } else
  { PRINT_SUB( fail, 
    "oyjlOption_PrintArg() \"%s\"", txt?txt:"(none)" );
  }
  if(txt) {free(txt); txt = NULL;}

  txt = oyjlOption_PrintArg( o, style | oyjlOPTIONSTYLE_MARKDOWN );
  if((txt && res_mark && strcmp(txt,res_mark) == 0) || (!txt && !res_mark))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOption_PrintArg() \"%s\"", txt?txt:"(none)" );
  } else
  { PRINT_SUB( fail, 
    "oyjlOption_PrintArg() \"%s\"", txt?txt:"(none)" );
  }
  if(txt) {free(txt); txt = NULL;}

  txt = oyjlOption_PrintArg( o, style | oyjlOPTIONSTYLE_ONELETTER );
  if((txt && res_o && strcmp(txt,res_o) == 0) || (!txt && !res_o))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOption_PrintArg() \"%s\"            ", txt?txt:"(none)" );
  } else
  { PRINT_SUB( fail, 
    "oyjlOption_PrintArg() \"%s\"            ", txt?txt:"(none)" );
  }
  if(txt) {free(txt); txt = NULL;}

  txt = oyjlOption_PrintArg( o, style | oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MAN );
  if((txt && res_o_man && strcmp(txt,res_o_man) == 0) || (!txt && !res_o_man))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOption_PrintArg() \"%s\"", txt?txt:"(none)" );
  } else
  { PRINT_SUB( fail, 
    "oyjlOption_PrintArg() \"%s\"", txt?txt:"(none)" );
  }
  if(txt) {free(txt); txt = NULL;}

  txt = oyjlOption_PrintArg( o, style | oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN );
  if((txt && res_o_mark && strcmp(txt,res_o_mark) == 0) || (!txt && !res_o_mark))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOption_PrintArg() \"%s\"", txt?txt:"(none)" );
  } else
  { PRINT_SUB( fail, 
    "oyjlOption_PrintArg() \"%s\"", txt?txt:"(none)" );
  }
  if(txt) {free(txt); txt = NULL;}

  txt = oyjlOption_PrintArg( o, style | oyjlOPTIONSTYLE_STRING );
  if((txt && res_opt && strcmp(txt,res_opt) == 0) || (!txt && !res_opt))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOption_PrintArg() \"%s\"            ", txt?txt:"(none)" );
  } else
  { PRINT_SUB( fail, 
    "oyjlOption_PrintArg() \"%s\"            ", txt?txt:"(none)" );
  }
  if(txt) {free(txt); txt = NULL;}

  txt = oyjlOption_PrintArg( o, style | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MAN );
  if((txt && res_opt_man && strcmp(txt,res_opt_man) == 0) || (!txt && !res_opt_man))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOption_PrintArg() \"%s\"", txt?txt:"(none)" );
  } else
  { PRINT_SUB( fail, 
    "oyjlOption_PrintArg() \"%s\"", txt?txt:"(none)" );
  }
  if(txt) {free(txt); txt = NULL;}

  txt = oyjlOption_PrintArg( o, style | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MARKDOWN );
  if((txt && res_opt_mark && strcmp(txt,res_opt_mark) == 0) || (!txt && !res_opt_mark))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOption_PrintArg() \"%s\"", txt?txt:"(none)" );
  } else
  { PRINT_SUB( fail, 
    "oyjlOption_PrintArg() \"%s\"", txt?txt:"(none)" );
  }
  if(txt) {free(txt); txt = NULL;}

  txt = oyjlOption_PrintArg( o, style | oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING );
  if((txt && res_oopt && strcmp(txt,res_oopt) == 0) || (!txt && !res_oopt))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOption_PrintArg() \"%s\"            ", txt?txt:"(none)" );
  } else
  { PRINT_SUB( fail, 
    "oyjlOption_PrintArg() \"%s\"            ", txt?txt:"(none)" );
  }
  if(txt) {free(txt); txt = NULL;}

  txt = oyjlOption_PrintArg( o, style | oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MAN );
  if((txt && res_oopt_man && strcmp(txt,res_oopt_man) == 0) || (!txt && !res_oopt_man))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOption_PrintArg() \"%s\"", txt?txt:"(none)" );
  } else
  { PRINT_SUB( fail, 
    "oyjlOption_PrintArg() \"%s\"", txt?txt:"(none)" );
  }
  if(txt) {free(txt); txt = NULL;}

  txt = oyjlOption_PrintArg( o, style | oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MARKDOWN );
  if((txt && res_oopt_mark && strcmp(txt,res_oopt_mark) == 0) || (!txt && !res_oopt_mark))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOption_PrintArg() \"%s\"", txt?txt:"(none)" );
  } else
  { PRINT_SUB( fail, 
    "oyjlOption_PrintArg() \"%s\"", txt?txt:"(none)" );
  }
  if(txt) {free(txt); txt = NULL;}

  return result;
}

oyjlTESTRESULT_e testArgsPrint()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );
  setlocale(LC_ALL,"en_GB.UTF8");

  char * txt;
  int style = 0;
  oyjlOption_s * o = NULL;
  txt = oyjlOption_PrintArg( o, style );
  if(txt && strcmp(txt,"") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOption_PrintArg() %s                       ", txt?txt:"---" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOption_PrintArg() %s                       ", txt?txt:"---" );
  }
  if(txt) {free(txt); txt = NULL;}

  setenv("FORCE_NO_COLORTERM", "1",1);
  oyjlOption_s o1 = {
    "oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG, "o", "option", NULL, "Option", "Option Description", "Some Help", "args", oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s = NULL}
  };

  result = testArgsPrintOne( &o1, 0,
                             "[=args]",            "\\fI[=args]\\fR",                                  "<em>[=args]</em>",
                             "-o[=args]",          "\\fB\\-o\\fR\\fI[=args]\\fR",                      "<strong>-o</strong><em>[=args]</em>",
                             "--option[=args]",    "\\fB\\-\\-option\\fR\\fI[=args]\\fR",              "<strong>--option</strong><em>[=args]</em>",
                             "-o|--option[=args]", "\\fB\\-o\\fR|\\fB\\-\\-option\\fR\\fI[=args]\\fR", "<strong>-o</strong>|<strong>--option</strong><em>[=args]</em>",
                             result, oyjlTESTRESULT_FAIL );
  fprintf(stdout, "OYJL_OPTION_FLAG_ACCEPT_NO_ARG, oyjlOPTIONSTYLE_OPTIONAL_START | oyjlOPTIONSTYLE_OPTIONAL_END\n" );
  result = testArgsPrintOne( &o1, oyjlOPTIONSTYLE_OPTIONAL_START | oyjlOPTIONSTYLE_OPTIONAL_END,
                             "[[=args]]",            "[\\fI[=args]\\fR]",                                  "[<em>[=args]</em>]",
                             "[-o[=args]]",          "[\\fB\\-o\\fR\\fI[=args]\\fR]",                      "[<strong>-o</strong><em>[=args]</em>]",
                             "[--option[=args]]",    "[\\fB\\-\\-option\\fR\\fI[=args]\\fR]",              "[<strong>--option</strong><em>[=args]</em>]",
                             "[-o|--option[=args]]", "[\\fB\\-o\\fR|\\fB\\-\\-option\\fR\\fI[=args]\\fR]", "[<strong>-o</strong>|<strong>--option</strong><em>[=args]</em>]",
                             result, oyjlTESTRESULT_FAIL );
  o1.flags = OYJL_OPTION_FLAG_REPETITION | OYJL_OPTION_FLAG_ACCEPT_NO_ARG;
  fprintf(stdout, "OYJL_OPTION_FLAG_REPETITION | OYJL_OPTION_FLAG_ACCEPT_NO_ARG, oyjlOPTIONSTYLE_OPTIONAL_START | oyjlOPTIONSTYLE_OPTIONAL_END\n" );
  result = testArgsPrintOne( &o1, oyjlOPTIONSTYLE_OPTIONAL_START | oyjlOPTIONSTYLE_OPTIONAL_END,
                             "[[=args] ...]",            "[\\fI[=args]\\fR ...]",                                  "[<em>[=args]</em> ...]",
                             "[-o[=args] ...]",          "[\\fB\\-o\\fR\\fI[=args]\\fR ...]",                      "[<strong>-o</strong><em>[=args]</em> ...]",
                             "[--option[=args] ...]",    "[\\fB\\-\\-option\\fR\\fI[=args]\\fR ...]",              "[<strong>--option</strong><em>[=args]</em> ...]",
                             "[-o|--option[=args] ...]", "[\\fB\\-o\\fR|\\fB\\-\\-option\\fR\\fI[=args]\\fR ...]", "[<strong>-o</strong>|<strong>--option</strong><em>[=args]</em> ...]",
                             result, oyjlTESTRESULT_FAIL );
  fprintf(stdout, "OYJL_OPTION_FLAG_REPETITION | OYJL_OPTION_FLAG_ACCEPT_NO_ARG, OYJL_GROUP_FLAG_SUBCOMMAND\n" );
  result = testArgsPrintOne( &o1, OYJL_GROUP_FLAG_SUBCOMMAND,
                             "[=args] ...",       "\\fI[=args]\\fR ...",               "<em>[=args]</em> ...",
                             "option[=args] ...", "\\fBoption\\fR\\fI[=args]\\fR ...", "<strong>option</strong><em>[=args]</em> ...",
                             "option[=args] ...", "\\fBoption\\fR\\fI[=args]\\fR ...", "<strong>option</strong><em>[=args]</em> ...",
                             "option[=args] ...", "\\fBoption\\fR\\fI[=args]\\fR ...", "<strong>option</strong><em>[=args]</em> ...",
                             result, oyjlTESTRESULT_XFAIL );

  fprintf(stdout, "OYJL_OPTION_FLAG_EDITABLE | OYJL_OPTION_FLAG_ACCEPT_NO_ARG, 0\n" );
  o1.flags = OYJL_OPTION_FLAG_ACCEPT_NO_ARG | OYJL_OPTION_FLAG_EDITABLE;
  result = testArgsPrintOne( &o1, 0,
                             "[=args...]",            "\\fI[=args...]\\fR",                                  "<em>[=args...]</em>",
                             "-o[=args...]",          "\\fB\\-o\\fR\\fI[=args...]\\fR",                      "<strong>-o</strong><em>[=args...]</em>",
                             "--option[=args...]",    "\\fB\\-\\-option\\fR\\fI[=args...]\\fR",              "<strong>--option</strong><em>[=args...]</em>",
                             "-o|--option[=args...]", "\\fB\\-o\\fR|\\fB\\-\\-option\\fR\\fI[=args...]\\fR", "<strong>-o</strong>|<strong>--option</strong><em>[=args...]</em>",
                             result, oyjlTESTRESULT_FAIL );

  fprintf(stdout, "\"#\" OYJL_OPTION_FLAG_EDITABLE | OYJL_OPTION_FLAG_ACCEPT_NO_ARG, 0\n" );
  o1.o = "#"; o1.option = "", o1.value_type = oyjlOPTIONTYPE_NONE; o1.value_name = NULL;
  result = testArgsPrintOne( &o1, 0,
                             NULL,                    NULL,                                                  NULL,
                             "|",                     "|",                                                   "|",
                             "|",                     "|",                                                   "|",
                             "|",                     "|",                                                   "|",
                             result, oyjlTESTRESULT_FAIL );

  fprintf(stdout, "\"@\" OYJL_OPTION_FLAG_EDITABLE, 0\n" );
  o1.o = "@"; o1.option = "", o1.value_type = oyjlOPTIONTYPE_CHOICE; o1.value_name = "ARG";
  o1.flags = OYJL_OPTION_FLAG_EDITABLE;
  result = testArgsPrintOne( &o1, 0,
                             "ARG",                   "\\fIARG\\fR",                                         "<em>ARG</em>",
                             "ARG",                   "\\fIARG\\fR",                                         "<em>ARG</em>",
                             "ARG",                   "\\fIARG\\fR",                                         "<em>ARG</em>",
                             "ARG",                   "\\fIARG\\fR",                                         "<em>ARG</em>",
                             result, oyjlTESTRESULT_FAIL );

  return result;
}

oyjlTESTRESULT_e testArgsCheck()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );
  setlocale(LC_ALL,"en_GB.UTF8");

  char * group = NULL;
  const char * option = "opt";
  const char * delimiter = ",";
  int flags = verbose ? 0 : OYJL_QUIET;
  int error = oyjlManAddOptionToGroup_( &group, 0, option, delimiter, flags );
  if(error == 0 && group && strcmp(group, "opt") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlManAddOptionToGroup_(NULL,\"opt\")           " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlManAddOptionToGroup_(NULL,\"opt\")           " );
  }

  error = oyjlManAddOptionToGroup_( &group, 0, option, delimiter, flags );
  if(error == 1 && group && strcmp(group, "opt") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlManAddOptionToGroup_(\"opt\" ,\"opt\")         " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlManAddOptionToGroup_(\"opt\" ,\"opt\")         " );
  }

  error = oyjlManAddOptionToGroup_( &group, 0, NULL, delimiter, flags );
  if(error == 0 && group && strcmp(group, "opt") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlManAddOptionToGroup_(NULL ,NULL)         " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlManAddOptionToGroup_(NULL ,NULL) = %s    ", group );
  }

  option = "pt";
  error = oyjlManAddOptionToGroup_( &group, 0, option, delimiter, flags );
  if(error == 0 && group && strcmp(group, "opt,pt") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlManAddOptionToGroup_(\"opt\" ,\"pt\") = %s ", group );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlManAddOptionToGroup_(\"opt\" ,\"pt\") = %s ", group );
  }

  option = "o";
  error = oyjlManAddOptionToGroup_( &group, 0, option, delimiter, flags );
  if(error == 0 && group && strcmp(group, "opt,pt,o") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlManAddOptionToGroup_(\"opt\" ,\"pt\") = %s", group );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlManAddOptionToGroup_(\"opt\" ,\"pt\") = %s", group );
  }
  if(group) { free(group); group = NULL; }

  group = oyjlStringCopy("d|device-name,short,path", 0);
  option = NULL;
  error = oyjlManAddOptionToGroup_( &group, 'r', option, delimiter, flags );
  if(error == 0 && group && strcmp(group, "d|device-name,short,path,r") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlManAddOptionToGroup_(\"d|device-name,short,path\" ,\"r\") = %s", group );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlManAddOptionToGroup_(\"d|device-name,short,path\" ,\"r\") = %s", group );
  }
  if(group) { free(group); group = NULL; }

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
  double degree = 0;
  int help = 0;
  int verbose_ = 0;
  int state = 0;
  int argc = 1;
  const char * argv[] = {"test-args","-vvv","--input","file-name.json", "-z"};

  /* handle options */
  /* Select from *version*, *manufacturer*, *copyright*, *license*, *url*,
   * *support*, *download*, *sources*, *oyjl_module_author* and
   * *documentation* what you see fit. Add new ones as needed. */
  oyjlUiHeaderSection_s sections_static[] = {
    /* type, nick,            label, name,                  description  */
    {"oihs", "version",       NULL,  "1.0",                 NULL},
    {"oihs", "documentation", NULL,  "",                    _("The example tool demontrates the usage of the libOyjl API's.")},
    {"oihs", "date",          NULL,  "2018-10-10T12:00:00", _("October 10, 2018")},
    {"",0,0,0,0}},
  /* use newline separated permissions in name + write newline separated list in description; both lists need to match in count */
                      * sections = oyjlUiHeaderSection_Append( sections_static, "permissions", NULL,
                                     "android.permission.READ_EXTERNAL_STORAGE\nandroid.permission.WRITE_EXTERNAL_STORAGE",
                                     _("Read external storage for global data access, like downloads, music ...\nWrite external storage to create and modify global data.") );

  /* declare some option choices */
  oyjlOptionChoice_s i_choices[] = {{"oyjl.json", _("oyjl.json"), "First file", ""},
                                    {"oyjl2.json", _("oyjl2.json"), "Second file", ""},
                                    {"","","",""}};
  oyjlOptionChoice_s o_choices[] = {{"0", _("Print All"), _("All Device Classes"), ""},
                                    {"1", _("Print Camera"), _("JSON"), ""},
                                    {"2", _("Print None"), "", ""},
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
    {"oiwi", 0,     NULL,"candle",  NULL, "candle",     "Candle",            _("This options explanation text."), NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &output} },
    {"oiwi", 0,     NULL,"degree",  NULL, "Degree",     "Degree",            _("This options explanation text."), "DEGREE",      oyjlOPTIONTYPE_DOUBLE, {.dbl.d = 0, .dbl.start = 0, .dbl.end = 25000, .dbl.tick = 100}, oyjlDOUBLE, {.d = &degree} },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups_no_args[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Mode1"),_("Simple mode"),     NULL, "#",       "o,v",    "o" }, /* accepted even if none of the mandatory options is set */
    {"oiwg", 0,     _("Mode2"),_("Simple mode"),     NULL, "#",       "o,v",    "#,o" }, /* accepted even if none of the mandatory options is set */
    {"oiwg", 0,     _("Mode3"),_("Simple mode"),     NULL, "#",       "",       "#" }, /* accepted even if none of the mandatory options is set */
    {"oiwg", 0,     _("Mode4"),_("Simple mode"),     NULL, "#",       "",       "" }, /* accepted even if none of the mandatory options is set */
    {"oiwg", 0,     _("Mode5"),_("Any arg mode"),    NULL, "@",       "o,v",    "@,o"},/* accepted if anonymous arguments are set */
    {"oiwg", 0,     _("Mode6"),_("Actual mode"),     NULL, "i",       "o,v",    "i,o" },/* parsed and checked with -i option */
    {"oiwg", 0,     _("Mode7"),_("Alternate"),       NULL, "i|o",     "h|v",    "i,o,h,v" },
    {"oiwg", 0,     _("Mode8"),_("Long"),            _("This Group handles Long options"), "b",       "candle,degree,v","b,candle,degree,v" },
    {"oiwg", 0,     _("Mode9"),_("Double mandatory"),_("This Group checks correct constraints."), "b,candle",  "v","b,candle,v" },
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "",        "",       "v,h" },/* just show in documentation */
    {"",0,0,0,0,0,0,0}
  };

  /* tell about the tool */
  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups_no_args, NULL );
  /* done with options handling */

  const char * fn = "test-args-0-help.txt"; remove(fn);
  oyjl_help_zout = fopen(fn, "w");
  oyjlOptions_PrintHelp( ui->opts, ui, 1, "%s v%s - %s", argv[0],
                            "1.0", "Test Tool for testing" );
  fclose(oyjl_help_zout); oyjl_help_zout = NULL;
  int size = 0;
  char * text;
  text = oyjlReadFile( fn , &size );
  if(ui && size == 2490)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui created - no args %d                      ", size );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui created - no args %d                      ", size );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlOptions_PrintHelp", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  char * syn = ui ? oyjlOptions_PrintHelpSynopsis_( ui->opts, &ui->opts->groups[0], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN ) : NULL;
  if(syn && strcmp(syn,"<strong>test-args</strong> | [<strong>-o</strong>=<em>0|1|2</em>] [<strong>-v</strong>]") == 0)
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

  syn = ui ? oyjlOptions_PrintHelpSynopsis_( ui->opts, &ui->opts->groups[1], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN ) : NULL;
  if(syn && strcmp(syn,"<strong>test-args</strong> | [<strong>-o</strong>=<em>0|1|2</em>] [<strong>-v</strong>]") == 0)
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

  syn = ui ? oyjlOptions_PrintHelpSynopsis_( ui->opts, &ui->opts->groups[2], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN ) : NULL;
  if(syn && strcmp(syn,"<strong>test-args</strong>") == 0)
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

  syn = ui ? oyjlOptions_PrintHelpSynopsis_( ui->opts, &ui->opts->groups[3], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN ) : NULL;
  if(syn && strcmp(syn,"<strong>test-args</strong>") == 0)
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

  syn = ui ? oyjlOptions_PrintHelpSynopsis_( ui->opts, &ui->opts->groups[4], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN ) : NULL;
  if(syn && strcmp(syn,"<strong>test-args</strong> [<strong>-o</strong>=<em>0|1|2</em>] [<strong>-v</strong>] FILENAME") == 0)
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

  syn = ui ? oyjlOptions_PrintHelpSynopsis_( ui->opts, &ui->opts->groups[5], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN ) : NULL;
  if(syn && strcmp(syn,"<strong>test-args</strong> <strong>-i</strong>=<em>FILENAME</em> [<strong>-o</strong>=<em>0|1|2</em>] [<strong>-v</strong>]") == 0)
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

  syn = ui ? oyjlOptions_PrintHelpSynopsis_( ui->opts, &ui->opts->groups[6], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN ) : NULL;
  if(syn && strcmp(syn,"<strong>test-args</strong> <strong>-i</strong>=<em>FILENAME</em> | <strong>-o</strong>=<em>0|1|2</em> [<strong>-h</strong>|<strong>-v</strong>]") == 0)
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

  syn = ui ? oyjlOptions_PrintHelpSynopsis_( ui->opts, &ui->opts->groups[7], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN ) : NULL;
  if(syn && strcmp(syn,"<strong>test-args</strong> <strong>-b</strong> [<strong>--candle</strong>] [<strong>--degree</strong>=<em>DEGREE</em>] [<strong>-v</strong>]") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "SynopsisMode8  b,      candle,degree,v, b,candle,degree,v" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "SynopsisMode8  b,      candle,degree,v, b,candle,degree,v" );
  }
  OYJL_TEST_WRITE_RESULT( syn, strlen(syn), "SynopsisMode8", "txt" )
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || verbose)
    fprintf( zout, "SynopsisMode8: %s\n", syn );
  free(syn);

  syn = ui ? oyjlOptions_PrintHelpSynopsis_( ui->opts, &ui->opts->groups[8], oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN ) : NULL;
  if(syn && strcmp(syn,"<strong>test-args</strong> <strong>-b</strong> <strong>--candle</strong> [<strong>-v</strong>]") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "SynopsisMode9  b,candle       b,candle,v      " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "SynopsisMode9  b,      candle, b,candle,v      " );
  }
  OYJL_TEST_WRITE_RESULT( syn, strlen(syn), "SynopsisMode9", "txt" )
  if(oy_test_last_result == oyjlTESTRESULT_FAIL || verbose)
    fprintf( zout, "SynopsisMode9: %s\n", syn );
  free(syn);

  oyjlUi_ReleaseArgs( &ui);

  const char * argv_multiple_mandatory[] = {"test","-b","--candle"};
  int argc_multiple_mandatory = 2;
  ui = oyjlUi_Create( argc_multiple_mandatory, argv_multiple_mandatory, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups_no_args, NULL );
  if(ui)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui created - multiple mandatory args; argc = %d ", argc_multiple_mandatory );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui created - multiple mandatory args; argc = %d ", argc_multiple_mandatory );
  }
  oyjlUi_ReleaseArgs( &ui);

  ++argc_multiple_mandatory;
  ui = oyjlUi_Create( argc_multiple_mandatory, argv_multiple_mandatory, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups_no_args, NULL );
  if(ui)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui created - multiple mandatory args; argc = %d ", argc_multiple_mandatory );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui created - multiple mandatory args; argc = %d ", argc_multiple_mandatory );
  }
  oyjlUi_ReleaseArgs( &ui);


  const char * argv_anonymous[] = {"test","-v","file-name.json","file-name2.json", "--candle", "--degree=200"};
  int argc_anonymous = 6;
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
  oyjlStringListRelease( &results, count, 0 );

  count = 0;
  char ** text_array = oyjlOptions_ResultsToList( ui->opts, NULL, &count );
  if( text_array && count == 5 &&
      strcmp(text_array[0], "-v") == 0 &&
      strcmp(text_array[1], "-@=file-name.json") == 0 &&
      strcmp(text_array[2], "-@=file-name2.json") == 0 &&
      strcmp(text_array[3], "--candle") == 0 &&
      strcmp(text_array[4], "--degree=200") == 0
      )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_ResultsToList() %d                  ", count );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_ResultsToList() %d                  ", count );
  }
  if(text_array)
  {
    if(oy_test_last_result != oyjlTESTRESULT_SUCCESS || verbose)
      for(i = 0; i < count; ++i)
        fprintf( stdout, "[%d]:\t%s\n", i, text_array[i] );
  }
  if(oy_test_last_result == oyjlTESTRESULT_SUCCESS)
  {
    oyjlOption_s * v = oyjlOptions_GetOptionL( ui->opts, text_array[0], 0/* flags */ ), /* "-v" single letter opt */
                 * at = oyjlOptions_GetOptionL( ui->opts, text_array[1], 0/* flags */ ), /* "-@=file-name.json" non cli opt with arg */
                 * candle = oyjlOptions_GetOptionL( ui->opts, text_array[3], 0/* flags */ ), /* "--candle" long opt */
                 * degree = oyjlOptions_GetOptionL( ui->opts, text_array[4], 0/* flags */ ); /* "--degree=200" long opt */
    if(v && strcmp(v->o,"v") == 0)
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
      "oyjlOptions_GetOptionL(\"%s\")                 ", text_array[0] );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "oyjlOptions_GetOptionL(\"%s\")                 ", text_array[0] );
    }
    if(at && strcmp(at->o,"@") == 0)
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
      "oyjlOptions_GetOptionL(\"%s\")        ", text_array[1] );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "oyjlOptions_GetOptionL(\"%s\")        ", text_array[1] );
    }
    if(candle && strcmp(candle->option,"candle") == 0)
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
      "oyjlOptions_GetOptionL(\"%s\")                 ", text_array[3] );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "oyjlOptions_GetOptionL(\"%s\")                 ", text_array[3] );
    }
    if(candle && strcmp(degree->option,"degree") == 0)
    { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
      "oyjlOptions_GetOptionL(\"%s\")             ", text_array[4] );
    } else
    { PRINT_SUB( oyjlTESTRESULT_FAIL, 
      "oyjlOptions_GetOptionL(\"%s\")             ", text_array[4] );
    }
  }

  int group = 1;
  int found = oyjlOptions_GroupHasOptionL_( ui->opts, group, text_array[0] );
  if(found > 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_GroupHasOptionL_( %d,%s ) = %d       ", group, text_array[0], found );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_GroupHasOptionL_( %d,%s ) = %d       ", group, text_array[0], found );
  }
  group = 4;
  found = oyjlOptions_GroupHasOptionL_( ui->opts, group, text_array[1] );
  if(found > 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_GroupHasOptionL_( %d,%s)=%d", group, text_array[1], found );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_GroupHasOptionL_( %d,%s)=%d", group, text_array[1], found );
  }
  group = 7;
  found = oyjlOptions_GroupHasOptionL_( ui->opts, group, text_array[3] );
  if(found > 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_GroupHasOptionL_( %d,%s ) = %d", group, text_array[3], found );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_GroupHasOptionL_( %d,%s ) = %d", group, text_array[3], found );
  }
  found = oyjlOptions_GroupHasOptionL_( ui->opts, group, text_array[4] );
  if(found > 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_GroupHasOptionL_( %d,%s ) = %d", group, text_array[4], found );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_GroupHasOptionL_( %d,%s ) = %d", group, text_array[4], found );
  }

  int n = -1;
  double d = -1.0;
  const char * s = "-1";
  int pos = 0;
  oyjlOPTIONSTATE_e st;
  st = oyjlOptions_GetResult( ui->opts, text_array[pos], NULL, NULL, &n );
  if(st == oyjlOPTION_USER_CHANGED && n == 1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_GetResult( %s ) = %s i:%d", text_array[pos], oyjlOPTIONSTATE_eToString_(st), n );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_GetResult( %s ) = %s i:%d", text_array[pos], oyjlOPTIONSTATE_eToString_(st), n );
  }
  pos = 2;
  st = oyjlOptions_GetResult( ui->opts, text_array[pos], &s,   NULL, NULL );
  if(st == oyjlOPTION_USER_CHANGED && s && strcmp(s,"file-name2.json") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_GetResult( %s ) = %s s:%s", text_array[pos], oyjlOPTIONSTATE_eToString_(st), "file-name2.json" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_GetResult( %s ) = %s s:%s", text_array[pos], oyjlOPTIONSTATE_eToString_(st), "file-name2.json" );
  }
  pos = 3;
  st = oyjlOptions_GetResult( ui->opts, text_array[pos], NULL, NULL, &n );
  if(st == oyjlOPTION_USER_CHANGED && n == 1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_GetResult( %s ) = %s i:%d", text_array[pos], oyjlOPTIONSTATE_eToString_(st), n );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_GetResult( %s ) = %s i:%d", text_array[pos], oyjlOPTIONSTATE_eToString_(st), n );
  }
  pos = 4;
  st = oyjlOptions_GetResult( ui->opts, text_array[pos], NULL, &d,   NULL );
  if(st == oyjlOPTION_USER_CHANGED && d == 200)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_GetResult( %s ) = %s d:%g", text_array[pos], oyjlOPTIONSTATE_eToString_(st), d );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_GetResult( %s ) = %s d:%g", text_array[pos], oyjlOPTIONSTATE_eToString_(st), d );
  }
  st = oyjlOptions_GetResult( ui->opts, "none",        NULL, NULL, &n );
  if(st == oyjlOPTION_NONE)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_GetResult( %s ) = %s", "none", oyjlOPTIONSTATE_eToString_(st) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_GetResult( %s ) = %s", "none", oyjlOPTIONSTATE_eToString_(st) );
  }
  st = oyjlOptions_GetResult( ui->opts, "i",           &s,   NULL, NULL );
  if(st == oyjlOPTION_NONE)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_GetResult( %s ) = %s    ", "i", oyjlOPTIONSTATE_eToString_(st) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_GetResult( %s ) = %s    ", "i", oyjlOPTIONSTATE_eToString_(st) );
  }

  oyjlStringListRelease( &text_array, count, free );
  oyjlUi_ReleaseArgs( &ui);

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

  if(!ui)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui not created - missing mandatory option      " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui not created - missing mandatory option      " );
  }
  oyjlUi_ReleaseArgs( &ui);

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
  oyjlUi_ReleaseArgs( &ui);

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
  oyjlUi_ReleaseArgs( &ui);

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
  oyjlUi_ReleaseArgs( &ui);

  argc = 2;
  const char * argv_empty[] = {"test-args","--input="};
  ui = oyjlUi_Create( argc, argv_empty, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups, &state );
  if(!ui)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui not created - empty arg  %d                 ", state >> oyjlUI_STATE_OPTION );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui not created - empty arg                     " );
  }
  oyjlUi_ReleaseArgs( &ui);


  argc = 4;
  ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyjl-config-read", "Oyjl Config Reader", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray, groups_no_args, NULL );

  text = oyjlUi_ToMan( ui, 0 );
  if( text && strlen(text) == 3241 &&
      strstr(text, "\n\\fB\\-\\-candle\\fR\tCandle"))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlUi_ToMan() %lu                            ", strlen(text) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlUi_ToMan() 3241 == %lu                    ", text ? strlen(text) : 0 );
  }
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "oyjlUi_ToMan", "txt" )
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  text = oyjlUi_ToMarkdown( ui, 0 );
  if( text && strlen(text) == 8634 &&
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
  if(text && strlen(text) == 29)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_ResultsToText() %lu                 ", strlen(text) );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_ResultsToText() %lu                 ", text?strlen(text):0 );
  }
  if(verbose && text)
    fprintf( zout, "%s\n", text );
  if(text) {free(text);} text = NULL;

  count = 0;
  text_array = oyjlOptions_ResultsToList( ui->opts, "v", &count );
  if(text_array && count == 3)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_ResultsToList(\"v\") %d               ", count );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_ResultsToList(\"v\") %d               ", count );
  }
  if(text_array)
  {
    if(verbose)
      for(i = 0; i < count; ++i)
        fprintf( stdout, "[%d]:\t%s\n", i, text_array[i] );
    for(i = 0; i < count; ++i)
      free(text_array[i]);
    free(text_array); text_array = NULL;
  }

  count = 0;
  text_array = oyjlOptions_ResultsToList( ui->opts, NULL, &count );
  if(text_array && count == 4)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlOptions_ResultsToList(NULL) %d              ", count );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlOptions_ResultsToList(NULL) %d              ", count );
  }
  if(text_array)
  {
    if(verbose)
      for(i = 0; i < count; ++i)
        fprintf( stdout, "[%d]:\t%s\n", i, text_array[i] );
    for(i = 0; i < count; ++i)
      free(text_array[i]);
    free(text_array); text_array = NULL;
  }


  if(verbose)
  {
    fprintf(stdout, "Help text -> stderr:\n" );
    oyjlOptions_PrintHelp( ui->opts, ui, 1, "%s v%s - %s", argv[0],
                            "1.0", "Test Tool for testing" );
  }

  oyjlUi_ReleaseArgs( &ui);
  char * wrong = "test";
  fprintf(stdout, "oyjlUi_ReleaseArgs(&\"test\") - should give a warning message:\n" );
  oyjlUi_ReleaseArgs( (oyjlUi_s **)&wrong);

  verbose_ = 0;
  const char * helpstr = NULL;
  oyjlOption_s oarray2[] = {
  /* type,   flags, o,   option,    key,  name,         description,         help, value_name,    value_type,               values,                                                          variable_type, output variable */
    {"oiwi", 0,     "i", "input",   NULL, _("input"),   _("Set Input"),      NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)i_choices, sizeof(i_choices), malloc )}, oyjlSTRING, {.s = &file} },
    {"oiwi", 0,     "o", "output",  NULL, _("output"),  _("Control Output"), NULL, "0|1|2",       oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)o_choices, sizeof(o_choices), malloc )}, oyjlINT, {.i = &output} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,     "h", "help",    NULL, _("help"),    _("Help"), NULL, "synopsis|...",          oyjlOPTIONTYPE_CHOICE, {}, oyjlSTRING, {.s = &helpstr} },
    {"oiwi", 0,     "v", "verbose", NULL, _("verbose"), _("verbose"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &verbose_} },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups2[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Mode"), _("Actual mode"),     NULL, "i",       "o,v",    "i,o" },
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "",        "",       "v,h" },
    {"",0,0,0,0,0,0,0}
  };
  argc = 3;
  const char * argv2[] = {"test-args","--help","input"};
  ui = oyjlUi_Create( argc, argv2, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray2, groups2, NULL );
  if(!ui)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui not created - sub command followed by undeclared unbound" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui not created - sub command followed by undeclared unbound" );
  }
  oyjlUi_ReleaseArgs( &ui);
  help = 0;

  oarray2[2].flags = OYJL_OPTION_FLAG_EDITABLE | OYJL_OPTION_FLAG_ACCEPT_NO_ARG;
  oyjl_help_zout = fopen(fn, "w");
  ui = oyjlUi_Create( argc, argv2, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray2, groups2, NULL );
  fclose(oyjl_help_zout); oyjl_help_zout = NULL;
  size = 0;
  text = oyjlReadFile( fn , &size );
  if(!ui && (size == 471 || size == 359))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "help - sub command followed by unbound    %d  ", size );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "help - sub command followed by unbound    %d  ", size );
  }
  oyjlUi_ReleaseArgs( &ui);
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "help-subCommandFollowedByUnbound", "txt" )
  help = 0; size = 0;
  free(text); text = NULL;

  groups2[1].mandatory = "h";
  remove(fn); oyjl_help_zout = fopen(fn, "w");
  ui = oyjlUi_Create( argc, argv2, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray2, groups2, NULL );
  fclose(oyjl_help_zout); oyjl_help_zout = NULL;
  size = 0;
  text = oyjlReadFile( fn , &size );
  if(!ui && (size == 471 || size == 359))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "help - mandatory sub command followed by unbound" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "help - mandatory sub command followed by unbound %d", size );
  }
  oyjlUi_ReleaseArgs( &ui);
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "help-mandatorySubCommandFollowedByUnbound", "txt" )
  help = 0; size = 0;
  free(text); text = NULL;

  oarray2[2].flags = OYJL_OPTION_FLAG_ACCEPT_NO_ARG;
  oarray2[2].value_type = oyjlOPTIONTYPE_NONE;
  oarray2[2].value_name = NULL;
  oarray2[2].variable_type = oyjlINT;
  oarray2[2].variable.i = &help;
  remove(fn); oyjl_help_zout = fopen(fn, "w");
  ui = oyjlUi_Create( argc, argv2, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray2, groups2, NULL );
  fclose(oyjl_help_zout); oyjl_help_zout = NULL;
  size = 0;
  text = oyjlReadFile( fn , &size );
  if(!ui && (size == 471 || size == 359))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "help - mandatory integer sub command followed by unbound" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "help - mandatory integer sub command followed by unbound %d 0x%tx", size, (ptrdiff_t)ui );
  }
  oyjlUi_ReleaseArgs( &ui);
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "help-mandatoryIntegerSubCommandFollowedByUnbound", "txt" )
  help = 0; size = 0;
  free(text); text = NULL;

  argc = 2;
  remove(fn); oyjl_help_zout = fopen(fn, "w");
  ui = oyjlUi_Create( argc, argv2, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray2, groups2, NULL );
  fclose(oyjl_help_zout); oyjl_help_zout = NULL;
  size = 0;
  text = oyjlReadFile( fn , &size );
  if(!ui && (size == 894 || size == 638))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "help - mandatory integer sub command      %d  ", size );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "help - mandatory integer sub command      %d  ", size );
  }
  oyjlUi_ReleaseArgs( &ui);
  OYJL_TEST_WRITE_RESULT( text, strlen(text), "help-mandatoryIntegerSubCommand", "txt" )
  helpstr = 0; size = 0;
  free(text); text = NULL;

  oyjlOption_s oarray3[] = {
  /* type,   flags, o,   option,    key,  name,         description,         help, value_name,    value_type,               values,                                                          variable_type, output variable */
    {"oiwi", OYJL_OPTION_FLAG_NO_DASH,     NULL,"input",   NULL, _("input"),   _("Set Input"),      NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)i_choices, sizeof(i_choices), malloc )}, oyjlSTRING, {.s = &file} },
    {"oiwi", 0,     "o", "output",  NULL, _("output"),  _("Control Output"), NULL, "0|1|2",       oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)o_choices, sizeof(o_choices), malloc )}, oyjlINT, {.i = &output} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_ACCEPT_NO_ARG,     "h", "help",    NULL, _("help"),    _("Help"),           NULL, NULL,          oyjlOPTIONTYPE_CHOICE, {}, oyjlSTRING, {.s = &helpstr} },
    {"oiwi", 0,     "v", "verbose", NULL, _("verbose"), _("verbose"),        NULL, NULL,          oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i = &verbose_} },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups3[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", OYJL_GROUP_FLAG_SUBCOMMAND,     _("Mode"), _("Actual mode"),     NULL, "input,output",       "h,v",    "input,output" },
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "",        "",       "v,h" },
    {"",0,0,0,0,0,0,0}
  };
  argc = 4;
  const char * argv3[] = {"test-args", "-v", "input", "file-arg"};
  setenv("OYJL_NO_EXIT", "1", 0);
  ui = oyjlUi_Create( argc, argv3, /* argc+argv are required for parsing the command line options */
                                       "oiCR", "oyjl-config-read", _("Short example tool using libOyjl"), "logo",
                                       sections, oarray3, groups3, NULL );
  if(!ui)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "ui not created - wrong order sub commands          " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "ui not created - wrong order sub commands          " );
  }
  oyjlUi_ReleaseArgs( &ui);
  help = 0;


  free(oarray[2].values.choices.list);
  free(oarray[3].values.choices.list);
  free(oarray2[0].values.choices.list);
  free(oarray2[1].values.choices.list);
  free(oarray2[2].values.choices.list);
  free(oarray3[0].values.choices.list);
  free(oarray3[1].values.choices.list);
  free(oarray3[2].values.choices.list);
  free(sections);

  return result;
}


/* --- end actual tests --- */


char *    oyjlReadFile( const char * file_name,
                        int        * size_ptr )
{
  FILE * fp = NULL;
  int size = 0, s = 0;
  char * text = NULL;

  if(file_name)
  {
    fp = fopen(file_name,"rb");
    if(fp)
    {
      fseek( fp, 0L, SEEK_END );
      size = ftell( fp );
      if(size == -1)
      {
        switch(errno)
        {
          case EBADF:        WARNc_S("Not a seekable stream %d", errno); break;
          case EINVAL:       WARNc_S("Wrong argument %d", errno); break;
          default:           WARNc_S("%s", strerror(errno)); break;
        }
        if(size_ptr)
          *size_ptr = size;
        fclose( fp );
        return NULL;
      }
      rewind(fp);
      text = malloc(size+1);
      if(text == NULL)
      {
        WARNc_S( "Error: Could allocate memory: %lu", (long unsigned int)size);
        fclose( fp );
        return NULL;
      }
      s = fread(text, sizeof(char), size, fp);
      text[size] = '\000';
      if(s != size)
        WARNc_S( "Error: fread %lu but should read %lu",
                (long unsigned int) s, (long unsigned int)size);
      fclose( fp );
    } else
    {
      WARNc_S( "Error: Could not open file - \"%s\"", file_name);
    }
  }

  if(size_ptr)
    *size_ptr = size;

  return text;
}

