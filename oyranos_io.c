/** @file oyranos_io.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    input / output  methods
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */

#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_check.h"
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

/* search in profile path and in current path */
char* oyFindProfile_ (const char* name);



/* --- Helpers  --- */
/* small helpers */

/* --- function definitions --- */


size_t
oyReadFileSize_(const char* name)
{
  FILE *fp = 0;
  const char* filename = name;
  size_t size = 0;

  DBG_MEM_START

  {
    fp = fopen(filename, "rb");
    DBG_MEM2_S ("fp = %d filename = %s\n", (int)(intptr_t)fp, filename)

    if (fp)
    {
      /* get size */
      fseek(fp,0L,SEEK_END); 
      size = ftell (fp);
      fclose (fp);

    } else
      WARNc1_S( "could not read %s", filename );
  }

  DBG_MEM_ENDE
  return size;
}

char*
oyReadFileToMem_(const char* name, size_t *size,
                 oyAlloc_f     allocate_func)
{
  FILE *fp = 0;
  char* mem = 0;
  const char* filename = name;

  DBG_MEM_START

  DBG_MEM

  {
    fp = fopen(filename, "rb");
    DBG_MEM2_S ("fp = %u filename = %s\n", (unsigned int)((intptr_t)fp), filename)

    if (fp)
    {
      /* get size */
      fseek(fp,0L,SEEK_END); 
      /* read file possibly partitial */
      if(!*size || *size > ftell(fp))
        *size = ftell (fp);
      rewind(fp);

      DBG_MEM1_S("%u\n",((unsigned int)((size_t)size)));

      /* allocate memory */
      oyAllocHelper_m_( mem, char, *size+1, oyAllocateFunc_, return 0);

      /* check and read */
      if ((fp != 0)
       && mem
       && *size)
      {
        int s = fread(mem, sizeof(char), *size, fp);

        DBG_MEM

        /* check again */
        if (s != *size)
        { *size = 0;
          oyFree_m_ (mem)
          mem = 0;
        } else {
          /* copy to external allocator */
          char* temp = mem;
          mem = oyAllocateWrapFunc_( *size+1, allocate_func );
          if(mem) {
            memcpy( mem, temp, *size );
            oyFree_m_ (temp)
            mem[*size] = 0;
          } else {
            oyFree_m_ (mem)
            *size = 0;
          }
        }
      }
    } else {
      WARNc1_S( "could not read: \"%s\"\n", filename );
    }
  }
 
  /* clean up */
  if (fp) fclose (fp);

  DBG_MEM_ENDE
  return mem;
}

int
oyWriteMemToFile_(const char* name, const void* mem, size_t size)
{
  FILE *fp = 0;
  /*int   pt = 0;
  char* block = mem;*/
  const char* filename;
  char * full_name = 0;
  int r = !name;
  size_t written_n = 0;
  char * path = 0;

  DBG_PROG_START

  DBG_PROG3_S("name = %s mem = %d size = %d\n", name, (int)(intptr_t)mem, (int)(intptr_t)size)

  filename = name;

  if(!r)
  {
    full_name = oyResolveDirFileName_( filename );
    path = oyExtractPathFromFileName_( full_name );
    r = oyMakeDir_( path );
  }

  {
    fp = fopen(full_name, "wb");
    DBG_PROG2_S("fp = %d filename = %s", (int)(intptr_t)fp, filename)
    if ((fp != 0)
     && mem
     && size)
    {
#if 0
      do {
        r = fputc ( block[pt++] , fp);
      } while (--size);
#else
      written_n = fwrite( mem, size, 1, fp );
      if(written_n != size)
        r = 1;
#endif
    } else
    {
      r = errno;
      switch (errno)
      {
        case EACCES:       WARNc1_S("Permission denied: %s", filename); break;
        case EIO:          WARNc1_S("EIO : %s", filename); break;
        case ELOOP:        WARNc1_S("Too many symbolic links encountered while traversing the path: %s", filename); break;
        case ENAMETOOLONG: WARNc1_S("ENAMETOOLONG : %s", filename); break;
        case ENOENT:       WARNc1_S("A component of the path/file_name does not exist, or the file_name is an empty string: \"%s\"", filename); break;
        case ENOTDIR:      WARNc1_S("ENOTDIR : %s", filename); break;
        case EOVERFLOW:    WARNc1_S("EOVERFLOW : %s", filename); break;
      }
    }

    if (fp) fclose (fp);
  }

  if(path) oyDeAllocateFunc_( path );
  if(full_name) oyDeAllocateFunc_( full_name );

  DBG_PROG_ENDE
  return r;
}

