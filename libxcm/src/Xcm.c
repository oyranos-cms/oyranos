/*  @file Xcm.c
 *
 *  libXcm  Xorg Colour Management
 *
 *  @par Copyright:
 *            2008 (C) Tomas Carnecky
 *            2008-2013 (C) Kai-Uwe Behrmann
 *
 *  @brief    X Color Management specification helpers
 *  @internal
 *  @author   Tomas Carnecky
 *            Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2008/04/00
 */

#include <Xcm.h>
#include <stdio.h>
#include "XcmEvents.h"

extern int * xcm_debug;
extern XcmMessage_f XcmMessage_p;

int  XcmChangeProperty_              ( Display           * dpy,
                                       Window              win,
                                       Atom                atom,
                                       int                 PropMode,
                                       unsigned char     * data,
                                       unsigned int        length )
{
  int result = XChangeProperty(dpy, win, atom, XA_CARDINAL, 8, PropMode, data, length);
  return result;
}
                                       

int XcolorProfileUpload(Display *dpy, XcolorProfile *profile)
{
	/* XcolorProfile::length is in network byte-order, swap it now */
	uint32_t length = htonl(profile->length);
        int i;

	Atom netColorProfiles = XInternAtom(dpy, XCM_COLOR_PROFILES, False);

	for (i = 0; i < ScreenCount(dpy); ++i) {
		XcmChangeProperty_(dpy, XRootWindow(dpy, i), netColorProfiles, PropModeAppend, (unsigned char *) profile, sizeof(XcolorProfile) + length);
	}

	return 0;
}

int XcolorProfileDelete(Display *dpy, XcolorProfile *profile)
{
	Atom netColorProfiles = XInternAtom(dpy, XCM_COLOR_PROFILES, False);
        int i;

	/* To delete a profile, send the header with a zero-length. */
	profile->length = 0;

	for (i = 0; i < ScreenCount(dpy); ++i) {
		XcmChangeProperty_(dpy, XRootWindow(dpy, i), netColorProfiles, PropModeAppend, (unsigned char *) profile, sizeof(XcolorProfile));
	}

	return 0;
}



int XcolorRegionInsert(Display *dpy, Window win, unsigned long pos, XcolorRegion *region, unsigned long nRegions)
{
	Atom netColorRegions = XInternAtom(dpy, XCM_COLOR_REGIONS, False);
	XcolorRegion *ptr;
	int result;

	unsigned long nRegs;
	XcolorRegion *reg = XcolorRegionFetch(dpy, win, &nRegs);

	/* Security check to ensure that the client doesn't try to insert the regions
	 * to a position beyond the stack end. */
	if (pos > nRegs) {
		XFree(reg);
		return -1;
	}

	ptr = calloc(sizeof(char), (nRegs + nRegions) * sizeof(XcolorRegion));
	if (ptr == NULL) {
		XFree(reg);
		return -1;
	}


	/* Make space for the new regions and copy them to the array. */
	if (nRegs)
	{
		memcpy(ptr, reg, nRegs * sizeof(XcolorRegion));
		memmove(ptr + pos + nRegs, ptr + pos, nRegions * sizeof(XcolorRegion));
	}
	memcpy(ptr + pos, region, nRegions * sizeof(XcolorRegion));

	result = !XcmChangeProperty_(dpy, win, netColorRegions, PropModeReplace, (unsigned char *) ptr, (nRegs + nRegions) * sizeof(XcolorRegion));

	if(reg)
		XFree(reg);
	free(ptr);

	return result;
}

XcolorRegion *XcolorRegionFetch(Display *dpy, Window win, unsigned long *nRegions)
{

	Atom actual, netColorRegions = XInternAtom(dpy, XCM_COLOR_REGIONS, False);

	unsigned long left, nBytes;
	unsigned char *data;
       
	int format, result = XGetWindowProperty(dpy, win, netColorRegions, 0, ~0, False, XA_CARDINAL, &actual, &format, &nBytes, &left, &data);

	*nRegions = 0;
	if (result != Success)
		return NULL;

	*nRegions = nBytes / sizeof(XcolorRegion);
	return (XcolorRegion *) data;
}


