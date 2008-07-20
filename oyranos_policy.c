/** @file oyranos_policy.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2006-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    policy loader - for usage during installation and on commandline
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2006/09/14
 */

/**
 *
 * It takes a policy XML file as argument and sets the behaviour accordingly
 * 
 */


#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_config.h"
#include "oyranos_version.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void* oyAllocFunc(size_t size) {return malloc (size);}

int main( int argc , char** argv )
{
  int error = 0;
  const char* fileName = NULL;
  size_t size = 0;
  char *xml = NULL;

  if(getenv("OYRANOS_DEBUG"))
  {
    int value = atoi(getenv("OYRANOS_DEBUG"));
    if(value > 0)
      oy_debug = value;
  }

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyI18NInit_();

  if(argc >= 1)
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
              case 'h':
              default:
                        fprintf( stderr, "\n");
                        fprintf( stderr, "oyranos-policy v%d.%d.%d %s\n",
                        OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C,
                                _("is a policy administration tool"));
                        fprintf( stderr, "%s\n",                 _("Usage"));
                        fprintf( stderr, "  %s\n",               _("Set new policy:"));
                        fprintf( stderr, "      %s\n",           argv[0]);
                        fprintf( stderr, "            %s\n",     _("policy filename"));
                        fprintf( stderr, "\n");
                        fprintf( stderr, "\n");
                        exit (0);
                        break;
            }
            break;
        default:
            fileName = argv[pos];
      }
      if( wrong_arg )
      {
        fprintf(stderr, "%s %s\n", _("wrong argument to option:"), wrong_arg);
        exit(1);
      }
      ++pos;
    }
  }

  /* check the default paths */
  /*oyPathAdd( OY_PROFILE_PATH_USER_DEFAULT );*/


  /* load the policy file into memory */
  xml = oyReadFileToMem_( oyMakeFullFileDirName_(fileName), &size,
                          oyAllocateFunc_ );
  /* parse and set policy */
  if(xml)
  {
    oyReadXMLPolicy( oyGROUP_ALL, xml );
    oyDeAllocateFunc_( xml );
  }
  else if ( fileName )
  {
    fprintf( stderr, "%s:%d could not read file: %s\n",__FILE__,__LINE__, fileName);
    return 1;
  }

  if(!fileName)
  {
    size = 0;
    xml = oyPolicyToXML( oyGROUP_ALL, 1, oyAllocateFunc_ );
    DBG_PROG2_S("%s:%d new policy:\n\n",__FILE__,__LINE__);
    fprintf(stdout, "%s\n", xml);

    if(xml) oyDeAllocateFunc_( xml );
  }

  return error;
}
