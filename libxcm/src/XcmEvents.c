/** XcmEvents.c
 *
 *  X11 Colour Management Event observation
 *
 *  @par License: 
 *             MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @par Copyright:
               2009-2011 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 */


#ifdef __cplusplus
extern "C" {
#endif
#include <X11/Xutil.h>
#include "Xcm.h"
#include "XcmEdidParse.h"
#include "XcmEvents.h"
#include "XcmInternal.h"

#define __USE_POSIX2 1
#include <math.h>   /* modf() */
#include <stdio.h>  /* popen() */
#include <string.h>
#include <stdlib.h>
#include <signal.h> /* signal */
#include <time.h>
#include <stdarg.h>

#include <X11/extensions/Xfixes.h>
#include <X11/Xproto.h>
#ifdef __cplusplus
}
#endif


const char * xcmPrintTime            ( );
#define M(code, context, format, ...) XcmMessage_p( code,context, format, \
                                                       __VA_ARGS__)
#define DE(format, ...) { XcmMessage_p( XCME_MSG_DISPLAY_EVENT, 0, "%s " format, xcmPrintTime(), \
                                         __VA_ARGS__); result = 0; }
#define DERR(format, ...) XcmMessage_p( XCME_MSG_DISPLAY_ERROR, 0, format, \
                                         __VA_ARGS__)
#define DS(format, ...) XcmMessage_p( XCME_MSG_DISPLAY_STATUS, 0, format, \
                                         __VA_ARGS__)
#define S(format, ...) XcmMessage_p( XCME_MSG_SYSTEM, 0, format, __VA_ARGS__ )

#ifdef STRING_ADD
#undef STRING_ADD
#endif
#define STRING_ADD(t, txt) XcmStringAdd_( &t, txt, malloc, free )

/** \addtogroup XcmEvents X Color Management Event API's

 *  @{
 *
 *  The observer context can be allocated and setup through 
 *  XcmeContext_Create.  You can pass in a own X11 Display handle through
 *  XcmeContext_New(), XcmeContext_DisplaySet() and XcmeContext_Setup().
 *  The events can be processed by passing all XEvents to XcmeContext_InLoop()
 *  from inside your X event handling loop.
 *  To release allocated resources call XcmeContext_Release().
 *  For a examle look at the xcmsevents application, which is distributed along
 *  the sources.
 */

struct XcmeContext_s_ {
  int type;
  intptr_t a_dummy;
  intptr_t b_dummy;
  intptr_t c_dummy;
  Display * display;
  int display_is_owned;
  int screen;
  Window root;
  int nWindows;
  Window * Windows;
  Window w;
  pid_t old_pid;
  Atom aProfile, aOutputs, aCM, aRegion, aDesktop, aAdvanced, aNetDesktopGeometry;
};

static inline XcolorProfile *
         XcolorProfileNext           ( XcolorProfile     * profile );
static inline unsigned long 
         XcolorProfileCount          ( void              * data,
                                       unsigned long       nBytes);
int      myXErrorHandler             ( Display           * display,
                                       XErrorEvent       * e );


/** Function XcmMessage
 *  @brief   default message function
 *
 *  Messages are printed out to stdout console text stream.
 *
 *  @return                            - 0: fine
 *                                     - 1: error
 *
 *  @version libXcm: 0.3.0
 *  @since   2008/04/03 (libXcm: 0.3.0)
 *  @date    2010/10/01
 */
int            XcmMessage            ( XCME_MSG_e          code,
                                       const void        * context XCM_UNUSED,
                                       const char        * format,
                                       ... )
{
  char * text = 0;
  va_list list;
  int i,len;
  size_t sz = 0;

  if(code == XCME_MSG_INFO)
    return 0;

  va_start( list, format);
  len = vsnprintf( text, sz, format, list);
  va_end  ( list );

  {
    text = calloc( sizeof(char), len + 1 );
    if(!text)
    {
      fprintf(stderr,
      "Xcm_events.c:93 XcmMessage() Could not allocate 256 byte of memory.\n");
      return 1;
    }

    va_start( list, format);
    len = vsnprintf( text, len+1, format, list);
    va_end  ( list );
  }

  switch(code)
  {
    case XCME_MSG_DISPLAY_ERROR:
         fprintf( stdout, "!!! ERROR");
         break;
    case XCME_MSG_TITLE:
    case XCME_MSG_COPYRIGHT:
    case XCME_MSG_INFO:
    case XCME_MSG_DISPLAY_EVENT:
    case XCME_MSG_DISPLAY_STATUS:
    case XCME_MSG_SYSTEM:
         /* nothing to add */
         break;
  }

  i = 0;
  while(text[i])
    fputc(text[i++], stdout);
  fprintf( stdout, "\n" );

  free( text );

  return 0;
}
XcmMessage_f XcmMessage_p = XcmMessage;
/** @brief set a message function to customise messages */
int            XcmMessageFuncSet     ( XcmMessage_f        message_func )
{
  XcmMessage_p = message_func;
  return 0;
}

int xcm_debug_local_ = 0;
int * xcm_debug = &xcm_debug_local_;

/** @brief   set own debug variable */
void       XcmDebugVariableSet       ( int               * debug )
{ xcm_debug = debug; }

