/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2006  Kai-Uwe Behrmann
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
 */

/** @file @internal
 *  @brief input / output  methods
 */

/* Date:      25. 11. 2004 */

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_cmms.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_xml.h"

/* --- Helpers  --- */

/* --- static variables   --- */

/* --- structs, typedefs, enums --- */

/* --- internal API definition --- */

/* separate from the external functions */
int   oyPathsCount_             (void);
char* oyPathName_               (int           number,
                                 oyAllocFunc_t allocate_func);
int   oyPathAdd_                (const char* pathname);
void  oyPathRemove_             (const char* pathname);
void  oyPathSleep_              (const char* pathname);
void  oyPathActivate_           (const char* pathname);
char* oyGetPathFromProfileName_ (const char*   profilename,
                                 oyAllocFunc_t allocate_func);


/* oyranos part */
/* check for the global and the users directory */
void oyCheckDefaultDirectories_ ();
/* search in profile path and in current path */
char* oyFindProfile_ (const char* name);



/* --- Helpers  --- */
/* small helpers */
#define OY_FREE( ptr ) if(ptr) { free(ptr); ptr = 0; }
#if 1
#define ERR if (rc<=0 && oy_debug) { printf("%s:%d %d\n", __FILE__,__LINE__,rc); perror("Error"); }
#else
#define ERR
#endif


/* --- function definitions --- */


size_t
oyReadFileSize_(const char* name)
{ DBG_PROG_START
  FILE *fp = 0;
  const char* filename = name;
  size_t size = 0;

  {
    fp = fopen(filename, "r");
    DBG_PROG_S (("fp = %d filename = %s\n", (int)(intptr_t)fp, filename))

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
    DBG_PROG_S (("fp = %u filename = %s\n", (unsigned int)((intptr_t)fp), filename))

    if (fp)
    {
      /* get size */
      fseek(fp,0L,SEEK_END); 
      /* read file possibly partitial */
      if(!*size || *size > ftell(fp))
        *size = ftell (fp);
      rewind(fp);

      DBG_PROG_S(("%u\n",((unsigned int)((size_t)size))));

      /* allocate memory */
      mem = (char*) calloc (*size+1, sizeof(char));

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
          mem = allocate_func(*size+1);
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

  DBG_PROG_S(("name = %s mem = %d size = %d\n", name, (int)(intptr_t)mem, (int)(intptr_t)size))

  filename = name;

  {
    fp = fopen(filename, "w");
    DBG_PROG_S(("fp = %d filename = %s", (int)(intptr_t)fp, filename))
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
# if (__WINDOWS__)
  DBG_PROG_ENDE
  return "OS not supported yet";
# else
  char* name = (char*) getenv("HOME");
  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
# endif
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
      WARN_S((_("file name is too long %d\n"), len))

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




/* public API implementation */

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
oyProfileListGet_                  (const char* coloursig, int * size)
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

  static int war = 0;

  ++war;
  if(war >= 413)
    ;/* WARN_S(("schon %d mal\n", war)); */

  for(i = 0; i < count ; ++i)
  {
    struct stat statbuf;
    struct dirent *entry[MAX_DEPTH];
    DIR *dir[MAX_DEPTH];
    char *p = NULL;
    char *path = NULL;
    int l = 0; /* level */
    int run = !r;
    int path_is_double = 0;
    int j;

    p = oyPathName_(i, oyAllocateFunc_);
    path = oyMakeFullFileDirName_(p);
    if(!p || !strlen(p))
      WARN_S(("no path given on pos %d\n", i))
    OY_FREE( p );

    /* check for doubling of paths, without checking recursively */
    {
      for( j = 0; j < i; ++j )
      { char *p  = oyPathName_( j, oyAllocateFunc_);
        char *pp = oyMakeFullFileDirName_( p );
        if( p && pp &&
            (strstr( path, pp ) != 0) )
          path_is_double = 1;
        OY_FREE( p );
        OY_FREE( pp );
      }
      for( j = 0; j < MAX_DEPTH; ++j )
      {
        dir[j] = NULL;
        entry[j] = NULL;
      }
    }
    if( path_is_double )
      continue;

    if ((stat (path, &statbuf)) != 0) {
      WARN_S((_("%d. path %s does not exist"), i, path))
      continue;
    }
    if (!S_ISDIR (statbuf.st_mode)) {
      WARN_S((_("%d. path %s is not a directory"), i, path));
      continue;
    }
    if (S_ISLNK (statbuf.st_mode)) {
      WARN_S((_("%d. path %s is a link: ignored"), i, path));
      continue;
    }
    dir[l] = opendir (path);
    if (!dir[l]) {
      WARN_S((_("%d. path %s is not readable"), i, path));
      continue;
    }

    while(run)
    {
      if(l>=64) WARN_S(("max path depth reached: 64"));
      if(dir[l] == NULL)
      {
        WARN_S(("NULL\n"));
        --l;
        if(l<0) run = 0;
        goto cont;
      }
      if(!(entry[l] = readdir (dir[l]))) {
        closedir(dir[l]);
        dir[l] = NULL;
        --l;
        if(l<0)
          run = 0;
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
        DBG_PROG_S(("%d. %s is a link: ignored %d %d %d", l, name, (int)statbuf.st_mode , S_IFLNK, 0120000));
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

      cont:
        ;
    }

    for( j = 0; j < MAX_DEPTH; ++j )
      {
        if(dir[j]) closedir(dir[j]);;
        dir[j] = NULL;
      }
    OY_FREE(path)
  }

  DBG_PROG_ENDE
  return r;
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
    WARN_S (("False profile - size = %d pos = %lu ", (int)size, (long int)block))

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



