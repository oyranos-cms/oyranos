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
#define OYJL_PARSE_INT_ARG( opt ) \
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
#define OYJL_PARSE_INT_ARG2( opt, arg ) \
                        if( pos + 1 < argc && argv[pos][i+strlen(arg)+1] == 0 ) \
                        { opt = atoi(argv[pos+1]); \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+strlen(arg)+1] == '=') \
                        { opt = atoi(&argv[pos][i+strlen(arg)+2]); \
                          i = 1000; \
                        } else wrong_arg = "-" arg; \
                        if(*oyjl_debug) fprintf(stderr,arg "=%d\n",opt)
#define OYJL_PARSE_STRING_ARG( opt ) \
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
#define OYJL_PARSE_STRING_ARG2( opt, arg ) \
                        if( pos + 1 < argc && argv[pos][i+strlen(arg)+1] == 0 ) \
                        { opt = argv[pos+1]; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+strlen(arg)+1] == '=') \
                        { opt = &argv[pos][i+strlen(arg)+2]; \
                          i = 1000; \
                        } else wrong_arg = "-" arg; \
                        if(*oyjl_debug) fprintf(stderr,arg "=%s\n",opt)
#define OYJL_IS_ARG( arg ) \
                        (strlen(argv[pos])-2 >= strlen(arg) && \
                         memcmp(&argv[pos][2],arg, strlen(arg)) == 0)

/** Obtain result from oyjlOptions_s by name e.g. inside ::oyjlOPTIONTYPE_FUNCTION callbacks.
 *  Example: OYJL_GET_RESULT_STRING( opts, "o", NULL, my_o_string_variable ); */
#define OYJL_GET_RESULT_STRING( options_, optionL_, default_, variable_ ) const char * variable_; if(oyjlOptions_GetResult( options_, optionL_, &variable_, NULL, NULL ) != oyjlOPTION_USER_CHANGED) variable_ = default_;
/** obtain result from oyjlOptions_s by name e.g. inside ::oyjlOPTIONTYPE_FUNCTION callbacks */
#define OYJL_GET_RESULT_DOUBLE( options_, optionL_, default_, variable_ ) double       variable_; if(oyjlOptions_GetResult( options_, optionL_, NULL, &variable_, NULL ) != oyjlOPTION_USER_CHANGED) variable_ = default_;
/** obtain result from oyjlOptions_s by name e.g. inside ::oyjlOPTIONTYPE_FUNCTION callbacks */
#define OYJL_GET_RESULT_INT( options_, optionL_, default_, variable_ )    int          variable_; if(oyjlOptions_GetResult( options_, optionL_, NULL, NULL, &variable_ ) != oyjlOPTION_USER_CHANGED) variable_ = default_;

#ifndef OYJL_DBG_FORMAT
# if defined(__GNUC__)
#  define  OYJL_DBG_FORMAT "%s "
#  define  OYJL_DBG_ARGS   oyjlFunctionPrint( __func__, strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__ )
//#  define  OYJL_DBG_FORMAT "%s(%s:%d) "
//#  define  OYJL_DBG_ARGS   oyjlTermColor(oyjlBOLD, __func__), strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__
# else
#  define  OYJL_DBG_FORMAT "%s:%d "
#  define  OYJL_DBG_ARGS   strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__
# endif
#endif

#define OYJL_E(x_,empty_) (x_?x_:empty_)

extern oyjlMessage_f oyjlMessage_p;
/** convert ( const char * format, ... ) function args into a string */
#ifndef OYJL_CREATE_VA_STRING
#define OYJL_CREATE_VA_STRING(format_, text_, alloc_, error_action) \
if(format_ && strchr(format_,'%') != NULL) { \
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
    text_ = (char*)allocate_( sizeof(char) * len + 2 ); \
    if(!text_) \
    { \
      oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "could not allocate memory", OYJL_DBG_ARGS ); \
      error_action; \
    } \
    va_start( list, format_); \
    len = vsnprintf( text_, len+1, format_, list); \
    va_end  ( list ); \
  } \
} else if(format_) \
{ \
  text_ = oyjlStringCopy( format_, alloc_ );\
}
#endif

#ifdef OYJL_HAVE_LOCALE_H
#define OYJL_SETLOCALE_C \
  char * save_locale = oyjlStringCopy( setlocale(LC_NUMERIC, 0 ), malloc ); \
  setlocale(LC_NUMERIC, "C");
#define OYJL_SETLOCALE_RESET \
  setlocale(LC_NUMERIC, save_locale); \
  if(save_locale) free( save_locale );
#else
#define OYJL_SETLOCALE_C
#endif

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

#if defined(_WIN32)                    
# define oyjlPOPEN_m    _popen
# define oyjlPCLOSE_m   _pclose
#else
# define oyjlPOPEN_m    popen
# define oyjlPCLOSE_m   pclose
#endif

#endif /* OYJL_MACROS_H */
