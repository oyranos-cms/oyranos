{% extends "Struct_s.template.c" %}

{% block IncludePrivateHeader %}
#include "{{ class_name }}_.h"
{% endblock %}

{% block GeneralPublicMethodsDefinitions %}
/** Function oy{{ class_base_name }}_New
 *  @memberof {{ class_name }}
 *  @brief   allocate a new {{ class_base_name }} object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
OYAPI {{ class_name }} * OYEXPORT
  oy{{ class_base_name }}_New( oyObject_s object )
{
  oyObject_s_ s = (oyObject_s_) object;
  {{ class_name }}_ * obj = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 );

  obj = oy{{ class_base_name }}_New_( s );

  return ({{ class_name }}*) obj;
}

/** Function oy{{ class_base_name }}_Copy
 *  @memberof {{ class_name }}
 *  @brief   copy or reference a {{ class_base_name }} object
 *
 *  @param[in]     obj                 {{ class_base_name }} struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
OYAPI {{ class_name }}* OYEXPORT
  oy{{ class_base_name }}_Copy( {{ class_name }} *obj, oyObject_s object )
{
  {{ class_name }}_ * s = ({{ class_name }}_*) obj;

  if(s)
    oyCheckType__m( oyOBJECT_{{ class_base_name|upper  }}_S, return 0 );

  s = oy{{ class_base_name }}_Copy_( s, (oyObject_s_*) object );

  return ({{ class_name }}*) s;
}
 
/** Function oy{{ class_base_name }}_Release
 *  @memberof {{ class_name }}
 *  @brief   release and possibly deallocate a {{ class_name }} object
 *
 *  @param[in,out] obj                 {{ class_base_name }} struct object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
OYAPI int OYEXPORT
  oy{{ class_base_name }}_Release( {{ class_name }} **obj )
{
  {{ class_name }}_ * s = 0;

  if(!obj || !*obj)
    return 0;

  s = ({{ class_name }}*) *obj;

  oyCheckType__m( oyOBJECT_{{ class_base_name|upper }}_S, return 1 )

  *obj = 0;

  return oy{{ class_base_name }}_Release_( &s );
}
{% endblock GeneralPublicMethodsDefinitions %}

{% block SpecificPublicMethodsDefinitions %}{% endblock %}