/* TODO: support flags - OY_FILE_APPEND */
int  oyWriteMemToFile2_              ( const char        * name,
                                       void              * mem,
                                       size_t              size,
                                       uint32_t            flags,
                                       char             ** result,
                                       oyAlloc_f           allocateFunc )
{
  int error = 0;
  const char * filename = name,
             * tmp_dir = 0;
  char * filename_tmp = 0;
  char * full_name = 0;
  char * mode = "wb";
  int exist = 0, pos = 1;
  char * tmp = 0;

  if(!name)
    return 1;

  if(flags & OY_FILE_APPEND)
    mode = "ab";

  if(flags & OY_FILE_TEMP_DIR)
  {
    if(getenv("TMP") && strlen(getenv("TMP")))
      tmp_dir = getenv("TMP");
    else
    if(getenv("TEMP") && strlen(getenv("TEMP")))
      tmp_dir = getenv("TEMP");
    else
    if(getenv("TMPDIR") && strlen(getenv("TMPDIR")))
      tmp_dir = getenv("TMPDIR");
    else
      tmp_dir = "/tmp";

    STRING_ADD( filename_tmp, tmp_dir );
    if(filename_tmp[oyStrlen_(filename_tmp)] != '/')
      STRING_ADD( filename_tmp, "/" );
    STRING_ADD( filename_tmp, filename );
    full_name = filename_tmp;
    filename = full_name;
  }

  if(!full_name)
    full_name = oyResolveDirFileName_( filename );
  exist = oyIsFile_(full_name);
  if(exist &&
     !(flags & OY_FILE_APPEND) && !(flags & OY_FILE_NAME_SEARCH))
  {
    WARNc2_S( "%s: %s", _("File exists"), full_name );
    return 1;
  }

  if(exist && flags & OY_FILE_NAME_SEARCH && !(flags & OY_FILE_APPEND))
  {
    char * end = 0;
    char * num = 0;
    char * format = oyAllocateFunc_( 32 );
    char * ext = 0;
    int digits = 3;
    int max = 1000;

    /* allocate memory */
    oyAllocHelper_m_( tmp, char, strlen(full_name)+12, oyAllocateFunc_, return 1);

    oySprintf_( tmp, "%s", full_name );
    ext = end = oyStrrchr_( tmp, '.' );
    if(!end)
      end = tmp + oyStrlen_( tmp );
    num = end;
    while(isdigit( num[0] ) && num != tmp) --num;
    if(end - num)
      digits = (int) (end - num);
    /* move the extension */
    else if(ext)
      memmove( &ext[digits], ext, digits + 1 );
    /* settle with a new format */
    sprintf( format, "%%0%dd", digits );
    pos += atoi(num);
    /* write a new number */
    sprintf( num, format, pos );
    if(ext)
      tmp[oyStrlen_(tmp)] = '.';

    /* search a non existing file name */
    while(oyIsFile_(tmp))
    {
      sprintf( num, format, pos++ );
      if(ext)
        tmp[oyStrlen_(tmp)] = '.';
    }
    max = (int)pow( 10, digits);
    if(pos >= max)
    {
      WARNc2_S( "%s: %s", _("File exists"), full_name );
      return 1;
    }

    if(result)
      *result = oyStringCopy_( tmp, allocateFunc?allocateFunc:oyAllocateFunc_ );

    filename = tmp;
    if(format) oyFree_m_(format)
  }

  if(!error)
    error = oyWriteMemToFile_( filename, mem, size );

  if(tmp) oyFree_m_(tmp)

  return error;
}