static char * net_color_desktop_text = 0;
char * printfNetColorDesktop ( XcmeContext_s * c, int verbose )
{
  Atom actual;
  int format;
  unsigned long left, n;
  unsigned char * data = 0;

  if(!net_color_desktop_text)
    net_color_desktop_text = (char*) malloc(1024);

  net_color_desktop_text[0] = 0;

  XGetWindowProperty( c->display, RootWindow(c->display,0),
                      c->aDesktop, 0, ~0, False, XA_STRING,
                      &actual,&format, &n, &left, &data );
  n += left;
  if(n && data)
  {
    int old_pid = 0;
    long atom_last_time = 0;
    char * atom_time_text = (char*)malloc(1024),
         * atom_colour_server_name = (char*)malloc(1024),
         * atom_capabilities_text = (char*)malloc(1024);
    struct tm * gmt;

    if(!atom_time_text ||
       !atom_colour_server_name ||
       !atom_capabilities_text)
      return NULL;

    atom_time_text[0]= atom_colour_server_name[0]= atom_capabilities_text[0]= '\000';

    if(n && data && strlen((char*)data))
    {
      time_t time;
      sscanf( (const char*)data, "%d %ld %s %s",
              &old_pid, &atom_last_time,
              atom_capabilities_text, atom_colour_server_name );
      time = atom_last_time;
      gmt = gmtime(&time);
      strftime(atom_time_text, 24, /*"%Y/%m/%d."*/"%H%M%S", gmt);
    }

    c->old_pid = (pid_t)old_pid;
    if(verbose)
    {
      sprintf( net_color_desktop_text, "%d %s[%s] %s",
               (int)c->old_pid, atom_colour_server_name, atom_capabilities_text,
               atom_time_text );
    }
    else
      sprintf( net_color_desktop_text, "%d %s",
               (int)c->old_pid, atom_capabilities_text );

    free(atom_time_text);
    free(atom_colour_server_name);
    free(atom_capabilities_text);
  }
  else
    sprintf( net_color_desktop_text, "0" );

  return net_color_desktop_text;
}


/** @brief     return a short window description text */
const char * XcmePrintWindowName( Display * display, Window w )
{
  static char * text = 0;
  Window root_return;
  int x_return, y_return;
  unsigned int width_return, height_return;
  unsigned int border_width_return;
  unsigned int depth_return;
  int screen = DefaultScreen( display );
  Window root = XRootWindow( display, screen );
  int dest_x_return, dest_y_return;
  Window child_return;
  Atom actual = 0;
  int format = 0;
  unsigned long left = 0, n = 0;
  unsigned char * data = 0;

  if(!text) text = (char*)malloc(1024);

  XGetGeometry( display, w, &root_return,
                        &x_return, &y_return, &width_return, &height_return,
                        &border_width_return, &depth_return );

  XTranslateCoordinates( display, w, root, x_return, y_return,
                         &dest_x_return, &dest_y_return, &child_return );

  XGetWindowProperty( display, w,
                      XInternAtom(display, "WM_NAME", False),
                      0, ~0, False, XA_STRING,
                      &actual, &format, &n, &left, &data );

  if(!n || !data)
    XGetWindowProperty( display, w,
                        XInternAtom(display, "_NET_WM_NAME", False),
                        0, ~0, False, AnyPropertyType,
                        &actual, &format, &n, &left, &data );

  if( RootWindow( display, DefaultScreen( display ) ) == w )
    sprintf( text, "root window" );
  else
    sprintf( text, "%dx%d%s%d%s%d \"%s\"", width_return, height_return,
             dest_x_return<0?"":"+", dest_x_return,
             dest_y_return<0?"":"+", dest_y_return,
             data?(char*)data:"" );

  if(data)
    XFree( data );

  return text;
}

XcmICCprofileGetFromMD5_f XcmICCprofileGetFromMD5_p = 0;
XcmICCprofileGetName_f XcmICCprofileGetName_p = 0;

/** Function XcmICCprofileGetNameFuncSet
 *  @brief   plugin more informations capabilities for ICC profiles
 *
 *  @param[in]     getName             get internal and external profile name
 *
 *  @version libXcm: 0.3.0
 *  @since   2010/10/01 (libXcm: 0.3.0)
 *  @date    2010/10/02
 */
void           XcmICCprofileGetNameFuncSet (
                                       XcmICCprofileGetName_f getName )
{
  XcmICCprofileGetName_p = getName;
}

/** Function XcmICCprofileFromMD5FuncSet
 *  @brief   plugin more informations capabilities for ICC profiles
 *
 *  @param[in]     fromMD5             a function to resolve a ICC profile in 
 *                                     ICC profile paths from a md5 hash sum
 *
 *  @version libXcm: 0.3.0
 *  @since   2010/10/01 (libXcm: 0.3.0)
 *  @date    2010/10/02
 */
void           XcmICCprofileFromMD5FuncSet (
                                       XcmICCprofileGetFromMD5_f fromMD5 )
{
  XcmICCprofileGetFromMD5_p = fromMD5;
}


/** Function XcmePrintWindowRegions
 *  @brief   provide info text about window regions
 *
 *  The function informs about _ICC_COLOR_REGIONS atom.
 *
 *  @param[in]     display             X display
 *  @param[in]     w                   X window
 *  @param[in]     always              send always a message, even for a empty
 *                                     property
 *
 *  @version libXcm: 0.4.3
 *  @since   2009/00/00 (libXcm: 0.3.0)
 *  @date    2011/10/01
 */
