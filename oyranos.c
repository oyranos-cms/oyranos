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
#include <dirent.h>
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
void  oyOpen_                   (void);
void  oyClose_                  (void);
int   oyPathsCount_             (void);
char* oyPathName_               (int number);
int   oyPathAdd_                (const char* pathname);
void  oyPathRemove_             (const char* pathname);
void  oyPathSleep_              (const char* pathname);
void  oyPathActivate_           (const char* pathname);
char* oyGetPathFromProfileName_ (const char* profilename);

int	oySetDefaultWorkspaceProfile_      (const char* name);
int	oySetDefaultWorkspaceProfile_Block (const char* name, void* mem, size_t size);
int	oySetDefaultXYZInputProfile_       (const char* name);
int	oySetDefaultXYZInputProfile_Block  (const char* name, void* mem, size_t size);
int	oySetDefaultLabInputProfile_       (const char* name);
int	oySetDefaultLabInputProfile_Block  (const char* name, void* mem, size_t size);
int	oySetDefaultRGBInputProfile_       (const char* name);
int	oySetDefaultRGBInputProfile_Block  (const char* name, void* mem, size_t size);
int	oySetDefaultCmykInputProfile_      (const char* name);
int	oySetDefaultCmykInputProfile_Block (const char* name, void* mem, size_t size);

char*	oyGetDefaultWorkspaceProfileName_  ();
char*	oyGetDefaultXYZInputProfileName_        ();
char*	oyGetDefaultLabInputProfileName_        ();
char*	oyGetDefaultRGBInputProfileName_        ();
char*	oyGetDefaultCmykInputProfileName_       ();

char**  oyProfileList_                 (const char* colourspace, int * size);

int	oyCheckProfile_                    (const char* name);
int	oyCheckProfile_Mem                 (const void* mem, size_t size);

size_t	oyGetProfileSize_                  (const char* profilename);
void*	oyGetProfileBlock_                 (const char* profilename, size_t *size);

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
                                           size_t size);
int     oyEraseDeviceProfile_             (const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3);

void oyOpen  (void) { oyOpen_(); }
void oyClose (void) { oyClose_(); }
void oyOpen_ (void) { kdbOpen(); }
void oyClose_(void) { kdbClose(); }


/* elektra key wrappers */
int     oyAddKey_valueComment_ (const char* keyName,
                                const char* value, const char* comment);
int     oyAddKey_value_        (const char* keyName, const char* value);

/* elektra key list handling */
char*   oySearchEmptyKeyname_  (const char* keyParentName,
                                const char* keyBaseName);
KeySet* oyReturnChildrenList_  (const char* keyParentName, int* rc);

/* complete an name from file including oyResolveDirFileName */
char*   oyMakeFullFileDirName_     (const char* name);
/* find an file/dir and do corrections on  ~ ; ../  */
char*   oyResolveDirFileName_      (const char* name);
char*   oyExtractPathFromFileName_ (const char* name);
char*   oyGetHomeDir_              ();
char*   oyGetParent_               (const char* name);

int oyIsDir_      (const char* path);
int oyIsFile_     (const char* fileName);
int oyIsFileFull_ (const char* fullFileName);
int oyMakeDir_    (const char* path);

int   oyWriteMemToFile_ (const char* name, void* mem, size_t size);
char* oyReadFileToMem_  (const char* fullFileName, size_t *size);

/* oyranos part */
/* check for the global and the users directory */
void oyCheckDefaultDirectories_ ();
/* search in profile path and in current path */
char* oyFindProfile_ (const char* name);

/* Profile registring */
int oySetProfile_      (const char* name, const char* typ, const char* comment);
int oySetProfile_Block (const char* name, void* mem, size_t size,
                        const char* typ, const char* comnt);

/* small search engine
 *
 * for one simple, single list, dont mix lists!!
 * name and val are not alloced or freed 
 */

struct OyComp {
  struct OyComp *next;
  struct OyComp *begin;
  char          *name;
  char          *val;
  int            hits;
};

typedef struct OyComp oyComp;

oyComp* oyInitComp_        (oyComp *compare, oyComp *top);
oyComp* oyAppendComp_      (oyComp *list, oyComp *new);
void    oyDestroyCompList_ (oyComp* list);

oyComp* oyGetDeviceProfile_sList          (const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3,
                                           KeySet* profilesList,
                                           int   rc);

