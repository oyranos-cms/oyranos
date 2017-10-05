{% extends "Base_s.c" %}

{% block LocalIncludeFiles %}
{{ block.super }}
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
{% endblock %}

{% block Copy_CheckType %}oyCheckCType__m{% endblock %}
{% block Release_CheckType %}oyCheckCType__m{% endblock %}


{% block SpecificPublicMethodsDefinitions %}
{{ block.super }}
#define oyCheck{{ class.baseName }}Type__m( type, action ) \
if(!(oyOBJECT_CONNECTOR_S <= s->type_ && s->type_ < oyOBJECT_CONNECTOR_MAX_S)) \
    { action; }

/** Function  oy{{ class.baseName }}_SetTexts
 *  @memberof {{ class.name }}
 *  @brief    set the texts in a connector
 *
 *  Set UI strings.
 *
 *  @param[in]     obj                 {{ class.baseName }} object
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
                 oy{{ class.baseName }}_SetTexts( {{ class.name }}     * obj,
                                       oyCMMGetText_f      getText,
                                       const char       ** text_classes )
{
  {{ class.name }}_ * s = ({{ class.name }}_*) obj;

  if(!obj)
    return 1;

  oyCheck{{ class.baseName }}Type__m( oyOBJECT_CONNECTOR_S, return 1 )

  s->getText = getText;
  s->texts = text_classes;

  return 0;
}

/** Function  oy{{ class.baseName }}_GetTexts
 *  @memberof {{ class.name }}
 *  @brief    get the text classes in a connector
 *
 *  Set UI strings.
 *
 *  @param[in]     obj                 {{ class.baseName }} object
 *  @return        text_classes        zero terminated list of classes, 
 *                                     e.g. {"name",NULL}
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/06/10
 *  @since    2013/06/10 (Oyranos: 0.9.5)
 */
OYAPI const char **  OYEXPORT
                 oy{{ class.baseName }}_GetTexts( {{ class.name }}     * obj )
{
  {{ class.name }}_ * s = ({{ class.name }}_*) obj;

  if(!obj)
    return 0;

  oyCheck{{ class.baseName }}Type__m( oyOBJECT_CONNECTOR_S, return 0 )

  return s->texts;
}

/** Function  oy{{ class.baseName }}_GetText
 *  @memberof {{ class.name }}
 *  @brief    get the names in a connector
 *
 *  Get UI strings.
 *
 *  @param[in]     obj                 {{ class.baseName }} object
 *  @param[in]     name_class          the names class, e.g. "name", "help" ...
 *  @param[in]     type                the names type
 *  @return                            the name string
 *
 *  @version  Oyranos: 0.9.5
 *  @since    2011/01/31 (Oyranos: 0.3.0)
 *  @date     2013/06/10
 */
OYAPI const char *  OYEXPORT
                 oy{{ class.baseName }}_GetText ( {{ class.name }}     * obj,
                                       const char        * name_class,
                                       oyNAME_e            type )
{
  {{ class.name }}_ * s = ({{ class.name }}_*) obj;
  const char * string = 0;

  if(!obj)
    return 0;

  oyCheck{{ class.baseName }}Type__m( oyOBJECT_CONNECTOR_S, return 0 )

  string = s->getText( name_class, type, (oyStruct_s*)s );

  return string;
}