const char * XcmePrintWindowRegions  ( Display           * display,
                                       Window              w,
                                       int                 always )
{
  unsigned long n = 0;
  int i, j;
  XcolorRegion * regions = 0;
  static char * text = 0;

  regions = XcolorRegionFetch( display, w, &n );

  if(!always && !n)
    return text;

  if(!text) text = (char*)malloc(256*(1+n));
  if(!text) return text;
  text[0] = 0;

  sprintf( &text[strlen(text)], "PropertyNotify : %s    vvvvv      %s %d\n",
      XGetAtomName( display, 
                    XInternAtom( display,XCM_COLOR_REGIONS, False)),
      XcmePrintWindowName( display, w ), (int)n );

          for(i = 0; i < (int)n; ++i)
          {
            int nRect = 0;
            XRectangle * rect = 0;
            uint32_t * md5 = 0;
            void * icc_data = 0;
            size_t icc_data_size = 0;
            char * name = 0;

            if(!regions[i].region)
            {
              DERR("server region id with zero: left %d", (int)n-i);
              break;
            }

            rect = XFixesFetchRegion( display, ntohl(regions[i].region),
                                      &nRect );
            md5 = (uint32_t*)&regions[i].md5[0];
            if(XcmICCprofileGetFromMD5_p)
            {
              icc_data = XcmICCprofileGetFromMD5_p( md5, &icc_data_size,
                                                    malloc );
              if(XcmICCprofileGetName_p && icc_data_size && icc_data)
                name = XcmICCprofileGetName_p( icc_data, icc_data_size,
                                               malloc, 0);
            }

            sprintf( &text[strlen(text)], "    %d local look up: %s[%x%x%x%x]:\n", i, name?name:"???",
                   md5[0], md5[1], md5[2], md5[3] );
            for(j = 0; j < nRect; ++j)
            sprintf( &text[strlen(text)], "        %dx%d+%d+%d\n",
                   rect[j].width, rect[j].height, rect[j].x, rect[j].y );

            if(icc_data_size && icc_data)
              free(icc_data);
            if(name)
              free(name);
          }

  return text;
}

/** Function xcmePrintWindowRegions
 *  @brief   send a message about window regions
 *
 *  The function informs about _ICC_COLOR_REGIONS atom.
 *
 *  @param[in]     display             X display
 *  @param[in]     w                   X window
 *  @param[in]     always              send always a message, even for a empty
 *                                     property
 *
 *  @version libXcm: 0.3.0
 *  @since   2009/00/00 (libXcm: 0.3.0)
 *  @date    2010/10/01
 */
void     xcmePrintWindowRegions      ( Display           * display,
                                       Window              w,
                                       int                 always )
{
  unsigned long n = 0;
  int i, j;
  int result XCM_UNUSED = -1;
  XcolorRegion * regions = 0;

  regions = XcolorRegionFetch( display, w, &n );

  if(!always && !n)
    return;

  DE( "PropertyNotify : %s    vvvvv      %s %d",
      XGetAtomName( display, 
                    XInternAtom( display,XCM_COLOR_REGIONS, False)),
      XcmePrintWindowName( display, w ), (int)n );

          for(i = 0; i < (int)n; ++i)
          {
            int nRect = 0;
            XRectangle * rect = 0;
            uint32_t * md5 = 0;
            void * icc_data = 0;
            size_t icc_data_size = 0;
            char * name = 0;

            if(!regions[i].region)
            {
              DERR("server region id with zero: left %d", (int)n-i);
              break;
            }

            rect = XFixesFetchRegion( display, ntohl(regions[i].region),
                                      &nRect );
            md5 = (uint32_t*)&regions[i].md5[0];
            if(XcmICCprofileGetFromMD5_p)
            {
              icc_data = XcmICCprofileGetFromMD5_p( md5, &icc_data_size,
                                                    malloc );
              if(XcmICCprofileGetName_p && icc_data_size && icc_data)
                name = XcmICCprofileGetName_p( icc_data, icc_data_size,
                                               malloc, 0);
            }

            DE("    %d local look up: %s[%x%x%x%x]:", i, name?name:"???",
                   md5[0], md5[1], md5[2], md5[3] );
            for(j = 0; j < nRect; ++j)
            DE("        %dx%d+%d+%d",
                   rect[j].width, rect[j].height, rect[j].x, rect[j].y );

            if(icc_data_size && icc_data)
              free(icc_data);
            if(name)
              free(name);
          }
}

/* code from Tomas Carnecky */
static inline XcolorProfile *XcolorProfileNext(XcolorProfile *profile)
{
  unsigned char *ptr = (unsigned char *) profile;
  return (XcolorProfile *) (ptr + sizeof(XcolorProfile) + ntohl(profile->length));
}
 
static inline unsigned long XcolorProfileCount(void *data, unsigned long nBytes)
{
  unsigned long count = 0;
  XcolorProfile * ptr;

  for (ptr = (XcolorProfile*)data;
       (uintptr_t)ptr < (uintptr_t)data + nBytes;
       ptr = XcolorProfileNext(ptr))
    ++count;

  return count;
}
/* end of code from Tomas Carnecky */

/*void mySignalHandler(int sig)
{
  switch(sig) {
  case SIGINT: printf("%s:%d catched SIGINT\n", __FILE__,__LINE__ ); break;
  default: printf("%s:%d catched unknown signal\n", __FILE__,__LINE__ );
  }
}*/

