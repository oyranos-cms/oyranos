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
#include <sys/stat.h>

#include "oyranos.h"
#include "oyranos_helper.h"

/* ---  Helpers  --- */
#define DEBUG
int level_PROG;

#define ERR if (rc) { printf("%s:%d\n", __FILE__,__LINE__); perror("Error"); }



/* --- internal API definition --- */
/* not oyranos specific part */
int oyAddKey_valueComment (char* keyName, char* value, char* comment);
int oyAddKey_value (char* keyName, char* value);

char* oySearchEmptyKeyname (char* keyParentName, char* keyBaseName);
KeySet* oyReturnChildrenList (char* keyParentName, int* rc);

char* oyCheckFullFileName (char* name);
char* oyGetHomeDir ();
int oyIsDir (char* path);
int oyIsFile (char* fileName);
int oyMakeDir (char* path);
char* oyResolveFileDirName (char* name);

int   oyWriteMemToFile(char* name, void* mem, size_t size);
char* oyReadFileToMem(char* fileName, size_t *size);

/* oyranos part */
void oyCheckDefaultDirectories ();
char* oyFindProfile (char* name);

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

char*
oyReadFileToMem(char* name, size_t *size)
{
  FILE *fp = 0;
  int   pt = 0;
  char* mem = 0;
  char* filename = 0;
  DBG

  // check profile name
  filename = oyCheckFullFileName(name);

  if (oyIsFile(filename))
  {
    fp = fopen(filename, "r");
    #ifdef DEBUG
    printf ("fp = %d filename = %s\n", (int)fp, filename);
    #endif

    // get size
    fseek(fp,0L,SEEK_END); 
    *size = ftell (fp);
    rewind(fp);

    // allocate memory
    mem = (char*) calloc (*size, sizeof(char));

    // check and read
    if ((fp != 0)
     && mem
     && *size)
    { DBG
      int s = fread(mem, sizeof(char), *size, fp);
      // check again
      if (s != *size)
      { *size = 0;
        free (mem);
        mem = 0;
      }
    }
  }
 
  // clean up
  if (fp) fclose (fp);
  if (filename) free (filename);

  return mem;
}

int
oyWriteMemToFile(char* name, void* mem, size_t size)
{
  FILE *fp = 0;
  int   pt = 0;
  char* block = mem;
  char* filename;
  int r = 0;
  DBG
#ifdef DEBUG
  printf ("name = %s mem = %d size = %d\n", name, (int)mem, size);
#endif

  filename = oyCheckFullFileName(name);

  if (oyIsFile (filename))
  {
    fp = fopen(filename, "w");
    #ifdef DEBUG
    printf ("fp = %d filename = %s\n", (int)fp, filename);
    #endif
    if ((fp != 0)
     && mem
     && size)
    { DBG
      do {
        r = fputc ( block[pt++] , fp);
      } while (--size);
    }

    if (fp) fclose (fp);
  }

  if (filename) free (filename);

  return r;
}

char*
oyGetHomeDir ()
{
  #if (__unix__ || __APPLE__)
  char* name = (char*) getenv("HOME");
  DBG
  return name;
  #else
  return "OS not supported yet";
  #endif
}

int
oyIsDir (char* path)
{
  struct stat status;
  int r = 0;
  char* name = oyResolveFileDirName (path);
  status.st_mode = 0;
  r = stat (name, &status);
  #ifdef DEBUG
  printf("status.st_mode = %d status.st_mode = %d name = %s ", (status.st_mode&S_IFMT)&S_IFDIR, status.st_mode, name); DBG
  #endif
  if (name) free (name);
  r = !r &&
       ((status.st_mode & S_IFMT) & S_IFDIR);
  return r;
}

int
oyIsFile (char* fileName)
{
  struct stat status;
  int r = 0;
  char* name = oyResolveFileDirName (fileName);
  status.st_mode = 0;
  r = stat (name, &status);
  #ifdef DEBUG
  printf("status.st_mode = %d status.st_mode = %d name = %s ", (status.st_mode&S_IFMT)&S_IFDIR, status.st_mode, name); DBG
  #endif
  r = !r &&
       (   ((status.st_mode & S_IFMT) & S_IFREG)
        || ((status.st_mode & S_IFMT) & S_IFLNK));

  if (r)
  {
    FILE* fp = fopen (name, "r"); DBG
    if (!fp)
      r = 0;
    else
      fclose (fp);
  } DBG

  if (name) free (name); DBG

  return r;
}

