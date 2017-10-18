/** @file oyranos-compat-gnome.c
 *
 *  @par Copyright:
 *            2016 (C) Kai-Uwe Behrmann
 *
 *  @brief    compatibility tool for handling gnome monitor profiles
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2016/11/22
 */
/* gcc -Wall -g -ldl oyranos-compat-gnome.c -o oyranos-compat-gnome */

#include <dlfcn.h> /* dlopen() */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef OY_LIB_PREFIX
#define OY_LIB_PREFIX "lib"
#endif
#ifndef OY_LIB_SUFFIX
#define OY_LIB_SUFFIX ".so"
#endif

char *       oyReadFilepToMem_       ( FILE              * fp,
                                       size_t            * size )
{
  char* mem = NULL;

  {
    if (fp)
    {
      int sz;
      /* get size */
      fseek(fp,0L,SEEK_END); 
      sz = ftell (fp);
      if(sz == -1)
      {
        switch(errno)
        {
          case EBADF:        fprintf(stderr, "Not a seekable stream\n"); break;
          case EINVAL:       fprintf(stderr, "Wrong argument\n"); break;
          default:           fprintf(stderr, "%s\n", strerror(errno)); break;
        }
        *size = 0;
        return NULL;
      }
      /* read file possibly partitial */
      if(!*size || (int)*size > ftell(fp))
        *size = sz;
      rewind(fp);

      if(!*size)
        return mem;

      /* allocate memory */
      mem = calloc( sizeof(char), *size+1 );

      /* check and read */
      if ((fp != 0)
       && mem
       && *size)
      {
        int s = fread(mem, sizeof(char), *size, fp);

        /* check again */
        if (s != (int)*size)
        { *size = 0;
          free (mem);
          mem = 0;
        } else {
          /* copy to external allocator */
          char* temp = mem;
          mem = calloc( sizeof(char), *size + 1 );
          if(mem) {
            memcpy( mem, temp, *size );
            free (temp);
            mem[*size] = 0;
          } else {
            free (mem);
            *size = 0;
          }
        }
      }
    }
  }
 
  return mem;
}
char*
oyReadFileToMem_(const char* name, size_t *size)
{
  FILE * fp = NULL;
  char * mem = NULL;
  const char * filename = name;

  if(filename && filename[0] && strlen(filename) > 7 &&
     memcmp(filename, "file://", 7) == 0)
    filename = &filename[7];

  if(filename)
  {
    fp = fopen(filename, "rb");

    if(fp)
    {
      mem = oyReadFilepToMem_( fp, size );
    } else {
      fprintf( stderr, "%s: %s\n", "Could not open file", filename?filename:"" );
    }
  }
 
  /* clean up */
  if (fp) fclose (fp);

  return mem;
}
char * oyReadStdinToMem_             ( size_t            * size )
{
  char * text = 0;
  {
    int text_size = 0, buf_size = 0; 
    int c;
    char * tmp = 0;

    while(((c = getc(stdin)) != EOF))
    {
      if(text_size >= buf_size-1)
      {
        buf_size = text_size + 65536;
        tmp = calloc( sizeof(char), buf_size );
        if(text_size)
          memcpy(tmp, text, text_size);
        free(text);
        text = tmp; tmp = 0;
      }
      text[text_size++] = c;
    }
    if(text)
      text[text_size] = '\000';

    if(size)
      *size = text_size;
  }

  return text;
}

typedef enum {
	UCMM_EDID_ERROR_OK = 0,
	UCMM_EDID_ERROR_RESOURCE,
	UCMM_EDID_ERROR_INVALID_PROFILE,
	UCMM_EDID_ERROR_NO_PROFILE,
	UCMM_EDID_ERROR_UNUSED1,
	UCMM_EDID_ERROR_NO_DATA,
	UCMM_EDID_ERROR_PROFILE_COPY,
	UCMM_EDID_ERROR_UNUSED2,
	UCMM_EDID_ERROR_ACCESS_CONFIG,
	UCMM_EDID_ERROR_SET_CONFIG,
	UCMM_EDID_ERROR_UNUSED3,
	UCMM_EDID_ERROR_MONITOR_NOT_FOUND
} UCMM_ERROR_e;

const char * getUcmmError( int error )
{
  switch(error)
  {
  case UCMM_EDID_ERROR_OK: return "OK";
  case UCMM_EDID_ERROR_RESOURCE: return "resource";
  case UCMM_EDID_ERROR_INVALID_PROFILE: return "invalid profile";
  case UCMM_EDID_ERROR_NO_PROFILE: return "no profile";
  case UCMM_EDID_ERROR_NO_DATA: return "no data";
  case UCMM_EDID_ERROR_PROFILE_COPY: return "profile copy";
  case UCMM_EDID_ERROR_ACCESS_CONFIG: return "access config";
  case UCMM_EDID_ERROR_SET_CONFIG: return "set config";
  case UCMM_EDID_ERROR_MONITOR_NOT_FOUND: return "monitor not found";
  default: return "???";
  }
  return "";
}

