/**
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2006  Kai-Uwe Behrmann
 *
 * @autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * -----------------------------------------------------------------------------
 */

/** @file @internal
 *  @brief pure text and string handling API
 */

/** @date      25. 11. 2004 */


#ifndef OYRANOS_TEXTS_H
#define OYRANOS_TEXTS_H

#include "oyranos.h"

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */


#define OY_STATIC_OPTS_  400


/** @internal
    @brief structure for UI text strings
 */
typedef struct {
  oyWIDGET_TYPE type;           /**< type */
  oyWIDGET    id;               /**< option */
  oyGROUP     category[10];     /**< layout for categories */
  int         flags;            /**< flags to control widget bebahiour */
  const char *name;             /**< label for setting */
  const char *description;      /**< description for setting */
  int         choices;          /**< number of options */
  const char *choice_list[10];  /**< label for each choice */
# if 0
  const char *choice_desc[10];  /**< description for each choices */
# endif
  double      range_start;      /**< start of range for a value selection */
  double      range_end;        /**< end of range for a value selection */
  double      range_step_major; /**< step size for a value selection */
  double      range_step_minor; /**< step size for a value selection */
  double      default_value;    /**< default selection */
  const char *config_string;    /**< full key name to store configuration */
  const char *config_string_xml;/**< key name to store configuration */
} oyOption_t_;


/* global variables */
//extern oyGROUP oy_groups_descriptions_;
//extern const char ***oy_groups_description_;
//extern oyOption_t_ *oy_option_;


void          oyTextsCheck_            (void);
void          oyTextsTranslate_        (void);

int           oyTestInsideBehaviourOptions_ (oyBEHAVIOUR type, int choice);
const oyOption_t_* oyOptionGet_        (oyWIDGET          type);

oyWIDGET    * oyWidgetListGet_         (oyGROUP           group,
                                        int             * count);
oyWIDGET_TYPE oyWidgetTypeGet_         (oyWIDGET          type);
oyWIDGET_TYPE oyWidgetTitleGet_        (oyWIDGET          option,
                                        const oyGROUP  ** categories,
                                        const char     ** name,
                                        const char     ** tooltip,
                                        int             * flags );
int           oyOptionChoicesGet_      (oyWIDGET          option,
                                        int             * choices,
                                        const char    *** choices_string_list,
                                        int             * current);
void          oyOptionChoicesFree_     (oyWIDGET_TYPE     option,
                                        const char    *** list,
                                        int               size);

//int           oyGroupGet_              (oyGROUP          type,
//                                        const char   *** strings);
oyGROUP       oyGroupAdd_              (const char *id, const char *cmm,
                                        const char *name, const char *tooltips);
int           oyGroupRemove_           (oyGROUP     id);



int         oySetDefaultProfile_       (oyDEFAULT_PROFILE type,
                                        const char*       file_name);
int         oySetDefaultProfileBlock_  (oyDEFAULT_PROFILE type,
                                        const char*       file_name,
                                        void*             mem,
                                        size_t            size);
char*       oyGetDefaultProfileName_   (oyDEFAULT_PROFILE type,
                                        oyAllocFunc_t     alloc_func);



#ifdef __cplusplus
} // extern "C"
} // namespace oyranos
#endif /* __cplusplus */

#endif /* OYRANOS_TEXTS_H */
