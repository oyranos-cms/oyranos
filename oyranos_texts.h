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
 *
 * pure text and string handling API
 * 
 */

/** @date      25. 11. 2004 */


#ifndef OYRANOS_TEXTS_H
#define OYRANOS_TEXTS_H

#include <oyranos.h>

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */

/* group/option handling */
typedef enum {
  oyOPTION_TYPE_START,
  oyTYPE_BEHAVIOUR,
  oyTYPE_DEFAULT_PROFILE,
  oyTYPE_PROFILE,
  oyTYPE_INT,
  /*oyTAPE_DOUBLE,*/
  oyOPTION_TYPE_END
} oyOPTION_TYPE;

/** @brief the internal only used structure for UI text strings
 */

typedef struct {
  oyOPTION    id;               /**< option */
  oyOPTION_TYPE type;           /**< type */
  oyGROUP     categories[10];   /**< layout for categories */
  const char *label;            /**< label for setting */
  const char *description;      /**< description for setting */
  int         choices;          /**< number of options */
  const char *choice_list[10];  /**< label for each choice */
# if 0
  const char *choice_desc[10];  /**< description for each choices */
# endif
  const char *config_string;    /**< full key name to store configuration */
  const char *config_string_xml;/**< key name to store configuration */
} oyOption_t;


/* global variables */
extern oyGROUP oy_groups_descriptions_;
extern const char ***oy_groups_description_;
extern oyOption_t *oy_option_;


#define OY_STATIC_OPTS_  400


oyOPTION_TYPE oyGetOptionType_         (oyOPTION          type);
int         oyTestInsideBehaviourOptions_ (oyBEHAVIOUR type, int choice);
const oyOption_t* oyGetOption_         (oyOPTION          type);
const char* oyGetOptionUITitle_        (oyOPTION          type,
                                        const oyGROUP   **categories,
                                        int              *choices,
                                        const char     ***choices_string_list,
                                        const char      **tooltips);
const char* oyGetGroupUITitle_         (oyGROUP          type,
                                        const char      **tooltips);
int         oyGetBehaviour_            (oyBEHAVIOUR type);
int         oySetBehaviour_            (oyBEHAVIOUR type, int choice);

void        oyCheckOptionStrings_      (oyOption_t *opt);


int         oySetDefaultProfile_       (oyDEFAULT_PROFILE type,
                                        const char*       file_name);
int         oySetDefaultProfileBlock_  (oyDEFAULT_PROFILE type,
                                        const char*       file_name,
                                        void*             mem,
                                        size_t            size);
char*       oyGetDefaultProfileName_   (oyDEFAULT_PROFILE type,
                                        oyAllocFunc_t     alloc_func);


/* miscellaneous */
void   oyI18Nrefresh_();
void   oyI18NSet_                    ( int active,
                                       int reserved );


#ifdef __cplusplus
} // extern "C"
} // namespace oyranos
#endif /* __cplusplus */

#endif /* OYRANOS_TEXTS_H */
