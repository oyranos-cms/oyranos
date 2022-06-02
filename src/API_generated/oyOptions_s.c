/** @file oyOptions_s.c

   [Template file inheritance graph]
   +-> oyOptions_s.template.c
   |
   +-> BaseList_s.c
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


  
#include "oyOptions_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"



#include "oyOptions_s_.h"

#include "oyOption_s_.h"

#include "oyranos_sentinel.h"
  



/** Function oyOptions_New
 *  @memberof oyOptions_s
 *  @brief   allocate a new Options object
 */
OYAPI oyOptions_s * OYEXPORT
  oyOptions_New( oyObject_s object )
{
  oyObject_s s = object;
  oyOptions_s_ * options = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  options = oyOptions_New_( s );

  return (oyOptions_s*) options;
}

/** Function  oyOptions_Copy
 *  @memberof oyOptions_s
 *  @brief    Copy or Reference a Options object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     options                 Options struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyOptions_s* OYEXPORT
  oyOptions_Copy( oyOptions_s *options, oyObject_s object )
{
  oyOptions_s_ * s = (oyOptions_s_*) options;

  if(s)
  {
    oyCheckType__m( oyOBJECT_OPTIONS_S, return NULL )
  }
  else
    return NULL;

  s = oyOptions_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyOptions_s" );

  return (oyOptions_s*) s;
}
 
/** Function oyOptions_Release
 *  @memberof oyOptions_s
 *  @brief   release and possibly deallocate a oyOptions_s object
 *
 *  @param[in,out] options                 Options struct object
 */
OYAPI int OYEXPORT
  oyOptions_Release( oyOptions_s **options )
{
  oyOptions_s_ * s = 0;

  if(!options || !*options)
    return 0;

  s = (oyOptions_s_*) *options;

  oyCheckType__m( oyOBJECT_OPTIONS_S, return 1 )

  *options = 0;

  return oyOptions_Release_( &s );
}

/** Function oyOptions_MoveIn
 *  @memberof oyOptions_s
 *  @brief   add an element to a Options list
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
           oyOptions_MoveIn          ( oyOptions_s       * list,
                                       oyOption_s       ** obj,
                                       int                 pos )
{
  oyOptions_s_ * s = (oyOptions_s_*)list;
  int error = 0;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_OPTIONS_S, return 1 )

  if(obj && *obj &&
     (*obj)->type_ == oyOBJECT_OPTION_S)
  {
    if(!s->list_)
    {
      s->list_ = oyStructList_Create( s->type_, 0, 0 );
      error = !s->list_;
    }
      
    if(!error)
    {
      if(oyStruct_IsObserved((oyStruct_s*)s, 0))
        oyStruct_ObserverAdd( (oyStruct_s*)*obj, (oyStruct_s*)s->list_, 0, 0 );
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos,
                                   OY_OBSERVE_AS_WELL );
    }
  }   

  return error;
}

/** Function oyOptions_ReleaseAt
 *  @memberof oyOptions_s
 *  @brief   release a element from a Options list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyOptions_ReleaseAt       ( oyOptions_s       * list,
                                       int                 pos )
{ 
  int error = !list;
  oyOptions_s_ * s = (oyOptions_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_OPTIONS_S, return 1 )

  if(!error && oyStructList_GetParentObjType(s->list_) != oyOBJECT_OPTIONS_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( s->list_, pos );

  return error;
}

/** Function oyOptions_Get
 *  @memberof oyOptions_s
 *  @brief   get a element of a Options list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI oyOption_s * OYEXPORT
           oyOptions_Get             ( oyOptions_s       * list,
                                       int                 pos )
{       
  oyOptions_s_ * s = (oyOptions_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_OPTIONS_S, return 0 )

  return (oyOption_s *) oyStructList_GetRefType( s->list_, pos, oyOBJECT_OPTION_S);
}   

/** Function oyOptions_Count
 *  @memberof oyOptions_s
 *  @brief   count the elements in a Options list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyOptions_Count           ( oyOptions_s       * list )
{       
  oyOptions_s_ * s = (oyOptions_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_OPTIONS_S, return 0 )

  return oyStructList_Count( s->list_ );
}

/** Function oyOptions_Clear
 *  @memberof oyOptions_s
 *  @brief   clear the elements in a Options list
 *
 *  @param[in,out] list                the list
 *  @return                            0 - success; otherwise - error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyOptions_Clear           ( oyOptions_s       * list )
{       
  oyOptions_s_ * s = (oyOptions_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_OPTIONS_S, return 0 )

  return oyStructList_Clear( s->list_ );
}

/** Function oyOptions_Sort
 *  @memberof oyOptions_s
 *  @brief   sort a list according to a rank_list
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyOptions_Sort            ( oyOptions_s       * list,
                                       int32_t           * rank_list )
{       
  oyOptions_s_ * s = (oyOptions_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_OPTIONS_S, return 0 )

  return oyStructList_Sort( s->list_, rank_list );
}



/* Include "Options.public_methods_definitions.c" { */
#include "oyranos_definitions.h"
#include "oyranos_db.h" /* oyjl API */

/** Function   oyOptions_FromBoolean
 *  @memberof  oyOptions_s
 *  @brief     boolean operations on two sets of options
 *
 *  @see oyOptions_Add
 *
 *  @param[in]     set_a               options set A
 *  @param[in]     set_b               options set B
 *  @param[in]     type                the operation to perform; oyBOOLEAN_UNION is supported
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/09/27
 *  @since   2008/06/28 (Oyranos: 0.1.8)
 */
oyOptions_s *  oyOptions_FromBoolean ( oyOptions_s       * set_a,
                                       oyOptions_s       * set_b,
                                       oyBOOLEAN_e         type,
                                       oyObject_s          object )
{
  int error = !set_a && !set_b;
  oyOptions_s * options = 0;
  oyOption_s * option_a = 0, * option_b = 0;
  oyOption_s_ * a_ = NULL, * b_ = NULL;
  int set_an = oyOptions_Count( set_a ),
      set_bn = oyOptions_Count( set_b );
  int i, j,
      found = 0;
  char * txt_1, * txt_2;

  if(error <= 0)
  {
    for(i = 0; i < set_an; ++i)
    {
      option_a = oyOptions_Get( set_a, i );
      a_ = (oyOption_s_*) option_a;
      txt_1 = oyFilterRegistrationToText( a_->registration,
                                          oyFILTER_REG_OPTION, 0);

      for(j = 0; j < set_bn; ++j)
      {
        option_b = oyOptions_Get( set_b, j );
        b_ = (oyOption_s_*) option_b;
        txt_2 = oyFilterRegistrationToText( b_->registration,
                                            oyFILTER_REG_OPTION, 0);

        found = oyTextboolean_( txt_1, txt_2, type );

        /* add to the list */
        if(found > 0)
        {
          if(!options)
            options = oyOptions_New(0);

          if(found != 2 && a_->value_type != b_->value_type)
            found = 0;

          if(found)
          {
            oyOptions_Add( options, option_a, -1, object );
            if(found == 2)
              oyOptions_Add( options, option_b, -1, object );
          }
        }

        oyFree_m_( txt_2 );
        oyOption_Release( &option_b );
      }

      oyFree_m_( txt_1 );
      oyOption_Release( &option_a );
    }

    if(!set_an && set_bn &&
       (type == oyBOOLEAN_UNION || type == oyBOOLEAN_DIFFERENZ))
    {
      options = oyOptions_Copy( set_b, object );
    }
    if(set_an && !set_bn &&
       (type == oyBOOLEAN_UNION))
    {
      options = oyOptions_Copy( set_a, object );
    }
  }

  return options;
}

/** Function    oyOptions_FromJSON
 *  @memberof   oyOptions_s
 *  @brief      deserialise a text file to oyOptions_s data
 *
 *  This function is based on oyjl. Arrays are ignored except
 *  the index is specified in the xpath with the xformat + ... arguments.
 *  The xpath is build from xformat + optional following printf style
 *  arguments.
 *
 *  The following code sample comes from @ref tutorial_json_options.c.
 *  @dontinclude   tutorial_json_options.c
    The "key_path" options is here set to demonstrate path substitution:
    @skipline key_path
    Then the JSON text is parsed and the second array member of org/free is
    picked:
    @skip json
    @until oyOptions_FromJSON
    The result options shall contain net/host/path/s2key_c:val_c and
    net/host/path/s2key_d:val_d with the path substitution applied and the
    selected array members taked each into a string option.
 *
 *  @param[in]     json_text           the text to process
 *  @param[in]     options             optional
 *                                     - "underline_key_suffix" will be used as
 *                                       suffix for keys starting with underline
 *                                       '_'
 *                                     - "key_path" will be used as suffix to
 *                                       the parsed keys below xpath.
 *                                     - "count" will be set to the number of
 *                                       elements in xpath.
 *  @param[out]    result              the result; A existing object will be
 *                                     merged.
 *  @param[in]     xformat             the xpath format string
 *  @param[in]     ...                 variable argument list for xpath
 *  @return                            error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/10/04
 *  @since   2016/11/17 (Oyranos: 0.9.6)
 */