int myXErrorHandler ( Display * display XCM_UNUSED, XErrorEvent * e XCM_UNUSED)
{
  DERR( "%s:%d catched a X11 error\n", 
          strrchr(__FILE__, '/')?strrchr(__FILE__, '/')+1:__FILE__,__LINE__ );
  return 0;
}

/** Function XcmeSelectInput
 *  @brief   register windows
 *
 *  @version libXcm: 0.5.3
 *  @date    2013/01/13
 *  @since   2013/01/13 (libXcm: 0.5.3)
 */
void XcmeSelectInput( XcmeContext_s * c )
{
        unsigned long nWindow = 0;
        Window * windows = 0;
  Atom actual = 0;
  int format = 0;
  unsigned long left = 0, n = 0, i,j;

        XGetWindowProperty( c->display, c->root,
          XInternAtom( c->display, "_NET_CLIENT_LIST", False), 0, ~0, False, XA_WINDOW, &actual, &format,
          &nWindow, &left, (unsigned char**)&windows );
        n = (int)(nWindow + left);

        for(i = 0; i < n; ++i)
        {
          /* search of a previous observation of a particular window */
          int found = 0;
          for(j = 0; (int)j < c->nWindows; ++j)
          {
            if(windows[i] == c->Windows[j])
              found = 1;
          }

          /* other new windows but not own */
          if( c->w != windows[i] &&
              !found )
          {
            /* observe other windows */
            XSelectInput( c->display, windows[i],
                       PropertyChangeMask |  /* Xcolor properties */
                       ExposureMask );       /* Xcolor client messages */
          }
        }

        if((int)n > c->nWindows)
        {
          if(c->Windows) free(c->Windows);
          c->Windows = (Window*)malloc( sizeof(Window) * n );
        }
        memcpy( c->Windows, windows, sizeof(Window) * n );
        c->nWindows = n;
}


/** Function XcmeContext_New
 *  @brief   allocate a event observer context structure
 *
 *  The context is needed to observe colour management events.
 *  After the allocation the context needs initialisation.
 *
 *  @return                            the context
 *
 *  @version libXcm: 0.3.0
 *  @since   2009/00/00 (libXcm: 0.3.0)
 *  @date    2010/10/01
 */
XcmeContext_s * XcmeContext_New      ( )
{
  XcmeContext_s * c = (XcmeContext_s*) calloc( sizeof(XcmeContext_s), 1 );
  if(!c)
    return c;

  c->type = 1000;
  c->display = 0;
  c->display_is_owned = 0;
  c->screen = -1;
  c->root = 0;
  c->nWindows = 0;
  c->Windows = 0;
  c->w = 0;
  c->old_pid = 0;
  /*c->aProfile, c->aOutputs, c->aCM, c->aRegion, c->aDesktop;, c->aAdvanced*/

  return c;
}

/** Function XcmeContext_Create
 *  @brief   allocate and initialise a event observer context structure
 *
 *  The context is needed to observe colour management events.
 *
 *  @param[in]     display_name        a valid X11 display name
 *  @return                            the context
 *
 *  @version libXcm: 0.3.0
 *  @since   2009/00/00 (libXcm: 0.3.0)
 *  @date    2010/10/01
 */
XcmeContext_s * XcmeContext_Create   ( const char        * display_name )
{
  int error = 0;
  XcmeContext_s * c = XcmeContext_New();
  if(!c)
    return c;

  error = XcmeContext_Setup( c, display_name );
  if(error)
    c = 0;

  return c;
}

int XcmeErrorHandler(Display * display, XErrorEvent * e)
{
  switch (e->request_code)
  {
    case X_QueryTree:
    case X_GetWindowAttributes:
        if (e->error_code == BadWindow) return 0;
        break;
    case X_GetGeometry:
        if (e->error_code == BadDrawable) return 0;
        break;
    default:
    {
        static int length = 256;
        char * buffer_return = malloc(length);
        if(!buffer_return) return 0;
        XGetErrorText(display, e->error_code, buffer_return, length);
        DERR("Xcme: %s", buffer_return);
        free(buffer_return);
    }
  }
  return 0;
}

/** Function XcmeContext_Setup2
 *  @brief   allocate and initialise a event observer context structure
 *
 *  The initialised context is needed for observing colour management events.
 *  No initial events are sent.
 *
 *  @param[in,out] c                   a event observer context
 *                                     A existing X11 display will be honoured.
 *  @param[in]     display_name        a valid X11 display name or NULL;
 *                                     With a existing X11 display inside c,
 *                                     this option will be ignored.
 *  @param[in]     flags               unused
 *
 *  @version libXcm: 0.5.0
 *  @since   2011/10/26 (libXcm: 0.5.0)
 *  @date    2011/10/26
 */
