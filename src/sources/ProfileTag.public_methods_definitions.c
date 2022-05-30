/** Function  oyProfileTag_Create
 *  @memberof oyProfileTag_s
 *  @brief    Create a oyProfileTag_s from a list of arguments
 *
 *  The API relies on an generic arguments inside a list. The arguments are not
 *  specified here but in the appropriate modules. This allows flexibility,
 *  which needs more understanding.
 *
 *  For the effect of the parameters look at the appropriate module
 *  documentation and the function infos.
 *
 *  @see @ref profile_modules
 *
 *  @param[in]     list                a list of arguments
 *  @param[in]     tag_use             purpose of the tag
 *  @param[in]     tag_type            type to create
 *  @param[in]     version             version as supported
 *  @param[in]     object              the user object for the tag creation
 *  @return                            a profile tag
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/01/26
 *  @since   2008/01/08 (Oyranos: 0.1.8)
 */
OYAPI oyProfileTag_s * OYEXPORT
               oyProfileTag_Create   ( oyStructList_s    * list,
                                       icTagSignature      tag_use,
                                       icTagTypeSignature  tag_type,
                                       uint32_t            version,
                                       oyObject_s          object)
{
  oyProfileTag_s * s = 0, * tag = 0;
  int error = !list;
  oyCMMProfileTag_Create_f funcP = 0;
  oyCMMapiQuery_s query = {oyQUERY_PROFILE_TAG_TYPE_WRITE, 0, oyREQUEST_HARD};
  oyCMMapiQuery_s *query_[2] = {0,0};
  oyCMMapiQueries_s queries = {1,0,{0}};

  if(error <= 0 && list->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(error <= 0)
  {
    query.value = tag_type;
    query_[0] = &query;
    queries.queries = query_;
  }

  if(error <= 0)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API3_S, 0,
                                      (oyCMMapi_Check_f)oyCMMapi3_Query_, &queries );
    if(api)
    {
      oyCMMapi3_s_ * api3 = (oyCMMapi3_s_*) api;
      funcP = api3->oyCMMProfileTag_Create;
    }
    error = !funcP;
  }

  if(error <= 0)
  {
    tag = oyProfileTag_New( object );
    error = !tag;
    
    if(error <= 0)
      error = funcP( tag, list, tag_type, version );

    if(error <= 0)
      oyProfileTagPriv_m(tag)->use = tag_use;

    if(error <= 0)
      s = tag;
  }

  return s;
}

/** Function  oyProfileTag_CreateFromText
 *  @memberof oyProfileTag_s
 *  @brief    Create a oyProfileTag_s from a string
 *
 *  @param[in]     text                a string
 *  @param[in]     tag_type            type to create, e.g. icSigTextDescriptionType or icSigTextType
 *  @param[in]     tag_usage           signature, e.g. icSigCopyrightTag
 *  @param[in]     object              the user object for the tag creation
 *  @return                            a profile tag
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/06 (Oyranos: 0.1.10)
 *  @date    2009/01/06
 */
OYAPI oyProfileTag_s * OYEXPORT
               oyProfileTag_CreateFromText (
                                       const char        * text,
                                       icTagTypeSignature  tag_type,
                                       icTagSignature      tag_usage,
                                       oyObject_s          object )
{
  int error = !text;
  oyProfileTag_s * tag = 0;
  oyStructList_s * list = 0;

  if(error <= 0)
  {
    list = oyStructList_Create( oyOBJECT_NONE, "oyProfileTag_CreateFromText", 0);
    error = !list;
  }

  if(error <= 0)
  {
    oyStructList_AddName( list, text, 0, oyNAME_NAME );
    error = oyStructList_AddName( list, "en_GB", 0, oyNAME_LC );
  }

  if(error <= 0)
  {
    tag = oyProfileTag_Create( list, tag_usage, tag_type, 0, object);
    error = !tag;
  }

  oyStructList_Release( &list );

  return tag;
}

/** Function  oyProfileTag_CreateFromData
 *  @memberof oyProfileTag_s
 *  @brief    Create a oyProfileTag_s from some data
 *
 *  @param[in]     sig                 usage signature
 *  @param[in]     type                content type
 *  @param[in]     status              to be set
 *  @param[in]     tag_size            memory size of tag_block
 *  @param[in]     tag_block           the to be copied memory
 *  @param[in]     object              the user object for the tag creation
 *  @return                            a profile tag
 *
 *  @version Oyranos: 0.3.1
 *  @since   2011/05/13 (Oyranos: 0.3.1)
 *  @date    2011/05/13
 */
