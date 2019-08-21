/** @file oyjl_args.c
 *
 *  oyjl - UI helpers
 *
 *  @par Copyright:
 *            2018-2019 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl argument handling
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *
 * Copyright (c) 2018-2019  Kai-Uwe Behrmann  <ku.b@gmx.de>
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


#include <stddef.h>
#include <ctype.h> /* toupper() */
#include <wchar.h>  /* wcslen() */
#ifdef OYJL_HAVE_LANGINFO_H
#include <langinfo.h> /* nl_langinfo() */
#endif

#if defined(OYJL_INTERNAL)
#include "oyjl.h"
#include "oyjl_macros.h"
extern int * oyjl_debug;
#include "oyjl_i18n.h"
#include "oyjl_tree_internal.h"

#else /* OYJL_INTERNAL */
int my_debug = 0;
int * oyjl_debug = &my_debug;
#if defined(__GNUC__)
# define  OYJL_DBG_FORMAT "%s:%d %s() "
# define  OYJL_DBG_ARGS   strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__,__func__
#else
# define  OYJL_DBG_FORMAT "%s:%d "
# define  OYJL_DBG_ARGS   strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__
#endif
#define OYJL_CREATE_VA_STRING(format_, text_, alloc_, error_action) \
{ \
  va_list list; \
  size_t sz = 0; \
  int len = 0; \
  void*(* allocate_)(size_t size) = alloc_; \
  if(!allocate_) allocate_ = malloc; \
\
  text_ = NULL; \
  \
  va_start( list, format_); \
  len = vsnprintf( text_, sz, format_, list); \
  va_end  ( list ); \
\
  { \
    text_ = allocate_( sizeof(char) * len + 2 ); \
    if(!text_) \
    { \
      oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "could not allocate memory", OYJL_DBG_ARGS ); \
      error_action; \
    } \
    va_start( list, format_); \
    len = vsnprintf( text, len+1, format_, list); \
    va_end  ( list ); \
  } \
}

#define oyjlAllocHelper_m(ptr_, type, size_, alloc_func, action) { \
  if ((size_) <= 0) {                                       \
      oyjlMessage_p( oyjlMSG_INSUFFICIENT_DATA, 0, "Nothing to allocate"); \
  } else {                                                  \
      void*(*a)(size_t size) = alloc_func;                  \
      if(!a) a = malloc;                                    \
      ptr_ = (type*) a(sizeof (type) * (size_t)(size_));    \
      memset( ptr_, 0, sizeof (type) * (size_t)(size_) );   \
  }                                                         \
  if (ptr_ == NULL) {                                       \
      oyjlMessage_p( oyjlMSG_ERROR, 0, "Out of memory"); \
    action;                                                 \
  }                                                         \
}