/* small helpers */
#define OY_FREE( ptr ) if(ptr) { free(ptr); ptr = 0; }

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

  if(keyParentName)
    DBG_PROG_S((keyParentName))
  if(keyBaseName)
    DBG_PROG_S((keyBaseName))

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

  if(keyName)
    DBG_PROG_S((keyName))

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

  if (keyName)
    DBG_PROG_S(( keyName ));
  if (value)
    DBG_PROG_S(( value ));
  if (comment)
    DBG_PROG_S(( comment ));

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

size_t
oyReadFileSize_(const char* name)
{ DBG_PROG_START
  FILE *fp = 0;
  const char* filename = name;
  size_t size = 0;

  {
    fp = fopen(filename, "r");
    DBG_PROG_S (("fp = %d filename = %s\n", (int)fp, filename))

    if (fp)
    {
      /* get size */
      fseek(fp,0L,SEEK_END); 
      size = ftell (fp);
      fclose (fp);

    } else
      printf ("could not read %s\n", filename);
  }

  DBG_PROG_ENDE
  return size;
}

char*
oyReadFileToMem_(const char* name, size_t *size)
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
      /* read file possibly partitial */
      if(!*size || *size > ftell(fp))
        *size = ftell (fp);
      rewind(fp);

      if(oy_debug)
        printf("%u\n",((size_t)size));

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
          OY_FREE (mem)
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
oyWriteMemToFile_(const char* name, void* mem, size_t size)
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
  #if (__WINDOWS__)
  DBG_PROG_ENDE
  return "OS not supported yet";
  #else
  char* name = (char*) getenv("HOME");
  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
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
  DBG_PROG_S(("status.st_mode = %d", (int)(status.st_mode&S_IFMT)&S_IFDIR))
  DBG_PROG_S(("status.st_mode = %d", (int)status.st_mode))
  DBG_PROG_S(("name = %s ", name))
  OY_FREE (name)
  r = !r &&
       ((status.st_mode & S_IFMT) & S_IFDIR);

  DBG_PROG_ENDE
  return r;
}

#include <errno.h>

int
oyIsFileFull_ (const char* fullFileName)
{ DBG_PROG_START
  struct stat status;
  int r = 0;
  const char* name = fullFileName;

  DBG_NUM_S(("fullFileName = \"%s\"", fullFileName))
  status.st_mode = 0;
  r = stat (name, &status);

  DBG_NUM_S(("status.st_mode = %d", (int)(status.st_mode&S_IFMT)&S_IFDIR))
  DBG_NUM_S(("status.st_mode = %d", (int)status.st_mode))
  DBG_NUM_S(("name = %s", name))
  DBG_NUM_V( r )
  switch (r)
  {
    case EACCES:       WARN_S(("EACCES = %d\n",r)); break;
    case EIO:          WARN_S(("EIO = %d\n",r)); break;
    case ELOOP:        WARN_S(("ELOOP = %d\n",r)); break;
    case ENAMETOOLONG: WARN_S(("ENAMETOOLONG = %d\n",r)); break;
    case ENOENT:       WARN_S(("ENOENT = %d\n",r)); break;
    case ENOTDIR:      WARN_S(("ENOTDIR = %d\n",r)); break;
    case EOVERFLOW:    WARN_S(("EOVERFLOW = %d\n",r)); break;
  }

  r = !r &&
       (   ((status.st_mode & S_IFMT) & S_IFREG)
        || ((status.st_mode & S_IFMT) & S_IFLNK));

  DBG_NUM_V( r )
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

  OY_FREE (name) DBG_PROG

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
  OY_FREE (name)

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

  if(name)
    DBG_PROG_S (("name %s", name));
  if(home)
    DBG_PROG_S (("home %s", home));
  if(newName)
    DBG_PROG_S (("newName = %s", newName));

  DBG_PROG_ENDE
  return newName;
}

