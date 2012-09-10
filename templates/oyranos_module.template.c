{% include "source_file_header.txt" %}

#include "oyranos_debug.h"
#include "oyranos_sentinel.h"

#include "oyranos_generic_internal.h"
#include "oyranos_module_internal.h"
#include "oyObject_s.h"
#include "oyCMMapi5_s.h"
#include "oyCMMapi9_s.h"
#include "oyCMMapiFilters_s.h"
#include "oyCMMapiFilters_s_.h"
#include "oyFilterPlug_s.h"

/** Function oyCMMsGetFilterApis_
 *  @brief   Let a oyCMMapi5_s meta module open a set of modules
 *  @internal
 *
 *  The oyCMMapiLoadxxx_ function family loads a API from a external module.\n
 *  The module system shall support:
 *    - use of the desired CMM for the task at hand
 *    - provide fallbacks for incapabilities
 *    - process in different ways and by different modules through the same API
 *    - select complementing API's
 *
 *  We have modules with well defined capabilities and some with fuzzy ones.\n
 *  For instance the X11 API's is well defined and we can use it, once it is
 *  loaded.\n
 *  A CMM for colour conversion has often limitations or certain features,
 *  which makes the search for alternatives desireable. So we search
 *  for match to our automatic criteria.\n
 *
 *
 *  This function allowes to obtain API's for matching modules/CMM's.
 *  The modules are filtered for double occurencies. The highest rank or
 *  version is selected.
 *
 *  @param[in]   cmm_meta              the selected meta API will be used,
 *                                     can be Zero or at least 4 bytes long,
 *                                     e.g. "oyIM" for the library module loader
 *  @param[in]   cmm_required          if present take this or fail, the arg
 *                                     simplifies and speeds up the search
 *  @param[in]   registration          point'.' separated list of identifiers
 *  @param[in]   type                  CMM API
 *  @param[in]   flags                 supported is
 *                                     - oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR
 *                                       for omitting implementation attributes
 *  @param[out]  rank_list             the ranks matching the returned list;
 *                                     without that only the most matching API
 *                                     is returned at position 0
 *  @param[out]  count                 count of returned modules
 *  @return                            a zero terminated list of modules
 *
 *  @version Oyranos: 0.1.11
 *  @since   2008/12/19 (Oyranos: 0.1.10)
 *  @date    2010/08/12
 */
