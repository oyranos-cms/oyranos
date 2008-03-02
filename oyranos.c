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
 * sorting
 * 
 */

// Date:      25. 11. 2004


#include <kdb.h>

#include "oyranos.h"

// definitions
#define OY_USER_PATHS "user/sw/oyranos/paths"
#define OY_USER_PATH  "path"

#ifndef MAX_PATH
#define MAX_PATH 1024
#endif


/* ---  Helpers  --- */
//#define DEBUG

#ifdef DEBUG
#define DBG printf("%s:%d\n", __FILE__,__LINE__);
#define DBG_S( text ) printf("%s:%d %s = %s\n", __FILE__,__LINE__, #text, text);
#else
#define DBG
#define DBG_S( text )
#endif

#define ERR if (rc) { printf("%s:%d\n", __FILE__,__LINE__); perror("Error"); }



/* --- internal API --- */
/* generic part */
int oyAddKey_valueComment (char* keyName, char* value, char* comment);
int oyAddKey_value (char* keyName, char* value);

char* oySearchEmptyKeyname (char* keyParentName, char* keyBaseName);
KeySet* oyReturnChildrenList (char* keyParentName, int* rc);

/* oyranos part */


/* --- function definitions --- */

KeySet*
oyReturnChildrenList (char* keyParentName, int* rc)
{ 
  KeySet *myKeySet = (KeySet*) malloc (sizeof(KeySet) * 1);
  ksInit(myKeySet);

  *rc = kdbGetChildKeys(keyParentName, myKeySet,KDB_O_RECURSIVE);

  return myKeySet;
}

char*
oySearchEmptyKeyname (char* keyParentName, char* keyBaseName)
{
  char* keyName = (char*)     calloc (strlen(keyParentName)
                                    + strlen(keyBaseName) + 24, sizeof(char));
  char* pathkeyName = (char*) calloc (strlen(keyBaseName) + 24, sizeof(char));
  int nth = 0, i = 1, rc;
  Key key;

  rc=keyInit(&key); ERR

    // search for empty keyname
    while (!nth)
    { sprintf (pathkeyName , "%s%d", keyBaseName, i);
      rc=kdbGetKeyByParent (keyParentName, pathkeyName, &key);
      if (rc != KDB_RET_OK)
        nth = i;
      i++;
    }
    sprintf (keyName, ("%s/%s"), OY_USER_PATHS, pathkeyName);

  return keyName;
} 

int
oyAddKey_valueComment (char* keyName, char* value, char* comment)
{
  int rc;
  Key key;

    rc=keyInit(&key); ERR
    rc=keySetName (&key, keyName);
    rc=kdbGetKey(&key);
    rc=keySetString (&key, value);
    rc=keySetComment (&key, comment);
    rc=kdbSetKey(&key);

  return rc;
}

int
oyAddKey_value (char* keyName, char* value)
{
  int rc;
  Key key;

    rc=keyInit(&key); ERR
    rc=keySetName (&key, keyName);
    rc=kdbGetKey(&key);
    rc=keySetString (&key, value);
    rc=kdbSetKey(&key);

  return rc;
}

void
oyHandleError (int rc)
{
}

int
oyPathAdd (char* pfad)
{
  int rc, n = 0;
  Key *current;
  char* keyName = (char*) calloc (sizeof(char), MAX_PATH);
  char* value = (char*) calloc (sizeof(char), MAX_PATH);

  kdbOpen();

  // take all keys in the paths directory
  KeySet* myKeySet = oyReturnChildrenList(OY_USER_PATHS, &rc ); ERR

  // search for allready included path
  for (current=myKeySet->start; current; current=current->next)
  {
    keyGetString(current, value, MAX_PATH);
    DBG_S( value )
    if (strcmp (value, pfad) == 0)
      n++;		
  }

  if (n) printf ("Key was allready %d times there\n",n);

  // erase double occurencies of this path
  if (n > 1)
  { for (current=myKeySet->start; current; current=current->next)
    {
      rc=keyGetString(current,value, MAX_PATH); ERR

      if (strcmp (value, pfad) == 0
       && n)
      {
        rc=keyGetFullName(current,keyName, MAX_PATH); ERR
        rc=kdbRemove(keyName); ERR
        n--;
      }
    }
  }

  // create new key
  if (!n)
  {
    // search for empty keyname
    keyName = oySearchEmptyKeyname (OY_USER_PATHS, "path");

    // write key
    rc = oyAddKey_value (keyName, pfad);
  }

  ksClose (myKeySet);
  kdbClose();
  free (keyName);
  free (value);
  return rc;
}