extern oyjlMessage_f oyjlMessage_p;
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
oyjlMessage_f oyjlMessage_p = oyjlMessageFunc;
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
int        oyjlStringReplace         ( char             ** text,
                                       const char        * search,
                                       const char        * replacement,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  char * t = 0;
  const char * start, * end;
  int n = 0;

  void*(* allocate)(size_t size) = alloc?alloc:malloc;
  void (* deAllocate)(void * data ) = deAlloc?deAlloc:free;

  if(!text)
    return 0;

  start = end = *text;

  if(start && search && replacement)
  {
    int s_len = strlen(search);
    while((end = strstr(start,search)) != 0)
    {
      oyjlStringAddN( &t, start, end-start, allocate, deAllocate );
      oyjlStringAddN( &t, replacement, strlen(replacement), allocate, deAllocate );
      ++n;
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
    if(n && start && end == NULL)
      oyjlStringAddN( &t, start, strlen(start), allocate, deAllocate );
  }

  if(t)
  {
    deAllocate(*text);
    *text = t;
  }

  return n;
}

int        oyjlWStringLen            ( const char        * text )
{
  int len = strlen(text), wlen = 0;
  wchar_t * wcs = (wchar_t*) calloc( len + 1, sizeof(wchar_t) );
  if(wcs)
  {
    mbstowcs( wcs, text, len + 1 );
    wlen = wcslen(wcs);
    free(wcs);
  }
  return wlen;
}

struct oyjl_string_s
{
    char * s;                          /**< @brief UTF-8 text */
    size_t len;                        /**< @brief string length. */
    size_t alloc_len;                  /**< @brief last string allocation. */
    void*(*alloc)(size_t);             /**< @brief custom allocator; optional, default is malloc */
    void (*deAlloc)(void*);            /**< @brief custom deallocator; optional, default is free */
    int    alloc_count;
};
oyjl_str   oyjlStrNew                ( size_t              length,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  struct oyjl_string_s * string = NULL;
  if(!alloc) alloc = malloc;
  if(!deAlloc) deAlloc = free;

  oyjlAllocHelper_m( string, struct oyjl_string_s, 1, alloc, return NULL );
  string->len = 0;
  if(length == 0)
    length = 8;
  oyjlAllocHelper_m( string->s, char, length, alloc, return NULL );
  string->s[0] = '\000';
  string->alloc_len = length;
  string->alloc = alloc;
  string->deAlloc = deAlloc;
  string->alloc_count = 1;

  return (oyjl_str) string;
}
int        oyjlStrAppendN            ( oyjl_str            string,
                                       const char        * append,
                                       int                 append_len )
{
  struct oyjl_string_s * str = string;
  int error = 0;
  if(append && append_len)
  {
    if((append_len + str->len) >= str->alloc_len)
    {
      int len = (append_len + str->len) * 2;
      char * edit = str->s;
      oyjlAllocHelper_m( str->s, char, len, str->alloc, return 1 );
      str->alloc_len = len;
      ++str->alloc_count;
      memcpy(str->s, edit, str->len);
      str->deAlloc(edit);
    }
    memcpy( &str->s[str->len], append, append_len );
    str->len += append_len;
    str->s[str->len] = '\000';
  }
  return error;
}
int        oyjlStrAdd                ( oyjl_str            string,
                                       const char        * format,
                                                           ... )
{
  struct oyjl_string_s * str = string;
  char * text = 0;

  void*(* allocate)(size_t size) = str->alloc;
  void (* deAllocate)(void * data ) = str->deAlloc;

  OYJL_CREATE_VA_STRING(format, text, allocate, return 1)

  if(text)
  {
    oyjlStrAppendN( string, text, strlen(text) );
    deAllocate( text );
  }

  return 0;
}
int        oyjlStrReplace            ( oyjl_str            text,
                                       const char        * search,
                                       const char        * replacement,
                                       void             (* modifyReplacement)(const char * text, const char * start, const char * end, const char * search, const char ** replace, void * user_data),
                                       void              * user_data )
{
  struct oyjl_string_s * str = text;
  oyjl_str t = NULL;
  const char * start, * end;
  int n = 0;

  if(!text)
    return 0;

  start = end = oyjlStr(text);

  if(start && search && replacement)
  {
    int s_len = strlen(search);
    while((end = strstr(start,search)) != 0)
    {
      if(!t) t = oyjlStrNew(10,0,0);
      oyjlStrAppendN( t, start, end-start );
      if(modifyReplacement) modifyReplacement( oyjlStr(text), start, end, search, &replacement, user_data );
      oyjlStrAppendN( t, replacement, strlen(replacement) );
      ++n;
      if(strlen(end) >= (size_t)s_len)
        start = end + s_len;
      else
      {
        if(strstr(start,search) != 0)
          oyjlStrAppendN( t, replacement, strlen(replacement) );
        start = end = end + s_len;
        break;
      }
    }
    if(n && start && end == NULL)
      oyjlStrAppendN( t, start, strlen(start) );
  }

  if(t)
  {
    void (* deAlloc)(void*) = str->deAlloc;
    if(str->s && str->alloc_len) deAlloc(str->s);
    str->len = str->alloc_len = 0;

    deAlloc = t->deAlloc;
    if(str->alloc == t->alloc)
    {
      str->s = t->s;
      str->len = t->len;
      str->alloc_len = t->alloc_len;
      str->alloc_count = t->alloc_count;
      deAlloc(t); t = NULL;
    } else
    {
      int length = 8;
      oyjlAllocHelper_m( str->s, char, length, str->alloc, return 0 );
      str->s[0] = '\000';
      str->alloc_len = length;
      oyjlStrAppendN( str, oyjlStr(t), strlen(oyjlStr(t)) );
      oyjlStrRelease( &t );
    }
  }

  return n;
}
char *     oyjlStrPull               ( oyjl_str            str )
{
  struct oyjl_string_s * string;
  char * t = NULL;
  int length = 8;

  if(!str) return t;

  string = str;
  t = string->s;
  string->s = NULL;

  string->len = 0;
  oyjlAllocHelper_m( string->s, char, length, string->alloc, return NULL );
  string->s[0] = '\000';
  string->alloc_len = length;
  string->alloc_count = 1;
  
  return t;
}
void       oyjlStrClear              ( oyjl_str            string )
{
  struct oyjl_string_s * str = string;
  void (* deAlloc)(void*) = str->deAlloc;
  char * s = oyjlStrPull( string );
  if(s) deAlloc(s);
}
void       oyjlStrRelease            ( oyjl_str          * string_ptr )
{
  struct oyjl_string_s * str;
  if(!string_ptr) return;
  str = *string_ptr;
  void (* deAlloc)(void*) = str->deAlloc;
  if(str->s) deAlloc(str->s);
  deAlloc(str);
  *string_ptr = NULL;
}

const char*oyjlStr                   ( oyjl_str            string )
{
  return (const char*)string->s;
}
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
int          oyjlStringToDouble      ( const char        * text,
                                       double            * value )
{
  char * p = NULL, * t = NULL;
  int len, pos = 0;
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

  /* remove leading empty space */
  while(text[pos] && isspace(text[pos])) pos++;
  memcpy( t, &text[pos], len );

  *value = strtod( t, &p );

#ifdef OYJL_HAVE_LOCALE_H
  setlocale(LC_NUMERIC, save_locale);
  if(save_locale) free( save_locale );
#endif

  if(p && p != t && p[0] == '\000')
    error = 0;
  else if(p && p == t)
  {
    *value = NAN;
    error = 2;
  }

  free( t );

  return error;
}
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

  if(!deAlloc) deAlloc = free;
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
const char *   oyjlStringGetNext     ( const char        * text )
{
  /* remove leading white space */
  if(text && text[0] && isspace(text[0]))
  {
    while( text && text[0] && isspace(text[0]) ) text++;
    return text;
  }

  /* find the end of the word */
  while( text && text[0] && !isspace(text[0]) ) text++;

  /* find the next word */
  while( text && text[0] && isspace(text[0]) ) text++;

  return text && text[0] ? text : NULL;
}
int            oyjlStringNextSpace   ( const char        * text )
{
  int len = 0;
  while( text && text[len] && !isspace(text[len]) ) len++;
  return len;
}
char **        oyjlStringSplitSpace  ( const char        * text,
                                       int               * count,
                                       void*            (* alloc)(size_t))
{
  char ** list = NULL;
  int n = 0, i;

  /* split the string by empty space */
  if(text && text[0])
  {
    const char * tmp = text;

    if(!alloc) alloc = malloc;

    if(tmp && tmp[0] && !isspace(tmp[0])) ++n;
    while( tmp && tmp[0] && (tmp = oyjlStringGetNext( tmp )) != NULL ) ++n;

    if((list = alloc( (n+1) * sizeof(char*) )) == 0) return NULL;
    memset( list, 0, (n+1) * sizeof(char*) );

    {
      const char * start = text;
      if(start && isspace(start[0]))
        start = oyjlStringGetNext( start );

      for(i = 0; i < n; ++i)
      {
        int len = oyjlStringNextSpace( start );

        if((list[i] = alloc( len+1 )) == 0) return NULL;

        memcpy( list[i], start, len );
        list[i][len] = 0;
        start = oyjlStringGetNext( start );
      }
    }
  }

  if(count)
    *count = n;

  return list;
}

const char * oyjlStringDelimiter ( const char * text, const char * delimiter, int * length )
{
  int i,j, dn = delimiter ? strlen(delimiter) : 0, len = text?strlen(text):0;
  for(j = 0; j < len; ++j)
    for(i = 0; i < dn; ++i)
      if(text[j] && text[j] == delimiter[i])
      {
        if(length)
          *length = j;
        return &text[j];
      }
  return NULL;
}
char **        oyjlStringSplit2      ( const char        * text,
                                       const char        * delimiter,
                                       int               * count,
                                       int              ** index,
                                       void*            (* alloc)(size_t))
{
  char ** list = 0;
  int n = 0, i;

  /* split the path search string by a delimiter */
  if(text && text[0])
  {
    const char * tmp = text;

    if(!alloc) alloc = malloc;

    if(!delimiter)
      return oyjlStringSplitSpace( text, count, alloc );

    tmp = oyjlStringDelimiter(tmp, delimiter, NULL);
    if(tmp == text) ++n;
    tmp = text;
    do { ++n;
    } while( (tmp = oyjlStringDelimiter(tmp + 1, delimiter, NULL)) );

    tmp = 0;

    if((list = alloc( (n+1) * sizeof(char*) )) == NULL) return NULL;
    memset( list, 0, (n+1) * sizeof(char*) );
    if(index && (*index = alloc( n * sizeof(int) )) == NULL) { free(list); return NULL; }
    if(index) memset( *index, 0, n * sizeof(int) );

    {
      const char * start = text;
      for(i = 0; i < n; ++i)
      {
        intptr_t len = 0;
        int length = 0;
        const char * end = oyjlStringDelimiter(start, delimiter, &length);
        if(index && length) (*index)[i] = length + start - text;

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
#include "oyjl_version.h"
int            oyjlVersion           ( int                 type OYJL_UNUSED )
{
  return OYJL_VERSION;
}
#define WARNc_S(...) oyjlMessage_p( oyjlMSG_ERROR, 0, __VA_ARGS__ )
#include <sys/stat.h> /* stat() */
#include <errno.h>
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
static char * oyjl_nls_path_ = NULL;
void oyjlLibRelease() { if(oyjl_nls_path_) { putenv("NLSPATH=C"); free(oyjl_nls_path_); } }
#define OyjlToString2_M(t) OyjlToString_M(t)
#define OyjlToString_M(t) #t
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

  if(!msg) msg = oyjlMessage_p;

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
      char * var = NULL, * tmp = NULL;
      const char * environment_locale_dir = NULL,
                 * locpath = NULL,
                 * path = NULL,
                 * domain_path = default_locdir;

      if(getenv(env_var_locdir) && strlen(getenv(env_var_locdir)))
      {
        tmp = strdup(getenv(env_var_locdir));
        environment_locale_dir = domain_path = tmp;
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
      if(var) /* do not release */
      {
        putenv(var); /* Solaris */
        if(oyjl_nls_path_)
          free(oyjl_nls_path_);
        oyjl_nls_path_ = var;
      }

      /* LOCPATH appears to be ignored by bindtextdomain("domain", NULL),
       * so it is set here to bindtextdomain(). */
      path = domain_path ? domain_path : locpath;
# ifdef OYJL_HAVE_LIBINTL_H
      bindtextdomain( loc_domain, path );
#endif
      if(*debug_variable)
      {
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
      if(tmp)
        free(tmp);
    }
  }
#endif /* OYJL_USE_GETTEXT */

  return error;
}

typedef struct {
  char       ** options; /* detected vanilla args + probably "@" for anonymous args */
  const char ** values; /* the vanilla args from main(argv[]) */
  int           count; /* number of detected options */
} oyjlOptsPrivate_s;
#endif /* OYJL_INTERNAL */

#ifdef OYJL_HAVE_LANGINFO_H
#include <langinfo.h> /* nl_langinfo() */
#endif

/** \addtogroup oyjl_args Options Handling
 *  @brief   Structured Options and Arguments for many aspects on command line and more in a single file
 *
 *  @section args_intro Introduction
 *  Oyjl argument handling uses a compact, table like creation syntax.
 *  oyjlUi_Create() is a high level API for tools to feed only the
 *  declarations and obtain the results in one call. On a lower level parsing
 *  with oyjlOptions_Parse() detects conflicts during programming and
 *  on run time. The arguments can be printed as a typical command line tool
 *  help text with oyjlOptions_PrintHelp(). The Oyjl JSON
 *  output from oyjlUi_ToJson() is useable for automatical generated
 *  (G)UI's and further processing. oyjlUi_ToMan() creates unix manual pages.
 *  oyjlUi_ToMarkdown() lets you produce a man page for your web page.
 *  Generation of other formats is simple. The implementation in *oyjl_args.c*
 *  can simply be included or use oyjl.h and link to *libOyjlCore*.
 *  Translations are supported by *oyjl-tanslate* tool through gettext.
 *
 *  Command line options support single letter in oyjlOption_s::o and
 *  long options in oyjlOption_s::option without
 *  and with empty space and equal sign style single argument.
 *  @verbatim
    > myProgramm -o --option -w=1 --with-argument 1
    @endverbatim
 *
 *  @section args_tut Tutorial
 *  from @ref test-core.c :
    @dontinclude test-core.c
    @skip handle options
    @until done with options handling
 *
 *  @section args_orga Option Organisation - Groups
 *  Options can be read pure informational or express constraints.
 *  A **action** is a way to place the affected option(s) into the result, where
 *  the **result** can be the return of a textual structure or a executed
 *  command line call.
 *  The oyjlOptionGroup_s declares the layout and a simple syntax.
 *  In case the oyjlOptionGroup_s::mandatory or
 *  oyjlOptionGroup_s::optional members contain options, a action should be
 *  displayed inside the group. In absence of oyjlOptionGroup_s::mandatory
 *  options, the oyjlOptionGroup_s::optional options are handled like a
 *  single group and a widget for starting a action is needed.
 *  The oyjlOptionGroup_s::detail simply
 *  tells about displaying of a option inside a group.
 *  All oyjlOptionGroup_s::mandatory options shall be inside the
 *  oyjlOptionGroup_s::detail array for simplicity of man page generation.
 *
 *  @subsection args_single_call One Action Group
 *  Very simple tools might have few optional arguments. It makes
 *  sense to have no constraints in groups. So there is no mandatory
 *  option at all needed. All options shall be applied at once. Example:
 *  @verbatim
    tool [-a] [-b] [-c] [-v]
    @endverbatim
 @code
    oyjlOptionGroup_s groups[] = {
    // type,   flags, name,     description,help, mandatory,  optional, detail
      // place here the action widget; all optional options [a+b+c+v] are in this action group
      {"oiwg", 0,     "Group1", 0,0,              "",         "a,b,c,v","a,b" },
      // only description; no action
      {"oiwg", 0,     "Group2", 0,0,              "",         "",       "c" },
      // only description; no action
      {"oiwg", 0,     "Common", 0,0,              "",         "",       "v" },
      {0,0,0,0,0,0,0,0}
    } // note: no mandatory option is named
 @endcode
 *  The GUI should show one over all action widget, e.g. press button, in the
 *  group(s) with optional options.
 *  This mode implies the empty option mark '#' needs to be specified, as the
 *  tool needs no mandatory option(s).
 *
 *  @subsection args_group_modes Grouped Modes
 *  More complex tools might have different modes, in order to do one
 *  task. The 'tar' tool is organised to do belonging tasks as a task.
 *  Each group is introduced by a mandatory option and might be followed
 *  by some more optional options. Options can be expluced from groups.
 *  Some more options can by applied to all groups. Example:
 *  @verbatim
    tool -a -b [-c] [-v]
    tool -h [-v]
    @endverbatim
 @code
    oyjlOptionGroup_s groups[] = {
    // type,   flags, name,     description,help, mandatory,  optional, detail
      // a separate action widget is needed, so 'a' and 'b' can be set before action; a+b+c+[v] are in this action group
      {"oiwg", 0,     "Group1", 0,0,              "a,b",      "c,v",    "a,b,c" },
      // the 'h' option could be handled as the action widget; h+[v] are in this action group
      {"oiwg", 0,     "Group2", 0,0,              "h",        "v",      "h" },
      // only description; no action
      {"oiwg", 0,     "Common", 0,0,              "",         "",       "v" },
      {0,0,0,0,0,0,0,0}
    }
 @endcode
 *  The GUI should show one action widget, e.g. press button, per group.
 *
 *  @subsection args_group_independence Independent Options
 *  Each option is independent. However it is possible to group them
 *  in a pure informational way. E.g.
 *  @verbatim
    tool -a|-b [-v]
    tool -h [-v]
    @endverbatim
 *  The above example knows three basic modes: a, b and h. The -v
 *  option might be applied to all of them. Each of a,b,h is called
 *  independently and that means the GUI reacts instantly on any changed state
 *  in them. A group object might look like:
 @code
    oyjlOptionGroup_s groups[] = {
    // type,   flags, name,     description,help, mandatory,  optional, detail
      // 'a' and 'b' can be action widgets, as they are independent; a+[v] or b+[v] are in this action group
      {"oiwg", 0,     "Group1", 0,0,              "a|b",      "v",      "a,b" },
      // 'h' can be action widget; h+[v] are in this action group
      {"oiwg", 0,     "Group2", 0,0,              "h",        "v",      "h" },
      // only description; no action
      {"oiwg", 0,     "Common", 0,0,              "",         "",       "v" },
      {0,0,0,0,0,0,0,0}
    } // note: all manatory options are ored '|' or single in the group
 @endcode
 *  The GUI should show one action widget, e.g. press button, per option.
 *  All options in oyjlOptionGroup_s::mandatory should be or'ed with '|',
 *  otherwise the individial actions make no sense.
 *
 *  @section args_renderer Viewers
 *  The option tables allow for different views. Those can be pure
 *  informational, like man pages created with the *cli -X man* command.
 *  Or a view can be interactive and call the command with the options.
 *
 *  @subsection args_interactive Interactive Viewers
 *  A example of a interactive viewer is the included *oyjl-args-qml* renderer.
 *  Tools have to be more careful, in case they want to be displayed by
 *  a interactive viewer. They should declare, in which order options
 *  apply and add command line information for the *-X json+command* option.
 *
 *  A GUI renderer might display the result of a command immediately. The
 *  simplest form is plain text output. The text font should eventually be
 *  a monotyped on, like typical for interactive shells. This builds as well
 *  a command like aesthetic. Some tools return graphics in multiline
 *  ASCII or use color codes.
 *  The tool might output HTML and should be displayed formatted. Some tools
 *  stream their result as image to stdout. A oyjl-args-qml viewer supports
 *  PNG image output.
 *
 *  The *-X json* and *-X json+command* options are only available with
 *  *libOyjlCore* and **not** in the stand alone version with *oyjl_args.c*
 *  inclusion.
 *
 *  @subsection args_developer Developer Support
 *  The oyjlUi_ExportToJson() API allows to dump all data toward a JSON
 *  representation. The function is contained in *libOyjlCore*.
 *  The oyjlUiJsonToCode() API converts a JSON UI data representation to
 *  source code. It is used inside the *oyjl-args* tool.
 *
 *  @{ */

/** @brief    Release dynamic structure
 *  @memberof oyjlOptionChoice_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
void oyjlOptionChoice_Release     ( oyjlOptionChoice_s**choices )
{
  int n = 0,i;
  oyjlOptionChoice_s * ca;
  if(!choices || !*choices) return;
  ca = *choices;
  while(ca[n].nick && ca[n].nick[0] != '\000') ++n;
  for(i = 0; i < n; ++i)
  {
    oyjlOptionChoice_s * c = &ca[i];
    if(c->nick) free(c->nick);
    if(c->name) free(c->name);
    if(c->description) free(c->description);
    if(c->help) free(c->help);
  }
  *choices = NULL;
  free(*choices);
}

#ifndef OYJL_CTEND
/* true color codes */
#define OYJL_RED_TC "\033[38;2;240;0;0m"
#define OYJL_GREEN_TC "\033[38;2;0;250;100m"
#define OYJL_BLUE_TC "\033[38;2;0;150;255m"
/* basic color codes */
#define OYJL_BOLD "\033[1m"
#define OYJL_ITALIC "\033[3m"
#define OYJL_UNDERLINE "\033[4m"
#define OYJL_RED_B "\033[0;31m"
#define OYJL_GREEN_B "\033[0;32m"
#define OYJL_BLUE_B "\033[0;34m"
/* switch back */
#define OYJL_CTEND "\033[0m"
typedef enum {
  oyjlRED,
  oyjlGREEN,
  oyjlBLUE,
  oyjlBOLD,
  oyjlITALIC,
  oyjlUNDERLINE
} oyjlCOLORTERM_e;
#endif
static const char * oyjlTermColor( oyjlCOLORTERM_e rgb, const char * text) {
  int len = strlen(text);
  static char t[256];
  static int colorterm_init = 0;
  static const char * oyjl_colorterm = NULL;
  static int truecolor = 0,
             color = 0;
  if(!colorterm_init)
  {
    colorterm_init = 1;
    oyjl_colorterm = getenv("COLORTERM");
    color = oyjl_colorterm != NULL ? 1 : 0;
    if(!oyjl_colorterm) oyjl_colorterm = getenv("TERM");
    truecolor = oyjl_colorterm && strcmp(oyjl_colorterm,"truecolor") == 0;
  }
  if(len < 200)
  {
    switch(rgb)
    {
      case oyjlRED: sprintf( t, "%s%s%s", truecolor ? OYJL_RED_TC : color ? OYJL_RED_B : "", text, OYJL_CTEND ); break;
      case oyjlGREEN: sprintf( t, "%s%s%s", truecolor ? OYJL_GREEN_TC : color ? OYJL_GREEN_B : "", text, OYJL_CTEND ); break;
      case oyjlBLUE: sprintf( t, "%s%s%s", truecolor ? OYJL_BLUE_TC : color ? OYJL_BLUE_B : "", text, OYJL_CTEND ); break;
      case oyjlBOLD: sprintf( t, "%s%s%s", truecolor || color ? OYJL_BOLD : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
      case oyjlITALIC: sprintf( t, "%s%s%s", truecolor || color ? OYJL_ITALIC : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
      case oyjlUNDERLINE: sprintf( t, "%s%s%s", truecolor || color ? OYJL_UNDERLINE : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
    }
    return t;
  } else
    return text;
}

/** @brief    Return number of array elements
 *  @memberof oyjlOptionChoice_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/06/17
 *  @since   2019/06/17 (Oyjl: 1.0.0)
 */
int  oyjlOptionChoice_Count          ( oyjlOptionChoice_s* list )
{
  int n = 0;
  if(list)
    while(list[n].nick && list[n].nick[0] != '\000')
      ++n;
  return n;
}


/** @brief    Return number of "oiwi" array elements
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
int oyjlOptions_Count             ( oyjlOptions_s  * opts )
{
  int n = 0;
  while( *(oyjlOBJECT_e*)&opts->array[n] /*"oiwi"*/ == oyjlOBJECT_OPTION) ++n;
  return n;
}

/** @brief    Return number of "oiwi" groups elements
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
int oyjlOptions_CountGroups       ( oyjlOptions_s  * opts )
{
  int n = 0;
  while( *(oyjlOBJECT_e*)&opts->groups[n] /*"oiwg"*/ == oyjlOBJECT_OPTION_GROUP) ++n;
  return n;
}

#define OYJL_E(x_) (x_?x_:"")
#define OYJL_IS_NOT_O( x ) (!o->o || strcmp(o->o,x) != 0)
#define OYJL_IS_O( x ) (o->o && strcmp(o->o,x) == 0)
enum {
  oyjlOPTIONSTYLE_ONELETTER = 0x01,
  oyjlOPTIONSTYLE_STRING = 0x02,
  oyjlOPTIONSTYLE_OPTIONAL_START = 0x04,
  oyjlOPTIONSTYLE_OPTIONAL_END = 0x08,
  oyjlOPTIONSTYLE_OPTIONAL_INSIDE_GROUP = 0x10,
  oyjlOPTIONSTYLE_MAN = 0x20,
  oyjlOPTIONSTYLE_MARKDOWN = 0x40
};
#define oyjlOPTIONSTYLE_OPTIONAL (oyjlOPTIONSTYLE_OPTIONAL_START | oyjlOPTIONSTYLE_OPTIONAL_END)
static
char *       oyjlOption_PrintArg     ( oyjlOption_s      * o,
                                       int                 style )
{
  char * text = NULL;
  if(!o) return oyjlStringCopy("", malloc);
  if(style & oyjlOPTIONSTYLE_OPTIONAL_START)
    oyjlStringAdd( &text, malloc, free, "[" );
  if(style & oyjlOPTIONSTYLE_ONELETTER && o->o && OYJL_IS_NOT_O("@") && OYJL_IS_NOT_O("#"))
  {
    if(style & oyjlOPTIONSTYLE_MAN)
      oyjlStringAdd( &text, malloc, free, "\\-%s", o->o );
    else if(style & oyjlOPTIONSTYLE_MARKDOWN)
      oyjlStringAdd( &text, malloc, free, "<strong>-%s</strong>", o->o );
    else
      oyjlStringAdd( &text, malloc, free, "-%s", o->o );
  }
  if(style & oyjlOPTIONSTYLE_ONELETTER && OYJL_IS_O("#"))
    oyjlStringAdd( &text, malloc, free, "|" );
  if(style & oyjlOPTIONSTYLE_ONELETTER && style & oyjlOPTIONSTYLE_STRING && OYJL_IS_NOT_O("@") && OYJL_IS_NOT_O("#") && o->o && o->o[0] && o->option)
    oyjlStringAdd( &text, malloc, free, "|" );
  if( o->option &&
      ( style & oyjlOPTIONSTYLE_STRING ||
        ( !(o->o && o->o[0]) &&
          OYJL_IS_NOT_O("@") &&
          OYJL_IS_NOT_O("#") )))
  {
    if(style & oyjlOPTIONSTYLE_MAN)
      oyjlStringAdd( &text, malloc, free, "\\-\\-%s", o->option );
    else if(style & oyjlOPTIONSTYLE_MARKDOWN)
      oyjlStringAdd( &text, malloc, free, "<strong>--%s</strong>", o->option );
    else
      oyjlStringAdd( &text, malloc, free, "--%s", o->option );
  }
  if(o->value_name)
  {
    if(style & oyjlOPTIONSTYLE_MAN)
      oyjlStringAdd( &text, malloc, free, "%s\\fI%s\\fR", OYJL_IS_NOT_O("@") && OYJL_IS_NOT_O("#") ? " ":"", o->value_name );
    else
    {
      if(style & oyjlOPTIONSTYLE_MARKDOWN)
        oyjlStringAdd( &text, malloc, free, "%s<em>%s</em>", OYJL_IS_NOT_O("@") && !(style & oyjlOPTIONSTYLE_STRING)?"=":" ", o->value_name ); /* allow for easier word wrap in table */
      else if(style & oyjlOPTIONSTYLE_OPTIONAL_INSIDE_GROUP)
        oyjlStringAdd( &text, malloc, free, "%s", oyjlTermColor(oyjlITALIC,o->value_name) );
      else
        oyjlStringAdd( &text, malloc, free, "%s%s", (!o->o || strcmp(o->o, "@")) != 0?"=":"", oyjlTermColor(oyjlITALIC,o->value_name) );
    }
  }
  if(style & oyjlOPTIONSTYLE_OPTIONAL_END)
    oyjlStringAdd( &text, malloc, free, "]" );
  return text;
}

oyjlOptionChoice_s oyjl_X_choices[] = {
                                    {"man", "", "", ""},
                                    {"markdown", "", "", ""},
                                    {"json", "", "", ""},
                                    {"json+command", "", "", ""},
                                    {"export", "", "", ""},
                                    {"","","",""}};
void oyjlOptions_EnrichInbuild( oyjlOption_s * o )
{
  if(strcmp(o->o, "X") == 0)
  {
    if(o->value_type == oyjlOPTIONTYPE_CHOICE && o->values.choices.list == NULL)
    {
      oyjl_X_choices[0].name = _("Man");
      oyjl_X_choices[0].description = _("Unix Man page");
      oyjl_X_choices[0].help = _("Get a unix man page");
      oyjl_X_choices[1].name = _("Markdown");
      oyjl_X_choices[1].description = _("Formated text");
      oyjl_X_choices[1].help = _("Get formated text");
#if defined(OYJL_INTERNAL)
      oyjl_X_choices[2].name = _("Json");
      oyjl_X_choices[2].description = _("GUI");
      oyjl_X_choices[2].help = _("Get a Oyjl Json UI declaration");
      oyjl_X_choices[3].name = _("Json + Command");
      oyjl_X_choices[3].description = _("GUI + Command");
      oyjl_X_choices[3].help = _("Get Oyjl Json UI declaration incuding command");
      oyjl_X_choices[4].name = _("Export");
      oyjl_X_choices[4].description = _("All available data");
      oyjl_X_choices[4].help = _("Get UI data for developers");
#else
      oyjl_X_choices[2].nick = "";
      oyjl_X_choices[3].nick = "";
      oyjl_X_choices[4].nick = "";
#endif
      o->values.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)oyjl_X_choices, sizeof(oyjl_X_choices), malloc );
      if(o->value_name == NULL)
      {
#if defined(OYJL_INTERNAL)
        o->value_name = "json|json+command|man|markdown";
#else
        o->value_name = "man|markdown";
#endif
        if(o->name == NULL)
        {
          o->name = _("export");
          if(o->description == NULL)
          {
            o->description = _("Export formated text");
            if(o->help == NULL)
              o->help = _("Get UI converted into text formats");
          }
        }
      }
    }
  }
}

static int oyjlStringDelimiterCount ( const char * text, const char * delimiter )
{
  int i,j, dn = delimiter ? strlen(delimiter) : 0, len = text?strlen(text):0, n = 0;
  if(len) ++n;
  for(j = 0; j < len; ++j)
    for(i = 0; i < dn; ++i)
      if(text[j] && text[j] == delimiter[i])
        ++n;
  return n;
}

/** @brief    Obtain the specified option from one letter member::o
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/07/29
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlOption_s * oyjlOptions_GetOption ( oyjlOptions_s     * opts,
                                       const char        * ol )
{
  int i;
  int nopts = oyjlOptions_Count( opts );
  oyjlOption_s * o = NULL;

  if(!ol) return o;

  for(i = 0; i < nopts; ++i)
  {
    o = &opts->array[i];
    if(o->o && strcmp(o->o, ol) == 0)
    {
      if(strcmp(ol, "X") == 0 && o->value_type == oyjlOPTIONTYPE_CHOICE && o->values.choices.list == NULL)
        oyjlOptions_EnrichInbuild(o);

      return o;
    }
    else
      o = NULL;
  }
  return o;
}

/** @brief    Obtain the specified option from option string
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/08/04
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlOption_s * oyjlOptions_GetOptionL( oyjlOptions_s     * opts,
                                       const char        * ostring )
{
  int i;
  int nopts = oyjlOptions_Count( opts );
  oyjlOption_s * o = NULL;
  char * str = oyjlStringCopy(ostring, malloc);
  char * t = strchr(str, '=');
  char ol[8];

  memset(ol, 0, 8);
  if(t)
    t[0] = '\000';

  if(str[0])
  {
    int len = strlen(str);
    if(len == 1)
      strcpy( ol, str );
    else
    {
      int l = 0;
      oyjlStringDelimiter(str, "|,", &l);
      if(l > 0)
        str[l] = '\000';

      if(oyjlWStringLen(str) == 1)
        strcpy( ol, str );
    }
  }

  for(i = 0; i < nopts; ++i)
  {
    o = &opts->array[i];
    if( (ol[0] && o->o && strcmp(o->o, ol) == 0) ||
        (!ol[0] && o->option && strcmp(o->option, str) == 0)
      )
    {
      free(str);
      return o;
    }
    else
      o = NULL;
  }
  fprintf( stderr, "Option not found: %s\n", str );
  free(str);

  return o;
}
static
oyjlOPTIONSTATE_e oyjlOptions_Check (
                                       oyjlOptions_s  * opts )
{
  int i,j;
  int nopts = oyjlOptions_Count( opts );
  oyjlOption_s * o = NULL, * b = NULL;

  for(i = 0; i < nopts; ++i)
  {
    o = &opts->array[i];
    for(j = i+1; j < nopts; ++j)
    {
      b = &opts->array[j];
      if(o->o && b->o && strcmp(o->o, b->o) == 0)
      {
        fprintf( stderr, "%s %s \'%s\'\n", _("Usage Error:"), _("Double occuring option"), b->o );
        return oyjlOPTION_DOUBLE_OCCURENCE;
      }

    }
    /* some error checking */
    /*if(!(('0' <= o->o && o->o <= '9') ||
         ('a' <= o->o && o->o <= 'z') ||
         ('A' <= o->o && o->o <= 'Z') ||
         o->o == '@' || o->o == '#' || o->o == '\000' || o->o == '|'))
    {
      fprintf( stderr, "%s %s \'%s\' %s\n", _("Usage Error:"), _("Option not supported"), o->o, _("need range 0-9 or a-z or A-Z") );
      return oyjlOPTION_NOT_SUPPORTED;
    }*/
    if(OYJL_IS_NOT_O("#") && o->value_name && o->value_name[0] && o->value_type == oyjlOPTIONTYPE_NONE)
    {
      fprintf( stderr, "%s %s \'%s\': %s\n", _("Usage Error:"), _("Option not supported"), o->o, _("need a value_type") );
      return oyjlOPTION_NOT_SUPPORTED;
    }
    if( OYJL_IS_NOT_O("#") &&
        OYJL_IS_NOT_O("X") &&
        o->value_type == oyjlOPTIONTYPE_CHOICE &&
        !((o->flags & OYJL_OPTION_FLAG_EDITABLE) || o->values.choices.list))
    {
      fprintf( stderr, "%s %s \'%s\' %s\n", _("Usage Error:"), _("Option not supported"), o->o, _("needs OYJL_OPTION_FLAG_EDITABLE or choices") );
      return oyjlOPTION_NOT_SUPPORTED;
    }
  }
  return oyjlOPTION_NONE;
}