char*
oyGetHomeDir_ ()
{
# if (__WINDOWS__)
  DBG_PROG_START
  DBG_PROG_ENDE
  WARNc_S("OS not supported yet");
  return 0;
# else
  char* name = (char*) getenv("HOME");

  DBG_PROG_START

  if(!name)
    WARNc_S("Could not get \"HOME\" directory name");

  DBG_PROG_ENDE
  return name;
# endif
}

char*
oyPathGetParent_ (const char* name)
{
  char *parentDir = 0, *ptr = 0;

  DBG_PROG_START

  oyAllocString_m_( parentDir, MAX_PATH,
                    oyAllocateFunc_, return 0 );

  oySprintf_ (parentDir, "%s", name);
  ptr = strrchr( parentDir, OY_SLASH_C);
  if (ptr)
  {
    if (ptr[1] == 0) /* ending dir separator */
    {
      ptr[0] = 0;
      if (strrchr( parentDir, OY_SLASH_C))
      {
        ptr = strrchr (parentDir, OY_SLASH_C);
        ptr[0+1] = 0;
      }
    }
    else
      ptr[0+1] = 0;
  }

  DBG_PROG_S(parentDir)

  DBG_PROG_ENDE
  return parentDir;
}

int
oyIsDir_ (const char* path)
{
  struct stat status;
  int r = 0;
  char* name = oyResolveDirFileName_ (path);

  DBG_MEM_START

  status.st_mode = 0;
  r = stat (name, &status);
  DBG_MEM1_S("status.st_mode = %d", (int)(status.st_mode&S_IFMT)&S_IFDIR)
  DBG_MEM1_S("status.st_mode = %d", (int)status.st_mode)
  DBG_MEM1_S("name = %s ", name)
  oyFree_m_ (name)
  r = !r &&
       ((status.st_mode & S_IFMT) & S_IFDIR);

  DBG_MEM_ENDE
  return r;
}

int
oyIsFileFull_ (const char* fullFileName)
{
  struct stat status;
  int r = 0;
  const char* name = fullFileName;

  DBG_MEM_START

  DBG_MEM1_S("fullFileName = \"%s\"", fullFileName)
  status.st_mode = 0;
  r = stat (name, &status);

  DBG_MEM1_S("status.st_mode = %d", (int)(status.st_mode&S_IFMT)&S_IFDIR)
  DBG_MEM1_S("status.st_mode = %d", (int)status.st_mode)
  DBG_MEM1_S("name = %s", name)
  DBG_MEM_V( r )
  switch (r)
  {
    case EACCES:       WARNc1_S("EACCES = %d\n",r); break;
    case EIO:          WARNc1_S("EIO = %d\n",r); break;
    case ELOOP:        WARNc1_S("ELOOP = %d\n",r); break;
    case ENAMETOOLONG: WARNc1_S("ENAMETOOLONG = %d\n",r); break;
    case ENOENT:       WARNc1_S("ENOENT = %d\n",r); break;
    case ENOTDIR:      WARNc1_S("ENOTDIR = %d\n",r); break;
    case EOVERFLOW:    WARNc1_S("EOVERFLOW = %d\n",r); break;
  }

  r = !r &&
       (   ((status.st_mode & S_IFMT) & S_IFREG)
        || ((status.st_mode & S_IFMT) & S_IFLNK));

  DBG_MEM_V( r )
  if (r)
  {
    FILE* fp = fopen (name, "rb");
    if (!fp) { DBG_PROG1_S("not existent: %s", name )
      r = 0;
    } else { DBG_MEM_S(name)
      fclose (fp);
    }
  }

  DBG_MEM_ENDE
  return r;
}

int
oyIsFile_ (const char* fileName)
{
  int r = 0;
  char* name = oyResolveDirFileName_ (fileName);

  DBG_MEM_START

  r = oyIsFileFull_(name);

  oyFree_m_ (name) DBG_MEM

  DBG_MEM_ENDE
  return r;
}

