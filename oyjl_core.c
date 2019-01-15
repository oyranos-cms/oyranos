/*  @file oyjl_core.c
 *
 *  oyjl - string, file i/o and other basic helpers
 *
 *  @par Copyright:
 *            2016-2019 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl core functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *
 * Copyright (c) 2004-2018  Kai-Uwe Behrmann  <ku.b@gmx.de>
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

#include <math.h>    /* NAN */
#include <stdarg.h>  /* va_list */
#include <stddef.h>  /* ptrdiff_t size_t */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "oyjl.h"
#include "oyjl_macros.h"
#include "oyjl_version.h"
#include "oyjl_tree_internal.h"
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>           /* setlocale LC_NUMERIC */
#endif
int oyjl_debug_local = 0;
int * oyjl_debug = &oyjl_debug_local;

/** @brief   set own debug variable */
void       oyjlDebugVariableSet      ( int               * debug )
{ oyjl_debug = debug; }

/** @brief   the default message handler to stderr
 *
 *  @version OpenICC: 0.1.0
 *  @date    2011/10/21
 *  @since   2008/04/03 (OpenICC: 0.1.0)
 */
int          oyjlMessageFunc         ( int/*oyjlMSG_e*/    error_code,
                                       const void        * context_object OYJL_UNUSED,
                                       const char        * format,
                                       ... )
{
  char * text = NULL;
  int error = 0;
  const char * status_text = NULL;

  OYJL_CREATE_VA_STRING(format, text, malloc, return 1)

  if(error_code == oyjlMSG_INFO) status_text = "Info: ";
  if(error_code == oyjlMSG_CLIENT_CANCELED) status_text = "Client Canceled: ";
  if(error_code == oyjlMSG_INSUFFICIENT_DATA) status_text = "Insufficient data: ";
  if(error_code == oyjlMSG_ERROR) status_text = "!!! ERROR: ";

  if(status_text)
    fprintf( stderr, "%s", status_text );
  if(text)
    fprintf( stderr, "%s\n", text );
  fflush( stderr );

  free( text ); text = 0;

  return error;
}

oyjlMessage_f     oyjlMessage_p = oyjlMessageFunc;

/** @brief   set a custom message listener
 *
 *  @version OpenICC: 0.1.0
 *  @date    2011/10/21
 *  @since   2008/04/03 (OpenICC: 0.1.0)
 */
int            oyjlMessageFuncSet    ( oyjlMessage_f       message_func )
{
  if(message_func)
    oyjlMessage_p = message_func;
  return 0;
}

/** \addtogroup oyjl_core
 *  @{ *//* oyjl_core */

/** @brief   convert a string into list
 *
 *  @param[in]     text                source string
 *  @param[in]     delimiter           the char which marks the split; e.g. comma ','
 *  @param[out]    count               number of detected string segments; optional
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @return                            array of detected string segments
 */
char **        oyjlStringSplit       ( const char        * text,
                                       const char          delimiter,
                                       int               * count,
                                       void*            (* alloc)(size_t))
{
  char ** list = 0;
  int n = 0, i;

  /* split the path search string by a delimiter */
  if(text && text[0] && delimiter)
  {
    const char * tmp = text;

    if(!alloc) alloc = malloc;

    if(tmp[0] == delimiter) ++n;
    do { ++n;
    } while( (tmp = strchr(tmp + 1, delimiter)) );

    tmp = 0;

    if((list = alloc( (n+1) * sizeof(char*) )) == 0) return NULL;
    memset( list, 0, (n+1) * sizeof(char*) );

    {
      const char * start = text;
      for(i = 0; i < n; ++i)
      {
        intptr_t len = 0;
        char * end = strchr(start, delimiter);

        if(end > start)
          len = end - start;
        else if (end == start)
          len = 0;
        else
          len = strlen(start);

        if((list[i] = alloc( len+1 )) == 0) return NULL;

        memcpy( list[i], start, len );
        list[i][len] = 0;
        start += len + 1;
      }
    }
  }

  if(count)
    *count = n;

  return list;
}

/** @brief   duplicate a string with custom allocator
 *
 *  The function adds the allocator over standard strdup().
 *
 *  @param[in]     string              source string
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @return                            copy
 */
char *     oyjlStringCopy            ( const char        * string,
                                       void*            (* alloc)(size_t))
{
  char * text_copy = NULL;

  if(string)
  {
    oyjlAllocHelper_m( text_copy, char, strlen(string) + 1,
                        alloc, return NULL );
    strcpy( text_copy, string );
  }
    
  return text_copy;
}

/** @brief   sprintf with de-/allocator
 *
 *  The function adds memory management over standard sprintf().
 *
 *  @param[in]     string              source string
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @param[in]     deAlloc             custom deallocator matching alloc; optional, default is free
 *  @param[in]     format              printf style format string
 *  @param[in]     ...                 argument list for format
 *  @return                            constructed string
 */