int          oyOptions_FromJSON      ( const char        * json_text,
                                       oyOptions_s       * options,
                                       oyOptions_s      ** result,
                                       const char        * xformat,
                                       ... )
{
  int error = !json_text || !result;
  oyjl_val json = 0, v = 0, xv = 0;
  char * val, * key = NULL, * t = NULL;
  int count, i;
  oyOptions_s * opts = NULL;
  const char * underline_key_suffix = oyOptions_FindString( options,
                                                    "underline_key_suffix", 0 ),
             * key_path = oyOptions_FindString( options, "key_path", 0 );
  oyOption_s * co = oyOptions_Find( options, "count", oyNAME_PATTERN );
  char * xpath = NULL;

  if(!json_text)
  { WARNc_S( "missed json_text argument" );
    return error;
  }
  if(!result)
  { WARNc_S( "missed result argument" );
    return error;
  }

  if(!xformat) xformat = "";

  {
    va_list list;
    size_t sz = 0;
    int len = 0,
        l;

    va_start( list, xformat);
    len = vsnprintf( xpath, sz, xformat, list);
    va_end  ( list );

    oyAllocHelper_m_(xpath, char, len + 1, oyAllocateFunc_, return 1);
    va_start( list, xformat);
    l = vsnprintf( xpath, len+1, xformat, list);
    if(l != len)
      fprintf(stderr, "vsnprintf lengths differ: %d %d\n", l,len );
    va_end  ( list );
  }

  json = oyJsonParse( json_text, NULL );
  if(!json)
  {
    error = 1;
    goto cleanFJson;
  }

  opts = *result;

  if(strlen(xpath))
    xv = oyjlTreeGetValue( json, 0, xpath );
  else
  {
    char ** paths = oyjlTreeToPaths( json, 1000000, NULL, OYJL_KEY, &count );
    for(i = 0; i < count; ++i)
    {
      const char * xpath = paths[i];
      v = oyjlTreeGetValue( json, 0, xpath );
      val = oyjlValueText( v, oyAllocateFunc_ );

      /* ignore empty keys or values */
      if(val)
        oyOptions_SetFromString( &opts, xpath, val, OY_CREATE_NEW );

      if(key) oyDeAllocateFunc_(key);
      if(val) oyDeAllocateFunc_(val);
    }
    oyjlStringListRelease( &paths, count, NULL );
  }

  count = oyjlValueCount( xv );
  if(co) oyOption_SetFromInt( co, count, 0, 0 );
  for(i = 0; i < count; ++i)
  {
    char * k = NULL;
    if(xv->type == oyjl_t_object)
      key = oyStringCopy_( xv->u.object.keys[i], oyAllocateFunc_ );
    v = oyjlValuePosGet( xv, i );
    val = oyjlValueText( v, oyAllocateFunc_ );

    if(key && key[0] && key[0] == '_' && underline_key_suffix)
      oyStringAddPrintf( &k, 0,0, "%s/%s%s", key_path ? key_path : xpath,
                                  underline_key_suffix, key );
    else
      oyStringAddPrintf( &k, 0,0, "%s/%s", key_path ? key_path : xpath, key );

    /* ignore empty keys or values */
    if(k && val)
      oyOptions_SetFromString( &opts, k, val, OY_CREATE_NEW );

    if(k) oyDeAllocateFunc_(k);
    if(key) oyDeAllocateFunc_(key);
    if(val) oyDeAllocateFunc_(val);
  }

  *result = opts;
  opts = NULL;

cleanFJson:
  oyFree_m_( xpath );
  if(json) oyjlTreeFree( json );
  if(t) oyFree_m_(t);

  return error;
}

/** Function oyOptions_FromText
 *  @memberof oyOptions_s
 *  @brief   deserialise a text file to oyOptions_s data
 *
 *  This function is based on libxml2.
 *
 *  @param[in]     text                the text to process
 *  @param[in]     flags               for future use
 *  @param         object              the optional object
 *  @return                            the data
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/17 (Oyranos: 0.1.9)
 *  @date    2008/11/17
 */
oyOptions_s *  oyOptions_FromText    ( const char        * text,
                                       uint32_t            flags OY_UNUSED,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0;
  int error = !text;
  int is_json = !error && oyjlDataFormat(text) == oyNAME_JSON;
#ifdef HAVE_LIBXML2
  xmlDocPtr doc = 0;
  xmlNodePtr cur = 0;
  char ** texts = 0;
  int texts_n = 0;
  const char * root_start = "<top>", * root_end = "</top>";
  char * tmp = 0;

  if(error <= 0 && !is_json)
  {
    /* add a root level node - <top> */
    if(strlen(text) > 5 && memcmp( text, "<?xml", 5 ) == 0)
    {
      /* cut off xf:model xf:instance tags and place a simple <top> instead */
      const char * t = strstr(text,"xf:instance");
      char * tx = NULL;
      if(t)
        t = strstr(t,">");
      if(t)
        tmp = oyStringAppend_( root_start, t+1, oyAllocateFunc_ );
      if(tmp)
        tx = strstr(tmp, "</xf:instance>");
      if(tx)
        /* there should be enough bytes in the array */
        sprintf(tx, "%s", root_end);
    } else
    {
      tmp = oyStringAppend_( root_start, text, oyAllocateFunc_ );
      STRING_ADD( tmp, root_end );
    }

    doc = xmlParseMemory( tmp, oyStrlen_( tmp ) );
    error = !doc;

    oyFree_m_( tmp );

    if(doc)
    {
      cur = xmlDocGetRootElement(doc);
      /* skip now the artifically added <top> level */
      cur = cur->xmlChildrenNode;
    }
    error = !cur;
  }

  if(error <= 0 && !is_json)
  {
    s = oyOptions_New(object);

    oyOptions_ParseXML_( (oyOptions_s_*)s, &texts, &texts_n, doc, cur );

    oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
    xmlFreeDoc(doc);
  }
#endif
  if(error <= 0 && is_json)
  {
    error = oyOptions_FromJSON( text, NULL, &s, NULL );
  }

  return s;
}

/** Function oyOptions_Add
 *  @memberof oyOptions_s
 *  @brief   add a element to a Options list
 *
 *  We must not add any already listed option. 
 *  (oyFILTER_REG_TOP)
 *
 *  Adding a new element without any checks is as simple as following code:
 *  @code
 *     tmp = oyOption_Copy( option, object );
 *     oyOptions_MoveIn( options, &tmp, -1 ); @endcode
 *
 *  @param[in,out] options             options to manipulate
 *  @param[in]     option              the option to eventually add
 *  @param[in]     pos                 the position where to add; use -1 for append
 *  @param[in]     object              the optional object
 *  @return                            status
 *                                     - 0 on success
 *                                     - 1 error
 *                                     - -2 skipped adding
 *
 *  @version Oyranos: 0.9.6
 *  @since   2008/11/17 (Oyranos: 0.1.9)
 *  @date    2015/04/29
 */
