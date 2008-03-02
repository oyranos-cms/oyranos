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

#define DEBUG 1
int oy_debug = 0;

#include <kdb.h>
#include <sys/stat.h>

#include "oyranos.h"
#include "oyranos_helper.h"

/* ---  Helpers  --- */
int level_PROG = 0;
clock_t _oyranos_clock = 0;
#define ERR if (rc) { printf("%s:%d\n", __FILE__,__LINE__); perror("Error"); }



/* --- internal API definition --- */
/* not oyranos specific part */
int oyAddKey_valueComment (char* keyName, char* value, char* comment);
int oyAddKey_value (char* keyName, char* value);

char* oySearchEmptyKeyname (char* keyParentName, char* keyBaseName);
KeySet* oyReturnChildrenList (char* keyParentName, int* rc);

char* oyMakeFullFileDirName (char* name);
char* oyResolveDirFileName (char* name);
char* oyGetHomeDir ();
int oyIsDir (char* path);
int oyIsFile (char* fileName);
int oyIsFileFull (char* fullFileName);
int oyMakeDir (char* path);

int   oyWriteMemToFile(char* name, void* mem, size_t size);
char* oyReadFileToMem(char* fullFileName, size_t *size);

/* oyranos part */
void oyCheckDefaultDirectories ();
char* oyFindProfile (char* name);

// should mayby all public



/* --- function definitions --- */

KeySet*
oyReturnChildrenList (char* keyParentName, int* rc)
{ DBG_PROG_START
  KeySet *myKeySet = (KeySet*) malloc (sizeof(KeySet) * 1);
  ksInit(myKeySet);

  *rc = kdbGetChildKeys(keyParentName, myKeySet,KDB_O_RECURSIVE);

  DBG_PROG_ENDE
  return myKeySet;
}

char*
oySearchEmptyKeyname (char* keyParentName, char* keyBaseName)
{ DBG_PROG_START
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

  DBG_PROG_ENDE
  return keyName;
} 

int
oyAddKey_valueComment (char* keyName, char* value, char* comment)
{ DBG_PROG_START
  int rc;
  Key key;

    rc=keyInit(&key); ERR
    rc=keySetName (&key, keyName);
    rc=kdbGetKey(&key);
    rc=keySetString (&key, value);
    rc=keySetComment (&key, comment);
    rc=kdbSetKey(&key);

  DBG_PROG_ENDE
  return rc;
}

int
oyAddKey_value (char* keyName, char* value)
{ DBG_PROG_START
  int rc;
  Key key;

    rc=keyInit(&key); ERR
    rc=keySetName (&key, keyName);
    rc=kdbGetKey(&key);
    rc=keySetString (&key, value);
    rc=kdbSetKey(&key);

  DBG_PROG_ENDE
  return rc;
}

