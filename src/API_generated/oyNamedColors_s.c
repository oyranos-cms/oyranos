/** @file oyNamedColors_s.c

   [Template file inheritance graph]
   +-> oyNamedColors_s.template.c
   |
   +-> oyList_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyNamedColors_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyNamedColors_s_.h"

#include "oyNamedColor_s_.h"
  



/** Function oyNamedColors_New
 *  @memberof oyNamedColors_s
 *  @brief   allocate a new NamedColors object
 */
OYAPI oyNamedColors_s * OYEXPORT
  oyNamedColors_New( oyObject_s object )
{
  oyObject_s s = object;
  oyNamedColors_s_ * namedcolors = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  namedcolors = oyNamedColors_New_( s );

  return (oyNamedColors_s*) namedcolors;
}

/** Function  oyNamedColors_Copy
 *  @memberof oyNamedColors_s
 *  @brief    Copy or Reference a NamedColors object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     namedcolors                 NamedColors struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyNamedColors_s* OYEXPORT
  oyNamedColors_Copy( oyNamedColors_s *namedcolors, oyObject_s object )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*) namedcolors;

  if(s)
  {
    oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return NULL )
  }
  else
    return NULL;

  s = oyNamedColors_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyNamedColors_s" );

  return (oyNamedColors_s*) s;
}
 
/** Function oyNamedColors_Release
 *  @memberof oyNamedColors_s
 *  @brief   release and possibly deallocate a oyNamedColors_s object
 *
 *  @param[in,out] namedcolors                 NamedColors struct object
 */
OYAPI int OYEXPORT
  oyNamedColors_Release( oyNamedColors_s **namedcolors )
{
  oyNamedColors_s_ * s = 0;

  if(!namedcolors || !*namedcolors)
    return 0;

  s = (oyNamedColors_s_*) *namedcolors;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 1 )

  *namedcolors = 0;

  return oyNamedColors_Release_( &s );
}

/** Function oyNamedColors_MoveIn
 *  @memberof oyNamedColors_s
 *  @brief   add an element to a NamedColors list
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
           oyNamedColors_MoveIn          ( oyNamedColors_s       * list,
                                       oyNamedColor_s       ** obj,
                                       int                 pos )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*)list;
  int error = 0;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 1 )

  if(obj && *obj &&
     (*obj)->type_ == oyOBJECT_NAMED_COLOR_S)
  {
    if(!s->list_)
    {
      s->list_ = oyStructList_Create( s->type_, 0, 0 );
      error = !s->list_;
    }
      
    if(!error)
    error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos, 0 );
  }   

  return error;
}

/** Function oyNamedColors_ReleaseAt
 *  @memberof oyNamedColors_s
 *  @brief   release a element from a NamedColors list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyNamedColors_ReleaseAt       ( oyNamedColors_s       * list,
                                       int                 pos )
{ 
  int error = !list;
  oyNamedColors_s_ * s = (oyNamedColors_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 1 )

  if(!error && oyStructList_GetParentObjType(s->list_) != oyOBJECT_NAMED_COLORS_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( s->list_, pos );

  return error;
}

/** Function oyNamedColors_Get
 *  @memberof oyNamedColors_s
 *  @brief   get a element of a NamedColors list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI oyNamedColor_s * OYEXPORT
           oyNamedColors_Get             ( oyNamedColors_s       * list,
                                       int                 pos )
{       
  int error = !list;
  oyNamedColors_s_ * s = (oyNamedColors_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 0 )

  if(!error)
    return (oyNamedColor_s *) oyStructList_GetRefType( s->list_, pos, oyOBJECT_NAMED_COLOR_S);
  else
    return 0;
}   

/** Function oyNamedColors_Count
 *  @memberof oyNamedColors_s
 *  @brief   count the elements in a NamedColors list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyNamedColors_Count           ( oyNamedColors_s       * list )
{       
  int error = !list;
  oyNamedColors_s_ * s = (oyNamedColors_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 0 )

  if(!error)
    return oyStructList_Count( s->list_ );
  else return 0;
}

/** Function oyNamedColors_Clear
 *  @memberof oyNamedColors_s
 *  @brief   clear the elements in a NamedColors list
 *
 *  @param[in,out] list                the list
 *  @return                            0 - success; otherwise - error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyNamedColors_Clear           ( oyNamedColors_s       * list )
{       
  int error = !list;
  oyNamedColors_s_ * s = (oyNamedColors_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 0 )

  if(!error)
    return oyStructList_Clear( s->list_ );
  else return 0;
}

/** Function oyNamedColors_Sort
 *  @memberof oyNamedColors_s
 *  @brief   sort a list according to a rank_list
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyNamedColors_Sort            ( oyNamedColors_s       * list,
                                       int32_t           * rank_list )
{       
  int error = !list;
  oyNamedColors_s_ * s = (oyNamedColors_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 0 )

  if(!error)
    return oyStructList_Sort( s->list_, rank_list );
  else return 0;
}



/* Include "NamedColors.public_methods_definitions.c" { */
/** @memberof oyNamedColors_s
 *  @brief    set name prefix
 *
 *  @param[in]     colors              Oyranos colors struct pointer
 *  @param[in]     string              prefix string
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/08/25
 *  @since   2013/08/25 (Oyranos: 0.9.5)
 */