int      XcmeContext_Setup2          ( XcmeContext_s     * c,
                                       const char        * display_name,
                                       int                 flags XCM_UNUSED )
{
  /* Open the display and create our window. */
  Visual * vis = 0;
  Colormap cmap = 0;
  XSetWindowAttributes attrs;
  Atom actual;
  int format,
      has_display = 0;
  unsigned long left, n;
  unsigned char * data;

  XSetErrorHandler( XcmeErrorHandler );

  if(c->display)
    has_display = 1;
  else
  {
    c->display = XOpenDisplay( display_name );
    c->display_is_owned = 1;
  }
  if(!c->display)
  {
    DERR( "could not open display %s", display_name?display_name:"???" );
    return 1;
  }

  c->screen = DefaultScreen( c->display );
  c->root = XRootWindow( c->display, c->screen );

  /* define the observers interesst */
  c->aProfile = XInternAtom( c->display, XCM_COLOR_PROFILES, False );
  c->aOutputs = XInternAtom( c->display, XCM_COLOR_OUTPUTS, False );
  c->aCM = XInternAtom( c->display, "_ICC_COLOR_MANAGEMENT", False );
  c->aRegion = XInternAtom( c->display, XCM_COLOR_REGIONS, False );
  c->aDesktop = XInternAtom( c->display, XCM_COLOR_DESKTOP, False );
  c->aAdvanced = XInternAtom(c->display, XCM_COLOUR_DESKTOP_ADVANCED,False);
  c->aNetDesktopGeometry = XInternAtom( c->display, "_NET_DESKTOP_GEOMETRY", False );

  if(!has_display)
  {
    char * strings[] = {"XcmEvents",NULL};
    XTextProperty text;

    vis = DefaultVisual( c->display, c->screen );
    cmap = XCreateColormap( c->display, c->root, vis, AllocNone );
    attrs.colormap = cmap;
    attrs.border_pixel = 0;
    attrs.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask |
                       PropertyChangeMask;

    c->w = XCreateWindow( c->display, c->root, 0, 0, 300, 300, 5, 24,
                          InputOutput, DefaultVisual( c->display, c->screen ),
                          CWBorderPixel | CWColormap | CWEventMask, &attrs);

    if(XStringListToTextProperty( strings, 1, &text))
      XSetWMName( c->display, c->w, &text );
    /*XMapWindow( display, w );*/
  }

  /* check if we can see other clients */
  XGetWindowProperty( c->display, RootWindow(c->display,0),
                      XInternAtom(c->display, "_NET_CLIENT_LIST", False),
                      0, ~0, False, XA_WINDOW,
                      &actual,&format, &n, &left, &data );
  if(!data || !n)
    DERR( "\nThe extented ICCCM hint _NET_CLIENT_LIST atom is %s\n"
          "!!! xcmevents will work limited !!!\n", n ? "missed" : "zero" );

  /* observe the root window as well for newly appearing windows */
  XSelectInput( c->display, c->root,
                PropertyChangeMask |   /* _ICC_COLOR_PROFILES */
                ExposureMask );        /* _ICC_COLOR_MANAGEMENT */

  /* observe windows */
  XcmeSelectInput( c );

  return 0;
}

/** Function XcmeContext_Setup
 *  @brief   allocate and initialise a event observer context structure
 *
 *  The initialised context is needed for observing colour management events.
 *
 *  @param[in,out] c                   a event observer context
 *                                     A existing X11 display will be honoured.
 *  @param[in]     display_name        a valid X11 display name or NULL;
 *                                     With a existing X11 display inside c,
 *                                     this option will be ignored.
 *
 *  @version libXcm: 0.4.1
 *  @since   2009/00/00 (libXcm: 0.3.0)
 *  @date    2011/05/06
 */
int      XcmeContext_Setup           ( XcmeContext_s    * c,
                                       const char        * display_name )
{
  /* Open the display and create our window. */

  XcmeContext_Setup2(c, display_name, 0);

  /* print some general information */
  M( XCME_MSG_TITLE, 0,
     "libXcm based X11 colour management system events observer%s", "");
  M( XCME_MSG_COPYRIGHT, 0,
     "(c) 2009-2022 - Kai-Uwe Behrmann  License: MIT%s", "" );
  DS( "atom: \"" XCM_COLOR_PROFILES "\": %d", (int)c->aProfile );
  DS( "atom: \"" XCM_COLOR_OUTPUTS "\": %d", (int)c->aOutputs );
  DS( "atom: \"_ICC_COLOR_MANAGEMENT\": %d", (int)c->aCM );
  DS( "atom: \"" XCM_COLOR_REGIONS "\": %d", (int)c->aRegion );
  DS( "atom: \"" XCM_COLOUR_DESKTOP_ADVANCED "\": %d", (int)c->aAdvanced );
  DS( "atom: \"" XCM_COLOR_DESKTOP "\": %d %s", (int)c->aDesktop,
                                          printfNetColorDesktop(c, 0) );


  DS( "root window ID: %d", (int)c->root );
  {
    FILE * fp;
    size_t size = 256, i = 0;
    char * txt;
    S( "running \"oyranos-monitor -lc\":%s", "" );
    fp = popen(  "oyranos-monitor -lc", "r" );
    if( fp )
    {
      int c;
      txt = (char*) malloc( size + 1 );
      while( (c = getc(fp)) != 0 && c != EOF && i < size-1 )
      {
        txt[i] = c;
        ++i;
      }
      if(txt[i-1] == '\n')
        txt[i-1] = 0;
      else
        txt[i] = 0;
      S( "%s", txt );
      free( txt ); txt = 0;
      pclose( fp );
    }
  }

  /* tell about existing regions */
  {
    Window root_return = 0,
           parent_return = 0, 
         * children_return = 0,
         * wins = 0;
    unsigned int nchildren_return = 0, wins_n = 0;
    int i;
    XWindowAttributes window_attributes_return;

    XSync( c->display, 0 );
    XQueryTree( c->display, c->root,
                         &root_return, &parent_return,
                         &children_return, &nchildren_return );
    wins = (Window*)malloc(sizeof(Window) * nchildren_return );
    memcpy( wins, children_return, sizeof(Window) * nchildren_return );
    XFree( children_return );
    children_return = wins; wins = 0;

    for(i = nchildren_return - 1; i >= 0; --i)
    {
      root_return = 0;
      XQueryTree( c->display, children_return[i],
                           &root_return, &parent_return,
                           &wins, &wins_n );
      XGetWindowAttributes( c->display, children_return[i],
                                     &window_attributes_return );
      if(window_attributes_return.map_state == IsViewable &&
         parent_return == c->root)
      {
        xcmePrintWindowRegions( c->display, children_return[i], 0 );
        /*printf( "[%d] \"%s\"\n", (int)children_return[i],
                     XcmePrintWindowName(display, children_return[i]) );*/

      }

      XFree( wins );
    }

    free( children_return );
  }

  return 0;
}

