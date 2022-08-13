/** @file oyjl_io.c
 *
 *  oyjl - file i/o and other basic helpers
 *
 *  @par Copyright:
 *            2016-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl core functions
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *
 * Copyright (c) 2004-2022  Kai-Uwe Behrmann  <ku.b@gmx.de>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <ctype.h>   /* isspace() tolower() isdigit() */
#include <math.h>    /* NAN */
#include <stdarg.h>  /* va_list */
#include <stddef.h>  /* ptrdiff_t size_t */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>   /* time_t localtime() */
#include <unistd.h>
#include <errno.h>
#include <wchar.h>  /* wcslen() */

#include "oyjl.h"
#include "oyjl_macros.h"
#include "oyjl_i18n_internal.h"
#include "oyjl_tree_internal.h"
#include "oyjl_version.h"

/* --- IO_Section --- */

/** \addtogroup oyjl_core
 *  @{ *//* oyjl_core */
/** \addtogroup oyjl_io
 *  @{ *//* oyjl_io */

/** @brief read FILE into memory
 *
 *  allocators are malloc()/realloc()
 */
char *     oyjlReadFileStreamToMem   ( FILE              * fp,
                                       int               * size )
{
  size_t mem_size = 256;
  char* mem;
  int c;

  if(!fp) return NULL;

  mem = (char*) malloc(mem_size+1);
  if(!mem) return NULL;

  if(size)
  {
    *size = 0;
    do
    {
      c = getc(fp);
      if(*size >= (int)mem_size)
      {
        mem_size *= 2;
        mem = (char*) realloc( mem, mem_size+1 );
        if(!mem) { *size = 0; return NULL; }
      }
      mem[(*size)++] = c;
    } while(!feof(fp));

    --*size;
    mem[*size] = '\000';
  }

  return mem;
}

#define WARNc_S(...) oyjlMessage_p( oyjlMSG_ERROR, 0, __VA_ARGS__ )
#include <errno.h>
/** @brief read local file into memory
 *
 *  uses malloc()
 */
char *    oyjlReadFile( const char * file_name,
                        int        * size_ptr )
{
  FILE * fp = NULL;
  int size = 0, s = 0;
  char * text = NULL;

  if(file_name)
  {
    fp = fopen(file_name,"rb");
    if(fp)
    {
      fseek( fp, 0L, SEEK_END );
      size = ftell( fp );
      if(size == -1)
      {
        switch(errno)
        {
          case EBADF:        WARNc_S("Not a seekable stream %d", errno); break;
          case EINVAL:       WARNc_S("Wrong argument %d", errno); break;
          default:           WARNc_S("%s", strerror(errno)); break;
        }
        if(size_ptr)
          *size_ptr = size;
        fclose( fp );
        return NULL;
      }
      rewind(fp);
      text = (char*) malloc(size+1);
      if(text == NULL)
      {
        WARNc_S( "Could allocate memory: %lu", (long unsigned int)size);
        fclose( fp );
        return NULL;
      }
      s = fread(text, sizeof(char), size, fp);
      text[size] = '\000';
      if(s != size)
        WARNc_S( "fread %lu but should read %lu",
                (long unsigned int) s, (long unsigned int)size);
      fclose( fp );
    } else
    {
      WARNc_S( "%s\"%s\"", _("Could not open: "), file_name);
    }
  }

  if(size_ptr)
    *size_ptr = size;

  return text;
}

/** @internal
 *  Copy to external allocator */
char *       oyjlReAllocFromStdMalloc_(char              * mem,
                                       int               * size,
                                       void*            (* alloc)(size_t) )
{
  if(mem)
  {
    if(alloc != malloc)
    {
      char* temp = mem;

      mem = alloc( *size + 1 );
      if(mem)
      {
        memcpy( mem, temp, *size );
        mem[*size] = '\000';
      }
      else
        *size = 0;

      free( temp );
    } else
      mem[*size] = '\000';
  }

  return mem;
}