/* list of static strings */
void       oyjlStringListStaticAdd   ( const char      *** list,
                                       int               * n,
                                       const char        * string,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  const char ** nlist = 0;
  int n_alt;

  if(!list || !n) return;

  if(!deAlloc) deAlloc = free;
  n_alt = *n;

  oyjlAllocHelper_m(nlist, const char*, n_alt + 2, alloc, return);

  memmove( nlist, *list, sizeof(const char*) * n_alt);
  nlist[n_alt] = string;
  nlist[n_alt+1] = NULL;

  *n = n_alt + 1;

  if(*list)
    deAlloc(*list);

  *list = nlist;
}

/** @brief    Parse the options into a private data structure
 *  @memberof oyjlOptions_s
 *
 *  The returned status can be used to detect usage errors and hint them on
 *  the command line.
 *  In the usual case where the variable fields are set, the results
 *  will be set too.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/08/06
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlOPTIONSTATE_e oyjlOptions_Parse  ( oyjlOptions_s     * opts )
{
  oyjlOPTIONSTATE_e state = oyjlOPTION_NONE;
  oyjlOption_s * o;
  oyjlOptsPrivate_s * result;

  /* parse the command line arguments */
  if(opts && !opts->private_data)
  {
    int i, len = opts->argc + 1;
    result = (oyjlOptsPrivate_s*) calloc( 1, sizeof(oyjlOptsPrivate_s) );
    /* The first string contains the detected single char options */
    result->options = (char**) calloc( len + 1, sizeof(char*) );
    result->values = (const char**) calloc( len + 1, sizeof(char*) );
    if((state = oyjlOptions_Check(opts)) != oyjlOPTION_NONE)
      goto clean_parse;
    for(i = 1; i < opts->argc; ++i)
    {
      const char * str = opts->argv[i];
      int l = strlen(str);
      const char * long_arg = NULL;
      const char * value = NULL;

      if(strstr(str,"-qmljsdebugger") != NULL) /* detect a QML option */
        continue;

      /* parse -a | -a value | -a=value | -ba | -ba value | -ba=value */
           if(l > 1 && str[0] == '-' && str[1] != '-')
      {
        int require_value, j;
        for(j = 1; j < l; ++j)
        {
          char arg[8] = {str[j],0,0,0,0,0,0,0};
          o = oyjlOptions_GetOption( opts, arg );
          if(!o)
          {
            fprintf( stderr, "%s %s \'%s\'\n", _("Usage Error:"), _("Option not supported"), arg );
            state = oyjlOPTION_NOT_SUPPORTED;
            break;
          }
          require_value = o->value_type != oyjlOPTIONTYPE_NONE;
          if( require_value )
          {
            value = NULL;
            if( j == l-1 && opts->argc > i+1 && (opts->argv[i+1][0] != '-' || strlen(opts->argv[i+1]) <= 1) )
            {
              value = opts->argv[i+1];
              ++i;
            }
            else if( str[j+1] == '=' )
            {
              ++j;
              value = &str[j+1];
              j = l;
            }
            else
            {
              fprintf( stderr, "%s %s \'%s\'\n", _("Usage Error:"), _("Option needs a argument"), arg );
              state = oyjlOPTION_MISSING_VALUE;
            }
            if(value)
            {
              result->options[result->count] = strdup(o->o?o->o:o->option);
              result->values[result->count] = value;
              ++result->count;
            }
          }
          else if(!require_value && !(j < l-1 && str[j+1] == '='))
          {
            result->options[result->count] = strdup(o->o?o->o:o->option);
            result->values[result->count] = "1";
            ++result->count;
          }
          else
          {
            int i;
            for(i = 0; i < opts->argc; ++i)
              fprintf( stderr, "\'%s\' ", opts->argv[i]);
            fprintf( stderr, "\n");
            fprintf( stderr, "%s %s \'%s\'\n", _("Usage Error:"), _("Option has a unexpected argument"), arg );
            state = oyjlOPTION_UNEXPECTED_VALUE;
            j = l;
          }
        }
      }
      /* parse --arg | --arg value | --arg=value */
      else if(l > 2 && str[0] == '-' && str[1] == '-')
      {
        int require_value;
        long_arg = &str[2];
        o = oyjlOptions_GetOptionL( opts, long_arg );
        if(!o)
        {
          fprintf( stderr, "%s %s \'%s\'\n", _("Usage Error:"), _("Option not supported"), long_arg );
          state = oyjlOPTION_NOT_SUPPORTED;
          goto clean_parse;
        }
        require_value = o->value_type != oyjlOPTIONTYPE_NONE;
        if( require_value )
        {
          value = NULL;

          if( strchr(str, '=') != NULL )
            value = strchr(str, '=') + 1;
          else if( opts->argc > i+1 && opts->argv[i+1][0] != '-' )
          {
            value = opts->argv[i+1];
            ++i;
          }
          else
          {
            fprintf( stderr, "%s %s \'%s\'\n", _("Usage Error:"), _("Option needs a argument"), long_arg );
            state = oyjlOPTION_MISSING_VALUE;
          }

          if(value)
          {
            result->options[result->count] = strdup(o->o?o->o:o->option);
            result->values[result->count] = value;
            ++result->count;
          }
        } else
        {
          if(!( strchr(str, '=') != NULL || (opts->argc > i+1 && opts->argv[i+1][0] != '-') ))
          {
            result->options[result->count] = strdup(o->o?o->o:o->option);
            result->values[result->count] = "1";
            ++result->count;
          } else
          {
            fprintf( stderr, "%s %s \'%s\'\n", _("Usage Error:"), _("Option has a unexpected argument"), opts->argv[i+1] );
            state = oyjlOPTION_UNEXPECTED_VALUE;
          }
        }
      }
      /* parse anonymous value, if requested */
      else
      {
        result->options[result->count] = strdup("@");
        o = oyjlOptions_GetOption( opts, "@" );
        if(o)
          value = str;
        if(value)
        {
          result->values[result->count] = value;
        } else
        {
          fprintf( stderr, "%s %s: \"%s\"\n", _("Usage Error:"), _("Unbound options are not supported"), opts->argv[i] );
          state = oyjlOPTION_NOT_SUPPORTED;
          goto clean_parse;
        }
        ++result->count;
      }
    }
    opts->private_data = result;

    i = 0;
    while(result->options[i])
    {
      oyjlOption_s * o = oyjlOptions_GetOption( opts, result->options[i] );
      if(o)
      switch(o->variable_type)
      {
        case oyjlNONE:   break;
        case oyjlSTRING: oyjlOptions_GetResult( opts, o->o, o->variable.s, 0, 0 ); break;
        case oyjlDOUBLE: oyjlOptions_GetResult( opts, o->o, 0, o->variable.d, 0 ); break;
        case oyjlINT:    oyjlOptions_GetResult( opts, o->o, 0, 0, o->variable.i ); break;
      }
      ++i;
    }

    o = oyjlOptions_GetOption( opts, "#" );
    if(opts->argc == 1 && !o)
    {
      fprintf( stderr, "%s %s\n", _("Usage Error:"), _("Optionless mode not supported. (That would need '#' option declaration.)") );
      state = oyjlOPTIONS_MISSING;
      return state;
    }

    /** Put the count of found anonymous arguments into '@' options variable.i of variable_type oyjlINT. */
    o = oyjlOptions_GetOption( opts, "@" );
    if(o && o->variable_type == oyjlINT && o->variable.i)
    {
      int count = 0;
      /* detect all '@' anonymous arguments */
      char ** results = oyjlOptions_ResultsToList( opts, "@", &count );
      *o->variable.i = count;
      oyjlStringListRelease( &results, count, free );
    }
  }

  return state;

