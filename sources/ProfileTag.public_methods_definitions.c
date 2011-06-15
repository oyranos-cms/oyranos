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
 *  @see oyraProfileTag_Create
 *  @see oyraFunctionGetInfo
 *
 *  @param[in]     list                a list of arguments
 *  @param[in]     tag_type            type to create
 *  @param[in]     version             version as supported
 *  @param[in,out] required_cmm        in: CMM to create the tag; out: used CMM
 *  @param[in]     object              the user object for the tag creation
 *  @return                            a profile tag
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/08 (Oyranos: 0.1.8)
 *  @date    2008/01/08
 */
OYAPI oyProfileTag_s * OYEXPORT
               oyProfileTag_Create   ( oyStructList_s    * list,
                                       icTagTypeSignature  tag_type,
                                       uint32_t            version,
                                       const char        * required_cmm,
                                       oyObject_s          object)
{
  oyProfileTag_s * s = 0, * tag = 0;
  int error = !list;
  oyCMMProfileTag_Create_f funcP = 0;
  char cmm[] = {0,0,0,0,0};
  oyCMMapiQuery_s query = {oyQUERY_PROFILE_TAG_TYPE_WRITE, 0, oyREQUEST_HARD};
  oyCMMapiQuery_s *query_[2] = {0,0};
  oyCMMapiQueries_s queries = {1,0};

  if(error <= 0 && list->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(error <= 0)
  {
    query.value = tag_type;
    query_[0] = &query;
    queries.queries = query_;
    if(required_cmm)
      error = !memcpy( queries.prefered_cmm, required_cmm, 4 ); 

    if(error <= 0 && required_cmm)
      error = !memcpy( cmm, required_cmm, 4 );
  }

  if(error <= 0)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API3_S, cmm, 0,
                                      oyCMMapi3_Query_, &queries );
    if(api)
    {
      oyCMMapi3_s * api3 = (oyCMMapi3_s*) api;
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
      error = !memcpy( oyProfileTagPriv_m(tag)->last_cmm_, cmm, 4 );
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
  oyName_s * name = 0;
  oyStructList_s * list = 0;

  if(error <= 0)
  {
    name = oyName_set_ ( name, text, oyNAME_NAME,
                         oyAllocateFunc_, oyDeAllocateFunc_ );
    error = !name;
  }

  if(error <= 0)
  {
    memcpy( name->lang, "en_GB", 5 );
    list = oyStructList_New(0);
    error = oyStructList_MoveIn( list, (oyStruct_s**) &name, 0,
                                 OY_OBSERVE_AS_WELL );
  }

  if(error <= 0)
  {
    tag = oyProfileTag_Create( list, tag_type, 0, OY_MODULE_NICK, object);
    error = !tag;
  }

  if(error <= 0)
    oyProfileTagPriv_m(tag)->use = tag_usage;

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
      oyAllocHelper_m_( s->block_, char, tag_size, s->oy_->allocateFunc_,
                        return 0 );
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
 *  @version Oyranos: 0.1.10
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 *  @date    2009/11/06
 */
OYAPI int  OYEXPORT
                   oyProfileTag_Set  ( oyProfileTag_s    * tag,
                                       icTagSignature      sig,
                                       icTagTypeSignature  type,
                                       oySTATUS_e          status,
                                       size_t              tag_size,
                                       oyPointer           tag_block )
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
    s->block_ = tag_block;
  }

  return error;
}

/** Function oyProfileTag_Get
 *  @memberof oyProfileTag_s
 *
 *  Hint: to select a certain module use the oyProfileTag_s::required_cmm
 *  element from the tag parameter.
 *
 *  @param[in]     tag                 the tag to read
 *  @return                            a list of strings
 *
 *  @version Oyranos: 0.3.1
 *  @since   2008/06/19 (Oyranos: 0.1.8)
 *  @date    2008/05/17
 */