int oyjlIsFileFull_ (const char* fullFileName, const char * read_mode);
/* resembles which */
char * oyjlFindApplication_(const char * app_name)
{
  const char * path = getenv("PATH");
  char * full_app_name = NULL;
  if(path && app_name)
  {
    int paths_n = 0, i;
    char ** paths = oyjlStringSplit( path, ':', &paths_n, malloc );
    for(i = 0; i < paths_n; ++i)
    {
      char * full_name = NULL;
      int found;
      oyjlStringAdd( &full_name, 0,0, "%s/%s", paths[i], app_name );
      found = oyjlIsFileFull_( full_name, "rb" );
      if(found)
      {
        i = paths_n;
        full_app_name = strdup( full_name );
      }
      free( full_name );
      if(found) break;
    }
    oyjlStringListRelease( &paths, paths_n, free );
  }
  return full_app_name;
}

/** @brief detect program
 *
 *  Search for a command in the executeable path. It resembles 'which'.
 *
 *  @param[in]      app_name            application name withou path
 *  @return                             1 - if found, otherwise 0
 *
 *  @version Oyjl: 1.0.0
 *  @date    2022/04/23
 *  @since   2022/04/23 (Oyjl: 1.0.0)
 */
int        oyjlHasApplication        ( const char        * app_name)
{
  char * full_app_name = oyjlFindApplication_(app_name);
  int found = full_app_name == NULL ? 0 : 1;
  if(full_app_name) free(full_app_name);
  return found;
}

/** @internal
 *  Read a file stream without knowing its size in advance.
 */
char * oyjlReadCmdToMem_             ( const char        * command,
                                       int               * size,
                                       const char        * mode,
                                       void*            (* alloc)(size_t) )
{
  char * text = 0;
  FILE * fp = 0;

  if(!alloc) alloc = malloc;

  if(command && command[0] && size )
  {
    {
      if(*oyjl_debug && (strstr(command, "addr2line") == NULL && *oyjl_debug == 2))
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "%s", OYJL_DBG_ARGS, command );
      fp = oyjlPOPEN_m( command, mode );
    }
    if(fp)
    {
      int mem_size = 0;
      char* mem = NULL;

      text = oyjlReadFileStreamToMem(fp, size);

      if(!feof(fp))
      {
        if(text) { free( text ); text = NULL; }
        *size = 0;
        mem_size = 1024;
        mem = (char*) malloc(mem_size+1);
        oyjlPCLOSE_m(fp);
        fp = oyjlPOPEN_m( command, mode );
      }
      if(fp)
      while(!feof(fp))
      {
        if(*size >= mem_size)
        {
          mem_size *= 10;
          mem = realloc( mem, mem_size+1 );
          if(!mem) { *size = 0; break; }
        }
        if(mem)
          *size += fread( &mem[*size], sizeof(char), mem_size-*size, fp );
      }
      if(fp && mem)
      {
        mem = oyjlReAllocFromStdMalloc_( mem, size, alloc );
        text = mem;
      }
      if(fp)
        oyjlPCLOSE_m(fp);
      fp = 0;

      if(*size == 0)
      {
        char * t = strdup(command);
        char * end = strstr( t?t:"", " " ), * app;
        if(end)
          end[0] = '\000';

        if((app = oyjlFindApplication_( t )) == NULL)
          oyjlMessage_p( oyjlMSG_ERROR,0, OYJL_DBG_FORMAT "%s: \"%s\"",
                         OYJL_DBG_ARGS, _("Program not found"), command?command:"");

        if(t) free(t);
        if(app) free(app);
      }
    }
  }

  return text;
}

/** @brief Read a stream from shell command.
 */
char *     oyjlReadCommandF          ( int               * size,
                                       const char        * mode,
                                       void*            (* alloc)(size_t),
                                       const char        * format,
                                                           ... )
{
  char * result = NULL;
  char * text = 0;

  if(!alloc) alloc = malloc;

  OYJL_CREATE_VA_STRING(format, text, malloc, return NULL)

  result = oyjlReadCmdToMem_( text, size, mode, alloc );

  free(text);

  return result;
}


