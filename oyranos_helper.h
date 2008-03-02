/**
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2005  Kai-Uwe Behrmann
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
 *
 * API
 * 
 */

/** @date      25. 11. 2004 */


#ifndef OYRANOS_HELPER_H
#define OYRANOS_HELPER_H

#include "oyranos.h"
#include "oyranos_debug.h"
#include "config.h"

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */

/* memory handling */

void* oyAllocateFunc_           (size_t        size);
void  oyDeAllocateFunc_         (void*         data);


/* complete an name from file including oyResolveDirFileName */
char*   oyMakeFullFileDirName_     (const char* name);
/* find an file/dir and do corrections on  ~ ; ../  */
char*   oyResolveDirFileName_      (const char* name);
char*   oyExtractPathFromFileName_ (const char* name);
char*   oyGetHomeDir_              ();
char*   oyGetParent_               (const char* name);
int     oyRecursivePaths_      (int (*doInPath) (void*,const char*,const char*),
                                void* data);

int oyIsDir_      (const char* path);
int oyIsFile_     (const char* fileName);
int oyIsFileFull_ (const char* fullFileName);
int oyMakeDir_    (const char* path);

int   oyWriteMemToFile_ (const char* name, void* mem, size_t size);
char* oyReadFileToMem_  (const char* fullFileName, size_t *size,
                         oyAllocFunc_t allocate_func);


/* oyFree_ (void*) */
#define oyFree_m_(x) {                                      \
  if (x != NULL) {    /* defined in oyranos_helper.h */     \
    free (x); x = NULL;                                     \
  } else {                                                  \
    WARN_S (("%s:%d %s() nothing to delete " #x "\n",       \
    __FILE__,__LINE__,__func__));                           \
  }                                                         \
}

/* oyAllocHelper_ (void*, type, size_t, action) */ 
#define oyAllocHelper_m_(ptr_, type, size_, alloc_func, action) { \
  if (ptr_ != NULL)    /* defined in oyranos_helper.h */     \
    oyFree_m_( ptr_ )                                        \
  if ((size_) <= 0) {                                       \
    WARN_S (("%s:%d %s() nothing to allocate - size: %d\n", \
    __FILE__,__LINE__,__func__, (int)size_));               \
  } else {                                                  \
    oyAllocFunc_t temp = alloc_func;                        \
    if( temp )                                              \
      ptr_ = (type*) temp( (size_t)(size_) * sizeof(type) ); \
    else                                                    \
      ptr_ = (type*) calloc (sizeof (type), (size_t)size_);  \
  }                                                         \
  if (ptr_ == NULL) {                                        \
    WARN_S( ("%s:%d %s() %s %d %s %s .",__FILE__,__LINE__,  \
         __func__, _("Can not allocate"),(int)size_,        \
         _("bytes of  memory for"), #ptr_));                 \
    action;                                                 \
  }                                                         \
}

/* oyPostAllocHelper_ (void*, size, action) */
#define oyPostAllocHelper_m_(ptr_, size_, action) {          \
  if ((size_) <= 0 ||                                       \
      ptr_ == NULL ) { /* defined in oyranos_helper.h */     \
    WARN_S (("%s:%d %s() nothing allocated %s\n",           \
    __FILE__,__LINE__,__func__, #ptr_));                     \
    action;                                                 \
  }                                                         \
}

#ifdef USE_GETTEXT
# include <libintl.h>
# include <locale.h>
extern const char *domain;
# define _(text) gettext( text )
#else
# define _(text) text
#endif

/* mathematical helpers */

#define MIN(a,b)    (((a) <= (b)) ? (a) : (b))
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define HYP(a,b)    pow((a)*(a) + (b)*(b),1.0/2.0)
#define HYP3(a,b,c) pow( (a)*(a) + (b)*(b) + (c)*(c) , 1.0/2.0)
#define RUND(a)     ((a) + 0.5)



#ifdef __cplusplus
} // extern "C"
} // namespace oyranos
#endif /* __cplusplus */

#endif /* OYRANOS_HELPER_H */