oyCMMapiFilters_s * oyCMMsGetFilterApis_(const char        * cmm_meta,
                                         const char        * cmm_required,
                                         const char        * registration,
                                         oyOBJECT_e          type,
                                         uint32_t            flags,
                                         uint32_t         ** rank_list,
                                         uint32_t          * count )
{
  int error = !oyIsOfTypeCMMapiFilter( type );
  oyCMMapiFilters_s * apis = 0, * apis2 = 0;
  oyCMMapiFilter_s * api = 0,
                   * api2 = 0;
  oyCMMapiFilter_s_ ** api_ = (oyCMMapiFilter_s_**)&api;
  oyCMMapiFilter_s_ ** api2_ = (oyCMMapiFilter_s_**)&api2;
  uint32_t * rank_list_ = 0, * rank_list2_ = 0;
  int rank_list_n = 5, count_ = 0;
  oyObject_s object = oyObject_New();

  /*{
    if(!oy_module_cache_)
      oy_module_cache_ = oyStructList_New( 0 );

    entry = oyCacheListGetEntry_ ( oy_module_cache_, registration );

    api = (oyCMMapiFilter_s*) oyHash_GetPointer_( entry, type );

    if(api)
    {
      oyHash_Release_( &entry );
      return api;
    }
  }*/

  if(error <= 0)
  {
    oyCMMapi5_s_ * api5 = 0;
    oyCMMapis_s * meta_apis = oyCMMGetMetaApis_( cmm_meta );
    int meta_apis_n = 0;
    char ** files = 0;
    uint32_t  files_n = 0;
    int i, j, k = 0, match_j = -1, ret, match_i = -1, rank = 0, old_rank = 0,
        n, accept;
    char * match = 0, * reg = 0;
    oyCMMInfo_s_ * info = 0;

    meta_apis_n = oyCMMapis_Count( meta_apis );
    for(k = 0; k < meta_apis_n; ++k)
    {
      api5 = (oyCMMapi5_s_*) oyCMMapis_Get( meta_apis, k );

      files_n = 0;

      if(error <= 0)
      files = oyCMMsGetNames_(&files_n, api5->sub_paths, api5->ext,
                              cmm_required,
                              api5->data_type == 0 ? oyPATH_MODULE :
                                                     oyPATH_SCRIPT);
      else
        WARNc2_S("%s: %s", _("Could not open meta module API"),
                 oyNoEmptyString_m_( registration ));

      if(files)
      for( i = 0; i < files_n; ++i)
      {
        ret = 0; j = 0;
        ret = api5->oyCMMFilterScan( 0,0, files[i], type, j,
                                     0, 0, oyAllocateFunc_, (oyCMMInfo_s*)&info, object );
        while(!ret)
        {
          ret = api5->oyCMMFilterScan( 0,0, files[i], type, j,
                                       &reg, 0, oyAllocateFunc_, 0, 0 );
          if(!ret && reg)
          {
            rank = oyFilterRegistrationMatch( reg, registration, type );
            if(rank && OYRANOS_VERSION == info->oy_compatibility)
              ++rank;

            if(rank && rank_list)
            {

              if(!rank_list_ && !apis)
              {
                *rank_list = 0;
                oyAllocHelper_m_( *rank_list, uint32_t, rank_list_n+1, 0,
                                  goto clean );
                rank_list_ = *rank_list;
                apis = oyCMMapiFilters_New(0);
              } else
              if(count_ >= rank_list_n)
              {
                rank_list_n *= 2;
                rank_list_ = 0;
                oyAllocHelper_m_( rank_list_, uint32_t, rank_list_n+1, 0,
                                  goto clean );
                error = !memcpy( rank_list_, *rank_list,
                                 sizeof(uint32_t) * rank_list_n/2 );
                oyFree_m_(*rank_list);
                *rank_list = rank_list_;
              }

              rank_list_[k++] = rank;
              api = api5->oyCMMFilterLoad( 0,0, files[i], type, j);
              if(!api)
                continue;

              if(!(*api_)->id_)
                (*api_)->id_ = oyStringCopy_( files[i], oyAllocateFunc_ );
              (*api_)->api5_ = api5;
              if(!apis)
                apis = oyCMMapiFilters_New( 0 );
              oyCMMapiFilters_MoveIn( apis, &api, -1 );
              ++ count_;
              if(count)
                *count = count_;

            } else
            if(rank > old_rank)
            {
              match = reg;
              match_j = j;
              match_i = i;
              old_rank = rank;
            }
          }
          ++j;
          if(reg) oyFree_m_( reg );
        }
      }

      oyCMMInfo_Release( (oyCMMInfo_s**)&info );
#if 0
      if(api5->release)
        api5->release( (oyStruct_s**)&api5 );
#endif
    }

    oyCMMapis_Release( &meta_apis );

    if(match && !rank_list)
    {
      apis2 = oyCMMapiFilters_New( 0 );
      api = api5->oyCMMFilterLoad( 0,0, files[match_i], type, match_j );
      if(!(*api_)->id_)
        (*api_)->id_ = oyStringCopy_( files[match_i], oyAllocateFunc_ );
      (*api_)->api5_ = api5;
      oyCMMapiFilters_MoveIn( apis2, &api, -1 );
      if(count)
        *count = 1;
    }

    if(rank_list_)
    {
      /* filter doubled entries */
      n = oyCMMapiFilters_Count( apis );
      oyAllocHelper_m_( rank_list2_, uint32_t, rank_list_n+1, 0, goto clean );
      k = 0;
      for(i = 0 ; i < n; ++i)
      {
        char * apir = 0;

        api = oyCMMapiFilters_Get( apis, i );
        if(flags | oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR)
          oyFilterRegistrationModify( (*api_)->registration,
                                    oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                      &apir, 0 );
        accept = 1;

        for(j = i+1; j < n; ++j)
        {
          char * api2r = 0;

          api2 = oyCMMapiFilters_Get( apis, j );
          if(flags | oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR)
            oyFilterRegistrationModify( (*api2_)->registration,
                                    oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                        &api2r, 0 );

          /* for equal registration compare rank and version */
          if(oyStrcmp_( apir,  api2r ) == 0 &&
             rank_list_[i] <= rank_list_[j])
            accept = 0;

          oyFree_m_(api2r);

          if(api2->release)
            api2->release( (oyStruct_s**)&api2 );
        }

        if(!apis2)
          apis2 = oyCMMapiFilters_New( 0 );

        if(accept)
        {
          oyCMMapiFilters_MoveIn( apis2, &api, -1 );
          rank_list2_[k++] = rank_list_[i];
        }

        oyFree_m_(apir);

        if(api && api->release)
          api->release( (oyStruct_s**)&api );
      }

      if(rank_list)
        *rank_list = rank_list2_;
      if(rank_list_)
        oyFree_m_(rank_list_);
      if(count)
        *count = k;
    }

    oyStringListRelease_( &files, files_n, oyDeAllocateFunc_ );
  }

  /*if(api)
  {
    (*api_)->id_ = lib_name;
    error = oyHash_SetPointer_( entry, (oyStruct_s*) s );
  }*/

  clean:
    oyObject_Release( &object );

  return apis2;
}

