
/** @file @internal
 *  @brief misc alpha APIs
 */

/** @date      12. 11. 2007 */


#include "oyranos_cmm.h"
/*#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_monitor.h" */ /* TODO */
#include <math.h>

int                oyCMMCanHandle      ( oyCMMQUERY_e      type,
                                         int               value )
{
  int ret = -1;

  switch(type)
  {
    case oyQUERY_OYRANOS_COMPATIBILITY:
         ret = OYRANOS_VERSION; break;
    case oyQUERY_PIXELLAYOUT_DATATYPE:
         switch(value) {
         case oyUINT8:
         case oyUINT16: ret = 1 ; break;
         case oyUINT32:
         case oyHALF:
         case oyFLOAT:
         case oyDOUBLE: ret = 0; break;
         }
         break;
    case oyQUERY_PIXELLAYOUT_CHANNELCOUNT:
         ret = 16; break;
    case oyQUERY_PIXELLAYOUT_SWAP_COLOURCHANNELS:
         ret = 1; break;
    case oyQUERY_PIXELLAYOUT_COLOURCHANNEL_OFFSET:
         ret = 1; break;
    case oyQUERY_PIXELLAYOUT_PLANAR:
         ret = 1; break;
    case oyQUERY_PIXELLAYOUT_FLAVOUR:
         ret = 1; break;
    case oyQUERY_HDR:
         ret = 0; break;
    case oyQUERY_PROFILE_FORMAT:
         ret = 1; break;
  }

  return ret;
}
