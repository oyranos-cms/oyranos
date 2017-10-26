/*  @file openicc_io.c
 *
 *  libOpenICC - OpenICC Colour Management Configuration
 *
 *  @par Copyright:
 *            2011-2016 (C) Kai-Uwe Behrmann
 *
 *  @brief    OpenICC Colour Management configuration helpers
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2011/06/27
 */

#include "openicc_config_internal.h"

#if HAVE_POSIX
#include <unistd.h>  /* getpid() */
#endif
#include <string.h>  /* strdup() */
#include <stdarg.h>  /* vsnprintf() */
#include <stdio.h>   /* vsnprintf() */
#include <errno.h>



char * openiccOpenFile( const char * file_name,
                        size_t   * size_ptr )
{
  FILE * fp = NULL;
  size_t size = 0, s = 0;
  char * text = NULL;

  if(file_name)
  {
    fp = fopen(file_name,"rb");
    if(fp)
    {
      fseek(fp,0L,SEEK_END); 
      size = ftell (fp);
      if(size == (size_t)-1)
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
      text = malloc(size+1);
      if(text == NULL)
      {
        WARNc_S( "Error: Could allocate memory: %lu", (long unsigned int)size);
        fclose( fp );
        return NULL;
      }
      s = fread(text, sizeof(char), size, fp);
      text[size] = '\000';
      if(s != size)
        WARNc_S( "Error: fread %lu but should read %lu",
                (long unsigned int) s, (long unsigned int)size);
      fclose( fp );
    } else
    {
      WARNc_S( "Error: Could not open file - \"%s\"", file_name);
    }
  }

  if(size_ptr)
    *size_ptr = size;

  return text;
}

char *       openiccReadFileSToMem   ( FILE              * fp,
                                       size_t            * size)
{
  size_t mem_size = 256;
  char* mem = (char*) malloc(mem_size);
  int c;

  if(!mem) return NULL;

  if (fp && size)
  {
    *size = 0;
    do
    {
      c = getc(fp);
      if(*size >= mem_size)
      {
        mem_size *= 2;
        mem = realloc( mem, mem_size );
        if(!mem) { *size = 0; return NULL; }
      }
      mem[(*size)++] = c;
    } while(!feof(fp));

    --*size;

    if(mem)
      mem[*size] = 0;
  }

  return mem;
}

#include <errno.h>
#include <sys/stat.h> /* mkdir() */
char* openiccExtractPathFromFileName_ (const char* file_name)
{
  char *path_name = 0;
  char *ptr;

  path_name = strdup( file_name );
  ptr = strrchr (path_name, '/');
  if(ptr)
    ptr[0] = 0;
  else
    strcpy( path_name, "." );
  return path_name;
}
int openiccIsDirFull_ (const char* name)
{
  struct stat status;
  int r = 0;

  memset(&status,0,sizeof(struct stat));
  r = stat (name, &status);

  if(r != 0 && openicc_debug > 1)
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

char * openiccPathGetParent_ (const char* name)
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


int openiccMakeDir_ (const char* path)
{
  const char * full_name = path;
  char * path_parent = 0,
       * path_name = 0;
  int rc = !full_name;

#ifdef HAVE_POSIX
  mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; /* 0755 */
#endif

  if(full_name)
    path_name = openiccExtractPathFromFileName_(full_name);
  if(path_name)
  {
    if(!openiccIsDirFull_(path_name))
    {
      path_parent = openiccPathGetParent_(path_name);
      if(!openiccIsDirFull_(path_parent))
      {
        rc = openiccMakeDir_(path_parent);
        free( path_parent );
      }

      rc = mkdir (path_name
#ifdef HAVE_POSIX
                            , mode
#endif
                                  );
      if(rc && openicc_debug > 1)
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

int openiccIsFileFull_ (const char* fullFileName, const char * read_mode)
{
  struct stat status;
  int r = 0;
  const char* name = fullFileName;

  memset(&status,0,sizeof(struct stat));
  r = stat (name, &status);

  if(r != 0 && openicc_debug > 1)
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
      openiccMessage_p( openiccMSG_DBG, 0, "not existent: %s", name );
      r = 0;
    } else {
      fclose (fp);
    }
  }

  return r;
}
  
size_t openiccWriteFile(const char * filename,
                        void       * mem,
                        size_t       size )
{
  FILE *fp = 0;
  const char * full_name = filename;
  int r = !filename;
  size_t written_n = 0;
  char * path = 0;

  if(!r)
  {
    path = openiccExtractPathFromFileName_( full_name );
    r = openiccMakeDir_( path );
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
      written_n = fwrite( mem, 1, size, fp );
      if(written_n != size)
        r = errno;
#endif
    } else 
      if(mem && size)
        r = errno;
      else
        WARNc_S("no data to write into: \"%s\"", filename );

    if(r && openicc_debug > 1)
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
