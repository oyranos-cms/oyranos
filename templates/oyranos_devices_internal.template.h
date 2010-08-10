{% load gsoc_extensions %}{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

#include "oyOptions_s_.h"

#define OY_ERR if(l_error != 0) error = l_error;

char * oyDeviceRegistrationCreate_   ( const char        * device_type,
                                       const char        * device_class,
                                       const char        * key,
                                       char              * old_text );

#endif /* {{ file_name|underscores|upper|tr:". _" }} */