int            oyOptions_Add         ( oyOptions_s       * options,
                                       oyOption_s        * option,
                                       int                 pos,
                                       oyObject_s          object )
{
  oyOption_s_ * tmp = NULL,
              * o = (oyOption_s_ *) option;
  int error = !options || !option;
  int n, i, skip = 0;
  char * o_opt,  /* option right key name part */
       * o_top,  /* option top key name part */
       * l_opt,  /* l - list */
       * l_top,
       * t;

  if(error <= 0)
  {
    o_opt = oyFilterRegistrationToText( o->registration,
                                        oyFILTER_REG_MAX, 0 );
    if(strstr(o_opt, "\\." ) || strchr(o_opt, '.' ))
    {
      t = strstr(o_opt, "\\." );
      if(!t)
        t = strrchr(o_opt, '.' );
      *t = 0;
    }
    o_top = oyFilterRegistrationToText( o->registration,
                                        oyFILTER_REG_TOP, 0 );
    n = oyOptions_Count( options );

    for(i = 0; i < n; ++i)
    {
      tmp = (oyOption_s_ *) oyOptions_Get( options, i );
      l_opt = oyFilterRegistrationToText( tmp->registration,
                                          oyFILTER_REG_MAX, 0 );
      if(strstr(l_opt, "\\." ) || strrchr(l_opt, '.' ))
      {
        t = strstr(l_opt, "\\." );
        if(!t)
          t = strrchr(l_opt, '.' );
        *t = 0;
      }
      l_top = oyFilterRegistrationToText( tmp->registration,
                                          oyFILTER_REG_TOP, 0 );
      if(oyStrcmp_(l_opt, o_opt) == 0)
      {
        skip = 2;
        error = -skip;
      }

      oyFree_m_( l_opt );
      oyFree_m_( l_top );
      oyOption_Release( (oyOption_s**)&tmp );
    }

    if(skip == 0)
    {
      tmp = (oyOption_s_*) oyOption_Copy( option, object );
      oyOptions_MoveIn( options, (oyOption_s**)&tmp, pos );

    } else if(skip == 2)
    {
      tmp = (oyOption_s_*) oyOptions_Find( options, o_opt, oyNAME_PATTERN );
      if(!tmp)
      {
        oyMessageFunc_p( oyMSG_WARN, (oyStruct_s*)options,
                         OY_DBG_FORMAT_ "Could not find: \"%s\"", OY_DBG_ARGS_,
                         oyNoEmptyName_m_(o_opt) );
        error = 1;
      }

      if(error <= 0)
      {
        /* preserve manual / on the fly settings */
        if(o->flags & oyOPTIONATTRIBUTE_EDIT &&
           !(tmp->flags & oyOPTIONATTRIBUTE_EDIT))
        {
          oyOption_Copy__Members( tmp, o );
          error = 0;
        } else
        /* preserve automatic / DB settings */
        if(o->flags & oyOPTIONATTRIBUTE_AUTOMATIC &&
           !(tmp->flags & oyOPTIONATTRIBUTE_AUTOMATIC) &&
           !(tmp->flags & oyOPTIONATTRIBUTE_EDIT))
        {
          oyOption_Copy__Members( tmp, o );
          error = 0;
        }

        oyOption_Release( (oyOption_s**)&tmp );
      }
    }

    oyFree_m_( o_opt );
    oyFree_m_( o_top );
  }

  return error;
}

/** Function oyOptions_Set
 *  @memberof oyOptions_s
 *  @brief   set a element in a Options list
 *
 *  Already listed options are replaced by the new option. A option new to
 *  the existing set will be referenced if no object argument is given.
 *
 *  Adding a new element without any checks is as simple as following code:
 *  @code
 *     tmp = oyOption_Copy( option, object );
 *     oyOptions_MoveIn( options, &tmp, -1 ); @endcode
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/25 (Oyranos: 0.1.10)
 *  @date    2009/10/25
 */
int            oyOptions_Set         ( oyOptions_s       * options,
                                       oyOption_s        * option,
                                       int                 pos,
                                       oyObject_s          object )
{
  oyOption_s *tmp = 0;
  int error = !options || !option;
  int n, i, replace = 0;

  if(error <= 0)
  {
    n = oyOptions_Count( options );

    for(i = 0; i < n && !replace; ++i)
    {
      tmp = oyOptions_Get( options, i );
      if(oyFilterRegistrationMatchKey( oyOptionPriv_m(tmp)->registration,
                                       oyOptionPriv_m(option)->registration, 0))
      {
        replace = 2;
        /* replace as we priorise the new value */
        oyOption_Copy__Members( oyOptionPriv_m(tmp), oyOptionPriv_m(option) );
      }
      oyOption_Release( &tmp );
    }

    if(replace == 0)
    {
      tmp = oyOption_Copy( option, object );
      oyOptions_MoveIn( options, &tmp, pos );
    }
  }

  return error;
}

/** Function oyOptions_SetOpts
 *  @memberof oyOptions_s
 *  @brief   set options in a Options list
 *
 *  Already listed options are replaced by the new options.
 *
 *  @see oyOptions_Set It will use copying.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/25 (Oyranos: 0.1.10)
 *  @date    2009/10/25
 */
int            oyOptions_SetOpts     ( oyOptions_s       * list,
                                       oyOptions_s       * add )
{
  int error = !list;
  int i,n;
  oyOption_s * o = 0;

  if(error <= 0)
  {
    n = oyOptions_Count( add );
    for(i = 0; i < n; ++i)
    {
      o = oyOptions_Get( add, i );
      oyOptions_Set( list, o, -1, list->oy_ );
      oyOption_Release( &o );
    }
  }

  return error;
}

/** Function oyOptions_AppendOpts
 *  @memberof oyOptions_s
 *  @brief   
 *
 *  Basic reference options from one list into an other.
 *
 *  @see oyOptions_MoveIn
 *
 *  @version Oyranos: 
 *  @since
 *  @date
 */
int            oyOptions_AppendOpts  ( oyOptions_s       * list,
                                       oyOptions_s       * append )
{
  int error = !list;
  int i,n;
  oyOption_s * o = 0;

  if(error <= 0)
  {
    n = oyOptions_Count( append );
    for(i = 0; i < n; ++i)
    {
      o = oyOptions_Get( append, i );
      oyOptions_MoveIn( list, &o, -1 );
    }
  }

  return error;
}

/**
 *  Function oyOptions_CopyFrom
 *  @memberof oyOptions_s
 *  @brief   copy from one option set to an other option set
 *
 *  @param[out]    list                target
 *  @param[in]     from                source
 *  @param         type                boolean operation before copy
 *  @param         fields              registration fields of each option
 *  @param         object              the optional object
 *  @return                            0 - success; 1 - error; -1 issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/05 (Oyranos: 0.1.10)
 *  @date    2009/10/25
 */
int            oyOptions_CopyFrom    ( oyOptions_s      ** list,
                                       oyOptions_s       * from,
                                       oyBOOLEAN_e         type,
                                       oyFILTER_REG_e      fields,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0,
              * tmp = 0, * tmp2 = 0;
  int error = !list;

  if(!from || error)
    return error ? error : -1;

  if(!*list)
  {
    s = oyOptions_New( object );
    if(!*list)
      *list = s;
    error = !*list;
  }

  if(error <= 0)
  {
    s = *list;

    if(type == oyBOOLEAN_UNION)
    {
      if(oyOptionsPriv_m(s)->list_ || oyOptionsPriv_m(from)->list_)
      {
        if(!oyOptionsPriv_m(s)->list_)
          oyOptionsPriv_m(s)->list_ = oyStructList_Create( oyOBJECT_OPTIONS_S, "oyOptions_CopyFrom", 0 );
        if(!oyOptionsPriv_m(from)->list_)
          oyOptionsPriv_m(from)->list_ = oyStructList_Create( oyOBJECT_OPTIONS_S, "oyOptions_CopyFrom", 0 );
      }
      error = oyOptions_SetOpts( s, from );

    } else
    {
      int list_n = oyOptions_Count( *list ),
          from_n = oyOptions_Count( from ),
          i,j, found;
      oyOption_s * list_o = 0,
                 * from_o = 0;
      char * list_reg = 0, * from_reg = 0;
      
      tmp = oyOptions_New( 0 );

      for(i = 0; i < list_n && error <= 0; ++i)
      {
        found = 0;
        list_o = oyOptions_Get( *list, i );
        list_reg = oyFilterRegistrationToText( oyOptionPriv_m(list_o)->registration,
                                               fields, 0 );

        for(j = 0; j < from_n && error <= 0; ++j)
        {
          from_o = oyOptions_Get( from, j );
          from_reg = oyFilterRegistrationToText( oyOptionPriv_m(from_o)->registration,
                                                 fields, 0 );

          if(oyFilterRegistrationMatch( list_reg, from_reg, 0 ))
            found = 1;

          if(type == oyBOOLEAN_INTERSECTION && found)
          {
            oyOptions_MoveIn( tmp, &from_o, -1 );
            break;
          }

          oyFree_m_( from_reg );
          oyOption_Release( &from_o );
        }

        if((type == oyBOOLEAN_SUBSTRACTION ||
            type == oyBOOLEAN_DIFFERENZ) &&
           !found)
          oyOptions_MoveIn( tmp, &list_o, -1 );

        oyFree_m_( list_reg );
        oyOption_Release( &list_o );
      }

      if(type == oyBOOLEAN_SUBSTRACTION ||
         type == oyBOOLEAN_INTERSECTION)
      {
        oyStructList_Release( &(oyOptionsPriv_m(*list))->list_ );
        oyOptionsPriv_m(*list)->list_ = oyOptionsPriv_m(tmp)->list_;
        oyOptionsPriv_m(tmp)->list_ = 0;

      } else
      if(type == oyBOOLEAN_DIFFERENZ)
      {
        tmp2 = oyOptions_New( 0 );
        for(i = 0; i < from_n && error <= 0; ++i)
        {
          found = 0;
          from_o = oyOptions_Get( from, i );
          from_reg = oyFilterRegistrationToText( oyOptionPriv_m(from_o)->registration,
                                                 fields, 0 );

          for(j = 0; j < list_n && error <= 0; ++j)
          {
            list_o = oyOptions_Get( *list, j );
            list_reg = oyFilterRegistrationToText( oyOptionPriv_m(list_o)->registration,
                                                   fields, 0 );

            if(oyFilterRegistrationMatch( from_reg, list_reg, 0 ))
              found = 1;

            oyFree_m_( list_reg );
            oyOption_Release( &list_o );
          }

          if(!found)
            oyOptions_MoveIn( tmp2, &from_o, -1 );

          oyFree_m_( from_reg );
          oyOption_Release( &from_o );
        }

        oyStructList_Release( &oyOptionsPriv_m(*list)->list_ );
        oyOptions_CopyFrom( list, tmp, oyBOOLEAN_UNION, 0, 0 );
        oyOptions_CopyFrom( list, tmp2, oyBOOLEAN_UNION, 0, 0 );
      }

      oyOptions_Release( &tmp );
      oyOptions_Release( &tmp2 );
    }

  }

  if(error > 0)
    oyOptions_Release( &s );

  return error;
}