int
oyMakeDir (char* path)
{
  char *name = oyResolveFileDirName (path);
  int rc = 0;
  mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; /* 0755 */
  DBG
  rc = mkdir (name, mode);
  free (name);
  return rc;
}

char*
oyResolveFileDirName (char* name)
{
  char* newName = (char*) calloc (MAX_PATH, sizeof(char)),
       *home = 0;
  int len = 0;

  // user directory
  if (name[0] == '~')
  { DBG
    home = oyGetHomeDir();
    len = strlen(name) + strlen(home) + 1;
    if (len >  FILENAME_MAX)
      printf("Warning at %s:%d : file name is too long %d\n", __FILE__,__LINE__,
              len);

    sprintf (newName, "%s%s", home, &name[0]+1);

  } else { DBG
    sprintf (newName, name);

    // relative names - where the first sign is no directory separator
    if (newName[0] != OY_SLASH_C)
    { char* cn = (char*) calloc(MAX_PATH, sizeof(char)); DBG
      sprintf (cn, "%s%s%s", getenv("PWD"), OY_SLASH, name);
      #ifdef DEBUG
      printf ("canonoical %s ", cn); DBG
      #endif
      sprintf (newName, cn);
    }
  }

  #ifdef DEBUG
  printf ("newName = %s name %s home %s ", newName, name, home); DBG
  #endif
  

  return newName;
}

char*
oyCheckFullFileName (char* name)
{
  char* ptr = 0;
  char* newName = 0, *home = 0, *dirName = 0;
  int len = 0;

  DBG
  // substitute ~ with HOME variable from environment
  if (name)
    newName = oyResolveFileDirName (name);

  // create directory name
  if(name &&
     strrchr( newName, OY_SLASH_C ))
  { DBG
    dirName = (char*) calloc (MAX_PATH, sizeof(char)); DBG
    sprintf (dirName , newName); DBG
    ptr = strrchr( dirName, OY_SLASH_C ); DBG
    ptr++;
    ptr[0] = '\0';
  } else
  { DBG
    dirName = (char*) getenv("PWD");
    ptr = 0;
  }

  #ifdef DEBUG
  printf ("newName = %s dirName = %s ", newName, dirName); DBG
  #endif

  if (dirName) free (dirName); DBG
  return newName;
}

void
oyCheckDefaultDirectories ()
{
  // test dirName : existing in path, default dirs are existing
  if (!oyIsDir (OY_DEFAULT_SYSTEM_PROFILE__PATH))
  { DBG
    printf ("no default system directory %s\n",OY_DEFAULT_SYSTEM_PROFILE__PATH);
  }

  if (!oyIsDir (OY_DEFAULT_USER_PROFILE_PATH))
  { DBG 
    printf ("Try to create users default directory %s %d ",
               OY_DEFAULT_USER_PROFILE_PATH,
    oyMakeDir( OY_DEFAULT_USER_PROFILE_PATH )); DBG
    oyPathAdd (OY_DEFAULT_USER_PROFILE_PATH);
  }
}

char*
oyFindProfile (char* fileName)
{
  char  *fullFileName = 0;
  int    success = 0;
  char  *header = 0;
  size_t size;

  DBG
  // test for pure file without dir; search in configured paths only
  if (strchr(fileName, OY_SLASH_C))
  {
    char* pathName;
    int   n_paths = oyPathsCount (),
          i;

    fullFileName = (char*) calloc (MAX_PATH, sizeof(char));

    for (i = 0; i < n_paths; i++)
    { // test profile
      pathName = oyPathName (i);
      sprintf (fullFileName, "%s%s%s", pathName, OY_SLASH, fileName);

      if (oyIsFile(fileName))
      {
        header = oyReadFileToMem (fullFileName, &size);
        if (size >= 128)
          success = oyCheckProfileMem (header, 128);
      }

      if (pathName) free (pathName);
      if (header) free (header);

      if (success) // found
        break;
    }

    if (!success)
      printf ("Warning : profile %s not found in colour path\n", fileName);

  } else
  { // else use fileName as an full qualified name, check name and test profile
    fullFileName = oyCheckFullFileName (fileName);

    if (oyIsFile(fileName))
    {
      header = oyReadFileToMem (fullFileName, &size);

      if (size >= 128)
        success = oyCheckProfileMem (header, 128);
    }

    if (!success)
      printf ("Warning : profile %s not found\n", fileName);

    if (header) free (header);
  }

  if (!success)
  { free (fullFileName);
    fullFileName = 0;
  }

  return fullFileName;
}


/* public API implementation */

/* path names API */