int
oyMakeDir_ (const char* path)
{
  char * full_name = oyResolveDirFileName_ (path),
       * path_parent = 0,
       * path_name = 0;
  int rc = !full_name;

  mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; /* 0755 */

  DBG_PROG_START


  if(full_name)
    path_name = oyExtractPathFromFileName_(full_name);
  if(path_name)
  {
    if(!oyIsDir_(path_name))
    {
      path_parent = oyPathGetParent_(path_name);
      if(!oyIsDir_(path_parent))
      {
        rc = oyMakeDir_(path_parent);
        oyDeAllocateFunc_( path_parent );
      }

      rc = mkdir (path_name, mode);
      if(rc)
      switch (errno)
      {
        case EACCES:       WARNc1_S("Permission denied: %s", path); break;
        case EIO:          WARNc1_S("EIO : %s", path); break;
        case ELOOP:        WARNc1_S("Too many symbolic links encountered while traversing the path: %s", path); break;
        case ENAMETOOLONG: WARNc1_S("ENAMETOOLONG : %s", path); break;
        case ENOENT:       WARNc1_S("A component of the path/file_name does not exist, or the file_name is an empty string: \"%s\"", path); break;
        case ENOTDIR:      WARNc1_S("ENOTDIR : %s", path); break;
        case EOVERFLOW:    WARNc1_S("EOVERFLOW : %s", path); break;
      }
    }
  }

  oyFree_m_ (full_name)

  DBG_PROG_ENDE
  return rc;
}

int  oyRemoveFile_                   ( const char        * full_file_name )
{
  return remove( full_file_name );
}

char*
oyResolveDirFileName_ (const char* name)
{
  char * newName = NULL,
       * home = NULL;
  int len = 0;

  DBG_MEM_START

  DBG_MEM_S(name)

  /* user directory */
  if (name[0] == '~')
  {
    home = oyGetHomeDir_();
    len = strlen(name) + strlen(home) + 1;
    oyAllocHelper_m_( newName, char, len + 2, oyAllocateFunc_, fprintf(stderr,"oyranos_io.c:367 oyResolveDirFileName_() Could not allocate enough memory.\n"); return 0 );

    oySprintf_ (newName, "%s%s", home, &name[0]+1);

  } else {
    oyAllocHelper_m_( newName, char, MAX_PATH, oyAllocateFunc_, fprintf(stderr,"oyranos_io.c:371 oyResolveDirFileName_() Could not allocate enough memory.\n"); return 0 );
    oySprintf_ (newName, "%s", name);

    /* relative names - where the first sign is no directory separator */
    if (newName[0] != OY_SLASH_C)
    {
      char* cn = 0;

      oyAllocHelper_m_( cn, char, MAX_PATH, oyAllocateFunc_, fprintf(stderr,"oyranos_io.c:379 oyResolveDirFileName_() Could not allocate 4096 byte of memory.\n"); return 0 );
      oySprintf_ (cn, "%s%s%s", getenv("PWD"), OY_SLASH, name);
      DBG_MEM1_S("canonoical %s ", cn)
      oySprintf_ (newName, "%s", cn);
      if(cn) oyDeAllocateFunc_(cn); cn = 0;
    }
  }

  if(name)
    DBG_MEM1_S ("name %s", name);
  if(home)
    DBG_MEM1_S ("home %s", home);
  if(newName)
    DBG_MEM1_S ("newName = %s", newName);

  DBG_MEM_ENDE
  return newName;
}

char*
oyExtractPathFromFileName_ (const char* file_name)
{
  char *path_name = 0;
  char *ptr;

  DBG_MEM_START

  oyAllocString_m_( path_name, strlen(file_name)+1,
                    oyAllocateFunc_, return 0 );

  oySprintf_( path_name, "%s", file_name );
  DBG_MEM1_S ("path_name = %s", path_name)
  ptr = strrchr (path_name, '/');
  ptr[0+1] = 0;
  DBG_MEM1_S ("path_name = %s", path_name)
  DBG_MEM1_S ("ptr = %s", ptr)
  DBG_MEM_ENDE
  return path_name;
}