clean_parse:
  oyjlStringListRelease( &result->options, result->count, free );
  free(result->values);
  free(result);
  return state;
}

/** @brief    Obtain the parsed result
 *  @memberof oyjlOptions_s
 *
 *  This function is only useful, if the results shall be obtained
 *  independently from oyjlOptions_Parse().
 *
 *  If the option was not specified the state oyjlOPTION_NONE will be
 *  returned and otherwise oyjlOPTION_USER_CHANGED. With result_int and
 *  a option type of oyjlOPTIONTYPE_NONE, the number of occurences is
 *  obtained, e.g. -vvv will give result_int = 3. A option type
 *  oyjlOPTIONTYPE_DOUBLE can ask for the floating point result with a
 *  result_dbl argument.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/08/04
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlOPTIONSTATE_e oyjlOptions_GetResult (
                                       oyjlOptions_s     * opts,
                                       const char        * opt,
                                       const char       ** result_string,
                                       double            * result_dbl,
                                       int               * result_int )
{
  oyjlOPTIONSTATE_e state = oyjlOPTION_NONE;
  int pos = -1, i, hits = 0;
  const char * t;
  oyjlOptsPrivate_s * results;
  oyjlOption_s * o = oyjlOptions_GetOption( opts, opt );

  /* parse the command line arguments */
  if(!opts->private_data)
    state = oyjlOptions_Parse( opts );
  if(state != oyjlOPTION_NONE)
    return state;

  results = opts->private_data;
  if(opt == NULL && results->count)
  {
    if(result_int)
      *result_int = results->count;
    return oyjlOPTION_USER_CHANGED;
  }

  /* flat search */
  for(i = 0; i < results->count; ++i)
  {
    if(strcmp(results->options[i], opt) == 0)
    {
      pos = i;
      ++hits;
      state = oyjlOPTION_USER_CHANGED;
    }
  }
  /* object search */
  if(pos == -1)
  for(i = 0; i < results->count; ++i)
  {
    if((o->o && strcmp(results->options[i], o->o) == 0) ||
       (o->option && strcmp(results->options[i], o->option) == 0))
    {
      pos = i;
      ++hits;
      state = oyjlOPTION_USER_CHANGED;
    }
  }

  if(pos == -1)
    return oyjlOPTION_NONE;

  t = results->values[pos];

  if(result_string)
    *result_string = t;

  if(result_dbl)
  {
    oyjlStringToDouble( t, result_dbl );
    if( o->value_type == oyjlOPTIONTYPE_DOUBLE &&
        ( o->values.dbl.start > *result_dbl ||
          o->values.dbl.end < *result_dbl) )
    {
      fprintf( stderr, "%s %s \'%s\' %s %g->%g !: %g\n", _("Usage Error:"), _("Option has a different value range"), opt, o->name, o->values.dbl.start, o->values.dbl.end, *result_dbl  );
    }
  }
  if(result_int)
  {
    if(o->value_type == oyjlOPTIONTYPE_NONE)
    {
      *result_int = hits;
    } else
    {
      long lo = 0;
      if(oyjlStringToLong( t, &lo ) == 0)
        *result_int = lo;
    }
  }

  return state;
}

/** @brief    Convert the parsed content to a text list
 *  @memberof oyjlOptions_s
 *
 *  @param[in]     opts                the argument object
 *  @param[in]     oc                  a filter; use NULL to get all results;
 *                                     e.g. use "@" for all anonymous results
 *  @param[out]    count               the number of matched results
 *  @return                            a possibly filterd string list of results;
 *                                     Without a filter it contains the argument
 *                                     Id followed by double point and the
 *                                     result starting at index 3.
 *                                     With filter it contains only results if
 *                                     apply and without Id.
 *                                     The memory is owned by caller.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/08/06
 *  @since   2019/03/25 (Oyjl: 1.0.0)
 */
char **  oyjlOptions_ResultsToList   ( oyjlOptions_s     * opts,
                                       const char        * oc,
                                       int               * count )
{
  char * text = NULL,
       ** list = NULL;
  oyjlOptsPrivate_s * results = NULL;
  int i,list_len = 0;
  oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );

  if(!opts)
    return NULL;
  results = opts->private_data;
  if(!results)
  {
    if(oyjlOptions_Parse( opts ))
      return NULL;

    results = opts->private_data;
    if(!results)
      return NULL;
  }

  for(i = 0; i < results->count; ++i)
  {
    const char * option = results->options[i];
    const char * value = results->values[i];
    if(oc == NULL)
      oyjlStringAdd( &text, malloc, free, "%s:%s", option, value );
    else if(option && option[0] &&
            ((o->o && o->o[0] && strcmp(option,o->o) == 0) ||
             (o->option && o->option[0] && strcmp(option,o->option) == 0)))
      oyjlStringAdd( &text, malloc, free, "%s", value );
    if(text)
    {
      oyjlStringListAddStaticString( &list, &list_len, text, malloc, free );
      free(text); text = NULL;
    }
  }
  if(count)
    *count = list_len;

  return list;
}

