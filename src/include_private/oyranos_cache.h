/** @file oyranos_cache.h
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
 *
 *  internal cache declarations and handling
 */

#ifndef OYRANOS_CACHE_H
#define OYRANOS_CACHE_H

#include "oyConfigs_s.h"
#include "oyProfile_s_.h"
#include "oyProfiles_s.h"
#include "oyOptions_s.h"
#include "oyStructList_s_.h"
extern oyStructList_s_ * oy_profile_s_file_cache_;
extern oyStructList_s  * oy_cmm_cache_;
extern oyStructList_s  * oy_cmm_infos_;
extern oyStructList_s  * oy_cmm_handles_;
extern oyConfigs_s     * oy_monitors_cache_;
extern oyProfiles_s    * oy_profile_list_cache_;
extern oyProfile_s_   ** oy_profile_s_std_cache_;
extern oyOptions_s     * oy_db_cache_;

int      oyObjectUsedByCache_        ( int                 id );
int *    get_oy_db_cache_init_();

#endif /* OYRANOS_CACHE_H */