char*
oyMakeFullFileDirName_ (const char* name)
{
  char *newName = 0;
  char *dirName = 0;

  DBG_MEM_START

  DBG_MEM
  if(name &&
     strrchr( name, OY_SLASH_C ))
  { DBG_MEM
    /* substitute ~ with HOME variable from environment */
    newName = oyResolveDirFileName_ (name);
  } else
  { DBG_MEM
    /* create directory name */
    oyAllocString_m_( newName, MAX_PATH,
                      oyAllocateFunc_, return 0 );
    dirName = (char*) getenv("PWD");
    oySprintf_ (newName, "%s%s", dirName, OY_SLASH);
    if (name)
      oySprintf_ (strrchr(newName,OY_SLASH_C)+1, "%s", name);
    DBG_MEM1_S("newName = %s", newName)
  }

  DBG_MEM1_S("newName = %s", newName)

  DBG_MEM_ENDE
  return newName;
}


char*
oyFindProfile_ (const char* fileName)
{
  char  *fullFileName = 0;
  int len = 0;

  DBG_PROG_START

  if(!fileName || !fileName[0])
    return fullFileName;

  /*DBG_NUM_S((fileName)) */
  if (fileName && !strchr(fileName, OY_SLASH_C))
  {
    char* path_name = oyGetPathFromProfileName_(fileName, oyAllocateFunc_);

    DBG_PROG
    oyAllocString_m_( fullFileName, MAX_PATH,
                      oyAllocateFunc_, return 0 );
    if(path_name)
    {
      oySprintf_(fullFileName, "%s%s%s", path_name, OY_SLASH, fileName);
      oyFree_m_(path_name)
    } else
      oySprintf_(fullFileName, "%s", fileName);
    DBG_PROG_S( fullFileName )
  } else
  {
    if (oyIsFileFull_(fileName)) {
      oyAllocString_m_( fullFileName, MAX_PATH,
                        oyAllocateFunc_, return 0 );
      len = oyStrlen_(fileName);
      memcpy(fullFileName, fileName, len), fullFileName[len] = 0;
    } else
      fullFileName = oyMakeFullFileDirName_ (fileName);
  }

  DBG_PROG_ENDE
  return fullFileName;
}




/* public API implementation */

/* profile and other file lists API */

#define MAX_DEPTH 64

int oyProfileListCb_ (oyFileList_s * data,
                      const char* full_name, const char* filename)
{
  oyFileList_s *l = (oyFileList_s*)data;

  if(l->type != oyOBJECT_FILE_LIST_S_)
    WARNc_S("Could not find a oyFileList_s objetc.");

      if (!oyCheckProfile_(full_name, l->coloursig))
      {
        if(l->count_files >= l->mem_count)
        {
          char** temp = l->names;

          l->names = 0;
          oyAllocHelper_m_( l->names, char*, l->mem_count+l->hopp,
                            oyAllocateFunc_, return 1);
          memcpy(l->names, temp, sizeof(char*) * l->mem_count);
          l->mem_count += l->hopp;
        }

        oyAllocString_m_( l->names[l->count_files], oyStrblen_(filename) + 1,
                          oyAllocateFunc_, return 1 );
        strcpy(l->names[l->count_files], filename);
        ++l->count_files;
      }

  return 0;
}

