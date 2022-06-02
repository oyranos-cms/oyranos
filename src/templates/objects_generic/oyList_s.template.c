{% extends "Base_s.c" %}
{% load gsoc_extensions %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oy{{ class.listBaseName }}_s_.h"
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
                                       oy{{ class.listBaseName }}_s       ** obj,
                                       int                 pos )
{
  {{ class.privName }} * s = ({{ class.privName }}*)list;
  int error = 0;

  if(!s)
    return -1;

  {% block MoveIn_CheckType %}oyCheckType__m{% endblock %}( oyOBJECT_{{ class.baseName|underscores|upper }}_S, return 1 )

  if(obj && *obj &&
     {% block MoveIn_CheckTypes %}(*obj)->type_ == oyOBJECT_{{ class.listBaseName|underscores|upper }}_S{% endblock %})
  {
    if(!s->list_)
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

  if(!s)
    return -1;

  {% block ReleaseAt_CheckType %}oyCheckType__m{% endblock %}( oyOBJECT_{{ class.baseName|underscores|upper }}_S, return 1 )

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
OYAPI oy{{ class.listBaseName }}_s * OYEXPORT
           oy{{ class.baseName }}_Get             ( {{ class.name }}       * list,
                                       int                 pos )
{       
  {{ class.privName }} * s = ({{ class.privName }}*)list;

  if(!s)
    return 0;

  {% block Get_CheckType %}oyCheckType__m{% endblock %}( oyOBJECT_{{ class.baseName|underscores|upper }}_S, return 0 )

  return (oy{{ class.listBaseName }}_s *) {% block Get_RefType %}oyStructList_GetRefType( s->list_, pos, oyOBJECT_{{ class.listBaseName|underscores|upper }}_S){% endblock %};
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
  {{ class.privName }} * s = ({{ class.privName }}*)list;

  if(!s)
    return 0;

  {% block Count_CheckType %}oyCheckType__m{% endblock %}( oyOBJECT_{{ class.baseName|underscores|upper }}_S, return 0 )

  return oyStructList_Count( s->list_ );
}

/** Function oy{{ class.baseName }}_Clear
 *  @memberof {{ class.name }}
 *  @brief   clear the elements in a {{ class.baseName }} list
 *
 *  @param[in,out] list                the list
 *  @return                            0 - success; otherwise - error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oy{{ class.baseName }}_Clear           ( {{ class.name }}       * list )
{       
  {{ class.privName }} * s = ({{ class.privName }}*)list;

  if(!s)
    return -1;

  {% block Clear_CheckType %}oyCheckType__m{% endblock %}( oyOBJECT_{{ class.baseName|underscores|upper }}_S, return 0 )

  return oyStructList_Clear( s->list_ );
}

/** Function oy{{ class.baseName }}_Sort
 *  @memberof {{ class.name }}
 *  @brief   sort a list according to a rank_list
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oy{{ class.baseName }}_Sort            ( {{ class.name }}       * list,
                                       int32_t           * rank_list )
{       
  {{ class.privName }} * s = ({{ class.privName }}*)list;

  if(!s)
    return -1;

  {% block Sort_CheckType %}oyCheckType__m{% endblock %}( oyOBJECT_{{ class.baseName|underscores|upper }}_S, return 0 )

  return oyStructList_Sort( s->list_, rank_list );
}
{% endblock GeneralPublicMethodsDefinitions %}