char *       XcmStringCopy_          ( const char        * string,
                                       void              *(allocate_func) (size_t) )
{
  char * t = allocate_func( strlen( string ) + 1 );
  strcpy( t, string );
  return t;
}

void         XcmStringAdd_           ( char             ** text,
                                       const char        * append,
                                       void              *(allocate_func)(size_t),
                                       void               (dealloc_func)(void*))
{
  char * text_copy = NULL;
  int len = 0;

  if(text && *text)
    len += strlen(*text);
  if(append)
    len += strlen(append);

  text_copy = allocate_func(len + 1);
  if(text_copy)
    sprintf( text_copy, "%s%s", (text && *text)?*text:"", append?append:"" );

  if(text)
  {
    if(*text && dealloc_func)
      dealloc_func(*text);

    *text = text_copy;
  }
}

void         xcmeUnrollEdid1_       ( void              * edid,
                                       char             ** manufacturer,
                                       char             ** mnft,
                                       char             ** model,
                                       char             ** serial,
                                       char             ** vendor,
                                       uint32_t          * week,
                                       uint32_t          * year,
                                       uint32_t          * mnft_id,
                                       uint32_t          * model_id,
                                       double            * c,
                                       void              *(allocate_func) (size_t) )
{
  int i, count = 0;
  XcmEdidKeyValue_s * list = 0;
  XCM_EDID_ERROR_e err = 0;

  err = XcmEdidParse( edid, &list, &count );
  if(err != XCM_EDID_OK)
    DERR( "%s", XcmEdidErrorToString(err) );

  if(list)
  for(i = 0; i < count; ++i)           
  {
         if(manufacturer && strcmp( list[i].key, "manufacturer") == 0)
      *manufacturer = XcmStringCopy_(list[i].value.text, allocate_func);
    else if(mnft && strcmp( list[i].key, "mnft") == 0)
      *mnft = XcmStringCopy_(list[i].value.text, allocate_func);
    else if(model && strcmp( list[i].key, "model") == 0)
      *model = XcmStringCopy_(list[i].value.text, allocate_func);
    else if(serial && strcmp( list[i].key, "serial") == 0)
      *serial = XcmStringCopy_(list[i].value.text, allocate_func);
    else if(vendor && strcmp( list[i].key, "vendor") == 0)
      *vendor = XcmStringCopy_(list[i].value.text, allocate_func);
    else if(week && strcmp( list[i].key, "week") == 0)
      *week = list[i].value.integer;    
    else if(year && strcmp( list[i].key, "year") == 0)
      *year = list[i].value.integer; 
    else if(mnft_id && strcmp( list[i].key, "mnft_id") == 0)
      *mnft_id = list[i].value.integer;
    else if(model_id && strcmp( list[i].key, "model_id") == 0)
      *model_id = list[i].value.integer;
    else if(c && strcmp( list[i].key, "redx") == 0)
      c[0] = list[i].value.dbl;
    else if(c && strcmp( list[i].key, "redy") == 0)
      c[1] = list[i].value.dbl;
    else if(c && strcmp( list[i].key, "greenx") == 0)
      c[2] = list[i].value.dbl;
    else if(c && strcmp( list[i].key, "greeny") == 0)
      c[3] = list[i].value.dbl;
    else if(c && strcmp( list[i].key, "bluex") == 0)
      c[4] = list[i].value.dbl;
    else if(c && strcmp( list[i].key, "bluey") == 0)
      c[5] = list[i].value.dbl;
    else if(c && strcmp( list[i].key, "whitex") == 0)
      c[6] = list[i].value.dbl;
    else if(c && strcmp( list[i].key, "whitey") == 0)
      c[7] = list[i].value.dbl;
    else if(c && strcmp( list[i].key, "gamma") == 0)
      c[8] = list[i].value.dbl;
  }

  if(list)
    XcmEdidFree( &list );
}

/** Function XcmeContext_Release
 *  @brief   clean owned resources
 *
 *  @param[in,out] c                   a event observer context
 *
 *  @version libXcm: 0.3.0
 *  @since   2009/00/00 (libXcm: 0.3.0)
 *  @date    2010/10/01
 */
int      XcmeContext_Release         ( XcmeContext_s   ** c )
{
  XcmeContext_s * s = 0;

  if(*c)
    s = *c;

  if(s)
  {
    if(s->display_is_owned)
    {
      XDestroyWindow( s->display, s->w );
      XCloseDisplay( s->display );
    }

    free(s);

    *c = NULL;
  }
  return 0;
}