/** @brief    Convert the parsed content to simple text
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
char * oyjlOptions_ResultsToText  ( oyjlOptions_s  * opts )
{
  char * text = NULL;
  oyjlOptsPrivate_s * results = opts->private_data;
  int i;

  if(!results)
  {
    if(oyjlOptions_Parse( opts ))
      return NULL;

    results = opts->private_data;
    if(!results)
      return NULL;
  }

  for(i = 0; i < results->count; ++i)
  {
    const char * option = results->options[i];
    const char * value = results->values[i];
    oyjlStringAdd( &text, malloc, free, "%s:%s\n", option, value );
  }

  return text;
}

/** @internal
 *  @brief    Print synopsis of a option group to stderr
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/08/19
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
char *       oyjlOptions_PrintHelpSynopsis (
                                       oyjlOptions_s  *    opts,
                                       oyjlOptionGroup_s * g,
                                       int                 style )
{
  int i;
  char ** m_list = NULL, ** on_list = NULL;
  int * m_index = NULL, * on_index = NULL;
  int m = oyjlStringDelimiterCount(g->mandatory, ",|");
  int on = oyjlStringDelimiterCount(g->optional, ",|");
  int opt_group = 0;
  int gstyle = style;
  const char * prog = opts->argv[0];
  char * text = oyjlStringCopy( "", malloc );
  if(prog && strchr(prog,'/'))
    prog = strrchr(prog,'/') + 1;

  if( m || on )
  {
    if(style & oyjlOPTIONSTYLE_MAN)
      oyjlStringAdd( &text, malloc, free, "\\fB%s\\fR", prog );
    else if(style & oyjlOPTIONSTYLE_MARKDOWN)
      oyjlStringAdd( &text, malloc, free, "**%s**", prog );
    else
      oyjlStringAdd( &text, malloc, free, "%s", oyjlTermColor(oyjlBOLD,prog) );
  }
  else
    return text;

  m_list = oyjlStringSplit2( g->mandatory, "|,", &m, &m_index, malloc );
  for(i = 0; i < m; ++i)
  {
    char * option = m_list[i];
    oyjlOption_s * o = oyjlOptions_GetOptionL( opts, option );
    char next_delimiter = g->mandatory[m_index[i]];
    if(!o)
    {
      fprintf(stderr, "\n%s: option not declared: %s\n", g->name, option);
      exit(1);
    }
    if(option[0] != '@' && !(option[0] == '#' && m+on == 1))
    {
      char * t = oyjlOption_PrintArg(o, style);
      oyjlStringAdd( &text, malloc, free, " %s", t );
      free(t);
    }
    if(next_delimiter == '|')
      oyjlStringAdd( &text, malloc, free, " |" );
  }

  on_list = oyjlStringSplit2( g->optional, "|,", &on, &on_index, malloc );
  for(i = 0; i < on; ++i)
  {
    char * option = on_list[i];
    oyjlOption_s * o = oyjlOptions_GetOptionL( opts, option );
    char next_delimiter = g->optional[on_index[i]];
    gstyle = style | oyjlOPTIONSTYLE_OPTIONAL;
    if(i < on - 1 && next_delimiter == '|')
    {
      if(opt_group == 0)
        gstyle = style | oyjlOPTIONSTYLE_OPTIONAL_START | oyjlOPTIONSTYLE_OPTIONAL_INSIDE_GROUP;
      else
        gstyle = style | oyjlOPTIONSTYLE_OPTIONAL_INSIDE_GROUP;
      opt_group = 1;
    }
    else if(opt_group)
    {
      gstyle = style | oyjlOPTIONSTYLE_OPTIONAL_END;
      opt_group = 0;
    }
    else if(!o)
    {
      fprintf(stderr, "\n%s: option not declared: %s\n", g->name, &g->optional[i]);
      exit(1);
    }
    {
      char * t = oyjlOption_PrintArg(o, gstyle);
      oyjlStringAdd( &text, malloc, free, "%s%s", gstyle & oyjlOPTIONSTYLE_OPTIONAL_START ? " ":"", t );
      free(t);
    }
    if(next_delimiter == '|')
    {
      oyjlStringAdd( &text, malloc, free, "|" );
    }
  }
  for(i = 0; i < m; ++i)
  {
    char * option = m_list[i];
    oyjlOption_s * o = oyjlOptions_GetOptionL( opts, option );
    char next_delimiter = g->mandatory[m_index[i]];
    if(next_delimiter != '|' && !o)
    {
      fprintf(stderr, "\n%s: option not declared: %s\n", g->name, option);
      exit(1);
    }
    if(strcmp(option, "@") == 0)
      oyjlStringAdd( &text, malloc, free, " %s", o->value_name?o->value_name:"..." );
  }
  oyjlStringListRelease( &m_list, m, free );
  oyjlStringListRelease( &on_list, on, free );
  free( m_index ); m_index = NULL;
  free( on_index ); on_index = NULL;
  return text;
}

static oyjlOptionChoice_s ** oyjl_get_choices_list_ = NULL;
static int * oyjl_get_choices_list_selected_;
oyjlOptionChoice_s * oyjlOption_GetChoices_ (
                                       oyjlOption_s      * o,
                                       int               * selected,
                                       oyjlOptions_s     * opts )
{
  int nopts = oyjlOptions_Count( opts ), pos = -1, i;
  if(!o) return NULL;

  for(i = 0; i < nopts; ++i)
    if(o == &opts->array[i])
      pos = i;

  if(!oyjl_get_choices_list_)
  {
    int i;
    oyjl_get_choices_list_selected_ = calloc( sizeof(int), nopts + 1 );
    for(i = 0; i < nopts; ++i) oyjl_get_choices_list_selected_[i] = -1;
    oyjl_get_choices_list_ = calloc( sizeof(oyjlOptionChoice_s*), nopts + 1 );
  }

  if( !oyjl_get_choices_list_[pos] ||
      (selected && oyjl_get_choices_list_selected_[pos] == -1) )
    oyjl_get_choices_list_[pos] = o->values.getChoices(o, selected ? &oyjl_get_choices_list_selected_[pos] : selected, opts );

  if(selected)
    *selected = oyjl_get_choices_list_selected_[pos];
  return oyjl_get_choices_list_[pos];
}


#include <stdarg.h> /* va_list */
/** @brief    Print help text to stderr
 *  @memberof oyjlOptions_s
 *
 *  @param   opts                      options to print
 *  @param   ui                        more info for e.g. from the documentation section for the description block; optional
 *  @param   verbose                   gives debug output
 *  @param   motto_format              prints a customised intoduction line
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/05/06
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
void  oyjlOptions_PrintHelp          ( oyjlOptions_s     * opts,
                                       oyjlUi_s          * ui,
                                       int                 verbose,
                                       const char        * motto_format,
                                                           ... )
{
  int i,ng;
  va_list list;
  int indent = 2;
  oyjlUiHeaderSection_s * section = NULL;
  fprintf( stdout, "\n");
  if(verbose)
  {
    for(i = 0; i < opts->argc; ++i)
      fprintf( stdout, "\'%s\' ", oyjlTermColor( oyjlITALIC, opts->argv[i] ));
    fprintf( stdout, "\n");
  }

  if(!motto_format)
  {
    oyjlUiHeaderSection_s * version = oyjlUi_GetHeaderSection( ui,
                                                               "version" );
    fprintf( stdout, "%s v%s - %s", oyjlTermColor( oyjlBOLD, opts->argv[0] ),
                              version && version->name ? version->name : "",
                              ui->description ? ui->description : ui->name ? ui->name : "" );
  }
  else
  {
    va_start( list, motto_format );
    vfprintf( stdout, motto_format, list );
    va_end  ( list );
  }
  fprintf( stdout, "\n");

  ng = oyjlOptions_CountGroups(opts);
  if(!ng) return;

  if( ui && (section = oyjlUi_GetHeaderSection(ui, "documentation")) != NULL &&
      section->description )
    fprintf( stdout, "\n%s:\n  %s\n", oyjlTermColor(oyjlBOLD,_("Description")), section->description );

  fprintf( stdout, "\n%s:\n", oyjlTermColor(oyjlBOLD,_("Synopsis")) );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    char * t = oyjlOptions_PrintHelpSynopsis( opts, g, oyjlOPTIONSTYLE_ONELETTER );
    fprintf( stdout, "  %s\n", t );
    free(t);
  }

  fprintf( stdout, "\n%s:\n", oyjlTermColor(oyjlBOLD,_("Usage"))  );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    int d = 0,
        j,k;
    char ** d_list = oyjlStringSplit2( g->detail, "|,", &d, NULL, malloc );
    fprintf( stdout, "  %s\n", g->description?oyjlTermColor(oyjlUNDERLINE,g->description):"" );
    if(g->mandatory && g->mandatory[0])
    {
      char * t = oyjlOptions_PrintHelpSynopsis( opts, g, oyjlOPTIONSTYLE_ONELETTER );
      fprintf( stdout, "\t%s\n", t );
      free(t);
    }
    for(j = 0; j < d; ++j)
    {
      const char * option = d_list[j];
      oyjlOption_s * o = oyjlOptions_GetOptionL( opts, option );
      if(!o)
      {
        fprintf(stdout, "\n%s: option not declared: %s\n", g->name, &g->detail[j]);
        exit(1);
      }
      for(k = 0; k < indent; ++k) fprintf( stdout, " " );
      switch(o->value_type)
      {
        case oyjlOPTIONTYPE_CHOICE:
          {
            int n = 0,l;
            if(o->value_name)
            {
              char * t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING);
              fprintf( stdout, "\t" );
              fprintf( stdout, "%s", t );
              fprintf( stdout, "\t%s%s%s\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"" );
              free(t);
            }
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            while(o->values.choices.list && o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000')
              ++n;
            for(l = 0; l < n; ++l)
              fprintf( stdout, "\t\t-%s %s\t\t# %s%s%s\n",
                  o->o,
                  o->values.choices.list[l].nick,
                  o->values.choices.list[l].name && o->values.choices.list[l].nick[0] ? o->values.choices.list[l].name : o->values.choices.list[l].description,
                  o->values.choices.list[l].help&&o->values.choices.list[l].help[0]?" - ":"",
                  o->values.choices.list[l].help?o->values.choices.list[l].help:"" );
          }
          break;
        case oyjlOPTIONTYPE_FUNCTION:
          {
            int n = 0,l;
            oyjlOptionChoice_s * list;
            if(o->value_name)
            {
              char * t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING);
              fprintf( stdout, "\t" );
              fprintf( stdout, "%s", t );
              fprintf( stdout, "\t%s%s%s\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"" );
              free(t);
            }
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            list = oyjlOption_GetChoices_(o, NULL, opts );
            if(list)
              while(list[n].nick && list[n].nick[0] != '\000')
                ++n;
            for(l = 0; l < n; ++l)
              fprintf( stdout, "\t  -%s %s\t\t# %s\n", o->o, list[l].nick, list[l].name && list[l].nick[0] ? list[l].name : list[l].description );
            /* not possible, as the result of oyjlOption_GetChoices_() is cached - oyjlOptionChoice_Release( &list ); */
          }
          break;
        case oyjlOPTIONTYPE_DOUBLE:
          {
            char * t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING);
            fprintf( stdout, "\t" );
            fprintf( stdout, "%s", t );
            fprintf( stdout, "\t%s%s%s (%s%s%g [≥%g ≤%g])\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"", o->value_name?o->value_name:"", o->value_name?":":"", o->values.dbl.d, o->values.dbl.start, o->values.dbl.end );
            free(t);
          }
          break;
        case oyjlOPTIONTYPE_NONE:
          {
            char * t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING);
            fprintf( stdout, "\t" );
            fprintf( stdout, "%s", t );
            fprintf( stdout, "\t%s%s%s\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"" );
            free(t);
          }
        break;
        case oyjlOPTIONTYPE_START: break;
        case oyjlOPTIONTYPE_END: break;
      }
    }
    oyjlStringListRelease( &d_list, d, free );
    if(d) fprintf( stdout, "\n" );
  }
  fprintf( stdout, "\n" );
}

/** @brief    Allocate a new options structure
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/03/24
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlOptions_s * oyjlOptions_New      ( int                 argc,
                                       const char       ** argv )
{
  oyjlOptions_s * opts = calloc( sizeof(oyjlOptions_s), 1 );
  memcpy( opts->type, "oiws", 4 );

  opts->argc = argc;
  opts->argv = argv;
  return opts;
}

/** @brief    Allocate a new ui structure
 *  @memberof oyjlUi_s
 *
 *  The oyjlUi_s contains already options in the opts member.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/03/24
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlUi_s* oyjlUi_New                 ( int                 argc,
                                       const char       ** argv )
{
  oyjlUi_s * ui = calloc( sizeof(oyjlUi_s), 1 );
  memcpy( ui->type, "oiui", 4 );
  ui->opts = oyjlOptions_New( argc, argv );
  return ui;
}

oyjlOPTIONSTATE_e  oyjlUi_Check      ( oyjlUi_s          * ui,
                                       int                 flags OYJL_UNUSED )
{
  oyjlOPTIONSTATE_e status = oyjlOPTION_NONE;
  int i,ng;
  oyjlOptions_s * opts;
 
  if(!ui) return status;
  opts = ui->opts;

  ng = oyjlOptions_CountGroups(opts);
  if(!ng)
  {
    fprintf(stderr, "no ui::opts::groups\n");
    status = oyjlOPTION_MISSING_VALUE;
  }

  if(!ui->nick || !ui->nick[0])
  {
    fprintf(stderr, "no ui::nick\n");
    status = oyjlOPTION_MISSING_VALUE;
  }

  if(!ui->name || !ui->name[0])
  {
    fprintf(stderr, "no ui::name\n");
    status = oyjlOPTION_MISSING_VALUE;
  }

  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    int d = oyjlStringDelimiterCount(g->detail, ",|"),
        j;
    if(g->mandatory && g->mandatory[0])
    {
      int n = 0;
      char ** list = oyjlStringSplit2( g->mandatory, "|,", &n, NULL, malloc );
      for( j = 0; j  < n; ++j )
      {
        const char * option = list[j];
        if( !g->detail ||
            (!strstr(g->detail, option) &&
             strcmp(option, "|") &&
             strcmp(option, "#")) )
        {
          fprintf(stderr, "\"%s\" not found in group->details\n", option );
          status = oyjlOPTION_MISSING_VALUE;
        }
      }
      oyjlStringListRelease( &list, n, free );
    }
    for(j = 0; j < d; ++j)
    {
      oyjlOption_s * o = oyjlOptions_GetOptionL( opts, &g->detail[j] );
      if(!o)
      {
        fprintf(stderr, "\n%s: option not declared: %s\n", g->name, &g->detail[j]);
        exit(1);
      }
      switch(o->value_type)
      {
        case oyjlOPTIONTYPE_CHOICE:
          {
            int n = 0;
            while(o->values.choices.list && o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000')
              ++n;
            if( !n && !(o->flags & OYJL_OPTION_FLAG_EDITABLE) &&
                strcmp(o->o, "X") != 0)
            {
              fprintf( stderr, "%s %s \'%s\' %s\n", _("Usage Error:"), _("Option not supported"), o->o, _("needs OYJL_OPTION_FLAG_EDITABLE or choices") );
              status = oyjlOPTION_NOT_SUPPORTED;
            }
          }
          break;
        case oyjlOPTIONTYPE_FUNCTION:
          break;
        case oyjlOPTIONTYPE_DOUBLE:
          break;
        case oyjlOPTIONTYPE_NONE:
        break;
        case oyjlOPTIONTYPE_START: break;
        case oyjlOPTIONTYPE_END: break;
      }
      while(g->detail[j] && g->detail[j] != ',') ++j;
    }
  }

  return status;
}

/** @brief    Create a new UI structure
 *  @memberof oyjlUi_s
 *
 *  This is a high level convinience function.
 *  The returned oyjlUi_s is a comlete description of the UI and can be
 *  used instantly. The options are parsed, errors are printed, help text
 *  is printed for the boolean -h/--help option. Boolean -v/--verbose
 *  is handled too. The results are set to the declared variables. 
 *  The app_type defaults to "tool", but it can be replaced if needed.
 *
 *  @code
  oyjlUi_s * ui = oyjlUi_Create( argc, argv,
                                       "myCl",
                                       _("My Command"),
                                       _("My Command line tool from Me"),
                                       "my_logo",
                                       info, options, groups, NULL )
    @endcode
 *
 *  @param[in]     argc                number of command line arguments
 *  @param[in]     argv                command line args from C/C++ main()
 *  @param[in]     nick                four byte string; e.g. "myCl"
 *  @param[in]     name                short name of the tool; i18n;
 *                 e.g. _("My Command")
 *  @param[in]     description         compact sentence starting with full name; i18n;
 *                 e.g. _("My Command line tool from Me")
 *  @param[in]     logo                icon name; This variable must contain
 *                 the file name only, without ending. The icon needs
 *                 to be installed in typical icon search path and will be
 *                 detected there. e.g. "my_logo" points to "my_logo.{png|svg}"
 *  @param[in]     info                general information for rich UI's and
 *                                     for help text
 *  @param[in,out] options             the main option declaration, with
 *                 syntax declaration and variable passing for setting results
 *  @param[in]     groups              the option grouping declares
 *                 dependencies of options and provides a UI layout
 *  @param[in,out] state               inform about processing
 *                                     - ::oyjlUI_STATE_HELP : help was detected, printed and oyjlUi_s was released
 *                                     - ::oyjlUI_STATE_EXPORT : export of json, man or markdown was detected, printed and oyjlUi_s was released
 *                                     - ::oyjlUI_STATE_VERBOSE : verbose was detected
 *                                     - ::oyjlUI_STATE_OPTION+ : error occured in option parser, message printed, ::oyjlOPTIONSTATE_e is placed in >> ::oyjlUI_STATE_OPTION and oyjlUi_s was released
 *                                     -- ::oyjlUI_STATE_NO_CHECKS : skip any checks during creation; Only useful if part of the passed in data is omitted or needs to be passed through.
 *  @return                            UI object for later use
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/08/09
 *  @since   2018/08/20 (OpenICC: 0.1.1)
 */