OYAPI oyProfileTag_s * OYEXPORT
               oyProfileTag_CreateFromData ( 
                                       icTagSignature      sig,
                                       icTagTypeSignature  type,
                                       oySTATUS_e          status,
                                       size_t              tag_size,
                                       oyPointer           tag_block,
                                       oyObject_s          object )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)oyProfileTag_New(object);
  int error = !s;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 0 )

  if(error <= 0)
  {
    s->use = sig;
    s->tag_type_ = type;
    s->status_ = status;
    s->size_ = tag_size;
    if(s->size_)
    {
      oyStruct_AllocHelper_m_( s->block_, char, tag_size, s, return 0 );
      memcpy( s->block_, tag_block, tag_size );
    }
  }

  return (oyProfileTag_s*)s;
}

/** Function  oyProfileTag_Set
 *  @memberof oyProfileTag_s
 *  @brief    The function is a simple setter for the object elements.
 *
 *  @param[in,out] tag                 the to be manipulated ICC profile object
 *  @param[in]     sig                 usage signature
 *  @param[in]     type                content type
 *  @param[in]     status              to be set
 *  @param[in]     tag_size            memory size of tag_block
 *  @param[in]     tag_block           the block to be moved into the object;
 *                                     The pointer is owned by the object. Its
 *                                     memory should be allocated as with the
 *                                     same allocators as the object.
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/10/07
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 */
OYAPI int  OYEXPORT
                   oyProfileTag_Set  ( oyProfileTag_s    * tag,
                                       icTagSignature      sig,
                                       icTagTypeSignature  type,
                                       oySTATUS_e          status,
                                       size_t              tag_size,
                                       oyPointer         * tag_block )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;
  int error = !s;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 1 )

  if(error <= 0)
  {
    s->use = sig;
    s->tag_type_ = type;
    s->status_ = status;
    s->size_ = tag_size;
    if(s->block_)
      s->oy_->deallocateFunc_( s->block_ );
    s->block_ = *tag_block;
    if(s->block_)
      *((uint32_t*)s->block_) = oyValueUInt32( type );
    *tag_block = NULL;
  }

  return error;
}

/** oyProfileTag_Get()
 *  @brief    obtain parsed data and describing information from a tag
 *  @memberof oyProfileTag_s
 *
 *  Compile time supported tags are listed here @ref profile_modules .
 *
 *  @param[in]     tag                 the tag to read
 *  @return                            a list of strings
 *
 *  @version Oyranos: 0.3.1
 *  @since   2008/06/19 (Oyranos: 0.1.8)
 *  @date    2008/05/17
 */
oyStructList_s *   oyProfileTag_Get  ( oyProfileTag_s    * tag )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;
  int error = !s;
  oyCMMProfileTag_GetValues_f funcP = 0;
  char cmm[] = {0,0,0,0,0};
  oyStructList_s * values = 0;
  oyCMMapiQuery_s query = {oyQUERY_PROFILE_TAG_TYPE_READ, 0, oyREQUEST_HARD};
  oyCMMapiQuery_s *query_[2] = {0,0};
  oyCMMapiQueries_s queries = {1,0,{0}};
  const char * sig, * tag_type; /* use for debugging */

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 0 )

  if(error <= 0)
  {
    query.value = s->tag_type_;
    query_[0] = &query;
    queries.queries = query_;
    error = !memcpy( queries.prefered_cmm, s->profile_cmm_, 4 );
    sig = oyICCTagName(s->use);
    tag_type = oyICCTagTypeName(s->tag_type_);
  }

  if(error <= 0)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API3_S, 0,
                                      (oyCMMapi_Check_f)oyCMMapi3_Query_, &queries );
    if(api)
    {
      oyCMMapi3_s_ * api3 = (oyCMMapi3_s_*) api;
      funcP = api3->oyCMMProfileTag_GetValues;
    }
    error = !funcP;
  }

  if(error <= 0)
  {
    values = funcP( tag );

    error = !memcpy( s->last_cmm_, cmm, 4 );
  }

  sig = tag_type = NULL;

  return values;
}

#include <ctype.h> /* isalpha() */

/** Function oyProfileTag_GetText
 *  @memberof oyProfileTag_s
 *
 *  For the effect of the parameters look at the appropriate module.
 *  @see oyIMProfileTag_GetValues
 *
 *  For localised strings, e.g. icSigMultiLocalizedUnicodeType: \n
 *    - zero language and country args: all localisation strings are returned 
 *    - with language and/or country args: return appropriate matches
 *    - for language != "", the string starts with language code, the text follows after a colon ":"
 *    - a non zero but empty language argument == "", 
 *      returns the pure string in the actual Oyranos locale, no language code
 *
 *  @param[in]     tag                 the tag to read
 *  @param[out]    n                   the number of returned strings
 *  @param[in]     language            2 byte language code, or "" for current
 *  @param[in]     country             2 byte country code
 *  @param[out]    tag_size            the processed tag size
 *  @param[in]     allocateFunc        the user allocator for the returned list
 *  @return                            a list of strings
 *
 *  @version Oyranos: 0.9.5
 *  @since   2008/01/03 (Oyranos: 0.1.8)
 *  @date    2014/01/18
 */
