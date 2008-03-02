/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004  Kai-Uwe Behrmann
 *
 * Autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * -----------------------------------------------------------------------------
 *
 * test
 * 
 */

// Date:      25. 11. 2004

#include <kdb.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <oyranos.h>

char* _oyReadFileToMem(char* fullFileName, size_t *size);

int
main(int argc, char** argv)
{
  char a[] = {"/Hallo/share/color/icc/display"};

  int rc = oyPathAdd (a), i;

  if (rc)
    printf ("rc = %d\n", rc); 

  for (i = 0; i < 1; i++) {
    oyPathRemove (a);
    oyPathAdd (a);
  }

  printf ("count of paths = %d\n", oyPathsCount());

  for (i = 0; i < oyPathsCount(); i++)
  { char* name = oyPathName(i);
    printf ("the %dth path has value = %s\n", i, name);
    free (name);
  }

  oyPathSleep (a);
  for (i = 0; i < oyPathsCount(); i++)
  { char* name = oyPathName(i);
    printf ("the %dth path has value = %s\n", i, name);
    free (name);
  }

  oyPathActivate (a);
  for (i = 0; i < oyPathsCount(); i++)
  { char* name = oyPathName(i);
    printf ("the %dth path has value = %s\n", i, name);
    free (name);
  }

  oyPathRemove(a);

  printf ("count of paths after removing one = %d\n\n", oyPathsCount());


  // set an (hopefully existing) profile as default
  rc = oySetDefaultCmykProfile ("CMYK.icc");
  printf ("setting CMYK.icc %d\n\n", rc);


  {
    char *profil = (char*) calloc (sizeof(char), 3000);
    int r = 0;

    r=oySetDefaultWorkspaceProfileBlock ("example_workspace.icm", profil, 3000);
    if (r)
      printf ("profil = %d written  %s\n\n", (int)profil, "example_workspace.icm");
    else
      printf ("profil = %d invalid  %s\n\n", (int)profil, "example_workspace.icm");
      
    free (profil);

    // take an real file as input
    if (argc > 0)
    { printf (":%d %d %s %s\n",__LINE__, argc, argv[0], argv[1]);
      size_t size = 0;
      profil = oyGetProfileBlock (argv[1], &size);

      printf ("size = %u pos = %d\n\n",size, (int) profil);
      if (profil)
        r=oySetDefaultWorkspaceProfileBlock ("ex_workspace.icm", profil, size);
      r=oySetDefaultWorkspaceProfile("~/.color/../.color/icc/ex_workspace.icm");
      if (r)
        printf ("profil = %d written  %s\n\n", (int)profil, "ex_workspace.icm");
      else
        printf ("profil = %d not written  %s\n\n", (int)profil, "ex_workspace.icm");
      
      free (profil);

      printf ("%s exist %d\n\n", argv[1], !oyCheckProfile(argv[1], 0));
    }
  }


  // Show what we have
  { char* name = oyGetDefaultImageProfileName();
    printf ("default %s profile = %s\n", OY_DEFAULT_IMAGE_PROFILE, name);
    name = oyGetDefaultWorkspaceProfileName();
    printf ("default %s profile = %s\n", OY_DEFAULT_WORKSPACE_PROFILE, name);
    name = oyGetDefaultCmykProfileName();
    printf ("default %s profile = %s\n", OY_DEFAULT_CMYK_PROFILE, name);
    free (name);
  }

  {
    char  *tempName = (char*) calloc (1024,sizeof(char));
    char  *model = 0, *product_ID = 0;
    size_t size;
    char* profil = 0;

    sprintf (tempName, "sh -c \"getMoniDDC1 -m >& $TMPDIR/MoniDDC.txt\"");
    if (!system(tempName))
    {
      sprintf (tempName, "%s/MoniDDC.txt", getenv("TMPDIR"));
      model = _oyReadFileToMem (tempName, &size);
      memcpy (tempName, model, size); tempName[size] = 0;
      if (model) free (model);
      model = (char*) calloc (strlen(tempName)+1,sizeof(char));
      sprintf (model, tempName);
      printf ("%s %d\n", model, strlen(model));
    } else
      model = 0;

    sprintf (tempName, "sh -c \"getMoniDDC1 -id >& $TMPDIR/MoniDDC.txt\"");
    if (!system(tempName))
    {
      sprintf (tempName, "%s/MoniDDC.txt", getenv("TMPDIR"));
      product_ID = _oyReadFileToMem (tempName, &size);
      memcpy (tempName, product_ID, size); tempName[size] = 0;
      if (product_ID) free (product_ID);
      product_ID = (char*) calloc (strlen(tempName)+1,sizeof(char));
      sprintf (product_ID, tempName);
      printf ("%s %d\n", product_ID, strlen(product_ID));
    } else
      product_ID = 0;

    //oy_debug = 1;
    if (model && product_ID)
    {
      printf ("%s %s\n",model, product_ID);
      //oySetDeviceProfile(DISPLAY,0,model,product_ID,0,0,0,0,0,"M20040609_NatWP_50H_50K.icm",0,0);
      profil = oyGetDeviceProfile (DISPLAY, 0, model, product_ID, "monitor",0,0,0,0);
    }
    //oy_debug = 0;
    printf ("selected profile = %s \n", profil);

    if (product_ID) free(product_ID);
    if (model) free (model);
  }

  return 0;
}