/** Function XcmeContext_InLoop
 *  @brief   check for colour management events
 *
 *  This function needs to be called inside the X11 event loop, to observe
 *  the related events and send messages about them.
 *
 *  @param[in,out] c                   a event observer context
 *  @param[in]     event               a X event handle
 *  @return                            - 0: success
 *                                     - -1: nothing to do
 *                                     - 1: error
 *
 *  @version libXcm: 0.3.0
 *  @since   2009/00/00 (libXcm: 0.3.0)
 *  @date    2010/10/01
 */
int      XcmeContext_InLoop          ( XcmeContext_s    * c,
                                       XEvent            * event )
{
  int result = -1;

  /* observe events */
  {
    Display *display = event->xany.display;
    Atom actual;
    int format;
    unsigned long left, n;
    unsigned char * data;
    char * actual_name = 0;

    if( event->xany.window == c->w && event->type == Expose &&
        c->display_is_owned )
    {
      /* draw something */
      XFillRectangle( display, c->w, DefaultGC( display, c->screen),
                      0, 0, 300, 300);

    } else if( event->type == PropertyNotify )
    {
      Atom atom = event->xproperty.atom;

      actual_name = XGetAtomName( display, atom );

      if(display != c->display)
      {
        DE( "PropertyNotify : event and context displays are different: %s",
               actual_name );
        result = 1;
      }
        
      actual = 0;
      format = 0;
      left = 0; n = 0;
      data = 0;

      /* --- report --- */
      if(c->w != event->xany.window)
      {
        if(event->xproperty.atom == c->aProfile ||
           event->xproperty.atom == c->aCM ||
           event->xproperty.atom == c->aRegion ||
           event->xproperty.atom == c->aDesktop ||
           event->xproperty.atom == c->aNetDesktopGeometry ||
           strstr( actual_name, XCM_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE) != 0 ||
           strstr( actual_name, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE) != 0 ||
           strstr( actual_name, "EDID") != 0)
        XGetWindowProperty( display, event->xany.window,
               event->xproperty.atom, 0, ~0, False, XA_CARDINAL,&actual,&format,
                &n, &left, &data );
        n += left;

        if(event->xproperty.atom == c->aAdvanced)
        {
          XGetWindowProperty( display, event->xany.window,
               event->xproperty.atom, 0, ~0, False, XA_STRING,&actual,&format,
                &n, &left, &data );
          result = 0;
        }
        n += left;

        if       ( event->xproperty.atom == c->aOutputs )
        {
          char * text;

          XGetWindowProperty( display, event->xany.window,
               event->xproperty.atom, 0, ~0, False, XA_STRING, &actual, &format,
                &n, &left, &data );
          n += left;
          text = (char*) "----"; /* TODO: data; */
          DE("PropertyNotify : %s     \"%s\"  %s",
               actual_name,
               text, XcmePrintWindowName( display, event->xany.window ) );

        } else if( event->xproperty.atom == c->aProfile )
        {
          unsigned long count = XcolorProfileCount(data, n);
          DE( "PropertyNotify : %s   %d         %s",
               actual_name,
               (int)count, XcmePrintWindowName( display, event->xany.window ) );

        } else if( event->xproperty.atom == c->aCM )
        {
          /* should not happen */

        } else if( event->xproperty.atom == c->aDesktop )
        {
          DE( "PropertyNotify : %s    %s          %s",
               actual_name,
               event->xproperty.state ? "0 - removed" :
               printfNetColorDesktop(c, 1),
               XcmePrintWindowName( display, event->xany.window ) );

        } else if( event->xproperty.atom == c->aRegion )
        {
          xcmePrintWindowRegions( display, event->xany.window, 1 );
          result = 0;

        } else if(
           strstr( actual_name, XCM_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE) != 0 ||
           strstr( actual_name, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE) != 0 ||
           strstr( actual_name, "EDID") != 0)
        {
          const char * an = actual_name, * name = 0;
          char * tmp = 0, * name_alloced = 0;
          double colours[9] = {0,0,0,0,0,0,0,0,0};

          if(strcmp( XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE, an ) == 0)
            an = XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE"  ";

          if(n &&
             (strstr( actual_name, XCM_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE) != 0 ||
              strstr( actual_name, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE) != 0)
             && strstr( "ICC_PROFILE_IN_X", actual_name) == 0)
          {
            if(XcmICCprofileGetName_p)
            {
              name_alloced = XcmICCprofileGetName_p( data, n, malloc, 1 );
              name = name_alloced;
            }
            if(name && strchr(name, '/'))
              name = strrchr( name, '/' ) + 1;
            else if(!name && XcmICCprofileGetName_p)
            {
              name_alloced = XcmICCprofileGetName_p( data, n, malloc, 0 );
              name = name_alloced;
            }
            else if(!name)
              name = "????";
          }
          if(n &&
             strstr( actual_name, "EDID") != 0)
          {
            char * manufacturer = 0,
                 * model = 0,
                 * serial = 0;

            xcmeUnrollEdid1_( data, &manufacturer,0,&model, &serial, 0,0,0,0,0,
                            colours, malloc );
            STRING_ADD( tmp, manufacturer ); STRING_ADD( tmp, " - " );
            STRING_ADD( tmp, model ); STRING_ADD( tmp, " - " );
            STRING_ADD( tmp, serial ); STRING_ADD( tmp, "\n  " );
          }
          DE(   "PropertyNotify : %s    \"%s\"[%d]  %s",
                 an, name?name:(tmp?"set":"removed"),(int)n,
                 XcmePrintWindowName( display, event->xany.window ) );

          if(tmp)
          {
            DE("  %s", tmp);
            DE("  {{%.03f,%.03f}{%.03f,%.03f}{%.03f,%.03f}{%.03f,%.03f}%.03f}",
                   colours[0], colours[1], colours[2], colours[3],
                   colours[4], colours[5], colours[6], colours[7], colours[8] );
            free(tmp); tmp = 0;
          }
          if(name_alloced)
            free(name_alloced);

        } else if( event->xproperty.atom == c->aAdvanced )
        {
          DE( "PropertyNotify : %s   %s   %s",
               actual_name,
               data, XcmePrintWindowName( display, event->xany.window ) );
        } else if( event->xproperty.atom == c->aNetDesktopGeometry )
        {
          unsigned long * geo = (unsigned long*) data;
          DE( "PropertyNotify : %s   %lux%lu  %s",
               actual_name,
               geo[0], geo[1],
               XcmePrintWindowName( display, event->xany.window ) );
        }

        if(data) XFree(data);
      }


      /* claim interesst in other windows events */
      if( c->w != event->xany.window &&
          XInternAtom( display, "_NET_CLIENT_LIST", False) ==
          event->xproperty.atom )
      {
        XcmeSelectInput( c );
        result = 0;
      }
      XFree( actual_name ); actual_name = 0;

    } else if( event->type == ClientMessage )
    {
      if(event->xclient.message_type == c->aCM )
      {
        /* --- report --- */
        unsigned long active[2];
        actual_name = 
                  XGetAtomName( display, event->xclient.message_type);

        active[0] = event->xclient.data.l[0];
        active[1] = event->xclient.data.l[1];
        DE( "ClientMessage  : %s %ld %ld        %s",
                actual_name, active[0], active[1],
                XcmePrintWindowName( display, event->xclient.window ) );
        XFree( actual_name ); actual_name = 0;
      }
    }
  }
  return result;
}

