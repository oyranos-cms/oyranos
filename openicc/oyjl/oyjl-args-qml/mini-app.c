/** @file mini-app.c
 *
 *  Oyjl JSON QML is a graphical renderer of UI files.
 *
 *  Copyright 2018-2023 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl Args + Qml example
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2019/11/28
 */

#include <unistd.h> /* sleep() */

#include "oyjl.h"
#include "oyjl_version.h"

#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>
#endif
#ifdef __USE_POSIX
#define HAVE_POSIX 1
#endif

#ifdef OYJL_USE_GETTEXT
# ifdef OYJL_HAVE_LIBINTL_H
#  include <libintl.h> /* bindtextdomain() */
# endif
# define _(text) dgettext( OYJL_DOMAIN, text )
#else
# define _(text) text
#endif

#include <dirent.h>
// flags 0x01 - skip recursive
void listFiles(const char * path, char *** list, int * count, const char * suffix, int flags)

{
  DIR * d = opendir(path);
  if(d == NULL) return;
  struct dirent * dir;
  while ((dir = readdir(d)) != NULL)
    {
      if(dir->d_type != DT_DIR)
      {
        char * name = NULL;
        if(strcmp(path,".") == 0)
            oyjlStringAdd( &name, 0,0, "%s", dir->d_name);
        else
            oyjlStringAdd( &name, 0,0, "%s/%s", path, dir->d_name);
        if(!suffix)
            oyjlStringListPush( list, count, name, malloc, free );
        else
        {
          char * end = &name[strlen(name) - strlen(suffix)];
          if(strcasecmp(end, suffix) == 0)
            oyjlStringListPush( list, count, name, malloc, free );
        }
        free(name);
      }
      else
      if( dir->d_type == DT_DIR && strcmp(dir->d_name,".") != 0 &&
          strcmp(dir->d_name,"..") != 0 )
      {
        char * d_path = NULL; oyjlStringAdd( &d_path, 0,0, "%s/%s", path, dir->d_name);
        if(!(flags & 0x01))
          listFiles(d_path, list, count, suffix, flags);
        free(d_path);
      }
    }

    closedir(d);
}

int oyjlLowerStrcmpInverseWrap_ (const void * a_, const void * b_)
{
  const char * a = *(const char **)b_,
             * b = *(const char **)a_;
#ifdef HAVE_POSIX
  return strcasecmp(a,b);
#else
  return strcmp(a,b);
#endif
}

oyjlOptionChoice_s * getFileChoices             ( oyjlOption_s      * o OYJL_UNUSED,
                                                  int               * selected,
                                                  oyjlOptions_s     * opts OYJL_UNUSED )
{
    int choices = 0, current = -1;
    char ** choices_string_list = NULL;
    int error = 0;

    if(!error)
    {
      int i;
      const char * man_page = getenv("DISPLAY");
      int skip_real_info = man_page && strcmp(man_page,"man_page") == 0;
      if(!skip_real_info)
      {
        const char * path =
#ifdef __ANDROID__
                "/storage/emulated/0/Downloads";
#else
                "/tmp";
#endif
        int flags = 0x01; // skip recursing into sub directories
        listFiles(path, &choices_string_list, &choices, NULL, flags);
        qsort( choices_string_list, choices, sizeof(char*), oyjlLowerStrcmpInverseWrap_ );
      }
      oyjlOptionChoice_s * c = calloc(choices+1, sizeof(oyjlOptionChoice_s));
      if(c)
      {
        for(i = 0; i < choices; ++i)
        {
          char * v = malloc(12);

          sprintf(v, "%d", i);
          c[i].nick = strdup(choices_string_list[i]);
          c[i].name = strdup(strrchr(c[i].nick, '/') + 1);
          c[i].description = strdup("");
          c[i].help = strdup("");
        }
        c[i].nick = malloc(4);
        c[i].nick[0] = '\000';
      }
      oyjlStringListRelease( &choices_string_list, choices, free );
      if(selected)
        *selected = current;

      return c;
    } else
      return NULL;
}

