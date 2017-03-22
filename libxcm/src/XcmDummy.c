#include "XcmEdidParse.h"
#include "XcmVersion.h"
#if defined(XCM_HAVE_LINUX)
#include "XcmDDC.h"
#endif
#if defined(XCM_HAVE_X11)
#include "Xcm.h"
#endif

/* This function is used to have at least one symbol in a library, in order
 * to compile.
 */
void XcmDummy_()
{
  XcmEdidErrorToString(XCM_EDID_OK);
#if defined(XCM_HAVE_LINUX)
  XcmDDCErrorToString(XCM_DDC_OK);
#endif
#if defined(XCM_HAVE_X11)
  XcmColorServerCapabilities( NULL );
#endif
}
