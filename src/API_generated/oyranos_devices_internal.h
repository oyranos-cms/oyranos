/** @file oyranos_devices_internal.h

   [Template file inheritance graph]
   +-- oyranos_devices_internal.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2012/09/15
 */


#ifndef OYRANOS_DEVICES_INTERNAL_H
#define OYRANOS_DEVICES_INTERNAL_H

#include "oyConfig_s_.h"
#include "oyOptions_s_.h"

#define OY_ERR if(l_error != 0) error = l_error;

char * oyDeviceRegistrationCreate_   ( const char        * device_type,
                                       const char        * device_class,
                                       const char        * key,
                                       char              * old_text );
int    oyOptions_SetDeviceTextKey_   ( oyOptions_s_      * options,
                                       const char        * device_type,
                                       const char        * device_class,
                                       const char        * key,
                                       const char        * value );
int    oyOptions_SetRegistrationTextKey_(
                                       oyOptions_s_      * options,
                                       const char        * registration,
                                       const char        * key,
                                       const char        * value );
oyOption_s *   oyOption_FromDB       ( const char        * registration,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
               oyOptions_SaveToDB    ( oyOptions_s       * options,
                                       const char        * registration,
                                       char             ** new_reg,
                                       oyAlloc_f           allocateFunc );
int          oyDeviceCheckProperties ( oyConfig_s        * device );

#endif /* OYRANOS_DEVICES_INTERNAL_H */
