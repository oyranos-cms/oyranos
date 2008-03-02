/**
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2007  Kai-Uwe Behrmann
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
  oyWIDGET_TYPE_e type;           /**< type */
  oyWIDGET_e    id;               /**< option */
  oyGROUP_e     category[10];     /**< layout for categories */
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
  double      default_value;    /**< default option */
  const char *default_string;   /**< default string */
  const char *config_string;    /**< full key name to store configuration */
  const char *config_string_xml;/**< key name to store configuration */
} oyOption_t_;


/* global variables */
/*/extern oyGROUP_e oy_groups_descriptions_;
//extern const char ***oy_groups_description_;
//extern oyOption_t_ *oy_option_;*/


void          oyTextsCheck_            (void);
void          oyTextsTranslate_        (void);

int           oyTestInsideBehaviourOptions_ (oyBEHAVIOUR_e type, int choice);
const oyOption_t_* oyOptionGet_        (oyWIDGET_e          type);

oyWIDGET_e    * oyWidgetListGet_         (oyGROUP_e           group,
                                        int             * count,
                                        oyAllocFunc_t     allocate_func );
oyWIDGET_TYPE_e oyWidgetTypeGet_         (oyWIDGET_e          type);
oyWIDGET_TYPE_e oyWidgetTitleGet_        (oyWIDGET_e          option,
                                        const oyGROUP_e  ** categories,
                                        const char     ** name,
                                        const char     ** tooltip,
                                        int             * flags );
int           oyOptionChoicesGet_      (oyWIDGET_e          option,
                                        int             * choices,
                                        const char    *** choices_string_list,
                                        int             * current);
void          oyOptionChoicesFree_     (oyWIDGET_TYPE_e     option,
                                        char          *** list,
                                        int               size);

/*/int           oyGroupGet_              (oyGROUP_e          type,
//                                        const char   *** strings);*/
oyGROUP_e       oyGroupAdd_              (const char *id, const char *cmm,
                                        const char *name, const char *tooltips);
int           oyGroupRemove_           (oyGROUP_e     id);



int         oySetDefaultProfile_       (oyPROFILE_e       type,
                                        const char*       file_name);
int         oySetDefaultProfileBlock_  (oyPROFILE_e       type,
                                        const char*       file_name,
                                        void*             mem,
                                        size_t            size);
char*       oyGetDefaultProfileName_   (oyPROFILE_e       type,
                                        oyAllocFunc_t     alloc_func);

char*       oyPolicyNameGet_         ( );
int         oyPolicySet_             ( const char      * policy_file,
                                       const char      * full_name );
oyWIDGET_e* oyPolicyWidgetListGet_   ( oyGROUP_e           group,
                                       int             * count );
char **     oyConfigPathsGet_        ( int             * count,
                                       const char      * subdir,
                                       int               data,
                                       int               owner,
                                       oyAllocFunc_t     allocateFunc );
char **     oyConfigFilesGet_        ( int             * count,
                                       const char      * subdir,
                                       int               data,
                                       int               owner,
                                       const char      * dir_string,
                                       const char      * string,
                                       const char      * suffix,
                                       oyAllocFunc_t     allocateFunc );
char **     oyProfilePathsGet_       ( int             * count,
                                       oyAllocFunc_t     allocate_func);
char **     oyLibPathsGet_           ( int             * count,
                                       const char      * subdir,
                                       int               owner,
                                       oyAllocFunc_t     allocateFunc );
char **     oyLibFilesGet_           ( int             * count,
                                       const char      * subdir,
                                       int               owner,
                                       const char      * dir_string,
                                       const char      * string,
                                       const char      * suffix,
                                       oyAllocFunc_t     allocateFunc );


char*              oyStringCopy_     ( const char        * text,
                                       oyAllocFunc_t       allocateFunc );
char*              oyStringAppend_   ( const char        * text,
                                       const char        * append,
                                       oyAllocFunc_t       allocateFunc );
char*              oyStringAdd_      ( char              * text,
                                       const char        * append,
                                       oyAllocFunc_t       allocateFunc,
                                       oyDeAllocFunc_t     deallocFunc );

char**             oyStringSplit_    ( const char        * text,
                                       const char          delimiter,
                                       int               * count,
                                       oyAllocFunc_t       allocateFunc );
char**             oyStringListAppend_(const char       ** list,
                                       int                 n_alt,
                                       const char       ** append,
                                       int                 n_app,
                                       int               * count,
                                       oyAllocFunc_t       allocateFunc );
void               oyStringListRelease_(char           *** l,
                                       int                 size,
                                       oyDeAllocFunc_t     deallocFunc );
char**             oyStringListFilter_(const char       ** list,
                                       int                 n_alt,
                                       const char        * dir_string,
                                       const char        * string,
                                       const char        * suffix,
                                       int               * count,
                                       oyAllocFunc_t       allocateFunc );


#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_TEXTS_H */