oyjlUi_s *  oyjlUi_Create            ( int                 argc,
                                       const char       ** argv,
                                       const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const char        * logo,
                                       oyjlUiHeaderSection_s * info,
                                       oyjlOption_s      * options,
                                       oyjlOptionGroup_s * groups,
                                       int               * status )
{
  int help = 0, verbose = 0, version = 0;
  const char * export = NULL;
  oyjlOption_s * h, * v, * X, * V;
  oyjlOPTIONSTATE_e opt_state = oyjlOPTION_NONE;
  int flags = 0;
  char * t;

  int use_gettext = 0;
#ifdef OYJL_USE_GETTEXT
  use_gettext = 1;
#endif
  oyjlInitLanguageDebug( "Oyjl", "OYJL_DEBUG", oyjl_debug, use_gettext, "OYJL_LOCALEDIR", OYJL_LOCALEDIR, OYJL_DOMAIN, NULL );

  if(status)
    flags = *status;

  /* allocate options structure */
  oyjlUi_s * ui = oyjlUi_New( argc, argv ); /* argc+argv are required for parsing the command line options */
  /* tell about the tool */
  if(!(flags & oyjlUI_STATE_NO_CHECKS))
    ui->app_type = "tool";
  ui->nick = nick;
  ui->name = name;
  ui->description = description;
  ui->logo = logo;

  /* Select from *version*, *manufacturer*, *copyright*, *license*, *url*,
   * *support*, *download*, *sources*, *oyjl_module_author* and
   * *documentation* what you see fit. Add new ones as needed. */
  ui->sections = info;
  ui->opts->array = options;
  ui->opts->groups = groups;

  /* get results and check syntax ... */
  opt_state = oyjlOptions_Parse( ui->opts );
  if(opt_state == oyjlOPTIONS_MISSING)
  {
    oyjlUiHeaderSection_s * version = oyjlUi_GetHeaderSection( ui,
                                                               "version" );
    const char * prog = argv[0];
    if(!verbose && prog && strchr(prog,'/'))
      prog = strrchr(prog,'/') + 1;
    oyjlOptions_PrintHelp( ui->opts, ui, verbose, "%s v%s - %s", prog,
                              version && version->name ? version->name : "",
                              ui->description ? ui->description : "" );
    oyjlUi_Release( &ui);
    if(status)
      *status |= oyjlUI_STATE_HELP;
    return NULL;
  }
  if(opt_state == oyjlOPTION_NONE && !(flags & oyjlUI_STATE_NO_CHECKS))
    opt_state = oyjlUi_Check(ui, 0);
  /* ... and report detected errors */
  if(opt_state != oyjlOPTION_NONE)
  {
    fputs( _("... try with --help|-h option for usage text. give up"), stderr );
    fputs( "\n", stderr );
    oyjlUi_Release( &ui);
    if(status)
      *status = opt_state << oyjlUI_STATE_OPTION;
    return NULL;
  }

  X = oyjlOptions_GetOption( ui->opts, "X" );
  if(X && X->variable_type == oyjlSTRING && X->variable.s)
    export = *X->variable.s;
  h = oyjlOptions_GetOption( ui->opts, "h" );
  if(h && h->variable_type == oyjlINT && h->variable.i)
    help = *h->variable.i;
  v = oyjlOptions_GetOption( ui->opts, "v" );
  if(v && v->variable_type == oyjlINT && v->variable.i)
  {
    verbose = *v->variable.i;
    if(status && verbose)
      *status |= oyjlUI_STATE_VERBOSE;
  }
  if(help)
  {
    oyjlUiHeaderSection_s * version = oyjlUi_GetHeaderSection( ui,
                                                               "version" );
    const char * prog = argv[0];
    if(!verbose && prog && strchr(prog,'/'))
      prog = strrchr(prog,'/') + 1;
    oyjlOptions_PrintHelp( ui->opts, ui, verbose, "%s v%s - %s", prog,
                              version && version->name ? version->name : "",
                              ui->description ? ui->description : "" );
    oyjlUi_Release( &ui);
    if(status)
      *status |= oyjlUI_STATE_HELP;
    return NULL;
  }
  V = oyjlOptions_GetOption( ui->opts, "V" );
  if(V && V->variable_type == oyjlINT && V->variable.i)
    version = *V->variable.i;
  if(version)
  {
    oyjlUiHeaderSection_s * version = oyjlUi_GetHeaderSection( ui, "version" );
    oyjlUiHeaderSection_s * author = oyjlUi_GetHeaderSection( ui, "manufacturer" );
    oyjlUiHeaderSection_s * copyright = oyjlUi_GetHeaderSection( ui, "copyright" );
    oyjlUiHeaderSection_s * license = oyjlUi_GetHeaderSection( ui, "license" );
    const char * prog = argv[0];
    if(!verbose && prog && strchr(prog,'/'))
      prog = strrchr(prog,'/') + 1;
    fprintf( stdout, "%s v%s - %s\n%s\n%s%s%s\n%s%s%s\n\n", prog,
                                      version && version->name ? version->name : "",
                                      ui->description ? ui->description : ui->name ? ui->name : "",
                                      copyright && copyright->name ? copyright->name : "",
                                      license ? _("License"):"", license?":\t":"", license && license->name ? license->name : "",
                                      author ? _("Author"):"", author?": \t":"", author && author->name ? author->name : "" );
    oyjlUi_Release( &ui);
    if(status)
      *status |= oyjlUI_STATE_HELP;
    return NULL;
  }
  if(export)
  {
    if(status)
      *status |= oyjlUI_STATE_EXPORT;
#if defined(OYJL_INTERNAL)
    if(strcmp(export, "json") == 0)
    {
      t = oyjlUi_ToJson( ui, flags );
      if(t) puts( t );
      oyjlUi_Release( &ui);
      return NULL;
    }
#endif
    if(strcmp(export, "man") == 0)
    {
      t = oyjlUi_ToMan( ui, flags );
      if(t) puts( t );
      oyjlUi_Release( &ui);
      return NULL;
    }
    if(strcmp(export, "markdown") == 0)
    {
      t = oyjlUi_ToMarkdown( ui, flags );
      if(t) puts( t );
      oyjlUi_Release( &ui);
      return NULL;
    }
#if defined(OYJL_INTERNAL)
    if(strcmp(export, "export") == 0)
    {
      puts( oyjlUi_ExportToJson( ui, flags ) );
      oyjlUi_Release( &ui);
      return NULL;
    }
#endif
  }
  /* done with options handling */

  return ui;
}

/** @brief    Release "oiui"
 *  @memberof oyjlUi_s
 *
 *  Release oyjlUi_s::opts, oyjlUi_s::private_data and oyjlUi_s.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
void           oyjlUi_Release     ( oyjlUi_s      ** ui )
{
  oyjlOptsPrivate_s * results;
  if(!ui || !*ui) return;
  if( *(oyjlOBJECT_e*)*ui != oyjlOBJECT_UI)
  {
    char * a = (char*)*ui;
    char type[5] = {a[0],a[1],a[2],a[3],0};
    fprintf(stderr, "Unexpected object: \"%s\"(expected: \"oyjlUi_s\")\n", type );
    return;
  }
  if((*ui)->opts->private_data)
  {
    results = (*ui)->opts->private_data;
    oyjlStringListRelease( &results->options, results->count, free );
    results->count = 0;
    free(results->options);
    free(results->values);
    free((*ui)->opts->private_data);
  }
  if((*ui)->opts) free((*ui)->opts);
  free((*ui));
  *ui = NULL;
}

/** @brief    Return the number of sections of type "oihs"
 *  @memberof oyjlUi_s
 *
 *  The oyjlUi_s contains already options in the opts member.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
int     oyjlUi_CountHeaderSections( oyjlUi_s       * ui )
{
  int n = 0;
  while( *(oyjlOBJECT_e*)&ui->sections[n] /*"oihs"*/ == oyjlOBJECT_UI_HEADER_SECTION) ++n;
  return n;
}

