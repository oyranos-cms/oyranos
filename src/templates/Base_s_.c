{% include "source_file_header.txt" %}
{% load gsoc_extensions %}

{% block GlobalIncludeFiles %}{% endblock %}  
#include "{{ class.name }}.h"
#include "{{ class.privName }}.h"

{% ifequal class.parent.parent.name "oyStruct_s" %}
#include "{{ class.parent.privName }}.h"
{% endifequal %}
{% ifequal class.parent.parent.parent.name "oyStruct_s" %}
#include "{{ class.parent.parent.privName }}.h"
#include "{{ class.parent.privName }}.h"
{% endifequal %}
{% ifequal class.parent.parent.parent.parent.name "oyStruct_s" %}
#include "{{ class.parent.parent.parent.privName }}.h"
#include "{{ class.parent.parent.privName }}.h"
#include "{{ class.parent.privName }}.h"
{% endifequal %}

#include "oyObject_s.h"
#include "oyranos_object_internal.h"
{% ifequal class.group "module_api" %}#include "oyranos_module_internal.h"{% endifequal %}
{% ifequal class.group "objects_generic" %}#include "oyranos_generic_internal.h"{% endifequal %}
{% block LocalIncludeFiles %}{% endblock %}  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif

{% block GeneralPrivateMethodsDefinitions %}
static int oy_{{ class.baseName|lower }}_init_ = 0;
static const char * oy{{ class.baseName }}_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  {{ class.privName }} * s = ({{ class.privName }}*) obj;
  static char * text = 0;
  static int text_n = 0;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if(s->oy_ && s->oy_->allocateFunc_)
    alloc = s->oy_->allocateFunc_;

  if( text == NULL || text_n == 0 )
  {
    text_n = 512;
    text = (char*) alloc( text_n );
    if(text)
      memset( text, 0, text_n );
  }

  if( text == NULL || text_n == 0 )
    return "Memory problem";

  text[0] = '\000';

  if(!(flags & 0x01))
    sprintf(text, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  {% block customStaticMessage %}
  {% endblock customStaticMessage %}

  return text;
}

