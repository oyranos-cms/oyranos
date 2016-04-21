{% extends "oyCMMapi_s.template.c" %}
{% load gsoc_extensions %}

{% block GeneralPublicMethodsDefinitions %}
{{ block.super }}
/**
 *  @memberof {{ class.name }}
 *  @brief   set filter type specific runtime data
 *
 *  Runtime data can be used as context by a backend during execution. The data
 *  is typical set during backend load.
 *
 *  That data is apart from a filter object, which can have lifetime data
 *  associated through a oyFilterNode_GetContext(). A filter connector
 *  can have its processing data associated through oyFilterNode_SetData().
 *
 *  @param[in,out] api                 api object
 *  @param[in]     ptr                 the data needed to run the filter type
 *  @return                            error
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/12/19
 *  @since   2013/12/19 (Oyranos: 0.9.5)
 */
OYAPI int  OYEXPORT
           oy{{ class.baseName }}_SetBackendContext ( {{ class.name }}       * api,
                                       oyPointer_s       * ptr )
{
  {{ class.privName }} * s = ({{ class.privName }}*)api;
  int error = 0;

  if(!s)
    return -1;

  /* slightly fragile but inheritable */
  oyCheckTypeRange_m( oyOBJECT_{{ class.baseName|underscores|upper }}_S, oyOBJECT_CMM_API_MAX, return 1 )

  {
    if(s->runtime_context)
      oyPointer_Release( &s->runtime_context );
    s->runtime_context = oyPointer_Copy( ptr, NULL );
  }   

  return error;
}

/**
 *  @memberof {{ class.name }}
 *  @brief   get filter type specific runtime data
 *
 *  Runtime data can be used as context by a backend during execution.
 *
 *  That data is apart from a filter object, which can have lifetime data
 *  associated through a oyFilterNode_GetContext(). A filter connector
 *  can have its processing data associated through oyFilterNode_SetData().
 *
 *  @param[in]     api                 api object
 *  @return                            the context needed to run the filter type
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/12/19
 *  @since   2013/12/19 (Oyranos: 0.9.5)
 */
OYAPI oyPointer_s * OYEXPORT
           oy{{ class.baseName }}_GetBackendContext ( {{ class.name }}       * api )
{
  {{ class.privName }} * s = ({{ class.privName }}*)api;
  oyPointer_s * ptr = NULL;

  if(!s)
    return ptr;

  /* slightly fragile but inheritable */
  oyCheckTypeRange_m( oyOBJECT_{{ class.baseName|underscores|upper }}_S, oyOBJECT_CMM_API_MAX, return NULL )

  oyPointer_Copy( s->runtime_context, NULL );
  return s->runtime_context;
}
{% endblock GeneralPublicMethodsDefinitions %}
