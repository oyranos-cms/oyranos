{% load gsoc_extensions %}{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

#include "oyConfig_s_.h"
#include "oyOptions_s_.h"

#define OY_ERR if(l_error != 0) error = l_error;

char * oyDeviceRegistrationCreate_   ( const char        * device_type,
                                       const char        * device_class,
                                       const char        * key,
                                       char              * old_text );
int    oyOptions_SetDeviceTextKey_   ( oyOptions_s      ** options,
                                       const char        * device_type,
                                       const char        * device_class,
                                       const char        * key,
                                       const char        * value );
int    oyOptions_SetRegistrationTextKey_(
                                       oyOptions_s      ** options,
                                       const char        * registration,
                                       const char        * key,
                                       const char        * value );
OYAPI int  OYEXPORT  oyOption_FromDB ( const char        * registration,
                                       oyOption_s       ** option,
                                       oyObject_s          object );
int          oyDeviceCheckProperties ( oyConfig_s        * device );

#endif /* {{ file_name|underscores|upper|tr:". _" }} */
