/** @file oyCMMapi7_s.c

   [Template file inheritance graph]
   +-> oyCMMapi7_s.template.c
   |
   +-> oyCMMapiFilter_s.template.c
   |
   +-> oyCMMapi_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2013 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2013/06/17
 */


  
#include "oyCMMapi7_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyCMMapi7_s_.h"
  


/** Function oyCMMapi7_New
 *  @memberof oyCMMapi7_s
 *  @brief   allocate a new CMMapi7 object
 */
OYAPI oyCMMapi7_s * OYEXPORT
  oyCMMapi7_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMapi7_s_ * cmmapi7 = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  cmmapi7 = oyCMMapi7_New_( s );

  return (oyCMMapi7_s*) cmmapi7;
}

/** Function oyCMMapi7_Copy
 *  @memberof oyCMMapi7_s
 *  @brief   copy or reference a CMMapi7 object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmapi7                 CMMapi7 struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyCMMapi7_s* OYEXPORT
  oyCMMapi7_Copy( oyCMMapi7_s *cmmapi7, oyObject_s object )
{
  oyCMMapi7_s_ * s = (oyCMMapi7_s_*) cmmapi7;

  if(s)
    oyCheckType__m( oyOBJECT_CMM_API7_S, return 0 )

  s = oyCMMapi7_Copy_( s, object );

  return (oyCMMapi7_s*) s;
}
 
/** Function oyCMMapi7_Release
 *  @memberof oyCMMapi7_s
 *  @brief   release and possibly deallocate a oyCMMapi7_s object
 *
 *  @param[in,out] cmmapi7                 CMMapi7 struct object
 */
OYAPI int OYEXPORT
  oyCMMapi7_Release( oyCMMapi7_s **cmmapi7 )
{
  oyCMMapi7_s_ * s = 0;

  if(!cmmapi7 || !*cmmapi7)
    return 0;

  s = (oyCMMapi7_s_*) *cmmapi7;

  oyCheckType__m( oyOBJECT_CMM_API7_S, return 1 )

  *cmmapi7 = 0;

  return oyCMMapi7_Release_( &s );
}



/* Include "CMMapi7.public_methods_definitions.c" { */
/** Function  oyCMMapi7_Create
 *  @memberof oyCMMapi7_s
 *  @brief    Create a oyCMMapi7_s filter object
 *
 *  @param         init                custom initialisation
 *  @param         msg_set             message function setter
 *  @param         registration        the modules @ref registration string,
 *  @param         version             module version
 *  - 0: major - should be stable for the live time of a filter
 *  - 1: minor - mark new features
 *  - 2: patch version - correct errors
 *  @param         module_api          tell compatibility with Oyranos API
 *  - 0: last major Oyranos version during development time, e.g. 0
 *  - 1: last minor Oyranos version during development time, e.g. 9
 *  - 2: last Oyranos patch version during development time, e.g. 5
 *  @param         context_type        four byte identifier if a context is needed
 *                                     A additional oyCMMapi6_s module can, 
 *                                     then be provided to convert between
 *                                     context formats, registred by this
 *                                     identifier.
 *  @param         run                 the filter function
 *  @param         plugs               a zero terminated list of connectors
 *  @param         plugs_n             the number of plugs in 'plugs'
 *  @param         plugs_last_add      the number of accepted repeats of the
 *                                     last plug
 *  @param         sockets             a zero terminated list of connectors
 *  @param         sockets_n           the number of sockets in 'sockets'
 *  @param         sockets_last_add    the number of accepted repeats of the
 *                                     last socket
 *  @param         properties
 *  A zero terminated list of strings. The semantics are plugin defined.
 *  e.g.: {"key1=value1","key2=value2",0}
 *  @param         object              Oyranos object (optional)
 *
 *  @see     oyCMMapi_s::oyCMMapi_Set()
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/07/11 (Oyranos: 0.9.5)
 *  @date    2013/07/11
 */
OYAPI oyCMMapi7_s *  OYEXPORT
             oyCMMapi7_Create        ( oyCMMInit_f         init,
                                       oyCMMMessageFuncSet_f msg_set,
                                       const char        * registration,
                                       int32_t             version[3],
                                       int32_t             module_api[3],
                                       const char        * context_type,
                                       oyCMMFilterPlug_Run_f run,
                                       oyConnector_s    ** plugs,
                                       uint32_t            plugs_n,
                                       uint32_t            plugs_last_add,
                                       oyConnector_s    ** sockets,
                                       uint32_t            sockets_n,
                                       uint32_t            sockets_last_add,
                                       const char       ** properties,
                                       oyObject_s          object )
{
  oyCMMapi7_s_ * s = (oyCMMapi7_s_*) oyCMMapi7_New( object );
  int n = 0;

  if(!s) return NULL;

  oyCMMapi_Set( (oyCMMapi_s*) s, init, msg_set, registration,
                version, module_api );

  s->oyCMMFilterPlug_Run = run;
  if(context_type)
    memcpy( s->context_type, context_type, 4 );
  s->plugs = plugs;
  s->plugs_n = plugs_n;
  s->plugs_last_add = plugs_last_add;
  s->sockets = sockets;
  s->sockets_n = sockets_n;
  s->sockets_last_add = sockets_last_add;
  while(properties && properties[n]) ++n;
  oyStringListAdd_( &s->properties, 0, properties, n,
                    oyObject_GetAlloc( s->oy_ ),
                    oyObject_GetDeAlloc( s->oy_ ) );

  return (oyCMMapi7_s*) s;
}

OYAPI int OYEXPORT
             oyCMMapi7_Run           ( oyCMMapi7_s       * api7,
                                       oyFilterPlug_s    * plug,
                                       oyPixelAccess_s   * ticket )
{
  oyCMMapi7_s_ * s = (oyCMMapi7_s_*)api7;
  int error = !plug;

  if(!api7)
    return -1;

  oyCheckType__m( oyOBJECT_CMM_API7_S, return 1 )

  if(!plug)
  {
    WARNcc_S(api7, "no plug argument given" );
    return error;
  }

  error = s->oyCMMFilterPlug_Run( plug, ticket );

  return error;
}


/* } Include "CMMapi7.public_methods_definitions.c" */