char*
oyReadFileToMem(char* name, size_t *size)
{ DBG_PROG_START
  FILE *fp = 0;
  int   pt = 0;
  char* mem = 0;
  char* filename = oyResolveDirFileName (name);

  DBG_PROG

  if (oyIsFileFull(filename))
  {
    fp = fopen(filename, "r");
    DBG_PROG_S (("fp = %d filename = %s\n", (int)fp, filename))

    if (fp)
    {
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
      { DBG_PROG
        int s = fread(mem, sizeof(char), *size, fp);
        // check again
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
 
  // clean up
  if (fp) fclose (fp);
  if (filename) free (filename); DBG_PROG

  DBG_PROG_ENDE
  return mem;
}

int
oyWriteMemToFile(char* name, void* mem, size_t size)
{ DBG_PROG_START
  FILE *fp = 0;
  int   pt = 0;
  char* block = mem;
  char* filename;
  int r = 0;

  DBG_PROG_S(("name = %s mem = %d size = %d\n", name, (int)mem, size))

  filename = oyMakeFullFileDirName(name);

  {
    fp = fopen(filename, "w");
    DBG_PROG_S(("fp = %d filename = %s", (int)fp, filename))
    if ((fp != 0)
     && mem
     && size)
    { DBG_PROG
      do {
        r = fputc ( block[pt++] , fp);
      } while (--size);
    }

    if (fp) fclose (fp);
  }

  if (filename) free (filename);

  DBG_PROG_ENDE
  return r;
}

char*
oyGetHomeDir ()
{ DBG_PROG_START
  #if (__unix__ || __APPLE__)
  char* name = (char*) getenv("HOME");
  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
  #else
  DBG_PROG_ENDE
  return "OS not supported yet";
  #endif
}

int
oyIsDir (char* path)
{ DBG_PROG_START
  struct stat status;
  int r = 0;
  char* name = oyResolveDirFileName (path);
  status.st_mode = 0;
  r = stat (name, &status);
  DBG_PROG_S(("status.st_mode = %d", (status.st_mode&S_IFMT)&S_IFDIR))
  DBG_PROG_S(("status.st_mode = %d", status.st_mode))
  DBG_PROG_S(("name = %s ", name))
  if (name) free (name);
  r = !r &&
       ((status.st_mode & S_IFMT) & S_IFDIR);

  DBG_PROG_ENDE
  return r;
}

int
oyIsFileFull (char* fullFileName)
{ DBG_PROG_START
  struct stat status;
  int r = 0;
  char* name = fullFileName;
  status.st_mode = 0;
  r = stat (name, &status);
  DBG_NUM_S(("status.st_mode = %d", (status.st_mode&S_IFMT)&S_IFDIR))
  DBG_NUM_S(("status.st_mode = %d", status.st_mode))
  DBG_NUM_S(("name = %s", name))
  r = !r &&
       (   ((status.st_mode & S_IFMT) & S_IFREG)
        || ((status.st_mode & S_IFMT) & S_IFLNK));

  if (r)
  {
    FILE* fp = fopen (name, "r"); DBG_PROG
    if (!fp)
      r = 0;
    else
      fclose (fp);
  } DBG_PROG

  DBG_PROG_ENDE
  return r;
}

int
oyIsFile (char* fileName)
{ DBG_PROG_START
  int r = 0;
  char* name = oyResolveDirFileName (fileName);

  r = oyIsFileFull(name);

  if (name) free (name); DBG_PROG

  DBG_PROG_ENDE
  return r;
}

int
oyMakeDir (char* path)
{ DBG_PROG_START
  char *name = oyResolveDirFileName (path);
  int rc = 0;
  mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; /* 0755 */
  DBG_PROG
  rc = mkdir (name, mode);
  free (name);

  DBG_PROG_ENDE
  return rc;
}

char*
oyResolveDirFileName (char* name)
{ DBG_PROG_START
  char* newName = (char*) calloc (MAX_PATH, sizeof(char)),
       *home = 0;
  int len = 0;

  DBG_PROG_S((name))

  // user directory
  if (name[0] == '~')
  { DBG_PROG_S(("in home directory"))
    home = oyGetHomeDir();
    len = strlen(name) + strlen(home) + 1;
    if (len >  FILENAME_MAX)
      printf("Warning at %s:%d : file name is too long %d\n", __FILE__,__LINE__,
              len);

    sprintf (newName, "%s%s", home, &name[0]+1);

  } else { DBG_PROG_S(("resolve  directory"))
    sprintf (newName, name);

    // relative names - where the first sign is no directory separator
    if (newName[0] != OY_SLASH_C)
    { char* cn = (char*) calloc(MAX_PATH, sizeof(char)); DBG_PROG
      sprintf (cn, "%s%s%s", getenv("PWD"), OY_SLASH, name);
      DBG_PROG_S(("canonoical %s ", cn))
      sprintf (newName, cn);
    }
  }

  DBG_PROG_S (("name %s", name))
  DBG_PROG_S (("home %s", home))
  DBG_PROG_S (("newName = %s", newName))

  DBG_PROG_ENDE
  return newName;
}

char*
oyMakeFullFileDirName (char* name)
{ DBG_PROG_START
  char* ptr = 0;
  char* newName = (char*) calloc (MAX_PATH, sizeof(char)), *dirName = 0;
  int len = 0;

  DBG_PROG
  if(name &&
     strrchr( name, OY_SLASH_C ))
  { DBG_PROG
    // substitute ~ with HOME variable from environment
    if (name)
      newName = oyResolveDirFileName (name);
  } else
  { DBG_PROG
    // create directory name
    dirName = (char*) getenv("PWD");
    DBG_PROG_S(("dirName = %s", dirName))
    sprintf (newName, "%s%s%s", newName, OY_SLASH_C, dirName);
    if (dirName) free (dirName); DBG_PROG
  }

  DBG_PROG_S(("newName = %s", newName))

  DBG_PROG_ENDE
  return newName;
}

void
oyCheckDefaultDirectories ()
{ DBG_PROG_START
  // test dirName : existing in path, default dirs are existing
  if (!oyIsDir (OY_DEFAULT_SYSTEM_PROFILE__PATH))
  { DBG_PROG
    printf ("no default system directory %s\n",OY_DEFAULT_SYSTEM_PROFILE__PATH);
  }

  if (!oyIsDir (OY_DEFAULT_USER_PROFILE_PATH))
  { DBG_PROG 
    printf ("Try to create users default directory %s %d ",
               OY_DEFAULT_USER_PROFILE_PATH,
    oyMakeDir( OY_DEFAULT_USER_PROFILE_PATH )); DBG_PROG
  }
  DBG_PROG_ENDE
}

char*
oyFindProfile (char* fileName)
{ DBG_PROG_START
  char  *fullFileName = 0;
  int    success = 0;
  char  *header = 0;
  size_t size;

  DBG_NUM_S((fileName))
  // test for pure file without dir; search in configured paths only
  if (!strchr(fileName, OY_SLASH_C))
  {
    char* pathName;
    int   n_paths = oyPathsCount (),
          i;

    DBG_PROG_S(("pure filename found"))
    fullFileName = (char*) calloc (MAX_PATH, sizeof(char));

    for (i = 0; i < n_paths; i++)
    { // test profile
      pathName = oyPathName (i);
      sprintf (fullFileName, "%s%s%s", pathName, OY_SLASH, fileName);

      DBG_PROG_S((pathName))
      DBG_PROG_S((fullFileName))

      if (oyIsFileFull(fullFileName))
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
    DBG_PROG_S(("dir/filename found"))
    fullFileName = oyMakeFullFileDirName (fileName);

    if (oyIsFileFull(fullFileName))
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

  DBG_PROG_ENDE
  return fullFileName;
}


/* public API implementation */

/* path names API */

int
oyPathsCount ()
{ DBG_PROG_START
  int rc, n = 0;
  kdbOpen();

  // take all keys in the paths directory
  KeySet* myKeySet = oyReturnChildrenList(OY_USER_PATHS, &rc ); ERR
  n = myKeySet->size;

  ksClose (myKeySet);
  kdbClose();

  DBG_PROG_ENDE
  return n;
}

char*
oyPathName (int number)
{ DBG_PROG_START
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

  DBG_PROG_ENDE
  return value;
}

int
oyPathAdd (char* pfad)
{ DBG_PROG_START
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

  oyCheckDefaultDirectories();

  DBG_PROG_ENDE
  return rc;
}

void
oyPathRemove (char* pfad)
{ DBG_PROG_START
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
      DBG_NUM_S(( "remove" ))
    }
  }

  ksClose (myKeySet);
  kdbClose();
  free (keyName);
  free (value);

  oyPathAdd (OY_DEFAULT_USER_PROFILE_PATH);

  DBG_PROG_ENDE
}

void
oyPathSleep (char* pfad)
{ DBG_PROG_START
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
      DBG_NUM_S(( "sleep" ))
    }
  }

  ksClose (myKeySet);
  kdbClose();
  free (value);
  DBG_PROG_ENDE
}

void
oyPathActivate (char* pfad)
{ DBG_PROG_START
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
      DBG_NUM_S(( "wake up" ))
    }
  }

  ksClose (myKeySet);
  kdbClose();
  free (value);
  DBG_PROG_ENDE
}

