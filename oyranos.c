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
/* not oyranos specific part */
int oyAddKey_valueComment (char* keyName, char* value, char* comment);
int oyAddKey_value (char* keyName, char* value);

char* oySearchEmptyKeyname (char* keyParentName, char* keyBaseName);
KeySet* oyReturnChildrenList (char* keyParentName, int* rc);

void oyWriteMemToFile(char* name, void* mem, size_t size);
char* oyGetHomeDir ();
char* oyCheckFilename (char* name);

/* oyranos part */
// should mayby all public

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
oyWriteMemToFile(char* name, void* mem, size_t size)
{
  FILE *fp = 0;
  int   pt = 0;
  char* block = mem;
  char* filename;
  DBG
#ifdef DEBUG
  printf ("name = %s mem = %d size = %d\n", name, (int)mem, size);
#endif

  filename = oyCheckFilename(name);
  fp = fopen(filename, "w");
#ifdef DEBUG
  printf ("fp = %d filename = %s\n", (int)fp, filename);
#endif
  if ((fp != 0)
   && mem
   && size)
  { DBG
    do {
      fputc ( block[pt++] , fp);
    } while (--size);
    fclose (fp);
  }

  if (filename) free (filename);
}

char*
oyGetHomeDir ()
{
  #if (__unix__ || __APPLE__)
  char* name = (char*) getenv("HOME");
  return name;
  #else
  return "OS not supported yet";
  #endif
}

char*
oyCheckFilename (char* name)
{
  char* ptr = strchr(name, '~');
  char* newName = 0, *home = 0, *dirName = 0;
  int len = 0;

  DBG
  // substitute ~ with HOME variable from environment
  if (ptr)
  { DBG
    home = oyGetHomeDir();
    #ifdef DEBUG
    printf ("name = %s home = %s\n", name, home);
    #endif
    len = strlen(name) + strlen(home) + 1;
    if (len >  FILENAME_MAX)
      printf("Warning at %s:%d : file name is too long %d\n", __FILE__,__LINE__,
              len);

    newName = (char*) calloc (len, sizeof(char));
    sprintf (newName, "%s%s", home, ptr+1);
    #ifdef DEBUG
    printf ("newName = %s dirName = %s\n", newName, dirName);
    #endif
    DBG
  } else
  { DBG
    newName = name;
    #ifdef DEBUG
    printf ("name = %s home = %s\n", name, home);
    #endif
  }

  // create directory name
  if(strrchr( newName, OY_SLASH_C ))
  { DBG
    dirName = (char*) calloc (strlen(newName) + 1, sizeof(char)); DBG
    sprintf (dirName , newName); DBG
    ptr = strrchr( dirName, OY_SLASH_C ); DBG
    ptr++;
    ptr[0] = '\0';
  } else
  { DBG
    dirName = (char*) getenv("PWD");
    ptr = 0;
  }


  // TODO test dirName : existing, in path, default dir exists
  
  #ifdef DEBUG
  printf ("newName = %s dirName = %s\n", newName, dirName);
  #endif
  DBG

  if (dirName) free (dirName); DBG
  return newName;
}


/* public API implementation */

/* path names API */

int
oyPathsRead ()
{
  int rc, n = 0;
  kdbOpen();

  // take all keys in the paths directory
  KeySet* myKeySet = oyReturnChildrenList(OY_USER_PATHS, &rc ); ERR
  n = myKeySet->size;

  ksClose (myKeySet);
  kdbClose();
  return n;
}

char*
oyPathName (int number)
{
  int rc, n = 0;
  Key *current;
  char* value = (char*) calloc (sizeof(char), MAX_PATH);

  kdbOpen();

  // take all keys in the paths directory
  KeySet* myKeySet = oyReturnChildrenList(OY_USER_PATHS, &rc ); ERR

  if (number <= myKeySet->size)
    for (current=myKeySet->start; current; current=current->next)
    {
      if (number == n) {
        keyGetComment (current, value, MAX_PATH);
        if (strstr(value, OY_SLEEP) == 0)
          keyGetString(current, value, MAX_PATH);
        DBG_S( value )
      }
      n++;
    }

  ksClose (myKeySet);
  kdbClose();

  return value;
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
    keyName = oySearchEmptyKeyname (OY_USER_PATHS, OY_USER_PATH);

    // write key
    rc = oyAddKey_valueComment (keyName, pfad, "");
  }

  ksClose (myKeySet);
  kdbClose();
  free (keyName);
  free (value);
  return rc;
}