int        oyjlStringAdd             ( char             ** string,
                                       void*            (* alloc)(size_t size),
                                       void             (* deAlloc)(void * data ),
                                       const char        * format,
                                                           ... )
{
  char * text_copy = NULL;
  char * text = 0;

  void*(* allocate)(size_t size) = alloc?alloc:malloc;
  void (* deAllocate)(void * data ) = deAlloc?deAlloc:free;

  OYJL_CREATE_VA_STRING(format, text, alloc, return 1)

  if(string && *string)
  {
    int l = strlen(*string),
        l2 = strlen(text);
    text_copy = allocate( l2 + l + 1 );
    strcpy( text_copy, *string );
    strcpy( &text_copy[l], text );

    deAllocate(*string);
    *string = text_copy;

    deAllocate(text);

  } else if(string)
    *string = text;
  else if(text)
    deAllocate( text );

  return 0;
}

/** @brief   append to the string end
 *
 *  For a plain memory duplication use:
 *  @code
    void * copy = oyjlStringAppendN( NULL, (const char*)ptr, ptr_len, malloc );
    @endcode
 *
 *  @param[in]     text                source string; optional
 *  @param[in]     append              to be added text to string; optional
 *  @param[in]     append_len          length of append
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @return                            constructed string+append
 */
char*      oyjlStringAppendN         ( const char        * text,
                                       const char        * append,
                                       int                 append_len,
                                       void*            (* alloc)(size_t size) )
{
  char * text_copy = NULL;
  int text_len = 0;

  if(text)
    text_len = strlen(text);

  if(text_len || append_len)
  {
    oyjlAllocHelper_m( text_copy, char,
                        text_len + append_len + 1,
                        alloc, return NULL );

    if(text_len)
      memcpy( text_copy, text, text_len );

    if(append_len)
      memcpy( &text_copy[text_len], append, append_len );

    text_copy[text_len+append_len] = '\000';
  }

  return text_copy;
}

/** @brief   append to the string end
 *
 *  @param[in]     text                source string
 *  @param[in]     append              to be added text to string
 *  @param[in]     append_len          length of append
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @param[in]     deAlloc             custom deallocator matching alloc; optional, default is free
 *  @return                            constructed string+append
 */
void       oyjlStringAddN            ( char             ** text,
                                       const char        * append,
                                       int                 append_len,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  char * text_copy = NULL;

  if(!text) return;

  text_copy = oyjlStringAppendN(*text, append, append_len, alloc);

  if(*text && deAlloc)
    deAlloc(*text);

  *text = text_copy;

  return;
}

/** @brief   substitute pattern in a string
 *
 *  @param[in]     text                source string
 *  @param[in]     search              pattern to be tested in text
 *  @param[in]     replacement         string to be put in place of search sub string
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @param[in]     deAlloc             custom deallocator matching alloc; optional, default is free
 *  @return                            manipulated result
 */
char*      oyjlStringReplace         ( const char        * text,
                                       const char        * search,
                                       const char        * replacement,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  char * t = 0;
  const char * start = text,
             * end = text;

  void*(* allocate)(size_t size) = alloc?alloc:malloc;
  void (* deAllocate)(void * data ) = deAlloc?deAlloc:free;

  if(text && search && replacement)
  {
    int s_len = strlen(search);
    while((end = strstr(start,search)) != 0)
    {
      oyjlStringAddN( &t, start, end-start, allocate, deAllocate );
      oyjlStringAddN( &t, replacement, strlen(replacement), allocate, deAllocate );
      if(strlen(end) >= (size_t)s_len)
        start = end + s_len;
      else
      {
        if(strstr(start,search) != 0)
          oyjlStringAddN( &t, replacement, strlen(replacement), allocate, deAllocate );
        start = end = end + s_len;
        break;
      }
    }
  }

  if(start && strlen(start))
    oyjlStringAddN( &t, start, strlen(start), allocate, deAllocate );

  return t;
}


/** @brief append a string list to an other string list */
char **    oyjlStringListCatList     ( const char       ** list,
                                       int                 n_alt,
                                       const char       ** append,
                                       int                 n_app,
                                       int               * count,
                                       void*            (* alloc)(size_t) )
{
  char ** nlist = 0;

  {
    int i = 0;
    int n = 0;

    if(n_alt || n_app)
      oyjlAllocHelper_m(nlist, char*, n_alt + n_app +1, alloc, return NULL);

    for(i = 0; i < n_alt; ++i)
    {
      if(list[i])
        nlist[n] = oyjlStringCopy( list[i], alloc );
      n++;
    }

    for(i = 0; i < n_app; ++i)
    {
      nlist[n] = oyjlStringCopy( append[i], alloc );
      n++;
    }

    if(count)
      *count = n;
  }

  return nlist;
}