/** Function oyOptions_Filter
 *  @memberof oyOptions_s
 *  @brief   filter options
 *
 *  Each option added to the add_list is a cheaply linked one.
 *
 *  @see oyOptions_Add
 *
 *  @param[out]    add_list            the options list to add to
 *  @param[out]    count               the number of matching options
 *  @param[in]     flags               for inbuild defaults 
 *                                     | oyOPTIONSOURCE_FILTER
 *  @param[in]     type                support are oyBOOLEAN_INTERSECTION and
 *                                     oyBOOLEAN_DIFFERENZ
 *  @param[in]     registration        a registration which shall be matched
 *  @param[in]     src_list            the options to select from
 *  @return                            0 - success; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/05 (Oyranos: 0.1.10)
 *  @date    2009/05/05
 */
int            oyOptions_Filter      ( oyOptions_s      ** add_list,
                                       int32_t           * count,
                                       uint32_t            flags,
                                       oyBOOLEAN_e         type,
                                       const char        * registration,
                                       oyOptions_s       * src_list )
{
  int error = !src_list || !add_list || *add_list == src_list,
      l_error = 0;
  oyOptions_s * s = src_list;
  oyOption_s * o = 0;
  int n, i;
  int32_t c = 0;
  uint32_t options_source = 0;

  oyCheckType__m( oyOBJECT_OPTIONS_S, return 1 )

  if(error <= 0)
  {
    n = oyOptions_Count( s );
    if(!*add_list)
      *add_list = oyOptions_New(0);

    if(!*add_list)
      error = 1;

    if(error <= 0)
    for(i = 0; i < n; ++i)
    {
      int found = 1;
      const char * o_registration;
      oyOPTIONSOURCE_e o_source;

      o = oyOptions_Get( s, i );
      o_registration = oyOptionPriv_m(o)->registration;
      o_source = oyOptionPriv_m(o)->source;

      if(found && registration &&
         !oyFilterRegistrationMatch( o_registration, registration, 0 ))
          found = 0;

      options_source = flags & oyOPTIONSOURCE_FILTER ? oyOPTIONSOURCE_FILTER :0;
      options_source |= flags & oyOPTIONSOURCE_DATA ? oyOPTIONSOURCE_DATA : 0;
      options_source |= flags & oyOPTIONSOURCE_USER ? oyOPTIONSOURCE_USER : 0;
      if(found && options_source && !(o_source & options_source))
        found = 0;

      if(type == oyBOOLEAN_UNION ||
         (type == oyBOOLEAN_INTERSECTION && found) ||
         (type == oyBOOLEAN_DIFFERENZ && !found)
        )
      {
        l_error = oyOptions_Add( *add_list, o, -1, 0 );
        if(l_error && error <= 0)
          error = l_error;
        ++c;
      }

      oyOption_Release( &o );
    }

    if(count)
      *count = c;
  }

  return error;
}

/** @memberof oyOptions_s
 *  @brief   dump options to text
 *
 *  The type argument should select the following string in return: \n
 *  - oyNAME_NAME - a readable XFORMS data model
 *  - oyNAME_NICK - the hash ID
 *  - oyNAME_DESCRIPTION - option registration name with key and without value
 *  - oyNAME_JSON - readable JSON supporting nested JSON
 *
 *  @todo streamline output and group, avoid writing all levels for each key
 *
 *  @param[in,out] options             the option
 *  @param         type                oyNAME_NICK is equal to an ID
 *  @return                            the text
 *
 *  @version Oyranos: 0.9.7
 *  @date    2020/03/20
 *  @since   2008/11/25 (Oyranos: 0.1.9)
 */