int
oyPathsCount ()
{
  int rc, n = 0;
  kdbOpen();

  // take all keys in the paths directory
  KeySet* myKeySet = oyReturnChildrenList(OY_USER_PATHS, &rc ); ERR
  n = myKeySet->size;

  ksClose (myKeySet);
  kdbClose();

  oyCheckDefaultDirectories();
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

  // compare and erase if matches
  for (current=myKeySet->start; current; current=current->next)
  {
    keyGetString(current, value, MAX_PATH);
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

  // set "SLEEP" in comment
  for (current=myKeySet->start; current; current=current->next)
  {
    keyGetString(current, value, MAX_PATH);
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

  // erase "SLEEP" from comment
  for (current=myKeySet->start; current; current=current->next)
  {
    keyGetString(current, value, MAX_PATH);
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

int
oySetDefaultImageProfile          (char* name)
{ DBG
  return oyAddKey_valueComment (OY_DEFAULT_IMAGE_PROFILE, name, "");
}

int
oySetDefaultWorkspaceProfile      (char* name)
{ DBG
  if ( oyCheckProfile (name) )
  // TODO accept all sort of directory name combinations
  return oyAddKey_valueComment (OY_DEFAULT_WORKSPACE_PROFILE, name, "");
}

int
oySetDefaultCmykProfile           (char* name)
{ DBG
  return oyAddKey_valueComment (OY_DEFAULT_CMYK_PROFILE, name, "");
}

int
oySetDefaultImageProfileBlock     (char* name, void* mem, size_t size)
{ DBG
  int r;
  char* fileName = (char*) calloc (sizeof(char),
                      strlen(OY_DEFAULT_USER_PROFILE_PATH) + strlen (name) + 4);

  sprintf (fileName, "%s%s%s", OY_DEFAULT_USER_PROFILE_PATH, OY_SLASH, name);

  if (oyCheckProfileMem( mem, size))
  { r = oyWriteMemToFile (fileName, mem, size);
    oySetDefaultImageProfile (name);
  }

  free (fileName);
  return r;
}

int
oySetDefaultWorkspaceProfileBlock (char* name, void* mem, size_t size)
{ DBG
  int r = 0;
  char* fileName;

  strrchr (name, OY_SLASH_C);
  fileName = (char*) calloc (sizeof(char),
                      strlen(OY_DEFAULT_USER_PROFILE_PATH) + strlen (name) + 4);

  sprintf (fileName, "%s%s%s", OY_DEFAULT_USER_PROFILE_PATH, OY_SLASH, name);

  if (oyCheckProfileMem( mem, size))
  { r = oyWriteMemToFile (fileName, mem, size);
    oySetDefaultWorkspaceProfile (name);
  }

  #ifdef DEBUG
  printf ("%s %s %d %d ", fileName, name, &((char*)mem)[0] , size); DBG
  #endif
  free (fileName);
  return r;
}

char*
oyGetDefaultWorkspaceProfileName  ()
{
  // 
}

int
oySetDefaultCmykProfileBlock      (char* name, void* mem, size_t size)
{ DBG
  int r = -1;
  char* fileName = (char*) calloc (sizeof(char),
                      strlen(OY_DEFAULT_USER_PROFILE_PATH) + strlen (name) + 4);
  sprintf (fileName, "%s%s%s", OY_DEFAULT_USER_PROFILE_PATH, OY_SLASH, name);

  if (oyCheckProfileMem( mem, size))
  { r = oyWriteMemToFile (fileName, mem, size);
    oySetDefaultCmykProfile (name);
  }

  free (fileName);
  return r;
}

int
oyCheckProfile (char* name)
{
  char *fullName = 0;
  char* header; 
  size_t size = 0;
  int r = 0;

  fullName = oyFindProfile(name);

  // do check
  if (oyIsFile(name))
    header = oyReadFileToMem (fullName, &size); DBG

  if (size >= 128)
    r = oyCheckProfileMem (header, 128);

  DBG
  return r;
}

int
oyCheckProfileMem (void* mem, size_t size)
{
  char* block = (char*) mem;
  int offset = 36;
  if (size >= 128 &&
      block[offset+0] == 'a' &&
      block[offset+1] == 'c' &&
      block[offset+2] == 's' &&
      block[offset+3] == 'p' )
  { DBG
    return 1;
  } else
  {
    printf ("Warning : False profile - size = %d pos = %lu ", size, block);
    if (size >= 128)
      printf(" sign: %c%c%c%c ", (char)block[offset+0], (char)block[offset+1], (char)block[offset+2], (char)block[offset+3] );
    DBG
    return 0;
  }
}