void   oyNamedColors_SetPrefix       ( oyNamedColors_s   * colors,
                                       const char        * string )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*) colors;
  if(!colors)
    return;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return )

  if(s->prefix)
    oyObject_GetDeAlloc( s->oy_ )( &s->prefix );

  if(string)
    s->prefix = oyStringCopy_( string, oyObject_GetAlloc( s->oy_) );
}

/** @memberof oyNamedColors_s
 *  @brief    set name suffix
 *
 *  @param[in]     colors              Oyranos colors struct pointer
 *  @param[in]     string              suffix string
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/08/25
 *  @since   2013/08/25 (Oyranos: 0.9.5)
 */
void   oyNamedColors_SetSuffix       ( oyNamedColors_s   * colors,
                                       const char        * string )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*) colors;
  if(!colors)
    return;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return )

  if(s->suffix)
    oyObject_GetDeAlloc( s->oy_ )( &s->suffix );

  if(string)
    s->suffix = oyStringCopy_( string, oyObject_GetAlloc( s->oy_) );
}

/** @memberof oyNamedColors_s
 *  @brief    get name prefix
 *
 *  @param[in]     colors              Oyranos colors struct pointer
 *  @return                            prefix string
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/09/03
 *  @since   2013/09/03 (Oyranos: 0.9.5)
 */
const char * oyNamedColors_GetPrefix ( oyNamedColors_s   * colors )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*) colors;
  if(!colors)
    return 0;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 0 )

  if(s->prefix)
    return s->prefix;
  else
    return "";
}

/** @memberof oyNamedColors_s
 *  @brief    get name suffix
 *
 *  @param[in]     colors              Oyranos colors struct pointer
 *  @return                            suffix string
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/09/03
 *  @since   2013/09/03 (Oyranos: 0.9.5)
 */
const char * oyNamedColors_GetSuffix ( oyNamedColors_s   * colors )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*) colors;
  if(!colors)
    return 0;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 0 )

  if(s->suffix)
    return s->suffix;
  else
    return "";
}

/** @memberof oyNamedColors_s
 *  @brief    get full length name
 *
 *  The name is constructed from suffix, name and prefix.
 *
 *  @param[in]     colors              Oyranos colors struct pointer
 *  @param[in]     pos                 nth color in list
 *  @return                            string
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/12/12
 *  @since   2013/09/03 (Oyranos: 0.9.5)
 */
const char * oyNamedColors_GetColorName (
                                       oyNamedColors_s   * colors,
                                       int                 pos )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*) colors;
  oyNamedColor_s * c;
  const char * text = "----";
  char * txt = NULL;

  if(!colors)
    return 0;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 0 )

  c = (oyNamedColor_s*) oyStructList_GetRefType( s->list_, pos,
                                                 oyOBJECT_NAMED_COLOR_S );

  oyStringAddPrintf_( &txt,
                      oyObject_GetAlloc( s->oy_ ),
                      oyObject_GetDeAlloc( s->oy_ ),
                      "%s%s%s",
                      oyNoEmptyString_m_(s->prefix),
                      oyNoEmptyString_m_(
                        oyNamedColor_GetName( c,
                                              oyNAME_NICK,0) ),
                      oyNoEmptyString_m_(s->suffix)
                    );
  oyObject_SetName ( c->oy_, txt, oyNAME_NAME );
  oyFree_m_( txt );
  text = oyObject_GetName( s->oy_, oyNAME_NAME );

  oyNamedColor_Release( &c );

  return text;
}

/* } Include "NamedColors.public_methods_definitions.c" */

