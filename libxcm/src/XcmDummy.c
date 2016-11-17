#include "XcmEdidParse.h"
#include "XcmDDC.h"
#include "Xcm.h"

/* This function is used to have at least one symbol in a library, in order
 * to compile.
 */
void XcmDummy_()
{
  XcmEdidErrorToString(XCM_EDID_OK);
#if defined(HAVE_LINUX)
  XcmDDCErrorToString(XCM_DDC_OK);
#endif
#if defined(HAVE_X11)
  XcmColorServerCapabilities( NULL );
#endif
}
