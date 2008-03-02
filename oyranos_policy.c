/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2005-2006  Kai-Uwe Behrmann
 *
 * Autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * -----------------------------------------------------------------------------
 */

/** @file @internal
 * @brief policy loader - for usage during installation and on commandline
 *
 * It takes a policy XML file as argument and sets the behaviour accordingly
 * 
 */

/* @date      14. 09. 2006 */


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

  setlocale(LC_ALL,"");
  oyI18NInit_();

  if(argc >= 1)
  {
    int pos = 1;
    char *wrong_arg = 0;
    printf("argc: %d\n", argc);
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            switch (argv[pos][1])
            {
              case 'h':
              default:
                        printf("\n");
                        printf("oyranos-policy v%d.%d.%d %s\n",
                        OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C,
                                _("is a policy administration tool"));
                        printf("%s\n",                 _("Usage"));
                        printf("  %s\n",               _("Set new policy:"));
                        printf("      %s\n",           argv[0]);
                        printf("            %s\n",     _("policy filename"));
                        printf("\n");
                        printf("\n");
                        exit (0);
                        break;
            }
            break;
        default:
            fileName = argv[pos];
      }
      if( wrong_arg )
      {
        printf("%s %s\n", _("wrong argument to option:"), wrong_arg);
        exit(1);
      }
      ++pos;
    }
  }

  /* check the default paths */
  oyPathAdd( OY_PROFILE_PATH_USER_DEFAULT );


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
    printf("%s:%d could not read file: %s\n",__FILE__,__LINE__, fileName);
    return 1;
  }

  if(!fileName)
  {
    size = 0;
    xml = oyPolicyToXML( oyGROUP_ALL, 1, oyAllocateFunc_ );
    printf("%s:%d new policy:\n%s\n",__FILE__,__LINE__,xml);

    if(xml) oyDeAllocateFunc_( xml );
  }

  return error;
}
