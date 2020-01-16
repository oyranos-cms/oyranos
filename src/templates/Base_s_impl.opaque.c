{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#include "{{ class.name }}.h"
#include "{{ class.name }}_impl.h"

{% block GeneralPrivateMethodsDefinitions %}
/** @internal
 *  Function oy{{ class.baseName }}_New_
 *  @memberof {{ class.privName }}
 *  @brief   allocate a new {{ class.privName }}  object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
{{ class.name }} oy{{ class.baseName }}_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_{{ class.baseName|underscores|upper }}_S;
  int error = 0;
  oyObject_s s_obj = oyObject_NewFrom( object, "oy{{ class.baseName }}_s" );
  {{ struct.name }} s = 0;

  if(s_obj)
    s = ({{ struct.name }})s_obj->allocateFunc_(sizeof(struct {{ struct.name }}));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(struct {{ struct.name }}) );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oy{{ class.baseName }}_Copy;
  s->release = (oyStruct_Release_f) oy{{ class.baseName }}_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  /* ---- start of custom {{ class.baseName }} constructor ----- */
  error = !oy{{ class.baseName }}_New_Custom( s );
  /* ---- end of custom {{ class.baseName }} constructor ------- */

  return s;
}

/** @internal
 *  Function oy{{ class.baseName }}_Copy__
 *  @memberof {{ class.privName }}
 *  @brief   real copy a {{ class.baseName }} object
 *
 *  @param[in]     {{ class.baseName|lower }} {{ class.baseName }} struct object
 *  @param         object                     the optional object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
{{ class.name }} oy{{ class.baseName }}_Copy__ ( {{ class.name }} {{ class.baseName|lower }}, oyObject_s object )
{
  {{ class.name }} s = 0;
  int error = 0;

  if(!{{ class.baseName|lower }} || !object)
    return s;

  s = oy{{ class.baseName }}_New_( object );
  error = !s;

  /* ---- start of custom {{ class.baseName }} copy constructor ----- */
  oy{{ class.baseName }}_Copy__Custom( s, {{ class.baseName|lower }}, object);
  /* ---- end of custom {{ class.baseName }} copy constructor ------- */

  if(error)
    oy{{ class.baseName }}_Release_( &s );

  return s;
}

/** @internal
 *  Function oy{{ class.baseName }}_Copy_
 *  @memberof {{ class.privName }}
 *  @brief   copy or reference a {{ class.baseName }} object
 *
 *  @param[in]     {{ class.baseName|lower }} {{ class.baseName }} struct object
 *  @param         object                     the optional object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
{{ class.name }} oy{{ class.baseName }}_Copy_ ( {{ class.name }} {{ class.baseName|lower }}, oyObject_s object )
{
  {{ class.name }} s = {{ class.baseName|lower }};

  if(!{{ class.baseName|lower }})
    return 0;

  if(s && !object)
  {
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oy{{ class.baseName }}_Copy__( {{ class.baseName|lower }}, object );

  return s;
}
 
/** @internal
 *  Function oy{{ class.baseName }}_Release_
 *  @memberof {{ class.privName }}
 *  @brief   release and possibly deallocate a {{ class.baseName }} object
 *
 *  @param[in,out] {{ class.baseName|lower }} {{ class.baseName }} struct object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oy{{ class.baseName }}_Release_( {{ class.name }} *{{ class.baseName|lower }} )
{
  /* ---- start of common object destructor ----- */
  {{ class.name }} *s = 0;

  if(!{{ class.baseName|lower }} || !*{{ class.baseName|lower }})
    return 0;

  s = *{{ class.baseName|lower }};

  *{{ class.baseName|lower }} = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}
{% endblock GeneralPrivateMethodsDefinitions %}

{% block SpecificPrivateMethodsDefinitions %}
/* Include "{{ class.private_methods_definitions_c }}" { */
{% include class.private_methods_definitions_c %}
/* } Include "{{ class.private_methods_definitions_c }}" */
{# TODO Remove uneeded pointer in oyClass_s* #}
{% endblock SpecificPrivateMethodsDefinitions %}
