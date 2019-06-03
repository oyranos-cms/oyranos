/** @file oyranos_cache.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2017 (C) Kai-Uwe Behrmann
 *
 *  @brief    private Oyranos cache API's
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2017/07/03
 */

#include "oyranos_cache.h"

#include "oyObject_s.h"
#include "oyObject_s_.h"
#include "oyProfiles_s_.h"
#include "oyOptions_s_.h"



/** @internal
 *  The oy_cmm_cache_ is publicly accessed with oyPointer_LookUpFromText()
 *  by modules, as of writing only by @ref lcm2_graph for private
 *  cmsHTRANSFORM and cmsHPROFILE handles.\n
 *  The cache is privately accessed with oyCMMCacheListGetEntry_() and a
 *  string as hash value. Convinience API's are oyCMMsGetFilterApis_() and
 *  oyCMMsGetMetaApis_(). The oyCMMapi4_s and oyCMMapi7_s contexts are
 *  created and transformed with a oyCMMapi6_s filter by
 *  oyFilterNode_SetContext_() and cached here. oyFilterNode_ToBlob() is
 *  a public API for oyFilterNode_SetContext_()' oyCMMapi4_s context part.
 *  oyFilterGraph_PrepareContexts() uses oyFilterNode_SetContext_() as well
 *  and is itself called by oyConversion_RunPixels() for automatic
 *  resources resolving during DAG processing.
 *
 *  Concepts:\n
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
 *  to add as we can not know how many options and other influential parameters
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
oyStructList_s * oy_cmm_cache_ = NULL;
/** @internal
 *  @brief internal Oyranos module handle list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyStructList_s * oy_cmm_infos_ = NULL;
/** @internal
 *  @brief internal Oyranos filter handle list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyStructList_s * oy_cmm_handles_ = NULL;
/** @internal
 *  @brief internal Oyranos color profile list
 *
 *  Accessed with oyCacheListGetEntry_() and the file name as hash value.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyStructList_s_ * oy_profile_s_file_cache_ = NULL;
/** @internal
 *  @brief internal Oyranos color profile list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyProfiles_s * oy_profile_list_cache_ = NULL;
/** @internal
 *  @brief internal DB strings
 *
 *  The key name is used as hash value.
 *  Can be reset with NULL arg to oyGetPersistentStrings().
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyOptions_s * oy_db_cache_ = NULL;
/** @internal
 *  @brief internal monitor devices
 *
 *  Can be reset with NULL arg to oyGetPersistentStrings().
 *
 *  @since Oyranos: version 0.9.7
 *  @date  05.2018 (API 0.9.7)
 */
oyConfigs_s * oy_monitors_cache_ = NULL;


int      oyObjectUsedByStructList_   ( int                 id,
                                       oyStructList_s    * sl )
{
  int found = 0, count, i;
  oyStruct_s * st;

  count = oyStructList_Count(sl);
  for(i = 0; i < count; ++i)
  {
    st = oyStructList_Get_( (oyStructList_s_ *)sl, i );
    if(st && st->oy_ && st->oy_->id_ == id)
      found = 1;
    if(found) break;
  }

  return found;
}

#include "oyConfigs_s_.h"
int      oyObjectUsedByCache_        ( int                 id )
{
  int found = 0;

  found = oyObjectUsedByStructList_( id, (oyStructList_s*)oy_profile_s_file_cache_ );
  if(!found)
    found = oyObjectUsedByStructList_( id, oy_cmm_cache_ );
  if(!found)
    found = oyObjectUsedByStructList_( id, oy_cmm_infos_ );
  if(!found)
    found = oyObjectUsedByStructList_( id, oy_cmm_handles_ );
  if(!found && oy_profile_list_cache_)
    found = oyObjectUsedByStructList_( id, ((oyProfiles_s_*)oy_profile_list_cache_)->list_ );
  if(!found && oy_monitors_cache_)
    found = oyObjectUsedByStructList_( id, ((oyConfigs_s_*)oy_monitors_cache_)->list_ );

  if(found)
    return 1;

  return 0;
}