/* This function is called the
 * * first time for GUI generation and then
 * * for executing the tool.
 */
int myMain( int argc, const char ** argv )
{
  int output = 0;
  const char * file = NULL;
  int file_count = 0;
  int show_status = 0;
  const char * render = NULL;
  int help = 0;
  int verbose = 0;
  int error = 0;
  int state = 0;

  /* handle options */
  /* Select from *version*, *manufacturer*, *copyright*, *license*, *url*,
   * *support*, *download*, *sources*, *oyjl_modules_author* and
   * *documentation* what you see fit. Add new ones as needed. */
  oyjlUiHeaderSection_s sections[] = {
    /* type, nick,            label, name,                  description  */
    {"oihs", "version",       NULL,  "1.0",                 NULL},
    {"oihs", "documentation", NULL,  NULL,                  _("The example tool demontrates the usage of the libOyjl API's and loads libOyjlArgsQml on demand.")},
    {"oihs", "date",          NULL,  "2019-11-28T12:00:00", _("November 28, 2019")},
    /* use newline separated permissions in name + write newline separated list in description; both lists need to match in count */
    {"oihs", "permissions",   NULL,  "android.permission.READ_EXTERNAL_STORAGE\nandroid.permission.WRITE_EXTERNAL_STORAGE", _("Read external storage for global data access, like downloads, music ...\nWrite external storage to create and modify global data.")},
    {"",0,0,0,0}};

  /* declare some option choices */
  oyjlOptionChoice_s i_choices[] = {{"oyjl.json", "oyjl.json", "oyjl.json", ""},
                                    {"oyjl2.json", "oyjl2.json", "oyjl2.json", ""},
                                    {"","","",""}};
  oyjlOptionChoice_s o_choices[] = {{"0", _("Print All"), _("Print All"), ""},
                                    {"1", _("Print Camera"), _("Print Camera JSON"), ""},
                                    {"2", _("Print None"), _("Print None"), ""},
                                    {"","","",""}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags, o,   option,    key,  name,         description,         help, value_name,    value_type,               values,                                                          variable_type, output variable */
    {"oiwi", 0,     "#", "",        NULL, _("status"),  _("Show Status"),    NULL, NULL,          oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i = &show_status}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,     "@", "",       NULL, _("input"),   _("Set Input"),      NULL, _("FILENAME"), oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)i_choices, sizeof(i_choices), malloc )}, oyjlINT, {.i = &file_count}, NULL},
    {"oiwi", 0/*OYJL_OPTION_FLAG_EDITABLE*/,     "i", "input",  NULL, _("input"),   _("Set Input"), _("File"), _("FILENAME"), oyjlOPTIONTYPE_FUNCTION, {.getChoices = getFileChoices}, oyjlSTRING, {.s=&file}, NULL},
    {"oiwi", 0,     "o", "output",  NULL, _("output"),  _("Control Output"), NULL, "0|1|2",       oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)o_choices, sizeof(o_choices), malloc )}, oyjlINT, {.i = &output}, NULL},
    /* The --render option can be hidden and used only internally. */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE | OYJL_OPTION_FLAG_MAINTENANCE,     "R", "render", NULL, _("render"),  _("Render"),         NULL, NULL,          oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s = &render}, NULL},
    {"oiwi", 0,     "h", "help",    NULL, _("help"),    _("Help"),           _("Print help for command line style interface"), NULL,          oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i = &help}, NULL},
    {"oiwi", 0,     "v", "verbose", NULL, _("verbose"), NULL,                NULL, NULL,          oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i = &verbose}, NULL},
    {"oiwi", 0,     "e", "error",   NULL, _("error"),   NULL,                NULL, NULL,          oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i = &error}, NULL},
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {0},0,{0},0}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups_no_args[] = {
  /* type,   flags, name,      description,          help, mandatory, optional, detail */
    {"oiwg", 0,     _("Mode1"),_("Simple mode"),     NULL, "#",       "o,v",    "o", NULL}, /* accepted even if none of the mandatory options is set */
    {"oiwg", 0,     _("Mode2"),_("Any arg mode"),    NULL, "@",       "o,v",    "@,o", NULL},/* accepted if anonymous arguments are set */
    {"oiwg", 0,     _("Mode3"),_("Actual mode"),     NULL, "i",       "R,o,v",  "i,o", NULL},/* parsed and checked with -i option */
    {"oiwg", 0,     _("Misc"), _("General options"), NULL, "h,e",     "v",      "h,v,e", NULL},/* -v appears in documentation */
    {"",0,0,0,0,0,0,0,0}
  };

  /* tell about the tool */
  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "miniApp", "mini App", _("mini OyjlArgsQml example"), ":/images/logo",
                                       sections, oarray, groups_no_args, &state );
  /* done with options handling */

  static int main_count = 0;
  if(verbose)
  {
    ++main_count;
    fprintf(stderr, "%s %s %d\n", _("started"), __func__, main_count );
  }

  /* Render boilerplate */
  if(ui && render)
  {
    int debug = verbose;
    oyjlArgsRender( argc, argv, NULL, NULL,NULL, debug, ui, myMain );
  }
  else if(ui)
  {
    /* get the results and do something with it */
    const char * result = oyjlOptions_ResultsToJson(ui->opts, OYJL_JSON);
    if(verbose)
        fprintf(stdout, "%s\n", result);
    if(output)
        fprintf( stdout, "output:\t%d\n", output );
    if(file)
        fprintf( stdout, "file:\t%s\n", file );
    if(file_count)
    {
        fprintf( stdout, "file_count:\t%d\n", file_count );
        int n = 0,i;
        char ** input = oyjlOptions_ResultsToList( ui->opts, "@", &n );
        for(i = 0; i < n; ++i)
            fprintf( stdout, "[%d]:\t%s\n", i, input[i] );
    }
    if(error)
    {
        fprintf( stderr, "send to stderr\n" );
        sleep(5);
    }
  }

  oyjlUi_Release( &ui);

  if(verbose)
  {
    fprintf(stderr, "%s %s %d\n", _("finished"), __func__, main_count );
    --main_count;
  }

  return 0;
}

