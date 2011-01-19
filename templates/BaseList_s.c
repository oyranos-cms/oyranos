{% extends "Base_s.c" %}
{% load gsoc_extensions %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "{{ class.content.privName }}.h"
{% endblock %}

{% block GeneralPublicMethodsDefinitions %}
{{ block.super }}
/** Function oy{{ class.baseName }}_MoveIn
 *  @memberof {{ class.name }}
 *  @brief   add an element to a {{ class.baseName }} list
 *
 *  @param[in]     list                list
 *  @param[in,out] obj                 list element
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oy{{ class.baseName }}_MoveIn          ( {{ class.name }}       * list,
                                       {{ class.listOf }}       ** obj,
                                       int                 pos )
{
  {{ class.privName }} * s = ({{ class.privName }}*)list;
  int error = 0;

  if(s)
    oyCheckType__m( oyOBJECT_{{ class.baseName|underscores|upper }}_S, return 1 )

  if(obj && *obj &&
      {% block MoveIn_CheckTypes %}(*obj)->type_ == oyOBJECT_{{ class.content.baseName|underscores|upper }}_S{% endblock %})
  {
    if(!list)
    {
      list = oy{{ class.baseName }}_New(0);
      error = !list;
    }                                  

    if(!error && !s->list_)
    {
      s->list_ = oyStructList_Create( s->type_, 0, 0 );
      error = !s->list_;
    }
      
    if(!error)
      {% block MoveIn_Observed %}error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos, 0 );{% endblock %}
  }   

  return error;
}

/** Function oy{{ class.baseName }}_ReleaseAt
 *  @memberof {{ class.name }}
 *  @brief   release a element from a {{ class.baseName }} list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oy{{ class.baseName }}_ReleaseAt       ( {{ class.name }}       * list,
                                       int                 pos )
{ 
  int error = !list;
  {{ class.privName }} * s = ({{ class.privName }}*)list;

  if(!error)
    oyCheckType__m( oyOBJECT_{{ class.baseName|underscores|upper }}_S, return 1 )

  if(!error && oyStructList_GetParentObjType(s->list_) != oyOBJECT_{{ class.baseName|underscores|upper }}_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( s->list_, pos );

  return error;
}

/** Function oy{{ class.baseName }}_Get
 *  @memberof {{ class.name }}
 *  @brief   get a element of a {{ class.baseName }} list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI {{ class.listOf }} * OYEXPORT
           oy{{ class.baseName }}_Get             ( {{ class.name }}       * list,
                                       int                 pos )
{       
  int error = !list;
  {{ class.privName }} * s = ({{ class.privName }}*)list;

  if(!error)
    oyCheckType__m( oyOBJECT_{{ class.baseName|underscores|upper }}_S, return 0 )

  if(!error)
    return ({{ class.listOf }} *) oyStructList_GetRefType( s->list_, pos, oyOBJECT_{{ class.content.baseName|underscores|upper }}_S );
  else
    return 0;
}   

/** Function oy{{ class.baseName }}_Count
 *  @memberof {{ class.name }}
 *  @brief   count the elements in a {{ class.baseName }} list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oy{{ class.baseName }}_Count           ( {{ class.name }}       * list )
{       
  int error = !list;
  {{ class.privName }} * s = ({{ class.privName }}*)list;

  if(!error)
    oyCheckType__m( oyOBJECT_{{ class.baseName|underscores|upper }}_S, return 0 )

  if(!error)
    return oyStructList_Count( s->list_ );
  else return 0;
}
{% endblock GeneralPublicMethodsDefinitions %}
