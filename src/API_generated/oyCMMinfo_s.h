/** @file oyCMMinfo_s.h

   [Template file inheritance graph]
   +-> oyCMMinfo_s.template.h
   |
   +-> Base_s.h
   |
   +-- oyStruct_s.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



#ifndef OY_CMM_INFO_S_H
#define OY_CMM_INFO_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyCMMinfo_s oyCMMinfo_s;


#include "oyStruct_s.h"


/* Include "CMMinfo.public.h" { */
#include "oyranos_module.h"

/**
 *  typedef oyCMMinfoInit_f
 *  @brief   optional module init function
 *  @ingroup module_api
 *  @memberof oyCMMinfo_s
 */
typedef int      (*oyCMMinfoInit_f)  ( oyStruct_s        * library );

/**
 *  typedef oyCMMinfoReset_f
 *  @brief   optional module reset function
 *  @ingroup module_api
 *  @memberof oyCMMinfo_s
 *  @see oyCMMinfoInit_f
 */
typedef int      (*oyCMMinfoReset_f) ( oyStruct_s        * library );


/* } Include "CMMinfo.public.h" */


/* Include "CMMinfo.dox" { */
/** @struct  oyCMMinfo_s 
 *  @ingroup module_api
 *  @extends oyStruct_s
 *  @brief   The CMM API resources struct to implement and set by a CMM
 *  @internal
 *  
 *  Given an example CMM with name "little cms", which wants to use the 
 *  four-char ID 'lcms', the CMM can register itself to Oyranos as follows:
 *  The CMM module file must be named
 *  something_lcms_cmm_module_something.something .
 *  On Linux this could be "liboyranos_lcms_cmm_module.so.0.1.8".
 *  The four-chars 'lcms' must be prepended with OY_MODULE_NAME alias
 *  "_cmm_module".
 *
 *  Oyranos will scan the $(libdir)/color/cmms/ path, opens the available 
 *  CMM's from this directory and extracts the four-chars before OY_MODULE_NAME
 *  from the library file names. Module paths can be added through the
 *  OY_MODULE_PATH environment variable.
 *  Oyranos looks for a symbol to a oyCMMinfo_s struct of the four-byte ID plus
 *  OY_MODULE_NAME which results in our example in the name "lcms_cmm_module".
 *  On Posix system this should be loadable by dlsym.
 *  The lcms_cmm_module should be of type oyCMMinfo_s with the type field
 *  and all other fields set appropriately.
 *
 *  The api field is a placeholder to get a real api struct assigned. If the CMM
 *  wants to provide more than one API, they can be chained.
 *
 *  @version Oyranos: 0.5.0
 *  @date    2012/09/22
 *  @since   2007/12/05 (Oyranos: 0.1.8)
 */

/* } Include "CMMinfo.dox" */

struct oyCMMinfo_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyCMMinfo_s* OYEXPORT
  oyCMMinfo_New( oyObject_s object );
OYAPI oyCMMinfo_s* OYEXPORT
  oyCMMinfo_Copy( oyCMMinfo_s *cmminfo, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMinfo_Release( oyCMMinfo_s **cmminfo );



/* Include "CMMinfo.public_methods_declarations.h" { */
#include "oyCMMapi_s.h"

OYAPI const char *  OYEXPORT
                   oyCMMinfo_GetCMM  ( oyCMMinfo_s       * info );
OYAPI const char *  OYEXPORT
                   oyCMMinfo_GetVersion
                                     ( oyCMMinfo_s       * info );
OYAPI oyCMMGetText_f  OYEXPORT
                   oyCMMinfo_GetTextF( oyCMMinfo_s       * info );
OYAPI const char **  OYEXPORT
                   oyCMMinfo_GetTexts( oyCMMinfo_s       * info );
OYAPI int  OYEXPORT
                   oyCMMinfo_GetCompatibility
                                     ( oyCMMinfo_s       * info );
OYAPI oyCMMapi_s * OYEXPORT
                   oyCMMinfo_GetApi  ( oyCMMinfo_s       * info );
OYAPI void  OYEXPORT
                   oyCMMinfo_SetApi  ( oyCMMinfo_s       * info,
                                       oyCMMapi_s        * first_api );
#ifdef ICON_IS_OBJECTIFIED
OYAPI oyIcon_s * OYEXPORT
                   oyCMMinfo_GetIcon ( oyCMMinfo_s       * info );
#endif
OYAPI oyCMMinfoInit_f  OYEXPORT
                   oyCMMinfo_GetInitF( oyCMMinfo_s       * info );
OYAPI void  OYEXPORT
                   oyCMMinfo_SetInitF( oyCMMinfo_s       * info,
                                       oyCMMinfoInit_f     init );
OYAPI oyCMMinfoReset_f  OYEXPORT
                   oyCMMinfo_GetResetF(oyCMMinfo_s       * info );
OYAPI void  OYEXPORT
                   oyCMMinfo_SetResetF(oyCMMinfo_s       * info,
                                       oyCMMinfoReset_f    reset );

/* } Include "CMMinfo.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_INFO_S_H */