char*
oyExtractPathFromFileName_ (const char* file_name)
{ DBG_PROG_START
  char *path_name = (char*) calloc (strlen(file_name)+1, sizeof(char));
  sprintf( path_name, file_name );
  DBG_PROG_S (("path_name = %s", path_name))
  char *ptr = strrchr (path_name, '/');
  ptr[0] = 0;
  DBG_PROG_S (("path_name = %s", path_name))
  DBG_PROG_S (("ptr = %s", ptr))
  DBG_PROG_ENDE
  return path_name;
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
  if (!oyIsDir_ (OY_DEFAULT_SYSTEM_PROFILE_PATH))
  { DBG_PROG
    printf ("no default system directory %s\n",OY_DEFAULT_SYSTEM_PROFILE_PATH);
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
    OY_FREE (parentDefaultUserDir)

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

  //DBG_NUM_S((fileName))
  if (fileName && !strchr(fileName, OY_SLASH_C))
  { DBG_PROG
    char* path_name = oyGetPathFromProfileName_(fileName);
    fullFileName = (char*) calloc (MAX_PATH, sizeof(char));
    sprintf(fullFileName, "%s%s%s", path_name, OY_SLASH, fileName);
    OY_FREE(path_name)
  } else
  {
    if (oyIsFileFull_(fileName)) {
      fullFileName = (char*) calloc (MAX_PATH, sizeof(char));
      sprintf(fullFileName, fileName);
    } else
      fullFileName = oyMakeFullFileDirName_ (fileName);
  }

  DBG_PROG_ENDE
  return fullFileName;
}

char*
oyGetPathFromProfileName_ (const char* fileName)
{ DBG_PROG_START
  char  *fullFileName = 0;
  char  *pathName = 0;
  int    success = 0;
  char  *header = 0;
  int    size;

  //DBG_NUM_S((fileName))
  /* test for pure file without dir; search in configured paths only */
  if (fileName && !strchr(fileName, OY_SLASH_C))
  { DBG_PROG
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
      //DBG_PROG_S((fullFileName))

      if (oyIsFileFull_(fullFileName))
      { DBG_PROG
        size = 128;
        header = oyReadFileToMem_ (fullFileName, &size);
        success = !oyCheckProfile_Mem (header, size);
      }

      OY_FREE (ptr)
      OY_FREE (header)

      if (success) { /* found */
        OY_FREE (fullFileName)
       DBG_PROG_ENDE
       return pathName;
      } else
        OY_FREE (pathName)
    }

    if (!success) {
      WARN_S( ("profile %s not found in colour path\n", fileName))
      DBG_PROG_ENDE
      return 0;
    }

  } else
  {/* else use fileName as an full qualified name, check name and test profile*/
    DBG_PROG_S(("dir/filename found"))
    fullFileName = oyMakeFullFileDirName_ (fileName);

    if (oyIsFileFull_(fullFileName))
    {
      size = 128;
      header = oyReadFileToMem_ (fullFileName, &size);

      if (size >= 128)
        success = !oyCheckProfile_Mem (header, 128);
    }

    if (!success) {
      WARN_S (("profile %s not found\n", fileName))
      DBG_PROG_ENDE
      return 0;
    }

    pathName = oyExtractPathFromFileName_(fullFileName);

    OY_FREE (header)
  }

  if (!success)
  { OY_FREE (pathName)
    pathName = 0;
  }

  OY_FREE (fullFileName)

  DBG_PROG_ENDE
  return pathName;
}

