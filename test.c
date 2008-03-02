/*
 * Oyranos - an open source Colour Management System 
 * 
 * Copyright (C) 2004  Kai-Uwe Behrmann 
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
 *
 * test
 * 
 */

// Date:      25. 11. 2004

#include <kdb.h>

#include "oyranos.h"


int
main()
{
  char a[] = {"/Hallo/share/color/icc/display"};

  int rc = oyPathAdd (a), i;

  if (rc)
    printf ("rc = %d\n", rc); 

  for (i = 0; i < 100; i++) {
    oyPathRemove (a);
    oyPathAdd (a);
  }

  printf ("count of paths = %d\n", oyPathsRead());

  for (i = 0; i < oyPathsRead(); i++)
  { char* name = oyPathName(i);
    printf ("the %dth path has value = %s\n", i, name);
    free (name);
  }

  oyPathSleep (a);
  for (i = 0; i < oyPathsRead(); i++)
  { char* name = oyPathName(i);
    printf ("the %dth path has value = %s\n", i, name);
    free (name);
  }

  oyPathActivate (a);
  for (i = 0; i < oyPathsRead(); i++)
  { char* name = oyPathName(i);
    printf ("the %dth path has value = %s\n", i, name);
    free (name);
  }


  oyPathRemove(a);

  printf ("count of paths after removing one = %d\n", oyPathsRead());

  oySetDefaultCmykProfile ("CMYK.icc");

  {
    char *profil = (char*) calloc (sizeof(char), 3000);
    printf ("profil = %lu written  %s\n", profil, "example_workspace.icm");
    oySetDefaultWorkspaceProfileBlock ("example_workspace.icm", profil, 3000);
    free (profil);
  }

  return 0;
}


