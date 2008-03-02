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
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oyranos.h"
#include "oyranos_helper.h"
#include "oyranos_debug.h"

/* --- Helpers  --- */
#if 1
#define ERR if (rc<=0 && oy_debug) { printf("%s:%d %d\n", __FILE__,__LINE__,rc); perror("Error"); }
#else
#define ERR
#endif

/* --- static variables   --- */

static int oyranos_init = 0;
int oy_warn_ = 1;



/* --- internal API definition --- */

/* separate from the external functions */
void  oyOpen_                   (void);
void  oyClose_                  (void);
int   oyPathsCount_             (void);
char* oyPathName_               (int           number,
                                 oyAllocFunc_t allocate_func);
int   oyPathAdd_                (const char* pathname);
void  oyPathRemove_             (const char* pathname);
void  oyPathSleep_              (const char* pathname);
void  oyPathActivate_           (const char* pathname);
char* oyGetPathFromProfileName_ (const char*   profilename,
                                 oyAllocFunc_t allocate_func);


const char* oyMapDEFAULT_PROFILEtoString_       (oyDEFAULT_PROFILE type);
const char* oyMapDEFAULT_PROFILEtoConfigString_ (oyDEFAULT_PROFILE type);
int         oySetDefaultProfile_       (oyDEFAULT_PROFILE type,
                                        const char*       file_name);
int         oySetDefaultProfileBlock_  (oyDEFAULT_PROFILE type,
                                        const char*       file_name,
                                        void*             mem,
                                        size_t            size);
const char* oyGetDefaultProfileUITitle_(oyDEFAULT_PROFILE type);
char*       oyGetDefaultProfileName_   (oyDEFAULT_PROFILE type,
                                        oyAllocFunc_t     alloc_func);


char**  oyProfileList_                 (const char* coloursig, int * size);
void    oyProfileListFree_             (char** list, int size);

int	oyCheckProfile_                    (const char* name,const char* coloursig);
int	oyCheckProfile_Mem                 (const void* mem, size_t size,
                                        const char* coloursig);

size_t	oyGetProfileSize_                 (const char*   profilename);
void*	oyGetProfileBlock_                (const char*   profilename,
                                           size_t       *size,
                                           oyAllocFunc_t allocate_func);

char*   oyGetDeviceProfile_               (const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3,
                                           oyAllocFunc_t);
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
                                           const void* mem,
                                           size_t size);
int     oyEraseDeviceProfile_             (const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3);


#define oyDEVICE_PROFILE oyDEFAULT_PROFILE_TYPES
const char* oy_default_profile_types_names_[] = {
 "Rgb Editing",         /**< oyEDITING_RGB */
 "Cmyk Editing",        /**< oyEDITING_CMYK */
 "Assumed XYZ source",  /**< oyASSUMED_XYZ */
 "Assumed Lab source",  /**< oyASSUMED_LAB */
 "Assumed Rgb source",  /**< oyASSUMED_RGB */
 "Assumed Cmyk source", /**< oyASSUMED_CMYK*/
 "Device"               /**< oyDEVICE_PROFILE : a device profile */
};

void oyOpen_ (void)
{
  if(!oyranos_init) {
    kdbOpenDefault();
    oyranos_init = 1;
  }
  kdbOpen();
}
void oyClose_(void) { /* kdbClose(); */ }
/** @todo make oyOpen unnecessary */
void oyOpen  (void) { oyOpen_(); }
void oyClose (void) { oyClose_(); }


/* elektra key wrappers */
int     oyAddKey_valueComment_ (const char* keyName,
                                const char* value, const char* comment);

/* elektra key list handling */
char*   oySearchEmptyKeyname_  (const char* keyParentName,
                                const char* keyBaseName);
KeySet* oyReturnChildrenList_  (const char* keyParentName,int* rc);

const char* oySelectUserSys_   ();
enum {
  oyUSER_SYS = 0,
  oyUSER,
  oySYS,
};

/* complete an name from file including oyResolveDirFileName */
char*   oyMakeFullFileDirName_     (const char* name);
/* find an file/dir and do corrections on  ~ ; ../  */
char*   oyResolveDirFileName_      (const char* name);
char*   oyExtractPathFromFileName_ (const char* name);
char*   oyGetHomeDir_              ();
char*   oyGetParent_               (const char* name);
int     oyRecursivePaths_      (int (*doInPath) (void*,const char*,const char*),
                                void* data);

int oyIsDir_      (const char* path);
int oyIsFile_     (const char* fileName);
int oyIsFileFull_ (const char* fullFileName);
int oyMakeDir_    (const char* path);

int   oyWriteMemToFile_ (const char* name, void* mem, size_t size);
char* oyReadFileToMem_  (const char* fullFileName, size_t *size,
                         oyAllocFunc_t allocate_func);

/* oyranos part */
/* check for the global and the users directory */
void oyCheckDefaultDirectories_ ();
/* search in profile path and in current path */
char* oyFindProfile_ (const char* name);

/* Profile registring */
int oySetProfile_      (const char* name, oyDEFAULT_PROFILE type, const char* comment);
int oySetProfile_Block (const char* name, void* mem, size_t size,
                        oyDEFAULT_PROFILE type, const char* comnt);

/**@internal A small search engine
 *
 * for one simple, single list, dont mix lists!!
 * name and val are not alloced or freed 
 */

struct OyComp_s {
  struct OyComp_s *next;   /* chain connection */
  struct OyComp_s *begin;  /* chain connection */
  char            *name;   /* key name */
  char            *val;    /* its value */
  int              hits;   /* weighting */
};

typedef struct OyComp_s oyComp_t;

oyComp_t* oyInitComp_      (oyComp_t *compare, oyComp_t *top);
oyComp_t* oyAppendComp_    (oyComp_t *list,    oyComp_t *new);
void    oySetComp_         (oyComp_t *compare, const char* keyName,
                            const char* value, int hits );
void    oyDestroyCompList_ (oyComp_t* list);