/** @internal
 *  Function oyCMMsGetApi__
 *  @brief get a specified module
 *
 *  The oyCMMapiLoadxxx_ function family loads a API from a external module.\n
 *
 *  This function allowes to obtain a desired API from a certain library.
 *
 *  @param[in]   type                  the API to return
 *  @param[in]   lib_name              if present take this or fail, the arg
 *                                     simplifies and speeds up the search
 *  @param[in]   apiCheck              custom api selector
 *  @param[in]   check_pointer         data to pass to apiCheck
 *  @param[in]   num                   position in api chain matching to type
 *                                     and apiCheck/check_pointer starting from
 *                                     zero,
 *                                     -1 means: pick the first match, useful
 *                                     in case the API position is known or to
 *                                     iterate through all matching API's
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/08 (Oyranos: 0.1.9)
 *  @date    2008/12/23
 */
oyCMMapi_s *     oyCMMsGetApi__      ( oyOBJECT_e          type,
                                       const char        * lib_name,
                                       oyCMMapi_Check_f    apiCheck,
                                       oyPointer           check_pointer,
                                       int                 num )
{
  int error = !type,
      i = 0;
  oyCMMapi_s * api = 0;
  uint32_t rank = 0;

  if(error <= 0 &&
     !(oyOBJECT_CMM_API1_S <= type && type < oyOBJECT_CMM_API_MAX))
    error = 1;

  if(error <= 0)
  {
    /* open the module */
    oyCMMInfo_s * cmm_info = oyCMMInfoFromLibName_( lib_name );

    if(cmm_info)
    {
      oyCMMapi_s * tmp = cmm_info->api;

      if(!apiCheck)
        apiCheck = oyCMMapi_CheckWrap_;

      while(tmp)
      {
        if(apiCheck(cmm_info, tmp, check_pointer, &rank) == type)
        {
          if((num >= 0 && num == i) ||
             num < 0 )
            api = tmp;

          ++i;
        }
        tmp = tmp->next;
      }
    }

    oyCMMInfo_Release( &cmm_info );
  }

  return api;
}

