/**
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2007  Kai-Uwe Behrmann
 *
 * @autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * -----------------------------------------------------------------------------
 */

/** API @internal
 * 
 */

/** @date      25. 11. 2004 */

/* Dont use in non Oyranos projects. */

#ifndef OYRANOS_HELPER_H
#define OYRANOS_HELPER_H

#include "config.h"
#include "oyranos_debug.h"
#if defined(OY_CONFIG_H)
#include "oyranos.h"
#else
#include "oyranos/oyranos.h"
#endif

#include <unistd.h> /* intptr_t */
#include <ctype.h>  /* toupper */

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */

/* memory handling */

void* oyAllocateFunc_           (size_t        size);
void* oyAllocateWrapFunc_       (size_t        size,
                                 oyAllocFunc_t allocate_func);
void  oyDeAllocateFunc_         (void *        data);


/* complete an name from file including oyResolveDirFileName */
char*   oyMakeFullFileDirName_     (const char* name);
/* find an file/dir and do corrections on  ~ ; ../  */
char*   oyResolveDirFileName_      (const char* name);
char*   oyExtractPathFromFileName_ (const char* name);
char*   oyGetHomeDir_              ();
char*   oyGetParent_               (const char* name);
int     oyRecursivePaths_      (int (*doInPath) (void*,const char*,const char*),
                                void        * data,
                                const char ** path_names,
                                int           path_count);

int oyIsDir_      (const char* path);
int oyIsFile_     (const char* fileName);
int oyIsFileFull_ (const char* fullFileName);
int oyMakeDir_    (const char* path);

int   oyWriteMemToFile_ (const char* name, void* mem, size_t size);
char* oyReadFileToMem_  (const char* fullFileName, size_t *size,
                         oyAllocFunc_t allocate_func);


/* oyNoEmptyName_( name ) */
#define oyNoEmptyName_m_( text ) text?text:"\"---\""
extern intptr_t oy_observe_pointer_;

/* oyFree_ (void*) */
#define oyFree_m_(x) {                                      \
  if(oy_observe_pointer_ == (intptr_t)x)                    \
    WARNc_S(( "%s:%d %s() pointer %s freed",                 \
            __FILE__,__LINE__,__func__,#x ));               \
  if (x != NULL) {    /* defined in oyranos_helper.h */     \
    oyDeAllocateFunc_ (x);                                  \
    x = NULL;                                               \
  } else {                                                  \
    char *t = _("%s:%d %s() nothing to delete %s\n");       \
    WARNc_S (( t, __FILE__,__LINE__,__func__, #x ));         \
  }                                                         \
}

/* oyAllocHelper_ (void*, type, size_t, action) */ 
#define oyAllocHelper_m_(ptr_, type, size_, alloc_func, action) { \
  if (ptr_ != NULL)    /* defined in oyranos_helper.h */    \
    oyFree_m_( ptr_ )                                       \
  if ((size_) <= 0) {                                       \
    WARNc_S ((_("%s:%d %s() nothing to allocate - size: %d\n"), \
    __FILE__,__LINE__,__func__, (int)(size_)));             \
  } else {                                                  \
      ptr_ = (type*) oyAllocateWrapFunc_(sizeof (type) * (size_t)(size_), \
                                         alloc_func ); \
      memset( ptr_, 0, sizeof (type) * (size_t)(size_) );   \
  }                                                         \
  if (ptr_ == NULL) {                                       \
    WARNc_S( ("%s:%d %s() %s %d %s %s .",__FILE__,__LINE__, \
         __func__, _("Can not allocate"),(int)(size_),      \
         _("bytes of  memory for"), #ptr_));                \
    action;                                                 \
  }                                                         \
}

/* oyPostAllocHelper_ (void*, size, action) */
#define oyPostAllocHelper_m_(ptr_, size_, action) {         \
  if ((size_) <= 0 ||                                       \
      ptr_ == NULL ) { /* defined in oyranos_helper.h */    \
    WARNc_S ((_("%s:%d %s() nothing allocated %s\n"),        \
    __FILE__,__LINE__,__func__, #ptr_));                    \
    action;                                                 \
  }                                                         \
}

/* string helpers to switch to unicode or utf8 */
#define oyAllocString_m_( sptr_, ssize_,  salloc_func, saction ) \
  oyAllocHelper_m_( sptr_, oyChar, ssize_+1, salloc_func, saction );
#define oyStrlen_( str_ ) \
           strlen( str_ )
#define oySprintf_ \
           sprintf
#define oySnprintf_( str_, len_, patrn_, args_ ) \
           snprintf( str_, len_, patrn_, args_ )
#define oyStrcpy_( targ_, src_ ) \
           strcpy( targ_, src_ )
#define oyStrrchr_( str_, c_ ) \
           strrchr( str_, c_ )
#define oyStrstr_( str1_, str2_ ) \
           strstr( str1_, str2_ )
#define oyStrcmp_( str1_, str2_ ) \
           strcmp( str1_, str2_ )
#define oyToupper_( c_ ) \
           toupper( c_ )
size_t oyStrblen_(const char *s);


/* mathematical helpers */

#define MIN(a,b)    (((a) <= (b)) ? (a) : (b))
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define HYP(a,b)    pow((a)*(a) + (b)*(b),1.0/2.0)
#define HYP3(a,b,c) pow( (a)*(a) + (b)*(b) + (c)*(c) , 1.0/2.0)
#define RUND(a)     ((a) + 0.5)



#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_HELPER_H */