char **        oyProfileTag_GetText  ( oyProfileTag_s    * tag,
                                       int32_t           * n,
                                       const char        * language,
                                       const char        * country,
                                       int32_t           * tag_size,
                                       oyAlloc_f           allocateFunc )
{
  oyProfileTag_s * s = tag;
  int error = !s;
  char t_l[8] = {0,0,0,0,0,0,0,0}, t_c[8] = {0,0,0,0,0,0,0,0}, *t_ptr;
  int implicite_i18n = 0;
  char ** texts = 0, * text_tmp = 0, * temp = 0;
  const char * text = NULL;
  oyStructList_s * values = 0;
  oyBlob_s * blob = 0;
  const char * name = NULL;
  const char * lang = NULL;
  size_t size = 0;
  int values_n = 0, i = 0, k;
  int texts_n = 0;

  *n = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 0 )

  if(error <= 0)
  {
    values = oyProfileTag_Get( tag );

    /* check for a "" in the lang variable -> want the best i18n match */
    if(language && !language[0])
    {
      implicite_i18n = 1;
      language = oyLanguage() ? oyLanguage() : "";
      country  = oyCountry() ? oyCountry() : "";
    }

    if(!allocateFunc)
      allocateFunc = oyAllocateFunc_;

    if(oyStructList_Count( values ) )
      {
        blob = 0;
        values_n = oyStructList_Count( values );

        for(k = 0; k < 4; ++k)
        {
          for(i = 0; i < values_n; ++i)
          {
            name = oyStructList_GetName( values, i, oyNAME_NAME );
            if(!name)
            blob = (oyBlob_s*) oyStructList_GetRefType( values, i,
                                                        oyOBJECT_BLOB_S );
            lang = oyStructList_GetName( values, i, oyNAME_LC );

            text = 0;
            if(name)
            {
              int len = lang ? strlen(lang) : 0;
              if(len)
                memcpy(t_l, lang, len<=8 ? len : 8);
              t_c[0] = 0;
              t_ptr = oyStrchr_(t_l, '_');
              if(t_ptr)
              {
                memcpy(t_c, t_ptr+1, 3);
                *t_ptr = 0;
              }
            }

            if(name)
              text = name;
            else if(blob && oyBlob_GetPointer(blob) && oyBlob_GetSize(blob))
            {
              error = oyStringFromData_( oyBlob_GetPointer(blob),
                                         oyBlob_GetSize(blob), &text_tmp,
                      &size, oyAllocateFunc_ );
              if(error <= 0 && size && text_tmp)
                text = text_tmp;
            }

            /* select by language and/or country or best i18n match or all */
            if(
               (k == 0 && language && language[0] &&
                          oyStrcmp_( language, t_l ) == 0 &&
                          country  && country[0] &&
                          oyStrcmp_( country, t_c ) == 0 )              ||
               (k == 1 && language && language[0] &&
                          oyStrcmp_( language, t_l ) == 0 &&
                          (!country || implicite_i18n ))                ||
               (k == 2 && country  && country[0] &&
                          oyStrcmp_( country, t_c ) == 0  &&
                          (!language || implicite_i18n ))               ||
               (k == 3 && ((!language && !country) || implicite_i18n))
              )
            {
              if(name && lang && isalpha(lang[0]) && !implicite_i18n)
              {
                /* string with i18n infos -> "de_DE:Licht" */
                oyjlStringAdd( &temp, oyAllocateFunc_, oyDeAllocateFunc_, "%s:%s", lang, text);
                oyjlStringListAddString( &texts, &texts_n, temp,
                                            oyAllocateFunc_, oyDeAllocateFunc_);
                oyFree_m_(temp);

              } else {
                /* pure string -> "Licht" */
                oyStringListAddStaticString( &texts, &texts_n, text,
                                             oyAllocateFunc_, oyDeAllocateFunc_);
                /* no selection for best i18n match and no lang: take all */
                if(k == 3 && implicite_i18n)
                {
                  implicite_i18n = 0;
                  language = 0;
                  country = 0;
                }
              }
            }

            /* best i18n match found -> end */
            if(implicite_i18n && texts_n)
            {
              k = 4;
              break;
            }

            if(text_tmp)
              oyFree_m_( text_tmp );
          }
        }

        *n = texts_n;
      }

    if(tag_size)
      *tag_size = ((oyProfileTag_s_*)s)->size_;

    oyStructList_Release( &values );
  }

  return texts;
}

