/** xcmsevents.c
 *
 *  A small X11 colour management event observer.
 *
 *  License: newBSD
 *  Copyright: 2009-2010 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 *  c++: ar cru liboyranosedid.a ../../modules/devices/oyranos_edid_parse.o ../../modules/devices/oyranos_monitor.o
 *  c++: prefix=/opt/local; g++ -Wall -g -o qcmsevents xcmsevents.c `PKG_CONFIG_PATH=$prefix/lib/pkgconfig pkg-config --cflags --libs x11 xmu xfixes xinerama xrandr xxf86vm oyranos` -DHAVE_X11 -DTARGET=\"xcmsevents\" -I$prefix/include -I../.. -L$prefix/lib -lXcolor -L./ -loyranosedid
 */

#ifdef __cplusplus
extern "C" {
#endif
#include <X11/Xutil.h>
#include <X11/Xmu/WinUtil.h>
#include "Xcolor.h"

#define __USE_POSIX2 1
#include <stdio.h>  /* popen() */
#include <string.h>
#include <stdlib.h>
#include <signal.h> /* signal */

#include <X11/extensions/Xfixes.h>
#include <X11/Xmu/Error.h> /* XmuSimpleErrorHandler */
#ifdef __cplusplus
}
#endif

#include "oyranos_alpha.h" /* use Oyranos to obtain profile names */
#include "oyranos_monitor_internal.h" /* EDID parsing */
#include "oyranos_helper.h" /* allocators */
#include "oyranos_definitions.h" /* ICC Profile in X */

#ifdef __cplusplus
using namespace oyranos;
extern "C" {
}
#endif

#define M(code, context, format, ...) oyMessageFunc_p( code,context, format, \
                                                       __VA_ARGS__)
#define DE(format, ...) oyMessageFunc_p( oyMSG_DISPLAY_EVENT, 0, format, \
                                         __VA_ARGS__)
#define DERR(format, ...) oyMessageFunc_p( oyMSG_DISPLAY_ERROR, 0, format, \
                                         __VA_ARGS__)
#define DS(format, ...) oyMessageFunc_p( oyMSG_DISPLAY_STATUS, 0, format, \
                                         __VA_ARGS__)
#define S(format, ...) oyMessageFunc_p( oyMSG_SYSTEM, 0, format, __VA_ARGS__ )

#ifdef STRING_ADD
#undef STRING_ADD
#endif
#define STRING_ADD(t, txt) oyStringAdd_( &t, txt, malloc, free )

static inline XcolorProfile *
         XcolorProfileNext           ( XcolorProfile     * profile );
static inline unsigned long 
         XcolorProfileCount          ( void              * data,
                                       unsigned long       nBytes);
int      myXErrorHandler             ( Display           * display,
                                       XErrorEvent       * e );

int      xcmseSetup                  ( xcmseContext_s    * c,
                                       const char        * display_name );

static char * net_color_desktop_text = 0;
char * printfNetColorDesktop ( xcmseContext_s * c, int verbose )
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
    long atom_last_time = 0,
         atom_time = 0;
    char * atom_time_text = (char*)malloc(1024),
         * atom_colour_server_name = (char*)malloc(1024);
    if(n && data && strlen((char*)data))
    {
      sscanf( (const char*)data, "%d %ld %s %s",
              &old_pid, &atom_last_time,
              atom_time_text, atom_colour_server_name );
      atom_time = atol( atom_time_text );
    }

    c->old_pid = (pid_t)old_pid;
    if(verbose)
    {
      sprintf( net_color_desktop_text, "%d %s %s",
               (int)c->old_pid, atom_colour_server_name,
               atom_time_text );
    }
    else
      sprintf( net_color_desktop_text, "%d",
               (int)c->old_pid );
    if(atom_time_text) free(atom_time_text);
    if(atom_colour_server_name) free(atom_colour_server_name);
  }
  else
    sprintf( net_color_desktop_text, "0" );

  return net_color_desktop_text;
}

char * printWindowName( Display * display, Window w )
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
  Window child_return, window;
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

  /* a pain to work with that cruft */
  window = XmuClientWindow( display, w );

  XGetWindowProperty( display, window,
                      XInternAtom(display, "WM_NAME", False),
                      0, ~0, False, XA_STRING,
                      &actual, &format, &n, &left, &data );


  if( RootWindow( display, DefaultScreen( display ) ) == w )
    sprintf( text, "root window" );
  else
    sprintf( text, "%dx%d%s%d%s%d %s", width_return, height_return,
             dest_x_return<0?"":"+", dest_x_return,
             dest_y_return<0?"":"+", dest_y_return,
             data?(char*)data:"" );

  return text;
}

