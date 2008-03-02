/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2005  Kai-Uwe Behrmann
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
 * monitor device detection
 * 
 */

/* Date:      31. 01. 2005 */

#define DEBUG 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "oyranos_helper.h"
#include "oyranos_definitions.h"
#include "oyranos_debug.h"

#include <X11/Xutil.h>

/* ---  Helpers  --- */

/* --- internal API definition --- */

char*
oyGetMonitorProfile_              (const char* display,
                                   char*       manufacturer,
                                   char*       model,
                                   char*       serial);

/* separate from the external functions */

char*
oyReadFileToMem(char* name, size_t *size)
{
  FILE *fp = 0;
  char* mem = 0;
  char* filename = name;


  {
    fp = fopen(filename, "r");

    if (fp)
    {
      /* get size */
      fseek(fp,0L,SEEK_END);
      *size = ftell (fp);
      rewind(fp);

      /* allocate memory */
      mem = (char*) calloc (*size, sizeof(char));

      /* check and read */
      if ((fp != 0)
       && mem
       && *size)
      {
        int s = fread(mem, sizeof(char), *size, fp);
        /* check again */
        if (s != *size)
        { *size = 0;
          free (mem);
          mem = 0;
        }
      }
    } else {
      printf ("could not read %s\n", filename);
    }
  }

  /* clean up */
  if (fp) fclose (fp);

  return mem;
}

enum {
  MANUFACTURER = 1,
  PRODUCT = 2,
  PRODUCT_ID = 4
};

struct DDC_EDID1 {
 char dummy[78];
 char HW_ID[10];
 char dummy2[7];
 char Mnf_Model[16];
};

char*
parseMoniDDC(char* block, int opts)
{
  char *info = (char*) calloc (1024, sizeof(char)),
       *tmp = (char*) calloc (1024, sizeof(char)), *moni=0;
  struct DDC_EDID1 *ddc_info = (struct DDC_EDID1*)info;
  int i, r=1;

  // Wie ist diese X function ausserhalb X aufzurufen?
  //cout << xf86PrintEDID() << endl;

  // suche XFree86_DDC_EDID1_RAWDATA und dann lese alle zahlen ab "="
  if (block)
  {
    char* ptr = strstr(block, "XFree86_DDC_EDID1_RAWDATA");
    if (ptr)
    {
      //printf( "Monitor DDC found %s\n", ptr );
      ptr = strchr(ptr, '=');
      if (ptr)
      { int count;
        //printf( "start of values %s\n", ptr+1 );
        ptr++;

        /* XFree86_DDC_EDID1_RAWDATA scheint 128 Buchstaben abzuliefern */
        for (count = 0; count < 128; count ++)
        { char num[5];
          int c;
          int i = 0, len = 0;

          while ( isspace(ptr[0]) )
            ptr++;
          while ( isgraph((ptr+i)[0]) )
            i++;
          len = i;
          memcpy(num, ptr, len);
          if (len <= 5 && num[len-1] == ',')
          {
            num[len-1] = 0;
          }
          else if (len < 5)
            num[len+1] = 0;
          else
          { printf("parse error at char %d with value %d ", count, c); DBG
            exit(1);
          }

          c = atoi(num);
          if ((c > 128) || (c < -128))
          {
            printf("parse error at char %d with value %d ", count, c); DBG
            info[count] = 0;
          } else
            info[count] = c;
          ptr = ptr + len;
        }
      }
    } else {
      r = 0;
      printf( "Monitor DDC not found\n" ); DBG
    }

  }
  if (r)
  {
    if (strlen (ddc_info->Mnf_Model) > 16)
      ddc_info->Mnf_Model[16] = 0;
    if (strlen (ddc_info->HW_ID) > 10)
      ddc_info->HW_ID[10] = 0;
    if (opts & MANUFACTURER || opts & PRODUCT)
      sprintf(tmp, "%s", ddc_info->Mnf_Model);
    if (opts & PRODUCT_ID)
      sprintf(&tmp[strlen(tmp)], "%s", ddc_info->HW_ID);

    moni = (char*) calloc (strlen(tmp)+1, sizeof(char));
    sprintf (moni, tmp);
    moni [strlen(moni)] = 10;
  }

  if (info) free (info);
  if (tmp) free (tmp);

  return moni;
}

