{% extends "Struct_s.template.c" %}

{% block IncludePrivateHeader %}
#include "{{ class.name }}_.h"
{% endblock %}

{% block GeneralPublicMethodsDefinitions %}
/** Function oy{{ class.baseName }}_New
 *  @memberof {{ class.name }}
 *  @brief   allocate a new {{ class.baseName }} object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
OYAPI {{ class.name }} * OYEXPORT
  oy{{ class.baseName }}_New( oyObject_s object )
{
  oyObject_s_ s = (oyObject_s_) object;
  {{ class.privName }} * obj = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 );

  obj = oy{{ class.baseName }}_New_( s );

  return ({{ class.name }}*) obj;
}

/** Function oy{{ class.baseName }}_Copy
 *  @memberof {{ class.name }}
 *  @brief   copy or reference a {{ class.baseName }} object
 *
 *  @param[in]     obj                 {{ class.baseName }} struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
OYAPI {{ class.name }}* OYEXPORT
  oy{{ class.baseName }}_Copy( {{ class.name }} *obj, oyObject_s object )
{
  {{ class.privName }} * s = ({{ class.privName }}*) obj;

  if(s)
    oyCheckType__m( oyOBJECT_{{ class.baseName|upper  }}_S, return 0 );

  s = oy{{ class.baseName }}_Copy_( s, (oyObject_s_*) object );

  return ({{ class.name }}*) s;
}
 
/** Function oy{{ class.baseName }}_Release
 *  @memberof {{ class.name }}
 *  @brief   release and possibly deallocate a {{ class.name }} object
 *
 *  @param[in,out] obj                 {{ class.baseName }} struct object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
OYAPI int OYEXPORT
  oy{{ class.baseName }}_Release( {{ class.name }} **obj )
{
  {{ class.privName }} * s = 0;

  if(!obj || !*obj)
    return 0;

  s = ({{ class.name }}*) *obj;

  oyCheckType__m( oyOBJECT_{{ class.baseName|upper }}_S, return 1 )

  *obj = 0;

  return oy{{ class.baseName }}_Release_( &s );
}
{% endblock GeneralPublicMethodsDefinitions %}