/** @internal
 *  Function oyCMMsGetApi_
 *  @brief get a oyranos module
 *
 *  The oyCMMapiLoadxxx_ function family loads a API from a external module.\n
 *  The module system shall support:
 *    - use of the desired CMM for the task at hand
 *    - provide fallbacks for incapabilities
 *    - process in different ways and by different modules through the same API
 *
 *  We have modules with well defined capabilities and some with fuzzy ones.\n
 *  For instance the X11 API's is well defined and we can use it, once it is
 *  loaded.\n
 *  A CMM for colour conversion has often limitations or certain features,
 *  which make it desireable. So we have to search for match to our automatic
 *  criteria.\n
 *
 *
 *  This function allowes to obtain a API for a certain modul/CMM.
 *  oyCMMapi4_s is excluded.
 *
 *  @param[in]   type                  the API type to return
 *  @param[in]   cmm_required          if present take this or fail, the arg
 *                                     simplifies and speeds up the search
 *  @param[out]  lib_used              inform about the selected CMM
 *  @param[in]   apiCheck              custom API selector
 *  @param[in]   check_pointer         data to pass to apiCheck
 *
 *  @version Oyranos: 0.1.9
 *  @since   2007/12/12 (Oyranos: 0.1.9)
 *  @date    2008/12/16
 */
oyCMMapi_s *     oyCMMsGetApi_       ( oyOBJECT_e          type,
                                       const char        * cmm_required,
                                       char             ** lib_used,
                                       oyCMMapi_Check_f    apiCheck,
                                       oyPointer           check_pointer )
{
  int error = !type;
  oyCMMapi_s * api = 0;

  if(!apiCheck)
    apiCheck = oyCMMapi_CheckWrap_;

  if(error <= 0 &&
     !(oyOBJECT_CMM_API1_S <= type && type < oyOBJECT_CMM_API_MAX))
    error = 1;

  if(error <= 0)
  {
    char ** files = 0;
    uint32_t  files_n = 0;
    int i, oy_compatibility = 0;
    uint32_t rank = 0,
             max_rank = 0;
    int max_pos = -1;

    files = oyCMMsGetLibNames_(&files_n, cmm_required);

    /* open the modules */
    for( i = 0; i < files_n; ++i)
    {
      oyCMMInfo_s * cmm_info = oyCMMInfoFromLibName_(files[i]);

      if(cmm_info)
      {
        oyCMMapi_s * tmp = cmm_info->api;

        while(tmp)
        {

          if(apiCheck(cmm_info, tmp, check_pointer, &rank) == type)
          {

              if( /* if we found already a matching version, do not exchange*/
                  oy_compatibility != OYRANOS_VERSION &&
                    /* possibly newly found */
                  ( oy_compatibility = 0 ||
                    /* or a bigger version but not greater than current oy_ */
                    ( cmm_info->oy_compatibility <= OYRANOS_VERSION &&
                      oy_compatibility < cmm_info->oy_compatibility ) ||
                    /* or we select a less greater in case we are above oy_ */
                    ( cmm_info->oy_compatibility > OYRANOS_VERSION &&
                      oy_compatibility > cmm_info->oy_compatibility )
                  )
                )
              {
                if(cmm_info->oy_compatibility == OYRANOS_VERSION)
                  ++rank;
                if(rank > max_rank)
                {
                  api = tmp;
                  max_rank = rank;
                  max_pos = i;

                }
                oy_compatibility = cmm_info->oy_compatibility;
              }
          }
          tmp = tmp->next;
        }
      }

      oyCMMInfo_Release( &cmm_info );
    }

    if(max_rank >= 0 && lib_used)
    {
      if(*lib_used)
        oyFree_m_(*lib_used);
      if(files && files_n)
        *lib_used = oyStringCopy_( files[max_pos], oyAllocateFunc_ );
    }

    oyStringListRelease_( &files, files_n, oyDeAllocateFunc_ );
  }

  return api;
}