/** @brief    Return the section which was specified by its nick name
 *  @memberof oyjlUi_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlUiHeaderSection_s * oyjlUi_GetHeaderSection (
                                       oyjlUi_s          * ui,
                                       const char        * nick )
{
  oyjlUiHeaderSection_s * section = NULL;
  int i, count = oyjlUi_CountHeaderSections(ui);
  for(i = 0; i < count; ++i)
    if( strcmp(ui->sections[i].nick, nick) == 0 )
      section = &ui->sections[i];
  return section;
}

char *       oyjlStringToUpper       ( const char        * t )
{
  char * text = oyjlStringCopy(t, malloc);
  int slen = strlen(t), i;
  for(i = 0; i < slen; ++i)
    text[i] = toupper(t[i]);
  return text;
}
char *       oyjlStringToLower       ( const char        * t )
{
  char * text = oyjlStringCopy(t, malloc);
  int slen = strlen(t), i;
  for(i = 0; i < slen; ++i)
    text[i] = tolower(t[i]);
  return text;
}


#define ADD_SECTION( sec, link, format, ... ) { \
  oyjlStringAdd( &text, malloc, free, "## %s <a name=\"%s\"></a>\n" format, sec, link, __VA_ARGS__ ); \
  oyjlStringListAddStaticString( sections, sn, sec, 0,0 ); \
  oyjlStringListAddStaticString( sections, sn, link, 0,0 ); }

static
char *       oyjlExtraManSection     ( oyjlOptions_s     * opts,
                                       const char        * opt_name,
                                       int                 flags,
                                       char            *** sections,
                                       int               * sn )
{
  oyjlOption_s * o = oyjlOptions_GetOptionL( opts, opt_name );
  char * text = NULL;
  if(o)
  {
    int n = 0,l;
    if(o->value_type == oyjlOPTIONTYPE_CHOICE)
    {
      oyjlOptionChoice_s * list = o->values.choices.list;
      while(o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000') ++n;
      if(n)
      {
        char * up = oyjlStringToUpper( &opt_name[4] );
        oyjlStringReplace( &up, "_", " ", malloc, free );
        const char * section = up;
        if(strcmp(section,"EXAMPLES") == 0)
          section = _("EXAMPLES");
        else if(strcmp(section,"EXIT-STATE") == 0)
          section = _("EXIT-STATE");
        else if(strcmp(section,"ENVIRONMENT VARIABLES") == 0)
          section = _("ENVIRONMENT VARIABLES");
        else if(strcmp(section,"HISTORY") == 0)
          section = _("HISTORY");
        else if(strcmp(section,"FILES") == 0)
          section = _("FILES");
        else if(strcmp(section,"SEE AS WELL") == 0)
          section = _("SEE AS WELL");
        else if(strcmp(section,"SEE ALSO") == 0)
          section = _("SEE ALSO");
        if(flags & oyjlOPTIONSTYLE_MARKDOWN)
        {
          char * low = oyjlStringToLower( &opt_name[4] );
          oyjlStringReplace( &low, "_", "", malloc, free );
          ADD_SECTION( _(section), low, "", "" )
          free(low);
        }
        else
          oyjlStringAdd( &text, malloc, free, ".SH %s\n", _(section) );
        for(l = 0; l < n; ++l)
        {
          if(flags & oyjlOPTIONSTYLE_MARKDOWN)
          {
            if( strcmp(up,"SEE AS WELL") == 0 ||
                strcmp(up,"SEE ALSO") == 0 )
            {
              int li_n = 0, i;
              char ** li = oyjlStringSplit2( list[l].nick, 0, &li_n, NULL, malloc );
              oyjlStringAdd( &text, malloc, free, "###" );
              for(i = 0; i < li_n; ++i)
              {
                char * md = oyjlStringCopy( li[i], 0 );
                int len = strlen(md), is_man_page = 0;
                if(len > 3 && md[len-3] == '(' && md[len-1] == ')')
                  ++is_man_page;
                if(is_man_page)
                {
                  char * end = oyjlStringCopy( &md[len-3], malloc );
                  char * t;
                  md[len-3] = '\000';
                  t = oyjlStringCopy( md, 0 );
                  oyjlStringReplace( &md, "-", "", malloc, free );

                  oyjlStringAdd( &text, malloc, free, "  [%s](%s.html)<a href=\"%s.md\">%s</a>", t, md, md, end );
                  free( t );
                  free( end );
                }
                else
                  oyjlStringAdd( &text, malloc, free, " %s", md );

                free( md );
              }
              oyjlStringAdd( &text, malloc, free, "\n" );
              oyjlStringListRelease( &li, li_n, free );
            }
            else
            oyjlStringAdd( &text, malloc, free, "### %s\n", list[l].nick );
            if(list[l].name && list[l].name[0])
            oyjlStringAdd( &text, malloc, free, "&nbsp;&nbsp;%s\n", list[l].name );
            if(list[l].description && list[l].description[0])
            oyjlStringAdd( &text, malloc, free, "  <br />\n&nbsp;&nbsp;%s\n", list[l].description );
            if(list[l].help && list[l].help[0])
            oyjlStringAdd( &text, malloc, free, "  <br />\n&nbsp;&nbsp;%s\n", list[l].help );
          }
          else
          {
            oyjlStringAdd( &text, malloc, free, ".TP\n%s\n.br\n", list[l].nick );
            if(list[l].name && list[l].name[0])
            oyjlStringAdd( &text, malloc, free, "%s\n", list[l].name );
            if(list[l].description && list[l].description[0])
            oyjlStringAdd( &text, malloc, free, ".br\n%s\n", list[l].description );
            if(list[l].help && list[l].help[0])
            oyjlStringAdd( &text, malloc, free, ".br\n%s\n", list[l].help );
          }
        }
        free(up);
      }
    }
  }
  return text;
}

static
char *       oyjlExtraManSections ( oyjlOptions_s  * opts, int flags, char *** sections, int * sn )
{
  char * text = NULL;
  int nopts = oyjlOptions_Count( opts );
  int l;
  for(l = 0; l < nopts; ++l)
  {
    oyjlOption_s * o = &opts->array[l];
    const char * option = o->option;
    int olen = option ? strlen(option) : 0;
    if(olen > 7 && option[0] == 'm' && option[1] == 'a' && option[2] == 'n' && option[3] == '-')
    {
      char * tmp = oyjlExtraManSection(opts, option, flags, sections, sn);
      if(tmp)
      {
        oyjlStringAdd( &text, malloc, free, tmp );
        free(tmp);
      }
    }
  }
  return text;
}

/** @brief    Return a man page from options
 *  @memberof oyjlUi_s
 *
 *  Some man pages might contain some additional sections.
 *  They are supported as options. To generate a custom man page section,
 *  add a blind option to your options list and set the oyjlOption_s::o
 *  char to something non interupting like, dot '.' or similar.
 *  The oyjlOption_s::option string
 *  contains "man-section_head", with "section-head" being adapted to your
 *  needs. The "man-" part will be cut off and 
 *  "section_head" will become uppercase and underline '_' become empty
 *  space: "SECTION HEAD".
 *  Use oyjlOption_s::value_type=oyjlOPTIONTYPE_CHOICE
 *  and place your string list into oyjlOptionChoice_s::nick by filling all
 *  members. Translated section heads are "EXAMPLES, "SEE AS WELL", "HISTORY",
 *  "ENVIRONMENT VARIABLES", "EXIT-STATE" and "FILES".
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/10/15
 *  @since   2018/10/10 (OpenICC: 0.1.1)
 */
char *       oyjlUi_ToMan            ( oyjlUi_s          * ui,
                                       int                 flags OYJL_UNUSED )
{
  char * text = NULL, * tmp;
  const char * date = NULL,
             * desc = NULL,
             * mnft = NULL, * mnft_url = NULL,
             * copy = NULL, * lice = NULL,
             * bugs = NULL, * bugs_url = NULL,
             * vers = NULL;
  int i,n,ng;
  oyjlOptions_s * opts;
 
  if(!ui) return text;
  opts = ui->opts;

  n = oyjlUi_CountHeaderSections( ui );
  for(i = 0; i < n; ++i)
  {
    oyjlUiHeaderSection_s * s = &ui->sections[i];
    if(strcmp(s->nick, "manufacturer") == 0) { mnft = s->name; mnft_url = s->description; }
    else if(strcmp(s->nick, "copyright") == 0) copy = s->name;
    else if(strcmp(s->nick, "license") == 0) lice = s->name;
    else if(strcmp(s->nick, "url") == 0) continue;
    else if(strcmp(s->nick, "support") == 0) { bugs = s->name; bugs_url = s->description; }
    else if(strcmp(s->nick, "download") == 0) continue;
    else if(strcmp(s->nick, "sources") == 0) continue;
    else if(strcmp(s->nick, "development") == 0) continue;
    else if(strcmp(s->nick, "oyjl_module_author") == 0) continue;
    else if(strcmp(s->nick, "documentation") == 0) desc = s->description ? s->description : s->name;
    else if(strcmp(s->nick, "version") == 0) vers = s->name;
    else if(strcmp(s->nick, "date") == 0) date = s->description ? s->description : s->name;
  }

  ng = oyjlOptions_CountGroups(opts);
  if(!ng) return NULL;

  if(ui->app_type && ui->app_type[0])
  {
    int tool = strcmp( ui->app_type, "tool" ) == 0;
    oyjlStringAdd( &text, malloc, free, ".TH \"%s\" %d \"%s\" \"%s\"\n", ui->nick,
                   tool?1:7, date?date:"", tool?"User Commands":"Misc" );
  }

  oyjlStringAdd( &text, malloc, free, ".SH NAME\n%s %s%s \\- %s\n", ui->nick, vers?"v":"", vers?vers:"", ui->name );

  oyjlStringAdd( &text, malloc, free, ".SH %s\n", _("SYNOPSIS") );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    char * syn = oyjlOptions_PrintHelpSynopsis( opts, g,
                         oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MAN );
    if(syn && syn[0])
      oyjlStringAdd( &text, malloc, free, "%s\n%s", syn, (i < (ng-1)) ? ".br\n" : "" );
    free(syn);
  }

  if(desc)
    oyjlStringAdd( &text, malloc, free, ".SH %s\n%s\n", _("DESCRIPTION"), desc );

  oyjlStringAdd( &text, malloc, free, ".SH %s\n", _("OPTIONS") );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    int dn = 0,
        j;
    char ** d_list = oyjlStringSplit2( g->detail, "|,", &dn, NULL, malloc );
    if(g->description)
      oyjlStringAdd( &text, malloc, free, ".SS\n%s\n", g->description  );
    else
    if(g->name)
      oyjlStringAdd( &text, malloc, free, ".SS\n%s\n", g->name );
    else
      oyjlStringAdd( &text, malloc, free, "\n"  );
    if(g->mandatory && g->mandatory[0])
    {
      char * t = oyjlOptions_PrintHelpSynopsis( opts, g, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MAN );
      oyjlStringAdd( &text, malloc, free, "%s\n", t );
      free(t);
    }
    oyjlStringAdd( &text, malloc, free, ".br\n"  );
    if(g->help)
    {
      oyjlStringAdd( &text, malloc, free, "%s\n.br\n.sp\n.br\n", g->help );
    }
    for(j = 0; j < dn; ++j)
    {
      const char * option = d_list[j];
      char * t;
      oyjlOption_s * o = oyjlOptions_GetOptionL( opts, option );
      if(!o)
      {
        fprintf(stderr, "\n%s: option not declared: %s\n", g->name, option);
        exit(1);
      }
      switch(o->value_type)
      {
        case oyjlOPTIONTYPE_CHOICE:
          {
            int n = 0,l;
            t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MAN);
            oyjlStringAdd( &text, malloc, free, "%s", t );
            free(t);
            if(o->name && !o->description)
              oyjlStringAdd( &text, malloc, free, "\t%s", o->name );
            oyjlStringAdd( &text, malloc, free, "\t%s%s%s%s", o->description ? o->description:"", o->help?"\n.RS\n":"", o->help?o->help:"", o->help?"\n.RE\n":"\n.br\n" );
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            while(o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000')
              ++n;
            for(l = 0; l < n; ++l)
              oyjlStringAdd( &text, malloc, free, "\t\\-%s %s\t\t# %s %s %s\n.br\n",
                  o->o,
                  o->values.choices.list[l].nick,
                  o->values.choices.list[l].name && o->values.choices.list[l].nick[0] ? o->values.choices.list[l].name : o->values.choices.list[l].description,
                  o->values.choices.list[l].help&&o->values.choices.list[l].help[0]?"\n.br\n\t":"",
                  o->values.choices.list[l].help?o->values.choices.list[l].help:"" );
          }
          break;
        case oyjlOPTIONTYPE_FUNCTION:
          {
            int n = 0,l;
            oyjlOptionChoice_s * list;
            t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MAN);
            oyjlStringAdd( &text, malloc, free, "%s", t );
            free(t);
            oyjlStringAdd( &text, malloc, free, "\t%s%s%s%s", o->description ? o->description:"", o->help && o->help[0]?"\n.RS\n":"", o->help?o->help:"", o->help?"\n.RE\n":"\n.br\n" );
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            list = oyjlOption_GetChoices_(o, NULL, opts );
            if(list)
              while(list[n].nick && list[n].nick[0] != '\000')
                ++n;
            for(l = 0; l < n; ++l)
              oyjlStringAdd( &text, malloc, free, "\t\\-%s %s\t\t# %s\n.br\n", o->o, list[l].nick, list[l].name && list[l].nick[0] ? list[l].name : list[l].description );
            /* not possible, as the result of oyjlOption_GetChoices_() is cached - oyjlOptionChoice_Release( &list ); */
          }
          break;
        case oyjlOPTIONTYPE_DOUBLE:
          {
            t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MAN);
            oyjlStringAdd( &text, malloc, free, "%s", t );
            free(t);
            oyjlStringAdd( &text, malloc, free, "\t%s (%s%s%g [≥%g ≤%g])%s%s%s", o->description ? o->description:"", o->value_name?o->value_name:"", o->value_name?":":"", o->values.dbl.d, o->values.dbl.start, o->values.dbl.end, o->help?"\n.RS\n":"", o->help?o->help:"", o->help?"\n.RE\n":"\n.br\n" );
          }
          break;
        case oyjlOPTIONTYPE_NONE:
          {
            t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MAN);
            oyjlStringAdd( &text, malloc, free, "%s", t );
            free(t);
            oyjlStringAdd( &text, malloc, free, "\t%s%s%s%s", o->description ? o->description:"", o->help?"\n.RS\n":"", o->help?o->help:"", o->help?"\n.RE\n":"\n.br\n" );
          }
        break;
        case oyjlOPTIONTYPE_START: break;
        case oyjlOPTIONTYPE_END: break;
      }
    }
    oyjlStringListRelease( &d_list, dn, free );
  }

  tmp = oyjlExtraManSections( opts, oyjlOPTIONSTYLE_MAN, NULL, NULL );
  if(tmp)
  {
    oyjlStringAdd( &text, malloc, free, "%s", tmp );
    free(tmp);
  }

  if(mnft)
    oyjlStringAdd( &text, malloc, free, ".SH %s\n%s %s\n", _("AUTHOR"), mnft, mnft_url?mnft_url:"" );

  if(lice || copy)
  {
    oyjlStringAdd( &text, malloc, free, ".SH %s\n%s\n", _("COPYRIGHT"), copy?copy:"" );
    if(lice)
      oyjlStringAdd( &text, malloc, free, ".br\n%s: %s\n", _("License"), lice?lice:"" );
  }

  if(bugs)
    oyjlStringAdd( &text, malloc, free, ".SH %s\n%s %s\n", _("BUGS"), bugs, bugs_url?bugs_url:"" );
  else if(bugs_url)
    oyjlStringAdd( &text, malloc, free, ".SH %s\n%s\n", _("BUGS"), bugs_url );

  return text;
}