int main(int argc, char **argv_)
{
  int  (*lcd_edid_install_profile) ( char * edid, int edid_len, int scope, char * profile_fn ) = NULL;
  int  (*lcd_edid_remove_profile) ( char * edid, int edid_len, char *  profile_fn ) = NULL;
  int  (*lcd_edid_get_profile) ( char * edid, int edid_len, char ** profile_fn ) = NULL;

  char ** argv = argv_;
  int verbose = 1;
  if(argc > 1 && strcmp(argv[1],"-q") == 0)
  {
    verbose = 0;
    --argc;
    argv = &argv_[1];
  }

  if(argc == 4 || argc == 6)
  {
    const char * lib_name = "libcolordcompat.so";
    void * gnome_handle = dlopen(lib_name, RTLD_LAZY);
    int error = 0, report = 0;
#define LOAD_FUNC( func, fallback_func ) l##func = dlsym(gnome_handle, #func ); \
               if(!l##func) \
               { \
                 if(#fallback_func != NULL) \
                 { \
                   l##func = fallback_func; \
                 } else \
                 { \
                   error = 1; \
                 } \
                 report = 1; \
                 if(verbose) fprintf( stderr, "dlsym failed: %s\n", \
                                       dlerror() ); \
               }
    char * type = argv[1],
         * edid_fn = argv[3],
         * profile_fn = (argc == 6) ? argv[5] : NULL;
    char * edid = NULL;
    size_t size = 0;

    LOAD_FUNC( cd_edid_install_profile, 0 )
    LOAD_FUNC( cd_edid_remove_profile, 0 )
    LOAD_FUNC( cd_edid_get_profile, 0 )

    if(report && verbose)
      fprintf(stderr, "can not load symbols from libcolordcompat.so\n");

    if(strcmp(edid_fn,"-") == 0)
      edid = oyReadStdinToMem_( &size );
    else
      edid = oyReadFileToMem_( edid_fn, &size );

    if(strcmp( type, "-a" ) == 0 && argc == 6)
    {
      error = lcd_edid_install_profile( edid, size, 0, profile_fn );
      if(error)
      {
#if 0
        char * fn = strdup(profile_fn),
             * t = strstr(fn, "/share/color/icc/"),
             * pure_fn;
        if(t)
        {
          pure_fn = strrchr(t,'/');
          if(pure_fn++)
            memmove( &t[17], pure_fn, strlen(pure_fn) + 1 );
          memmove( &t[7], &t[13], strlen(t) - 12 );
        }
#else
        const char * home = getenv("HOME");
        char * fn = calloc( sizeof(char), strlen(home?home:"") + strlen(profile_fn) + 256 );
        char * pure_fn = strrchr(profile_fn,'/');
        if(pure_fn && home)
          sprintf( fn, "%s/.local/share/icc/%s", home, pure_fn + 1 );
#endif
        if(fn && fn[0] && strcmp(fn, profile_fn) != 0)
          error = lcd_edid_install_profile( edid, size, 0, fn );
      }
      if(!error)
        return 0;
    } 
    if(strcmp( type, "-e" ) == 0 && argc == 4)
    {
      do {
        profile_fn = NULL;
        error = lcd_edid_get_profile( edid, size, &profile_fn );
        if(verbose)
          fprintf( stdout, "profile to erase: %s\n", profile_fn );
        if(profile_fn)
          error = lcd_edid_remove_profile( edid, size, profile_fn );
      } while (profile_fn);
      if(!error)
        return 0;
    }
    if(strcmp( type, "-l" ) == 0 && argc == 4)
    {
      error = lcd_edid_get_profile( edid, size, &profile_fn );
      if(verbose)
        fprintf( stdout, "%s", profile_fn );
      if(!error)
        return 0;
    } 
    if(verbose)
      fprintf( stderr, "type: %s EDID_FILENAME: %s ICC_FILENAME: %s status: %s\n", type, edid_fn, profile_fn, getUcmmError(error));
  }

  if(verbose)
  {
    fprintf( stderr, "usage: %s [-q] -a [-i -| -i EDID_FILENAME] -p ICC_FILENAME\n", argv[0]);
    fprintf( stderr, "usage: %s [-q] -e [-i -| -i EDID_FILENAME]\n", argv[0]);
    fprintf( stderr, "usage: %s [-q] -l [-i -| -i EDID_FILENAME]\n", argv[0]);
    fprintf( stderr, "\t-a\t\t\tadd a profile\n");
    fprintf( stderr, "\t-e\t\t\tdelete all profiles\n");
    fprintf( stderr, "\t-l\t\t\tget a profile\n");
    fprintf( stderr, "\t-p ICC_FILENAME\t\tthe file to handle\n");
    fprintf( stderr, "\t-i EDID_FILENAME\tread EDID blob from file\n");
    fprintf( stderr, "\t-i -\t\t\tread EDID blob from input stream\n");
    fprintf( stderr, "\t-q\t\t\tquiet mode\n");
    fprintf( stderr, "example: oyranos-monitor -d 0 -f edid | oyranos-compat-gnome -a -i - -p \"`oyranos-profile -l --path sRGB`\"\n" );
    fprintf( stderr, "note: arguments are order dependent\n");
  }

  return 0;
}