int oyPolicyListCb_ (oyFileList_s * data,
                     const char* full_name, const char* filename)
{
  oyFileList_s *l = (oyFileList_s*)data;
  /* last 4 chars */
  const char * end = NULL;

  if(l->type != oyOBJECT_FILE_LIST_S_)
    WARNc_S("Could not find a oyFileList_s objetc.");

  if(strlen(full_name) > 4)
    end = full_name + strlen(full_name) - 4;

      if( (end[0] == '.') &&
          (end[1] == 'x' || end[1] == 'X') &&
          (end[2] == 'm' || end[2] == 'M') &&
          (end[3] == 'l' || end[3] == 'L') &&
          !oyCheckPolicy_(full_name) )
      {
        if(l->count_files >= l->mem_count)
        {
          char** temp = l->names;

          l->names = 0;
          oyAllocHelper_m_( l->names, char*, l->mem_count+l->hopp,
                            oyAllocateFunc_, return 1);
          memcpy(l->names, temp, sizeof(char*) * l->mem_count);
          l->mem_count += l->hopp;
        }

        oyAllocString_m_( l->names[l->count_files], oyStrblen_(full_name),
                          oyAllocateFunc_, return 1 );
        
        oyStrcpy_(l->names[l->count_files], full_name);
        ++l->count_files;
      } /*else */
        /*WARNc_S(("%s in %s is not a valid profile", file_name, path)); */
  return 0;
}

int oyFileListCb_ ( oyFileList_s * data,
                    const char * full_name, const char * filename)
{
  oyFileList_s *l = (oyFileList_s*)data;

  if(l->type != oyOBJECT_FILE_LIST_S_)
    WARNc_S("Could not find a oyFileList_s objetc.");

  {
        if(l->count_files >= l->mem_count)
        {
          char** temp = l->names;

          l->names = 0;
          oyAllocHelper_m_( l->names, char*, l->mem_count+l->hopp,
                            oyAllocateFunc_, return 1);
          memcpy(l->names, temp, sizeof(char*) * l->mem_count);
          l->mem_count += l->hopp;
        }

        oyAllocString_m_( l->names[l->count_files], oyStrblen_(full_name),
                          oyAllocateFunc_, return 1 );
        
        oyStrcpy_(l->names[l->count_files], full_name);
        ++l->count_files;
  }

  return 0;
}

char **  oyProfileListGet_           ( const char        * coloursig,
                                       uint32_t          * size )
{
  oyFileList_s l = {oyOBJECT_FILE_LIST_S_, 128, NULL, 128, 0, 0};
  int32_t count = 0;/*oyPathsCount_();*/
  char ** path_names = NULL;

  path_names = oyProfilePathsGet_( &count, oyAllocateFunc_ );

  l.coloursig = coloursig;

  DBG_PROG_START
 
  oy_warn_ = 0;

  l.names = 0;
  l.mem_count = l.hopp;
  l.count_files = 0;

  oyAllocHelper_m_(l.names, char*, l.mem_count, oyAllocateFunc_, return 0);

  oyRecursivePaths_( oyProfileListCb_, &l,
                     (const char**)path_names, count );

  oyStringListRelease_( &path_names, count, oyDeAllocateFunc_ );

  *size = l.count_files;
  oy_warn_ = 1;
  DBG_PROG_ENDE
  return l.names;
}

char**
oyPolicyListGet_                  (int * size)
{
  oyFileList_s l = {oyOBJECT_FILE_LIST_S_, 128, NULL, 128, 0, 0};
  int count = 0;
  char ** path_names = NULL;
 
  DBG_PROG_START
 
  path_names = oyConfigPathsGet_( &count, "settings", oyALL, oyUSER_SYS,
                                  oyAllocateFunc_ );

  oy_warn_ = 0;

  l.names = 0;
  l.mem_count = l.hopp;
  l.count_files = 0;

  oyAllocHelper_m_(l.names, char*, l.mem_count, oyAllocateFunc_, return 0);

  oyRecursivePaths_(oyPolicyListCb_, &l,(const char**)path_names, count);

  oyStringListRelease_(&path_names, count, oyDeAllocateFunc_);

  *size = l.count_files;
  oy_warn_ = 1;
  DBG_PROG_ENDE
  return l.names;
}

/** @internal
 *  @brief get files out of the Oyranos default paths
 *
 *  Since: 0.1.8
 */
