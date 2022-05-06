/*  @file Xcm.h
 *
 *  libXcm  Xorg Colour Management
 *
 *  @par Copyright:
 *            2008 (C) Tomas Carnecky
 *            2008-2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    X Color Management specfication helpers
 *  @internal
 *  @author   Tomas Carnecky
 *            Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2008/04/00
 */

#ifndef __XCOLOR_H__
#define __XCOLOR_H__

#include "XcmVersion.h"
#ifdef XCM_HAVE_X11

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <stdint.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <string.h>

#endif /* XCM_HAVE_X11 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef XCM_HAVE_X11
/** \addtogroup Xcm X Color Management Core API's

 *  @{
 */


/**
 *    The XcolorProfile typedefed structure
 * describes a single ICC profile. The 'md5' field is used to identify the
 * profile. The actual data follows right after the structure.
 */
typedef struct {
	uint8_t md5[16];  /**< ICC MD5 of the profile	        */
	uint32_t length;  /**< number of bytes following, network byte order */
} XcolorProfile;

/**
 *    The XCM_COLOR_PROFILES macro
 * represents a atom name for under which XcolorProfile data can be stored.
 */
#define XCM_COLOR_PROFILES "_ICC_COLOR_PROFILES"

/**
 *    The XcolorRegion typedefed structure
 * describes a single region with an attached profile. The region is
 * evaluated when the client enables the region and not when the region
 * is atttached to the window. This allows clients to update the region
 * (when the window is resized for example) by simply modifying the
 * XserverRegion and then sending a ClientMessage.
 */ 
typedef struct {
	uint32_t region;  /**< XserverRegion, network byte order */
	uint8_t md5[16];  /**< ICC MD5 of the associated profile	*/
} XcolorRegion;

/**
 *    The XCM_COLOR_REGIONS macro
 * represents a atom name for under which XcolorRegions data can be stored.
 */
#define XCM_COLOR_REGIONS "_ICC_COLOR_REGIONS"

/** Function  XcolorProfileUpload
 *  @brief    Uploads the profile into all screens of the display.
 *
 * Uploads the profile into all screens of the display. Profiles are
 * ref-counted inside the compositing manager, so make sure to call
 * XcolorProfileDelete() before your application exits or when you don't
 * need the profile anymore.
 */
int XcolorProfileUpload(Display *dpy, XcolorProfile *profile);

/** Function  XcolorProfileDelete
 *  @brief    Decreases the ref-count of the profile
 * 
 * You shouldn't use the profile
 * anymore after this call because it could have been removed from the
 * internal database in the compositing manager. If you do, nothing bad
 * will happen, just that color management won't work on regions that use
 * this profile.
 */
int XcolorProfileDelete(Display *dpy, XcolorProfile *profile);


/** Function  XcolorRegionInsert
 *  @brief    Inserts the supplied regions into the stack
 *
 * Inserts the supplied regions into the stack at the position 'pos', shifting
 * the existing profiles upwards in the stack. If 'pos' is beyond the stack end,
 * nothing will be done and the function returns '-1'.
 */
int XcolorRegionInsert(Display *dpy, Window win, unsigned long pos, XcolorRegion *region, unsigned long nRegions);

/** Function  XcolorRegionFetch
 *  @brief    Fetches the existing regions
 *
 * Fetches the existing regions and returns an array of 'XcolorRegion'. After
 * you're done, free the array using XFree(). The number of regions is put into
 * 'nRegions'.
 */
XcolorRegion *XcolorRegionFetch(Display *dpy, Window win, unsigned long *nRegions);

/** Function  XcolorRegionDelete
 *  @brief    Deletes regions
 *
 * Deletes 'count' regions starting at 'start' in the stack. If 'start + count'
 * in beyond the stack end, nothing will be done and the function returns '-1'.
 */
int XcolorRegionDelete(Display *dpy, Window win, unsigned long start, unsigned long count);


/** Function  XcolorRegionActivate
 *  @brief    Activates regions
 *
 * Activates 'count' regions starting at positiong 'start' in the stack. Unlike
 * the other functions it does not check whether 'start + count' extends beyond
 * the stack end. To disable all regions pass zero to 'count'.
 */
int XcolorRegionActivate(Display *dpy, Window win, unsigned long start, unsigned long count);

/**
 *    The XCM_COLOR_OUTPUTS macro
 * is attached to windows and specifies on which output the window should
 * look correctly. The type is XA_STRING.
 */
#define XCM_COLOR_OUTPUTS "_ICC_COLOR_OUTPUTS"

