{% extends "Base_s.c" %}

{% block GeneralPublicMethodsDefinitions %}
{{ block.super }}
/** Function oy{{ class.baseName }}_MoveIn
 *  @memberof {{ class.name }}
 *  @brief   add a element to a {{ class.baseName }} list
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
  {{ class.name }} * s = list;
  int error = 0;

  if(s)
    oyCheckType__m( oyOBJECT_{{ class.baseName|upper }}_S, return 1 )

  if(obj && *obj && (*obj)->type_ == oyOBJECT_{{ class.listOf|upper }}_S)
  {
    if(!s)
    {
      s = oy{{ class.baseName }}_New(0);
      error = !s;
    }                                  

    if(!error && !s->list_)
    {
      s->list_ = oyStructList_Create( s->type_, 0, 0 );
      error = !s->list_;
    }
      
    if(!error)
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos );
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
  {{ class.name }} * s = list;

  if(!error)
    oyCheckType__m( oyOBJECT_{{ class.baseName|upper }}_S, return 1 )

  if(!error && list->type_ != oyOBJECT_{{ class.baseName|upper }}_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( list->list_, pos );

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
  {{ class.name }} * s = list;

  if(!error)
    oyCheckType__m( oyOBJECT_{{ class.baseName|upper }}_S, return 0 )

  if(!error)
    return ({{ class.listOf }} *) oyStructList_GetRefType( list->list_, pos, oyOBJECT_{{ class.listOf|upper }}_S ); 
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
  {{ class.name }} * s = list;

  if(!error)
    oyCheckType__m( oyOBJECT_{{ class.baseName|upper }}_S, return 0 )

  if(!error)
    return oyStructList_Count( list->list_ );
  else return 0;
}
{% endblock GeneralPublicMethodsDefinitions %}