oyStructList_s*oyProfileTag_Get      ( oyProfileTag_s    * tag )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*)tag;
  int error = !s;
  oyCMMProfileTag_GetValues_f funcP = 0;
  char cmm[] = {0,0,0,0,0};
  oyStructList_s * values = 0;
  oyCMMapiQuery_s query = {oyQUERY_PROFILE_TAG_TYPE_READ, 0, oyREQUEST_HARD};
  oyCMMapiQuery_s *query_[2] = {0,0};
  oyCMMapiQueries_s queries = {1,0};

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_TAG_S, return 0 )

  if(error <= 0)
  {
    query.value = s->tag_type_;
    query_[0] = &query;
    queries.queries = query_;
    error = !memcpy( queries.prefered_cmm, s->profile_cmm_, 4 );

    if(error <= 0)
      error = !memcpy( cmm, s->required_cmm, 4 );
  }

  if(error <= 0)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API3_S, cmm, 0,
                                      oyCMMapi3_Query_, &queries );
    if(api)
    {
      oyCMMapi3_s * api3 = (oyCMMapi3_s*) api;
      funcP = api3->oyCMMProfileTag_GetValues;
    }
    error = !funcP;
  }

  if(error <= 0)
  {
    values = funcP( tag );

    error = !memcpy( s->last_cmm_, cmm, 4 );
  }

  return values;
}

/** Function oyProfileTag_GetText
 *  @memberof oyProfileTag_s
 *
 *  For the effect of the parameters look at the appropriate module.
 *  @see oyIMProfileTag_GetValues
 *
 *  Hint: to select a certain module use the oyProfileTag_s::required_cmm
 *  element from the tag parameter.
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
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/03 (Oyranos: 0.1.8)
 *  @date    2008/06/19
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
  char ** texts = 0, * text = 0, * text_tmp = 0, * temp = 0;
  oyStructList_s * values = 0;
  oyName_s * name = 0;
  oyBlob_s * blob = 0;
  size_t size = 0;
  int values_n = 0, i = 0, k;
  int32_t texts_n = 0;

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
        name = 0;
        blob = 0;
        values_n = oyStructList_Count( values );

        for(k = 0; k < 4; ++k)
        {
          for(i = 0; i < values_n; ++i)
          {
            text = 0;
            name = (oyName_s*) oyStructList_GetRefType( values, i,
                                                        oyOBJECT_NAME_S );
            if(!name)
            blob = (oyBlob_s*) oyStructList_GetRefType( values, i,
                                                        oyOBJECT_BLOB_S );
            if(name)
            {
              memcpy(t_l, name->lang, 8); t_c[0] = 0;
              t_ptr = oyStrchr_(t_l, '_');
              if(t_ptr)
              {
                memcpy(t_c, t_ptr+1, 3);
                *t_ptr = 0;
              }
            }

            if(name)
              text = name->name;
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
              if(name && name->lang[0] && !implicite_i18n)
              {
                /* string with i18n infos -> "de_DE:Licht" */
                temp = oyStringAppend_(name->lang, ":", oyAllocateFunc_);
                temp = oyStringAppend_(temp, text, oyAllocateFunc_);
                oyStringListAddString_( &texts, &texts_n, &temp,
                                            oyAllocateFunc_, oyDeAllocateFunc_);

              } else {
                /* pure string -> "Licht" */
                oyStringListAddStaticString_( &texts, &texts_n, text,
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
    oyStructList_Release( &values );
  }

  return texts;
}

/** Function  oyProfileTag_GetBlock
 *  @memberof oyProfileTag_s
 *  @brief    Get the raw memory block of the tag.
 *
 *  @param[in]     tag                 the tag to read
 *  @param[out]    tag_block           the raw data owned by the user; on success the block if it has a size; else undefined
 *  @param[out]    tag_size            the data size; mandatory arg; on success the size returned in tag_block else undefined
 *  @param[in]     allocateFunc        the user allocator
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