void
oyPathRemove (char* pfad)
{
  int rc;
  Key *current;
  char* value = (char*) calloc (sizeof(char), MAX_PATH);
  char* keyName = (char*) calloc (sizeof(char), MAX_PATH);

  kdbOpen();

  // take all keys in the paths directory
  KeySet* myKeySet = oyReturnChildrenList(OY_USER_PATHS, &rc ); ERR

  for (current=myKeySet->start; current; current=current->next)
  {
    keyGetString(current, value, MAX_PATH);
    DBG_S( value )
    if (strcmp (value, pfad) == 0)
    {
      keyGetFullName(current,keyName, MAX_PATH); ERR
      kdbRemove (keyName);
      DBG_S( "remove" )
    }
  }

  ksClose (myKeySet);
  kdbClose();
  free (keyName);
  free (value);
}

void
oyPathSleep (char* pfad)
{
  int rc;
  Key *current;
  char* value = (char*) calloc (sizeof(char), MAX_PATH);

  kdbOpen();

  // take all keys in the paths directory
  KeySet* myKeySet = oyReturnChildrenList(OY_USER_PATHS, &rc ); ERR

  for (current=myKeySet->start; current; current=current->next)
  {
    keyGetString(current, value, MAX_PATH);
    DBG_S( value )
    if (strcmp (value, pfad) == 0)
    {
      keySetComment (current, OY_SLEEP);
      kdbSetKey (current);
      DBG_S( "sleep" )
    }
  }

  ksClose (myKeySet);
  kdbClose();
  free (value);
}

void
oyPathActivate (char* pfad)
{
  int rc;
  Key *current;
  char* value = (char*) calloc (sizeof(char), MAX_PATH);

  kdbOpen();

  // take all keys in the paths directory
  KeySet* myKeySet = oyReturnChildrenList(OY_USER_PATHS, &rc ); ERR

  for (current=myKeySet->start; current; current=current->next)
  {
    keyGetString(current, value, MAX_PATH);
    DBG_S( value )
    if (strcmp (value, pfad) == 0)
    {
      keySetComment (current, "");
      kdbSetKey (current);
      DBG_S( "wake up" )
    }
  }

  ksClose (myKeySet);
  kdbClose();
  free (value);
}

/* default profiles API */

void
oySetDefaultImageProfile          (char* name)
{ DBG
  oyAddKey_valueComment (OY_DEFAULT_IMAGE_PROFILE, name, "");
}

void
oySetDefaultWorkspaceProfile      (char* name)
{ DBG
  // TODO oyCheckProfile (name);
  // TODO accept all sort of directory name combinations
  oyAddKey_valueComment (OY_DEFAULT_WORKSPACE_PROFILE, name, "");
}

void
oySetDefaultCmykProfile           (char* name)
{ DBG
  oyAddKey_valueComment (OY_DEFAULT_CMYK_PROFILE, name, "");
}

void
oySetDefaultImageProfileBlock     (char* name, void* mem, size_t size)
{ DBG
  char* fileName = (char*) calloc (sizeof(char),
                      strlen(OY_DEFAULT_USER_PROFILE_PATH) + strlen (name) + 4);
  sprintf (fileName, "%s%s%s", OY_DEFAULT_USER_PROFILE_PATH, OY_SLASH, name);
  oyWriteMemToFile (fileName, mem, size);
  oySetDefaultImageProfile (name);
  free (fileName);
}

void
oySetDefaultWorkspaceProfileBlock (char* name, void* mem, size_t size)
{ DBG
  char* fileName = (char*) calloc (sizeof(char),
                      strlen(OY_DEFAULT_USER_PROFILE_PATH) + strlen (name) + 4);

  sprintf (fileName, "%s%s%s", OY_DEFAULT_USER_PROFILE_PATH, OY_SLASH, name);
  // TODO oyCheckProfileMem (mem, size);
  oyWriteMemToFile (fileName, mem, size);
  #ifdef DEBUG
  DBG
  printf ("%s %s %d %d\n", fileName, name, &((char*)mem)[0] , size);
  #endif
  oySetDefaultWorkspaceProfile (name);
  free (fileName);
}

void
oySetDefaultCmykProfileBlock      (char* name, void* mem, size_t size)
{ DBG
  char* fileName = (char*) calloc (sizeof(char),
                      strlen(OY_DEFAULT_USER_PROFILE_PATH) + strlen (name) + 4);
  sprintf (fileName, "%s%s%s", OY_DEFAULT_USER_PROFILE_PATH, OY_SLASH, name);
  oyWriteMemToFile (fileName, mem, size);
  oySetDefaultCmykProfile (name);
  free (fileName);
}


