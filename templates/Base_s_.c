{% block GeneralPrivateMethodsDefinitions %}
/** @internal
 *  Function oy{{ class_base_name }}_New_
 *  @memberof {{ class_name }}
 *  @brief   allocate a new {{ class_base_name }}  object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
{{ class_name }} * oy{{ class_base_name }}_New_ ( oyObject_s_ object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_{{ class_base_name }}_S;
# define STRUCT_TYPE {{ class_name }}
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
  s->copy = (oyStruct_Copy_f) oy{{ class_base_name }}_Copy;
  s->release = (oyStruct_Release_f) oy{{ class_base_name }}_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */


  return s;
}

/** @internal
 *  Function oy{{ class_base_name }}_Copy__
 *  @memberof {{ class_name }}
 *  @brief   real copy a {{ class_base_name }} object
 *
 *  @param[in]     obj                 {{ class_base_name }} struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
{{ class_name }} * oy{{ class_base_name }}_Copy__ ( {{ class_name }} *obj, oyObject_s_ object )
{
  {{ class_name }} *s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!obj || !object)
    return s;

  s = oy{{ class_base_name }}_New_( object );
  error = !s;

  if(!error)
  {
    allocateFunc_ = s->oy_->allocateFunc_;
  }

  if(error)
    oy{{ class_base_name }}_Release_( &s );

  return s;
}

/** @internal
 *  Function oy{{ class_base_name }}_Copy_
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
oyDevice_s_ * oyDevice_Copy_ ( {{ class_name }} *obj, oyObject_s_ object )
{
  {{ class_name }} *s = obj;

  if(!obj)
    return 0;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy_( s->oy_ );
    return s;
  }

  s = oy{{ class_base_name }}_Copy__( obj, object );

  return s;
}
 
/** @internal
 *  Function oy{{ class_base_name }}_Release_
 *  @memberof {{ class_name }}
 *  @brief   release and possibly deallocate a {{ class_base_name }} object
 *
 *  @param[in,out] obj                 {{ class_base_name }} struct object
 *
 *  @version Oyranos: {{ oyranos_version }}
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oy{{ class_base_name }}_Release_( {{ class_name }} **obj )
{
  /* ---- start of common object destructor ----- */
  {{ class_name }} *s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  *obj = 0;

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
{% endblock SpecificPrivateMethodsDefinitions %}