extern int *oyjl_debug;
int main( int argc_ OYJL_UNUSED, char**argv_ OYJL_UNUSED)
{
  int argc = argc_;
  const char * argv[] = {"test",argc_>=2?argv_[1]:NULL,argc_>=3?argv_[2]:NULL,argc_>=4?argv_[3]:NULL,argc_>=5?argv_[4]:NULL, NULL};
  if(argc > 4) argc = 4;
  oyjlTranslation_s * trc = NULL;
  char * loc = NULL;

#ifdef __ANDROID__
  setenv("COLORTERM", "1", 0); /* show rich text format on non GNU color extension environment */
  argv[argc++] = "--render=gui"; /* start QML */
#endif

  /* language needs to be initialised before setup of data structures */
  int use_gettext = 0;
#ifdef OYJL_USE_GETTEXT
  use_gettext = 1;
#endif
#ifdef OYJL_HAVE_LOCALE_H
  loc = setlocale(LC_ALL,"");
#endif
  if(loc)
    trc = oyjlTranslation_New( loc, 0,0,0,0,0,0 );
  oyjlInitLanguageDebug( "Oyjl", "OYJL_DEBUG", oyjl_debug, use_gettext, "OYJL_LOCALEDIR", OYJL_LOCALEDIR, &trc, NULL );
  oyjlTranslation_Release( &trc );

  fprintf(stderr, "%s %s\n", _("started"), __func__ );

  myMain(argc, argv);

  fprintf(stderr, "%s %s\n", _("finished"), __func__ );

  return 0;
}


