/** @file oyranos_helper.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2005-2010 (C) Kai-Uwe Behrmann
 *
 *  @brief    helpers
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/09/02
 *
 *  Dont use in non Oyranos projects.
 */


#ifndef OYRANOS_HELPER_H
#define OYRANOS_HELPER_H

#include "config.h"
#include "oyranos_debug.h"
#include "oyranos_alpha.h"
#if defined(OY_CONFIG_H)
#include "oyranos.h"
#else
#include "oyranos/oyranos.h"
#endif


#include <ctype.h>  /* toupper */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* --- memory handling --- */

void* oyAllocateFunc_           (size_t        size);
void* oyAllocateWrapFunc_       (size_t        size,
                                 oyAlloc_f     allocate_func);
void  oyDeAllocateFunc_         (void *        data);


extern intptr_t oy_observe_pointer_;
extern int oy_debug_memory;
extern int oy_debug_signals;
extern const char * oy_backtrace;

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
#ifdef DEBUG
int     oyStrlen_ ( const char * str_ );
void    oyStrcpy_ ( char * targ_, const char * src_ );
char *  oyStrchr_ ( const char * str_, char c_ );
char *  oyStrrchr_( const char * str_, char c_ );
char *  oyStrstr_ ( const char * str1_, const char * str2_ );
int     oyStrcmp_ ( const char * str1_, const char * str2_ );
char    oyToupper_( char c_ );
#else
#define oyStrlen_( str_ ) \
           strlen( str_ )
#define oyStrcpy_( targ_, src_ ) \
           strcpy( targ_, src_ )
#define oyStrchr_( str_, c_ ) \
           strchr( str_, c_ )
#define oyStrrchr_( str_, c_ ) \
           strrchr( str_, c_ )
#define oyStrstr_( str1_, str2_ ) \
           strstr( str1_, str2_ )
#define oyStrcmp_( str1_, str2_ ) \
           strcmp( str1_, str2_ )
#define oyToupper_( c_ ) \
           toupper( c_ )
#endif



/* --- file i/o helpers --- */

/* complete an name from file including oyResolveDirFileName_ */
char*   oyMakeFullFileDirName_     (const char* name);
/* find an file/dir and do corrections on  ~ ; ../  */
char*   oyResolveDirFileName_      (const char* name);
char*   oyExtractPathFromFileName_ (const char* name);
char*   oyGetHomeDir_              ();
char*   oyGetParent_               (const char* name);
struct oyFileList_s {
  oyOBJECT_e type;                  /* oyOBJECT_FILE_LIST_S_ */
  int hopp;
  const char* coloursig;
  int mem_count;
  int count_files;
  char** names;
};
typedef struct oyFileList_s oyFileList_s;
int     oyRecursivePaths_      (int (*doInPath) (oyFileList_s *,
                                const char*,const char*),
                                struct oyFileList_s        * data,
                                const char ** path_names,
                                int           path_count);

int oyIsDir_      (const char* path);
int oyIsFile_     (const char* fileName);
int oyIsFileFull_ (const char* fullFileName);
int oyMakeDir_    (const char* path);
int  oyRemoveFile_                   ( const char        * full_file_name );

int   oyWriteMemToFile_ (const char* name, const void* mem, size_t size);

#define OY_FILE_APPEND 0x01
#define OY_FILE_NAME_SEARCH 0x02
#define OY_FILE_TEMP_DIR 0x04
int  oyWriteMemToFile2_              ( const char        * name,
                                       void              * mem,
                                       size_t              size,
                                       uint32_t            flags,
                                       char             ** result,
                                       oyAlloc_f           allocateFunc );
char* oyReadFileToMem_  (const char* fullFileName, size_t *size,
                         oyAlloc_f     allocate_func);



/* --- mathematical helpers --- */

#define OY_MIN(a,b)    (((a) <= (b)) ? (a) : (b))
#define OY_MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define OY_HYP(a,b)    pow((a)*(a) + (b)*(b),1.0/2.0)
#define OY_HYP3(a,b,c) pow( (a)*(a) + (b)*(b) + (c)*(c) , 1.0/2.0)
#define OY_ROUND(a)    ((a) + 0.5)

/* --- miscellaneous helpers --- */

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

int                oyMiscBlobGetMD5_ ( void              * buffer,
                                       size_t              size,
                                       unsigned char     * md5_return );
uint32_t           oyMiscBlobGetL3_  ( void              * buffer,
                                       size_t              size );

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_HELPER_H */
