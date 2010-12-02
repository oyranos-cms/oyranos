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
  fprintf( stderr, "      %s -l [-f] [-i]\n",        argv[0]);
  fprintf( stderr, "  %s\n",               _("List search paths:"));
  fprintf( stderr, "      %s -p\n",        argv[0]);
  fprintf( stderr, "  %s\n",               _("Print a help text:"));
  fprintf( stderr, "      %s -h\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "\n");

  if(version) oyDeAllocateFunc_(version);
  if(id) oyDeAllocateFunc_(id);
  if(cfg_date) oyDeAllocateFunc_(cfg_date);
  if(devel_time) oyDeAllocateFunc_(devel_time);
}

int main( int argc , char** argv )
{
  int error = 0;
  int list_profiles = 0, list_paths = 0, list_profile_full_names = 0, 
      list_profile_internal_names = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyExportStart_(EXPORT_CHECK_NO);

  if(argc >= 2)
  {
    int pos = 1;
    char *wrong_arg = 0;
    DBG_PROG1_S("argc: %d\n", argc);
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            switch (argv[pos][1])
            {
              case 'l': list_profiles = 1; break;
              case 'f': list_profile_full_names = 1; break;
              case 'i': list_profile_internal_names = 1; break;
              case 'p': list_paths = 1; break;
              case 'v': oy_debug += 1; break;
              case 'h':
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


  if(list_profiles || list_paths)
  {
    char ** names = NULL;
    uint32_t count = 0, i;

    names = /*(const char**)*/ oyProfileListGet_ ( NULL, &count );

    if(list_profiles)
    {
      fprintf(stderr, "%s:\n", _("ICC profiles"));
      for(i = 0; i < (int)count; ++i)
      {
        oyProfile_s * p = 0;
        const char * t = 0;

        if(list_profile_full_names || list_profile_internal_names)
          p = oyProfile_FromFile( names[i], 0,0 );

        if(!list_profile_full_names && !list_profile_internal_names)
            fprintf(stdout, "%s", names[i]);

        if(list_profile_internal_names)
        {
          t = oyProfile_GetText(p, oyNAME_DESCRIPTION);
          if(t)
            fprintf(stdout, "%s", t);
        }

        if(list_profile_full_names)
        {
          if(list_profile_internal_names)
            fprintf(stdout, " (");
          t = oyProfile_GetFileName(p, -1);
          if(t)
            fprintf(stdout, "%s", t);
          if(list_profile_internal_names)
            fprintf(stdout, ")");
        }
            fprintf(stdout, "\n");

        oyProfile_Release( &p );
      }
    }
    oyStringListRelease_(&names, count, oyDeAllocateFunc_);

    if(list_paths)
    {
      int n = 0;
      char ** path_names = oyDataPathsGet_( &n, "color/icc",
                                              oyALL, oyUSER_SYS,
                                              oyAllocateFunc_ );
      fprintf(stderr, "%s:\n", _("ICC profile search paths"));
      for(i = 0; i < n; ++i)
        fprintf(stdout, "%s\n", path_names[i]);

      oyStringListRelease_(&path_names, n, oyDeAllocateFunc_);
    }

  }

  return error;
}