const char *   oyOptions_GetText     ( oyOptions_s       * options,
                                       oyNAME_e            type )
{
  int error = !options;
  const char * erg = 0;
  char * text = 0;
  oyOption_s * o = 0, *o2 = 0;
  int i, n, ti, c, j,j_n,k, min_level;
  int indent = ((int)type == oyNAME_JSON)?1:6; /* base indentation for better looking XFORMS documents */
  int * sort, changed;
  char ** old_levels = 0;
  int old_levels_n = 0;
  int close_oy_struct = 0;
  oyjl_val root = oyjlTreeNew("org"), v;

  if(error <= 0)
  {
    n = oyOptions_Count( options );
    if(!n)
      return 0;
    sort = oyAllocateFunc_( n * sizeof(int) );
    for( i = 0; i < n; ++i )
      sort[i]=i;

    if((int)type == oyNAME_JSON && n)
      STRING_ADD ( text, "{\n" );

    /* sort the options alphabetical */
    if(type == oyNAME_NAME ||
       (int)type == oyNAME_JSON)
    do
    {
      changed = 0;
      for( i = 0; i < n-1; ++i )
      {
        o = oyOptions_Get( options, sort[i] );
        o2 = oyOptions_Get( options, sort[i + 1] );
        c = oyStrcmp_(oyOptionPriv_m(o)->registration, oyOptionPriv_m(o2)->registration);
        if(c > 0)
        {
          ti = sort[i];
          sort[i] = sort[i + 1];
          sort[i+1] = ti;
          changed = 1;
        }
        oyOption_Release( &o );
        oyOption_Release( &o2 );
      }
      for( i = n-2; i >= 0; --i )
      {
        o = oyOptions_Get( options, sort[i] );
        o2 = oyOptions_Get( options, sort[i + 1] );
        c = oyStrcmp_(oyOptionPriv_m(o)->registration, oyOptionPriv_m(o2)->registration);
        if(c > 0)
        {
          ti = sort[i];
          sort[i] = sort[i + 1];
          sort[i+1] = ti;
          changed = 1;
        }
        oyOption_Release( &o );
        oyOption_Release( &o2 );
      }
    } while(changed);

    for( i = 0; i < n; ++i )
    {
      oyOption_s_ * o_ = 0;
      o = oyOptions_Get( options, sort[i] );
      o_ = (oyOption_s_*)o;

      if(!oyObject_GetName( o->oy_, oyNAME_DESCRIPTION ))
          oyOption_GetText(o, oyNAME_DESCRIPTION);

      if((int)type == oyNAME_JSON)
      {
        const char * key = oyObject_GetName( o->oy_, oyNAME_DESCRIPTION ),
                   * val = NULL;
        char * tmp = NULL;

        if( o_->value_type == oyVAL_STRUCT )
        {
          val = oyOption_GetText( o, type );
          if(oyjlDataFormat(val) == oyNAME_JSON)
          {
            oyjl_val opt_root = oyJsonParse( val, NULL );
            if(!opt_root)
              WARNc1_S("could not parse:\n%s", val);

            int count = 0;
            char ** paths = oyjlTreeToPaths( opt_root, 1000000, NULL, OYJL_KEY, &count );

            for(k = 0; k < count; ++k)
            {
              const char * xpath = paths[k];
              char * val;
              v = oyjlTreeGetValue( opt_root, 0, xpath );
              val = oyjlValueText( v, oyAllocateFunc_ );
              v = oyjlTreeGetValue( root, OYJL_CREATE_NEW, xpath );

              /* ignore empty keys or values */
              if(val)
                oyjlValueSetString( v, val );

              if(val) oyDeAllocateFunc_(val);
            }

            oyjlStringListRelease( &paths, count, free );
            oyjlTreeFree( opt_root );
          }
        }
        else
        {
          val = tmp = oyOption_GetValueText( o, oyAllocateFunc_ );

          v = oyjlTreeGetValue( root, OYJL_CREATE_NEW, key );
          oyjlValueSetString( v, val );
        }

        if( tmp ) oyFree_m_( tmp );
      }
      /* Omit redundant XML level closes and opens based on alphabetical input.
       */
      else if(type == oyNAME_NAME)
      {
        char * tmp = 0, **list = 0;

        j_n = 1;

        list = oyStringSplit_( oyObject_GetName( o->oy_, oyNAME_DESCRIPTION ),
                               '/', &j_n, oyAllocateFunc_);
        min_level = 1000;
        /* find the minimum different level */
        for( j = 0; j < j_n; ++j )
        {
          if(!old_levels ||
             (old_levels_n > j && oyStrcmp_(old_levels[j],list[j]) != 0))
            if(min_level > j)
              min_level = j;
        }

        /* close old levels */
        if(old_levels)
        for( j = old_levels_n-1; j >= min_level; --j )
        {
          if(j+1 < old_levels_n)
            for(k = 0; k < indent+j; ++k)
              STRING_ADD ( text, " " );
          if(old_levels_n > j)
          {
            if( close_oy_struct )
            {
              for(k = 0; k < indent+j_n-1; ++k)
                STRING_ADD ( text, " " );
              close_oy_struct = 0;
            }
            STRING_ADD ( text, "</" );
            STRING_ADD ( text, old_levels[j] );
            STRING_ADD ( text, ">" );
            if(j)
              STRING_ADD ( text, "\n" );
          }
        }

        /* open new levels */
        for( j = min_level; j < j_n; ++j )
        {
          for(k = 0; k < indent+j; ++k)
            STRING_ADD ( text, " " );
          STRING_ADD ( text, "<" );
          STRING_ADD ( text, list[j] );
          if(j+1==j_n)
            STRING_ADD ( text, ">" );
          else
            STRING_ADD ( text, ">\n" );
        }

        if( o_->value_type == oyVAL_STRUCT )
        {
          STRING_ADD ( text, "\n" );
          for(k = 0; k < indent+j_n-1; ++k)
            STRING_ADD ( text, " " );
          STRING_ADD ( text, oyOption_GetText( o, oyNAME_NAME ) );
          close_oy_struct = 1;
        }
        else
        {
          tmp = oyOption_GetValueText( o, oyAllocateFunc_ );
          STRING_ADD ( text, tmp );
          oyFree_m_( tmp );
        }
        if(old_levels)
          oyStringListRelease_( &old_levels, old_levels_n, oyDeAllocateFunc_ );
        old_levels = list;
        old_levels_n = j_n;
      }
      else
      {
        STRING_ADD ( text, oyOption_GetText( o, type) );
        STRING_ADD ( text, "\n" );
      }

      oyOption_Release( &o );
    }

    /* close all old levels */
    if(old_levels)
      for( j = j_n-1; j >= 0; --j )
      {
        if(j+1 < j_n)
          for(k = 0; k < indent+j; ++k)
            STRING_ADD ( text, " " );
        if(old_levels_n > j)
        {
          STRING_ADD ( text, "</" );
          STRING_ADD ( text, old_levels[j] );
          STRING_ADD ( text, ">" );

          if(j)
            STRING_ADD ( text, "\n" );
        }
      }

    if(old_levels)
      oyStringListRelease_( &old_levels, old_levels_n, oyDeAllocateFunc_ );

    if((int)type == oyNAME_JSON && n)
    {
      oyFree_m_(text);
      i = 0;
      oyjlTreeToJson( root, &i, &text );
      oyjlStringReplace( &text, "\n", "", oyStruct_GetAllocator((oyStruct_s*)options), oyStruct_GetDeAllocator((oyStruct_s*)options) );
      error = oyObject_SetName( options->oy_, text, type );
      if(text) { free(text); text = NULL; }
    } else
      error = oyObject_SetName( options->oy_, text, type );

    if(text)
      oyFree_m_( text );
    oyFree_m_( sort );
  }

  oyjlTreeFree( root );

  if(error <= 0)
    erg = oyObject_GetName( options->oy_, type );

  return erg;
}

/** Function oyOptions_CountType
 *  @memberof oyOptions_s
 *  @brief   search for options with special attributes
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/07/04
 *  @since   2009/03/04 (Oyranos: 0.1.10)
 */
int            oyOptions_CountType   ( oyOptions_s       * options,
                                       const char        * registration,
                                       oyOBJECT_e          type )
{
  int error = !options;
  int i, n, m = 0, found;
  oyOption_s_ * o = 0;
  oyOptions_s * s = options;


  if(!error)
    oyCheckType__m( oyOBJECT_OPTIONS_S, return 0 );

    n = oyOptions_Count( options );
    for(i = 0; i < n;  ++i)
    {
      o = (oyOption_s_*)oyOptions_Get( options, i );
      found = 1;

      if(found && registration &&
         !oyFilterRegistrationMatch( o->registration, registration, 0 ))
          found = 0;

      if(found && type &&
         (o->value_type != oyVAL_STRUCT || !o->value ||
          !o->value->oy_struct || o->value->oy_struct->type_ != type))
        found = 0;

      if(found)
        ++m;

      oyOption_Release( (oyOption_s**)&o );
    }

  return m;
}

int          oyOptionRegistrationMatch(const char        * registration,
                                       const char        * pattern,
                                       uint32_t            type )
{
  int match = 0;
  if(registration && pattern)
  {
    if(type == oyNAME_REGISTRATION &&
       strcmp(registration, pattern) == 0)
      match = 1;
    else if(type == oyNAME_PATTERN &&
       oyFilterRegistrationMatch( registration, pattern, 0 ))
      match = 1;
  }
  return match;
}

/** Function oyOptions_Find
 *  @memberof oyOptions_s
 *  @brief   search for a certain option key
 *
 *  This function returns the first found option for a given key.
 *  The key is represented by a registrations option level.
 *
 *  @param         options             set of options
 *  @param         registration        registration or key
 *  @param         type                allowed are:
 *                                     - oyNAME_PATTERN for a pattern match, that is what most users prefer
 *                                     - oyNAME_REGISTRATION for a exact comparision
 *  @return                            a matching options
 *
 *  @version Oyranos: 0.1.9
 *  @date    2009/04/13
 *  @since   2008/11/05 (Oyranos: 0.1.9)
 */
oyOption_s *   oyOptions_Find        ( oyOptions_s       * options,
                                       const char        * registration,
                                       uint32_t            type )
{
  int error = !options || !registration;
  oyOption_s * o = 0,
             * option = 0;
  int found;

  if(type != oyNAME_REGISTRATION &&
     type != oyNAME_PATTERN)
  {
    WARNc1_S("unrecognised type argument: %d\nallowed are oyNAME_REGISTRATION or oyNAME_PATTERN", type);
    return option;
  }

  if(error <= 0 && options && options->type_ == oyOBJECT_OPTIONS_S)
  {
    oyOptions_s * set_a = options;
    int set_an = oyOptions_Count( set_a ), i;

    for(i = 0; i < set_an; ++i)
    {
      o = oyOptions_Get( options, i );
      found = 1;

      if(found && registration)
      {
         if(!oyOptionRegistrationMatch( oyOptionPriv_m(o)->registration, registration, type ))
          found = 0;
      }

      if(found)
      {
        option = o;
        break;
      }

      oyOption_Release( &o );
    }
  }

  return option;
}

