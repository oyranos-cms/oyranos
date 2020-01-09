/** @file oyranos-config.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2017-2019 (C) Kai-Uwe Behrmann
 *
 *  @brief    DB manipulation tool
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2017/11/13
 *
 */
/* !cc -Wall -g test_device.c -o test_device2 -L. `oyranos-config --cflags --ldstaticflags` -lm -lltdl */

/* cc -Wall -g test_device.c -o test_device2 -L. `oyranos-config --cflags --ldstaticflags` `pkg-config --cflags --libs libxml-2.0` -lm -I ../../ -I ../../API_generated/ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oyranos.h"
#include "oyranos_db.h"
#include "oyranos_devices.h"
#include "oyranos_helper.h"
#include "oyranos_helper_macros_cli.h"
#include "oyranos_i18n.h"
#include "oyranos_icc.h"
#include "oyranos_module_internal.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"
#include "oyranos_config_internal.h"
#include "oyProfiles_s.h"

#include <locale.h>

void* oyAllocFunc(size_t size) {return malloc (size);}
void  oyDeAllocFunc ( oyPointer ptr) { if(ptr) free (ptr); }
void  getKey                         ( const char        * key,
                                       oySCOPE_e           scope,
                                       int                 verbose,
                                       int                 print );


void  printfHelp (int argc, char** argv)
{
  int i;
  fprintf( stderr, "\n");
  for(i = 0; i < argc; ++i)
    fprintf( stderr, "\'%s\' ", argv[i]);
  printf("\n");
  printf("oyranos-config v%d.%d.%d %s\n",
         OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C,
         _("is a settings administration tool"));
  printf("%s:\n",                 _("Usage"));
  printf("  %s:\n",               _("Persistent Settings"));
  printf("      %s -g XPATH\t%s\n", argv[0], _("Get a Value") );
  printf("      %s -s XPATH:VALUE\t%s\n", argv[0], _("Set a Value") );
  printf("      %s -l\t\t%s\n", argv[0], _("List existing paths") );
  printf("      %s -p\t\t%s\n", argv[0], _("Show DB File") );
#ifdef HAVE_DBUS
  printf("      %s --watch\t\t%s\n", argv[0], _("Watch DB changes") );
#endif
  printf("\n");
  printf("  %s:\n",               _("Show Install Paths"));
  printf("      %s --syscolordir\t%s\n", argv[0],  _("Path to system main color directory") );
  printf("      %s --usercolordir\t%s\n", argv[0], _("Path to users main color directory") );
  printf("      %s --iccdirname\t%s\n", argv[0], _("ICC profile directory name") );
  printf("      %s --settingsdirname\t%s\n", argv[0], _("Oyranos settings directory name") );
  printf("      %s --cmmdir\t%s\n", argv[0], _("Oyranos CMM directory name") );
  printf("      %s --metadir\t%s\n", argv[0], _("Oyranos meta module directory name") );
  printf("\n");
  printf("  %s\n",               _("Show Version"));
  printf("      %s --version\t%s\n", argv[0], _("Show official version") );
  printf("      %s --api-version\t%s\n", argv[0], _("Show version of API"));
  printf("      %s --num-version\t%s\n", argv[0], _("Show version as a simple number"));
  printf("\n");
  printf("  %s\n",               _("Show Help:"));
  printf("      %s [-h]\n", argv[0]);
  printf("\n");
  printf("  %s\n",               _("General options:"));
  printf("         -v      \t%s\n", _("verbose"));
  printf("\n");
  printf("  %s\n",               _("Miscellaneous options:"));
  printf("         --cflags\t%s\n", _("compiler flags"));
  printf("         --ldflags\t%s\n", _("dynamic link flags"));
  printf("         --ldstaticflags\t%s\n", _("static linking flags"));
  printf("         --sourcedir\t%s\n", _("Oyranos local source directory name"));
  printf("         --builddir\t%s\n", _("Oyranos local build directory name"));
  printf("\n");
  printf(_("For more information read the man page:"));
  printf("\n");
  printf("      man oyranos-config\n");
}

#ifdef HAVE_DBUS
#include "oyranos_dbus_macros.h"
#include "oyranos_threads.h"
oyDBusFilter_m
oyWatchDBus_m( oyDBusFilter )
oyFinishDBus_m
int oy_dbus_config_changed = 0;
static void oyConfigCallbackDBus     ( double              progress_zero_till_one OY_UNUSED,
                                       char              * status_text,
                                       int                 thread_id_ OY_UNUSED,
                                       int                 job_id OY_UNUSED,
                                       oyStruct_s        * cb_progress_context )
{
  const char * key;
  int verbose = oyOption_GetValueInt( (oyOption_s*)cb_progress_context, 0 );
  if(!status_text) return;

  oyGetPersistentStrings(NULL);

  key = strchr( status_text, '/' );
  if(key)
    ++key;
  else
    return;

  if(strstr(key,OY_STD) == NULL) return;

  getKey( key, oySCOPE_USER_SYS, verbose, 1/*print*/ );

  /* Clear the changed state, before a new check. */
  oy_dbus_config_changed = 1;
}

