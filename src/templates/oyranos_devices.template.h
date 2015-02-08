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
           oyDeviceSetup             ( oyConfig_s        * device,
                                       oyOptions_s       * options );
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
                                       oySCOPE_e           scope,
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
OYAPI int  OYEXPORT
             oyDevicesFromTaxiDB     ( oyConfig_s        * device,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** devices,
                                       oyObject_s          obj );
OYAPI int OYEXPORT oyDeviceSelectSimiliar
                                     ( oyConfig_s        * pattern,
                                       oyConfigs_s       * heap,
                                       uint32_t            flags,
                                       oyConfigs_s      ** matched_devices );
OYAPI int  OYEXPORT oyDeviceFromJSON ( const char        * json_text,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device );
OYAPI int OYEXPORT oyDeviceToJSON    ( oyConfig_s        * device,
                                       oyOptions_s       * options,
                                       char             ** json_text,
                                       oyAlloc_f           allocateFunc );


/** @struct  oyConfDomain_s
 *  @brief   a ConfDomain object
 *  @extends oyStruct_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
typedef struct oyConfDomain_s {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_CONF_DOMAIN_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */
} oyConfDomain_s;

OYAPI oyConfDomain_s * OYEXPORT
           oyConfDomain_FromReg      ( const char        * registration_domain,
                                       oyObject_s          object );
OYAPI oyConfDomain_s * OYEXPORT
           oyConfDomain_Copy         ( oyConfDomain_s    * obj,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
           oyConfDomain_Release      ( oyConfDomain_s    **obj );

OYAPI const char * OYEXPORT
           oyConfDomain_GetText      ( oyConfDomain_s    * obj,
                                       const char        * name,
                                       oyNAME_e            type );
OYAPI const char ** OYEXPORT
           oyConfDomain_GetTexts     ( oyConfDomain_s    * obj );



{% include "cpp_end.h" %}
#endif /* {{ file_name|underscores|upper|tr:". _" }} */