/** Function oyOptions_FindString
 *  @memberof oyOptions_s
 *  @brief   search for a certain option key and possibly value
 *
 *  This function returns the first found value for a given key.
 *  The key is represented by the oyOption_s::name::nick
 *  The returned string is valid along the livetime of the particular 
 *  option value.
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/10/07 (Oyranos: 0.1.8)
 *  @date    2009/04/13
 */
const char *   oyOptions_FindString  ( oyOptions_s       * options,
                                       const char        * registration,
                                       const char        * value )
{
  char * text = 0;
  int error = !options;
  oyOptions_s * s = options;
  oyOption_s_ * o = 0;
  int found = 0, j;

  if(!error)
    oyCheckType__m( oyOBJECT_OPTIONS_S, return 0 );

  if(error <= 0)
  {
    o = (oyOption_s_*)oyOptions_Find( options, registration,
                                      oyNAME_PATTERN );

    if(o && o->type_ == oyOBJECT_OPTION_S)
    {
      if(o->value_type == oyVAL_STRING)
      {
        text = o->value->string;

        if(!value ||
           (value && text && text[0] && oyStrstr_(value, text)))
          found = 1;
      } else if(o->value_type == oyVAL_STRING_LIST)
      {
        j = 0;

        while(o->value->string_list[j])
        {
          text = o->value->string_list[j];

          if(text && text[0])
            if(!value ||
               (value && oyStrstr_(value, text)))
              found = 1;

          if(found) break;

          ++j;
        }
      }
    }

    oyOption_Release( (oyOption_s**)&o );

    error = !found;

    if(!found)
      text = 0;
  }

  return text;
}

/** oyOptions_SetFromString()
 *  @memberof oyOptions_s
 *  @brief   change a value
 *
 *  @param[in,out] obj                 the options list or set to manipulate
 *  @param[in]     registration        the options registration name, e.g.
 *                                     "org/my_org/openicc/my_app/my_opt"
 *  @param[in]     value               the value to set
 *  @param[in]     flags               can be ::OY_CREATE_NEW for a new option,
 *                                     ::OY_STRING_LIST, ::OY_ADD_ALWAYS, ::OY_MATCH_KEY
 *  @return                            error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 *  @date    2011/03/15
 */
int            oyOptions_SetFromString(oyOptions_s      ** obj,
                                       const char        * registration,
                                       const char        * value,
                                       uint32_t            flags )
{
  int error = !obj ||
              (obj && *obj && (*obj)->type_ != oyOBJECT_OPTIONS_S) ||
              !registration ||
              !value;
  oyOption_s * o = NULL;

  if(error <= 0)
  {
    if(!*obj)
      *obj = oyOptions_New( 0 );

    if(flags & OY_MATCH_KEY)
    {
      const char * key = oyGetKeyFromRegistration( registration );
      o = oyOptions_Find( *obj, key, oyNAME_PATTERN );
    } else
      o = oyOptions_Find( *obj, registration, oyNAME_PATTERN );

    /** Add a new option if the OY_CREATE_NEW flag is present.
     */
    if((!o && oyToCreateNew_m(flags)) ||
        oyToAddAlways_m(flags))
    {
      o = oyOption_FromRegistration( registration, (*obj)->oy_ );
      error = !o;

      if(error <= 0)
      {
        /** Flags are passed on to oyOption_SetFromString, e.g. OY_STRING_LIST. */
        error = oyOption_SetFromString( o, value, flags & OY_STRING_LIST );
      } else
        WARNc3_S( "%s %s=%s",_("wrong argument to option:"),
                  oyNoEmptyString_m_(registration), oyNoEmptyString_m_(value) );

      oyOptions_MoveIn( (*obj), &o, -1 );

    } else
      oyOption_SetFromString( o, value, flags );

    oyOption_Release( &o );
  }

  return error;
}

/** Function oyOptions_FindInt
 *  @memberof oyOptions_s
 *  @brief   get a value
 *
 *  @param         options             the options list or set to manipulate
 *  @param         registration        the options registration name, e.g.
 *                                     "org/my_org/openicc/my_app/my_opt"
 *                                     or simple key, e.g. "my_opt"
 *  @param         pos                 the values position in a possible list
 *  @param         result              the integer
 *  @return                            0 -  option exists, is of correct type,
 *                                          holds a value at the position;
 *                                     -1 - not found;
 *                                     1 -  error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/04 (Oyranos: 0.1.10)
 *  @date    2009/05/04
 */
int            oyOptions_FindInt     ( oyOptions_s       * options,
                                       const char        * registration,
                                       int                 pos,
                                       int32_t           * result )
{
  int error = !options;
  oyOptions_s * s = options;
  oyOption_s * o = 0;

  if(!error)
    oyCheckType__m( oyOBJECT_OPTIONS_S, return error );

  if(error <= 0)
  {
    o = oyOptions_Find( options, registration, oyNAME_PATTERN );

    if(o && o->type_ == oyOBJECT_OPTION_S &&
       (oyOptionPriv_m(o)->value_type == oyVAL_INT ||
        oyOptionPriv_m(o)->value_type == oyVAL_INT_LIST))
    {
      if(result)
        *result = oyOption_GetValueInt( o, pos );
      error = 0;

    } else
      error = -1;

    oyOption_Release( &o );
  }

  return error;
}

/** Function oyOptions_SetFromInt
 *  @memberof oyOptions_s
 *  @brief   change a value
 *
 *  @param         obj                 the options list or set to manipulate
 *  @param         registration        the options registration name, e.g.
 *                                     "org/my_org/openicc/my_app/my_opt"
 *  @param         value               the value to set
 *  @param         pos                 the position in a value list
 *  @param         flags               can be OY_CREATE_NEW for a new option,
 *                                     OY_MATCH_KEY, OY_ADD_ALWAYS
 *  @return                            0 - success; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/04 (Oyranos: 0.1.10)
 *  @date    2009/05/04
 */
int            oyOptions_SetFromInt  ( oyOptions_s      ** obj,
                                       const char        * registration,
                                       int32_t             value,
                                       int                 pos,
                                       uint32_t            flags )
{
  int error = 0;
  oyOption_s * o = 0;
  oyOptions_s * s = obj ? *obj : NULL;

  if(!obj) return 1;
  if(s)
  { oyCheckType__m( oyOBJECT_OPTIONS_S, return 1 ) }

  if(error <= 0)
  {
    if(!*obj)
      *obj = oyOptions_New( 0 );

    if(flags & OY_MATCH_KEY)
    {
      const char * key = oyGetKeyFromRegistration( registration );
      o = oyOptions_Find( *obj, key, oyNAME_PATTERN );
    } else
      o = oyOptions_Find( *obj, registration, oyNAME_PATTERN );

    /** Add a new option if the OY_CREATE_NEW flag is present.
     */
    if((!o && oyToCreateNew_m(flags)) ||
        oyToAddAlways_m(flags))
    {
      o = oyOption_FromRegistration( registration, (*obj)->oy_ );
      error = !o;

      if(error <= 0)
        error = oyOption_SetFromInt( o, value, pos, 0 );

      oyOptions_MoveIn( (*obj), &o, -1 );

    } else
      oyOption_SetFromInt( o, value, pos, 0 );

    oyOption_Release( &o );
  }

  return error;
}

/** Function oyOptions_FindDouble
 *  @memberof oyOptions_s
 *  @brief   get a value
 *
 *  @param         options             the options list or set to manipulate
 *  @param         registration        the options registration name, e.g.
 *                                     "org/my_org/openicc/my_app/my_opt"
 *                                     or simple key, e.g. "my_opt"
 *  @param         result              the double
 *  @param         pos                 the value position
 *  @return                            0 -  option exists, is of correct type,
 *                                          holds a value at the position;
 *                                     -1 - not found;
 *                                     1 -  error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/04 (Oyranos: 0.1.10)
 *  @date    2009/09/10
 */
int            oyOptions_FindDouble  ( oyOptions_s       * options,
                                       const char        * registration,
                                       int                 pos,
                                       double            * result )
{
  int error = !options;
  oyOptions_s * s = options;
  oyOption_s_ * o = 0;

  if(!error)
    oyCheckType__m( oyOBJECT_OPTIONS_S, return error );

  if(error <= 0)
  {
    o = (oyOption_s_*)oyOptions_Find( options, registration,
                                      oyNAME_PATTERN );

    if(o && o->type_ == oyOBJECT_OPTION_S &&
       (o->value_type == oyVAL_DOUBLE ||
        o->value_type == oyVAL_DOUBLE_LIST))
    {
      if(result)
        *result = oyOption_GetValueDouble( (oyOption_s*)o, pos );
      if(o->value_type == oyVAL_DOUBLE_LIST && o->value->dbl_list[0] <= pos)
        error = -1;
      else
        error = 0;

    } else
      error = -1;

    oyOption_Release( (oyOption_s**)&o );
  }

  return error;
}