#include <sys/stat.h> /* stat() */
int oyjlIsFileFull_ (const char* fullFileName, const char * read_mode)
{
  struct stat status;
  int r = 0;
  const char* name = fullFileName;

  memset(&status,0,sizeof(struct stat));
  if(name && name[0])
    r = stat(name, &status);

  if(r != 0 && *oyjl_debug > 1)
  switch (errno)
  {
    case EACCES:       WARNc_S("Permission denied: %s", name); break;
    case EIO:          WARNc_S("EIO : %s", name); break;
    case ENAMETOOLONG: WARNc_S("ENAMETOOLONG : %s", name); break;
    case ENOENT:       WARNc_S("A component of the name/file_name does not exist, or the file_name is an empty string: \"%s\"", name); break;
    case ENOTDIR:      WARNc_S("ENOTDIR : %s", name); break;
    case ELOOP:        WARNc_S("Too many symbolic links encountered while traversing the name: %s", name); break;
    case EOVERFLOW:    WARNc_S("EOVERFLOW : %s", name); break;
    default:           WARNc_S("%s : %s", strerror(errno), name); break;
  }

  r = !r &&
       (   ((status.st_mode & S_IFMT) & S_IFREG)
        || ((status.st_mode & S_IFMT) & S_IFLNK)
                                                );

  if (r)
  {
    FILE* fp = fopen (name, read_mode);
    if (!fp) {
      oyjlMessage_p( oyjlMSG_INFO, 0, "not existent: %s", name );
      r = 0;
    } else {
      fclose (fp);
    }
  }

  return r;
}
int oyjlIsDirFull_ (const char* name)
{
  struct stat status;
  int r = 0;

  if(!name) return 0;

  memset(&status,0,sizeof(struct stat));
  if(name && name[0])
    r = stat(name, &status);

  if(r != 0 && *oyjl_debug > 1)
  switch (errno)
  {
    case EACCES:       WARNc_S("Permission denied: %s", name); break;
    case EIO:          WARNc_S("EIO : %s", name); break;
    case ENAMETOOLONG: WARNc_S("ENAMETOOLONG : %s", name); break;
    case ENOENT:       WARNc_S("A component of the name/file_name does not exist, or the file_name is an empty string: \"%s\"", name); break;
    case ENOTDIR:      WARNc_S("ENOTDIR : %s", name); break;
#ifdef HAVE_POSIX
    case ELOOP:        WARNc_S("Too many symbolic links encountered while traversing the name: %s", name); break;
    case EOVERFLOW:    WARNc_S("EOVERFLOW : %s", name); break;
#endif
    default:           WARNc_S("%s : %s", strerror(errno), name); break;
  }
  r = !r &&
       ((status.st_mode & S_IFMT) & S_IFDIR);

  return r;
}

int   oyjlIsFile                     ( const char        * fullname,
                                       const char        * mode,
                                       char              * info,
                                       int                 info_len )
{
  struct stat status;
  int r;
  memset(&status,0,sizeof(struct stat));
  double mod_time = 0.0;

  r = oyjlIsFileFull_( fullname, mode );

  if (r)
  {
    int error = stat(fullname, &status);
    if( error )
      return 0;
#   if defined(__APPLE__) || defined(BSD)
    mod_time = status.st_mtime ;
    mod_time += status.st_mtimespec.tv_nsec/1000000. ;
#   elif defined(WIN32)
    mod_time = (double)status.st_mtime ;
#   elif defined(__ANDROID__)
    mod_time = status.st_mtime ;
#   else
    mod_time = status.st_mtim.tv_sec ;
    mod_time += status.st_mtim.tv_nsec/1000000. ;
#   endif
    if(info)
      snprintf( info, info_len, "%.30f", mod_time );
  }

  return r;
}

char* oyjlExtractPathFromFileName_ (const char* file_name)
{
  char * path_name = NULL;
  char * ptr = NULL;

  if(!file_name) return NULL;

  path_name = strdup( file_name );
  if(path_name)
  {
    ptr = strrchr (path_name, '/');
    if(ptr)
      ptr[0] = 0;
    else
    {
      free(path_name);
      path_name = NULL;
    }
  }

  if(!path_name)
    path_name = strdup( "." );

  return path_name;
}

