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

#include "oyranos_helper.h"
#include "oyranos_definitions.h"

/* ---  Helpers  --- */
int level_PROG = 0;
clock_t _oyranos_clock = 0;
#define ERR if (rc) { printf("%s:%d\n", __FILE__,__LINE__); perror("Error"); }



/* --- internal API definition --- */

/* separate from the external functions */
int	_oyPathsCount         (void);
char*	_oyPathName           (int number);
int	_oyPathAdd            (char* pathname);
void	_oyPathRemove         (char* pathname);
void	_oyPathSleep          (char* pathname);
void	_oyPathActivate       (char* pathname);

int	_oySetDefaultImageProfile          (char* name);
int	_oySetDefaultImageProfileBlock     (char* name, void* mem, size_t size);
int	_oySetDefaultWorkspaceProfile      (char* name);
int	_oySetDefaultWorkspaceProfileBlock (char* name, void* mem, size_t size);
int	_oySetDefaultCmykProfile           (char* name);
int	_oySetDefaultCmykProfileBlock      (char* name, void* mem, size_t size);

char*	_oyGetDefaultImageProfileName      ();
char*	_oyGetDefaultWorkspaceProfileName  ();
char*	_oyGetDefaultCmykProfileName       ();

int	_oyCheckProfile (char* name);
int	_oyCheckProfileMem (void* mem, size_t size);

/* elektra key wrapper */
int _oyAddKey_valueComment (char* keyName, char* value, char* comment);
int _oyAddKey_value (char* keyName, char* value);

/* elektra key list handling */
char* _oySearchEmptyKeyname (char* keyParentName, char* keyBaseName);
KeySet* _oyReturnChildrenList (char* keyParentName, int* rc);

/* complete an name from file including oyResolveDirFileName */
char* oyMakeFullFileDirName (char* name);
/* find an file/dir and do corrections on  ~ ; ../  */
char* oyResolveDirFileName (char* name);
char* oyGetHomeDir ();
char* oyGetParent (char* name);

int oyIsDir (char* path);
int oyIsFile (char* fileName);
int oyIsFileFull (char* fullFileName);
int oyMakeDir (char* path);

int   oyWriteMemToFile(char* name, void* mem, size_t size);
char* oyReadFileToMem(char* fullFileName, size_t *size);

/* oyranos part */
/* check for the global and the users directory */
void oyCheckDefaultDirectories ();
/* search in profile path and in current path */
char* oyFindProfile (char* name);




/* --- function definitions --- */

KeySet*
_oyReturnChildrenList (char* keyParentName, int* rc)
{ DBG_PROG_START
  KeySet *myKeySet = (KeySet*) malloc (sizeof(KeySet) * 1);
  ksInit(myKeySet);

  *rc = kdbGetChildKeys(keyParentName, myKeySet,KDB_O_RECURSIVE);

  DBG_PROG_ENDE
  return myKeySet;
}

char*
_oySearchEmptyKeyname (char* keyParentName, char* keyBaseName)
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
_oyAddKey_valueComment (char* keyName, char* value, char* comment)
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
_oyAddKey_value (char* keyName, char* value)
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
  char* filename = name;

  DBG_PROG

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

  filename = name;

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