{% block CustomPrivateMethodsDefinitions %}
/* Include "{{ class.private_custom_definitions_c }}" { */
{% include class.private_custom_definitions_c %}
/* } Include "{{ class.private_custom_definitions_c }}" */
{% endblock CustomPrivateMethodsDefinitions %}

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
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  {{ class.privName }} * s = 0;

  if(s_obj)
    s = ({{ class.privName }}*)s_obj->allocateFunc_(sizeof({{ class.privName }}));
  else
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  if(!s)
  {
    if(s_obj)
      oyObject_Release( &s_obj );
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof({{ class.privName }}) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oy{{ class.baseName }}_Copy;
  s->release = (oyStruct_Release_f) oy{{ class.baseName }}_Release;

  s->oy_ = s_obj;

  {% ifequal class.parent.name "oyStruct_s" %}
  /* ---- start of custom {{ class.baseName }} constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_{{ class.baseName|underscores|upper }}_S, (oyPointer)s );
  /* ---- end of custom {{ class.baseName }} constructor ------- */
  {% endifequal %}
  {% ifequal class.parent.parent.name "oyStruct_s" %}
  /* ---- start of custom {{ class.parent.baseName }} constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_{{ class.parent.baseName|underscores|upper }}_S, (oyPointer)s );
  /* ---- end of custom {{ class.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.baseName }} constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_{{ class.baseName|underscores|upper }}_S, (oyPointer)s );
  /* ---- end of custom {{ class.baseName }} constructor ------- */
  {% endifequal %}
  {% ifequal class.parent.parent.parent.name "oyStruct_s" %}
  /* ---- start of custom {{ class.parent.parent.baseName }} constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_{{ class.parent.parent.baseName|underscores|upper }}_S, (oyPointer)s );
  /* ---- end of custom {{ class.parent.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.parent.baseName }} constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_{{ class.parent.baseName|underscores|upper }}_S, (oyPointer)s );
  /* ---- end of custom {{ class.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.baseName }} constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_{{ class.baseName|underscores|upper }}_S, (oyPointer)s );
  /* ---- end of custom {{ class.baseName }} constructor ------- */
  {% endifequal %}
  {% ifequal class.parent.parent.parent.parent.name "oyStruct_s" %}
  /* ---- start of custom {{ class.parent.parent.parent.baseName }} constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_{{ class.parent.parent.parent.baseName|underscores|upper }}_S, (oyPointer)s );
  /* ---- end of custom {{ class.parent.parent.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.parent.parent.baseName }} constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_{{ class.parent.parent.baseName|underscores|upper }}_S, (oyPointer)s );
  /* ---- end of custom {{ class.parent.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.parent.baseName }} constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_{{ class.parent.baseName|underscores|upper }}_S, (oyPointer)s );
  /* ---- end of custom {{ class.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.baseName }} constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_{{ class.baseName|underscores|upper }}_S, (oyPointer)s );
  /* ---- end of custom {{ class.baseName }} constructor ------- */
  {% endifequal %}
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  {% block customConstructor %}
  {% endblock customConstructor %}

  {% ifequal class.parent.name "oyStruct_s" %}
  /* ---- start of custom {{ class.baseName }} constructor ----- */
  error += oy{{ class.baseName }}_Init__Members( s );
  /* ---- end of custom {{ class.baseName }} constructor ------- */
  {% endifequal %}
  {% ifequal class.parent.parent.name "oyStruct_s" %}
  /* ---- start of custom {{ class.parent.baseName }} constructor ----- */
  error += oy{{ class.parent.baseName }}_Init__Members( ({{ class.parent.privName }}*)s );
  /* ---- end of custom {{ class.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.baseName }} constructor ----- */
  error += oy{{ class.baseName }}_Init__Members( s );
  /* ---- end of custom {{ class.baseName }} constructor ------- */
  {% endifequal %}
  {% ifequal class.parent.parent.parent.name "oyStruct_s" %}
  /* ---- start of custom {{ class.parent.parent.baseName }} constructor ----- */
  error += oy{{ class.parent.parent.baseName }}_Init__Members( ({{ class.parent.parent.privName }}*)s );
  /* ---- end of custom {{ class.parent.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.parent.baseName }} constructor ----- */
  error += oy{{ class.parent.baseName }}_Init__Members( ({{ class.parent.privName }}*)s );
  /* ---- end of custom {{ class.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.baseName }} constructor ----- */
  error += oy{{ class.baseName }}_Init__Members( s );
  /* ---- end of custom {{ class.baseName }} constructor ------- */
  {% endifequal %}
  {% ifequal class.parent.parent.parent.parent.name "oyStruct_s" %}
  /* ---- start of custom {{ class.parent.parent.parent.baseName }} constructor ----- */
  error += oy{{ class.parent.parent.parent.baseName }}_Init__Members( ({{ class.parent.parent.parent.privName }}*)s );
  /* ---- end of custom {{ class.parent.parent.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.parent.parent.baseName }} constructor ----- */
  error += oy{{ class.parent.parent.baseName }}_Init__Members( ({{ class.parent.parent.privName }}*)s );
  /* ---- end of custom {{ class.parent.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.parent.baseName }} constructor ----- */
  error += oy{{ class.parent.baseName }}_Init__Members( ({{ class.parent.privName }}*)s );
  /* ---- end of custom {{ class.parent.baseName }} constructor ------- */
  /* ---- start of custom {{ class.baseName }} constructor ----- */
  error += oy{{ class.baseName }}_Init__Members( s );
  /* ---- end of custom {{ class.baseName }} constructor ------- */
  {% endifequal %}

  if(!oy_{{ class.baseName|lower }}_init_)
  {
    oy_{{ class.baseName|lower }}_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oy{{ class.baseName }}_StaticMessageFunc_,
                                        &oy_{{ class.baseName|lower }}_init_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

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

  s = {% block altConstructor %}({{ class.privName }}*) oy{{ class.baseName }}_New( object );{% endblock %}
  error = !s;

  if(!error) {
    {% ifequal class.parent.name "oyStruct_s" %}
    /* ---- start of custom {{ class.baseName }} copy constructor ----- */
    error = oy{{ class.baseName }}_Copy__Members( s, {{ class.baseName|lower }} );
    /* ---- end of custom {{ class.baseName }} copy constructor ------- */
    {% endifequal %}
    {% ifequal class.parent.parent.name "oyStruct_s" %}
    /* ---- start of custom {{ class.parent.baseName }} copy constructor ----- */
    error = oy{{ class.parent.baseName }}_Copy__Members( ({{ class.parent.privName }}*)s, ({{ class.parent.privName }}*){{ class.baseName|lower }} );
    /* ---- end of custom {{ class.parent.baseName }} copy constructor ------- */
    /* ---- start of custom {{ class.baseName }} copy constructor ----- */
    error = oy{{ class.baseName }}_Copy__Members( s, {{ class.baseName|lower }} );
    /* ---- end of custom {{ class.baseName }} copy constructor ------- */
    {% endifequal %}
    {% ifequal class.parent.parent.parent.name "oyStruct_s" %}
    /* ---- start of custom {{ class.parent.parent.baseName }} copy constructor ----- */
    error = oy{{ class.parent.parent.baseName }}_Copy__Members( ({{ class.parent.parent.privName }}*)s, ({{ class.parent.parent.privName }}*){{ class.baseName|lower }} );
    /* ---- end of custom {{ class.parent.parent.baseName }} copy constructor ------- */
    /* ---- start of custom {{ class.parent.baseName }} copy constructor ----- */
    error = oy{{ class.parent.baseName }}_Copy__Members( ({{ class.parent.privName }}*)s, ({{ class.parent.privName }}*){{ class.baseName|lower }} );
    /* ---- end of custom {{ class.parent.baseName }} copy constructor ------- */
    /* ---- start of custom {{ class.baseName }} copy constructor ----- */
    error = oy{{ class.baseName }}_Copy__Members( s, {{ class.baseName|lower }} );
    /* ---- end of custom {{ class.baseName }} copy constructor ------- */
    {% endifequal %}
    {% ifequal class.parent.parent.parent.parent.name "oyStruct_s" %}
    /* ---- start of custom {{ class.parent.parent.parent.baseName }} copy constructor ----- */
    error = oy{{ class.parent.parent.parent.baseName }}_Copy__Members( ({{ class.parent.parent.parent.privName }}*)s, ({{ class.parent.parent.parent.privName }}*){{ class.baseName|lower }} );
    /* ---- end of custom {{ class.parent.parent.parent.baseName }} copy constructor ------- */
    /* ---- start of custom {{ class.parent.parent.baseName }} copy constructor ----- */
    error = oy{{ class.parent.parent.baseName }}_Copy__Members( ({{ class.parent.parent.privName }}*)s, ({{ class.parent.parent.privName }}*){{ class.baseName|lower }} );
    /* ---- end of custom {{ class.parent.parent.baseName }} copy constructor ------- */
    /* ---- start of custom {{ class.parent.baseName }} copy constructor ----- */
    error = oy{{ class.parent.baseName }}_Copy__Members( ({{ class.parent.privName }}*)s, ({{ class.parent.privName }}*){{ class.baseName|lower }} );
    /* ---- end of custom {{ class.parent.baseName }} copy constructor ------- */
    /* ---- start of custom {{ class.baseName }} copy constructor ----- */
    error = oy{{ class.baseName }}_Copy__Members( s, {{ class.baseName|lower }} );
    /* ---- end of custom {{ class.baseName }} copy constructor ------- */
    {% endifequal %}
    {% block copyConstructor %}
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
    {% block oyClass_Copy_notObject %}{% endblock %}
    if(oy_debug_objects >= 0 && s->oy_)
    {
      const char * t = getenv(OY_DEBUG_OBJECTS);
      int id_ = -1;

      if(t)
        id_ = atoi(t);
      else
        id_ = oy_debug_objects;

      if((id_ >= 0 && s->oy_->id_ == id_) ||
         (t && s && (strstr(oyStructTypeToText(s->type_), t) != 0)) ||
         id_ == 1)
      {
        oyStruct_s ** parents = NULL;
        int n = oyStruct_GetParents( (oyStruct_s*)s, &parents );
        {
          int i;
          const char * track_name = oyStructTypeToText(s->type_);
          OY_BACKTRACE_PRINT
          fprintf( stderr, "%s[%d] tracking refs: %d parents: %d\n",
                   track_name, s->oy_->id_, s->oy_->ref_, n );
          for(i = 0; i < n; ++i)
          {
            track_name = oyStructTypeToText(parents[i]->type_);
            fprintf( stderr, "parent[%d]: %s[%d]\n", i,
                     track_name, parents[i]->oy_->id_ );
          }
        }
      }
    }
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
 *  @version Oyranos: 0.9.7
 *  @date    2018/10/03
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oy{{ class.baseName }}_Release_( {{ class.privName }} **{{ class.baseName|lower }} )
{
  const char * track_name = NULL;
  int observer_refs = 0, i;
  /* ---- start of common object destructor ----- */
  {{ class.privName }} *s = 0;

  if(!{{ class.baseName|lower }} || !*{{ class.baseName|lower }})
    return 0;

  s = *{{ class.baseName|lower }};
  /* static object */
  if(!s->oy_)
    return 0;

  *{{ class.baseName|lower }} = 0;

  observer_refs = oyStruct_ObservationCount( (oyStruct_s*)s, 0 );

  if(oy_debug_objects >= 0)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;

    if(t)
      id_ = atoi(t);
    else
      id_ = oy_debug_objects;

    if((id_ >= 0 && s->oy_->id_ == id_) ||
       (t && (strstr(oyStructTypeToText(s->type_), t) != 0)) ||
       id_ == 1)
    {
      oyStruct_s ** parents = NULL;
      int n = oyStruct_GetParents( (oyStruct_s*)s, &parents );
      {
        int i;
        OY_BACKTRACE_PRINT
        track_name = oyStructTypeToText(s->type_);
        fprintf( stderr, "%s[%d] unref with refs: %d observers: %d parents: %d\n",
                 track_name, s->oy_->id_, s->oy_->ref_, observer_refs, n );
        for(i = 0; i < n; ++i)
        {
          track_name = oyStructTypeToText(parents[i]->type_);
          fprintf( stderr, "parent[%d]: %s[%d]\n", i,
                   track_name, parents[i]->oy_->id_ );
        }
      }
    }
  }

  {% block refCount %}
  if((oyObject_UnRef(s->oy_) - observer_refs) > 0)
    return 0;{% endblock %}
  /* ---- end of common object destructor ------- */

  if(oy_debug_objects >= 0)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;

    if(t)
      id_ = atoi(t);
    else
      id_ = oy_debug_objects;

    if((id_ >= 0 && s->oy_->id_ == id_) ||
       (t && s && (strstr(oyStructTypeToText(s->type_), t) != 0)) ||
       id_ == 1)
    {
      track_name = oyStructTypeToText(s->type_);
      fprintf( stderr, "%s[%d] destruct\n", track_name, s->oy_->id_);
    }
  }

  {% ifequal class.parent.name "oyStruct_s" %}
  /* ---- start of custom {{ class.baseName }} destructor ----- */
  oy{{ class.baseName }}_Release__Members( s );
  /* ---- end of custom {{ class.baseName }} destructor ------- */
  {% endifequal %}
  {% ifequal class.parent.parent.name "oyStruct_s" %}
  /* ---- start of custom {{ class.parent.baseName }} destructor ----- */
  oy{{ class.parent.baseName }}_Release__Members( ({{ class.parent.privName }}*)s );
  /* ---- end of custom {{ class.parent.baseName }} destructor ------- */
  /* ---- start of custom {{ class.baseName }} destructor ----- */
  oy{{ class.baseName }}_Release__Members( s );
  /* ---- end of custom {{ class.baseName }} destructor ------- */
  {% endifequal %}
  {% ifequal class.parent.parent.parent.name "oyStruct_s" %}
  /* ---- start of custom {{ class.parent.parent.baseName }} destructor ----- */
  oy{{ class.parent.parent.baseName }}_Release__Members( ({{ class.parent.parent.privName }}*)s );
  /* ---- end of custom {{ class.parent.parent.baseName }} destructor ------- */
  /* ---- start of custom {{ class.parent.baseName }} destructor ----- */
  oy{{ class.parent.baseName }}_Release__Members( ({{ class.parent.privName }}*)s );
  /* ---- end of custom {{ class.parent.baseName }} destructor ------- */
  /* ---- start of custom {{ class.baseName }} destructor ----- */
  oy{{ class.baseName }}_Release__Members( s );
  /* ---- end of custom {{ class.baseName }} destructor ------- */
  {% endifequal %}
  {% ifequal class.parent.parent.parent.parent.name "oyStruct_s" %}
  /* ---- start of custom {{ class.parent.parent.parent.baseName }} destructor ----- */
  oy{{ class.parent.parent.parent.baseName }}_Release__Members( ({{ class.parent.parent.parent.privName }}*)s );
  /* ---- end of custom {{ class.parent.parent.parent.baseName }} destructor ------- */
  /* ---- start of custom {{ class.parent.parent.baseName }} destructor ----- */
  oy{{ class.parent.parent.baseName }}_Release__Members( ({{ class.parent.parent.privName }}*)s );
  /* ---- end of custom {{ class.parent.parent.baseName }} destructor ------- */
  /* ---- start of custom {{ class.parent.baseName }} destructor ----- */
  oy{{ class.parent.baseName }}_Release__Members( ({{ class.parent.privName }}*)s );
  /* ---- end of custom {{ class.parent.baseName }} destructor ------- */
  /* ---- start of custom {{ class.baseName }} destructor ----- */
  oy{{ class.baseName }}_Release__Members( s );
  /* ---- end of custom {{ class.baseName }} destructor ------- */
  {% endifequal %}
{% block customDestructor %}
{% endblock customDestructor %}

  /* model and observer reference each other. So release the object two times.
   * The models and and observers are released later inside the
   * oyObject_s::handles. */
  for(i = 0; i < observer_refs; ++i)
  {
    oyObject_UnRef(s->oy_);
    oyObject_UnRef(s->oy_);
  }

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    int id = s->oy_->id_;
    int refs = s->oy_->ref_;

    if(refs > 1)
      fprintf( stderr, "!!!ERROR: node[%d]->object can not be untracked with refs: %d\n", id, refs);

    oyObject_Release( &s->oy_ );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", track_name, id );

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
