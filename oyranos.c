/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2005  Kai-Uwe Behrmann
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

/* Date:      25. 11. 2004 */

#include <kdb.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oyranos_helper.h"
#include "oyranos_definitions.h"
#include "oyranos_debug.h"

/* ---  Helpers  --- */
#define ERR if (rc) { printf("%s:%d\n", __FILE__,__LINE__); perror("Error"); }



/* --- internal API definition --- */

/* separate from the external functions */
int	oyPathsCount_         (void);
char*	oyPathName_           (int number);
int	oyPathAdd_            (const char* pathname);
void	oyPathRemove_         (const char* pathname);
void	oyPathSleep_          (const char* pathname);
void	oyPathActivate_       (const char* pathname);

int	oySetDefaultImageProfile_          (const char* name);
int	oySetDefaultImageProfile_Block     (const char* name, void* mem, int size);
int	oySetDefaultWorkspaceProfile_      (const char* name);
int	oySetDefaultWorkspaceProfile_Block (const char* name, void* mem, int size);
int	oySetDefaultXYZProfile_            (const char* name);
int	oySetDefaultXYZProfile_Block       (const char* name, void* mem, int size);
int	oySetDefaultLabProfile_            (const char* name);
int	oySetDefaultLabProfile_Block       (const char* name, void* mem, int size);
int	oySetDefaultRGBProfile_            (const char* name);
int	oySetDefaultRGBProfile_Block       (const char* name, void* mem, int size);
int	oySetDefaultCmykProfile_           (const char* name);
int	oySetDefaultCmykProfile_Block      (const char* name, void* mem, int size);

char*	oyGetDefaultImageProfileName_      ();
char*	oyGetDefaultWorkspaceProfileName_  ();
char*	oyGetDefaultXYZProfileName_        ();
char*	oyGetDefaultLabProfileName_        ();
char*	oyGetDefaultRGBProfileName_        ();
char*	oyGetDefaultCmykProfileName_       ();

int	oyCheckProfile_                    (const char* name);
int	oyCheckProfile_Mem                 (const void* mem, int size);

int	oyGetProfileSize_                  (const char* profilename);
void*	oyGetProfileBlock_                 (const char* profilename, int* size);

char*   oyGetDeviceProfile_               (const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3);
char**  oyGetDeviceProfile_s              (const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3,
                                           int** number);
int     oySetDeviceProfile_               (const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3,
                                           const char* profileName,
                                           void* mem,
                                           int size);



/* elektra key wrapper */
int oyAddKey_valueComment_ (const char* keyName,
                            const char* value, const char* comment);
int oyAddKey_value_ (const char* keyName, const char* value);

/* elektra key list handling */
char* oySearchEmptyKeyname_ (const char* keyParentName,
                             const char* keyBaseName);
KeySet* oyReturnChildrenList_ (const char* keyParentName, int* rc);

/* complete an name from file including oyResolveDirFileName */
char* oyMakeFullFileDirName_ (const char* name);
/* find an file/dir and do corrections on  ~ ; ../  */
char* oyResolveDirFileName_ (const char* name);
char* oyGetHomeDir_ ();
char* oyGetParent_ (const char* name);

int oyIsDir_ (const char* path);
int oyIsFile_ (const char* fileName);
int oyIsFileFull_ (const char* fullFileName);
int oyMakeDir_ (const char* path);

int   oyWriteMemToFile_(const char* name, void* mem, int size);
char* oyReadFileToMem_(const char* fullFileName, int *size);

/* oyranos part */
/* check for the global and the users directory */
void oyCheckDefaultDirectories_ ();
/* search in profile path and in current path */
char* oyFindProfile_ (const char* name);

/* Profile registring */
int oySetProfile_      (const char* name, const char* typ, const char* comment);
int oySetProfile_Block (const char* name, void* mem, int size, const char* typ, const char* comnt);

/* small search engine
 *
 * for one simple, single list, dont mix lists!!
 * name and val are not alloced or freed 
 */

struct Comp {
  struct Comp *next;
  struct Comp *begin;
  char* name;
  char* val;
  int   hits;
};

typedef struct Comp comp;

comp* initComp (comp *compare, comp *top);
comp* appendComp (comp *list, comp *new);
void destroyCompList (comp* list);

comp*   oyGetDeviceProfile_sList          (const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3,
                                           KeySet* profilesList,
                                           int   rc);


/* --- function definitions --- */

KeySet*
oyReturnChildrenList_ (const char* keyParentName, int* rc)
{ DBG_PROG_START
  KeySet *myKeySet = (KeySet*) malloc (sizeof(KeySet) * 1);
  ksInit(myKeySet);

  *rc = kdbGetChildKeys(keyParentName, myKeySet,KDB_O_RECURSIVE);

  DBG_PROG_ENDE
  return myKeySet;
}