oyComp_t* oyGetDeviceProfile_sList          (const char* manufacturer,
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

  /* ksNext uses the same entry twice in a 1 component KeySet, we avoid this */
#define FOR_EACH_IN_KDBKEYSET( current, list ) \
   ksRewind( list );  \
   for( current = ksNext( list ); current; current = ksNext( list )  )



/* --- function definitions --- */

KeySet*
oyReturnChildrenList_ (const char* keyParentName, int* rc)
{ DBG_PROG_START
  int user_sys = oyUSER_SYS;
  KeySet*list_user = 0;
  KeySet*list_sys = 0;
  KeySet*list = ksNew();
  char  *list_name_user = (char*)alloca(MAX_PATH);
  char  *list_name_sys = (char*)alloca(MAX_PATH);

  if( user_sys == oyUSER_SYS || user_sys == oyUSER ) {
    list_user = ksNew();
    sprintf(           list_name_user, "%s%s", OY_USER, keyParentName);
    *rc =
      kdbGetChildKeys( list_name_user, list_user, KDB_O_RECURSIVE | KDB_O_SORT);
  }
  if( user_sys == oyUSER_SYS || user_sys == oySYS ) {
    list_sys = ksNew();
    sprintf(           list_name_sys, "%s%s", OY_SYS, keyParentName);
    *rc =
      kdbGetChildKeys( list_name_sys, list_sys, KDB_O_RECURSIVE | KDB_O_SORT);
  }

  if(list_user)
    ksAppendKeys(list, list_user);
  if(list_sys)
    ksAppendKeys(list, list_sys);

  DBG_PROG_V(( (int)keyParentName ))
  DBG_PROG_S(( keyParentName ))
  DBG_PROG_V(( ksGetSize(list) ))

  DBG_PROG_ENDE
  return list;
}

char*
oySearchEmptyKeyname_ (const char* keyParentName, const char* keyBaseName)
{ DBG_PROG_START
  char* keyName = (char*)     calloc (strlen(keyParentName)
                                    + strlen(keyBaseName) + 24, sizeof(char));
  char* pathkeyName = (char*) alloca (strlen(keyBaseName) + 24);
  int nth = 0, i = 1, rc=0;
  Key *key;
  char *name = (char*)alloca(MAX_PATH);
  sprintf(name, "%s%s", oySelectUserSys_(), keyParentName);

  key = keyNew(keyBaseName);

  if(keyParentName)
    DBG_PROG_S((keyParentName))
  if(keyBaseName)
    DBG_PROG_S((keyBaseName))
  if(name)
    DBG_PROG_S((name))

    /* search for empty keyname */
    while (!nth)
    { sprintf (pathkeyName , "%s%d", keyBaseName, i);
      rc=kdbGetKeyByParent (name, pathkeyName, key);
      if (rc != KDB_RET_OK)
        nth = i;
      i++;
    }
    sprintf (keyName, ("%s/%s"), OY_PATHS, pathkeyName);

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
  int rc=0;
  Key *key;
  char *name = (char*)alloca(MAX_PATH);
  sprintf(name, "%s%s", oySelectUserSys_(), keyName);

  if (keyName)
    DBG_PROG_S(( keyName ));
  if (value)
    DBG_PROG_S(( value ));
  if (comment)
    DBG_PROG_S(( comment ));

  key=keyNew(name);

  //rc=keyInit(key); ERR
  //rc=keySetName (key, keyName);
  rc=kdbGetKey(key);
  rc=keySetString (key, value);
  rc=keySetComment (key, comment);
  //TODO debug
  oyOpen_();
  rc=kdbSetKey(key);
  oyClose_();

  DBG_PROG_ENDE
  return rc;
}

const char*
oySelectUserSys_()
{
  /* enable system wide keys for user root */
  if(geteuid() == 0)
    return OY_SYS;
  else
    return OY_USER;
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
      WARN_S( ("could not read %s\n", filename) );
  }

  DBG_PROG_ENDE
  return size;
}