#endif /* HAVE_DBUS */


int main(int argc, char *argv[])
{
  int error = 0;
  int i;

  char * v;
  int count = 0;

  /* the functional switches */
  int verbose = 0;
  char * set = NULL;
  char * get = NULL;
  int path = 0;
  int paths = 0;
  int daemon = -1;

  oySCOPE_e scope = oySCOPE_USER_SYS;

  if(getenv(OY_DEBUG))
  {
    int value = atoi(getenv(OY_DEBUG));
    if(value > 0)
      oy_debug += value;
  }

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyInit_();

  if(argc != 1)
  {
    int pos = 1;
    const char *wrong_arg = 0;
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            for(i = 1; i < (int)strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case 'g': OY_PARSE_STRING_ARG( get ); break;
              case 's': OY_PARSE_STRING_ARG( set ); break;
              case 'l': paths = 1; break;
              case 'p': path = 1; break;
              case 'v': if(verbose++) ++oy_debug; break;
              case 'h':
              case '-':
                        if(i == 1)
                        {
                             if(OY_IS_ARG("version"))
                        { fprintf( stdout, "%s\n", oyVersionString(1,0) ); return 0; }
                        else if(OY_IS_ARG("api-version"))
                        { fprintf( stdout, "%d\n", OYRANOS_VERSION_A ); return 0; }
                        else if(OY_IS_ARG("num-version"))
                        { fprintf( stdout, "%d\n", OYRANOS_VERSION ); return 0; }
                        else if(OY_IS_ARG("cflags"))
                        { system( "pkg-config --cflags oyranos" ); i=100; break; }
                        else if(OY_IS_ARG("ldflags"))
                        { system( "pkg-config --libs oyranos" ); i=100; break; }
                        else if(OY_IS_ARG("ldstaticflags"))
                        { puts( "-L" OY_LIBDIR " -loyranos-static" ); i=100; break; }
                        else if(OY_IS_ARG("syscolordir"))
                        { puts( OY_SYSCOLORDIR "\n" ); return 0; }
                        else if(OY_IS_ARG("usercolordir"))
                        { puts( OY_USERCOLORDIR "\n" ); return 0; }
                        else if(OY_IS_ARG("iccdirname"))
                        { puts( OY_ICCDIRNAME "\n" ); return 0; }
                        else if(OY_IS_ARG("settingsdirname"))
                        { puts( OY_SETTINGSDIRNAME "\n" ); return 0; }
                        else if(OY_IS_ARG("cmmdir"))
                        { puts( OY_CMMDIR "\n" ); return 0; }
                        else if(OY_IS_ARG("metadir"))
                        { puts( OY_LIBDIR OY_SLASH OY_METASUBPATH "\n" ); return 0; }
                        else if(OY_IS_ARG("sourcedir"))
                        { puts( OY_SOURCEDIR "\n" ); return 0; }
                        else if(OY_IS_ARG("builddir"))
                        { puts( OY_BUILDDIR "\n" ); return 0; }
                        else if(OY_IS_ARG("watch"))
                        { daemon = 1; i=100; break; }
                        else if(OY_IS_ARG("verbose"))
                        { if(verbose) oy_debug += 1; verbose = 1; i=100; break;}
                        else if(OY_IS_ARG("help"))
                        { printfHelp(argc, argv); i=100; break; }
                        else if(OY_IS_ARG("system-wide"))
                        { scope = oySCOPE_SYSTEM; i=100; break; }
                        } OY_FALLTHROUGH
              default:
                        printfHelp(argc, argv);
                        exit (0);
                        break;
            }
            break;
        default:
                        printfHelp(argc, argv);
                        exit (0);
      }
      if( wrong_arg )
      {
        printf("%s %s\n", _("wrong argument to option:"), wrong_arg);
        exit(1);
      }
      ++pos;
    }
    if(oy_debug)
      for(i = 0; i < argc; ++i)
        fprintf( stderr, "\'%s\' ", argv[i]);
  }
  else
  {
                        fprintf( stdout, "oyranos" );
                        exit (0);
  }

  if(oy_debug)
    fprintf( stderr, "  Oyranos v%s\n",
                  oyNoEmptyName_m_(oyVersionString(1,0)));

  if(path)
  {
    if(scope == oySCOPE_USER_SYS) scope = oySCOPE_USER;
    v = oyGetInstallPath( oyPATH_POLICY, scope, oyAllocateFunc_ );
    if(v)
      STRING_ADD( v, "/openicc.json" );
    fprintf( stdout, "%s\n", oyNoEmptyString_m_(v) );
    if(v) oyFree_m_(v);
  }

  if(paths)
  {
    char * p;
    if(scope == oySCOPE_USER_SYS) scope = oySCOPE_USER;
    p = oyGetInstallPath( oyPATH_POLICY, scope, oyAllocateFunc_ );

    if(p)
    {
      size_t size = 0;
      STRING_ADD( p, "/openicc.json" );
      v = oyReadFileToMem_( p, &size, oyAllocateFunc_ );
      if(v)
      {
        char ** paths = NULL;
        char error_buffer[128] = {0};
        oyjl_val root = oyjlTreeParse( v, error_buffer, 128 );

        oyjlTreeToPaths( root, 1000000, NULL, OYJL_KEY, &paths );
        while(paths && paths[count]) ++count;

        for(i = 0; i < count; ++i)
        {
          if(verbose)
          {
            getKey( paths[i], scope, verbose, 1 );
          } else
            fprintf(stdout,"%s\n", paths[i]);
        }

        oyjlStringListRelease( &paths, count, free );
        oyFree_m_(v);
      }
      oyFree_m_(p);
    }
  }

  if(get)
    getKey( get, scope, verbose, 1 );

  if(set)
  {
    char ** list = oyStringSplit( set, ':', &count, oyAllocateFunc_ );

    if(count == 2)
    {
      const char * key_name = list[0];
      const char * value = list[1],
                 * comment = NULL;
      if(scope == oySCOPE_USER_SYS) scope = oySCOPE_USER;
      if(verbose) getKey( key_name, scope, verbose, 0 );
      error = oySetPersistentString( key_name, scope, value, comment );
      if(verbose)
        fprintf( stderr, "%s->%s\n", oyNoEmptyString_m_(list[0]), oyNoEmptyString_m_(value) );
      if(verbose) getKey( key_name, scope, verbose, 0 );

      oyStringListRelease( &list, count, oyDeAllocateFunc_ );

      {
        /* ping X11 observers about option change
         * ... by setting a known property again to its old value
         */
        oyOptions_s * opts = oyOptions_New(NULL), * results = 0;
        error = oyOptions_Handle( "//" OY_TYPE_STD "/send_native_update_event",
                      opts,"send_native_update_event",
                      &results );
        oyOptions_Release( &opts );
      }

    if(error)
      printf("send_native_update_event failed\n");
    }
    else
    {
                        printfHelp(argc, argv);
                        exit (0);
    }
  } 