char*
oyGetParent (char* name)
{ DBG_PROG_START
  char *parentDir = (char*) calloc ( MAX_PATH, sizeof(char)), *ptr;

  sprintf (parentDir, name);
  ptr = strrchr( parentDir, OY_SLASH_C);
  if (ptr)
  {
    if (ptr[1] == 0) // ending dir separator
    {
      ptr[0] = 0;
      if (strrchr( parentDir, OY_SLASH_C))
      {
        ptr = strrchr (parentDir, OY_SLASH_C);
        ptr[0] = 0;
      }
    }
    else
      ptr[0] = 0;
  }

  DBG_PROG_S((parentDir))

  DBG_PROG_ENDE
  return parentDir;
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
  char* parentDefaultUserDir;

  // test dirName : existing in path, default dirs are existing
  if (!oyIsDir (OY_DEFAULT_SYSTEM_PROFILE__PATH))
  { DBG_PROG
    printf ("no default system directory %s\n",OY_DEFAULT_SYSTEM_PROFILE__PATH);
  }

  if (!oyIsDir (OY_DEFAULT_USER_PROFILE_PATH))
  { DBG_PROG 
    parentDefaultUserDir = oyGetParent (OY_DEFAULT_USER_PROFILE_PATH);

    if (!oyIsDir (parentDefaultUserDir))
    { DBG_PROG 
      printf ("Try to create part of users default directory %s %d \n",
                 parentDefaultUserDir,
      oyMakeDir( parentDefaultUserDir)); DBG_PROG
    }
    free (parentDefaultUserDir);

    printf ("Try to create users default directory %s %d \n",
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
    char* pathName = (char*) calloc (MAX_PATH, sizeof(char));
    int   n_paths = _oyPathsCount (),
          i;

    DBG_PROG_S(("pure filename found"))
    fullFileName = (char*) calloc (MAX_PATH, sizeof(char));

    for (i = 0; i < n_paths; i++)
    { // test profile
      char* ptr = _oyPathName (i);
      pathName = oyMakeFullFileDirName (ptr);
      sprintf (fullFileName, "%s%s%s", pathName, OY_SLASH, fileName);

      DBG_PROG_S((pathName))
      DBG_PROG_S((fullFileName))

      if (oyIsFileFull(fullFileName))
      {
        header = oyReadFileToMem (fullFileName, &size);
        if (size >= 128)
          success = _oyCheckProfileMem (header, 128);
      }

      if (ptr) free (ptr);
      if (header) free (header);

      if (success) // found
        break;
    }

    if (!success)
      printf ("Warning : profile %s not found in colour path\n", fileName);

    if (pathName) free (pathName);

  } else
  { // else use fileName as an full qualified name, check name and test profile
    DBG_PROG_S(("dir/filename found"))
    fullFileName = oyMakeFullFileDirName (fileName);

    if (oyIsFileFull(fullFileName))
    {
      header = oyReadFileToMem (fullFileName, &size);

      if (size >= 128)
        success = _oyCheckProfileMem (header, 128);
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
_oyPathsCount ()
{ DBG_PROG_START
  int rc, n = 0;
  kdbOpen();

  // take all keys in the paths directory
  KeySet* myKeySet = _oyReturnChildrenList(OY_USER_PATHS, &rc ); ERR
  n = myKeySet->size;

  ksClose (myKeySet);
  kdbClose();

  DBG_PROG_ENDE
  return n;
}

char*
_oyPathName (int number)
{ DBG_PROG_START
  int rc, n = 0;
  Key *current;
  char* value = (char*) calloc (sizeof(char), MAX_PATH);

  kdbOpen();

  // take all keys in the paths directory
  KeySet* myKeySet = _oyReturnChildrenList(OY_USER_PATHS, &rc ); ERR

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
_oyPathAdd (char* pfad)
{ DBG_PROG_START
  int rc, n = 0;
  Key *current;
  char* keyName = (char*) calloc (sizeof(char), MAX_PATH);
  char* value = (char*) calloc (sizeof(char), MAX_PATH);

  kdbOpen();

  // take all keys in the paths directory
  KeySet* myKeySet = _oyReturnChildrenList(OY_USER_PATHS, &rc ); ERR

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
    keyName = _oySearchEmptyKeyname (OY_USER_PATHS, OY_USER_PATH);

    // write key
    rc = _oyAddKey_valueComment (keyName, pfad, "");
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
_oyPathRemove (char* pfad)
{ DBG_PROG_START
  int rc;
  Key *current;
  char* value = (char*) calloc (sizeof(char), MAX_PATH);
  char* keyName = (char*) calloc (sizeof(char), MAX_PATH);

  kdbOpen();

  // take all keys in the paths directory
  KeySet* myKeySet = _oyReturnChildrenList(OY_USER_PATHS, &rc ); ERR

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

  _oyPathAdd (OY_DEFAULT_USER_PROFILE_PATH);

  DBG_PROG_ENDE
}

void
_oyPathSleep (char* pfad)
{ DBG_PROG_START
  int rc;
  Key *current;
  char* value = (char*) calloc (sizeof(char), MAX_PATH);

  kdbOpen();

  // take all keys in the paths directory
  KeySet* myKeySet = _oyReturnChildrenList(OY_USER_PATHS, &rc ); ERR

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
_oyPathActivate (char* pfad)
{ DBG_PROG_START
  int rc;
  Key *current;
  char* value = (char*) calloc (sizeof(char), MAX_PATH);

  kdbOpen();

  // take all keys in the paths directory
  KeySet* myKeySet = _oyReturnChildrenList(OY_USER_PATHS, &rc ); ERR

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
_oySetDefaultProfile      (char* name, char* typ)
{ DBG_PROG_START
  int r;
  char *fileName = 0;

  if (strrchr(name , OY_SLASH_C))
  {
    fileName = strrchr(name , OY_SLASH_C);
    fileName++;
  }
  else
    fileName = name;

  DBG_PROG_S(("name = %s", name))

  if ( _oyCheckProfile (fileName) )
  { DBG_PROG_S(("set fileName = %s as default %s profile\n",fileName, typ))
      if (strstr (typ , "Image"))
        r =
        _oyAddKey_valueComment (OY_DEFAULT_IMAGE_PROFILE, fileName, "");
      else
      if (strstr (typ , "Workspace"))
        r =
        _oyAddKey_valueComment (OY_DEFAULT_WORKSPACE_PROFILE, fileName, "");
      else
      if (strstr (typ , "Cmyk"))
        r =
        _oyAddKey_valueComment (OY_DEFAULT_CMYK_PROFILE, fileName, "");
      else
        printf ("!!! ERROR typ %s type does not exist for default profiles",__FILE__,__LINE__, typ);
  }

  DBG_PROG_ENDE
  return r;
}

int
_oySetDefaultImageProfile          (char* name)
{ DBG_PROG_START
  int r = _oySetDefaultProfile (name, "Image");
  DBG_PROG_ENDE
  return r;
}

int
_oySetDefaultWorkspaceProfile      (char* name)
{ DBG_PROG_START
  int r = _oySetDefaultProfile (name, "Workspace");
  DBG_PROG_ENDE
  return r;
}

int
_oySetDefaultCmykProfile           (char* name)
{ DBG_PROG_START
  int r = _oySetDefaultProfile (name, "Cmyk");
  DBG_PROG_ENDE
  return r;
}

int
_oySetDefaultProfileBlock (char* name, void* mem, size_t size, char* typ)
{ DBG_PROG_START
  int r = 0;
  char *fullFileName, *fileName, *resolvedFN;

  if (strrchr (name, OY_SLASH_C))
    fileName = strrchr (name, OY_SLASH_C);
  else
    fileName = name;

  fullFileName = (char*) calloc (sizeof(char),
                  strlen(OY_DEFAULT_USER_PROFILE_PATH) + strlen (fileName) + 4);

  sprintf (fullFileName, "%s%s%s",
           OY_DEFAULT_USER_PROFILE_PATH, OY_SLASH, fileName);

  resolvedFN = oyResolveDirFileName (fullFileName);
  free (fullFileName);
  fullFileName = resolvedFN;

  if (_oyCheckProfileMem( mem, size))
  {
    DBG_PROG_S((fullFileName))
    if ( oyIsFile(fullFileName) )
      printf ("Warning : file %s exist , please remove befor installing new profile\n", fullFileName);
    else
    { r = oyWriteMemToFile (fullFileName, mem, size);
      if (strstr (typ , "Image"))
        _oySetDefaultImageProfile (fileName);
      else
      if (strstr (typ , "Workspace"))
        _oySetDefaultWorkspaceProfile (fileName);
      else
      if (strstr (typ , "Cmyk"))
        _oySetDefaultCmykProfile (fileName);
      else
        printf ("!!! ERROR typ %s type does not exist for default profiles",__FILE__,__LINE__, typ);
    }
  }

  DBG_PROG_S(("%s", name))
  DBG_PROG_S(("%s", fileName))
  DBG_PROG_S(("%d %d", &((char*)mem)[0] , size))
  free (fullFileName);

  DBG_PROG_ENDE
  return r;
}

int
_oySetDefaultImageProfileBlock     (char* name, void* mem, size_t size)
{ DBG_PROG_START
  int r = _oySetDefaultProfileBlock (name, mem, size, "Image");
  DBG_PROG_ENDE
  return r;
}

int
_oySetDefaultWorkspaceProfileBlock (char* name, void* mem, size_t size)
{ DBG_PROG_START
  int r = _oySetDefaultProfileBlock (name, mem, size, "Workspace");
  DBG_PROG_ENDE
  return r;
}

int
_oySetDefaultCmykProfileBlock      (char* name, void* mem, size_t size)
{ DBG_PROG_START
  int r = _oySetDefaultProfileBlock (name, mem, size, "Cmyk");
  DBG_PROG_ENDE
  return r;
}

char*
_oyGetDefaultImageProfileName      ()
{ DBG_PROG_START
  char* name = (char*) calloc (MAX_PATH, sizeof(char));
  
  kdbGetValue (OY_DEFAULT_IMAGE_PROFILE, name, MAX_PATH);

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
}

char*
_oyGetDefaultWorkspaceProfileName  ()
{ DBG_PROG_START
  char* name = (char*) calloc (MAX_PATH, sizeof(char));

  kdbGetValue (OY_DEFAULT_WORKSPACE_PROFILE, name, MAX_PATH);

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
}

char*
_oyGetDefaultCmykProfileName       ()
{ DBG_PROG_START
  char* name = (char*) calloc (MAX_PATH, sizeof(char));

  kdbGetValue (OY_DEFAULT_CMYK_PROFILE, name, MAX_PATH);

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
}


/* profile check API */

int
_oyCheckProfile (char* name)
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
    r = _oyCheckProfileMem (header, 128);
  DBG_NUM_S(("oyCheckProfileMem = %d",r))

  DBG_PROG_ENDE
  return r;
}

int
_oyCheckProfileMem (void* mem, size_t size)
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



/* --- internal API hidding --- */

#include "oyranos.h"

int
oyPathsCount         (void)
{ DBG_PROG_START
  int n = _oyPathsCount();
  DBG_PROG_ENDE
  return n;
}

char*
oyPathName           (int number)
{ DBG_PROG_START
  char* name = _oyPathName (number);
  DBG_PROG_ENDE
  return name;
}

int
oyPathAdd            (char* pathname)
{ DBG_PROG_START
  int n = _oyPathAdd (pathname);
  DBG_PROG_ENDE
  return n;
}

void
oyPathRemove         (char* pathname)
{ DBG_PROG_START
  _oyPathRemove (pathname);
  DBG_PROG_ENDE
}

void
oyPathSleep          (char* pathname)
{ DBG_PROG_START
  _oyPathSleep (pathname);
  DBG_PROG_ENDE
}

void
oyPathActivate       (char* pathname)
{ DBG_PROG_START
  _oyPathActivate (pathname);
  DBG_PROG_ENDE
}


int
oySetDefaultImageProfile          (char* name)
{ DBG_PROG_START
  int n =  _oySetDefaultImageProfile (name);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultImageProfileBlock     (char* name, void* mem, size_t size)
{ DBG_PROG_START
  int n = _oySetDefaultImageProfileBlock (name, mem, size);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultWorkspaceProfile      (char* name)
{ DBG_PROG_START
  int n = _oySetDefaultWorkspaceProfile (name);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultWorkspaceProfileBlock (char* name, void* mem, size_t size)
{ DBG_PROG_START
  int n = _oySetDefaultWorkspaceProfileBlock (name, mem, size);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultCmykProfile           (char* name)
{ DBG_PROG_START
  int n = _oySetDefaultCmykProfile (name);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultCmykProfileBlock      (char* name, void* mem, size_t size)
{ DBG_PROG_START
  int n = _oySetDefaultCmykProfileBlock (name, mem, size);
  DBG_PROG_ENDE
  return n;
}

char*
oyGetDefaultImageProfileName      ()
{ DBG_PROG_START
  char* name = _oyGetDefaultImageProfileName ();
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultWorkspaceProfileName  ()
{ DBG_PROG_START
  char* name = _oyGetDefaultWorkspaceProfileName ();
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultCmykProfileName       ()
{ DBG_PROG_START
  char* name = _oyGetDefaultCmykProfileName ();
  DBG_PROG_ENDE
  return name;
}


int
oyCheckProfile (char* name)
{ DBG_PROG_START
  int n = _oyCheckProfile (name);
  DBG_PROG_ENDE
  return n;
}

int
oyCheckProfileMem (void* mem, size_t size)
{ DBG_PROG_START
  int n = _oyCheckProfileMem (mem, size);
  DBG_PROG_ENDE
  return n;
}



