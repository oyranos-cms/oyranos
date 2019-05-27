/*  @file oyjl_translate.c
 *
 *  libOyjl - JSON helper tool
 *
 *  @par Copyright:
 *            2018 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl JSON translation helper
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2018/01/06
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "oyjl_version.h"
#include "oyjl.h"
#include "oyjl_macros.h"
#include "oyjl_i18n.h"
#include "oyjl_tree_internal.h"

#ifdef OYJL_USE_GETTEXT
int use_gettext = 1;
#else
int use_gettext = 0;
#endif

void printfHelp(int argc, char ** argv, int verbose_)
{
  int pos = 0;
  while(pos < argc && verbose_)
    fprintf( stderr, "%s ", argv[pos++]);
  fprintf( stderr, "\n");
  fprintf( stderr, "%s %s\n",   argv[0],
                                _("is a JSON helper tool"));
  fprintf( stderr, "  v%s\n",
                  OYJL_VERSION_NAME );
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "  %s\n",               _("Convert JSON to gettext ready C strings:"));
  fprintf( stderr, "      %s -e [-v] -i FILE_NAME -o FILE_NAME -f '_(\\\"%%s\\\"); ' -k name,description,help\n",        argv[0]);
  fprintf( stderr, "        -f              %s\n", _("output format string"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Add gettext translated keys to JSON:"));
  fprintf( stderr, "      %s -a [-v] -i FILE_NAME -o FILE_NAME -k name,description,help -d TEXTDOMAIN -p LOCALEDIR -l de,es [-w C]\n",        argv[0]);
  fprintf( stderr, "        -d TEXTDOMAIN   %s\n", _("text domain of your project"));
  fprintf( stderr, "        -l locales      %s\n", _("locales in a comma separated list"));
  fprintf( stderr, "        -p LOCALEDIR    %s\n", _("locale directory containing the your-locale/LC_MESSAGES/your-textdomain.mo gettext translations"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print a help text:"));
  fprintf( stderr, "      %s -h\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("General options:"));
  fprintf( stderr, "        -i FILE_NAME    %s\n", _("specify JSON file"));
  fprintf( stderr, "        -o              %s\n", _("output"));
  fprintf( stderr, "        -k STRING_LIST  %s\n", _("to be used key names in a comma separated list"));
  fprintf( stderr, "        -w TYPE         %s\n", _("language specific wrap; -w C for C static char"));
  fprintf( stderr, "        -v              %s\n", _("verbose"));
  fprintf( stderr, "\n");
  fprintf( stderr, "\n");
}

#define WARNc_S(...) oyjlMessage_p( oyjlMSG_ERROR, 0, __VA_ARGS__ )

int main(int argc, char ** argv)
{
  int verbose = 0;
  int error = 0;
  int add = 0;
  int extract = 0;
  int size = 0;
  const char * output = NULL,
             * file_name = NULL,
             * format = NULL,
             * wrap = NULL,
             * key_list = NULL,
             * lang_list = NULL,
             * localedir = NULL,
             * ctextdomain = OYJL_DOMAIN;
  char * json = NULL;
  oyjl_val root = NULL,v;
  char * text = NULL;

#ifdef OYJL_USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyjlInitLanguageDebug( "Oyjl", OYJL_DEBUG, oyjl_debug, use_gettext, OYJL_LOCALE_VAR, OYJL_LOCALEDIR, OYJL_DOMAIN, NULL );

  if(argc >= 2)
  {
    int pos = 1;
    unsigned i;
    char *wrong_arg = 0;
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            for(i = 1; pos < argc && i < strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case 'a': add = 1; break;
              case 'd': OYJL_PARSE_STRING_ARG(ctextdomain); break;
              case 'e': extract = 1; break;
              case 'f': OYJL_PARSE_STRING_ARG(format); break;
              case 'i': OYJL_PARSE_STRING_ARG(file_name); break;
              case 'k': OYJL_PARSE_STRING_ARG(key_list); break;
              case 'l': OYJL_PARSE_STRING_ARG(lang_list); break;
              case 'o': OYJL_PARSE_STRING_ARG(output); break;
              case 'p': OYJL_PARSE_STRING_ARG(localedir); break;
              case 'w': OYJL_PARSE_STRING_ARG(wrap); break;
              case 'v': ++verbose; ++*oyjl_debug; break;
              case 'h':
              case '-':
                        if(i == 1)
                        {
                             if(OYJL_IS_ARG("verbose"))
                        { ++*oyjl_debug; ++verbose; i=100; break; }
                        } OYJL_FALLTHROUGH
              default:
                        WARNc_S( "%s %s", "wrong option:", argv[pos]);
                        printfHelp(argc, argv, verbose);
                        exit (0);
                        break;
            }
            break;
        default:
                        WARNc_S( "%s %s", "wrong option:", argv[pos]);
                        printfHelp(argc, argv, verbose);
                        exit (0);
                        break;
      }
      if( wrong_arg )
      {
       WARNc_S( "%s %s", "wrong argument to option:", wrong_arg);
       printfHelp(argc, argv, verbose);
       exit(1);
      }
      ++pos;
    }
  } else
  {
                        printfHelp(argc, argv, verbose);
                        exit (0);
  }

  if(verbose)
    fprintf(stderr, "i18n test:\t\"%s\" %s\n", _("Usage"), textdomain(NULL) );

  if(file_name)
    json = oyjlReadFile( file_name, &size );

  if(json)
  {
    char * text = malloc(256);

    text[0] = 0;

    /* parse json ... */
    root = oyjlTreeParse( json, text, 256 );
    if(text[0])
    {
      fprintf( stderr, "ERROR: %s\n%s\n", file_name, text );
    }
  }

  if(root)
  {
    char ** paths = NULL;
    int count = 0, i;

    oyjlTreeToPaths( root, 1000000, NULL, 0, &paths );
    if(verbose)
      fprintf(stderr, "processed:\t\"%s\"\n", file_name);
    while(paths && paths[count]) ++count;

    if(extract)
    {
      int n = 0;
      char ** list = oyjlStringSplit( key_list, ',', &n, malloc );
      if(verbose)
        fprintf(stderr, "use:\t%d keys\n", n);

      for(i = 0; i < count; ++i)
      {
        char * path = paths[i];
        int j;
        for(j = 0; j < n; ++j)
        {
          char * key = list[j];
          if(oyjlPathMatch(path, key, OYJL_PATH_MATCH_LAST_ITEMS ))
          {
            const char * t = NULL;
            v = oyjlTreeGetValue( root, 0, path );
            if(v)
              t = OYJL_GET_STRING(v);
            if(verbose)
              fprintf(stderr, "found:\t%d key: %s value: \"%s\"\n", i, path, t?t:"----" );
            if(t)
            {
              if(format)
                oyjlStringAdd( &text, malloc, free, format, t );
              else
                oyjlStringAdd( &text, malloc, free, "// %s:%s\n{ const char * t = _(\"%s\"); }\n\n",
                                 file_name, path, t );
            }
          }
        }
      }

    } else if(add)
    {
      int ln = 0, n = 0;
      char ** langs = oyjlStringSplit( lang_list, ',', &ln, malloc );
      char * var = NULL;
      const char * oyjl_domain_path = OYJL_LOCALEDIR;
      char ** list = oyjlStringSplit( key_list, ',', &n, malloc );
      char * dir;

      if(verbose)
        fprintf(stderr, "use:\t%d langs - %s : %s\n", ln, ctextdomain, dgettext( ctextdomain, "Rendering Intent" ));
      if(verbose)
        fprintf(stderr, "use:\t%d keys\n", n);

      if(localedir)
      {
        if(!oyjlIsDirFull_ (localedir))
        {
          fprintf(stderr, "ERROR: Can not find absolute path:\t%s\n", localedir);
          exit(1);
        }
        oyjl_domain_path = localedir;
      }
      if(!oyjl_domain_path && getenv(OYJL_LOCALE_VAR) && strlen(getenv(OYJL_LOCALE_VAR)))
        oyjl_domain_path = strdup(getenv(OYJL_LOCALE_VAR));

      var = textdomain( ctextdomain );
      dir = bindtextdomain( ctextdomain, oyjl_domain_path );

      if(*oyjl_debug)
        fprintf(stdout, "%s = bindtextdomain() to \"%s\"\ntextdomain: %s == %s\n", dir, oyjl_domain_path, ctextdomain, var );
      var = NULL;

      oyjlStringAdd( &var, 0,0, "NLSPATH=%s", oyjl_domain_path );
      putenv(var); /* Solaris */

      for(i = 0; i < ln; ++i)
      {
        char * lang = langs[i];
        int j;

        const char * checklocale = setlocale( LC_MESSAGES, lang );
        if(*oyjl_debug)
          fprintf(stdout, "setlocale(%s) == %s\n", lang, checklocale );

        for(j = 0; j < count; ++j)
        {
          char * path = paths[j];

          int k;
          for(k = 0; k < n; ++k)
          {
            char * key = list[k];

            if(oyjlPathMatch(path, key, OYJL_PATH_MATCH_LAST_ITEMS ))
            {
              const char * t = NULL,
                         * tr = NULL;
              v = oyjlTreeGetValue( root, 0, path );
              if(v)
                t = OYJL_GET_STRING(v);
              if(t)
                tr = dgettext( ctextdomain, t );
              if(verbose)
                fprintf(stderr, "found:\t key: %s value[%s]: \"%s\"\n", path, ctextdomain, tr?tr:"----" );
              if(t != tr)
              {
                char * new_path = NULL;
                oyjlStringAdd( &new_path, malloc, free, "%s.%s", path, lang );
                v = oyjlTreeGetValue( root, OYJL_CREATE_NEW, new_path );
                oyjlValueSetString( v, tr );
                oyjlStringAdd( &text, malloc, free, "%s\n", tr );
                free(new_path);
              }
            }
          }
        }
      }

      if(verbose)
        fprintf(stderr, "found i18n:\n%s", text );
      if(text) free(text);
      text = NULL;
      i = 0;
      oyjlTreeToJson( root, &i, &text );

    } else
      for(i = 0; i < count; ++i)
        fprintf( stdout, "%s\n", paths[i] );

    oyjlStringListRelease( &paths, count, free );

    if(text)
    {
      if(wrap)
      {
        char * tmp = NULL;
        char * sname = strdup(ctextdomain);
        if(strcmp(wrap,"C") != 0)
        {
          fprintf(stderr,"ERROR: Only -w C is supported.\n");
          exit(1);
        }

        oyjlStringReplace( &text, "\"", "\\\"", NULL,NULL );
        oyjlStringReplace( &text, "\n", "\\n\\\n", NULL,NULL );
        oyjlStringReplace( &sname, "-", "_", NULL,NULL );
        oyjlStringAdd( &tmp, malloc, free, "#define %s_json \"%s\"\n", sname, text );
        free(text); text = tmp; tmp = NULL;
      }

      if(!output || strcmp(output,"-") == 0)
        fputs( text, stdout );
      else
        oyjlWriteFile( output, text, strlen(text) );
    }
  }

  return error;
}

