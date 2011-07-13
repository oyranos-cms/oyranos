/** @file oyranos_helper.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2005-2011 (C) Kai-Uwe Behrmann
 *
 *  @brief    helper macros
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2010/11/16
 *
 */


#ifndef OYRANOS_HELPER_MACROS_H
#define OYRANOS_HELPER_MACROS_H

#if defined(OY_CONFIG_H)
#include "oyranos.h"
#else
#include "oyranos/oyranos.h"
#endif

#include "oyObject_s_.h"

#include "stdio.h"
#include "math.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* --- memory handling --- */

/* oyFree_ (void*) */
#define oyFree_m_(x) {                                      \
  char text_fm[80];                                         \
  if(oy_observe_pointer_ == (intptr_t)x) {                  \
    oySnprintf_( text_fm, 80, #x " pointer freed" );    \
    WARNc_S( text_fm ); }                                   \
  if (x != NULL) {    /* defined in oyranos_helper.h */     \
    oyDeAllocateFunc_ (x);                                  \
    x = NULL;                                               \
  } else {                                                  \
    oySnprintf1_(text_fm, 80, "%s " #x, _("nothing to delete")); \
    WARNc_S ( text_fm );                                    \
  }                                                         \
}

/* oyAllocHelper_ (void*, type, size_t, action) */ 
#define oyAllocHelper_m_(ptr_, type, size_, alloc_func, action) { \
  if (ptr_ != NULL)    /* defined in oyranos_helper.h */    \
  { \
    char text_fm[80];                                       \
    if(oy_observe_pointer_ == (intptr_t)ptr_) {             \
      oySnprintf_( text_fm, 80, #ptr_ " pointer freed" ); \
      WARNc_S( text_fm );                                   \
    }                                                       \
    /*oyDeAllocateFunc_ (ptr_);*/                           \
    oySnprintf_( text_fm, 80, "!!! " #ptr_ " needed to free pointer" ); \
    WARNc_S( text_fm );                                   \
    ptr_ = NULL;                                            \
  }                                                         \
  if ((size_) <= 0) {                                       \
    WARNc2_S ("%s %d", _("nothing to allocate - size:"),    \
              (int)(size_) );                               \
  } else {                                                  \
      ptr_ = (type*) oyAllocateWrapFunc_(sizeof (type) * (size_t)(size_), \
                                         alloc_func );      \
      memset( ptr_, 0, sizeof (type) * (size_t)(size_) );   \
  }                                                         \
  if (ptr_ == NULL) {                                       \
    WARNc3_S( "%s %d %s", _("Can not allocate memory for:"),\
              (int)(size_), #ptr_ );                        \
    action;                                                 \
  }                                                         \
}

/* oyAllocHelper_ (void*, type, size_t, action) */ 
#define oyStruct_AllocHelper_m_(ptr_, type, size_, obj_, action) { \
  if (ptr_ != NULL)    /* defined in oyranos_helper.h */    \
  { \
    char text_fm[80];                                       \
    if(oy_observe_pointer_ == (intptr_t)ptr_) {             \
      oySnprintf_( text_fm, 80, #ptr_ " pointer freed" ); \
      WARNc_S( text_fm );                                   \
    }                                                       \
    obj_->oy_->deallocateFunc_ (ptr_);                      \
    ptr_ = NULL;                                            \
  }                                                         \
  if ((size_) <= 0) {                                       \
    WARNc2_S ("%s %d", _("nothing to allocate - size:"),    \
              (int)(size_) );                               \
  } else {                                                  \
      ptr_ = (type*) oyStruct_Allocate( (oyStruct_s*) obj_, \
                                        sizeof (type) * (size_t)(size_)); \
      memset( ptr_, 0, sizeof (type) * (size_t)(size_) );   \
  }                                                         \
  if (ptr_ == NULL) {                                       \
    WARNc3_S( "%s %d %s", _("Can not allocate memory for:"),\
              (int)(size_), #ptr_ );                        \
    action;                                                 \
  }                                                         \
}


/* oyPostAllocHelper_ (void*, size, action) */
#define oyPostAllocHelper_m_(ptr_, size_, action) {         \
  if ((size_) <= 0 ||                                       \
      ptr_ == NULL ) { /* defined in oyranos_helper.h */    \
    WARNc2_S ("%s %s", _("nothing allocated"), #ptr_ );     \
    action;                                                 \
  }                                                         \
}


/* --- string helpers --- */

/* string helpers to switch to unicode or utf8 */
#define oyAllocString_m_( sptr_, ssize_,  salloc_func, saction ) \
  oyAllocHelper_m_( sptr_, oyChar, ssize_+1, salloc_func, saction );
#define oySnprintf_( str_, len_, patrn_ ) \
           snprintf( str_, len_, patrn_ )
#define oySnprintf1_( str_, len_, patrn_, arg ) \
           snprintf( str_, len_, patrn_, arg )
#define oySnprintf2_( str_, len_, patrn_, arg, arg2 ) \
           snprintf( str_, len_, patrn_, arg, arg2 )
#define oySnprintf3_( str_, len_, patrn_, arg, arg2, arg3 ) \
           snprintf( str_, len_, patrn_, arg, arg2, arg3 )
#define oySnprintf4_( str_, len_, patrn_, arg, arg2, arg3, arg4 ) \
           snprintf( str_, len_, patrn_, arg, arg2, arg3, arg4 )
#define oySnprintf5_( str_, len_, patrn_, arg, arg2, arg3, arg4, arg5 ) \
           snprintf( str_, len_, patrn_, arg, arg2, arg3, arg4, arg5 )
#define oySscanf_ sscanf
#define oySprintf_ \
           sprintf
int     oyStrlen_ ( const char * str_ );
void    oyStrcpy_ ( char * targ_, const char * src_ );
char *  oyStrchr_ ( const char * str_, char c_ );
char *  oyStrrchr_( const char * str_, char c_ );
char *  oyStrstr_ ( const char * str1_, const char * str2_ );
int     oyStrcmp_ ( const char * str1_, const char * str2_ );
char    oyToupper_( char c_ );

/* allow "--opt val" and "--opt=val" syntax */
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
                        if(oy_debug) printf(#opt "=%d\n",opt)
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
                        if(oy_debug) fprintf(stderr, #opt "=%s\n",opt)
#define OY_PARSE_STRING_ARG2( opt, arg ) \
                        if( pos + 1 < argc && argv[pos][i+strlen(arg)+1] == 0 ) \
                        { opt = argv[pos+1]; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+strlen(arg)+1] == '=') \
                        { opt = &argv[pos][i+strlen(arg)+2]; \
                          i = 1000; \
                        } else wrong_arg = "-" arg; \
                        if(oy_debug) printf(arg "=%s\n",opt)
#define OY_IS_ARG( arg ) \
                        (strlen(argv[pos])-2 >= strlen(arg) && \
                         memcmp(&argv[pos][2],arg, strlen(arg)) == 0)


/* --- mathematical helpers --- */

#define OY_MIN(a,b)    (((a) <= (b)) ? (a) : (b))
#define OY_MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define OY_HYP(a,b)    pow((a)*(a) + (b)*(b),1.0/2.0)
#define OY_HYP3(a,b,c) pow( (a)*(a) + (b)*(b) + (c)*(c) , 1.0/2.0)
#define OY_ROUND(a)    ((int)floor((a)+0.5))

/* --- miscellaneous helpers --- */

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_HELPER_MACROS_H */