/** @internal
 *  The lists are allocated one time and live until the application quits
 *  It contains the various caches for faster access of CPU intentsive data.\n
 *  We'd need a 3 dimensional table to map\n
 *    A: a function or resource type\n
 *    B: a CMM\n
 *    C: a hash value specifying under which conditions the resource was build\n
 *  The resulting cache entry is the result from the above 3 arguments. With the
 *  much implementation work and expectedly small to no speed advantage it is
 *  not a good strategy. Even though argument C would be extensible.\n
 *  \n
 *  A different approach would use two values to map the search request to the
 *  cache entry. The above hash map and the hash or combination of the two other *  values.\
 *  \n
 *  One point to consider is a readable string to end not with a anonymous list
 *  full of anonymous entries, where a user can pretty much nothing know.
 *  A transparent approach has to allow for easy identifying each entry.\n
 *  A help would be a function to compute both a md5 digest and a message from
 *  the 3 arguments outlined above. Probably it would allow much more arguments
 *  to add as we can not know how many optins and other influential parameters
 *  the cache entry depends on.\n
 *  \n
 *  A final implementation would consist of a
 *  - function to convert arbitrary (string) arguments to a single string and a
 *    hash sum
 *  - a function to create a cache entry struct from above hash and a the
 *    according description string plus the oyPointer_s struct. For simplicity
 *    the cache struct can be identical to the oyPointer_s, with the disadvantage
 *    of containing additional data not understandable for a CMM. We need to
 *    mark these data (hash + description) as internal to Oyranos.
 *  - a list that hold above cache entry stucts
 *  - several functions to reference, release, maps a hash value to the
 *    cached resource
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
oyStructList_s * oy_cmm_cache_ = 0;

/** @internal
 *  @brief get always a Oyranos cache entry from the CMM's cache
 *
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyHash_s *   oyCMMCacheListGetEntry_ ( const char        * hash_text)
{
  if(!oy_cmm_cache_)
    oy_cmm_cache_ = oyStructList_New( 0 );

  return oyCacheListGetEntry_(oy_cmm_cache_, 0, hash_text);
}

/** Function oyCMMGetMetaApis_
 *  @brief   Get module loaders
 *  @internal
 *
 *  This function searches for a meta modul API.
 *
 *  @param[in]   cmm                   the four byte module ID, optional
 *
 *  @version Oyranos: 0.1.11
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2010/09/14
 */
oyCMMapis_s_ * oyCMMGetMetaApis_     ( const char        * cmm )
{
  oyCMMapis_s_ * meta_apis = 0;
  meta_apis = oyCMMsGetMetaApis_( cmm );
  return meta_apis;
}

/** @internal
 *  Function oyCMMsGetMetaApis_
 *  @brief get oyranos modules
 *
 *  @param[in]   cmm                  the selected module, optional
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/06/25 (Oyranos: 0.1.10)
 *  @date    2010/09/14
 */
oyCMMapis_s *    oyCMMsGetMetaApis_  ( const char        * cmm )
{
  int error = 0;
  oyCMMapis_s * apis = 0;
  oyCMMapi_Check_f apiCheck = oyCMMapi_CheckWrap_;

  if(error <= 0)
  {
    char ** files = 0;
    uint32_t  files_n = 0;
    int i = 0;

    files = oyCMMsGetLibNames_(&files_n, cmm);

    if(!files_n)
      WARNc_S(_("Could not find any meta module. "
                "Did you set the OY_MODULE_PATHS variable,"
                " to point to a Oyranos module loader library?"));

    /* open the modules */
    for( i = 0; i < files_n; ++i)
    {
      oyCMMInfo_s * cmm_info = oyCMMInfoFromLibName_(files[i]);

      if(cmm_info)
      {
        oyCMMapi_s * tmp = cmm_info->api,
                   * copy = 0;

        while(tmp)
        {

          if(apiCheck(cmm_info, tmp, 0, 0) == oyOBJECT_CMM_API5_S)
          {
            if(!apis)
              apis = oyCMMapis_New(0);

            copy = tmp;

            oyCMMapis_MoveIn( apis, &copy, -1 );
          }

          tmp = tmp->next;
        }
      }

      oyCMMInfo_Release( &cmm_info );
    }

    oyStringListRelease_( &files, files_n, oyDeAllocateFunc_ );
  }

  return apis;
}