/** @brief free a string list */
void       oyjlStringListRelease  ( char            *** l,
                                       int                 size,
                                       void             (* deAlloc)(void*) )
{
  if(!deAlloc) deAlloc = free;

  if( l )
  {
    char ** list = *l;

    if(list)
    {
      size_t i;
      for(i = 0; (int)i < size; ++i)
        if((list)[i])
          deAlloc( (list)[i] );
      deAlloc( list );
      *l = NULL;
    }
  }
}

/** @brief append a string to a string list */
void       oyjlStringListAddStaticString (
                                       char            *** list,
                                       int               * n,
                                       const char        * string,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  char ** nlist = 0;
  int n_alt;

  if(!list || !n) return;

  n_alt = *n;

  oyjlAllocHelper_m(nlist, char*, n_alt + 2, alloc, return);

  memmove( nlist, *list, sizeof(char*) * n_alt);
  nlist[n_alt] = oyjlStringCopy( string, alloc );
  nlist[n_alt+1] = NULL;

  *n = n_alt + 1;

  if(*list)
    deAlloc(*list);

  *list = nlist;
}

/** @brief filter doubles out
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/08/04
 *  @since   2015/08/04 (Oyranos: 0.9.6)
 */
void       oyjlStringListFreeDoubles (
                                       char             ** list,
                                       int               * list_n,
                                       void             (* deAlloc)(void*) )
{
  int n, i, pos;

  if(!list) return;

  n = *list_n;
  pos = n ? 1 : 0;

  if(!deAlloc) deAlloc = free;

  for(i = pos; i < n; ++i)
  {
    int k, found = 0;
    char * ti = list[i];
    for( k = 0; k < i; ++k )
    {
      char * tk = list[k];
      if(ti && tk && strcmp(ti, tk) == 0)
      {
        deAlloc( ti );
        list[i] = ti = NULL;
        found = 1;
        continue;
      }
    }

    if(found == 0)
    {
      list[pos] = ti;
      ++pos;
    }
  }

  list[pos] = NULL;

  *list_n = pos;
}

/** @brief append a string list to an other and handle memory */
void     oyjlStringListAddList       ( char            *** list,
                                       int               * n,
                                       const char       ** append,
                                       int                 n_app,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  int alt_n = 0;
  char ** tmp;

  if(!list) return;

  if(n) alt_n = *n;
  tmp = oyjlStringListCatList((const char**)*list, alt_n, append, n_app,
                                     n, alloc);

  oyjlStringListRelease(list, alt_n, deAlloc);

  *list = tmp;
}


/** show better const behaviour and return instant error status */
int      oyjlStringToLong            ( const char        * text,
                                       long              * value )
{
  char * end = 0;
  *value = strtol( text, &end, 0 );
  if(end && end != text && end[0] == '\000' )
    return 0;
  else
    return 1;
}

/** @brief   text to double conversion
 *
 *  @return                            error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/03/18
 *  @since   2011/11/17 (Oyranos: 0.2.0)
 */
int          oyjlStringToDouble      ( const char        * text,
                                       double            * value )
{
  char * p = NULL, * t = NULL;
  int len;
  int error = -1;
#ifdef OYJL_HAVE_LOCALE_H
  char * save_locale = oyjlStringCopy( setlocale(LC_NUMERIC, 0 ), malloc );
  setlocale(LC_NUMERIC, "C");
#endif

  if(text && text[0])
    len = strlen(text);
  else
  {
    *value = NAN;
    error = 1;
    return error;
  }

  /* avoid irritating valgrind output of "Invalid read of size 8"
   * might be a glibc error or a false positive in valgrind */
  oyjlAllocHelper_m( t, char, len + 2*sizeof(double) + 1, malloc, return 1);
  memset( t, 0, len + 2*sizeof(double) + 1 );

  memcpy( t, text, len );

  *value = strtod( t, &p );

#ifdef OYJL_HAVE_LOCALE_H
  setlocale(LC_NUMERIC, save_locale);
  if(save_locale) free( save_locale );
#endif

  if(p && p != text && p[0] == '\000')
    error = 0;

  free( t );

  return error;
}