/**
 *    The XCM_COLOR_DESKTOP macro
The atom is attached on the root window to tell about a color servers activity.
The content is of type XA_STRING and has four sections separated by a 
empty space char ' '. 
The _ICC_COLOR_DESKTOP atom is a string with following usages:
- uniquely identify the colour server
- tell the name of the colour server
- tell the colour server is alive
All sections are separated by one space char ' ' for easy parsing.

The first section contains the process id (pid_t) of the color server process, 
which has set the atom.
The second section contains time since epoch GMT as returned by time(NULL).
The thired section contains the bar '|' separated and surrounded
capabilities:
  - ICP  _ICC_COLOR_PROFILES  - support per region profiles
  - ICM  _ICC_COLOR_MANAGEMENT - color server is active
  - ICO  _ICC_COLOR_OUTPUTS - support per window and output configuration
  - ICR  _ICC_COLOR_REGIONS - support regions
  - ICA  _ICC_COLOR_DISPLAY_ADVANCED - use CMS advanced settings, e.g. proofing
  - V0.3 indicates version compliance to the _ICC_Profile in X spec
The fourth section contains the servers name identifier.

As of this specification the third section must contain ICR and the 
supported _ICC_PROFILE in X version.

A example of a valid atom might look like:
_ICC_COLOR_DESKTOP(STRING) = "4518 1274001512 |ICR|ICM|V0.3| compiz_colour_desktop"
 */
#define XCM_COLOR_DESKTOP "_ICC_COLOR_DESKTOP"

#endif /* XCM_HAVE_X11 */

/**
 *   The XCM_COLOR_SERVER_ enums
 * describe colour server capabilities as specified in XCM_COLOR_DESKTOP atom.
 */
enum {
  XCM_COLOR_SERVER_REGIONS = 0x01,           /**< _ICC_COLOR_REGIONS */
  XCM_COLOR_SERVER_PROFILES = 0x02,          /**< _ICC_COLOR_PROFILES */
  XCM_COLOR_SERVER_DISPLAY_ADVANCED = 0x04,  /**< _ICC_COLOR_DISPLAY_ADVANCED */
  XCM_COLOR_SERVER_OUTPUTS = 0x08,           /**< _ICC_COLOR_OUTPUTS */
  XCM_COLOR_SERVER_MANAGEMENT = 0x10,        /**< _ICC_COLOR_MANAGEMENT */
  XCM_COLOR_SERVER_03 = 0x100,               /**< |V0.3| Spec Version 0.3 */
  XCM_COLOR_SERVER_04 = 0x200                /**< |V0.4| Spec Version 0.4 */
};
#ifdef XCM_HAVE_X11
/** Function  XcmColorServerCapabilities
 *  @brief    informs which colour server services are available
 *
 *  Query the capabilities of a colour server. In case no colour server is 
 *  running, the function should return zero. The return value consists of a bit
 *  mask of XCM_COLOR_SERVER_ properties from the XCM_COLOR_DESKTOP atom.
 */
int    XcmColorServerCapabilities    ( Display *dpy );

/**
 *    The _ICC_DEVICE_PROFILE atom
The atom will hold a native ICC profile with the exposed device 
characteristics at the compositing window manager level. 
The colour server shall if no _ICC_DEVICE_PROFILE(_xxx) is set, copy the 
_ICC_PROFILE(_xxx) profiles to each equivalent _ICC_DEVICE_PROFILE(_xxx) atom.
The _ICC_PROFILE(_xxx) profiles shall be replaced by a sRGB ICC profile.
The counting in the atoms (_xxx) name section follows the rules outlined in 
the ICC Profile in X recommendation. After finishing the session the the old
state has to be recovered by copying any _ICC_DEVICE_PROFILE(_xxx) atoms 
content into the appropriate _ICC_PROFILE(_xxx) atoms and removing all
_ICC_DEVICE_PROFILE(_xxx) atoms.
The colour server must be aware about change property events indicating that
a _ICC_PROFILE(_xxx) atom has changed by a external application and needs to
move that profile to the appropriate _ICC_DEVICE_PROFILE(_xxx) atom and set
the _ICC_PROFILE(_xxx) atom to sRGB as well.
The modification of the _ICC_DEVICE_PROFILE(_xxx) atoms by external applications
is undefined.
 */
#define XCM_DEVICE_PROFILE "_ICC_DEVICE_PROFILE"

/** 
 *  @} *//*Xcm
 */


#endif /* XCM_HAVE_X11 */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __XCOLOR_H__ */