/** Function oyOptions_SetFromDouble
 *  @memberof oyOptions_s
 *  @brief   change a value
 *
 *  @param         obj                 the options list or set to manipulate
 *  @param         registration        the options registration name, e.g.
 *                                     "org/my_org/openicc/my_app/my_opt"
 *  @param         value               the value to set
 *  @param         pos                 the position in a value list
 *  @param         flags               can be OY_CREATE_NEW for a new option,
 *                                     OY_ADD_ALWAYS, OY_MATCH_KEY
 *  @return                            0 - success; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/04 (Oyranos: 0.1.10)
 *  @date    2009/09/10
 */
int            oyOptions_SetFromDouble(oyOptions_s      ** obj,
                                       const char        * registration,
                                       double              value,
                                       int                 pos,
                                       uint32_t            flags )
{
  int error = 0;
  oyOption_s * o = 0;
  oyOptions_s * s = obj ? *obj : NULL;

  if(!obj) return 1;
  if(s)
  { oyCheckType__m( oyOBJECT_OPTIONS_S, return 1 ) }

  if(error <= 0)
  {
    if(!*obj)
      *obj = oyOptions_New( 0 );

    if(flags & OY_MATCH_KEY)
    {
      const char * key = oyGetKeyFromRegistration( registration );
      o = oyOptions_Find( *obj, key, oyNAME_PATTERN );
    } else
      o = oyOptions_Find( *obj, registration, oyNAME_PATTERN );

    /** Add a new option if the OY_CREATE_NEW flag is present.
     */
    if((!o && oyToCreateNew_m(flags)) ||
        oyToAddAlways_m(flags))
    {
      o = oyOption_FromRegistration( registration, (*obj)->oy_ );
      error = !o;

      if(error <= 0)
        error = oyOption_SetFromDouble( o, value, pos, 0 );

      oyOptions_MoveIn( (*obj), &o, -1 );

    } else
      oyOption_SetFromDouble( o, value, pos, 0 );

    oyOption_Release( &o );
  }

  return error;
}

/** Function oyOptions_GetType2
 *  @memberof oyOptions_s
 *  @brief   select from options with special attributes
 *
 *  @param[in]     options             the options to scan
 *  @param[in]     pos                 the position if the result appears
 *                                     multiple times
 *  @param[in]     pattern             the registration pattern to search for;
 *                                     optional
 *  @param[in]     pattern_type        supported types of registration matching:
 *                                     - oyNAME_PATTERN for a pattern match, that is what most users prefer
 *                                     - oyNAME_REGISTRATION for a exact comparision
 *  @param[in]     object_type         the acceptable object type or oyOBJECT_NONE
 *  @param[out]    result              the found object; optional
 *  @param[out]    option              the belonging option; optional
 *  @return                            error
 *
 *  @see oyOptions_CountType()
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/07/04
 *  @since   2017/06/10 (Oyranos: 0.9.7)
 */
int            oyOptions_GetType2    ( oyOptions_s       * options,
                                       int                 pos,
                                       const char        * pattern,
                                       uint32_t            pattern_type,
                                       oyOBJECT_e          object_type,
                                       oyStruct_s       ** result,
                                       oyOption_s       ** option )
{
  int error = !options;
  int i, n, m = -1, found;
  oyOption_s_ * o = 0;
  oyOptions_s * s = options;
  oyStruct_s * st = 0;

  if(!error)
    oyCheckType__m( oyOBJECT_OPTIONS_S, return 0 );

    n = oyOptions_Count( options );
    for(i = 0; i < n;  ++i)
    {
      o = (oyOption_s_*)oyOptions_Get( options, i );
      found = 1;

      if(found && pattern &&
         !oyOptionRegistrationMatch( o->registration, pattern, pattern_type ))
          found = 0;

      if(found && object_type &&
         (o->value_type != oyVAL_STRUCT || !(o->value && o->value->oy_struct)))
        error = 1;

      if(found && object_type && !error &&
         (o->value_type != oyVAL_STRUCT || !o->value ||
          o->value->oy_struct->type_ != object_type))
        found = 0;

      if(found && !error)
      if(pos == -1 || ++m == pos)
      {
        if(result)
        {
          if(o->value->oy_struct->copy)
          {
            st = o->value->oy_struct->copy( o->value->oy_struct, 0 );
            if(oy_debug_objects >= 0 && st)
              oyObjectDebugMessage_( st->oy_, __func__,
                                     oyStructTypeToText(st->type_) );
          } else if(result)
            st = o->value->oy_struct;

          *result = st;
        }

        if(option)
          *option = (oyOption_s*)o;
        else
          oyOption_Release( (oyOption_s**)&o );
        break;
      }

      oyOption_Release( (oyOption_s**)&o );
    }

  return error;
}

/** Function oyOptions_GetType
 *  @memberof oyOptions_s
 *  @brief   select from options with special attribute
 *
 *  @see oyOptions_CountType()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/04 (Oyranos: 0.1.10)
 *  @date    2009/03/04
 */
oyStruct_s *   oyOptions_GetType     ( oyOptions_s       * options,
                                       int                 pos,
                                       const char        * pattern,
                                       oyOBJECT_e          type )
{
  oyStruct_s * st = 0;

  oyOptions_GetType2( options, pos, pattern, oyNAME_PATTERN, type, &st, NULL );

  return st;
}

/** oyOptions_MoveInStruct()
 *  @memberof oyOptions_s
 *  @brief   change a value
 *
 *  @param[in,out] obj                 the options list or set to manipulate
 *  @param[in]     registration        the options registration name, e.g.
 *                                     "org/my_org/openicc/my_app/my_opt"
 *  @param[in]     oy_struct           the Oyranos style object to move in
 *  @param[in]     flags               can be OY_CREATE_NEW for a new option,
 *                                     OY_ADD_ALWAYS, OY_MATCH_KEY
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2009/03/05
 */
int            oyOptions_MoveInStruct( oyOptions_s      ** obj,
                                       const char        * registration,
                                       oyStruct_s       ** oy_struct,
                                       uint32_t            flags )
{
  int error = !obj ||
              (obj && *obj && (*obj)->type_ != oyOBJECT_OPTIONS_S) ||
              !registration ||
              !oy_struct;
  oyOption_s * o = 0;

  if(error <= 0)
  {
    if(!*obj)
      *obj = oyOptions_New( 0 );

    if(flags & OY_MATCH_KEY)
    {
      const char * key = oyGetKeyFromRegistration( registration );
      o = oyOptions_Find( *obj, key, oyNAME_PATTERN );
    } else
      o = oyOptions_Find( *obj, registration, oyNAME_PATTERN );

    /** Add a new option if the OY_CREATE_NEW flag is present.
     */
    if((!o && oyToCreateNew_m(flags)) ||
        oyToAddAlways_m(flags))
    {
      oyOption_Release( &o );

      o = oyOption_FromRegistration( registration, (*obj)->oy_ );
      error = !o;

      if(error <= 0)
        error = oyOption_MoveInStruct( o, oy_struct );

      if(error <= 0)
        error = oyOptions_MoveIn( (*obj), &o, -1 );
    }

    if(error <= 0 && o && *oy_struct)
      error = oyOption_MoveInStruct( o, oy_struct );
    oyOption_Release( &o );
  }

  return error;
}

/** Function oyOptions_SetFromData
 *  @memberof oyOptions_s
 *  @brief   set a data blob or plain pointer
 *
 *  @param         options             the options list or set to manipulate
 *  @param         registration        the options registration name, e.g.
 *                                     "org/my_org/openicc/my_app/my_opt"
 *  @param         ptr                 the pointer
 *  @param         size                the pointer size
 *  @param         flags               can be OY_CREATE_NEW for a new option,
 *                                     OY_ADD_ALWAYS, OY_MATCH_KEY
 *  @return                            0 - success; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/07 (Oyranos: 0.1.10)
 *  @date    2009/11/07
 */
