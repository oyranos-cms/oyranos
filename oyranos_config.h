/** @file oyranos_config.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2004-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    configuration access
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */


#ifndef OYRANOS_CONFIG_H
#define OYRANOS_CONFIG_H

#include <stdlib.h> /* for linux size_t */
#include "oyranos_definitions.h"

/** \namespace oyranos
    @brief The Oyranos namespace.
 */
#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */

/* --- behaviour / policies --- */

int         oySetBehaviour             (oyBEHAVIOUR_e       type,
                                        int               choice);


/* path names */

int   oyPathAdd                        (const char* pathname);
void  oyPathRemove                     (const char* pathname);

/* --- default profiles --- */

int         oySetDefaultProfile        (oyPROFILE_e       type,
                                        const char*       file_name);
int         oySetDefaultProfileBlock   (oyPROFILE_e       type,
                                        const char*       file_name,
                                        void*             mem,
                                        size_t            size);


/* --- profile lists --- */


/* --- profile checking --- */


/* --- profile access through oyranos --- */



#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_CONFIG_H */