#ifdef HAVE_DBUS
  if(daemon != -1)
  {
    int id;
    double hour_old = 0.0;
    oyOption_s * cb_option = oyOption_FromRegistration(OY_STD "/verbose",0);;
    oyOption_SetFromInt( cb_option, verbose+1, 0, 0 );

    oyStartDBusObserver( oyWatchDBus, oyFinishDBus, oyConfigCallbackDBus, OY_STD, cb_option )

    if(id && verbose)
      fprintf(stderr, "oyStartDBusObserver ID: %d\n", id);

    while(1)
    {
      double hour = oySeconds( ) / 3600.;
      double repeat_check = 1.0/60.0; /* every minute */

      oyLoopDBusObserver( hour, repeat_check, oy_dbus_config_changed, 0 )

      /* delay next polling */
      oySleep( 0.25 );
    }
  }
#endif /* HAVE_DBUS */

  oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );

  return error;
}

void  getKey                         ( const char        * key,
                                       oySCOPE_e           scope,
                                       int                 verbose,
                                       int                 print )
{
  if(key)
  {
    char * v = oyGetPersistentString( key, 0, scope, oyAllocateFunc_ );
    int hour, minute, second, gmt_diff_second;

    oyGetCurrentGMTHour( &gmt_diff_second );
    oySplitHour( oyGetCurrentLocalHour( oyGetCurrentGMTHour(0), gmt_diff_second ), &hour, &minute, &second );
    if(verbose > 1) fprintf( print == 1 ? stdout : stderr, "%02d:%02d:%02d ", hour, minute, second );
    if(verbose)     fprintf( print == 1 ? stdout : stderr, "%s:", key );
    fprintf( print ? stdout : stderr, "%s\n", oyNoEmptyString_m_(v) );
    if(v) oyFree_m_(v);
  }
}