/** Function  oyProfileTag_GetBlock
 *  @memberof oyProfileTag_s
 *  @brief    Get the raw memory block of the tag.
 *
 *  @param[in]     tag                 the tag to read
 *  @param[out]    tag_block           the raw data owned by the user; on success the block if it has a size; else undefined; optional
 *  @param[out]    tag_size            the data size; on success the size returned in tag_block else undefined
 *  @param[in]     allocateFunc        the user allocator, optional
 *  @return                            0 - success, >= 1 - error
 *
 *  @version Oyranos: 0.2.0
 *  @since   2010/01/31 (Oyranos: 0.2.0)
 *  @date    2010/06/31
 */
int            oyProfileTag_GetBlock ( oyProfileTag_s    * tag,
                                       oyPointer         * tag_block,
                                       size_t            * tag_size,
                                       oyAlloc_f           allocateFunc )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;
  int error = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 1 )

  if(error <= 0)
  {
    if(!allocateFunc)
      allocateFunc = oyAllocateFunc_;

    if(s->size_ && s->block_ && tag_block)
    {
      *tag_block = allocateFunc( s->size_ + 1 );
      memcpy( *tag_block, s->block_, s->size_ );
    }
    if(tag_size)
      *tag_size = s->size_;
  }

  return error;
}

/** Function  oyProfileTag_GetOffset
 *  @memberof oyProfileTag_s
 *  @brief    Get the offset of the tag in the profile.
 *
 *  @param[in]     tag                 the tag to read
 *  @return                            offset
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
size_t         oyProfileTag_GetOffset( oyProfileTag_s    * tag )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 0 )

  return s->offset_orig;
}
/** Function  oyProfileTag_SetOffset
 *  @memberof oyProfileTag_s
 *  @brief    Set the offset of the tag in the profile.
 *
 *  @param[in,out] tag                 the tag to read
 *  @param[in]     offset              new offset
 *  @return                            0 - success, >= 1 - error
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
int            oyProfileTag_SetOffset( oyProfileTag_s    * tag,
                                       size_t              offset )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 1 )

  s->offset_orig = offset;

  return 0;
}
/** Function  oyProfileTag_GetStatus
 *  @memberof oyProfileTag_s
 *  @brief    Get the status.
 *
 *  @param[in]     tag                 the tag to read
 *  @return                            status
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
oySTATUS_e     oyProfileTag_GetStatus( oyProfileTag_s    * tag )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;

  if(!s)
    return oyUNDEFINED;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return oyCORRUPTED )

  return s->status_;
}
/** Function  oyProfileTag_SetStatus
 *  @memberof oyProfileTag_s
 *  @brief    Set the status.
 *
 *  @param[in,out] tag                 the tag to read
 *  @param[in]     status              new status
 *  @return                            0 - success, >= 1 - error
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
int            oyProfileTag_SetStatus( oyProfileTag_s    * tag,
                                       oySTATUS_e          status )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 1 )

  s->status_ = status;

  return 0;
}

/** Function  oyProfileTag_GetCMM
 *  @memberof oyProfileTag_s
 *  @brief    Get the profile cmm of the tag.
 *
 *  @param[in,out] tag                 the tag to read
 *  @return                            cmm string
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI const char *  OYEXPORT
               oyProfileTag_GetCMM   ( oyProfileTag_s    * tag )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 0 )

  return s->profile_cmm_;
}
/** Function  oyProfileTag_SetCMM
 *  @memberof oyProfileTag_s
 *  @brief    Set the profile CMM in the tag.
 *
 *  @param[in,out] tag                 the tag to read
 *  @param[in]     profile_cmm         new cmm, 4 bytes
 *  @return                            0 - success, >= 1 - error
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI int  OYEXPORT
               oyProfileTag_SetCMM   ( oyProfileTag_s    * tag,
                                       const char        * profile_cmm )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;
  int error = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 1 )

  if(profile_cmm)
    error = !memcpy( s->profile_cmm_, profile_cmm, 4 );

  return error;
}
/** Function  oyProfileTag_GetLastCMM
 *  @memberof oyProfileTag_s
 *  @brief    Get the last processing cmm of the tag.
 *
 *  @param[in,out] tag                 the tag to read
 *  @return                            cmm string
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI const char *  OYEXPORT
               oyProfileTag_GetLastCMM(oyProfileTag_s    * tag )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 0 )

  return s->last_cmm_;
}
/** Function  oyProfileTag_SetLastCMM
 *  @memberof oyProfileTag_s
 *  @brief    Set the last processing CMM.
 *
 *  @param[in,out] tag                 the tag to read
 *  @param[in]     cmm                 CMM, 4 bytes
 *  @return                            0 - success, >= 1 - error
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI int  OYEXPORT
               oyProfileTag_SetLastCMM(oyProfileTag_s    * tag,
                                       const char        * cmm )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;
  int error = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 1 )

  if(cmm)
    error = !memcpy( s->last_cmm_, cmm, 4 );

  return error;
}
/** Function  oyProfileTag_GetRequiredCMM
 *  @memberof oyProfileTag_s
 *  @brief    Get the certainly selected CMM.
 *
 *  @param[in,out] tag                 the tag to read
 *  @return                            cmm string
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI const char *  OYEXPORT
               oyProfileTag_GetRequiredCMM
                                     ( oyProfileTag_s    * tag )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 0 )

  return s->required_cmm;
}
/** Function  oyProfileTag_SetRequiredCMM
 *  @memberof oyProfileTag_s
 *  @brief    Require a certain CMM.
 *
 *  @param[in,out] tag                 the tag to read
 *  @param[in]     cmm                 CMM, 4 bytes
 *  @return                            0 - success, >= 1 - error
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI int  OYEXPORT
               oyProfileTag_SetRequiredCMM
                                     ( oyProfileTag_s    * tag,
                                       const char        * cmm )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;
  int error = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 1 )

  if(cmm)
    error = !memcpy( s->required_cmm, cmm, 4 );

  return error;
}
/** Function  oyProfileTag_GetUse
 *  @memberof oyProfileTag_s
 *  @brief    Get the usage signature.
 *
 *  @param[in,out] tag                 the tag to read
 *  @return                            use signature
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI icTagSignature  OYEXPORT
               oyProfileTag_GetUse   ( oyProfileTag_s    * tag )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 0 )

  return s->use;
}
#if 0
/** Function  oyProfileTag_SetUse
 *  @memberof oyProfileTag_s
 *  @brief    Set the usage signature.
 *
 *  @param[in,out] tag                 the tag to read
 *  @param[in]     use                 usage signature
 *  @return                            0 - success, >= 1 - error
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI int  OYEXPORT
               oyProfileTag_SetUse   ( oyProfileTag_s    * tag,
                                       icTagSignature      use )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;
  int error = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 1 )

  s->use = use;

  return error;
}
#endif

/** Function  oyProfileTag_GetType
 *  @memberof oyProfileTag_s
 *  @brief    Get the type signature.
 *
 *  @param[in,out] tag                 the tag to read
 *  @return                            type signature
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/24 (Oyranos: 0.5.0)
 *  @date    2012/09/24
 */
