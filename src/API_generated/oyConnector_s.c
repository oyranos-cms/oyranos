/** @file oyConnector_s.c

   [Template file inheritance graph]
   +-> oyConnector_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyConnector_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyConnector_s_.h"

#ifdef oyCheckCType__m
#undef oyCheckCType__m
#endif
#define oyCheckCType__m( type, action ) \
  if( !s || !(oyOBJECT_CONNECTOR_S <= s->type_ && s->type_ < oyOBJECT_CONNECTOR_MAX_S)) \
  { \
    WARNc3_S( "%s %s(%s)", _("Unexpected object type:"), \
              oyStructTypeToText( s ? s->type_ : oyOBJECT_NONE ), \
              oyStructTypeToText( type )) \
    action; \
  }
  


/** Function oyConnector_New
 *  @memberof oyConnector_s
 *  @brief   allocate a new Connector object
 */
OYAPI oyConnector_s * OYEXPORT
  oyConnector_New( oyObject_s object )
{
  oyObject_s s = object;
  oyConnector_s_ * connector = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  connector = oyConnector_New_( s );

  return (oyConnector_s*) connector;
}

/** Function  oyConnector_Copy
 *  @memberof oyConnector_s
 *  @brief    Copy or Reference a Connector object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     connector                 Connector struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyConnector_s* OYEXPORT
  oyConnector_Copy( oyConnector_s *connector, oyObject_s object )
{
  oyConnector_s_ * s = (oyConnector_s_*) connector;

  if(s)
  {
    oyCheckCType__m( oyOBJECT_CONNECTOR_S, return NULL )
  }
  else
    return NULL;

  s = oyConnector_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyConnector_s" );

  return (oyConnector_s*) s;
}
 
/** Function oyConnector_Release
 *  @memberof oyConnector_s
 *  @brief   release and possibly deallocate a oyConnector_s object
 *
 *  @param[in,out] connector                 Connector struct object
 */
OYAPI int OYEXPORT
  oyConnector_Release( oyConnector_s **connector )
{
  oyConnector_s_ * s = 0;

  if(!connector || !*connector)
    return 0;

  s = (oyConnector_s_*) *connector;

  oyCheckCType__m( oyOBJECT_CONNECTOR_S, return 1 )

  *connector = 0;

  return oyConnector_Release_( &s );
}




/* Include "Connector.public_methods_definitions.c" { */

/* } Include "Connector.public_methods_definitions.c" */

#define oyCheckConnectorType__m( type, action ) \
if(!(oyOBJECT_CONNECTOR_S <= s->type_ && s->type_ < oyOBJECT_CONNECTOR_MAX_S)) \
    { action; }

/** Function  oyConnector_SetTexts
 *  @memberof oyConnector_s
 *  @brief    set the texts in a connector
 *
 *  Set UI strings.
 *
 *  @param[in]     obj                 Connector object
 *  @param[in]     getText             the name function
 *  @param[in]     text_classes        zero terminated list of classes, 
 *                                     e.g. {"name",NULL}
 *                                     owned by the user, but has to live the
 *                                     lifetime of the object
 *  @return                            status
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/06/10
 *  @since    2013/06/10 (Oyranos: 0.9.5)
 */
OYAPI int  OYEXPORT
                 oyConnector_SetTexts( oyConnector_s     * obj,
                                       oyCMMGetText_f      getText,
                                       const char       ** text_classes )
{
  oyConnector_s_ * s = (oyConnector_s_*) obj;

  if(!obj)
    return 1;

  oyCheckConnectorType__m( oyOBJECT_CONNECTOR_S, return 1 )

  s->getText = getText;
  s->texts = text_classes;

  return 0;
}

/** Function  oyConnector_GetTexts
 *  @memberof oyConnector_s
 *  @brief    get the text classes in a connector
 *
 *  Set UI strings.
 *
 *  @param[in]     obj                 Connector object
 *  @return        text_classes        zero terminated list of classes, 
 *                                     e.g. {"name",NULL}
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/06/10
 *  @since    2013/06/10 (Oyranos: 0.9.5)
 */
OYAPI const char **  OYEXPORT
                 oyConnector_GetTexts( oyConnector_s     * obj )
{
  oyConnector_s_ * s = (oyConnector_s_*) obj;

  if(!obj)
    return 0;

  oyCheckConnectorType__m( oyOBJECT_CONNECTOR_S, return 0 )

  return s->texts;
}

/** Function  oyConnector_GetText
 *  @memberof oyConnector_s
 *  @brief    get the names in a connector
 *
 *  Get UI strings.
 *
 *  @param[in]     obj                 Connector object
 *  @param[in]     name_class          the names class, e.g. "name", "help" ...
 *  @param[in]     type                the names type
 *  @return                            the name string
 *
 *  @version  Oyranos: 0.9.5
 *  @since    2011/01/31 (Oyranos: 0.3.0)
 *  @date     2013/06/10
 */
