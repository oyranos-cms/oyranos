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

//#include "oyranos.h"
#include "oyranos_debug.h"

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */

/* memory handling */

void* oyAllocateFunc_           (size_t        size);
/*void  oyDeAllocateFunc_         (void*  data);*/

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
#define oyAllocHelper_m_(ptr, type, size_, alloc_func, action) { \
  if (ptr != NULL)    /* defined in oyranos_helper.h */     \
    oyFree_m_( ptr )                                        \
  if ((size_) <= 0) {                                       \
    WARN_S (("%s:%d %s() nothing to allocate - size: %d\n", \
    __FILE__,__LINE__,__func__, (int)size_));               \
  } else {                                                  \
    oyAllocFunc_t temp = alloc_func;                        \
    if( temp )                                              \
      ptr = (type*) temp( (size_t)size_ );                  \
    else                                                    \
      ptr = (type*) calloc (sizeof (type), (size_t)size_);  \
  }                                                         \
  if (ptr == NULL) {                                        \
    WARN_S( ("%s:%d %s() %s %d %s %s .",__FILE__,__LINE__,  \
         __func__, _("Can not allocate"),(int)size_,        \
         _("bytes of  memory for"), #ptr));                 \
    action;                                                 \
  }                                                         \
}

/* oyPostAllocHelper_ (void*, size, action) */
#define oyPostAllocHelper_m_(ptr, size_, action) {          \
  if ((size_) <= 0 ||                                       \
      ptr == NULL ) { /* defined in oyranos_helper.h */     \
    WARN_S (("%s:%d %s() nothing allocated %s\n",           \
    __FILE__,__LINE__,__func__, #ptr));                     \
    action;                                                 \
  }                                                         \
}

#define _(text) text


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