char*
oySearchEmptyKeyname_ (const char* keyParentName, const char* keyBaseName)
{ DBG_PROG_START
  char* keyName = (char*)     calloc (strlen(keyParentName)
                                    + strlen(keyBaseName) + 24, sizeof(char));
  char* pathkeyName = (char*) calloc (strlen(keyBaseName) + 24, sizeof(char));
  int nth = 0, i = 1, rc;
  Key key;

  rc=keyInit(&key); ERR

    /* search for empty keyname */
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
oyAddKey_valueComment_ (const char* keyName,
                        const char* value,
                        const char* comment)
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
oyAddKey_value_ (const char* keyName, const char* value)
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
oyReadFileToMem_(const char* name, int *size)
{ DBG_PROG_START
  FILE *fp = 0;
  char* mem = 0;
  const char* filename = name;

  DBG_PROG

  {
    fp = fopen(filename, "r");
    DBG_PROG_S (("fp = %d filename = %s\n", (int)fp, filename))

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
      { DBG_PROG
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

  DBG_PROG_ENDE
  return mem;
}

int
oyWriteMemToFile_(const char* name, void* mem, int size)
{ DBG_PROG_START
  FILE *fp = 0;
  int   pt = 0;
  char* block = mem;
  const char* filename;
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
oyGetHomeDir_ ()
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
oyGetParent_ (const char* name)
{ DBG_PROG_START
  char *parentDir = (char*) calloc ( MAX_PATH, sizeof(char)), *ptr;

  sprintf (parentDir, name);
  ptr = strrchr( parentDir, OY_SLASH_C);
  if (ptr)
  {
    if (ptr[1] == 0) /* ending dir separator */
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
oyIsDir_ (const char* path)
{ DBG_PROG_START
  struct stat status;
  int r = 0;
  char* name = oyResolveDirFileName_ (path);
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
oyIsFileFull_ (const char* fullFileName)
{ DBG_PROG_START
  struct stat status;
  int r = 0;
  const char* name = fullFileName;

  DBG_NUM_S(("fullFileName = %s", fullFileName))
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
    if (!fp) { DBG_PROG
      r = 0;
    } else { DBG_PROG
      fclose (fp);
    }
  } DBG_PROG

  DBG_PROG_ENDE
  return r;
}

int
oyIsFile_ (const char* fileName)
{ DBG_PROG_START
  int r = 0;
  char* name = oyResolveDirFileName_ (fileName);

  r = oyIsFileFull_(name);

  if (name) free (name); DBG_PROG

  DBG_PROG_ENDE
  return r;
}

int
oyMakeDir_ (const char* path)
{ DBG_PROG_START
  char *name = oyResolveDirFileName_ (path);
  int rc = 0;
  mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; /* 0755 */
  DBG_PROG
  rc = mkdir (name, mode);
  free (name);

  DBG_PROG_ENDE
  return rc;
}

char*
oyResolveDirFileName_ (const char* name)
{ DBG_PROG_START
  char* newName = (char*) calloc (MAX_PATH, sizeof(char)),
       *home = 0;
  int len = 0;

  DBG_PROG_S((name))

  /* user directory */
  if (name[0] == '~')
  { DBG_PROG_S(("in home directory"))
    home = oyGetHomeDir_();
    len = strlen(name) + strlen(home) + 1;
    if (len >  FILENAME_MAX)
      WARN_S(("file name is too long %d\n", len))

    sprintf (newName, "%s%s", home, &name[0]+1);

  } else { DBG_PROG_S(("resolve  directory"))
    sprintf (newName, name);

    /* relative names - where the first sign is no directory separator */
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
oyMakeFullFileDirName_ (const char* name)
{ DBG_PROG_START
  char *newName;
  char *dirName = 0;

  DBG_PROG
  if(name &&
     strrchr( name, OY_SLASH_C ))
  { DBG_PROG
    /* substitute ~ with HOME variable from environment */
    newName = oyResolveDirFileName_ (name);
  } else
  { DBG_PROG
    /* create directory name */
    newName = (char*) calloc (MAX_PATH, sizeof(char)),
    dirName = (char*) getenv("PWD");
    sprintf (newName, "%s%s", dirName, OY_SLASH);
    if (name)
      sprintf (strrchr(newName,OY_SLASH_C)+1, "%s", name);
    DBG_PROG_S(("newName = %s", newName))
  }

  DBG_PROG_S(("newName = %s", newName))

  DBG_PROG_ENDE
  return newName;
}

void
oyCheckDefaultDirectories_ ()
{ DBG_PROG_START
  char* parentDefaultUserDir;

  /* test dirName : existing in path, default dirs are existing */
  if (!oyIsDir_ (OY_DEFAULT_SYSTEM_PROFILE__PATH))
  { DBG_PROG
    printf ("no default system directory %s\n",OY_DEFAULT_SYSTEM_PROFILE__PATH);
  }

  if (!oyIsDir_ (OY_DEFAULT_USER_PROFILE_PATH))
  { DBG_PROG 
    parentDefaultUserDir = oyGetParent_ (OY_DEFAULT_USER_PROFILE_PATH);

    if (!oyIsDir_ (parentDefaultUserDir))
    { DBG_PROG 
      printf ("Try to create part of users default directory %s %d \n",
                 parentDefaultUserDir,
      oyMakeDir_( parentDefaultUserDir)); DBG_PROG
    }
    free (parentDefaultUserDir);

    printf ("Try to create users default directory %s %d \n",
               OY_DEFAULT_USER_PROFILE_PATH,
    oyMakeDir_( OY_DEFAULT_USER_PROFILE_PATH )); DBG_PROG
  }
  DBG_PROG_ENDE
}

char*
oyFindProfile_ (const char* fileName)
{ DBG_PROG_START
  char  *fullFileName = 0;
  int    success = 0;
  char  *header = 0;
  int size;

  DBG_NUM_S((fileName))
  /* test for pure file without dir; search in configured paths only */
  if (fileName && !strchr(fileName, OY_SLASH_C))
  { DBG_PROG
    char* pathName = (char*) calloc (MAX_PATH, sizeof(char)); DBG_PROG
    int   n_paths = oyPathsCount_ (),
          i;

    DBG_PROG_S(("pure filename found"))
    fullFileName = (char*) calloc (MAX_PATH, sizeof(char));

    for (i = 0; i < n_paths; i++)
    { /* test profile */
      char* ptr = oyPathName_ (i);
      pathName = oyMakeFullFileDirName_ (ptr);
      sprintf (fullFileName, "%s%s%s", pathName, OY_SLASH, fileName);

      DBG_PROG_S((pathName))
      DBG_PROG_S((fullFileName))

      if (oyIsFileFull_(fullFileName))
      { DBG_PROG
        header = oyReadFileToMem_ (fullFileName, &size);
        success = !oyCheckProfile_Mem (header, 128);
      }

      if (ptr) free (ptr);
      if (header) free (header);

      if (success) /* found */
        break;
    }

    if (!success)
      WARN_S( ("profile %s not found in colour path\n", fileName))

    if (pathName) free (pathName);

  } else
  {/* else use fileName as an full qualified name, check name and test profile*/
    DBG_PROG_S(("dir/filename found"))
    fullFileName = oyMakeFullFileDirName_ (fileName);

    if (oyIsFileFull_(fullFileName))
    {
      header = oyReadFileToMem_ (fullFileName, &size);

      if (size >= 128)
        success = !oyCheckProfile_Mem (header, 128);
    }

    if (!success)
      WARN_S (("profile %s not found\n", fileName))

    if (header) free (header);
  }

  if (!success)
  { free (fullFileName);
    fullFileName = 0;
  }

  DBG_PROG_ENDE
  return fullFileName;
}

int
oySetProfile_      (const char* name, const char* typ, const char* comment)
{ DBG_PROG_START
  int r = 1;
  const char *fileName = 0, *com = comment;

  if (strrchr(name , OY_SLASH_C))
  {
    fileName = strrchr(name , OY_SLASH_C);
    fileName++;
  }
  else
    fileName = name;

  DBG_PROG_S(("name = %s typ %s", name, typ))

  if ( !oyCheckProfile_ (fileName) )
  { DBG_PROG_S(("set fileName = %s as %s profile\n",fileName, typ))
           if (strstr (typ , "Image"))
        r = oyAddKey_valueComment_ (OY_DEFAULT_IMAGE_PROFILE, fileName, com);
      else if (strstr (typ , "Workspace"))
        r = oyAddKey_valueComment_ (OY_DEFAULT_WORKSPACE_PROFILE, fileName, com);
      else if (strstr (typ , "XYZ"))
        r = oyAddKey_valueComment_ (OY_DEFAULT_XYZ_PROFILE, fileName, com);
      else if (strstr (typ , "Lab"))
        r = oyAddKey_valueComment_ (OY_DEFAULT_LAB_PROFILE, fileName, com);
      else if (strstr (typ , "RGB"))
        r = oyAddKey_valueComment_ (OY_DEFAULT_RGB_PROFILE, fileName, com);
      else if (strstr (typ , "Cmyk"))
        r = oyAddKey_valueComment_ (OY_DEFAULT_CMYK_PROFILE, fileName, com);
      else if (strstr (typ , "Device"))
      {
        int len = strlen(OY_USER OY_SLASH OY_REGISTRED_PROFILES)
                  + strlen(fileName);
        char* keyName = (char*) calloc (len +10, sizeof(char)); DBG_PROG
        sprintf (keyName, "%s%s%s%s", OY_USER, OY_SLASH, OY_REGISTRED_PROFILES OY_SLASH, fileName); DBG_PROG
        r = oyAddKey_valueComment_ (keyName, com, 0); DBG_PROG
        //DBG_PROG_V(("%s %d", keyName, len))
        if (keyName) free(keyName);
      }
      else
        printf ("%s:%d !!! ERROR typ %s type does not exist for default profiles",__FILE__,__LINE__, typ);
  }

  DBG_PROG_ENDE
  return r;
}


/* public API implementation */

/* path names API */


int
oyPathsCount_ ()
{ DBG_PROG_START
  int rc, n = 0;
  kdbOpen();

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_USER_PATHS, &rc ); ERR
  n = myKeySet->size;

  ksClose (myKeySet);
  kdbClose();

  DBG_PROG_ENDE
  return n;
}

char*
oyPathName_ (int number)
{ DBG_PROG_START
  int rc, n = 0;
  Key *current;
  char* value = (char*) calloc (sizeof(char), MAX_PATH);

  kdbOpen();

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_USER_PATHS, &rc ); ERR

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
oyPathAdd_ (const char* pfad)
{ DBG_PROG_START
  int rc, n = 0;
  Key *current;
  char* keyName = (char*) calloc (sizeof(char), MAX_PATH);
  char* value = (char*) calloc (sizeof(char), MAX_PATH);

  kdbOpen();

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_USER_PATHS, &rc ); ERR

  /* search for allready included path */
  for (current=myKeySet->start; current; current=current->next)
  {
    keyGetString(current, value, MAX_PATH);
    if (strcmp (value, pfad) == 0)
      n++;		
  }

  if (n) printf ("Key was allready %d times there\n",n);

  /* erase double occurencies of this path */
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

  /* create new key */
  if (!n)
  {
    /* search for empty keyname */
    keyName = oySearchEmptyKeyname_ (OY_USER_PATHS, OY_USER_PATH);

    /* write key */
    rc = oyAddKey_valueComment_ (keyName, pfad, "");
  }

  ksClose (myKeySet);
  kdbClose();
  free (keyName);
  free (value);

  oyCheckDefaultDirectories_();

  DBG_PROG_ENDE
  return rc;
}

void
oyPathRemove_ (const char* pfad)
{ DBG_PROG_START
  int rc;
  Key *current;
  char* value = (char*) calloc (sizeof(char), MAX_PATH);
  char* keyName = (char*) calloc (sizeof(char), MAX_PATH);

  kdbOpen();

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_USER_PATHS, &rc ); ERR

  /* compare and erase if matches */
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

  oyPathAdd_ (OY_DEFAULT_USER_PROFILE_PATH);

  DBG_PROG_ENDE
}

void
oyPathSleep_ (const char* pfad)
{ DBG_PROG_START
  int rc;
  Key *current;
  char* value = (char*) calloc (sizeof(char), MAX_PATH);

  kdbOpen();

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_USER_PATHS, &rc ); ERR

  /* set "SLEEP" in comment */
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
oyPathActivate_ (const char* pfad)
{ DBG_PROG_START
  int rc;
  Key *current;
  char* value = (char*) calloc (sizeof(char), MAX_PATH);

  kdbOpen();

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_USER_PATHS, &rc ); ERR

  /* erase "SLEEP" from comment */
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
oySetDefaultImageProfile_          (const char* name)
{ DBG_PROG_START
  int r = oySetProfile_ (name, "Image", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultWorkspaceProfile_      (const char* name)
{ DBG_PROG_START
  int r = oySetProfile_ (name, "Workspace", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultXYZProfile_           (const char* name)
{ DBG_PROG_START
  int r = oySetProfile_ (name, "XYZ", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultLabProfile_           (const char* name)
{ DBG_PROG_START
  int r = oySetProfile_ (name, "Lab", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultRGBProfile_           (const char* name)
{ DBG_PROG_START
  int r = oySetProfile_ (name, "RGB", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultCmykProfile_           (const char* name)
{ DBG_PROG_START
  int r = oySetProfile_ (name, "Cmyk", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultImageProfile_Block     (const char* name, void* mem, int size)
{ DBG_PROG_START
  int r = oySetProfile_Block (name, mem, size, "Image", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultWorkspaceProfile_Block (const char* name, void* mem, int size)
{ DBG_PROG_START
  int r = oySetProfile_Block (name, mem, size, "Workspace", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultXYZProfile_Block      (const char* name, void* mem, int size)
{ DBG_PROG_START
  int r = oySetProfile_Block (name, mem, size, "XYZ", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultLabProfile_Block      (const char* name, void* mem, int size)
{ DBG_PROG_START
  int r = oySetProfile_Block (name, mem, size, "Lab", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultRGBProfile_Block      (const char* name, void* mem, int size)
{ DBG_PROG_START
  int r = oySetProfile_Block (name, mem, size, "RGB", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultCmykProfile_Block      (const char* name, void* mem, int size)
{ DBG_PROG_START
  int r = oySetProfile_Block (name, mem, size, "Cmyk", 0);
  DBG_PROG_ENDE
  return r;
}

char*
oyGetDefaultImageProfileName_      ()
{ DBG_PROG_START
  char* name = (char*) calloc (MAX_PATH, sizeof(char));
  
  kdbGetValue (OY_DEFAULT_IMAGE_PROFILE, name, MAX_PATH);

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultWorkspaceProfileName_  ()
{ DBG_PROG_START
  char* name = (char*) calloc (MAX_PATH, sizeof(char));

  kdbGetValue (OY_DEFAULT_WORKSPACE_PROFILE, name, MAX_PATH);

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultXYZProfileName_       ()
{ DBG_PROG_START
  char* name = (char*) calloc (MAX_PATH, sizeof(char));

  kdbGetValue (OY_DEFAULT_XYZ_PROFILE, name, MAX_PATH);

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultLabProfileName_       ()
{ DBG_PROG_START
  char* name = (char*) calloc (MAX_PATH, sizeof(char));

  kdbGetValue (OY_DEFAULT_LAB_PROFILE, name, MAX_PATH);

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultRGBProfileName_       ()
{ DBG_PROG_START
  char* name = (char*) calloc (MAX_PATH, sizeof(char));

  kdbGetValue (OY_DEFAULT_RGB_PROFILE, name, MAX_PATH);

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultCmykProfileName_       ()
{ DBG_PROG_START
  char* name = (char*) calloc (MAX_PATH, sizeof(char));

  kdbGetValue (OY_DEFAULT_CMYK_PROFILE, name, MAX_PATH);

  DBG_PROG_S(("%s %s %d",OY_DEFAULT_CMYK_PROFILE,name,MAX_PATH))
  DBG_PROG_ENDE
  return name;
}


/* profile check API */

int
oyCheckProfile_                    (const char* name)
{ DBG_PROG_START
  char *fullName = 0;
  char* header; 
  int size = 0;
  int r = 1;

  DBG_NUM_S((name))
  fullName = oyFindProfile_(name);
  if (!fullName)
    WARN_S(("%s not found",name))
  DBG_NUM_S((fullName))

  /* do check */
  if (oyIsFileFull_(fullName))
  {
    header = oyReadFileToMem_ (fullName, &size); DBG_PROG
    if (size >= 128)
      r = oyCheckProfile_Mem (header, 128);
  }

  DBG_NUM_S(("oyCheckProfileMem = %d",r))

  DBG_PROG_ENDE
  return r;
}

int
oyCheckProfile_Mem                 (const void* mem, int size)
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
    return 0;
  } else
  {
    if (size >= 128)
      printf(" sign: %c%c%c%c ", (char)block[offset+0], (char)block[offset+1], (char)block[offset+2], (char)block[offset+3] );
    else
      WARN_S (("False profile - size = %d pos = %lu ", size, (long int)block))

    DBG_PROG_ENDE
    return 1;
  }
}

/* profile handling API */

int
oyGetProfileSize_                  (const char* profilename)
{ DBG_PROG_START
  int size = 0;
  char* fullFileName = oyFindProfile_ (profilename);
  char* dummy;

  dummy = oyReadFileToMem_ (fullFileName, &size);
  if (dummy) free (dummy);

  DBG_PROG_ENDE
  return size;
}

void*
oyGetProfileBlock_                 (const char* profilename, int* size)
{ DBG_PROG_START
  char* fullFileName = oyFindProfile_ (profilename);
  char* block = oyReadFileToMem_ (fullFileName, size);

  DBG_PROG_ENDE
  return block;
}

int
oySetProfile_Block (const char* name, void* mem, int size, const char* typ,
                    const char* comnt)
{ DBG_PROG_START
  int r = 0;
  char *fullFileName, *resolvedFN;
  const char *fileName;

  if (strrchr (name, OY_SLASH_C))
    fileName = strrchr (name, OY_SLASH_C);
  else
    fileName = name;

  fullFileName = (char*) calloc (sizeof(char),
                  strlen(OY_DEFAULT_USER_PROFILE_PATH) + strlen (fileName) + 4);

  sprintf (fullFileName, "%s%s%s",
           OY_DEFAULT_USER_PROFILE_PATH, OY_SLASH, fileName);

  resolvedFN = oyResolveDirFileName_ (fullFileName);
  free (fullFileName);
  fullFileName = resolvedFN;

  if (!oyCheckProfile_Mem( mem, size))
  {
    DBG_PROG_S((fullFileName))
    if ( oyIsFile_(fullFileName) )
      WARN_S (("file %s exist , please remove befor installing new profile\n", fullFileName))
    else
    { r = oyWriteMemToFile_ (fullFileName, mem, size);
      oySetProfile_ ( name, typ, comnt);
    }
  }

  DBG_PROG_S(("%s", name))
  DBG_PROG_S(("%s", fileName))
  DBG_PROG_S(("%ld %d", (long int)&((char*)mem)[0] , size))
  free (fullFileName);

  DBG_PROG_ENDE
  return r;
}

/* small search engine */

comp*
initComp (comp *compare, comp *top)
{ DBG_PROG_START
  if (!compare)
    compare = (comp*) calloc (1, sizeof(comp));

  compare->next = 0;

  if (top)
    compare->begin = top;
  else
    compare->begin = compare;
  compare->name = 0;
  compare->val = 0;
  compare->hits = 0;
  DBG_PROG_ENDE

  return compare;
}

comp*
appendComp (comp *list, comp *new)
{ DBG_PROG_START

  if (!list)
    list = initComp(list,0);

  list = list->begin;
  while (list->next)
    list = list->next;

  if (!new)
    new = initComp(new, list->begin);

  new->begin = list->begin;
  list->next = new;

  DBG_PROG_ENDE
  return new;
}

void
destroyCompList (comp *list)
{ DBG_PROG_START
  comp *before;

  list = list->begin;
  while (list->next)
  {
    before = list;
    list = list->next;
    free (before);
  }
  free (list);

  DBG_PROG_ENDE
}

char*
printComp (comp* entry)
{ DBG_PROG_START
  #ifdef DEBUG
  static char text[MAX_PATH] = {0};
  DBG_PROG_S(("%d", (int)entry))
  sprintf( text, "begin %d next %d\nname %s val %s hits %d\n",
           (int)entry->begin, (int)entry->next, entry->name, entry->val, entry->hits);

  DBG_PROG_ENDE
  return text;
  #else
  return 0;
  #endif
}


/* device profiles API */

char*
oyGetDeviceProfile_                (const char* manufacturer,
                                    const char* model,
                                    const char* product_id,
                                    const char* host,
                                    const char* port,
                                    const char* attrib1,
                                    const char* attrib2,
                                    const char* attrib3)
{ DBG_PROG_START
  char* profileName = 0;
  int rc;

  comp *matchList = 0,
       *testEntry = 0,
       *foundEntry = 0;
  KeySet* profilesList;

  kdbOpen();

  // TODO merge User and System KeySets in oyReturnChildrenList_
  profilesList = oyReturnChildrenList_(OY_USER OY_REGISTRED_PROFILES, &rc ); ERR

  matchList = oyGetDeviceProfile_sList (manufacturer, model, product_id,
                                        host, port, attrib1, attrib2, attrib3,
                                        profilesList, rc);

  /* 6. select the profile from the match list with the most hits */
  if (matchList)
  {
    int max_hits = 0;
    foundEntry = 0;
    for (testEntry=matchList->begin; testEntry; testEntry=testEntry->next)
    {
      if (testEntry->hits > max_hits)
      {
        foundEntry = testEntry;
        max_hits = testEntry->hits;
      }
    }
    DBG_PROG_S ((printComp (foundEntry)))

    /* 7. tell about the profile and its hits */
    {
      char *fileName = 0;

      if (strrchr(foundEntry->name , OY_SLASH_C))
      {
        fileName = strrchr(foundEntry->name , OY_SLASH_C);
        fileName++;
      }
      else
        fileName = foundEntry->name;

      profileName = (char*) calloc (strlen (fileName)+1, sizeof(char));
      sprintf (profileName, fileName);

      DBG_PROG_S((foundEntry->name))
      DBG_PROG_S((profileName))
      destroyCompList (matchList);
    }
  }

  ksClose (profilesList);
  kdbClose();

  DBG_PROG_ENDE
  return profileName;
}

char**
oyGetDeviceProfile_s               (const char* manufacturer,
                                    const char* model,
                                    const char* product_id,
                                    const char* host,
                                    const char* port,
                                    const char* attrib1,
                                    const char* attrib2,
                                    const char* attrib3,
                                    int** number)
{ DBG_PROG_START
  char** profileNames = 0;
  char*  profileName = 0;
  int    rc;

  comp *matchList = 0,
       *testEntry = 0,
       *foundEntry = 0;
  KeySet* profilesList;

  kdbOpen();

  // TODO merge User and System KeySets in oyReturnChildrenList_
  profilesList = oyReturnChildrenList_(OY_USER OY_REGISTRED_PROFILES, &rc ); ERR

  matchList = oyGetDeviceProfile_sList (manufacturer, model, product_id,
                                        host, port, attrib1, attrib2, attrib3,
                                        profilesList, rc);

  /* 6. select the profile from the match list with the most hits */
  if (matchList)
  {
    int max_hits = 0;
    foundEntry = 0;
    for (testEntry=matchList->begin; testEntry; testEntry=testEntry->next)
    {
      if (testEntry->hits > max_hits)
      {
        foundEntry = testEntry;
        max_hits = testEntry->hits;
      }
    }
    DBG_PROG_S ((printComp (foundEntry)))

    /* 7. tell about the profile and its hits */
    {
      char *fileName = 0;

      if (strrchr(foundEntry->name , OY_SLASH_C))
      {
        fileName = strrchr(foundEntry->name , OY_SLASH_C);
        fileName++;
      }
      else
        fileName = foundEntry->name;

      profileName = (char*) calloc (strlen (fileName)+1, sizeof(char));
      sprintf (profileName, fileName);

      DBG_PROG_S((foundEntry->name))
      DBG_PROG_S((profileName))
      destroyCompList (matchList);
    }
  }

  ksClose (profilesList);
  kdbClose();

  DBG_PROG_ENDE
  return profileNames;
}

comp*
oyGetDeviceProfile_sList           (const char* manufacturer,
                                    const char* model,
                                    const char* product_id,
                                    const char* host,
                                    const char* port,
                                    const char* attrib1,
                                    const char* attrib2,
                                    const char* attrib3,
                                    KeySet *profilesList,
                                    int   rc)
{ DBG_PROG_START
  /* Search description
   *
   * This routine describes the A approach
   *   - registred profiles with assigned devices
   *
   * 1. take all arguments and walk through the named devices list
   *    //  named devices consist of an key with the profile name + attributes
   *    //  it is not allowed to have two profiles with the same name
   *    //  it is allowed to have different profiles for the same attributes :(
   *    //  specify more attributes to make an decission presumable
   *    //   or maintain profiles, erasing older and invalid ones
   * 2. test if attributes matches the value of the key, count the hits
   * 3. search the profile in an match list
   * 4. add the profile to the match list if not found
   * 5. increase the hits counter in the macht list for that profile
   * 6. select the profile from the match list with the most hits
   * 7. tell about the profile and its hits
   *
   * approach B: TODO
   * no attributes are assigned beside certain keyword ("monitor", "scanner")
   * scan profile tags for manufacturer, device descriptions ...
   * - When to start an automatic registration run?
   * - include profile tag editing?
   * 
   * other things: TODO
   * - spread weighting? 3 degrees are sufficient How to merge in the on string 
   *   approach?
   */

  /* 1. take all arguments and walk through the named devices list */
  int i = 0, n = 0;
  char* value = (char*) calloc (MAX_PATH, sizeof(char));
  const char **attributs = (const char**) calloc (8, sizeof (const char*));
  Key *current;
  comp *matchList = 0,
       *testEntry = 0,
       *foundEntry = 0;

  attributs[0] = manufacturer;
  attributs[1] = model;
  attributs[2] = product_id;
  attributs[3] = host;
  attributs[4] = port;
  attributs[5] = attrib1;
  attributs[6] = attrib2;
  attributs[7] = attrib3;

  #if 1
  for (i = 0; i <= 7; ++i)
    DBG_PROG_S (("%ld %ld", (long int)attributs[i], (long int)model))
  #endif


  if (profilesList && !rc)
  {
    for (current=profilesList->start; current; current=current->next)
    {
      foundEntry=0;
      n = 0;
      keyGetString (current, value, MAX_PATH);

      /* 2. test if attributes matches the value of the key, count the hits */
      for (i = 0; i < 8; i++)
      {
        DBG_PROG_S (("%s %s",value, attributs[i]))
        if (value && attributs[i] &&
            (strstr(value, attributs[i]) != 0))
        { DBG_PROG
          n++;
        }
      }
      if (n)
      { /* 3. search the profile in an match list */
        int found = 0; DBG_PROG
        if (matchList)
        {
          for (testEntry=matchList->begin; testEntry; testEntry=testEntry->next)
          {
            DBG_PROG_S(( "%s", strstr(testEntry->name, current->key) ))
            if (strstr(testEntry->name, current->key) != 0)
            { DBG_PROG
              found = 1;
              WARN_S(("double occurency of profile %s", testEntry->name))
              /* anyway increase the hits counter if attributes fits better */
              if (testEntry->hits < n)
                testEntry->hits = n;
            }
          }
        }
        /* 4. add the profile to the match list if not found (normal case) */
        if (!found)
        {
          DBG_PROG_S(( "new matching profile found %s",current->key ))
          if (!matchList)
            matchList = initComp(0,0);
          foundEntry = appendComp (matchList, 0);
          DBG_PROG_S ((printComp (foundEntry)))
          foundEntry->name = current->key; 
          foundEntry->val = current->data;
          /* 5. increase the hits counter in the match list for that profile */
          foundEntry->hits = n; 
          DBG_PROG_S ((printComp (foundEntry)))
        }
      }
    }
  } else
    WARN_S (("No profiles yet registred to devices"))

  DBG_PROG_ENDE
  return matchList;
}

int
oySetDeviceProfile_                (const char* manufacturer,
                                    const char* model,
                                    const char* product_id,
                                    const char* host,
                                    const char* port,
                                    const char* attrib1,
                                    const char* attrib2,
                                    const char* attrib3,
                                    const char* profileName,
                                    void* mem,
                                    int size)
{ DBG_PROG_START
  int rc = 0;
  char* comment = 0;

  if (mem && size && profileName)
  {
    rc = oyCheckProfile_Mem (mem, size); ERR
  }

  if (!rc)
  { DBG_PROG
    if (manufacturer || model || product_id || host || port || attrib1
        || attrib2 || attrib3)
    { int len = 0;
      DBG_PROG
      if (manufacturer) len += strlen(manufacturer);
      if (model) len += strlen(model);
      if (product_id) len += strlen(product_id);
      if (host) len += strlen(host);
      if (port) len += strlen(port);
      if (attrib1) len += strlen(attrib1);
      if (attrib2) len += strlen(attrib2);
      if (attrib3) len += strlen(attrib3);
      comment = (char*) calloc (len+10, sizeof(char)); DBG_PROG
      if (manufacturer) sprintf (comment, "%s", manufacturer); DBG_PROG
      if (model) sprintf (&comment[strlen(comment)], "%s", model); DBG_PROG
      if (product_id) sprintf (&comment[strlen(comment)], "%s", product_id);
      if (host) sprintf (&comment[strlen(comment)], "%s", host);
      if (port) sprintf (&comment[strlen(comment)], "%s", port);
      if (attrib1) sprintf (&comment[strlen(comment)], "%s", attrib1);
      if (attrib2) sprintf (&comment[strlen(comment)], "%s", attrib2);
      if (attrib3) sprintf (&comment[strlen(comment)], "%s", attrib3);
    } DBG_PROG

    rc =  oySetProfile_ (profileName, "Device", comment); ERR
  }

  DBG_PROG_ENDE
  return rc;
}



/* --- internal API hidding --- */

#include "oyranos.h"

int
oyPathsCount         (void)
{ DBG_PROG_START
  int n = oyPathsCount_();
  DBG_PROG_ENDE
  return n;
}

char*
oyPathName           (int number)
{ DBG_PROG_START
  char* name = oyPathName_ (number);
  DBG_PROG_ENDE
  return name;
}

int
oyPathAdd            (const char* pathname)
{ DBG_PROG_START
  int n = oyPathAdd_ (pathname);
  DBG_PROG_ENDE
  return n;
}

void
oyPathRemove         (const char* pathname)
{ DBG_PROG_START
  oyPathRemove_ (pathname);
  DBG_PROG_ENDE
}

void
oyPathSleep          (const char* pathname)
{ DBG_PROG_START
  oyPathSleep_ (pathname);
  DBG_PROG_ENDE
}

void
oyPathActivate       (const char* pathname)
{ DBG_PROG_START
  oyPathActivate_ (pathname);
  DBG_PROG_ENDE
}


int
oySetDefaultImageProfile          (const char* name)
{ DBG_PROG_START
  int n =  oySetDefaultImageProfile_ (name);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultImageProfileBlock     (const char* name, void* mem, int size)
{ DBG_PROG_START
  int n = oySetDefaultImageProfile_Block (name, mem, size);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultWorkspaceProfile      (const char* name)
{ DBG_PROG_START
  int n = oySetDefaultWorkspaceProfile_ (name);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultWorkspaceProfileBlock (const char* name, void* mem, int size)
{ DBG_PROG_START
  int n = oySetDefaultWorkspaceProfile_Block (name, mem, size);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultXYZProfile           (const char* name)
{ DBG_PROG_START
  int n = oySetDefaultXYZProfile_ (name);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultXYZProfileBlock      (const char* name, void* mem, int size)
{ DBG_PROG_START
  int n = oySetDefaultXYZProfile_Block (name, mem, size);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultLabProfile           (const char* name)
{ DBG_PROG_START
  int n = oySetDefaultLabProfile_ (name);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultLabProfileBlock      (const char* name, void* mem, int size)
{ DBG_PROG_START
  int n = oySetDefaultLabProfile_Block (name, mem, size);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultRGBProfile           (const char* name)
{ DBG_PROG_START
  int n = oySetDefaultRGBProfile_ (name);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultRGBProfileBlock      (const char* name, void* mem, int size)
{ DBG_PROG_START
  int n = oySetDefaultRGBProfile_Block (name, mem, size);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultCmykProfile           (const char* name)
{ DBG_PROG_START
  int n = oySetDefaultCmykProfile_ (name);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultCmykProfileBlock      (const char* name, void* mem, int size)
{ DBG_PROG_START
  int n = oySetDefaultCmykProfile_Block (name, mem, size);
  DBG_PROG_ENDE
  return n;
}

char*
oyGetDefaultImageProfileName      ()
{ DBG_PROG_START
  char* name = oyGetDefaultImageProfileName_ ();
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultWorkspaceProfileName  ()
{ DBG_PROG_START
  char* name = oyGetDefaultWorkspaceProfileName_ ();
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultXYZProfileName       ()
{ DBG_PROG_START
  char* name = oyGetDefaultXYZProfileName_ ();
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultLabProfileName       ()
{ DBG_PROG_START
  char* name = oyGetDefaultLabProfileName_ ();
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultRGBProfileName       ()
{ DBG_PROG_START
  char* name = oyGetDefaultRGBProfileName_ ();
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultCmykProfileName       ()
{ DBG_PROG_START
  char* name = oyGetDefaultCmykProfileName_ ();
  DBG_PROG_ENDE
  return name;
}


int
oyCheckProfile (const char* name, int flag)
{ DBG_PROG_START
  /* flag is currently ignored */
  int n = oyCheckProfile_ (name);
  DBG_PROG_ENDE
  return n;
}

int
oyCheckProfileMem (const void* mem, int size,int flag)
{ DBG_PROG_START
  /* flag is currently ignored */
  int n = oyCheckProfile_Mem (mem, size);
  DBG_PROG_ENDE
  return n;
}

int
oyGetProfileSize                  (const char* profilename)
{ DBG_PROG_START
  int size = oyGetProfileSize_ (profilename);
  DBG_PROG_ENDE
  return size;
}

void*
oyGetProfileBlock                 (const char* profilename, int *size)
{ DBG_PROG_START
  char* block = oyGetProfileBlock_ (profilename, size);
  DBG_PROG_S( ("%s %d %d", profilename, (int)block, *size) )
  DBG_PROG

  DBG_PROG_ENDE
  return block;
}

char*
oyGetDeviceProfile                (DEVICETYP typ,
                                   const char* manufacturer,
                                   const char* model,
                                   const char* product_id,
                                   const char* host,
                                   const char* port,
                                   const char* attrib1,
                                   const char* attrib2,
                                   const char* attrib3)
{ DBG_PROG_START
  char* profile_name = oyGetDeviceProfile_ (manufacturer, model, product_id,
                                    host, port, attrib1, attrib2, attrib3);
  DBG_PROG_S( (profile_name) )

  DBG_PROG_ENDE
  return profile_name;
}

int
oySetDeviceProfile                (DEVICETYP typ,
                                   const char* manufacturer,
                                   const char* model,
                                   const char* product_id,
                                   const char* host,
                                   const char* port,
                                   const char* attrib1,
                                   const char* attrib2,
                                   const char* attrib3,
                                   const char* profileName,
                                   void* mem,
                                   int size)
{ DBG_PROG_START
  int rc =     oySetDeviceProfile_ (manufacturer, model, product_id,
                                    host, port, attrib1, attrib2, attrib3,
                                    profileName, mem, size);
  DBG_PROG_ENDE
  return rc;
}