OYAPI const char *  OYEXPORT
                 oyConnector_GetText ( oyConnector_s     * obj,
                                       const char        * name_class,
                                       oyNAME_e            type )
{
  oyConnector_s_ * s = (oyConnector_s_*) obj;
  const char * string = 0;

  if(!obj)
    return 0;

  oyCheckConnectorType__m( oyOBJECT_CONNECTOR_S, return 0 )

  string = s->getText( name_class, type, (oyStruct_s*)s );

  return string;
}

/** Function  oyConnector_IsPlug
 *  @memberof oyConnector_s
 *  @brief    Is this connector a plug or a socket
 *
 *  @param[in]     obj                 Connector object
 *  @return                            boolean; 0 - socket; 1 - plug
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oyConnector_IsPlug  ( oyConnector_s     * obj )
{
  oyConnector_s_ * s = (oyConnector_s_*)obj;

  if(!obj)
    return 0;

  oyCheckConnectorType__m( oyOBJECT_CONNECTOR_S, return 0 )

  return s->is_plug;
}

/** Function  oyConnector_SetIsPlug
 *  @memberof oyConnector_s
 *  @brief    Set this connector as a plug or a socket
 *
 *  @param[in,out] obj                 Connector object
 *  @param[in]     is_plug             boolean; 0 - socket; 1 - plug
 *  @return                            1 - error; 0 - success; -1 - otherwise
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oyConnector_SetIsPlug(oyConnector_s     * obj,
                                       int                 is_plug )
{
  oyConnector_s_ * s = (oyConnector_s_*)obj;

  if(!obj)
    return 0;

  oyCheckConnectorType__m( oyOBJECT_CONNECTOR_S, return 1 )

  s->is_plug = is_plug;

  return 0;
}

/** Function  oyConnector_GetReg
 *  @memberof oyConnector_s
 *  @brief    Get the registration for the connection type
 *
 *  This is use as a rough check, if connections are possible.
 *
 *  @param[in]     obj                 Connector object
 *  @return                            registration string
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
const char *     oyConnector_GetReg  ( oyConnector_s     * obj )
{
  oyConnector_s_ * s = (oyConnector_s_*)obj;

  if(!obj)
    return 0;

  oyCheckConnectorType__m( oyOBJECT_CONNECTOR_S, return 0 )

  return s->connector_type;
}

/** Function  oyConnector_SetReg
 *  @memberof oyConnector_s
 *  @brief    Set this connectors type string
 *
 *  This is use as a rough check, if connections are possible.
 *
 *  @param[in,out] obj                 Connector object
 *  @param[in]     type_registration   the registration string to describe the
 *                                     type
 *  @return                            1 - error; 0 - success; -1 - otherwise
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oyConnector_SetReg  ( oyConnector_s     * obj,
                                       const char        * type_registration )
{
  oyConnector_s_ * s = (oyConnector_s_*)obj;

  if(!obj)
    return 0;

  oyCheckConnectorType__m( oyOBJECT_CONNECTOR_S, return 1 )

  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    oyAlloc_f allocateFunc = s->oy_->allocateFunc_;

    if(s->connector_type)
    { deallocateFunc( s->connector_type ); s->connector_type = 0; }

    s->connector_type = oyStringCopy_( type_registration, allocateFunc );
  }

  return 0;
}

/** Function  oyConnector_SetMatch
 *  @memberof oyConnector_s
 *  @brief    Set this connectors type check function
 *
 *  This is use as a check, if connections are possible.
 *  This allowes for a more fine grained control than the type registration.
 *
 *  @param[in,out] obj                 Connector object
 *  @param[in]     func                the check function
 *  @return                            1 - error; 0 - success; -1 - otherwise
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oyConnector_SetMatch( oyConnector_s     * obj,
                                       oyCMMFilterSocket_MatchPlug_f func )
{
  oyConnector_s_ * s = (oyConnector_s_*)obj;

  if(!obj)
    return 0;

  oyCheckConnectorType__m( oyOBJECT_CONNECTOR_S, return 1 )

  s->filterSocket_MatchPlug = func;

  return 0;
}

/** Function  oyConnector_GetMatch
 *  @memberof oyConnector_s
 *  @brief    Set this connectors type check function
 *
 *  This is use as a check, if connections are possible.
 *  This allowes for a more fine grained control than the type registration.
 *
 *  @param[in]     obj                 Connector object
 *  @return                            the check function
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
oyCMMFilterSocket_MatchPlug_f oyConnector_GetMatch (
                                       oyConnector_s     * obj )
{
  oyConnector_s_ * s = (oyConnector_s_*)obj;

  if(!obj)
    return 0;

  oyCheckConnectorType__m( oyOBJECT_CONNECTOR_S, return 0 )

  return s->filterSocket_MatchPlug;
}

