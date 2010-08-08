{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

{% block GlobalIncludeFiles %}{% endblock %}  
#include "{{ class.name }}.h"
#include "{{ class.privName }}.h"

#include "oyObject_s.h"
#include "oyranos_object_internal.h"
{% block LocalIncludeFiles %}{% endblock %}  

{% block CustomPrivateMethodsDefinitions %}
/* Include "{{ class.private_custom_definitions_c }}" { */
{% include class.private_custom_definitions_c %}
/* } Include "{{ class.private_custom_definitions_c }}" */
{% endblock CustomPrivateMethodsDefinitions %}

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
{{ class.privName }} * oy{{ class.baseName }}_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_{{ class.baseName|underscores|upper }}_S;
# define STRUCT_TYPE {{ class.privName }} // FIXME Why is this needed?
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;

  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oy{{ class.baseName }}_Copy;
  s->release = (oyStruct_Release_f) oy{{ class.baseName }}_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  {% block customConstructor %}
  {% ifequal class.parent.name "oyStruct_s" %}
  /* ---- start of custom {{ class.baseName }} constructor ----- */
  error = oy{{ class.baseName }}_Init__Members( s );
  /* ---- end of custom {{ class.baseName }} constructor ------- */
  {% endifequal %}
  {% ifequal class.parent.parent.name "oyStruct_s" %}
  /* ---- start of custom {{ class.parent.baseName }} constructor ----- */
  error = oy{{ class.parent.baseName }}_Init__Members( s );
  /* ---- end of custom {{ class.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.baseName }} constructor ----- */
  error = oy{{ class.baseName }}_Init__Members( s );
  /* ---- end of custom {{ class.baseName }} constructor ------- */
  {% endifequal %}
  {% ifequal class.parent.parent.parent.name "oyStruct_s" %}
  /* ---- start of custom {{ class.parent.parent.baseName }} constructor ----- */
  error = oy{{ class.parent.parent.baseName }}_Init__Members( s );
  /* ---- end of custom {{ class.parent.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.parent.baseName }} constructor ----- */
  error = oy{{ class.parent.baseName }}_Init__Members( s );
  /* ---- end of custom {{ class.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.baseName }} constructor ----- */
  error = oy{{ class.baseName }}_Init__Members( s );
  /* ---- end of custom {{ class.baseName }} constructor ------- */
  {% endifequal %}
  {% ifequal class.parent.parent.parent.parent.name "oyStruct_s" %}
  /* ---- start of custom {{ class.parent.parent.parent.baseName }} constructor ----- */
  error = oy{{ class.parent.parent.parent.baseName }}_Init__Members( s );
  /* ---- end of custom {{ class.parent.parent.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.parent.parent.baseName }} constructor ----- */
  error = oy{{ class.parent.parent.baseName }}_Init__Members( s );
  /* ---- end of custom {{ class.parent.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.parent.baseName }} constructor ----- */
  error = oy{{ class.parent.baseName }}_Init__Members( s );
  /* ---- end of custom {{ class.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.baseName }} constructor ----- */
  error = oy{{ class.baseName }}_Init__Members( s );
  /* ---- end of custom {{ class.baseName }} constructor ------- */
  {% endifequal %}
  {% endblock customConstructor %}

  return s;
}