char * oyjlPathGetParent_ (const char* name)
{
  char *parentDir = 0, *ptr = 0;

  parentDir = strdup( name );
  ptr = strrchr( parentDir, '/');
  if (ptr)
  {
    if (ptr[1] == 0) /* ending dir separator */
    {
      ptr[0] = 0;
      if (strrchr( parentDir, '/'))
      {
        ptr = strrchr (parentDir, '/');
        ptr[0+1] = 0;
      }
    }
    else
      ptr[0+1] = 0;
  }

  return parentDir;
}

int oyjlMakeDir_ (const char* path)
{
  const char * full_name = path;
  char * path_parent = 0,
       * path_name = 0;
  int rc = !full_name;

#ifdef HAVE_POSIX
  mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; /* 0755 */
#endif

  if(full_name)
    path_name = oyjlExtractPathFromFileName_(full_name);
  if(path_name && path_name[0] == '\000')
    oyjlStringAdd( &path_name, 0,0, "%s", full_name );
  if(path_name)
  {
    if(!oyjlIsDirFull_(path_name))
    {
      path_parent = oyjlPathGetParent_(path_name);
      if(!oyjlIsDirFull_(path_parent))
        rc = oyjlMakeDir_(path_parent);
      if(path_parent) free( path_parent );

      if(!rc)
        rc = mkdir (path_name
#ifdef HAVE_POSIX
                            , mode
#endif
                                  );
      if(rc && *oyjl_debug > 1)
      switch (errno)
      {
        case EACCES:       WARNc_S("Permission denied: %s", path); break;
        case EIO:          WARNc_S("EIO : %s", path); break;
        case ENAMETOOLONG: WARNc_S("ENAMETOOLONG : %s", path); break;
        case ENOENT:       WARNc_S("A component of the path/file_name does not exist, or the file_name is an empty string: \"%s\"", path); break;
        case ENOTDIR:      WARNc_S("ENOTDIR : %s", path); break;
#ifdef HAVE_POSIX
        case ELOOP:        WARNc_S("Too many symbolic links encountered while traversing the path: %s", path); break;
        case EOVERFLOW:    WARNc_S("EOVERFLOW : %s", path); break;
#endif
        default:           WARNc_S("%s : %s", strerror(errno), path); break;
      }
    }
    free( path_name );;
  }

  return rc;
}


/** @brief write memory to FILE
 */
int  oyjlWriteFile                   ( const char        * filename,
                                       const void        * mem,
                                       int                 size )
{
  FILE *fp = 0;
  const char * full_name = filename;
  int r = !filename;
  int written_n = 0;
  char * path = 0;

  if(!r)
  {
    path = oyjlExtractPathFromFileName_( full_name );
    r = oyjlMakeDir_( path );
  }

  if(!r)
  {
    fp = fopen(full_name, "wb");
    if ((fp != 0)
     && mem
     && size)
    {
#if 0
      do {
        r = fputc ( block[pt++] , fp);
      } while (--size);
#else
      if(*oyjl_debug && *oyjl_debug == 2)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "%s(%d)", OYJL_DBG_ARGS, full_name, size );
      written_n = fwrite( mem, 1, size, fp );
      if(written_n != size)
        r = errno;
#endif
    } else 
      if(mem && size)
        r = errno;
      else
        WARNc_S("no data to write into: \"%s\"", filename );

    if(r && *oyjl_debug > 1)
    {
      switch (errno)
      {
        case EACCES:       WARNc_S("Permission denied: %s", filename); break;
        case EIO:          WARNc_S("EIO : %s", filename); break;
        case ENAMETOOLONG: WARNc_S("ENAMETOOLONG : %s", filename); break;
        case ENOENT:       WARNc_S("A component of the path/file_name does not exist, or the file_name is an empty string: \"%s\"", filename); break;
        case ENOTDIR:      WARNc_S("ENOTDIR : %s", filename); break;
#ifdef HAVE_POSIX
        case ELOOP:        WARNc_S("Too many symbolic links encountered while traversing the path: %s", filename); break;
        case EOVERFLOW:    WARNc_S("EOVERFLOW : %s", filename); break;
#endif
        default:           WARNc_S("%s : %s", strerror(errno), filename);break;
      }
    }

    if (fp) fclose (fp);
  }

  if(path) free( path );

  return written_n;
}

/** @} *//* oyjl_io */
/** @} *//* oyjl_core */

/* --- IO_Section --- */

