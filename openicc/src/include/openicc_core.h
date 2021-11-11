/**
 *  @file openicc_core.h
 *
 *  libOpenICC - OpenICC Colour Management Tools
 *
 *  @par Copyright:
 *            2011-2018 (C) Kai-Uwe Behrmann
 *
 *  @brief    OpenICC Colour Management core types
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2011/10/21
 */

#ifndef __OPENICC_CORE_H__
#define __OPENICC_CORE_H__

#include <stdio.h>


#if   defined(__clang__)
#define OI_FALLTHROUGH
#elif __GNUC__ >= 7 
#define OI_FALLTHROUGH                 __attribute__ ((fallthrough));
#else
#define OI_FALLTHROUGH
#endif

#if   __GNUC__ >= 7
#define OI_DEPRECATED                  __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define OI_DEPRECATED                  __declspec(deprecated)
#else
#define OI_DEPRECATED
#endif

#if   (__GNUC__*100 + __GNUC_MINOR__) >= 406
#define OI_UNUSED                      __attribute__ ((unused))
#elif defined(_MSC_VER)
#define OI_UNUSED                      __declspec(unused)
#else
#define OI_UNUSED
#endif

/** \addtogroup misc

 *  @{ */

int            openiccInit           ( const char        * loc );

typedef void * (*openiccAlloc_f)     ( size_t              size );
typedef void   (*openiccDeAlloc_f)   ( void              * data );

#define OPENICC_SLASH "/"

/** @brief customisable messages */
#define OI_DEBUG                       "OI_DEBUG"
extern int * openicc_debug;
extern int openicc_backtrace;

typedef enum {
  openiccMSG_ERROR = 300,              /**< @brief fatal user messages */
  openiccMSG_WARN,                     /**< @brief log messages */
  openiccMSG_DBG,                      /**< @brief developer messages */
} openiccMSG_e;

typedef int  (*openiccMessage_f)     ( int/*openiccMSG_e*/ error_code,
                                       const void        * context_object,
                                       const char        * format,
                                       ... );
int            openiccMessageFuncSet ( openiccMessage_f    message_func );
int            openiccVersion        ( void );
void           openiccSetDebugVariable(int               * cmm_debug );

int            openiccReadFileSToMem ( FILE              * fp,
                                       char             ** ptr,
                                       int               * size);
void *         openiccMemDup         ( const void        * src,
                                       size_t              size );
void           openiccLibRelease     ( void );
/** 
 *  @} *//* misc
 */

#endif /* __OPENICC_CORE_H__ */