/* default profiles API */

int
oySetDefaultImageProfile          (char* name)
{ DBG_PROG_START
  int r =
  oyAddKey_valueComment (OY_DEFAULT_IMAGE_PROFILE, name, "");

  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultWorkspaceProfile      (char* name)
{ DBG_PROG_START
  int r;
  char *ptr = strrchr(name , OY_SLASH_C);

  if ( oyCheckProfile (name) )
  { DBG
  // TODO accept all sort of directory name combinations
    if (ptr)
    { r =
      oyAddKey_valueComment (OY_DEFAULT_WORKSPACE_PROFILE, name, "");
      DBG_PROG_S((name))
    } else {
      r =
      oyAddKey_valueComment (OY_DEFAULT_WORKSPACE_PROFILE, ptr, "");
      DBG_PROG_S((ptr))
    }
  }

  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultCmykProfile           (char* name)
{ DBG_PROG_START
  int r =
  oyAddKey_valueComment (OY_DEFAULT_CMYK_PROFILE, name, "");

  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultImageProfileBlock     (char* name, void* mem, size_t size)
{ DBG_PROG_START
  int r = 0;
  char* fileName = (char*) calloc (sizeof(char),
                      strlen(OY_DEFAULT_USER_PROFILE_PATH) + strlen (name) + 4);

  sprintf (fileName, "%s%s%s", OY_DEFAULT_USER_PROFILE_PATH, OY_SLASH, name);

  if (oyCheckProfileMem( mem, size))
  { r = oyWriteMemToFile (fileName, mem, size);
    oySetDefaultImageProfile (name);
  }

  free (fileName);

  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultWorkspaceProfileBlock (char* name, void* mem, size_t size)
{ DBG_PROG_START
  int r = 0;
  char* fileName;

  //TODO
  strrchr (name, OY_SLASH_C);
  fileName = (char*) calloc (sizeof(char),
                      strlen(OY_DEFAULT_USER_PROFILE_PATH) + strlen (name) + 4);

  sprintf (fileName, "%s%s%s", OY_DEFAULT_USER_PROFILE_PATH, OY_SLASH, name);

  if (oyCheckProfileMem( mem, size))
  { r = oyWriteMemToFile (fileName, mem, size);
    oySetDefaultWorkspaceProfile (name);
  }

  DBG_PROG_S(("%s", name))
  DBG_PROG_S(("%s", fileName))
  DBG_PROG_S(("%d %d", &((char*)mem)[0] , size))
  free (fileName);

  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultCmykProfileBlock      (char* name, void* mem, size_t size)
{ DBG_PROG_START
  int r = 0;
  char* fileName = (char*) calloc (sizeof(char),
                      strlen(OY_DEFAULT_USER_PROFILE_PATH) + strlen (name) + 4);

  sprintf (fileName, "%s%s%s", OY_DEFAULT_USER_PROFILE_PATH, OY_SLASH, name);

  if (oyCheckProfileMem( mem, size))
  { r = oyWriteMemToFile (fileName, mem, size);
    oySetDefaultCmykProfile (name);
  }

  free (fileName);

  DBG_PROG_ENDE
  return r;
}

char*
oyGetDefaultImageProfileName      ()
{ DBG_PROG_START
  char* name = (char*) calloc (MAX_PATH, sizeof(char));
  
  kdbGetValue (OY_DEFAULT_IMAGE_PROFILE, name, MAX_PATH);

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultWorkspaceProfileName  ()
{ DBG_PROG_START
  char* name = (char*) calloc (MAX_PATH, sizeof(char));

  kdbGetValue (OY_DEFAULT_WORKSPACE_PROFILE, name, MAX_PATH);

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultCmykProfileName       ()
{ DBG_PROG_START
  char* name = (char*) calloc (MAX_PATH, sizeof(char));

  kdbGetValue (OY_DEFAULT_CMYK_PROFILE, name, MAX_PATH);

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
}


/* profile check API */

int
oyCheckProfile (char* name)
{ DBG_PROG_START
  char *fullName = 0;
  char* header; 
  size_t size = 0;
  int r = 0;

  DBG_NUM_S((name))
  fullName = oyFindProfile(name);
  DBG_NUM_S((fullName))

  // do check
  if (oyIsFileFull(fullName))
    header = oyReadFileToMem (fullName, &size); DBG_PROG

  if (size >= 128)
    r = oyCheckProfileMem (header, 128);
  DBG_NUM_S(("oyCheckProfileMem = %d",r))

  DBG_PROG_ENDE
  return r;
}

int
oyCheckProfileMem (void* mem, size_t size)
{ DBG_PROG_START
  char* block = (char*) mem;
  int offset = 36;
  if (size >= 128 &&
      block[offset+0] == 'a' &&
      block[offset+1] == 'c' &&
      block[offset+2] == 's' &&
      block[offset+3] == 'p' )
  {
    DBG_PROG_ENDE
    return 1;
  } else
  {
    printf ("Warning : False profile - size = %d pos = %lu ", size, block);
    if (size >= 128)
      printf(" sign: %c%c%c%c ", (char)block[offset+0], (char)block[offset+1], (char)block[offset+2], (char)block[offset+3] );

    DBG_PROG_ENDE
    return 0;
  }
}