int
oySetProfile_      (const char* name, const char* typ, const char* comment)
{ DBG_PROG_START
  int r = 1;
  const char *fileName = 0, *com = comment;

  /* extract filename */
  if (name && strrchr(name , OY_SLASH_C))
  {
    fileName = strrchr(name , OY_SLASH_C);
    fileName++;
  } else
    fileName = name;

  DBG_PROG_S(("name = %s typ %s", name, typ))

  if ( name == 0 || !oyCheckProfile_ (fileName) )
  {
    const char* typ_name = 0;
    DBG_PROG_S(("set fileName = %s as %s profile\n",fileName, typ))
           if (strstr (typ , "Workspace"))
        typ_name = OY_DEFAULT_WORKSPACE_PROFILE;
      else if (strstr (typ , "XYZ"))
        typ_name = OY_DEFAULT_XYZ_INPUT_PROFILE;
      else if (strstr (typ , "Lab"))
        typ_name = OY_DEFAULT_LAB_INPUT_PROFILE;
      else if (strstr (typ , "RGB"))
        typ_name = OY_DEFAULT_RGB_INPUT_PROFILE;
      else if (strstr (typ , "Cmyk"))
        typ_name = OY_DEFAULT_CMYK_INPUT_PROFILE;
      else if (strstr (typ , "Device"))
      {
        int len = strlen(OY_USER OY_SLASH OY_REGISTRED_PROFILES)
                  + strlen(fileName);
        char* keyName = (char*) calloc (len +10, sizeof(char)); DBG_PROG
        sprintf (keyName, "%s%s%s%s", OY_USER, OY_SLASH, OY_REGISTRED_PROFILES OY_SLASH, fileName); DBG_PROG
        r = oyAddKey_valueComment_ (keyName, com, 0); DBG_PROG
        //DBG_PROG_V(("%s %d", keyName, len))
        OY_FREE (keyName)
      }
      else
        printf ("%s:%d !!! ERROR typ %s type does not exist for default profiles",__FILE__,__LINE__, typ);

    if(typ_name)
    {
      if(name)
        r = oyAddKey_valueComment_ (typ_name, fileName, com);
      else
      {
        KeySet* list;
        Key *current;
        char* value = (char*) calloc (sizeof(char), MAX_PATH);
        int rc = 0;

        DBG_PROG

        kdbOpen();


        // TODO merge User and System KeySets in oyReturnChildrenList_
        list = oyReturnChildrenList_(OY_USER OY_KEY OY_SLASH "default", &rc ); ERR
        for (current=list->start; current; current=current->next)
        {
          keyGetName(current, value, MAX_PATH);
          DBG_NUM_S(( value ))
          if(strstr(value, typ_name) != 0 && strlen(value) == strlen(typ_name))
          {
            DBG_PROG_S((value))
            kdbRemove (value); 
            break;
          }
        }

        DBG_NUM_S(( value ))

        OY_FREE (list) DBG_PROG
        OY_FREE (value) DBG_PROG
        kdbClose(); DBG_PROG
        r = rc;
      }
    }
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
  if(!rc)
    n = myKeySet->size;
  if(n < 2)
    oyPathAdd_(OY_DEFAULT_USER_PROFILE_PATH);

  ksClose (myKeySet);
  kdbClose();
  OY_FREE(myKeySet)

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
  OY_FREE(myKeySet)

  DBG_PROG_ENDE
  return value;
}

int
oyPathAdd_ (const char* pfad)
{
  DBG_PROG_START
  int rc, n = 0;
  Key *current;
  char* keyName = (char*) calloc (sizeof(char), MAX_PATH);
  char* value = (char*) calloc (sizeof(char), MAX_PATH);
  int has_local_path = 0, has_global_path = 0;

  /* are we setting a default path? */
  if (strcmp (pfad, OY_DEFAULT_USER_PROFILE_PATH) == 0)
    has_local_path = 1;
  if (strcmp (pfad, OY_DEFAULT_SYSTEM_PROFILE_PATH) == 0)
    has_global_path = 1;

  if(pfad)
    DBG_PROG_S(( pfad ));

  kdbOpen();

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_USER_PATHS, &rc ); ERR

  /* search for allready included path */
  for (current=myKeySet->start; current; current=current->next)
  {
    keyGetString(current, value, MAX_PATH);
    if(value)
      DBG_PROG_S(( value ));
    if (strcmp (value, pfad) == 0)
      ++n;		

    /* Are the default paths allready there? */
    if (strcmp (value, OY_DEFAULT_USER_PROFILE_PATH) == 0)
      has_local_path = 1;
    if (strcmp (value, OY_DEFAULT_SYSTEM_PROFILE_PATH) == 0)
      has_global_path = 1;
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
  } else if (!n) {
  /* add path */
    /* search for empty keyname */
    keyName = oySearchEmptyKeyname_ (OY_USER_PATHS, OY_USER_PATH);

    /* write key */
    rc = oyAddKey_valueComment_ (keyName, pfad, "");
  }

  if (!has_global_path)
  {
    keyName = oySearchEmptyKeyname_ (OY_USER_PATHS, OY_USER_PATH);
    rc = oyAddKey_valueComment_ (keyName, OY_DEFAULT_SYSTEM_PROFILE_PATH, "");
  }
  if (!has_local_path)
  {
    keyName = oySearchEmptyKeyname_ (OY_USER_PATHS, OY_USER_PATH);
    rc = oyAddKey_valueComment_ (keyName, OY_DEFAULT_USER_PROFILE_PATH, "");
  }

  ksClose (myKeySet);
  kdbClose();
  OY_FREE (myKeySet)
  OY_FREE (keyName)
  OY_FREE (value)

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

  /* after remove blindly add seeing */
  oyPathAdd_ (OY_DEFAULT_USER_PROFILE_PATH);

  kdbClose();
  OY_FREE(myKeySet)
  OY_FREE(keyName)
  OY_FREE(value)


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
  OY_FREE(myKeySet)
  OY_FREE (value);
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
  OY_FREE(myKeySet)
  OY_FREE (value);
  DBG_PROG_ENDE
}

