/** \file oyjl_macros.h
 *
 *  @par License: 
 *             MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @par Copyright:
 *             (c) 2011 - Kai-Uwe Behrmann <ku.b@gmx.de>
 */

#ifndef OYJL_MACROS_H
#define OYJL_MACROS_H

#include "oyjl_version.h"

/* command line parsing macros */
/* allow "-opt val" and "-opt=val" syntax */
#define OY_PARSE_INT_ARG( opt ) \
                        if( pos + 1 < argc && argv[pos][i+1] == 0 ) \
                        { opt = atoi( argv[pos+1] ); \
                          if( opt == 0 && strcmp(argv[pos+1],"0") ) \
                            wrong_arg = "-" #opt; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+1] == '=') \
                        { opt = atoi( &argv[pos][i+2] ); \
                          if( opt == 0 && strcmp(&argv[pos][i+2],"0") ) \
                            wrong_arg = "-" #opt; \
                          i = 1000; \
                        } else wrong_arg = "-" #opt; \
                        if(*oyjl_debug) fprintf(stderr,#opt "=%d\n",opt)
#define OY_PARSE_INT_ARG2( opt, arg ) \
                        if( pos + 1 < argc && argv[pos][i+strlen(arg)+1] == 0 ) \
                        { opt = atoi(argv[pos+1]); \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+strlen(arg)+1] == '=') \
                        { opt = atoi(&argv[pos][i+strlen(arg)+2]); \
                          i = 1000; \
                        } else wrong_arg = "-" arg; \
                        if(*oyjl_debug) fprintf(stderr,arg "=%d\n",opt)
#define OY_PARSE_STRING_ARG( opt ) \
                        if( pos + 1 < argc && argv[pos][i+1] == 0 ) \
                        { opt = argv[pos+1]; \
                          if( opt == 0 && strcmp(argv[pos+1],"0") ) \
                            wrong_arg = "-" #opt; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+1] == '=') \
                        { opt = &argv[pos][i+2]; \
                          if( opt == 0 && strcmp(&argv[pos][i+2],"0") ) \
                            wrong_arg = "-" #opt; \
                          i = 1000; \
                        } else wrong_arg = "-" #opt; \
                        if(*oyjl_debug) fprintf(stderr, #opt "=%s\n",opt)
#define OY_PARSE_STRING_ARG2( opt, arg ) \
                        if( pos + 1 < argc && argv[pos][i+strlen(arg)+1] == 0 ) \
                        { opt = argv[pos+1]; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+strlen(arg)+1] == '=') \
                        { opt = &argv[pos][i+strlen(arg)+2]; \
                          i = 1000; \
                        } else wrong_arg = "-" arg; \
                        if(*oyjl_debug) fprintf(stderr,arg "=%s\n",opt)
#define OY_IS_ARG( arg ) \
                        (strlen(argv[pos])-2 >= strlen(arg) && \
                         memcmp(&argv[pos][2],arg, strlen(arg)) == 0)


extern oyjlMessage_f oyjlMessage_p;
/** convert ( const char * format, ... ) function args into a string */
#define OYJL_CREATE_VA_STRING(format_, text_, alloc_, error_action) \
{ \
  va_list list; \
  size_t sz = 0; \
  int len = 0; \
  void*(* allocate)(size_t size) = alloc_; \
  if(!allocate) allocate = malloc; \
\
  text_ = NULL; \
  \
  va_start( list, format_); \
  len = vsnprintf( text_, sz, format_, list); \
  va_end  ( list ); \
\
  { \
    text_ = allocate( sizeof(char) * len + 2 ); \
    if(!text_) \
    { \
      oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "could not allocate memory", OYJL_DBG_ARGS_ ); \
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


#endif /* OYJL_MACROS_H */
