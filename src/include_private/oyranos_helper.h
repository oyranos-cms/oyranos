/** @internal
 *  @file oyranos_helper.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2005-2011 (C) Kai-Uwe Behrmann
 *
 *  @brief    helpers
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/09/02
 *
 *  Dont use in non Oyranos projects.
 */


#ifndef OYRANOS_HELPER_H
#define OYRANOS_HELPER_H

#include "oyranos_config_internal.h"
#include "oyranos_debug.h"
#include "oyStruct_s.h"
#if defined(OY_CONFIG_H)
#include "oyranos_core.h"
#else
#include "oyranos.h"
#endif

#include "oyranos_helper_macros.h"

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
#define OY_PRINT_POINTER "0x%tx"
extern int oy_debug_signals;
extern const char * oy_backtrace;



/* --- string helpers --- */

void     oyShowMessage               ( int                 type,
                                       const char        * show_text,
                                       int                 show_gui );


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
  const char* colorsig;
  int flags;
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
int oyIsFileFull_ (const char* fullFileName, const char * read_mode);
size_t oyReadFileSize_(const char* name);
int oyMakeDir_    (const char* path);
int  oyRemoveFile_                   ( const char        * full_file_name );

int   oyWriteMemToFile_ (const char* name, const void* mem, size_t size);

#define OY_FILE_APPEND 0x01
#define OY_FILE_NAME_SEARCH 0x02
#define OY_FILE_TEMP_DIR 0x04
char * oyGetTempFileName_            ( const char        * name,
                                       uint32_t            flags,
                                       oyAlloc_f           allocateFunc );
int  oyWriteMemToFile2_              ( const char        * name,
                                       void              * mem,
                                       size_t              size,
                                       uint32_t            flags,
                                       char             ** result,
                                       oyAlloc_f           allocateFunc );
char* oyReadFileToMem_  (const char* fullFileName, size_t *size,
                         oyAlloc_f     allocate_func);
char * oyReadStdinToMem_             ( size_t            * size,
                                       oyAlloc_f           allocate_func );
char * oyReadFilepToMem_             ( FILE              * file,
                                       size_t            * size,
                                       oyAlloc_f           allocate_func );
char * oyReadFileSToMem_             ( FILE              * fp,
                                       size_t            * size,
                                       oyAlloc_f           allocate_func);
char * oyReadUrlToMem_               ( const char        * url,
                                       size_t            * size,
                                       const char        * mode,
                                       oyAlloc_f           allocate_func );
char * oyReadUrlToMemf_              ( size_t            * size,
                                       const char        * mode,
                                       oyAlloc_f           allocate_func,
                                       const char        * format,
                                                           ... );


/* --- mathematical helpers --- */

float        oyLinInterpolateRampU16 ( uint16_t          * ramp,
                                       int                 ramp_size,
                                       float               pos );
float        oyLinInterpolateRampF32 ( float             * ramp,
                                       int                 ramp_size,
                                       float               pos );
double       oyLinInterpolateRampF64 ( double            * ramp,
                                       int                 ramp_size,
                                       double              pos );
uint16_t       oyAddU16              ( uint16_t            value1,
                                       uint16_t            value2 );
uint16_t       oySubstU16            ( uint16_t            value1,
                                       uint16_t            value2 );
uint16_t       oyMultU16             ( uint16_t            value1,
                                       uint16_t            value2 );
uint16_t       oyDivU16              ( uint16_t            value1,
                                       uint16_t            value2 );
int            oyRampManipulateU16   ( uint16_t          * ramp1,
                                       uint16_t          * ramp2,
                                       uint16_t          * ramp_result,
                                       int                 ramp_size,
                                       uint16_t          (*manip_f)(uint16_t,uint16_t) );
float          oyAddF32              ( float               value1,
                                       float               value2 );
float          oySubstF32            ( float               value1,
                                       float               value2 );
float          oyMultF32             ( float               value1,
                                       float               value2 );
float          oyDivF32              ( float               value1,
                                       float               value2 );
int            oyRampManipulateF32   ( float             * ramp1,
                                       float             * ramp2,
                                       float             * ramp_result,
                                       int                 ramp_size,
                                       float             (*manip_f)(float,float) );
int            oyRampManipulateF64   ( double            * ramp1,
                                       double            * ramp2,
                                       double            * ramp_result,
                                       int                 ramp_size,
                                       double            (*manip_f)(double,double) );

/* --- miscellaneous helpers --- */

int                oyMiscBlobGetMD5_ ( const void        * buffer,
                                       size_t              size,
                                       unsigned char     * md5_return );
uint32_t           oyMiscBlobL3_     ( void              * buffer,
                                       size_t              size );
enum {
  oyHASH_MD5  = 1,
  oyHASH_L3   = 2
};
int                oyMiscBlobGetHash_( void              * buffer,
                                       size_t              size,
                                       uint32_t            flags,
                                       unsigned char     * md5_return );

#ifdef HAVE_DL
#include <dlfcn.h> /* dlopen() */
#elif defined(HAVE_LTDL)
#include <ltdl.h>
#define dlopen(a,b)  lt_dlopen(a)
#define dlsym   lt_dlsym
#define dlerror lt_dlerror
#define dlclose lt_dlclose
#define RTLD_LAZY 1
#else
#error "need dlfcn.h or ltdl to open modules. STOP"
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_HELPER_H */