char**
oyFileListGet_                  (const char * subpath,
                                 int        * size,
                                 int          data,
                                 int          owner)
{
  oyFileList_s l = {oyOBJECT_FILE_LIST_S_, 128, NULL, 128, 0, 0};
  int count = 0;
  char ** path_names = NULL;
 
  DBG_PROG_START
 
  path_names = oyConfigPathsGet_( &count, subpath, oyALL, oyUSER_SYS,
                                  oyAllocateFunc_ );

  oy_warn_ = 0;

  l.names = 0;
  l.mem_count = l.hopp;
  l.count_files = 0;

  oyAllocHelper_m_(l.names, char*, l.mem_count, oyAllocateFunc_, return 0);

  oyRecursivePaths_(oyFileListCb_, &l, (const char**)path_names, count);

  oyStringListRelease_(&path_names, count, oyDeAllocateFunc_);

  *size = l.count_files;

  if(!l.count_files && l.names)
  {
    oyDeAllocateFunc_(l.names);
    l.names = 0;
  }

  oy_warn_ = 1;
  DBG_PROG_ENDE
  return l.names;
}

/** @internal
 *  @brief get files out of the Oyranos default paths
 *
 *  Since: 0.1.8
 */
char**
oyLibListGet_                   (const char * subpath,
                                 int        * size,
                                 int          owner)
{
  struct oyFileList_s l = {oyOBJECT_FILE_LIST_S_, 128, NULL, 128, 0, 0};
  int count = 0;
  char ** path_names = NULL;
 
  DBG_PROG_START
 
  path_names = oyLibPathsGet_( &count, subpath, oyUSER_SYS,
                                  oyAllocateFunc_ );

  oy_warn_ = 0;

  l.names = 0;
  l.mem_count = l.hopp;
  l.count_files = 0;

  oyAllocHelper_m_(l.names, char*, l.mem_count, oyAllocateFunc_, return 0);

  oyRecursivePaths_(oyFileListCb_, &l, (const char**)path_names, count);

  oyStringListRelease_(&path_names, count, oyDeAllocateFunc_);

  *size = l.count_files;
  oy_warn_ = 1;
  DBG_PROG_ENDE
  return l.names;
}

typedef int (*pathSelect_f_)(oyFileList_s*,const char*,const char*);

int oyStrcmpWrap( const void * a, const void * b)
{
  const char ** ca = (const char**)a, ** cb = (const char**)b;
  return strcmp(*ca,*cb);
}

/** @internal
 *  doInPath and data must fit, doInPath can operate on data and after finishing
 *  oyRecursivePaths_ data can be processed further

 * TODO: move specifying paths out as arguments

 */