OYAPI icTagTypeSignature  OYEXPORT
               oyProfileTag_GetType  ( oyProfileTag_s    * tag )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 1 )

  return s->tag_type_;
}
/** Function  oyProfileTag_GetSize
 *  @memberof oyProfileTag_s
 *  @brief    Get the data size.
 *
 *  @param[in]     tag                 the tag to read
 *  @return                            size
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/05 (Oyranos: 0.5.0)
 *  @date    2012/09/05
 */
OYAPI size_t  OYEXPORT
               oyProfileTag_GetSize  ( oyProfileTag_s    * tag )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 1 )

  return s->size_;
}
/** Function  oyProfileTag_GetSizeCheck
 *  @memberof oyProfileTag_s
 *  @brief    Get the data check size.
 *
 *  @param[in]     tag                 the tag to read
 *  @return                            check size
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/24 (Oyranos: 0.5.0)
 *  @date    2012/09/24
 */
OYAPI size_t  OYEXPORT
               oyProfileTag_GetSizeCheck
                                     ( oyProfileTag_s    * tag )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 1 )

  return s->size_;
}
/** Function  oyProfileTag_SetSizeCheck
 *  @memberof oyProfileTag_s
 *  @brief    Set the data check size.
 *
 *  @param[in,out] tag                 the tag to read
 *  @param[in]     size_check          check size
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/24 (Oyranos: 0.5.0)
 *  @date    2012/09/24
 */
OYAPI void  OYEXPORT
               oyProfileTag_SetSizeCheck (
                                       oyProfileTag_s    * tag,
                                       size_t              size_check )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;

  if(!s)
    return;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return )

  s->size_check_ = size_check;
}

