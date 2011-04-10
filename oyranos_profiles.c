/** @file oyranos_profiles.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2010 (C) Kai-Uwe Behrmann
 *
 *  @brief    ICC profile informations - on the command line
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2010/11/11
 *
 *  The program informs about installed ICC profiles.
 */


#include "oyranos.h"
#include "oyranos_alpha.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_config.h"
#include "oyranos_string.h"
#include "oyranos_version.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void* oyAllocFunc(size_t size) {return malloc (size);}

void  printfHelp (int argc, char** argv)
{
  char * version = oyVersionString(1,0);
  char * id = oyVersionString(2,0);
  char * cfg_date =  oyVersionString(3,0);
  char * devel_time = oyVersionString(4,0);

  fprintf( stderr, "\n");
  fprintf( stderr, "oyranos-profiles %s\n",
                                _("is a ICC profile information tool"));
  fprintf( stderr, "  Oyranos v%s config: %s devel period: %s\n",
                  oyNoEmptyName_m_(version),
                  oyNoEmptyName_m_(cfg_date), oyNoEmptyName_m_(devel_time) );
  if(id)
  fprintf( stderr, "  Oyranos git id %s\n", oyNoEmptyName_m_(id) );
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",
                                           _("Hint: search paths are influenced by the XDG_CONFIG_HOME shell variable."));
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "  %s\n",               _("List available ICC profiles:"));
  fprintf( stderr, "      %s -l [-f] [-e] [-acdknot] \n",        argv[0]);
  fprintf( stderr, "      -f  %s\n",       _("full path and file name"));
  fprintf( stderr, "      -e  %s\n",       _("internal name"));
  fprintf( stderr, "      -a  %s\n",       _("abstract class"));
  fprintf( stderr, "      -c  %s\n",       _("colour space class"));
  fprintf( stderr, "      -d  %s\n",       _("display class"));
  fprintf( stderr, "      -k  %s\n",       _("(device) link class"));
  fprintf( stderr, "      -n  %s\n",       _("named colour class"));
  fprintf( stderr, "      -o  %s\n",       _("output class"));
  fprintf( stderr, "      -i  %s\n",       _("input class"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("List search paths:"));
  fprintf( stderr, "      %s -p [-usym]\n",        argv[0]);
  fprintf( stderr, "      -u  %s\n",       _("user path"));
  fprintf( stderr, "      -s  %s\n",       _("linux system path"));
  fprintf( stderr, "      -y  %s\n",       _("oyranos install path"));
  fprintf( stderr, "      -m  %s\n",       _("machine specific path"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Install ICC profile:"));
  fprintf( stderr, "      %s --install [-u|-s|-y|-m] ICC_file_name(s)\n", argv[0]);
  fprintf( stderr, "      -u  %s\n",       _("user path"));
  fprintf( stderr, "      -s  %s\n",       _("system path"));
  fprintf( stderr, "      -y  %s\n",       _("oyranos install path"));
  fprintf( stderr, "      -m  %s\n",       _("machine specific path"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Print a help text:"));
  fprintf( stderr, "      %s -h\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("General options:"));
  fprintf( stderr, "      %s\n",           _("-v verbose"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s:\n",               _("Example"));
  fprintf( stderr, "      SAVEIFS=$IFS ; IFS=$'\\n\\b'; for f in `oyranos-profiles -ldf`; do oymd5icc \"$f\"; done; IFS=$SAVEIFS");
  fprintf( stderr, "\n");
  fprintf( stderr, "\n");

  if(version) oyDeAllocateFunc_(version);
  if(id) oyDeAllocateFunc_(id);
  if(cfg_date) oyDeAllocateFunc_(cfg_date);
  if(devel_time) oyDeAllocateFunc_(devel_time);
}

/* allow "-opt val" and "-opt=val" syntax */
#define OY_PARSE_STRING_ARG( opt ) \
                        if( pos + 1 < argc && argv[pos][i+1] == 0 ) \
                        { opt = argv[pos+1]; \
                          if( opt == 0 && strcmp(argv[pos+1],"0") ) \
                            wrong_arg = "-" #opt; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+1] == '=') \
                        { opt = &argv[pos][i+2]; \
                          if( opt == 0 && strcmp(&argv[pos][i+2],"0") ) \
                            wrong_arg = "-" #opt; \
                          i = 1000; \
                        } else wrong_arg = "-" #opt; \
                        if(oy_debug) fprintf(stderr, #opt "=%s\n",opt)
#define OY_PARSE_STRING_ARG2( opt, arg ) \
                        if( pos + 1 < argc && argv[pos][i+strlen(arg)+1] == 0 ) \
                        { opt = argv[pos+1]; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+strlen(arg)+1] == '=') \
                        { opt = &argv[pos][i+strlen(arg)+2]; \
                          i = 1000; \
                        } else wrong_arg = "-" arg; \
                        if(oy_debug) printf(arg "=%s\n",opt)
#define OY_IS_ARG( arg ) \
                        (strlen(argv[pos])-2 >= strlen(arg) && \
                         memcmp(&argv[pos][2],arg, strlen(arg)) == 0)

int main( int argc , char** argv )
{
  int error = 0;
  int list_profiles = 0, 
      list_profile_full_names = 0, list_profile_internal_names = 0,
      list_paths = 0, user_path = 0, oyranos_path = 0, system_path = 0, machine_path = 0;
  int colour_space = 0,
      display = 0,
      input = 0,
      output = 0,
      abstract = 0,
      named_colour = 0,
      device_link = 0;
  char ** install = 0;
  int install_n = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyExportStart_(EXPORT_CHECK_NO);

  if(argc >= 2)
  {
    int pos = 1, i;
    char *wrong_arg = 0;
    DBG_PROG1_S("argc: %d\n", argc);
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            for(i = 1; pos < argc && i < strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case 'l': list_profiles = 1; break;
              case 'f': list_profile_full_names = 1; break;
              case 'e': list_profile_internal_names = 1; break;
              case 'c': colour_space = 1; break;
              case 'd': display = 1; break;
              case 'i': input = 1; break;
              case 'o': output = 1; break;
              case 'a': abstract = 1; break;
              case 'k': device_link = 1; break;
              case 'n': named_colour = 1; break;
              case 'p': list_paths = 1; break;
              case 'u': user_path = 1; break;
              case 'y': oyranos_path = 1; break;
              case 's': system_path = 1; break;
              case 'm': machine_path = 1; break;
              case 'v': oy_debug += 1; break;
              case 'h':
              case '-':
                        if(i == 1)
                        {
                             if(OY_IS_ARG("install"))
                        {
                          if(pos+1 < argc)
                          {
                            if(strcmp(argv[pos+1],"-u") == 0)
                            {  user_path = 1; ++pos; }
                            if(strcmp(argv[pos+1],"-s") == 0)
                            {  system_path = 1; ++pos; }
                            if(strcmp(argv[pos+1],"-y") == 0)
                            {  oyranos_path = 1; ++pos; }
                            if(strcmp(argv[pos+1],"-m") == 0)
                            {  machine_path = 1; ++pos; }
                          }
                          if(user_path + oyranos_path + system_path +
                             machine_path != 1)
                          {
                            fprintf(stderr, "%s: [-u,-s,-y,-m]\n", _("A single path option is required"));
                            exit (0);
                          }
                          ++pos;
                          /* use all following arguments as profiles */
                          while(pos < argc)
                          {
                            oyStringListAddStaticString_(&install, &install_n,
                                argv[pos], oyAllocateFunc_, oyDeAllocateFunc_ );
                            ++pos;
                          }
                          if(!install_n)
                          {
                            fprintf(stderr, "%s: --install [-u|-s|-y|-m] ICC_file_name(s)\n", _("File name is missed"));
                            exit (0);
                          }
                          i=100; break; }
                        }
              default:
                        printfHelp(argc, argv);
                        exit (0);
                        break;
            }
            break;
        default:
                        printfHelp(argc, argv);
                        exit (0);
                        break;
      }
      if( wrong_arg )
      {
       fprintf(stderr, "%s %s\n", _("wrong argument to option:"), wrong_arg);
       printfHelp(argc, argv);
       exit(1);
      }
      ++pos;
    }
  } else
  {
                        printfHelp(argc, argv);
                        exit (0);
  }

  /* check the default paths */
  /*oyPathAdd( OY_PROFILE_PATH_USER_DEFAULT );*/


  if(list_profiles || list_paths || install_n)
  {
    oyProfile_s * p = 0;
    char ** names = NULL;
    uint32_t count = 0, i;
    int accept;
    const char * t = 0;
    const char * path = 0;

    names = /*(const char**)*/ oyProfileListGet_ ( NULL, &count );

    if(user_path || oyranos_path || system_path || machine_path)
    {
        if(user_path)
          path = OY_USERCOLORDATA OY_SLASH OY_ICCDIRNAME;
        if(system_path)
          path = "/usr/share/color/" OY_ICCDIRNAME;
        if(oyranos_path)
          path = OY_SYSCOLORDIR OY_SLASH OY_ICCDIRNAME;
        if(machine_path)
          path = "/var/lib/color/" OY_ICCDIRNAME;
    }

    if(list_profiles)
    {
      fprintf(stderr, "%s:\n", _("ICC profiles"));
      for(i = 0; i < (int)count; ++i)
      {
        icSignature sig_class = 0;

        accept = 1;
        p = 0;
        t = 0;

        if(list_profile_full_names || list_profile_internal_names)
        {
          p = oyProfile_FromFile( names[i], 0,0 );
        }

        if( colour_space || input || display || output || abstract ||
            named_colour || device_link)
        {
          accept = 0;
          if(!p)
            p = oyProfile_FromFile( names[i], 0,0 );
          sig_class = oyProfile_GetSignature( p, oySIGNATURE_CLASS );
        }

        if(!accept)
        {
          if(colour_space && sig_class == icSigColorSpaceClass)
            accept = 1;
          else if(input && sig_class == icSigInputClass)
            accept = 1;
          else if(display && sig_class == icSigDisplayClass)
            accept = 1;
          else if(output && sig_class == icSigOutputClass)
            accept = 1;
          else if(abstract && sig_class == icSigAbstractClass)
            accept = 1;
          else if(device_link && sig_class == icSigLinkClass)
            accept = 1;
          else if(named_colour && sig_class == icSigNamedColorClass)
            accept = 1;
        }

        if(!list_profile_full_names && !list_profile_internal_names &&
           accept)
            fprintf(stdout, "%s", names[i]);

        if(list_profile_internal_names && accept)
        {
          t = oyProfile_GetText(p, oyNAME_DESCRIPTION);
          if(t)
            fprintf(stdout, "%s", t);
        }

        if(list_profile_full_names && accept)
        {
          if(list_profile_internal_names)
            fprintf(stdout, " (");
          t = oyProfile_GetFileName(p, -1);
          if(t)
            fprintf(stdout, "%s", t);
          if(list_profile_internal_names)
            fprintf(stdout, ")");
        }

        if(accept)
            fprintf(stdout, "\n");

        oyProfile_Release( &p );
      }
    }

    if(list_paths)
    {
      int n = 0;
      char ** path_names = oyDataPathsGet_( &n, "color/icc",
                                              oyALL, oyUSER_SYS,
                                              oyAllocateFunc_ );
      fprintf(stderr, "%s:\n", _("ICC profile search paths"));
      if(path)
      {
        fprintf(stdout, "%s\n", path );
      } else
        for(i = 0; i < n; ++i)
          fprintf(stdout, "%s\n", path_names[i]);

      oyStringListRelease_(&path_names, n, oyDeAllocateFunc_);
    }

    if(install_n)
      for(i = 0; i < install_n; ++i)
      {
        const char * file_name = install[i];
        int j;
        accept = 1;

        if(strrchr(file_name, OY_SLASH_C))
          file_name = strrchr(file_name, OY_SLASH_C) + 1;

        for(j = 0; j < (int)count; ++j)
        {
          if(strcmp(file_name, names[j]) == 0)
          {
            p = oyProfile_FromFile(names[j], 0,0);
            t = oyProfile_GetFileName(p, -1);
            fprintf(stderr, "%s: \"%s\"\n", _("Profile already installed"), t );
            oyProfile_Release( &p );
            accept = 0;
          }
        }
        if(accept)
        {
          size_t size = 0;
          char * data, * fn = 0;
          const char * sfn = install[i];
          int error = 0;

          data = oyReadFileToMem_(install[i], &size, oyAllocateFunc_);
          STRING_ADD( fn, path );
          STRING_ADD( fn, OY_SLASH );
          if(strrchr(sfn, OY_SLASH_C))
            sfn = strrchr(sfn, OY_SLASH_C) + 1;
          STRING_ADD( fn, sfn );
          if(size)
            error = oyWriteMemToFile_ ( fn, data, size );

          fprintf(stderr, "%s[%d%s]: \"%s\" -> \"%s\"\n",
                   _("Installed"), (int)size, _("bytes"), install[i], fn );
          oyDeAllocateFunc_(data); size = 0; data = 0;
        }
      }
    oyStringListRelease_(&names, count, oyDeAllocateFunc_);
  }


  return error;
}