/** Function  oyStruct_GetTextFromModule
 *  @memberof oyStruct_s
 *  @brief    get object infos from a module
 *
 *  @param[in,out] obj                 the objects structure
 *  @param[in]     name_type           the type
 *  @param[in]     flags               @see oyStruct_GetText
 *  @return                            the text
 *
 *  @version  Oyranos: 0.3.3
 *  @since    2009/09/15 (Oyranos: 0.3.3)
 *  @date     2011/10/31
 */
const char * oyStruct_GetTextFromModule (
                                       oyStruct_s        * obj,
                                       oyNAME_e            name_type,
                                       uint32_t            flags )
{
  int error = !obj;
  const char * text = 0;

  if(!error)
    text = oyObject_GetName( obj->oy_, oyNAME_NICK );

  if(!error && !text)
  {
    if(obj->type_)
    {
      oyCMMapiFilters_s * apis;
      int apis_n = 0, i,j,n;
      oyCMMapi9_s * cmm_api9 = 0;
      char * api_reg = 0;

      apis = oyCMMsGetFilterApis_( 0,0, api_reg, oyOBJECT_CMM_API9_S,
                                   oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                   0, 0);
      apis_n = oyCMMapiFilters_Count( apis );
      for(i = 0; i < apis_n; ++i)
      {
        cmm_api9 = (oyCMMapi9_s*) oyCMMapiFilters_Get( apis, i );

        n = 0;
        while( cmm_api9->object_types && cmm_api9->object_types[n])
          ++n;
        for(j = 0; j < n; ++j)
          if( cmm_api9->object_types[j]->oyCMMobjectGetText &&
              cmm_api9->object_types[j]->id == obj->type_ )
          {
            text = cmm_api9->object_types[j]->oyCMMobjectGetText( flags?0:obj,
                                                   name_type, 0 );
            if(text)
              break;
          }

        if(cmm_api9->release)
          cmm_api9->release( (oyStruct_s**)&cmm_api9 );

        if(text)
          break;
      }
      oyCMMapiFilters_Release( &apis );
    }
  }

  if(!error && !text)
    text = oyStructTypeToText( obj->type_ );

  return text;
}


/** @internal
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
int    oyIsOfTypeCMMapiFilter        ( oyOBJECT_e          type )
{
  return        type == oyOBJECT_CMM_API4_S ||
                type == oyOBJECT_CMM_API6_S ||
                type == oyOBJECT_CMM_API7_S ||
                type == oyOBJECT_CMM_API8_S ||
                type == oyOBJECT_CMM_API9_S ||
                type == oyOBJECT_CMM_API10_S;
}

/**
 *  @brief oyDATATYPE_e to byte mapping
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2007/11/00
 */
int      oySizeofDatatype            ( oyDATATYPE_e        t )
{
  int n = 0;
  switch(t)
  {
    case oyUINT8:
         return 1;
    case oyUINT16:
    case oyHALF:
         return 2;
    case oyUINT32:
    case oyFLOAT:
         return 4;
    case oyDOUBLE:
         return 8;
  }
  return n;
}

/**
 *  @brief oyDATATYPE_e to string mapping
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */

const char *   oyDatatypeToText      ( oyDATATYPE_e        t)
{
  const char * text = 0;
  switch(t)
  {
    case oyUINT8:
         text = "oyUINT8"; break;
    case oyUINT16:
         text = "oyUINT16"; break;
    case oyHALF:
         text = "oyHALF"; break;
    case oyUINT32:
         text = "oyUINT32"; break;
    case oyFLOAT:
         text = "oyFLOAT"; break;
    case oyDOUBLE:
         text = "oyDOUBLE"; break;
  }
  return text;
}

