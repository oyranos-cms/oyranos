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
    { "oihs", "version", NULL, OYRANOS_VERSION_NAME, oyVersionString( 2, 0 ) },
    { "oihs", "date", NULL, iso_dateTtime, date_description },
    { "oihs", "manufacturer", NULL, "Kai-Uwe Behrmann", "http://www.oyranos.org" },
    { "oihs", "copyright", NULL, "© 2005-2021 Kai-Uwe Behrmann and others", NULL },
    { "oihs", "license", NULL, "newBSD", "http://www.oyranos.org" },
    { "oihs", "url", NULL, "http://www.oyranos.org", NULL },
    { "oihs", "support", NULL, "https://www.gitlab.com/oyranos/oyranos/issues", NULL },
    { "oihs", "download", NULL, "http://www.oyranos.org", NULL },
    { "oihs", "sources", NULL, "http://www.oyranos.org", NULL },
    { "oihs", "development", NULL, "https://gitlab.com/oyranos/oyranos", NULL },
    { "oihs", "oyjl_module_author", NULL, "Kai-Uwe Behrmann", "http://www.behrmann.name" },
    { "oihs", "documentation", NULL, "http://www.openicc.info", documentation },
    { "", NULL, NULL, NULL, NULL }
  };
  return (oyjlUiHeaderSection_s*) oyjlStringAppendN( NULL, (const char*)s, sizeof(s), malloc );
}

