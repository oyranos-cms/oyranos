{% load gsoc_extensions %}{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

{% include "cpp_begin.h" %}

#include <oyranos_object.h>

#include "oyConfig_s.h"
#include "oyProfile_s.h"
#include "oyConfigs_s.h"
#include "oyOptions_s.h"

OYAPI int  OYEXPORT
           oyDevicesGet              ( const char        * device_type,
                                       const char        * device_class,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** devices );
OYAPI int  OYEXPORT
           oyDeviceGet               ( const char        * device_type,
                                       const char        * device_class,
                                       const char        * device_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device );
OYAPI int  OYEXPORT
           oyDeviceSetup             ( oyConfig_s        * device );
OYAPI int  OYEXPORT
           oyDeviceUnset             ( oyConfig_s        * device );
OYAPI int  OYEXPORT
           oyDeviceGetInfo           ( oyConfig_s        * device,
                                       oyNAME_e            type,
                                       oyOptions_s       * options,
                                       char             ** info_text,
                                       oyAlloc_f           allocateFunc );
OYAPI int  OYEXPORT
           oyDeviceBackendCall       ( oyConfig_s        * device,
                                       oyOptions_s       * options );
OYAPI int  OYEXPORT
           oyDeviceSetProfile        ( oyConfig_s        * device,
                                       const char        * profile_name );
OYAPI int  OYEXPORT
           oyDeviceGetProfile        ( oyConfig_s        * device,
                                       oyOptions_s       * options,
                                       oyProfile_s      ** profile );
OYAPI int  OYEXPORT
           oyDeviceAskProfile2       ( oyConfig_s        * device,
                                       oyOptions_s       * options,
                                       oyProfile_s      ** profile );
OYAPI int  OYEXPORT
           oyDeviceProfileFromDB     ( oyConfig_s        * device,
                                       char             ** profile_name,
                                       oyAlloc_f           allocateFunc );
OYAPI int OYEXPORT oyDeviceSelectSimiliar
                                     ( oyConfig_s        * pattern,
                                       oyConfigs_s       * heap,
                                       uint32_t            flags,
                                       oyConfigs_s      ** matched_devices );


{% include "cpp_end.h" %}
#endif /* {{ file_name|underscores|upper|tr:". _" }} */
