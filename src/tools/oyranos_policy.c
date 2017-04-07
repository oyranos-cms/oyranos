/** @file oyranos_policy.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2006-2015 (C) Kai-Uwe Behrmann
 *
 *  @brief    policy loader - for usage during installation and on commandline
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2006/09/14
 *
 *  The program takes a policy XML file as argument and sets the behaviour 
 *  accordingly.
 */


#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_helper_macros.h"
#include "oyranos_internal.h"
#include "oyranos_config.h"
#include "oyranos_sentinel.h"
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
  fprintf( stderr, "oyranos-policy %s\n",
                                _("is a policy administration tool"));
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
  fprintf( stderr, "  %s\n",               _("Dump out the actual settings:"));
  fprintf( stderr, "      %s -d\n",        argv[0]);
  fprintf( stderr, "  %s\n",               _("Select active policy:"));
  fprintf( stderr, "      %s -i %s\n",     argv[0], _("policy or filename"));
  fprintf( stderr, "  %s\n",               _("List available policies:"));
  fprintf( stderr, "      %s -l [-f] [-e]\n",        argv[0]);
  fprintf( stderr, "  %s\n",               _("Currently active policy:"));
  fprintf( stderr, "      %s -c [-f] [-e]\n",        argv[0]);
  fprintf( stderr, "             -f %s\n", _("full path and file name"));
  fprintf( stderr, "             -e %s\n", _("display name"));
  fprintf( stderr, "  %s\n",               _("List search paths:"));
  fprintf( stderr, "      %s -p\n",        argv[0]);
  fprintf( stderr, "  %s\n",               _("Save to a new policy:"));
  fprintf( stderr, "      %s -s %s [--system-wide]\n",     argv[0], _("policy name"));
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
  const char* save_policy = NULL,
            * import_policy = NULL;
  oySCOPE_e scope = oySCOPE_USER;
  size_t size = 0;
  char * xml = NULL;
  char * import_policy_fn = NULL;
  int current_policy = 0, list_policies = 0, list_paths = 0,
      dump_policy = 0;
  int long_help = 0,
      docbook = 0,
      internal_name = 0,
      file_name = 0;
  const char * doc_title = NULL,
             * doc_version = NULL;
  int verbose = 0;

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
            for(i = 1; i < strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case 'c': current_policy = 1; break;
              case 'd': dump_policy = 1; break;
              case 'e': internal_name = 1; break;
              case 'f': file_name = 1; break;
              case 'i': OY_PARSE_STRING_ARG(import_policy); break;
              case 'l': list_policies = 1; break;
              case 'p': list_paths = 1; break;
              case 's': OY_PARSE_STRING_ARG(save_policy); break;
              case 'v': if(verbose) oy_debug += 1; verbose = 1; break;
              case '-':
                        if(i == 1)
                        {
                             if(OY_IS_ARG("help"))
                        { long_help = 1; i=100; break; }
                        else if(OY_IS_ARG("docbook"))
                        { docbook = 1; i=100; break; }
                        else if(OY_IS_ARG("doc-title"))
                        { OY_PARSE_STRING_ARG2(doc_title, "doc-title"); break; }
                        else if(OY_IS_ARG("doc-version"))
                        { OY_PARSE_STRING_ARG2(doc_version, "doc-version"); break; }
                        else if(OY_IS_ARG("path"))
                        { list_paths = 1; i=100; break; }
                        else if(OY_IS_ARG("system-wide"))
                        { scope = oySCOPE_SYSTEM; i=100; break; }
                        }
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

  if(verbose)
    fprintf( stderr, "  Oyranos v%s\n",
                  oyNoEmptyName_m_(oyVersionString(1,0)));

  /* check the default paths */
  /*oyPathAdd( OY_PROFILE_PATH_USER_DEFAULT );*/


  /* load the policy file into memory */
  import_policy_fn = oyMakeFullFileDirName_(import_policy);
  if(oyIsFile_(import_policy_fn))
  {
    xml = oyReadFileToMem_( oyMakeFullFileDirName_(import_policy), &size,
                            oyAllocateFunc_ );
    oyDeAllocateFunc_( import_policy_fn );
  }
  /* parse and set policy */
  if(xml)
  {
    oyReadXMLPolicy( oyGROUP_ALL, xml );
    oyDeAllocateFunc_( xml );
  }
  else if ( import_policy )
  {
    error = oyPolicySet( import_policy, 0 );
    if(error)
      fprintf( stderr, "%s:%d could not read file: %s\n",__FILE__,__LINE__, import_policy);
    return 1;
  }

  if(save_policy)
  {
    error = oyPolicySaveActual( oyGROUP_ALL, scope, save_policy );
    if(!error)
      fprintf( stdout, "%s \"%s\"\n",
               _("installed new policy"), save_policy);
    else
      fprintf( stdout, "\"%s\" %s %d\n", save_policy,
               _("installation of new policy file failed with error:"), error);

  } else
  if(current_policy || list_policies || list_paths)
  {
    const char ** names = NULL;
    int count = 0, i, current = -1;
    oyOptionChoicesGet( oyWIDGET_POLICY, &count, &names, &current );

    if(list_policies)
      for(i = 0; i < count; ++i)
      {
        if(file_name)
        {
          char * full_name = NULL;
          error = oyPolicyFileNameGet_( names[i],
                                            &full_name,
                                            oyAllocateFunc_ );
          if(error)
            fprintf(stderr, "%s error: %d\n", names[i], error);
          if(internal_name)
            fprintf(stdout, "%s (%s)\n", names[i], full_name);
          else
            fprintf(stdout, "%s\n", full_name);
          oyFree_m_( full_name );
        } else
          fprintf(stdout, "%s\n", names[i]);
      }

    if(current_policy)
    {
      fprintf( stderr, "%s\n", _("Currently active policy:"));
      if(current >= 0 && file_name)
      {
        char * full_name = NULL;
        error = oyPolicyFileNameGet_( names[current], &full_name,
                                          oyAllocateFunc_ );
        if(internal_name)
          fprintf(stdout, "%s (%s)\n", names[current], full_name);
        else
          fprintf(stdout, "%s\n", full_name);
        oyFree_m_( full_name );
      } else
        fprintf( stdout, "%s\n", current>=0?names[current]:"---");
    }

    if(list_paths)
    {
      char ** path_names = oyDataPathsGet_( &count, "color/settings",
                                              oyALL, oySCOPE_USER_SYS,
                                              oyAllocateFunc_ );
      fprintf(stdout, "%s:\n", _("Policy search paths"));
      for(i = 0; i < count; ++i)
        fprintf(stdout, "%s\n", path_names[i]);

      oyStringListRelease_(&path_names, count, oyDeAllocateFunc_);
    }

  } else
  if(dump_policy)
  {
    size = 0;
    xml = oyPolicyToXML( oyGROUP_ALL, 1, oyAllocateFunc_ );
    DBG_PROG2_S("%s:%d new policy:\n\n",__FILE__,__LINE__);
    fprintf(stdout, "%s\n", xml);

    if(xml) oyDeAllocateFunc_( xml );

  } else
  if(long_help)
  {
    const char * opts[] = {"add_html_header","1",
                           "add_oyranos_title","1",
                           "add_oyranos_copyright","1",
                           NULL,NULL,
                           NULL,NULL,
                           NULL};
    int pos = 6;
    if(doc_title)
    {
      opts[pos++] = "title";
      opts[pos++] = doc_title;
    }
    if(doc_version)
    {
      opts[pos++] = "version";
      opts[pos++] = doc_version;
    }
    size = 0;
    xml = oyDescriptionToHTML( oyGROUP_ALL, opts, oyAllocateFunc_ );
    fprintf(stdout, "%s\n", xml);

    if(xml) oyDeAllocateFunc_( xml );
  } else
  if(docbook)
  {
    const char * opts[] = {"add_html_header","1",
                           "add_oyranos_title","1",
                           "add_oyranos_copyright","1",
                           "format","2",
                           NULL,NULL,
                           NULL,NULL,
                           NULL};
    int pos = 8;
    if(doc_title)
    {
      opts[pos++] = "title";
      opts[pos++] = doc_title;
    }
    if(doc_version)
    {
      opts[pos++] = "version";
      opts[pos++] = doc_version;
    }
    size = 0;
    xml = oyDescriptionToHTML( oyGROUP_ALL, opts, oyAllocateFunc_ );
    fprintf(stdout, "%s\n", xml);

    if(xml) oyDeAllocateFunc_( xml );
  }

  oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );

  return error;
}