/** Function XcmeContext_DisplayGet
 *  @brief   return the Display
 *
 *  @param[in]     c                   a event observer context
 *  @return                            the X11 display
 *
 *  @version libXcm: 0.3.0
 *  @since   2010/10/01 (libXcm: 0.3.0)
 *  @date    2010/10/01
 */
Display *XcmeContext_DisplayGet      ( XcmeContext_s    * c )
{
  return c->display;
}

/** Function XcmeContext_DisplaySet
 *  @brief   set a custom X11 Display
 *
 *  @param[in,out] c                   a event observer context
 *  @param[in]     display             the custom X11 display
 *  @return                            error
 *
 *  @version libXcm: 0.3.0
 *  @since   2010/10/01 (libXcm: 0.3.0)
 *  @date    2010/10/01
 */
int      XcmeContext_DisplaySet      ( XcmeContext_s    * c,
                                       Display           * display )
{
  c->display = display;
  return 0;
}

/** Function xcmeContext_WindowSet
 *  @brief   set a custom X11 Window
 *
 *  @param[in,out] c                   a event observer context
 *  @param[in]     window              the custom X11 window
 *  @return                            error
 *
 *  @version libXcm: 0.3.0
 *  @since   2010/10/01 (libXcm: 0.3.0)
 *  @date    2010/10/01
 */
int      XcmeContext_WindowSet       ( XcmeContext_s    * c,
                                       Window              window )
{
  c->w = window;
  return 0;
}

#if defined(_WIN32) && !defined(__GNU__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h> // portable: uint64_t   MSVC: __int64 

// MSVC defines this in winsock2.h!?
typedef struct timeval {
    long tv_sec;
    long tv_usec;
} timeval;

int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}
# define   TIME_DIVIDER CLOCKS_PER_SEC
#else
# include <time.h>
# include <sys/time.h>
# define   TIME_DIVIDER 10000
# include <unistd.h>
#endif

time_t             xcmTime           ( )
{
  time_t time_;
  double divider = TIME_DIVIDER;
  struct timeval tv;
  double tmp_d;
  gettimeofday( &tv, NULL );
  time_ = tv.tv_usec/(1000000/(time_t)divider)
                   + (time_t)(modf( (double)tv.tv_sec / divider,&tmp_d )
                     * divider*divider);
  return time_;
}
double             xcmSeconds        ( )
{
           time_t zeit_ = xcmTime();
           double teiler = TIME_DIVIDER;
           double dzeit = zeit_ / teiler;
    return dzeit;
}

const char * xcmPrintTime            ( )
{
  static char t[64];
  struct tm * gmt;
  time_t cutime = time(NULL); /* time right NOW */
  gmt = localtime( &cutime );
  t[0] = '\000';

  {
    double tmp_d;
    sprintf( &t[strlen(t)], "[" );
    strftime( &t[strlen(t)], 60, "%F", gmt );
    sprintf( &t[strlen(t)], "T" );
    strftime( &t[strlen(t)], 50, "%H:%M:%S", gmt );
    snprintf( &t[strlen(t)], 44, ".%03d", (int)(modf(xcmSeconds(),&tmp_d)*1000) );
    strftime( &t[strlen(t)], 40, "%z", gmt );
    sprintf( &t[strlen(t)], "]" );
  }
  return t;
}


/** @} XcmEvents */

