/** @file oyranos_generic.c

   [Template file inheritance graph]
   +-- oyranos_generic.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#include <string.h>

#include "oyranos_helper_macros.h"
#include "oyranos_helper.h"
#include "oyranos_generic.h"
#include "oyranos_object.h"
#include "oyranos_object_internal.h"
#include "oyranos_string.h"

#include "oyObject_s.h"
#include "oyHash_s.h"

#include "oyStructList_s_.h"


oyObjectInfoStatic_s oy_connector_imaging_static_object = {
  oyOBJECT_INFO_STATIC_S, 0,0,0,
  {2,oyOBJECT_CONNECTOR_S,oyOBJECT_CONNECTOR_IMAGING_S,0,0,0,0,0}
};

/** Public function definitions { */

/** Function  oyContextCollectData_
 *  @brief    Describe a transform uniquely
 *  @internal
 *
 *  @param[in,out] s                   the context's object 
 *  @param[in]     opts                options
 *  @param[in]     ins                 input datas
 *  @param[in]     outs                output datas
 *  @return                            the objects ID text
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 *  @date    2008/11/02
 */
const char *   oyContextCollectData_ ( oyStruct_s        * s,
                                       oyOptions_s       * opts,
                                       oyStructList_s    * ins,
                                       oyStructList_s    * outs )
{
  int error = !s;
  const char * model = NULL;

  char * hash_text = NULL;

  if(error <= 0)
  {
    /* input data */
    hashTextAdd_m(   " <data_in>\n" );
    hashTextAdd_m( oyStructList_GetID( ins, 0, 0 ) );
    hashTextAdd_m( "\n </data_in>\n" );

    /* options -> xforms */
    hashTextAdd_m(   " <oyOptions_s>\n" );
    model = oyOptions_GetText( opts, oyNAME_NAME );
    hashTextAdd_m( model );
    hashTextAdd_m( "\n </oyOptions_s>\n" );

    /* output data */
    hashTextAdd_m(   " <data_out>\n" );
    hashTextAdd_m( oyStructList_GetID( outs, 0, 0 ) );
    hashTextAdd_m( "\n </data_out>\n" );

    oyObject_SetName( s->oy_, hash_text, oyNAME_NICK );

    if(hash_text && s->oy_->deallocateFunc_)
      s->oy_->deallocateFunc_( hash_text );
    hash_text = NULL;

    hash_text = (oyChar*) oyObject_GetName( s->oy_, oyNAME_NICK );
  }

  return hash_text;
}

/** } Public function definitions */



/** Private function definitions { */

/** @internal
 *  @brief get always a Oyranos cache entry from a cache list
 *
 *  @param[in]     cache_list          the list to search in
 *  @param[in]     flags               0 - assume text, 1 - assume 16 byte hash
 *  @param[in]     hash_text           the text to search for in the cache_list
 *  @return                            the cache entry may not have a entry
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/24 (Oyranos: 0.1.8)
 *  @date    2009/06/04
 */
oyHash_s *   oyCacheListGetEntry_    ( oyStructList_s    * cache_list,
                                       uint32_t            flags,
                                       const char        * hash_text )
{
  oyHash_s * entry = 0,
           * search_key = 0;
  int error = !(cache_list && hash_text);
  int n = 0, i;
  uint32_t search_int[8] = {0,0,0,0,0,0,0,0};
  char hash_text_copy[32];
  const char * search_ptr = (const char*)search_int;

  if(error <= 0 && cache_list->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(error <= 0)
  {
    if(flags & 0x01)
      search_ptr = hash_text;
    else
    if(oyStrlen_(hash_text) < OY_HASH_SIZE*2-1)
    {
      memset( hash_text_copy, 0, OY_HASH_SIZE*2 );
      memcpy( hash_text_copy, hash_text, oyStrlen_(hash_text) );
      search_ptr = hash_text_copy;
    } else
      error = oyMiscBlobGetHash_( (void*)hash_text, oyStrlen_(hash_text), 0,
                                  (unsigned char*)search_int );
  }

  if(error <= 0)
    n = oyStructList_Count(cache_list);

  for(i = 0; i < n; ++i)
  {
    oyHash_s * compare = (oyHash_s*) oyStructList_GetType_( (oyStructList_s_*)cache_list, i,
                                                         oyOBJECT_HASH_S );

    if(compare )
    if(memcmp(search_ptr, compare->oy_->hash_ptr_, OY_HASH_SIZE*2) == 0)
    {
      entry = compare;
      oyHash_Copy( entry, 0 );
      return entry;
    }
  }

  if(error <= 0 && !entry)
  {
    search_key = oyHash_Create(hash_text, 0);
    error = !search_key;

    if(error <= 0)
      entry = oyHash_Copy( search_key, 0 );

    if(error <= 0)
    {
      error = oyStructList_MoveIn(cache_list, (oyStruct_s**)&search_key, -1, 0);
      search_key = 0;
    }

    oyHash_Release( &search_key );
  }


  return entry;
}

/** } Private function definitions */

