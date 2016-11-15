/**
 *  @internal
 *  @file     oyjl_tree_internal.h
 *
 *  oyjl - convinient tree JSON APIs
 *
 *  @par Copyright:
 *            2011-2016 (C) Kai-Uwe Behrmann
 *
 *  @brief    tree based JSON API
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2016/10/28
 */

#ifndef OYJL_TREE_INTERNAL_H
#define OYJL_TREE_INTERNAL_H 1

#include "oyjl_tree.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) 
#define snprintf sprintf_s
#endif
#define oyjlAllocHelper_m_(ptr_, type, size_, alloc_func, action) { \
  if ((size_) <= 0) {                                       \
      oyjl_message_p( oyjl_message_insufficient_data, 0, "Nothing to allocate"); \
  } else {                                                  \
      void*(*a)(size_t size) = alloc_func;                  \
      if(!a) a = malloc;                                    \
      ptr_ = (type*) a(sizeof (type) * (size_t)(size_));    \
      memset( ptr_, 0, sizeof (type) * (size_t)(size_) );   \
  }                                                         \
  if (ptr_ == NULL) {                                       \
      oyjl_message_p( oyjl_message_error, 0, "Out of memory"); \
    action;                                                 \
  }                                                         \
}

#if defined(__GNUC__)
# define  OYJL_DBG_FORMAT_ "%s:%d %s() "
# define  OYJL_DBG_ARGS_   strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__,__func__
#else
# define  OYJL_DBG_FORMAT_ "%s:%d "
# define  OYJL_DBG_ARGS_   strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__
#endif

extern oyjl_message_f oyjl_message_p;

#ifdef __cplusplus
}
#endif

#endif /* OYJL_TREE_INTERNAL_H */
