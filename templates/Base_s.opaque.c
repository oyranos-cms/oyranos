{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#include "{{ class.name }}.h"
#include "{{ class.name }}_impl.h"

{% block GeneralPublicMethodsDefinitions %}
/** Function oy{{ class.baseName }}_New
 *  @memberof {{ class.name }}
 *  @brief   allocate a new {{ class.baseName }} object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
OYAPI {{ class.name }} OYEXPORT
  oy{{ class.baseName }}_New( oyObject_s object )
{
  oyObject_s s = object;
  {{ class.name }} {{ class.baseName|lower }} = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 );

  {{ class.baseName|lower }} = oy{{ class.baseName }}_New_( s );

  return {{ class.baseName|lower }};
}

/** Function oy{{ class.baseName }}_Copy
 *  @memberof {{ class.name }}
 *  @brief   copy or reference a {{ class.baseName }} object
 *
 *  @param[in]     {{ class.baseName|lower }} {{ class.baseName }} struct object
 *  @param         object                     the optional object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
OYAPI {{ class.name }} OYEXPORT
  oy{{ class.baseName }}_Copy( {{ class.name }} {{ class.baseName|lower }}, oyObject_s object )
{
  {{ class.name }} s = {{ class.baseName|lower }};

  if(s)
    oyCheckType__m( {{ class.enumName  }}, return 0 );

  s = oy{{ class.baseName }}_Copy_( s, object );

  return s;
}

/** Function oy{{ class.baseName }}_Release
 *  @memberof {{ class.name }}
 *  @brief   release and possibly deallocate a {{ class.name }} object
 *
 *  @param[in,out] {{ class.baseName|lower }} {{ class.baseName }} struct object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
OYAPI int OYEXPORT
  oy{{ class.baseName }}_Release( {{ class.name }} *{{ class.baseName|lower }} )
{
  {{ class.name }} s = 0;

  if(!{{ class.baseName|lower }} || !*{{ class.baseName|lower }})
    return 0;

  s = *{{ class.baseName|lower }};

  oyCheckType__m( oyOBJECT_{{ class.baseName|underscores|upper }}_S, return 1 )

  *{{ class.baseName|lower }} = 0;

  return oy{{ class.baseName }}_Release_( &s );
}
{% endblock GeneralPublicMethodsDefinitions %}

{% block SpecificPublicMethodsDefinitions %}
/* Include "{{ class.public_methods_definitions_c }}" { */
{% include class.public_methods_definitions_c %}
/* } Include "{{ class.public_methods_definitions_c }}" */
{% endblock %}
{# TODO Remove uneeded pointer in oyClass_s* #}