/* default profiles API */

int
oySetDefaultWorkspaceProfile_      (const char* name)
{ DBG_PROG_START
  int r = oySetProfile_ (name, "Workspace", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultXYZInputProfile_           (const char* name)
{ DBG_PROG_START
  int r = oySetProfile_ (name, "XYZ", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultLabInputProfile_           (const char* name)
{ DBG_PROG_START
  int r = oySetProfile_ (name, "Lab", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultRGBInputProfile_           (const char* name)
{ DBG_PROG_START
  int r = oySetProfile_ (name, "RGB", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultCmykInputProfile_           (const char* name)
{ DBG_PROG_START
  int r = oySetProfile_ (name, "Cmyk", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultWorkspaceProfile_Block (const char* name, void* mem, size_t size)
{ DBG_PROG_START
  int r = oySetProfile_Block (name, mem, size, "Workspace", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultXYZInputProfile_Block      (const char* name, void* mem, size_t size)
{ DBG_PROG_START
  int r = oySetProfile_Block (name, mem, size, "XYZ", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultLabInputProfile_Block      (const char* name, void* mem, size_t size)
{ DBG_PROG_START
  int r = oySetProfile_Block (name, mem, size, "Lab", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultRGBInputProfile_Block      (const char* name, void* mem, size_t size)
{ DBG_PROG_START
  int r = oySetProfile_Block (name, mem, size, "RGB", 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultCmykInputProfile_Block      (const char* name, void* mem, size_t size)
{ DBG_PROG_START
  int r = oySetProfile_Block (name, mem, size, "Cmyk", 0);
  DBG_PROG_ENDE
  return r;
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
oyGetDefaultXYZInputProfileName_       ()
{ DBG_PROG_START
  char* name = (char*) calloc (MAX_PATH, sizeof(char));

  kdbGetValue (OY_DEFAULT_XYZ_INPUT_PROFILE, name, MAX_PATH);

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultLabInputProfileName_       ()
{ DBG_PROG_START
  char* name = (char*) calloc (MAX_PATH, sizeof(char));

  kdbGetValue (OY_DEFAULT_LAB_INPUT_PROFILE, name, MAX_PATH);

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultRGBInputProfileName_       ()
{ DBG_PROG_START
  char* name = (char*) calloc (MAX_PATH, sizeof(char));

  kdbGetValue (OY_DEFAULT_RGB_INPUT_PROFILE, name, MAX_PATH);

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultCmykInputProfileName_       ()
{ DBG_PROG_START
  char* name = (char*) calloc (MAX_PATH, sizeof(char));

  kdbGetValue (OY_DEFAULT_CMYK_INPUT_PROFILE, name, MAX_PATH);

  DBG_PROG_S(("%s %s %d",OY_DEFAULT_CMYK_INPUT_PROFILE,name,MAX_PATH))
  DBG_PROG_ENDE
  return name;
}

/* profile lists API */

char**
oyProfileList_                     (const char* colourspace, int * size)
{
  DBG_PROG_START
  char** names = 0;
  int count = oyPathsCount_();
  static const int hopp = 128;
  int i, mem_count = hopp;
  int count_files = 0;

  names = (char**) calloc (sizeof(char*), mem_count);

  for(i = 0; i < count ; ++i)
  {
    struct stat statbuf;
    DIR *dir;
    struct dirent *entry;
    char *p = oyPathName_(i);
    char *path = oyMakeFullFileDirName_(p);

    if ((stat (path, &statbuf)) != 0) {
      WARN_S(("%d. path %s does not exist", i, path))
      continue;
    }
    if (!S_ISDIR (statbuf.st_mode)) {
      WARN_S(("%d. path %s is not a directory", i, path));
      continue;
    }
    dir = opendir (path);
    if (!dir) {
      WARN_S(("%d. path %s is not readable", i, path));
      continue;
    }

    while ((entry = readdir (dir)))
    {
      char *file_name = entry->d_name;
      if ((strcmp (file_name, "..") == 0) || (strcmp (file_name, ".") == 0))
        continue;

      if (!oyCheckProfile_(file_name))
      {
        if(count_files >= mem_count)
        {
          char** temp = names;
          names = (char**) calloc (sizeof(char*), mem_count+hopp);
          memcpy(names, temp, sizeof(char*) * mem_count);
          mem_count += hopp;
        }

        if(!colourspace) {
          names[count_files] = (char*) calloc (sizeof(char)*2, strlen(file_name));
          strcpy(names[count_files], file_name);
          ++count_files;
        }
      } else
        WARN_S(("%s in %s is not a valid profile", file_name, path));
    }
    closedir(dir);

  }

  *size = count_files;
  DBG_PROG_ENDE
  return names;
}

/* profile check API */

int
oyCheckProfile_                    (const char* name)
{ DBG_PROG_START
  char *fullName = 0;
  char* header = 0; 
  size_t size = 0;
  int r = 1;

  //if(name) DBG_NUM_S((name));
  fullName = oyFindProfile_(name);
  if (!fullName)
    WARN_S(("%s not found",name))
  else
    ;//DBG_NUM_S((fullName));

  /* do check */
  if (oyIsFileFull_(fullName))
  {
    size = 128;
    header = oyReadFileToMem_ (fullName, &size); DBG_PROG
    if (size >= 128)
      r = oyCheckProfile_Mem (header, 128);
  }

  /* release memory */
  if(header && size)
    free(header);

  DBG_NUM_S(("oyCheckProfileMem = %d",r))

  DBG_PROG_ENDE
  return r;
}

int
oyCheckProfile_Mem                 (const void* mem, size_t size)
{ DBG_PROG_START
  char* block = (char*) mem;
  int offset = 36;
  if (size >= 128) 
  {
    if (block[offset+0] == 'a' &&
        block[offset+1] == 'c' &&
        block[offset+2] == 's' &&
        block[offset+3] == 'p' )
    {
      DBG_PROG_ENDE
      return 0;
    } else {
      WARN_S((" sign: %c%c%c%c ", (char)block[offset+0], (char)block[offset+1], (char)block[offset+2], (char)block[offset+3] ))
      DBG_PROG_ENDE
      return 1;
    }
  } else {
    WARN_S (("False profile - size = %d pos = %lu ", size, (long int)block))

    DBG_PROG_ENDE
    return 1;
  }
}

/* profile handling API */

size_t
oyGetProfileSize_                  (const char* profilename)
{ DBG_PROG_START
  size_t size = 0;
  char* fullFileName = oyFindProfile_ (profilename);

  size = oyReadFileSize_ (fullFileName);

  DBG_PROG_ENDE
  return size;
}

void*
oyGetProfileBlock_                 (const char* profilename, size_t *size)
{ DBG_PROG_START
  char* fullFileName = oyFindProfile_ (profilename);
  char* block = oyReadFileToMem_ (fullFileName, size);

  DBG_PROG_ENDE
  return block;
}

int
oySetProfile_Block (const char* name, void* mem, size_t size, const char* typ,
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
  OY_FREE(fullFileName)
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
  OY_FREE(fullFileName)

  DBG_PROG_ENDE
  return r;
}

/* small search engine */

oyComp*
oyInitComp_ (oyComp *compare, oyComp *top)
{ DBG_PROG_START
  if (!compare)
    compare = (oyComp*) calloc (1, sizeof(oyComp));

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

oyComp*
oyAppendComp_ (oyComp *list, oyComp *new)
{ DBG_PROG_START

  if (!list)
    list = oyInitComp_(list,0);

  list = list->begin;
  while (list->next)
    list = list->next;

  if (!new)
    new = oyInitComp_(new, list->begin);

  new->begin = list->begin;
  list->next = new;

  DBG_PROG_ENDE
  return new;
}

void
oyDestroyCompList_ (oyComp *list)
{ DBG_PROG_START
  oyComp *before;

  list = list->begin;
  while (list->next)
  {
    before = list;
    list = list->next;
    OY_FREE(before)
  }
  OY_FREE(list)

  DBG_PROG_ENDE
}

char*
printComp (oyComp* entry)
{ DBG_PROG_START
  #ifdef DEBUG
  static char text[MAX_PATH] = {0};
  DBG_PROG_S(("%d", (int)entry))
  sprintf( text, "begin %d next %d\nname %s val %s hits %d\n",
           (int)entry->begin, (int)entry->next, entry->name, entry->val, entry->hits);

  DBG_PROG_ENDE
  return text;
  #else
  DBG_PROG_ENDE
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
  DBG_PROG
  char* profileName = 0;
  int rc;

  oyComp *matchList = 0,
         *testEntry = 0,
         *foundEntry = 0;
  KeySet* profilesList;
  DBG_PROG

  kdbOpen();

  DBG_PROG
  if(manufacturer)
    DBG_PROG_S(( manufacturer ));
  if(model)
    DBG_PROG_S(( model ));
  if(product_id)
    DBG_PROG_S(( product_id ));
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
      oyDestroyCompList_ (matchList);
    }
  }

  ksClose (profilesList);
  kdbClose();
  OY_FREE(profilesList)

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

  oyComp *matchList = 0,
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
      oyDestroyCompList_ (matchList);
    }
  }

  ksClose (profilesList);
  kdbClose();
  OY_FREE(profilesList)

  DBG_PROG_ENDE
  return profileNames;
}

oyComp*
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
  oyComp *matchList = 0,
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
          { DBG_PROG
            DBG_PROG_S(( "%s %s", testEntry->name, current->key ))
            if (testEntry->name && current->key &&
                strstr(testEntry->name, current->key) != 0)
            { DBG_PROG_S(( "%s", strstr(testEntry->name, current->key) ))
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
            matchList = oyInitComp_(0,0);
          foundEntry = oyAppendComp_ (matchList, 0);
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

  OY_FREE(attributs)
  OY_FREE(value)
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
                                    size_t size)
{ DBG_PROG_START
  int rc = 0;
  char* comment = 0;

  if (mem && size && profileName)
  {
    rc = oyCheckProfile_Mem (mem, size); ERR
  }

  if (!rc)
  { DBG_PROG
    oyEraseDeviceProfile_          ( manufacturer, model, product_id,
                                     host, port,
                                     attrib1, attrib2, attrib3);

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

int
oyEraseDeviceProfile_              (const char* manufacturer,
                                    const char* model,
                                    const char* product_id,
                                    const char* host,
                                    const char* port,
                                    const char* attrib1,
                                    const char* attrib2,
                                    const char* attrib3)
{ DBG_PROG_START
  DBG_PROG
  char* profile_name = 0;
  int rc;
  KeySet* profilesList;

  DBG_PROG

  kdbOpen();

  profile_name = oyGetDeviceProfile_ (manufacturer, model, product_id,
                                      host, port, attrib1, attrib2, attrib3);

  DBG_PROG_S(("profile_name %s", profile_name ))

  // TODO merge User and System KeySets in oyReturnChildrenList_
  profilesList = oyReturnChildrenList_(OY_USER OY_REGISTRED_PROFILES, &rc ); ERR
  Key *current;
  char* value = (char*) calloc (sizeof(char), MAX_PATH);
  for (current=profilesList->start; current; current=current->next)
  {
    keyGetName(current, value, MAX_PATH);
    DBG_NUM_S(( value ))
    if(strstr(value, profile_name) != 0) {
      DBG_PROG_S((value))
      kdbRemove (value); 
      break;
    }
  }

  DBG_NUM_S(( value ))

  OY_FREE (profilesList) DBG_PROG
  OY_FREE (profile_name) DBG_PROG
  OY_FREE (value) DBG_PROG
  kdbClose(); DBG_PROG

  DBG_PROG_ENDE
  return rc;
}



/* --- internal API decoupling --- */

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

char*
oyGetPathFromProfileName (const char* profile_name)
{ DBG_PROG_START
  char* path_name = oyGetPathFromProfileName_ (profile_name);
  DBG_PROG_ENDE
  return path_name;
}

int
oySetDefaultWorkspaceProfile      (const char* name)
{ DBG_PROG_START
  int n = oySetDefaultWorkspaceProfile_ (name);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultWorkspaceProfileBlock (const char* name, void* mem, size_t size)
{ DBG_PROG_START
  int n = oySetDefaultWorkspaceProfile_Block (name, mem, size);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultXYZInputProfile           (const char* name)
{ DBG_PROG_START
  int n = oySetDefaultXYZInputProfile_ (name);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultXYZInputProfileBlock      (const char* name, void* mem, size_t size)
{ DBG_PROG_START
  int n = oySetDefaultXYZInputProfile_Block (name, mem, size);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultLabInputProfile           (const char* name)
{ DBG_PROG_START
  int n = oySetDefaultLabInputProfile_ (name);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultLabProfileInputBlock      (const char* name, void* mem, size_t size)
{ DBG_PROG_START
  int n = oySetDefaultLabInputProfile_Block (name, mem, size);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultRGBInputProfile           (const char* name)
{ DBG_PROG_START
  int n = oySetDefaultRGBInputProfile_ (name);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultRGBInputProfileBlock      (const char* name, void* mem, size_t size)
{ DBG_PROG_START
  int n = oySetDefaultRGBInputProfile_Block (name, mem, size);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultCmykInputProfile           (const char* name)
{ DBG_PROG_START
  int n = oySetDefaultCmykInputProfile_ (name);
  DBG_PROG_ENDE
  return n;
}

int
oySetDefaultCmykInputProfileBlock      (const char* name, void* mem, size_t size)
{ DBG_PROG_START
  int n = oySetDefaultCmykInputProfile_Block (name, mem, size);
  DBG_PROG_ENDE
  return n;
}

char*
oyGetDefaultWorkspaceProfileName  ()
{ DBG_PROG_START
  char* name = oyGetDefaultWorkspaceProfileName_ ();
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultXYZInputProfileName       ()
{ DBG_PROG_START
  char* name = oyGetDefaultXYZInputProfileName_ ();
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultLabInputProfileName       ()
{ DBG_PROG_START
  char* name = oyGetDefaultLabInputProfileName_ ();
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultRGBInputProfileName       ()
{ DBG_PROG_START
  char* name = oyGetDefaultRGBInputProfileName_ ();
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultCmykInputProfileName       ()
{ DBG_PROG_START
  char* name = oyGetDefaultCmykInputProfileName_ ();
  DBG_PROG_ENDE
  return name;
}

char**
oyProfileList                      (const char* colourspace, size_t *size)
{
  DBG_PROG_START
  char** names = oyProfileList_(colourspace, size);
  DBG_PROG_ENDE
  return names;
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
oyCheckProfileMem (const void* mem, size_t size,int flag)
{ DBG_PROG_START
  /* flag is currently ignored */
  int n = oyCheckProfile_Mem (mem, size);
  DBG_PROG_ENDE
  return n;
}

size_t
oyGetProfileSize                  (const char* profilename)
{ DBG_PROG_START
  size_t size = oyGetProfileSize_ (profilename);
  DBG_PROG_ENDE
  return size;
}

void*
oyGetProfileBlock                 (const char* profilename, size_t *size)
{ DBG_PROG_START
  char* block = oyGetProfileBlock_ (profilename, size);
  DBG_PROG_S( ("%s %d %d", profilename, (int)block, *size) )
  DBG_PROG

  DBG_PROG_ENDE
  return block;
}

char*
oyGetDeviceProfile                (oyDEVICETYP typ,
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
  if(profile_name)
    DBG_PROG_S( (profile_name) );

  DBG_PROG_ENDE
  return profile_name;
}

int
oySetDeviceProfile                (oyDEVICETYP typ,
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
                                   size_t size)
{ DBG_PROG_START
  int rc =     oySetDeviceProfile_ (manufacturer, model, product_id,
                                    host, port, attrib1, attrib2, attrib3,
                                    profileName, mem, size);
  DBG_PROG_ENDE
  return rc;
}

int
oyEraseDeviceProfile              (oyDEVICETYP typ,
                                   const char* manufacturer,
                                   const char* model,
                                   const char* product_id,
                                   const char* host,
                                   const char* port,
                                   const char* attrib1,
                                   const char* attrib2,
                                   const char* attrib3)
{ DBG_PROG_START
  int rc = oyEraseDeviceProfile_ (manufacturer, model, product_id,
                                    host, port, attrib1, attrib2, attrib3);

  DBG_PROG_ENDE
  return rc;
}



