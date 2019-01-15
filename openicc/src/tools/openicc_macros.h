/** openicc_macros.h
 *
 *  @par License: 
 *             MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @par Copyright:
 *             (c) 2011-2017 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 *  @brief    helper macros
 */

#ifndef OPENICC_MACROS_H
#define OPENICC_MACROS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if !defined(USE_GETTEXT) && !defined(_)
#define _(text) text
#endif

#include "openicc_version.h"

extern openiccMessage_f     openiccMessage_p;

#define DBG(context, format_,...) openiccMessage_p( openiccMSG_DBG, context, \
                                                format_, __VA_ARGS__)
#define WARN(context, format_,...) openiccMessage_p( openiccMSG_WARN, context, \
                                                format_, __VA_ARGS__)
#define ERR(context, format_,...) openiccMessage_p( openiccMSG_ERROR, context, \
                                                format_, __VA_ARGS__)



#define verbose oy_debug

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OPENICC_MACROS_H */