static void replaceOutsideHTML(const char * text OYJL_UNUSED, const char * start, const char * end, const char * search, const char ** replace, void * data)
{
  if(start < end)
  {
    const char * word = start;
    int * insideTable = (int*) data;

    if( insideTable[0] )
      *replace = search;
    else
    {
      if(strcmp(search,"`") == 0)
        *replace = "\\`";
      if(strcmp(search,"-") == 0)
        *replace = "\\-";
      if(strcmp(search,"_") == 0)
        *replace = "\\_";
    }

    word = start;
    while(word && (word = strstr(word+1,"<table")) != NULL && word < end)
      ++insideTable[0];
    word = start;
    while(word && (word = strstr(word+1,"</table>")) != NULL && word < end)
      --insideTable[0];

  }
}

/** @brief    Return markdown formated text from options
 *  @memberof oyjlUi_s
 *
 *  @see oyjlUi_ToMan()
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/08/02
 *  @since   2018/11/07 (OpenICC: 0.1.1)
 */
char *       oyjlUi_ToMarkdown       ( oyjlUi_s          * ui,
                                       int                 flags )
{
  char * text = NULL, * tmp, * doxy_link = NULL;
  const char * date = NULL,
             * desc = NULL,
             * mnft = NULL, * mnft_url = NULL,
             * copy = NULL, * lice = NULL,
             * bugs = NULL, * bugs_url = NULL,
             * vers = NULL,
             * country = NULL;
  int i,n,ng;
  oyjlOptions_s * opts;
  char ** sections_ = NULL, *** sections = &sections_;
  int sn_ = 0, * sn = &sn_;

  if( !ui ) return text;

  opts = ui->opts;
  n = oyjlUi_CountHeaderSections( ui );
  for(i = 0; i < n; ++i)
  {
    oyjlUiHeaderSection_s * s = &ui->sections[i];
    if(strcmp(s->nick, "manufacturer") == 0) { mnft = s->name; mnft_url = s->description; }
    else if(strcmp(s->nick, "copyright") == 0) copy = s->name;
    else if(strcmp(s->nick, "license") == 0) lice = s->name;
    else if(strcmp(s->nick, "url") == 0) continue;
    else if(strcmp(s->nick, "support") == 0) { bugs = s->name; bugs_url = s->description; }
    else if(strcmp(s->nick, "download") == 0) continue;
    else if(strcmp(s->nick, "sources") == 0) continue;
    else if(strcmp(s->nick, "development") == 0) continue;
    else if(strcmp(s->nick, "oyjl_module_author") == 0) continue;
    else if(strcmp(s->nick, "documentation") == 0) desc = s->description ? s->description : s->name;
    else if(strcmp(s->nick, "version") == 0) vers = s->name;
    else if(strcmp(s->nick, "date") == 0) date = s->description ? s->description : s->name;
  }

  ng = oyjlOptions_CountGroups(opts);
  if(!ng && !(flags & oyjlUI_STATE_NO_CHECKS)) return NULL;

#ifdef OYJL_HAVE_LANGINFO_H
  country = nl_langinfo( _NL_ADDRESS_LANG_AB );
#endif

  oyjlStringAdd( &doxy_link, malloc, free, "{#%s%s}", ui->nick, country?country:"" );
  oyjlStringReplace( &doxy_link, "-", "", malloc, free );

  if(ui->app_type && ui->app_type[0])
  {
    int tool = strcmp( ui->app_type, "tool" ) == 0;
    oyjlStringAdd( &text, malloc, free, "*\"%s\"* *%d* *\"%s\"* \"%s\"\n", ui->nick,
                   tool?1:7, date?date:"", tool?"User Commands":"Misc" );
  }

  ADD_SECTION( _("NAME"), "name", "%s %s%s - %s\n", ui->nick, vers?"v":"", vers?vers:"", ui->name )

  if(ng)
  ADD_SECTION( _("SYNOPSIS"), "synopsis", "", "" )
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    char * syn = oyjlOptions_PrintHelpSynopsis( opts, g,
                         oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN );
    if(syn[0])
      oyjlStringAdd( &text, malloc, free, "%s\n%s", syn, (i < (ng-1)) ? "<br />\n" : "" );
    free(syn);
  }

  if(desc)
    ADD_SECTION( _("DESCRIPTION"), "description", "%s\n", desc )

  if(ng)
  ADD_SECTION( _("OPTIONS"), "options", "", "" )
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    int d = 0,
        j;
    char ** d_list = oyjlStringSplit2( g->detail, "|,", &d, NULL, malloc ),
         * t;
    if(g->description)
      oyjlStringAdd( &text, malloc, free, "### %s\n", g->description  );
    if(g->mandatory && g->mandatory[0])
    {
      char * t = oyjlOptions_PrintHelpSynopsis( opts, g, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN );
      oyjlStringAdd( &text, malloc, free, "&nbsp;&nbsp;%s\n", t );
      free(t);
    }
    oyjlStringAdd( &text, malloc, free, "\n"  );
    if(g->help)
    {
      oyjlStringAdd( &text, malloc, free, "%s\n\n", g->help );
    }
    if(d)
      oyjlStringAdd( &text, malloc, free, "<table style='width:100%'>\n" );
    for(j = 0; j < d; ++j)
    {
      const char * option = d_list[j];
      oyjlOption_s * o = oyjlOptions_GetOptionL( opts, option );
      if(!o)
      {
        fprintf(stderr, "\n%s: option not declared: %s\n", g->name, option );
        exit(1);
      }
#define OYJL_LEFT_TD_STYLE " style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'"
      switch(o->value_type)
      {
        case oyjlOPTIONTYPE_CHOICE:
          {
            int n = 0,l;
            t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MARKDOWN);
            oyjlStringAdd( &text, malloc, free, " <tr><td" OYJL_LEFT_TD_STYLE ">%s</td>", t );
            free(t);
            oyjlStringAdd( &text, malloc, free, " <td>%s%s%s", o->description ? o->description:"", o->help?"<br />":"", o->help?o->help :"" );
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            while(o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000')
              ++n;
            if(n) oyjlStringAdd( &text, malloc, free, "\n  <table>\n");
            for(l = 0; l < n; ++l)
              oyjlStringAdd( &text, malloc, free, "   <tr><td style='padding-left:0.5em'><strong>-%s %s</strong></td><td># %s</td></tr>\n", o->o, o->values.choices.list[l].nick, o->values.choices.list[l].name && o->values.choices.list[l].nick[0] ? o->values.choices.list[l].name : o->values.choices.list[l].description );
            if(n) oyjlStringAdd( &text, malloc, free, "  </table>\n");
            oyjlStringAdd( &text, malloc, free, "  </td>\n");
          }
          break;
        case oyjlOPTIONTYPE_FUNCTION:
          {
            int n = 0,l;
            oyjlOptionChoice_s * list;
            t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MARKDOWN);
            oyjlStringAdd( &text, malloc, free, " <tr><td" OYJL_LEFT_TD_STYLE ">%s</td>", t );
            free(t);
            oyjlStringAdd( &text, malloc, free, " <td>%s%s%s", o->description ? o->description:"", o->help?"<br />":"", o->help?o->help :"" );
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            list = oyjlOption_GetChoices_(o, NULL, opts );
            if(list)
              while(list[n].nick && list[n].nick[0] != '\000')
                ++n;
            if(n) oyjlStringAdd( &text, malloc, free, "\n  <table>\n");
            for(l = 0; l < n; ++l)
              oyjlStringAdd( &text, malloc, free, "   <tr><td style='padding-left:0.5em'><strong>-%s %s</strong></td><td># %s</td></tr>\n", o->o, list[l].nick, list[l].name && list[l].nick[0] ? list[l].name : list[l].description );
            if(n) oyjlStringAdd( &text, malloc, free, "  </table>\n");
            oyjlStringAdd( &text, malloc, free, "  </td>\n");
            /* not possible, as the result of oyjlOption_GetChoices_() is cached - oyjlOptionChoice_Release( &list ); */
          }
          break;
        case oyjlOPTIONTYPE_DOUBLE:
          t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MARKDOWN);
          oyjlStringAdd( &text, malloc, free, " <tr><td" OYJL_LEFT_TD_STYLE ">%s</td>", t );
          free(t);
          oyjlStringAdd( &text, malloc, free, " <td>%s%s%s (%s%s%g [≥%g ≤%g])</td>", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"", o->value_name?o->value_name:"", o->value_name?":":"", o->values.dbl.d, o->values.dbl.start, o->values.dbl.end );
          break;
        case oyjlOPTIONTYPE_NONE:
          t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MARKDOWN);
          oyjlStringAdd( &text, malloc, free, " <tr><td" OYJL_LEFT_TD_STYLE ">%s</td>", t );
          free(t);
          oyjlStringAdd( &text, malloc, free, " <td>%s%s%s</td>", o->description ? o->description:"", o->help?"<br />":"", o->help?o->help :"" );
        break;
        case oyjlOPTIONTYPE_START: break;
        case oyjlOPTIONTYPE_END: break;
      }
      oyjlStringAdd( &text, malloc, free, " </tr>\n" );
    }
    oyjlStringListRelease( &d_list, d, free );
    if(d)
      oyjlStringAdd( &text, malloc, free, "</table>\n" );
    oyjlStringAdd( &text, malloc, free, "\n"  );
  }

  tmp = oyjlExtraManSections( opts, oyjlOPTIONSTYLE_MARKDOWN, sections, sn );
  if(tmp)
  {
    oyjlStringAdd( &text, malloc, free, "%s", tmp );
    free(tmp);
  }

  if(mnft)
    ADD_SECTION( _("AUTHOR"), "author", "%s %s\n", mnft, mnft_url?mnft_url:"" )

  if(lice || copy)
  {
    ADD_SECTION( _("COPYRIGHT"), "copyright", "*%s*\n", copy?copy:"" )
    if(lice)
      oyjlStringAdd( &text, malloc, free, "\n\n### %s <a name=\"license\"></a>\n%s\n", _("License"), lice?lice:"" );
  }

  if(bugs && bugs_url)
    ADD_SECTION( _("BUGS"), "bugs", "%s [%s](%s)\n", bugs, bugs_url, bugs_url )
  else if(bugs)
    ADD_SECTION( _("BUGS"), "bugs", "[%s](%s)\n", bugs, bugs )

  {
    char * txt = NULL;
    int i;

    oyjlStringAdd( &txt, malloc, free, "# %s %s%s %s\n", ui->nick, vers?"v":"", vers?vers:"", doxy_link );
    for(i = 0; i < sn_/2; ++i)
      oyjlStringAdd( &txt, malloc, free, "[%s](#%s) ", sections_[2*i+0], sections_[2*i+1] );
    oyjlStringAdd( &txt, malloc, free, "\n\n%s", text );
    free(text);
    text = txt;
  }

  {
    const char * t;
    int insideTable[3] = {0,0,0};
    oyjl_str tmp = oyjlStrNew(10,0,0);
    oyjlStrAppendN( tmp, text, strlen(text) );
    oyjlStrReplace( tmp, "`", "\\`", replaceOutsideHTML, insideTable );
    oyjlStrReplace( tmp, "-", "\\-", replaceOutsideHTML, insideTable );
    oyjlStrReplace( tmp, "_", "\\_", replaceOutsideHTML, insideTable );
    t = oyjlStr(tmp);
    text[0] = 0;
    oyjlStringAdd( &text, malloc,free, "%s", t );
    oyjlStrRelease( &tmp );
  }

  free(doxy_link);
  oyjlStringListRelease( sections, sn_, free );

  return text;
}
// TODO: increase robustness
// TODO: TOC in markdown like for HTML pages
// TODO: make the qml renderer aware of mandatory options as part of sending a call to the tool; add action button to all manatory options except bool options; render mandatory switch as a button
// TODO: MAN page synopsis logic ...
// TODO: man page generator: /usr/share/man/man1/ftp.1.gz + http://man7.org/linux/man-pages/man7/groff_mdoc.7.html
// TODO: synopsis syntax ideas: https://unix.stackexchange.com/questions/17833/understand-synopsis-in-manpage


/** 
 *  @} *//* oyjl_args
 */

/* private stuff */

oyjlUiHeaderSection_s * oyjlUiInfo   ( const char          * documentation )
{
  oyjlUiHeaderSection_s s[] = {
    /* type,  nick,      label,name,                 description */
    { "oihs", "version", NULL, OYJL_VERSION_NAME, NULL },
    { "oihs", "manufacturer", NULL, "Kai-Uwe Behrmann", "http://www.oyranos.org" },
    { "oihs", "copyright", NULL, "Copyright © 2018-2019 Kai-Uwe Behrmann", NULL },
    { "oihs", "license", NULL, "newBSD", "http://www.oyranos.org" },
    { "oihs", "url", NULL, "http://www.oyranos.org", NULL },
    { "oihs", "support", NULL, "https://www.github.com/oyranos-cms/oyranos/issues", NULL },
    { "oihs", "download", NULL, "https://github.com/oyranos-cms/oyranos/releases", NULL },
    { "oihs", "sources", NULL, "https://github.com/oyranos-cms/oyranos", NULL },
    { "oihs", "development", NULL, "https://github.com/oyranos-cms/oyranos", NULL },
    { "oihs", "oyjl_module_author", NULL, "Kai-Uwe Behrmann", "http://www.behrmann.name" },
    { "oihs", "documentation", NULL, "http://www.oyranos.org", documentation },
    { "oihs", "date", NULL, "1970-01-01T12:00:00", "" },
    { "", NULL, NULL, NULL, NULL }
  };
  return (oyjlUiHeaderSection_s*) oyjlStringAppendN( NULL, (const char*)s, sizeof(s), malloc );
}