/** @internal
 *  Function oy{{ class.baseName }}_Copy__
 *  @memberof {{ class.privName }}
 *  @brief   real copy a {{ class.baseName }} object
 *
 *  @param[in]     {{ class.baseName|lower }}                 {{ class.baseName }} struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
{{ class.privName }} * oy{{ class.baseName }}_Copy__ ( {{ class.privName }} *{{ class.baseName|lower }}, oyObject_s object )
{
  {{ class.privName }} *s = 0;
  int error = 0;

  if(!{{ class.baseName|lower }} || !object)
    return s;

  s = oy{{ class.baseName }}_New_( object );
  error = !s;

  if(!error) {
    {% block copyConstructor %}
    {% ifequal class.parent.name "oyStruct_s" %}
    /* ---- start of custom {{ class.baseName }} copy constructor ----- */
    error = oy{{ class.baseName }}_Copy__Members( s, {{ class.baseName|lower }} );
    /* ---- end of custom {{ class.baseName }} copy constructor ------- */
    {% endifequal %}
    {% ifequal class.parent.parent.name "oyStruct_s" %}
    /* ---- start of custom {{ class.parent.baseName }} copy constructor ----- */
    error = oy{{ class.parent.baseName }}_Copy__Members( s, {{ class.baseName|lower }} );
    /* ---- end of custom {{ class.parent.baseName }} copy constructor ------- */
    /* ---- start of custom {{ class.baseName }} copy constructor ----- */
    error = oy{{ class.baseName }}_Copy__Members( s, {{ class.baseName|lower }} );
    /* ---- end of custom {{ class.baseName }} copy constructor ------- */
    {% endifequal %}
    {% ifequal class.parent.parent.parent.name "oyStruct_s" %}
    /* ---- start of custom {{ class.parent.parent.baseName }} copy constructor ----- */
    error = oy{{ class.parent.parent.baseName }}_Copy__Members( s, {{ class.baseName|lower }} );
    /* ---- end of custom {{ class.parent.parent.baseName }} copy constructor ------- */
    /* ---- start of custom {{ class.parent.baseName }} copy constructor ----- */
    error = oy{{ class.parent.baseName }}_Copy__Members( s, {{ class.baseName|lower }} );
    /* ---- end of custom {{ class.parent.baseName }} copy constructor ------- */
    /* ---- start of custom {{ class.baseName }} copy constructor ----- */
    error = oy{{ class.baseName }}_Copy__Members( s, {{ class.baseName|lower }} );
    /* ---- end of custom {{ class.baseName }} copy constructor ------- */
    {% endifequal %}
    {% ifequal class.parent.parent.parent.parent.name "oyStruct_s" %}
    /* ---- start of custom {{ class.parent.parent.parent.baseName }} copy constructor ----- */
    error = oy{{ class.parent.parent.parent.baseName }}_Copy__Members( s, {{ class.baseName|lower }} );
    /* ---- end of custom {{ class.parent.parent.parent.baseName }} copy constructor ------- */
    /* ---- start of custom {{ class.parent.parent.baseName }} copy constructor ----- */
    error = oy{{ class.parent.parent.baseName }}_Copy__Members( s, {{ class.baseName|lower }} );
    /* ---- end of custom {{ class.parent.parent.baseName }} copy constructor ------- */
    /* ---- start of custom {{ class.parent.baseName }} copy constructor ----- */
    error = oy{{ class.parent.baseName }}_Copy__Members( s, {{ class.baseName|lower }} );
    /* ---- end of custom {{ class.parent.baseName }} copy constructor ------- */
    /* ---- start of custom {{ class.baseName }} copy constructor ----- */
    error = oy{{ class.baseName }}_Copy__Members( s, {{ class.baseName|lower }} );
    /* ---- end of custom {{ class.baseName }} copy constructor ------- */
    {% endifequal %}
    {% endblock copyConstructor %}
  }

  if(error)
    oy{{ class.baseName }}_Release_( &s );

  return s;
}

/** @internal
 *  Function oy{{ class.baseName }}_Copy_
 *  @memberof {{ class.privName }}
 *  @brief   copy or reference a {{ class.baseName }} object
 *
 *  @param[in]     {{ class.baseName|lower }}                 {{ class.baseName }} struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
{{ class.privName }} * oy{{ class.baseName }}_Copy_ ( {{ class.privName }} *{{ class.baseName|lower }}, oyObject_s object )
{
  {{ class.privName }} *s = {{ class.baseName|lower }};

  if(!{{ class.baseName|lower }})
    return 0;

  if({{ class.baseName|lower }} && !object)
  {
    s = {{ class.baseName|lower }};
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oy{{ class.baseName }}_Copy__( {{ class.baseName|lower }}, object );

  return s;
}
 
/** @internal
 *  Function oy{{ class.baseName }}_Release_
 *  @memberof {{ class.privName }}
 *  @brief   release and possibly deallocate a {{ class.baseName }} {% block Container %}object{% endblock %}
 *
 *  @param[in,out] {{ class.baseName|lower }}                 {{ class.baseName }} struct object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oy{{ class.baseName }}_Release_( {{ class.privName }} **{{ class.baseName|lower }} )
{
  /* ---- start of common object destructor ----- */
  {{ class.privName }} *s = 0;

  if(!{{ class.baseName|lower }} || !*{{ class.baseName|lower }})
    return 0;

  s = *{{ class.baseName|lower }};

  *{{ class.baseName|lower }} = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

{% block customDestructor %}
  /* ---- start of custom {{ class.baseName }} destructor ----- */
  oy{{ class.baseName }}_Release__Members( s );
  /* ---- end of custom {{ class.baseName }} destructor ------- */
{% endblock customDestructor %}

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
{% endblock SpecificPrivateMethodsDefinitions %}