char*
oyReadMoniDDC(char* display, int opts)
{
  char *tempName = (char*) calloc (1024,sizeof(char));
  size_t size; 
  char *info = 0;
  
  // temporäres Verzeichnis aussuchen
  char *tmp = getenv("TMPDIR");
  if (tmp)
    #ifdef DEBUG
    printf( "tmp = %s\n",tmp );
    #else
    ;
    #endif
  else
    tmp = "/tmp";
  
  // Automatisches auslesen der Monitordaten über X
  sprintf (tempName, "sh -c \"xprop -display %s -root >& %s/xDDC.txt\"",display, tmp);
  system(tempName);
  sprintf (tempName, "%s/xDDC.txt", tmp);
  
  char* block = oyReadFileToMem (tempName, &size);
  if (block)
  { 
    info = parseMoniDDC (block, opts);
    free (block);
  }
  if (tempName) free (tempName);
  
  return info;
}

static int
ignore_errors (Display *dpy, XErrorEvent *ev)
{
    return 0;
}


static const char *
Format_Atom (Atom atom, Display* display)
{
    const char *found;
    char *name;
    XErrorHandler handler;
    static char buf_[10100];

//    if ((found = GetAtomName(atom)) != NULL)
//    return found;

    handler = XSetErrorHandler (ignore_errors);
    name = XGetAtomName(display, atom);
    XSetErrorHandler(handler);
    if (! name)
    sprintf(buf_, "undefined atom # 0x%lx", atom);
    else {
    int namelen = strlen(name);
    if (namelen > 10000) namelen = 10000;
    memcpy(buf_, name, namelen);
    buf_[namelen] = '\0';
    XFree(name);
    }
    return buf_;
}


char*
oyGetMonitorProfile_              (const char* display_name,
                                   char*       manufacturer,
                                   char*       model,
                                   char*       serial)
{ DBG_PROG_START

  Display *display = XOpenDisplay(display_name); DBG_PROG
  int screen = 0;
  Window w;
  Atom *atoms, atom;
  const char *name;
  int count, i;

  DBG_PROG_V((display_name))

  if( !display )
    WARN_S(("open X Display failed"))

  screen = DefaultScreen(display); DBG_PROG_V(("%d",screen))
  w = RootWindow(display, screen); DBG_PROG

  atom = XInternAtom(display, "XFree86_DDC_EDID1_RAWDATA", 1);

  #if 0
  if (screen != -1) { DBG_PROG_V( screen )
    atoms = XListProperties(display, screen, &count);
    for (i = 0; i < count; i++) { DBG_PROG_V( i )
        name = Format_Atom(atoms[i], display);
        //Show_Prop(NULL, NULL, name);
    }
  }
  #endif

  int zahl; DBG_PROG
  DBG_PROG_V(("zahl: %d",zahl))

  long low = 1;
    for (; low>0 ; low++) {
      char *s = XGetAtomName (display, (Atom)low);
      if (s) {
        //if (strtok(s,"DDC"))
        {
          printf ("%ld %s\n", low, s);
        }
        XFree (s);
      } else
        low = -2;
    }


  DBG_PROG_ENDE
  return 0;
}


char*
oyGetMonitorProfile               (const char* display,
                                   char*       manufacturer,
                                   char*       model,
                                   char*       serial)
{ DBG_PROG_START
  char* moni_profile = 0;

  moni_profile = oyGetMonitorProfile_( display, manufacturer, model, serial );

  DBG_PROG_ENDE
  return moni_profile;
}


