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

#include "oyProfiles_s_.h"
#include "oyOptions_s_.h"

oyStructList_s_ * oy_profile_s_file_cache_ = NULL;
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
oyStructList_s * oy_cmm_cache_ = NULL;
/** @internal
 *  @brief internal Oyranos module handle list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyStructList_s * oy_cmm_infos_ = NULL;
oyStructList_s * oy_cmm_handles_ = NULL;
oyProfiles_s * oy_profile_list_cache_ = NULL;
oyProfile_s_ ** oy_profile_s_std_cache_ = NULL;
oyOptions_s * oy_db_cache_ = NULL;
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
  if(!found && oy_profile_s_std_cache_)
  {
     int i, count = oyDEFAULT_PROFILE_END - oyDEFAULT_PROFILE_START;
     for(i = 0; i < count; ++i)
     {
       oyStruct_s * st = (oyStruct_s*) oy_profile_s_std_cache_[i];
       if(st && st->oy_ && st->oy_->id_ == id)
         found = 1;
     }
  }

  if(found)
    return 1;

  return 0;
}