/** @brief read FILE into memory
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

#include <errno.h>
#include <sys/stat.h> /* stat() */
#define WARNc_S(...) oyjlMessage_p( oyjlMSG_ERROR, 0, __VA_ARGS__ )
int oyjlIsFileFull_ (const char* fullFileName, const char * read_mode)
{
  struct stat status;
  int r = 0;
  const char* name = fullFileName;

  memset(&status,0,sizeof(struct stat));
  r = stat (name, &status);

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
/** @} *//* oyjl_core */

/** \addtogroup oyjl
 *  @{ *//* oyjl */


#define OyjlToString2_M(t) OyjlToString_M(t)
#define OyjlToString_M(t) #t
/** @brief   init the libraries language; optionaly
 *
 *  Additionally use setlocale() somewhere in your application.
 *  The message catalog search path is detected from the project specific
 *  environment variable specified in \em env_var_locdir and
 *  the LOCPATH environment variables. If those are not present
 *  a expected fall back directory from \em default_locdir is used.
 *
 *  @param         project_name        project name display string; e.g. "MyProject"
 *  @param         environment_debug_variable string; e.g. "MP_DEBUG"
 *  @param         debug_variable      int C variable; e.g. my_project_debug
 *  @param         use_gettext         switch gettext support on or off
 *  @param         env_var_locdir      environment variable string for locale path; e.g. "MP_LOCALEDIR"
 *  @param         default_locdir      default locale path C string; e.g. "/usr/local/share/locale"
 *  @param         loc_domain          locale domain string related to your pot, po and mo files; e.g. "myproject"
 *  @param         msg                 your message function of type oyjlMessage_f
 *  @return                            error
 *                                     - -1 : issue
 *                                     - 0 : success
 *                                     - >= 1 : found error
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/01/13
 *  @since   2019/01/13 (Oyjl: 1.0.0)
 */
int oyjlInitLanguageDebug            ( const char        * project_name,
                                       const char        * env_var_debug,
                                       int               * debug_variable,
                                       int                 use_gettext,
                                       const char        * env_var_locdir,
                                       const char        * default_locdir,
                                       const char        * loc_domain,
                                       oyjlMessage_f       msg )
{
  int error = -1;

  if(getenv(env_var_debug))
  {
    *debug_variable = atoi(getenv(env_var_debug));
    if(*debug_variable)
    {
      int v = oyjlVersion(0);
      if(*debug_variable)
        msg( oyjlMSG_INFO, 0, "%s (Oyjl compile v: %s runtime v: %d)", project_name, OYJL_VERSION_NAME, v );
    }
  }

#ifdef OYJL_USE_GETTEXT
  {
    if( use_gettext )
    {
      char * var = NULL;
      const char * environment_locale_dir = NULL,
                 * locpath = NULL,
                 * path = NULL,
                 * domain_path = default_locdir;

      if(getenv(env_var_locdir) && strlen(getenv(env_var_locdir)))
      {
        environment_locale_dir = domain_path = strdup(getenv(env_var_locdir));
        if(*debug_variable)
          msg( oyjlMSG_INFO, 0,"found environment variable: %s=%s", env_var_locdir, domain_path );
      } else
        if(environment_locale_dir == NULL && getenv("LOCPATH") && strlen(getenv("LOCPATH")))
      {
        domain_path = NULL;
        locpath = getenv("LOCPATH");
        if(*debug_variable)
          msg( oyjlMSG_INFO, 0,"found environment variable: LOCPATH=%s", locpath );
      } else
        if(*debug_variable)
        msg( oyjlMSG_INFO, 0,"no %s or LOCPATH environment variable found; using default path: %s", env_var_locdir, domain_path );

      if(domain_path || locpath)
      {
        oyjlStringAdd( &var, 0,0, "NLSPATH=");
        oyjlStringAdd( &var, 0,0, domain_path ? domain_path : locpath);
      }
      if(var)
        putenv(var); /* Solaris */

      /* LOCPATH appears to be ignored by bindtextdomain("domain", NULL),
       * so it is set here to bindtextdomain(). */
      path = domain_path ? domain_path : locpath;
# ifdef OYJL_HAVE_LIBINTL_H
      bindtextdomain( loc_domain, path );
#endif
      if(*debug_variable)
      {\
        char * fn = NULL;
        int stat = -1;
        const char * gettext_call = OyjlToString2_M(_());

        if(path)
          oyjlStringAdd( &fn, 0,0, "%s/de/LC_MESSAGES/%s.mo", path ? path : "", loc_domain);
        if(fn)
          stat = oyjlIsFileFull_( fn, "r" );
        msg( oyjlMSG_INFO, 0,"bindtextdomain(\"%s\") to %s\"%s\" %s for %s", loc_domain, locpath?"effectively ":"", path ? path : "", (stat > 0)?"Looks good":"Might fail", gettext_call );
        if(fn) free(fn);
      }
    }
  }
#endif /* OYJL_USE_GETTEXT */

  return error;
}


#include "oyjl_version.h"
#include <yajl/yajl_parse.h>
#include <yajl/yajl_version.h>
/** @brief  give the compiled in library version
 *
 *  @param[in]  type           request API type
 *                             - 0 - Oyjl API
 *                             - 1 - Yajl API
 *  @return                    OYJL_VERSION at library compile time
 */
int            oyjlVersion           ( int                 type )
{
  if(type == 1)
    return YAJL_VERSION;

  return OYJL_VERSION;
}

/** @} *//* oyjl */