char*
oyReadFileToMem_(const char* name, size_t *size,
                 oyAllocFunc_t allocate_func)
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

      DBG_PROG_S(("%u\n",((size_t)size)));

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
        } else {
          /* copy to external allocator */
          char* temp = mem;
          mem = allocate_func(*size);
          if(mem) {
            memcpy( mem, temp, *size );
            OY_FREE (temp)
          } else {
            OY_FREE (mem)
            *size = 0;
          }
        }
      }
    } else {
      WARN_S( ("could not read %s\n", filename) );
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
  if (!oyIsDir_ (OY_PROFILE_PATH_SYSTEM_DEFAULT))
  { DBG_PROG
    WARN_S( ("no default system directory %s\n",OY_PROFILE_PATH_SYSTEM_DEFAULT) );
  }

  if (!oyIsDir_ (OY_PROFILE_PATH_USER_DEFAULT))
  { DBG_PROG 
    parentDefaultUserDir = oyGetParent_ (OY_PROFILE_PATH_USER_DEFAULT);

    if (!oyIsDir_ (parentDefaultUserDir))
    {
      DBG_PROG_S( ("Try to create part of users default directory %s\n",
                 parentDefaultUserDir ));
      oyMakeDir_( parentDefaultUserDir);
    }
    OY_FREE (parentDefaultUserDir)

    DBG_PROG_S( ("Try to create users default directory %s\n",
               OY_PROFILE_PATH_USER_DEFAULT ) )
    oyMakeDir_( OY_PROFILE_PATH_USER_DEFAULT );
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
    char* path_name = oyGetPathFromProfileName_(fileName, oyAllocateFunc_);
    fullFileName = (char*) calloc (MAX_PATH, sizeof(char));
    sprintf(fullFileName, "%s%s%s", path_name, OY_SLASH, fileName);
    OY_FREE(path_name)
    DBG_PROG_S(( fullFileName ))
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

int
oyGetPathFromProfileNameCb_ (void* data, const char* full_name,
                                         const char* filename)
{
  char* search = (char*) data;
  int success = 0;
  DBG_S(( search ))
  if(strcmp(filename,search)==0) {
    size_t size = 128;
    char* header = oyReadFileToMem_ (full_name, &size, oyAllocateFunc_);
    success = !oyCheckProfile_Mem (header, size, 0);
    OY_FREE (header)
    if (success) {
      DBG_S((full_name))
      DBG_V((strlen(full_name)))
      if (strlen(full_name) < MAX_PATH) {
        sprintf(search,full_name);
        search[strlen(full_name)] = 0;
      } else
        search[0] = 0;
    } else
      WARN_S(( "not a profile %s", full_name ))
  }
  /* break on success */
  DBG_V((success))
  return success;
}

char*
oyGetPathFromProfileName_       (const char*   fileName,
                                 oyAllocFunc_t allocate_func)
{
  DBG_PROG_START
  char  *fullFileName = 0;
  char  *pathName = 0;
  int    success = 0;
  char  *header = 0;
  size_t    size;

  //DBG_NUM_S((fileName))
  /* test for pure file without dir; search in configured paths */
  if (fileName && !strchr(fileName, OY_SLASH_C))
  { DBG_PROG
    char search[MAX_PATH];
    if(strlen(fileName) < MAX_PATH)
      sprintf(search, fileName);
    else {
      WARN_S(("name longer than %d", MAX_PATH));
      DBG_PROG_ENDE
      return 0;
    }
    success = oyRecursivePaths_(oyGetPathFromProfileNameCb_,(void*)search);
#   if 0
    int   n_paths = oyPathsCount_ (),
          i;

    DBG_PROG_S(("pure filename found"))
    DBG_PROG_S(("n_paths = %d", n_paths ))
    fullFileName = (char*) allocate_func( MAX_PATH );

    for (i = 0; i < n_paths; i++)
    { /* test profile */
      char* ptr = oyPathName_ (i, oyAllocateFunc_);
      pathName = oyMakeFullFileDirName_ (ptr);
      sprintf (fullFileName, "%s%s%s", pathName, OY_SLASH, fileName);

      DBG_PROG_S((pathName))
      DBG_PROG_S((fullFileName))

      if (oyIsFileFull_(fullFileName))
      { DBG_PROG
        size = 128;
        header = oyReadFileToMem_ (fullFileName, &size, allocate_func);
        success = !oyCheckProfile_Mem (header, size);
      }

      OY_FREE (ptr)
      OY_FREE (header)
#     endif
      if (success) { /* found */
        int len = 0;
        DBG_S((search))
        if(search[0] != 0) len = strlen(search);
        if(len) {
          char *ptr = 0;
          pathName = (char*) allocate_func( len+1 );
          sprintf(pathName, search);
          ptr = strrchr(pathName , OY_SLASH_C);
          if(ptr)
            ptr[0] = 0;
        }
        DBG_PROG_S(( pathName ))
        DBG_PROG_ENDE
        return pathName;
      }
    //}

    if (!success) {
      if(oy_warn_)
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
      header = oyReadFileToMem_ (fullFileName, &size, allocate_func);

      if (size >= 128)
        success = !oyCheckProfile_Mem (header, 128, 0);
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
oySetProfile_      (const char* name, oyDEFAULT_PROFILE type, const char* comment)
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

  DBG_PROG_S( ("name = %s type %d", name, type) )

  if ( name == 0 || !oyCheckProfile_ (fileName, 0) )
  {
    const char* config_name = 0;
    DBG_PROG_S(("set fileName = %s as %d profile\n",fileName, type))
    if ( type < 0 )
      WARN_S( ("%s:%d %s() !!! ERROR type %d; type does not exist",__FILE__,__LINE__, __func__, type ) );

    if(type < oyDEFAULT_PROFILE_TYPES)
      config_name = oyMapDEFAULT_PROFILEtoConfigString_(type);
    else if(type == oyDEVICE_PROFILE)
      {
        int len = strlen(OY_REGISTRED_PROFILES)
                  + strlen(fileName);
        char* keyName = (char*) calloc (len +10, sizeof(char)); DBG_PROG
        sprintf (keyName, "%s%s", OY_REGISTRED_PROFILES OY_SLASH, fileName); DBG_PROG
        r = oyAddKey_valueComment_ (keyName, com, 0); DBG_PROG
        DBG_PROG_S(( "%s %d", keyName, len ))
        OY_FREE (keyName)
      }
      else
        WARN_S( ("%s:%d !!! ERROR type %d type does not exist for default profiles",__FILE__,__LINE__, type));
      
    
    if(config_name)
    {
      if(name) {
        r = oyAddKey_valueComment_ (config_name, fileName, com);
        DBG_PROG_S(( "%s %s %s",config_name,fileName,com ))
      } else {
        KeySet* list;
        Key *current;
        char* value = (char*) calloc (sizeof(char), MAX_PATH);
        int rc = 0;

        DBG_PROG

        list = oyReturnChildrenList_(OY_KEY OY_SLASH "default", &rc ); ERR
        if(!list)
        {
          FOR_EACH_IN_KDBKEYSET( current, list )
          {
            keyGetName(current, value, MAX_PATH);
            DBG_NUM_S(( value ))
            if(strstr(value, config_name) != 0 && strlen(value) == strlen(config_name))
            {
              DBG_PROG_S((value))
              kdbRemove (value); 
              break;
            }
          }
        }

        DBG_NUM_S(( value ))

        OY_FREE (list) DBG_PROG
        OY_FREE (value) DBG_PROG
        oyClose_(); DBG_PROG
        r = rc;
      }
      DBG_PROG_V( r )
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
  int rc=0, n = 0;

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_PATHS, &rc ); ERR
  if(!myKeySet)
  {
    oyClose_();
    DBG_PROG
    DBG_PROG_ENDE
    return n;
  }

  //if(!rc)
    n = ksGetSize(myKeySet);

  ksClose (myKeySet);
  oyClose_();
  OY_FREE(myKeySet)

  DBG_PROG_ENDE
  return n;
}

char*
oyPathName_ (int number, oyAllocFunc_t allocate_func)
{ DBG_PROG_START
  int rc=0, n = 0;
  Key *current;
  char* value;

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_PATHS, &rc ); ERR
  if(!myKeySet)
  {
    oyClose_();
    DBG_PROG_ENDE
    return 0;
  }

  value = (char*) allocate_func( MAX_PATH );

  if (number <= ksGetSize(myKeySet))
    FOR_EACH_IN_KDBKEYSET( current, myKeySet )
    {
      if (number == n) {
        keyGetComment (current, value, MAX_PATH);
        if (strstr(value, OY_SLEEP) == 0)
          keyGetString(current, value, MAX_PATH);
      }
      n++;
    }

  ksClose (myKeySet);
  oyClose_();
  OY_FREE(myKeySet)

  DBG_PROG_ENDE
  return value;
}

int
oyPathAdd_ (const char* pfad)
{
  DBG_PROG_START
  int rc=0, n = 0;
  Key *current;
  char* keyName = 0;
  char* value = 0;
  int has_local_path = 0, has_global_path = 0;

  /* are we setting a default path? */
  if (strcmp (pfad, OY_PROFILE_PATH_USER_DEFAULT) == 0)
    has_local_path = 1;
  if (strcmp (pfad, OY_PROFILE_PATH_SYSTEM_DEFAULT) == 0)
    has_global_path = 1;

  if(pfad)
    DBG_PROG_S(( pfad ));

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_PATHS, &rc ); ERR
  if(!myKeySet) {
    has_local_path = 0;
    has_global_path = 0;
    goto finish;
  }

  keyName = (char*) calloc (sizeof(char), MAX_PATH);
  value = (char*) calloc (sizeof(char), MAX_PATH);

  /* search for allready included path */
  DBG_PROG_S(( "path items: %d", ksGetSize(myKeySet) ))
  FOR_EACH_IN_KDBKEYSET( current, myKeySet )
  {
    keyGetString(current, value, MAX_PATH);
    keyGetFullName(current, keyName, MAX_PATH);
    if (value) DBG_PROG_S(( value ));
    if (strstr (value, pfad) != 0) {
      ++n;
      DBG_PROG_S(( "occurency: %d %s %s", n, pfad, keyName ));
    }

    /* Are the default paths allready there? */
    if (strcmp(value, OY_PROFILE_PATH_USER_DEFAULT) == 0) has_local_path = 1;
    if (strcmp(value, OY_PROFILE_PATH_SYSTEM_DEFAULT) == 0) has_global_path = 1;
  }

  if (n) DBG_PROG_S(("Key %s was allready %d times there\n", pfad, n));

  /* erase double occurencies of this path */
  if (n > 1)
  {
    FOR_EACH_IN_KDBKEYSET( current, myKeySet )
    {
      rc=keyGetString(current,value, MAX_PATH); ERR

      if (strcmp (value, pfad) == 0 &&
          n)
      {
        rc=keyGetFullName(current,keyName, MAX_PATH); ERR
        rc=kdbRemove(keyName); ERR
        DBG_PROG_S(( "erase path key : %s %s", pfad, keyName ));
        n--;
      }
    }
  } else if (!n) {
  /* add path */
    DBG_PROG_S(( "path will be added: %s", pfad ));

    /* search for empty keyname */
    keyName = oySearchEmptyKeyname_ (OY_PATHS, OY_PATH);

    /* write key */
    rc = oyAddKey_valueComment_ (keyName, pfad, "");
  }

  finish:

  if (!has_global_path)
  {
    keyName = oySearchEmptyKeyname_ (OY_PATHS, OY_PATH);
    rc = oyAddKey_valueComment_ (keyName, OY_PROFILE_PATH_SYSTEM_DEFAULT, "");
  }
  if (!has_local_path)
  {
    keyName = oySearchEmptyKeyname_ (OY_PATHS, OY_PATH);
    rc = oyAddKey_valueComment_ (keyName, OY_PROFILE_PATH_USER_DEFAULT, "");
  }

  if (myKeySet) ksClose (myKeySet);
  if (myKeySet) ksDel (myKeySet);
  oyClose_();
  OY_FREE (keyName)
  OY_FREE (value)

  oyCheckDefaultDirectories_();

  DBG_PROG_ENDE
  return rc;
}

void
oyPathRemove_ (const char* pfad)
{ DBG_PROG_START
  int rc=0;
  Key *current;
  char* value;
  char* keyName;

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_PATHS, &rc ); ERR
  if(!myKeySet)
  {
    oyPathAdd_ (OY_PROFILE_PATH_USER_DEFAULT);
    oyClose_();
    DBG_PROG_ENDE
    return;
  }

  value = (char*) calloc (sizeof(char), MAX_PATH);
  keyName = (char*) calloc (sizeof(char), MAX_PATH);

  /* compare and erase if matches */
  FOR_EACH_IN_KDBKEYSET( current, myKeySet )
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
  oyPathAdd_ (OY_PROFILE_PATH_USER_DEFAULT);

  oyClose_();
  OY_FREE(myKeySet)
  OY_FREE(keyName)
  OY_FREE(value)


  DBG_PROG_ENDE
}

void
oyPathSleep_ (const char* pfad)
{ DBG_PROG_START
  int rc=0;
  Key *current;
  char* value;

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_PATHS, &rc ); ERR
  if(!myKeySet) {
    oyClose_();

    DBG_PROG_ENDE
    return;
  }

  value = (char*) calloc (sizeof(char), MAX_PATH);

  /* set "SLEEP" in comment */
  FOR_EACH_IN_KDBKEYSET( current, myKeySet )
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
  oyClose_();
  OY_FREE(myKeySet)
  OY_FREE (value);
  DBG_PROG_ENDE
}

void
oyPathActivate_ (const char* pfad)
{ DBG_PROG_START
  int rc=0;
  Key *current;
  char* value;

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_PATHS, &rc ); ERR
  if(!myKeySet) {
    oyClose_();

    DBG_PROG_ENDE
    return;
  }

  value = (char*) calloc (sizeof(char), MAX_PATH);

  /* erase "SLEEP" from comment */
  FOR_EACH_IN_KDBKEYSET( current, myKeySet )
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
  oyClose_();
  OY_FREE(myKeySet)
  OY_FREE (value);
  DBG_PROG_ENDE
}

/* default profiles API */
const char*
oyMapDEFAULT_PROFILEtoString_ (oyDEFAULT_PROFILE type)
{ DBG_PROG_START
  const char *type_string = 0;
  if(0 <= type && type < oyDEFAULT_PROFILE_TYPES)
    type_string = oy_default_profile_types_names_[type];
  DBG_PROG_ENDE
  return type_string;
}

const char*
oyMapDEFAULT_PROFILEtoConfigString_ (oyDEFAULT_PROFILE type)
{ DBG_PROG_START
  const char *config_string = 0;
  switch (type) {
    case oyEDITING_RGB: config_string = OY_DEFAULT_EDITING_RGB_PROFILE; break;
    case oyEDITING_CMYK: config_string = OY_DEFAULT_EDITING_CMYK_PROFILE; break;
    case oyASSUMED_XYZ: config_string = OY_DEFAULT_ASSUMED_XYZ_PROFILE; break;
    case oyASSUMED_LAB: config_string = OY_DEFAULT_ASSUMED_LAB_PROFILE; break;
    case oyASSUMED_RGB: config_string = OY_DEFAULT_ASSUMED_RGB_PROFILE; break;
    case oyASSUMED_CMYK: config_string = OY_DEFAULT_ASSUMED_CMYK_PROFILE; break;
    default: WARN_S( ("Default Profile type %d does not exist", type) )
  }
  DBG_PROG_ENDE
  return config_string;
}

int
oySetDefaultProfile_       (oyDEFAULT_PROFILE type,
                            const char*       file_name)
{ DBG_PROG_START
  int r = oySetProfile_ (file_name, type, 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultProfileBlock_  (oyDEFAULT_PROFILE type,
                            const char* file_name, void* mem, size_t size)
{ DBG_PROG_START
  int r = oySetProfile_Block (file_name, mem, size, type, 0);
  DBG_PROG_ENDE
  return r;
}

const char*
oyGetDefaultProfileUITitle_(oyDEFAULT_PROFILE type)
{ DBG_PROG_START
  const char* name = oyMapDEFAULT_PROFILEtoString_(type);
  DBG_PROG_ENDE
  return name;
}

char*
oyGetDefaultProfileName_   (oyDEFAULT_PROFILE type,
                            oyAllocFunc_t     alloc_func)
{ DBG_PROG_START
  char* name = (char*) alloc_func (MAX_PATH);
  char* keyName = (char*) alloca (MAX_PATH);
  int rc = 0;
  sprintf(keyName, "%s%s", oySelectUserSys_(),
                           oyMapDEFAULT_PROFILEtoConfigString_(type));

  rc = kdbGetValue (keyName, name, MAX_PATH);

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  if(rc == KDB_RET_OK)
    return name;
  else
    return 0;
}




/* profile lists API */

#define MAX_DEPTH 64
struct OyProfileList_s_ {
  int hopp;
  const char* coloursig;
  int mem_count;
  int count_files;
  char** names;
};

int oyProfileListCb_ (void* data, const char* full_name, const char* filename)
{
  struct OyProfileList_s_ *l = (struct OyProfileList_s_*)data;

      if (!oyCheckProfile_(full_name, l->coloursig))
      {
        if(l->count_files >= l->mem_count)
        {
          char** temp = l->names;
          l->names = (char**) calloc (sizeof(char*), l->mem_count+l->hopp);
          memcpy(l->names, temp, sizeof(char*) * l->mem_count);
          l->mem_count += l->hopp;
        }

        l->names[l->count_files] = (char*) calloc (sizeof(char)*2,
                                                   strlen(filename));
        strcpy(l->names[l->count_files], filename);
        ++l->count_files;
      } //else
        //WARN_S(("%s in %s is not a valid profile", file_name, path));
  return 0;
}

char**
oyProfileList_                     (const char* coloursig, int * size)
{
  DBG_PROG_START
  oy_warn_ = 0;
  struct OyProfileList_s_ l = {128, coloursig, 128, 0, 0};
  l.names = 0;
  l.mem_count = l.hopp;
  l.count_files = 0;

  l.names = (char**) calloc (sizeof(char*), l.mem_count);

  oyRecursivePaths_(oyProfileListCb_, (void*) &l);

  *size = l.count_files;
  oy_warn_ = 1;
  DBG_PROG_ENDE
  return l.names;
}

/** @internal
 *  doInPath and data must fit, doInPath can operate on data and after finishing
 *  oyRecursivePaths_ data can be processed further
 */
int
oyRecursivePaths_  ( int (*doInPath)(void*,const char*,const char*), void* data)
{
  DBG_PROG_START
  int r = 0;
  int count = oyPathsCount_();
  int i;

  for(i = 0; i < count ; ++i)
  {
    struct stat statbuf;
    struct dirent *entry[MAX_DEPTH];
    DIR *dir[MAX_DEPTH];
    char *p = oyPathName_(i, oyAllocateFunc_);
    char *path = oyMakeFullFileDirName_(p);
    int l = 0; /* level */
    int run = !r;

    if ((stat (path, &statbuf)) != 0) {
      WARN_S(("%d. path %s does not exist", i, path))
      continue;
    }
    if (!S_ISDIR (statbuf.st_mode)) {
      WARN_S(("%d. path %s is not a directory", i, path));
      continue;
    }
    if (S_ISLNK (statbuf.st_mode)) {
      WARN_S(("%d. path %s is a link: ignored", i, path));
      continue;
    }
    dir[l] = opendir (path);
    if (!dir[l]) {
      WARN_S(("%d. path %s is not readable", i, path));
      continue;
    }

    while(run)
    {
      if(!(entry[l] = readdir (dir[l]))) {
        closedir(dir[l]);
        --l;
        if(l<0) {
          run = 0;
        }
        goto cont;
      }

      char name[256];
      int k;
      sprintf(name,path);
      for (k=0; k <= l; ++k) {
        int len = strlen(name);
        if(len+strlen(entry[k]->d_name) < 256)
          sprintf(&name[strlen(name)],"/%s", entry[k]->d_name);
        else {
          DBG_PROG_S(("%d. %s/%s ignored", l, name, entry[k]->d_name))
          goto cont;
        }
      }

      if ((strcmp (entry[l]->d_name, "..") == 0) ||
          (strcmp (entry[l]->d_name, ".") == 0)) {
        DBG_PROG_S(("%d. %s ignored", l, name))
        goto cont;
      }
      if ((stat (name, &statbuf)) != 0) {
        DBG_PROG_S(("%d. %s does not exist", l, name))
        goto cont;
      }
      if (!S_ISLNK(statbuf.st_mode)){//((statbuf.st_mode & S_IFMT) & S_IFLNK)) 
        DBG_PROG_S(("%d. %s is a link: ignored %d %d %d", l, name, statbuf.st_mode , S_IFLNK, 0120000));
        //goto cont;
      }
      if (S_ISDIR (statbuf.st_mode) &&
          l < MAX_DEPTH ) {

        dir[l+1] = opendir (name);
        ++l;
        DBG_PROG_S(("%d. %s directory", l, name));
        goto cont;
      }
      if(!S_ISREG (statbuf.st_mode)) {
        DBG_PROG_S(("%d. %s is a non regular file", l, name));
        goto cont;
      }
      DBG_PROG_S( ("%d. a valid file %s", l, name) )

      /* use all file extensions */
      /* go recursively without following links, due to security */
      if( !r ) {
        r = doInPath(data, name, entry[l]->d_name);
        run = !r;
        if(r)
          DBG_S(("%d. %d %d found", i, r, run));
      }
      cont: ;
    }
  }

  DBG_PROG_ENDE
  return r;
}

void
oyProfileListFree_                 (char** list, int size)
{ DBG_PROG_START
  size_t i;
  for(i = 0; i < size; ++i)
    if (list[i] )
      free( list[i] );
  free( list );
  DBG_PROG_ENDE
}

/* profile check API */

int
oyCheckProfile_                    (const char* name,
                                    const char* coloursig)
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
    header = oyReadFileToMem_ (fullName, &size, oyAllocateFunc_); DBG_PROG
    if (size >= 128)
      r = oyCheckProfile_Mem (header, 128, coloursig);
  }

  /* release memory */
  if(header && size)
    free(header);
  if(fullName) free(fullName);

  DBG_NUM_S(("oyCheckProfileMem = %d",r))

  DBG_PROG_ENDE
  return r;
}

int
oyCheckProfile_Mem                 (const void* mem, size_t size,
                                    const char* coloursig)
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
      if(oy_warn_)
        WARN_S((" sign: %c%c%c%c ", (char)block[offset+0],
        (char)block[offset+1], (char)block[offset+2], (char)block[offset+3] ));
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
oyGetProfileBlock_                 (const char* profilename, size_t *size,
                                    oyAllocFunc_t allocate_func)
{ DBG_PROG_START
  char* fullFileName = oyFindProfile_ (profilename);
  char* block = oyReadFileToMem_ (fullFileName, size, allocate_func);

  DBG_PROG_ENDE
  return block;
}

int
oySetProfile_Block (const char* name, void* mem, size_t size,
                    oyDEFAULT_PROFILE type, const char* comnt)
{ DBG_PROG_START
  int r = 0;
  char *fullFileName, *resolvedFN;
  const char *fileName;

  if (strrchr (name, OY_SLASH_C))
    fileName = strrchr (name, OY_SLASH_C);
  else
    fileName = name;

  fullFileName = (char*) calloc (sizeof(char),
                  strlen(OY_PROFILE_PATH_USER_DEFAULT) + strlen (fileName) + 4);

  sprintf (fullFileName, "%s%s%s",
           OY_PROFILE_PATH_USER_DEFAULT, OY_SLASH, fileName);

  resolvedFN = oyResolveDirFileName_ (fullFileName);
  OY_FREE(fullFileName)
  fullFileName = resolvedFN;

  if (!oyCheckProfile_Mem( mem, size, 0))
  {
    DBG_PROG_S((fullFileName))
    if ( oyIsFile_(fullFileName) ) {
      WARN_S (("file %s exist , please remove befor installing new profile\n", fullFileName))
    } else
    { r = oyWriteMemToFile_ (fullFileName, mem, size);
      oySetProfile_ ( name, type, comnt);
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

oyComp_t*
oyInitComp_ (oyComp_t *list, oyComp_t *top)
{ DBG_PROG_START
  if (!list)
    list = (oyComp_t*) calloc (1, sizeof(oyComp_t));

  list->next = 0;

  if (top)
    list->begin = top;
  else
    list->begin = list;
  list->name = 0;
  list->val = 0;
  list->hits = 0;
  DBG_PROG_ENDE
  return list;
}

oyComp_t*
oyAppendComp_ (oyComp_t *list, oyComp_t *new)
{ DBG_PROG_START

  /* no list yet => first and only entry */
  if (!list) {
    list = oyInitComp_(list,0);
    DBG_PROG_ENDE
    return list;
  }

  list = list->begin;
  while (list->next)
    list = list->next;

  /* no new => add */
  if (!new)
    new = oyInitComp_(new, list->begin);

  list->next = new;

  DBG_PROG_ENDE
  return new;
}

void
oySetComp_         (oyComp_t *compare, const char* keyName,
                    const char* value, int hits )
{
  DBG_PROG_START
  compare->name = (char*) calloc( strlen(keyName)+1, sizeof(char) );
  memcpy (compare->name, keyName, strlen(keyName)+1); 
  compare->val = (char*) calloc( strlen(value)+1, sizeof(char) );;
  memcpy (compare->val, value, strlen(value)+1); 
  compare->hits = hits;
  DBG_PROG_ENDE
}

void
oyDestroyCompList_ (oyComp_t *list)
{ DBG_PROG_START
  oyComp_t *before;

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
printComp (oyComp_t* entry)
{ DBG_PROG_START
  #ifdef DEBUG
  static char text[MAX_PATH] = {0};
  DBG_PROG_S(("%d", (int)entry))
  sprintf( text, "%s:%d %s() begin %d next %d\n",
           __FILE__,__LINE__,__func__,
           (int)entry->begin, (int)entry->next );

  if(entry->name)
    sprintf( &text[strlen(text)], " name %s %d", entry->name, (int)entry->name);
  if(entry->val)
    sprintf( &text[strlen(text)], " val %s %d", entry->val, (int)entry->val);
  sprintf( &text[strlen(text)], " hits %d\n", entry->hits);

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
                                    const char* attrib3,
                                    oyAllocFunc_t allocate_func)
{ DBG_PROG_START
  DBG_PROG
  char* profileName = 0;
  int rc=0;

  oyComp_t *matchList = 0,
         *testEntry = 0,
         *foundEntry = 0;
  KeySet *profilesList;

  profilesList = 
   oyReturnChildrenList_(OY_REGISTRED_PROFILES, &rc ); ERR

  if(!profilesList) {
    oyClose_();

    DBG_PROG_ENDE
    return profileName;
  }

  matchList = oyGetDeviceProfile_sList (manufacturer, model, product_id,
                                        host, port, attrib1, attrib2, attrib3,
                                        profilesList, rc);

  /* 6. select the profile from the match list with the most hits */
  if (matchList)
  {
    int max_hits = 0;
    int count = 0;
    foundEntry = 0;
    DBG_PROG_S(( "matchList->begin->next: %d\n", (int)matchList->begin->next ))
    for (testEntry=matchList->begin; testEntry; testEntry=testEntry->next)
    {
      DBG_PROG_S(( "testEntry %d count: %d\n", (int)testEntry, count++ ))
      if (testEntry->hits > max_hits)
      {
        foundEntry = testEntry;
        max_hits = testEntry->hits;
      }
    }
    if(foundEntry) DBG_PROG_S ((printComp (foundEntry)))

    /* 7. tell about the profile and its hits */
    if(foundEntry)
    {
      char *fileName = 0;
      if (foundEntry->name)
        DBG_PROG_S(("%s\n",foundEntry->name) );
      if (foundEntry->name &&
          strlen(foundEntry->name) &&
          strrchr(foundEntry->name , OY_SLASH_C))
      {
        fileName = strrchr(foundEntry->name , OY_SLASH_C);
        fileName++;
      }
      else
        fileName = foundEntry->name;

      int len = strlen (fileName)+1;
      profileName = (char*) allocate_func (len);
      sprintf (profileName, fileName);

      DBG_PROG_S((foundEntry->name))
      DBG_PROG_S((profileName))
      oyDestroyCompList_ (matchList);
    }
  }

  ksClose (profilesList);
  oyClose_();
  OY_FREE(profilesList)

  DBG_PROG_ENDE
  return profileName;
}

#if 0
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

  oyComp_t *matchList = 0,
         *testEntry = 0,
         *foundEntry = 0;
  KeySet* profilesList;

  kdbOpen();

  // TODO merge User and System KeySets in oyReturnChildrenList_
  profilesList = oyReturnChildrenList_(OY_USER OY_REGISTRED_PROFILES, &rc ); ERR
  if(!profilesList) {
    oyClose_();

    DBG_PROG_ENDE
    return profileNames;
  }

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
    if(foundEntry) DBG_PROG_S ((printComp (foundEntry)))

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
      // @TODO add profileName to profileNames

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
#endif

  /**
   * @internal
   * Search description
   *
   * This routine describes the A approach
   *   - registred profiles with assigned devices
   *
   * -# take all arguments and walk through the named devices list \n
   *    //  named devices consist of an key with the profile name + attributes\n
   *    //  it is not allowed to have two profiles with the same name\n
   *    //  it is allowed to have different profiles for the same attribute :(\n
   *    //  specify more attributes to make an decission presumable\n
   *    //   or maintain profiles, erasing older and invalid ones
   * -# test if attributes matches the value of the key, count the hits
   * -# search the profile in an match list
   * -# add the profile to the match list if not found
   * -# increase the hits counter in the macht list for that profile
   * -# select the profile from the match list with the most hits
   * -# tell about the profile and its hits
   *
   * @todo approach B:\n
   * no attributes are assigned beside certain keyword ("monitor", "scanner")\n
   * scan profile tags for manufacturer, device descriptions ... \n<ul>
   * <li> When to start an automatic registration run?</li>\n
   * <li> include profile tag editing?</li>
   * </ul>
   * @todo other things:\n
   * <ul>
   * <li> spread weighting? 3 degrees are sufficient How to merge in the on
   *       string approach?</li></ul>
   *
   */


oyComp_t*
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
  /* 1. take all arguments and walk through the named devices list */
  int i = 0, n = 0;
  char* name  = (char*) calloc (MAX_PATH, sizeof(char));
  char* value = (char*) calloc (MAX_PATH, sizeof(char));
  const char **attributs = (const char**) alloca (8 * sizeof (const char*));
  Key *current;
  oyComp_t *matchList = 0,
         *testEntry = 0;

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


  if (profilesList)
  {
    FOR_EACH_IN_KDBKEYSET( current, profilesList )
    {
      n = 0;
      keyGetString (current, value, MAX_PATH);
      keyGetName   (current, name,  MAX_PATH);

      /* 2. test if attributes matches the value of the key, count the hits */
      for (i = 0; i < 8; i++)
      {
        DBG_PROG_S (("%d: %s", i, attributs[i]))
        if (value && attributs[i] &&
            (strstr(value, attributs[i]) != 0))
        {
          if      (i == 0) n += 2;
          else if (i == 1) n += 2;
          else if (i == 2) n += 5;
          else             ++n;
          DBG_PROG_S(( "attribute count n = %d", n ))
        }
      }

      if (n >= 5)
      { /* 3. search the profile in an match list */
        int found = 0; DBG_PROG
        if (matchList)
        {
          for (testEntry=matchList->begin; testEntry; testEntry=testEntry->next)
          {
            DBG_PROG_S(( "%s %s", testEntry->name, name ))
            if (testEntry->name && strlen(name) &&
                strstr(testEntry->name, name) != 0)
            { DBG_PROG_S(( "%s", strstr(testEntry->name, name) ))
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
          DBG_PROG_S(( "new matching profile found %s", name ))
          matchList = oyAppendComp_ (matchList, 0);
          DBG_PROG_S ((printComp (matchList)))
          /* 5. increase the hits counter in the match list for that profile */
          oySetComp_ ( matchList, name, value, n );
          DBG_PROG_S ((printComp (matchList)))
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
                                    const void* mem,
                                    size_t size)
{ DBG_PROG_START
  int rc = 0;
  char* comment = 0;

  if (mem && size && profileName)
  {
    rc = oyCheckProfile_Mem (mem, size, 0); ERR
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

    rc =  oySetProfile_ (profileName, oyDEVICE_PROFILE, comment); ERR
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
  int rc=0;
  KeySet* profilesList = 0;
  Key *current;
  char* value;

  DBG_PROG

  profilesList =
   oyReturnChildrenList_(OY_REGISTRED_PROFILES, &rc ); ERR
  if(!profilesList)
  {
    oyClose_();

    DBG_PROG_ENDE
    return rc;
  }

  value = (char*) calloc (sizeof(char), MAX_PATH);
  profile_name = oyGetDeviceProfile_ (manufacturer, model, product_id,
                                      host, port, attrib1, attrib2, attrib3,
                                      oyAllocateFunc_);

  DBG_PROG_S(("profile_name %s", profile_name ))

  FOR_EACH_IN_KDBKEYSET( current, profilesList )
  {
    keyGetName(current, value, MAX_PATH);
    DBG_NUM_S(( value ))
    if(profile_name &&
       strstr(value, profile_name) != 0) {
      DBG_PROG_S((value))
      kdbRemove (value); 
      break;
    }
  }

  DBG_NUM_S(( value ))

  if(profilesList) ksClose(profilesList); DBG_PROG
  OY_FREE (value) DBG_PROG
  OY_FREE (profile_name) DBG_PROG
  oyClose_(); DBG_PROG

  DBG_PROG_ENDE
  return rc;
}



/* --- internal API decoupling --- */

#include "oyranos.h"

/** \addtogroup path_names Path Names API
 *  Functions to handle path configuration for Oyranos.

 *  @see @ref profile_lists

 *  @{
 */

/** Determin the count of configured search paths.\n */
int
oyPathsCount         (void)
{ DBG_PROG_START
  oyOpen_();
  int n = oyPathsCount_();
  if(!n)
    oyPathAdd_ (OY_PROFILE_PATH_USER_DEFAULT);
  n = oyPathsCount_();
  oyClose_();
  DBG_PROG_ENDE
  return n;
}

/** Get the path name at a certain position in the Oyranos search path list.
 *
 *  @param number        number in list - dont rely on through sessions
 *  @param allocate_func user provided function for allocating the string memory
 *  @return              the pathname at position \e number
 */
char*
oyPathName           (int           number,
                      oyAllocFunc_t allocate_func)
{ DBG_PROG_START
  oyOpen_();
  char* name = oyPathName_ (number, allocate_func);
  oyClose_();
  DBG_PROG_ENDE
  return name;
}

/** Add a path name statically to allready configured ones.
 *
 *  @param  pathname new pathname to use by Oyranos for profile search
 *  @return success
 */
int
oyPathAdd            (const char* pathname)
{ DBG_PROG_START
  oyOpen_();
  int n = oyPathAdd_ (pathname);
  oyClose_();
  DBG_PROG_ENDE
  return n;
}

/** Remove a path name statically from the list of configured ones.
 *
 *  @param  pathname the pathname to remove from the Oyranos search path list
 */
void
oyPathRemove         (const char* pathname)
{ DBG_PROG_START
  oyOpen_();
  oyPathRemove_ (pathname);
  oyClose_();
  DBG_PROG_ENDE
}

void
oyPathSleep          (const char* pathname)
{ DBG_PROG_START
  oyOpen_();
  oyPathSleep_ (pathname);
  oyClose_();
  DBG_PROG_ENDE
}

void
oyPathActivate       (const char* pathname)
{ DBG_PROG_START
  oyOpen_();
  oyPathActivate_ (pathname);
  oyClose_();
  DBG_PROG_ENDE
}

/** Find out where in the Oyranos search path the specified profile resides.
 *
 *  @param  profile_name  the filename find in the Oyranos search path
 *  @param  allocate_func user provided function for allocating the string
                          memory
 *  @return the path name where the profile was found in the oyranos search path
 */
char*
oyGetPathFromProfileName (const char* profile_name, oyAllocFunc_t allocate_func)
{ DBG_PROG_START
  oyOpen_();
  char* path_name = oyGetPathFromProfileName_ (profile_name, allocate_func);
  oyClose_();
  DBG_PROG_ENDE
  return path_name;
}

/*  @} */

/** \addtogroup default_profiles Default Profiles API
 *  Functions to handle and configure default profiles.

 *  @see @ref path_names

 *  @{
 */

/** Sets a profile, which is available in the current configured path.
 *
 *  @param  type      the kind of default profile
 *  @param  file_name the profile which shall become the default for the above
 *                    specified profile type
 *  @return success
 */
int
oySetDefaultProfile        (oyDEFAULT_PROFILE type,
                            const char*       file_name)
{ DBG_PROG_START
  oyOpen_();
  int n = oySetDefaultProfile_ (type, file_name);
  oyClose_();
  DBG_PROG_ENDE
  return n;
}

/** Sets a profile, which is available in the current configured path.
 *
 *  @param  type      the kind of default profile
 *  @param  file_name the profile which shall become the default for the above
 *          specified profile type
 *  @param  mem       the memory containing the profile
 *  @param  size      the profile size in mem
 *  @return success
 */
int
oySetDefaultProfileBlock   (oyDEFAULT_PROFILE type,
                            const char*       file_name,
                            void*             mem,
                            size_t            size)
{ DBG_PROG_START
  oyOpen_();
  int n = oySetDefaultProfileBlock_ (type, file_name, mem, size);
  oyClose_();
  DBG_PROG_ENDE
  return n;
}

/** Gets a default profile type name for presenting in a GUI.
 *
 *  @param  type the kind of default profile
 *  @return type name
 */
const char*
oyGetDefaultProfileUITitle (oyDEFAULT_PROFILE type)
{ DBG_PROG_START
  oyOpen_();
  const char* name= oyGetDefaultProfileUITitle_ (type);
  oyClose_();
  DBG_PROG_ENDE
  return name;
}

/** Gets a default profile filename.
 *
 *  @param  type          the kind of default profile
 *  @param  allocate_func the allocation function needed to reserve memory for
                          the filename string
 *  @return filename
 */
char*
oyGetDefaultProfileName    (oyDEFAULT_PROFILE type,
                            oyAllocFunc_t     allocate_func)
{ DBG_PROG_START
  oyOpen_();
  char* name = oyGetDefaultProfileName_ (type, allocate_func);
  oyClose_();
  DBG_PROG_ENDE
  return name;
}
/*  @} */


/** \addtogroup profile_lists Profile Lists API
 *  Functions to handle profile name lists.

 *  @see @ref path_names

 *  @{
 */

/** @brief get a list of profile filenames
 *  @param coloursig filter as ICC 4 byte string
 *  @param[out] size profile filenames count
 *  @return the profiles filename list allocated within Oyranos
 *
 *  @see oyProfileListFree to free lists memory after usage is completed.
 *  @see @ref path_names
 *
 *  @todo use coloursig variable; extract such information from profiles
 */
char**
oyProfileList                      (const char* coloursig, int *size)
{
  DBG_PROG_START
  oyOpen_();
  char** names = oyProfileList_(coloursig, size);
  oyClose_();
  DBG_PROG_ENDE
  return names;
}

/** @brief delete a profile names list
 *  @param list a list of profile filenames
 *  @param size number of profile filenames in the list to free
 */
void
oyProfileListFree                  (char** list, int size)
{ DBG_PROG_START
  oyProfileListFree_ (list, size);
  DBG_PROG_ENDE
}

/** @} */


/** \addtogroup profile_handling Profile Handling API
 *  Functions to handle profiles.

 *  @{
 */

/** @brief check an file if it is a profile
 *
 *  @param  filename  the filename to check
 *  @param  coloursig colour space signature as described in the
                      <a href="http://www.color.org">ICC specification</a>
 *  @return true/false
 */
int
oyCheckProfile (const char* filename, const char* coloursig)
{ DBG_PROG_START
  /* coloursig is currently ignored */
  int n = oyCheckProfile_ (filename, coloursig);
  DBG_PROG_ENDE
  return n;
}

/** @brief check an memory block if it is a profile
 *
 *  @param  mem  the memory containing the profile
 *  @param  size the size of the memory block
 *  @param  coloursig currently ignored
 *  @return true/false
 */
int
oyCheckProfileMem (const void* mem, size_t size, const char* coloursig)
{ DBG_PROG_START
  /* coloursig is currently ignored */
  int n = oyCheckProfile_Mem (mem, size, coloursig);
  DBG_PROG_ENDE
  return n;
}

size_t
oyGetProfileSize                  (const char* profilename)
{ DBG_PROG_START
  oyOpen_();
  size_t size = oyGetProfileSize_ (profilename);
  oyClose_();
  DBG_PROG_ENDE
  return size;
}

/** @brief obtain an memory block in the responsibility of the user
 *
 *  @param  profilename  specifies the profile
 *  @param[in] size      desired size, or zero for a complete copy
    @param[out] size     the size of the returned block
 *  @param      allocate_func the users memory allocation function
 *  @return             the profile block in memory allocated by oyAllocFunc_t
 */
void*
oyGetProfileBlock                 (const char* profilename, size_t *size,
                                   oyAllocFunc_t allocate_func)
{ DBG_PROG_START
  oyOpen_();
  char* block = oyGetProfileBlock_ (profilename, size, allocate_func);
  oyClose_();
  DBG_PROG_S( ("%s %hd %d", profilename, (int)block, *size) )
  DBG_PROG

  DBG_PROG_ENDE
  return block;
}

/** @} */



/** \addtogroup device_profiles Device Profiles API
 * 
 * 
 * There different approaches to select an (mostly) fitting profile
 *
 * A: search and compare all available profiles by \n
 *    - ICC profile class
 *    - Manufacturer / Model (as written in profile tags)
 *    - other hints\n
 *    .
 * B: install an profile and tell Oyranos about the belonging device and the
 *    invalidating period\n
 * C: look for similarities of devices of allready installed profiles\n
 * D: use the md5 checksum stored in the profile

 * @see @ref path_names

 * @{
 */

/** @brief ask for a profile name by specifying device attributes
 *
 *  @param typ            kind of device
 *  @param manufacturer   the device manufacturer (EIZO)
 *  @param model          the model (LCD2100)
 *  @param product_id     the ID reported during connection (ID_701200xx)
 *  @param host           useful for monitor identification (grafic:0.0)
 *  @param port           kind of connection (Matrox G650)
 *  @param attrib1        additional attribute
 * 
 *  simply pass 0 for not specified properties<br>

   \code
   char* profile_name = oyGetDeviceProfile ("EIZO", "LCD2100",
                                            "ID 87-135.19",
                                            "grafic:0.0", "Matrox G650",
                                             "100lux", 0,
                                            "", allocate_func);
   if (profile_name)
   { char* ptr = (char*)malloc (oyGetProfileSize (profile_name),sizeof(int);
     ptr = oyGetProfileBlock (profile_name, allocate_func);
       // do something
     free (ptr);
   }
   \endcode

   \return allocated by oyAllocFunc_t
 */
char*
oyGetDeviceProfile                (oyDEVICETYP typ,
                                   const char* manufacturer,
                                   const char* model,
                                   const char* product_id,
                                   const char* host,
                                   const char* port,
                                   const char* attrib1,
                                   const char* attrib2,
                                   const char* attrib3,
                                   oyAllocFunc_t allocate_func)
{ DBG_PROG_START
  oyOpen_();
  char* profile_name = oyGetDeviceProfile_ (manufacturer, model, product_id,
                                    host, port, attrib1, attrib2, attrib3,
                                    allocate_func);
  oyClose_();
  if(profile_name)
    DBG_PROG_S( (profile_name) );

  DBG_PROG_ENDE
  return profile_name;
}

/** @brief set a profile name with specifying device attributes
 *  @param mem remains in the users domain
 */
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
                                   const void* mem,
                                   size_t size)
{ DBG_PROG_START
  oyOpen_();
  int rc =     oySetDeviceProfile_ (manufacturer, model, product_id,
                                    host, port, attrib1, attrib2, attrib3,
                                    profileName, mem, size);
  oyClose_();
  DBG_PROG_ENDE
  return rc;
}

/** @brief remove or deinstall the profile from the current path */
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
  oyOpen_();
  int rc = oyEraseDeviceProfile_ (manufacturer, model, product_id,
                                    host, port, attrib1, attrib2, attrib3);
  oyClose_();
  DBG_PROG_ENDE
  return rc;
}


/** @} */