void     printWindowRegions          ( Display           * display,
                                       Window              w,
                                       int                 always )
{
  unsigned long n = 0;
  int i, j;
  XcolorRegion * regions = 0;

  /* a pain to work with that cruft */
  w = XmuClientWindow( display, w );

  regions = XcolorRegionFetch( display, w, &n );

  if(!always && !n)
    return;

  DE( "PropertyNotify : %s    vvvvv      %s %d",
      XGetAtomName( display, 
                    XInternAtom( display,"_NET_COLOR_REGIONS", False)),
      printWindowName( display, w ), (int)n );

          for(i = 0; i < (int)n; ++i)
          {
            int nRect = 0;
            XRectangle * rect = 0;
            uint32_t * md5 = 0;
            oyProfile_s * p = 0;
            const char * name = 0;

            if(!regions[i].region)
            {
              DERR("server region id with zero: left %d", (int)n-i);
              break;
            }

            rect = XFixesFetchRegion( display, regions[i].region, &nRect );
            md5 = (uint32_t*)&regions[i].md5[0];
            p = oyProfile_FromMD5( md5, 0 );
            name = oyProfile_GetFileName( p, 0 );

            DE("    %d local look up: %s[%x%x%x%x]:", i, name?name:"???",
                   md5[0], md5[1], md5[2], md5[3] );
            for(j = 0; j < nRect; ++j)
            DE("        %dx%d+%d+%d",
                   rect[j].width, rect[j].height, rect[j].x, rect[j].y );
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

int myXErrorHandler ( Display * display, XErrorEvent * e)
{
  DERR( "%s:%d catched a X11 error\n", 
          strrchr(__FILE__, '/')?strrchr(__FILE__, '/')+1:__FILE__,__LINE__ );
  return 0;
}

xcmseContext_s *
         xcmseContext_New            ( )
{
  xcmseContext_s * c = (xcmseContext_s*) calloc( sizeof(xcmseContext_s), 1 );
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
  //c->aProfile, c->aTarget, c->aCM, c->aRegion, c->aDesktop;

  return c;
}

xcmseContext_s *
         xcmseContext_Create         ( const char        * display_name )
{
  int error = 0;
  xcmseContext_s * c = xcmseContext_New();
  if(!c)
    return c;

  error = xcmseContext_Setup( c, display_name );
  if(error)
    c = 0;

  return c;
}

int      xcmseContext_Setup          ( xcmseContext_s    * c,
                                       const char        * display_name )
{
  /* Open the display and create our window. */
  Visual * vis = 0;
  Colormap cmap = 0;
  XSetWindowAttributes attrs;
  Status status = 0;
  Atom actual;
  int format,
      has_display = 0;
  unsigned long left, n;
  unsigned char * data;

  /*signal( SIGINT, mySignalHandler );
  signal( SIGINT, SIG_IGN );*/
  /*XSetErrorHandler(myXErrorHandler);*/
  XSetErrorHandler( XmuSimpleErrorHandler );

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
    exit (1);
  }

  c->screen = DefaultScreen( c->display );
  c->root = XRootWindow( c->display, c->screen );

  /* define the observers interesst */
  c->aProfile = XInternAtom( c->display, "_NET_COLOR_PROFILES", False );
  c->aTarget = XInternAtom( c->display, "_NET_COLOR_TARGET", False );
  c->aCM = XInternAtom( c->display, "_NET_COLOR_MANAGEMENT", False );
  c->aRegion = XInternAtom( c->display, "_NET_COLOR_REGIONS", False );
  c->aDesktop = XInternAtom( c->display, "_NET_COLOR_DESKTOP", False );

  if(!has_display)
  {
    vis = DefaultVisual( c->display, c->screen );
    cmap = XCreateColormap( c->display, c->root, vis, AllocNone );
    attrs.colormap = cmap;
    attrs.border_pixel = 0;
    attrs.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask |
                       PropertyChangeMask;

    c->w = XCreateWindow( c->display, c->root, 0, 0, 300, 300, 5, 24,
                          InputOutput, DefaultVisual( c->display, c->screen ),
                          CWBorderPixel | CWColormap | CWEventMask, &attrs);
    /*XMapWindow( display, w );*/
  }

  /* check if we can see other clients */
  XGetWindowProperty( c->display, RootWindow(c->display,0),
                      XInternAtom(c->display, "_NET_CLIENT_LIST", False),
                      0, ~0, False, XA_WINDOW,
                      &actual,&format, &n, &left, &data );
  if(!data || !n)
    DERR( "\nThe extented ICCCM hint _NET_CLIENT_LIST atom is %s\n"
          "!!! xcmsevents will work limited !!!\n", n ? "missed" : "zero" );

  /* print some general information */
  M( oyMSG_TITLE, 0,
     TARGET " - observes X11 colour management system events%s", "");
  M( oyMSG_COPYRIGHT, 0,
     "(c) 2009-2010 - Kai-Uwe Behrmann  License: newBSD%s", "" );
  DS( "atom: \"_NET_COLOR_PROFILES\": %d", (int)c->aProfile );
  DS( "atom: \"_NET_COLOR_TARGET\": %d", (int)c->aTarget );
  DS( "atom: \"_NET_COLOR_MANAGEMENT\": %d", (int)c->aCM );
  DS( "atom: \"_NET_COLOR_REGIONS\": %d", (int)c->aRegion );
  DS( "atom: \"_NET_COLOR_DESKTOP\": %d %s", (int)c->aDesktop,
                                          printfNetColorDesktop(c, 0) );


  DS( "root window ID: %d", (int)c->root );
  {
    FILE * fp;
    size_t size = 256, i = 0;
    char * txt;
    S( "running \"oyranos-monitor -l\":%s", "" );
    fp = popen(  "oyranos-monitor -l", "r" );
    if( fp )
    {
      txt = (char*) oyAllocateFunc_( size + 1 );
      while( (txt[i] = getc(fp)) != 0 && txt[i] != EOF && i < size-1 )
        ++i;
      if(txt[i-1] == '\n')
        txt[i-1] = 0;
      else
      txt[i] = 0;
      S( "%s", txt );
      oyDeAllocateFunc_( txt ); txt = 0;
    }
    fclose( fp );
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
    status = XQueryTree( c->display, c->root,
                         &root_return, &parent_return,
                         &children_return, &nchildren_return );
    wins = (Window*)malloc(sizeof(Window) * nchildren_return );
    memcpy( wins, children_return, sizeof(Window) * nchildren_return );
    XFree( children_return );
    children_return = wins; wins = 0;

    for(i = nchildren_return - 1; i >= 0; --i)
    {
      root_return = 0;
      status = XQueryTree( c->display, children_return[i],
                           &root_return, &parent_return,
                           &wins, &wins_n );
      status = XGetWindowAttributes( c->display, children_return[i],
                                     &window_attributes_return );
      if(window_attributes_return.map_state == IsViewable &&
         parent_return == c->root)
      {
        printWindowRegions( c->display, children_return[i], 0 );
        /*printf( "[%d] \"%s\"\n", (int)children_return[i],
                     printWindowName(display, children_return[i]) );*/

      }

      XFree( wins );
    }

    free( children_return );
  }

  /* observe the root window as well for newly appearing windows */
  XSelectInput( c->display, c->root,
                PropertyChangeMask |   /* _NET_COLOR_PROFILES */
                ExposureMask );        /* _NET_COLOR_MANAGEMENT */

  return 0;
}

int      xcmseContext_Stop           ( xcmseContext_s    * c )
{
  if(c->display_is_owned)
  {
    XDestroyWindow( c->display, c->w );
    XCloseDisplay( c->display );
  }
  return 0;
}

int      xcmseContext_InLoop         ( xcmseContext_s    * c,
                                       XEvent            * event )
{
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
      int i,j = 0, r;
      Atom atom = event->xproperty.atom;

      actual_name = XGetAtomName( display, atom );

      if(display != c->display)
        DE( "PropertyNotify : event and context displays are different: %s",
               actual_name );
        
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
           strstr( actual_name, OY_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE) != 0 ||
           strstr( actual_name, OY_ICC_V0_3_TARGET_PROFILE_IN_X_BASE) != 0 ||
           strstr( actual_name, "EDID") != 0)
        r = XGetWindowProperty( display, event->xany.window,
               event->xproperty.atom, 0, ~0, False, XA_CARDINAL,&actual,&format,
                &n, &left, &data );
        n += left;

        if       ( event->xproperty.atom == c->aTarget )
        {
          char * text;

          r = XGetWindowProperty( display, event->xany.window,
               event->xproperty.atom, 0, ~0, False, XA_STRING, &actual, &format,
                &n, &left, &data );
          n += left;
          text = (char*)data;
          DE("PropertyNotify : %s     \"%s\"  %s",
               actual_name,
               text, printWindowName( display, event->xany.window ) );

        } else if( event->xproperty.atom == c->aProfile )
        {
          unsigned long count = XcolorProfileCount(data, n);
          DE( "PropertyNotify : %s   %d         %s",
               actual_name,
               (int)count, printWindowName( display, event->xany.window ) );

        } else if( event->xproperty.atom == c->aCM )
        {
          /* should not happen */

        } else if( event->xproperty.atom == c->aDesktop )
        {
          DE( "PropertyNotify : %s    %s          %s",
               actual_name,
               event->xproperty.state ? "0 - removed" :
               printfNetColorDesktop(c, 1),
               printWindowName( display, event->xany.window ) );

        } else if( event->xproperty.atom == c->aRegion )
        {
          printWindowRegions( display, event->xany.window, 1 );

        } else if(
           strstr( actual_name, OY_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE) != 0 ||
           strstr( actual_name, OY_ICC_V0_3_TARGET_PROFILE_IN_X_BASE) != 0 ||
           strstr( actual_name, "EDID") != 0)
        {
          const char * name = 0,
                     * an = actual_name;
          char * tmp = 0;
          double colours[9] = {0,0,0,0,0,0,0,0,0};
          oyProfile_s * p = 0;

          if(strcmp( OY_ICC_V0_3_TARGET_PROFILE_IN_X_BASE, an ) == 0)
            an = OY_ICC_V0_3_TARGET_PROFILE_IN_X_BASE"  ";

          if(n &&
             (strstr( actual_name, OY_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE) != 0 ||
              strstr( actual_name, OY_ICC_V0_3_TARGET_PROFILE_IN_X_BASE) != 0))
          {
            p = oyProfile_FromMem( n, data, 0, 0 );
            name = oyProfile_GetFileName( p, 0 );
            if(name && strchr(name, '/'))
              name = strrchr( name, '/' ) + 1;
            else if(!name)
              name = oyProfile_GetText( p, oyNAME_DESCRIPTION );
            else if(!name)
              name = "????";
          }
          if(n &&
             strstr( actual_name, "EDID") != 0)
          {
            char * manufacturer = 0,
                 * model = 0,
                 * serial = 0;

            oyUnrollEdid1_( data, &manufacturer,0,&model, &serial, 0,0,0,0,0,
                            colours, malloc );
            STRING_ADD( tmp, manufacturer ); STRING_ADD( tmp, " - " );
            STRING_ADD( tmp, model ); STRING_ADD( tmp, " - " );
            STRING_ADD( tmp, serial ); STRING_ADD( tmp, "\n  " );
          }
          DE(   "PropertyNotify : %s    \"%s\"[%d]  %s",
                 an, name?name:(tmp?"set":"removed"),(int)n,
                 printWindowName( display, event->xany.window ) );

          if(tmp)
          {
            DE("  %s", tmp);
            DE("  {{%.03f,%.03f}{%.03f,%.03f}{%.03f,%.03f}{%.03f,%.03f}%.03f}",
                   colours[0], colours[1], colours[2], colours[3],
                   colours[4], colours[5], colours[6], colours[7], colours[8] );
            free(tmp); tmp = 0;
          }
          oyProfile_Release( &p );
        }

        if(data) XFree(data);
      }


      /* claim interesst in other windows events */
      if( c->w != event->xany.window &&
          XInternAtom( display, "_NET_CLIENT_LIST", False) ==
          event->xproperty.atom )
      {
        unsigned long nWindow = 0;
        Window * windows = 0;
        actual = 0;
        format = n = 0;
        left = 0;

        r = XGetWindowProperty( display, c->root,
          event->xproperty.atom, 0, ~0, False, XA_WINDOW, &actual, &format,
          &nWindow, &left, (unsigned char**)&windows );
        n = (int)(nWindow + left);

        for(i = 0; i < (int)n; ++i)
        {
          /* search of a previous observation of a particular window */
          int found = 0;
          for(j = 0; j < c->nWindows; ++j)
          {
            if(windows[i] == c->Windows[j])
              found = 1;
          }

          /* other new windows but not own */
          if( c->w != windows[i] &&
              !found )
          {
            /* observe other windows */
            r = XSelectInput( display, windows[i],
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
                printWindowName( display, event->xclient.window ) );
        XFree( actual_name ); actual_name = 0;
      }
    }
  }
  return 0;
}