int XcolorRegionDelete(Display *dpy, Window win, unsigned long start, unsigned long count)
{
	Atom netColorRegions = XInternAtom(dpy, XCM_COLOR_REGIONS, False);
	int result;

	unsigned long nRegions;
	XcolorRegion *region = XcolorRegionFetch(dpy, win, &nRegions);

	/* Security check to ensure that the client doesn't try to delete regions
	 * beyond the stack end. */
	if (start + count > nRegions) {
		XFree(region);
		return -1;
	}

	/* Remove the regions and close the gap. */
	memmove(region + start, region + start + count, (nRegions - start - count) * sizeof(XcolorRegion));

  if(nRegions - count)
  	result = !XcmChangeProperty_(dpy, win, netColorRegions, PropModeReplace, (unsigned char *) region, (nRegions - count) * sizeof(XcolorRegion));
  else
    result = !XDeleteProperty( dpy, win, netColorRegions );

  XFree(region);


	return result;
}

int XcolorRegionActivate(Display *dpy, Window win, unsigned long start, unsigned long count)
{
	XWindowAttributes xwa;
	Status status;
	int result;

	/* Construct the XEvent. */
	XClientMessageEvent event;

	event.type = ClientMessage;
	event.window = win;
	event.message_type = XInternAtom(dpy, "_ICC_COLOR_MANAGEMENT", False);
	event.format = 32;

	event.data.l[0] = start;
	event.data.l[1] = count;

	/* The ClientMessage has to be sent to the root window. Find the root window
	 * of the screen containing 'win'. */
        status = XGetWindowAttributes(dpy, RootWindow(dpy, 0), &xwa);
	if (status == 0)
		return -1;

	/* Uhm, why ExposureMask? */
	result = XSendEvent(dpy, xwa.root, False, ExposureMask, (XEvent *) &event);

	return result;
}

static unsigned char * XcmFetchProperty(Display *dpy, Window w, Atom prop, Atom type, unsigned long *n, Bool del)
{
  Atom actual;
  int format;
  unsigned long left;
  unsigned char *data;
  int result;

  XFlush( dpy );

  result = XGetWindowProperty( dpy, w, prop, 0, ~0, del, type,
                               &actual, &format, n, &left, &data);
  if (result == Success)
    return data;

  return NULL;
}

#define DS(format, ...) XcmMessage_p( XCME_MSG_DISPLAY_STATUS, 0, format, \
                                         __VA_ARGS__)
int    XcmColorServerCapabilities    ( Display *dpy )
{
  int active = 0;
  unsigned long n = 0;
  unsigned char * data = 0;
  Atom iccColorDesktop = XInternAtom(dpy, XCM_COLOR_DESKTOP, False);

  data = XcmFetchProperty( dpy, RootWindow(dpy,0),
                           iccColorDesktop, XA_STRING, &n, False);
  if(data && n && strlen((char*)data))
  {
    int old_pid = 0;
    long atom_last_time = 0;
    char * atom_time_text = (char*)malloc(1024),
         * atom_colour_server_name = (char*)malloc(1024),
         * atom_capabilities_text = (char*)malloc(1024);

    atom_time_text[0]= atom_colour_server_name[0]= atom_capabilities_text[0]= 0;

    sscanf( (const char*)data, "%d %ld %s %s",
              &old_pid, &atom_last_time,
              atom_capabilities_text, atom_colour_server_name );
    if(atom_capabilities_text[0])
    {
      if(strstr(atom_capabilities_text, "|ICP|"))
        active |= XCM_COLOR_SERVER_PROFILES;
      if(strstr(atom_capabilities_text, "|ICR|"))
        active |= XCM_COLOR_SERVER_REGIONS;
      if(strstr(atom_capabilities_text, "|ICA|"))
        active |= XCM_COLOR_SERVER_DISPLAY_ADVANCED;
      if(strstr(atom_capabilities_text, "|ICM|"))
        active |= XCM_COLOR_SERVER_MANAGEMENT;
      if(strstr(atom_capabilities_text, "|V0.3|"))
        active |= XCM_COLOR_SERVER_03;
      if(strstr(atom_capabilities_text, "|V0.4|"))
        active |= XCM_COLOR_SERVER_04;
    }
    if(*xcm_debug)
      DS( "XCM_COLOR_DESKTOP: %s", atom_capabilities_text );
    free(atom_capabilities_text);
    free(atom_time_text);
    free(atom_colour_server_name);
  } else
    if(*xcm_debug)
      DS( "XCM_COLOR_DESKTOP: %s", "---" );
  return active;
}

