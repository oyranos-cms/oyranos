/** @file oyranos_widgets.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2008-2021  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    widget APIs
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2008/02/09
 */

#include "oyranos_texts.h"
#include "oyranos_version.h"

#include <string.h>
oyjlUiHeaderSection_s *    oyUiInfo  ( const char          * documentation,
                                       const char          * iso_dateTtime,
                                       const char          * date_description )
{
  oyjlUiHeaderSection_s s[] = {
    /* type,  nick,      label,name,                 description */
    { "oihs", "version", NULL, OYRANOS_VERSION_NAME, oyVersionString( 2 ) },
    { "oihs", "date", NULL, iso_dateTtime, date_description },
    { "oihs", "manufacturer", NULL, "Kai-Uwe Behrmann", "https://gitlab.com/beku" },
    { "oihs", "copyright", NULL, "© 2005-2024 Kai-Uwe Behrmann and others", NULL },
    { "oihs", "license", NULL, "newBSD", "https://gitlab.com/oyranos/oyranos" },
    { "oihs", "url", NULL, "https://gitlab.com/oyranos/oyranos", NULL },
    { "oihs", "support", NULL, "https://www.gitlab.com/oyranos/oyranos/issues", NULL },
    { "oihs", "download", NULL, "", NULL },
    { "oihs", "sources", NULL, "https://gitlab.com/oyranos/oyranos", NULL },
    { "oihs", "development", NULL, "https://gitlab.com/oyranos/oyranos", NULL },
    { "oihs", "oyjl_module_author", NULL, "Kai-Uwe Behrmann", "https://gitlab.com/beku" },
    { "oihs", "documentation", NULL, "https://gitlab.com/oyranos/oyranos#oyranos-readme", documentation },
    { "", NULL, NULL, NULL, NULL }
  };
  return (oyjlUiHeaderSection_s*) oyjlStringAppendN( NULL, (const char*)s, sizeof(s), malloc );
}