int
oyRecursivePaths_  ( pathSelect_f_ doInPath,
                     oyFileList_s * data,
                     const char ** path_names,
                     int count )
{
  int r = 0;
  int i;

  static int war = 0;

  DBG_MEM_START

  ++war;
  if(war >= 413)
    ;/* WARNc_S(("schon %d mal\n", war)); */

  for(i = 0; i < count ; ++i)
  {
    struct stat statbuf;
    struct dirent *entry[MAX_DEPTH];
    DIR *dir[MAX_DEPTH];
    const char *path = path_names[i];
    int l = 0; /* level */
    int run = !r;
    int path_is_double = 0;
    int j;

    /* check for doubling of paths, without checking recursively */
    {
      for( j = 0; j < i; ++j )
      { const char *p  = path_names[j];
        char *pp = oyMakeFullFileDirName_( p );
        if( p && pp &&
            (strcmp( path, pp ) == 0) )
          path_is_double = 1;
        oyFree_m_( pp );
      }
      memset(dir, 0, sizeof(DIR*) * MAX_DEPTH); 
      memset(entry, 0, sizeof(struct dirent*) * MAX_DEPTH);
    }

    if( path_is_double )
      continue;

    if ((stat (path, &statbuf)) != 0) {
      switch (errno)
      {
        case EACCES:       WARNc2_S("Permission denied: %s %d", path, i); break;
        case EIO:          WARNc2_S("EIO : %s %d", path, i); break;
        case ELOOP:        WARNc2_S("Too many symbolic links encountered while traversing the path: %s %d", path, i); break;
        case ENAMETOOLONG: WARNc2_S("ENAMETOOLONG : %s %d", path, i); break;
        case ENOENT:       DBG_MEM2_S("A component of the path file_name does not exist, or the path is an empty string: \"%s\" %d", path, i); break;
        case ENOTDIR:      WARNc2_S("ENOTDIR : %s %d", path, i); break;
        case EOVERFLOW:    WARNc2_S("EOVERFLOW : %s %d", path, i); break;
      }
      continue;
    }
    if (!S_ISDIR (statbuf.st_mode)) {
      WARNc3_S("%d. \"%s\" %s", i, path, _("path is not a directory"));
      continue;
    }
    if (S_ISLNK (statbuf.st_mode)) {
      WARNc3_S("%d. \"%s\" %s", i, path, _("path is a link: ignored"));
      continue;
    }
    dir[l] = opendir (path);
    if (!dir[l]) {
      WARNc3_S("%d. \"%s\" %s", i, path, _("path is not readable"));
      continue;
    }

    while(run)
    {
      char name[256];
      int k;

      if(l>=64) WARNc_S("max path depth reached: 64");
      if(dir[l] == NULL)
      {
        WARNc_S("NULL");
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

      oySprintf_(name, "%s",path);
      for (k=0; k <= l; ++k) {
        int len = strlen(name);
        if(len+strlen(entry[k]->d_name) < 256)
          oySprintf_(&name[strlen(name)],"/%s", entry[k]->d_name);
        else {
          DBG_MEM3_S("%d. %s/%s ignored", l, name, entry[k]->d_name)
          goto cont;
        }
      }

      if ((strcmp (entry[l]->d_name, "..") == 0) ||
          (strcmp (entry[l]->d_name, ".") == 0)) {
        DBG_MEM2_S("%d. %s ignored", l, name)
        goto cont;
      }
      if ((stat (name, &statbuf)) != 0) {
        DBG_MEM2_S("%d. %s does not exist", l, name)
        goto cont;
      }
      if (!S_ISLNK(statbuf.st_mode)){/*((statbuf.st_mode & S_IFMT) & S_IFLNK))  */
        DBG_MEM5_S("%d. %s is a link: ignored %d %d %d", l, name, (int)statbuf.st_mode , S_IFLNK, 0120000);
        /*goto cont; */
      }
      if (S_ISDIR (statbuf.st_mode) &&
          l < MAX_DEPTH ) {

        dir[l+1] = opendir (name);
        ++l;
        DBG_MEM2_S("%d. %s directory", l, name);
        goto cont;
      }
      if(!S_ISREG (statbuf.st_mode)) {
        DBG_MEM2_S("%d. %s is a non regular file", l, name);
        goto cont;
      }
      DBG_MEM2_S( "%d. a valid file %s", l, name )

      /* use all file extensions */
      /* go recursively without following links, due to security */
      if( !r ) {
        r = doInPath(data, name, entry[l]->d_name);
        run = !r;
        if(r)
          DBG_MEM3_S("%d. %d %d found", i, r, run);
      }

      cont:
        ;
    }

    for( j = 0; j < MAX_DEPTH; ++j ) { if(dir[j]) closedir(dir[j]); dir[j] = NULL; }

    qsort( data->names, data->count_files, sizeof(char*),
           oyStrcmpWrap );
  }

  DBG_MEM_ENDE
  return r;
}

/* profile handling API */

size_t
oyGetProfileSize_                  (const char* profilename)
{
  size_t size = 0;
  char* fullFileName = oyFindProfile_ (profilename);

  DBG_PROG_START

  size = oyReadFileSize_ (fullFileName);

  DBG_PROG_ENDE
  return size;
}

void*
oyGetProfileBlock_                 (const char* profilename, size_t *size,
                                    oyAlloc_f     allocate_func)
{
  char* fullFileName = 0;
  char* block = 0;

  DBG_PROG_START

  fullFileName = oyFindProfile_ (profilename);

  if(fullFileName)
  {
    block = oyReadFileToMem_ (fullFileName, size, allocate_func);
    oyFree_m_( fullFileName );
  }

  DBG_PROG_ENDE
  return block;
}