/** Function  oy{{ class.baseName }}_IsPlug
 *  @memberof {{ class.name }}
 *  @brief    Is this connector a plug or a socket
 *
 *  @param[in]     obj                 {{ class.baseName }} object
 *  @return                            boolean; 0 - socket; 1 - plug
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oy{{ class.baseName }}_IsPlug  ( {{ class.name }}     * obj )
{
  {{ class.name }}_ * s = ({{ class.name }}_*)obj;

  if(!obj)
    return 0;

  oyCheck{{ class.baseName }}Type__m( oyOBJECT_CONNECTOR_S, return 0 )

  return s->is_plug;
}

/** Function  oy{{ class.baseName }}_SetIsPlug
 *  @memberof {{ class.name }}
 *  @brief    Set this connector as a plug or a socket
 *
 *  @param[in,out] obj                 {{ class.baseName }} object
 *  @param[in]     is_plug             boolean; 0 - socket; 1 - plug
 *  @return                            1 - error; 0 - success; -1 - otherwise
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oy{{ class.baseName }}_SetIsPlug({{ class.name }}     * obj,
                                       int                 is_plug )
{
  {{ class.name }}_ * s = ({{ class.name }}_*)obj;

  if(!obj)
    return 0;

  oyCheck{{ class.baseName }}Type__m( oyOBJECT_CONNECTOR_S, return 1 )

  s->is_plug = is_plug;

  return 0;
}

/** Function  oy{{ class.baseName }}_GetReg
 *  @memberof {{ class.name }}
 *  @brief    Get the registration for the connection type
 *
 *  This is use as a rough check, if connections are possible.
 *
 *  @param[in]     obj                 {{ class.baseName }} object
 *  @return                            registration string
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
const char *     oy{{ class.baseName }}_GetReg  ( {{ class.name }}     * obj )
{
  {{ class.name }}_ * s = ({{ class.name }}_*)obj;

  if(!obj)
    return 0;

  oyCheck{{ class.baseName }}Type__m( oyOBJECT_CONNECTOR_S, return 0 )

  return s->connector_type;
}

/** Function  oy{{ class.baseName }}_SetReg
 *  @memberof {{ class.name }}
 *  @brief    Set this connectors type string
 *
 *  This is use as a rough check, if connections are possible.
 *
 *  @param[in,out] obj                 {{ class.baseName }} object
 *  @param[in]     type_registration   the registration string to describe the
 *                                     type
 *  @return                            1 - error; 0 - success; -1 - otherwise
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oy{{ class.baseName }}_SetReg  ( {{ class.name }}     * obj,
                                       const char        * type_registration )
{
  {{ class.name }}_ * s = ({{ class.name }}_*)obj;

  if(!obj)
    return 0;

  oyCheck{{ class.baseName }}Type__m( oyOBJECT_CONNECTOR_S, return 1 )

  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    oyAlloc_f allocateFunc = s->oy_->allocateFunc_;

    if(s->connector_type)
    { deallocateFunc( s->connector_type ); s->connector_type = 0; }

    s->connector_type = oyStringCopy_( type_registration, allocateFunc );
  }

  return 0;
}

/** Function  oy{{ class.baseName }}_SetMatch
 *  @memberof {{ class.name }}
 *  @brief    Set this connectors type check function
 *
 *  This is use as a check, if connections are possible.
 *  This allowes for a more fine grained control than the type registration.
 *
 *  @param[in,out] obj                 {{ class.baseName }} object
 *  @param[in]     func                the check function
 *  @return                            1 - error; 0 - success; -1 - otherwise
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oy{{ class.baseName }}_SetMatch( {{ class.name }}     * obj,
                                       oyCMMFilterSocket_MatchPlug_f func )
{
  {{ class.name }}_ * s = ({{ class.name }}_*)obj;

  if(!obj)
    return 0;

  oyCheck{{ class.baseName }}Type__m( oyOBJECT_CONNECTOR_S, return 1 )

  s->filterSocket_MatchPlug = func;

  return 0;
}

/** Function  oy{{ class.baseName }}_GetMatch
 *  @memberof {{ class.name }}
 *  @brief    Set this connectors type check function
 *
 *  This is use as a check, if connections are possible.
 *  This allowes for a more fine grained control than the type registration.
 *
 *  @param[in]     obj                 {{ class.baseName }} object
 *  @return                            the check function
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
oyCMMFilterSocket_MatchPlug_f oy{{ class.baseName }}_GetMatch (
                                       {{ class.name }}     * obj )
{
  {{ class.name }}_ * s = ({{ class.name }}_*)obj;

  if(!obj)
    return 0;

  oyCheck{{ class.baseName }}Type__m( oyOBJECT_CONNECTOR_S, return 0 )

  return s->filterSocket_MatchPlug;
}
{% endblock %}