int            oyOptions_SetFromData ( oyOptions_s      ** options,
                                       const char        * registration,
                                       oyPointer           ptr,
                                       size_t              size,
                                       uint32_t            flags )
{
  int error = !options;
  oyOption_s * o = 0;
  oyOptions_s * s = options ? *options : NULL;

  if(!options) return 1;
  if(s)
  { oyCheckType__m( oyOBJECT_OPTIONS_S, return 1 ) }

  if(error <= 0)
  {
    if(!*options)
      *options = oyOptions_New( 0 );

    if(flags & OY_MATCH_KEY)
    {
      const char * key = oyGetKeyFromRegistration( registration );
      o = oyOptions_Find( *options, key, oyNAME_PATTERN );
    } else
      o = oyOptions_Find( *options, registration, oyNAME_PATTERN );

    /** Add a new option if the OY_CREATE_NEW flag is present.
     */
    if((!o && oyToCreateNew_m(flags)) ||
        oyToAddAlways_m(flags))
    {
      o = oyOption_FromRegistration( registration, (*options)->oy_ );
      error = !o;

      if(error <= 0)
        error = oyOption_SetFromData( o, ptr, size );

      oyOptions_MoveIn( *options, &o, -1 );

    } else
      oyOption_SetFromData( o, ptr, size );

    oyOption_Release( &o );
  }

  return error;
}

/** Function oyOptions_FindData
 *  @memberof oyOptions_s
 *  @brief   get a value
 *
 *  @param         options             the options list or set to manipulate
 *  @param         registration        the options registration name, e.g.
 *                                     "org/my_org/openicc/my_app/my_opt"
 *                                     or simply a key, e.g. "my_opt"
 *  @param[out]    result              the data; With size == zero, the pointer
 *                                     is static and owned somewhere else.
 *                                     With size set, the pointer is owned my
 +                                     the caller.
 *  @param[out]    size                the data size
 *  @param[in]     allocateFunc        user allocator
 *  @return                            0 -  option exists, is of correct type,
 *                                          holds a value;
 *                                     -1 - not found;
 *                                     1 -  error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/07 (Oyranos: 0.1.10)
 *  @date    2009/11/07
 */
int            oyOptions_FindData    ( oyOptions_s       * options,
                                       const char        * registration,
                                       oyPointer         * result,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  int error = !options;
  oyOptions_s * s = options;
  oyOption_s * o = 0;

  if(!error)
    oyCheckType__m( oyOBJECT_OPTIONS_S, return error );

  if(error <= 0)
  {
    o = oyOptions_Find( options, registration,
                                      oyNAME_PATTERN );

    if(o && o->type_ == oyOBJECT_OPTION_S &&
       oyOptionPriv_m(o)->value_type == oyVAL_STRUCT)
    {
      if(result)
        *result = oyOption_GetData( o, size, allocateFunc );
      error = 0;

    } else
      error = -1;

    oyOption_Release( &o );
  }

  return error;
}

/** Function oyOptions_SetSource
 *  @memberof oyOptions_s
 *  @brief   set source attribute
 *
 *  @param         options             the options list or set to manipulate
 *  @param         source              the options new source attribute
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/27 (Oyranos: 0.1.9)
 *  @date    2009/01/27
 */
int            oyOptions_SetSource   ( oyOptions_s       * options,
                                       oyOPTIONSOURCE_e    source )
{
  int error = !(options && options->type_ == oyOBJECT_OPTIONS_S);
  int i,n;
  oyOption_s * o = 0;

  if(error <= 0)
  {
    n = oyOptions_Count( options );
    for(i = 0; i < n; ++i)
    {
      o = oyOptions_Get( options, i );
      oyOptionPriv_m(o)->source = source;
      oyOption_Release( &o );
    }
  }

  return error;
}


/** Function oyOptions_ObserverAdd
 *  @memberof oyOptions_s
 *  @brief   observe options and its elements
 *
 *  @param[in]     object              the options
 *  @param[in]     observer            the in observation intereressted object
 *  @param[in]     user_data           additional static informations
 *  @param[in]     signalFunc          the signal handler
 *  @return                            0 - fine; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/28 (Oyranos: 0.1.10)
 *  @date    2009/10/28
 */
OYAPI int  OYEXPORT
               oyOptions_ObserverAdd ( oyOptions_s       * object,
                                       oyStruct_s        * observer,
                                       oyStruct_s        * user_data,
                                       oyObserver_Signal_f signalFunc )
{
  int error = 0;

  error = oyStructList_ObserverAdd( oyOptionsPriv_m(object)->list_,
                                    (oyStruct_s*) object,
                                    user_data, 0 );
  error = oyStruct_ObserverAdd( (oyStruct_s*)object, observer,
                                user_data, signalFunc );
  return error;
}

/** Function oyOptions_ObserversDisable
 *  @memberof oyOptions_s
 *  @brief   disable signaling
 *
 *  @param[in]     object              the options
 *  @return                            0 - fine; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/04/07 (Oyranos: 0.1.10)
 *  @date    2010/04/07
 */
OYAPI int  OYEXPORT
               oyOptions_ObserversDisable (
                                       oyOptions_s       * object )
{
  int error = 0;
    oyOption_s * o;
    int i,n = oyOptions_Count( object );
    for(i = 0; i < n; ++i)
    {
      o = oyOptions_Get( object, i );
      oyStruct_DisableSignalSend( (oyStruct_s*)o );
      oyOption_Release( &o );
    }
  return error;
}

/** Function oyOptions_ObserversEnable
 *  @memberof oyOptions_s
 *  @brief   disable signaling
 *
 *  @param[in]     object              the options
 *  @return                            0 - fine; 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/04/07 (Oyranos: 0.1.10)
 *  @date    2010/04/07
 */
OYAPI int  OYEXPORT
               oyOptions_ObserversEnable (
                                       oyOptions_s       * object )
{
  int error = 0;
    oyOption_s * o;
    int i,n = oyOptions_Count( object );
    for(i = 0; i < n; ++i)
    {
      o = oyOptions_Get( object, i );
      oyStruct_EnableSignalSend( (oyStruct_s*)o );
      oyOption_Release( &o );
    }
  return error;
}

/** Function oyOptions_SetDriverContext
 *  @memberof oyOptions_s
 *  @brief   set a device option from a given external context
 *
 *  The options will be created in case they do not exist. The
 *  driver_context_type accepts "xml". The data in driver_context will be
 *  converted to a options set following the Oyranos options XML schemes with
 *  the help oyOptions_FromText().
 *  Any other pointer will be converted to a oyBlob_s object. The name of that
 *  object will come from driver_context_type.
 *
 *  @deprecated will be removed from v1.0; migrate to oyOptions_FromJSON().
 *
 *  @param[in,out] options             options for the device
 *  @param[in]     driver_context      driver context
 *  @param[in]     driver_context_type "xml" or something related to the driver
 *  @param[in]     driver_context_size size of driver_context
 *  @param[in]     object              a optional object
 *  @return                            1 - error; 0 - success; -1 - otherwise
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/18 (Oyranos: 0.1.10)
 *  @date    2009/05/18
 */
OYAPI int  OYEXPORT
           oyOptions_SetDriverContext( oyOptions_s      ** options,
                                       oyPointer           driver_context,
                                       const char        * driver_context_type,
                                       size_t              driver_context_size,
                                       oyObject_s          object )
{
  int error = !options || !driver_context_type,
      l_error = 0;
  oyOptions_s * s = 0,
              * opts_tmp = 0;
  oyOption_s * o = 0;
  char * key = 0;

  if(options && *options)
  {
    s = *options;
    oyCheckType__m( oyOBJECT_OPTIONS_S, return 1 );
  }

  if(!error)
  {
    if(!s)
      s = oyOptions_New( 0 );
    error = !s;

    key = oyStringAppend_( "driver_context.", driver_context_type,
                           oyAllocateFunc_ );
  }

  if(!error)
  {
    o = oyOption_FromRegistration( key, object );

    if(oyFilterRegistrationMatch( driver_context_type, "xml", 0 ))
    {
      opts_tmp = oyOptions_FromText( (char*)driver_context, 0, object );
      error = oyOption_MoveInStruct ( o, (oyStruct_s**) &opts_tmp );
    }
    else
      error = oyOption_SetFromData( o, driver_context, driver_context_size );

    if(error <= 0)
    { l_error = oyOptions_MoveIn( s, &o, -1 ); OY_ERR }

    oyFree_m_( key );
  }

  if(!error)
    *options = s;
  else
    oyOptions_Release( &s );

  return error;
}

/* } Include "Options.public_methods_definitions.c" */

