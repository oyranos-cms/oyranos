/** xcmsevents.c
 *
 *  A small X11 colour management event observer.
 *
 *  License: newBSD
 *  Copyright: 2009 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 *  c++: ar cru liboyranosedid.a ../../modules/devices/oyranos_edid_parse.o ../../modules/devices/oyranos_monitor.o
 *  c++: prefix=/opt/local; g++ -Wall -g -o qcmsevents xcmsevents.c `PKG_CONFIG_PATH=$prefix/lib/pkgconfig pkg-config --cflags --libs x11 xmu xfixes xinerama xrandr xxf86vm oyranos` -DHAVE_X11 -I$prefix/include -I../.. -L$prefix/lib -lXcolor -L./ -loyranosedid
 */

#ifdef __cplusplus
extern "C" {
#endif
#include <X11/Xutil.h>
#include <X11/Xmu/WinUtil.h>
#include "Xcolor.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h> /* signal */

#include <X11/extensions/Xfixes.h>
#include <X11/Xmu/Error.h> /* XmuSimpleErrorHandler */
#ifdef __cplusplus
}
#endif

#include <oyranos_alpha.h> /* use Oyranos to obtain profile names */
#include "oyranos_monitor_internal.h" /* EDID parsing */

#ifdef __cplusplus
using namespace oyranos;
extern "C" {
}
#endif

#ifdef STRING_ADD
#undef STRING_ADD
#endif
#define STRING_ADD(t, txt) oyStringAdd_( &t, txt, malloc, free )
/*void               oyStringAdd_      ( char             ** text,
                                       const char        * append,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocFunc );*/

static inline XcolorProfile *
         XcolorProfileNext           ( XcolorProfile     * profile );
static inline unsigned long 
         XcolorProfileCount          ( void              * data,
                                       unsigned long       nBytes);
int      myXErrorHandler             ( Display           * display,
                                       XErrorEvent       * e );

int      xcmseSetup                  ( xcmseContext_s    * c,
                                       const char        * display_name );



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

          printf("PropertyNotify : %s    vvvvv      %s %d\n",
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
              printf("server region id with zero: left %d\n", (int)n-i);
              break;
            }

            rect = XFixesFetchRegion( display, regions[i].region, &nRect );
            md5 = (uint32_t*)&regions[i].md5[0];
            p = oyProfile_FromMD5( md5, 0 );
            name = oyProfile_GetFileName( p, 0 );

            printf("    %d local look up: %s[%x%x%x%x]:\n", i, name?name:"???",
                   md5[0], md5[1], md5[2], md5[3] );
            for(j = 0; j < nRect; ++j)
            printf("        %dx%d+%d+%d\n",
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
  printf( "%s:%d catched a X11 error\n", 
          strrchr(__FILE__, '/')?strrchr(__FILE__, '/')+1:__FILE__,__LINE__ );
  return 0;
}


xcmseContext_s *
         xcmseContext_New            ( const char        * display_name )
{
  int error = 0;
  xcmseContext_s * c = (xcmseContext_s*) calloc( sizeof(xcmseContext_s), 1 );
  if(!c)
    return c;

  c->display = 0;
  c->screen = -1;
  c->root = 0;
  c->nWindows = 0;
  c->Windows = 0;
  //c->w;
  c->old_pid = 0;
  //c->aProfile, c->aTarget, c->aCM, c->aRegion, c->aDesktop;

  error = xcmseSetup( c, display_name );
  if(error)
    c = 0;

  return c;
}

int      xcmseSetup                  ( xcmseContext_s    * c,
                                       const char        * display_name )
{
  /* Open the display and create our window. */
  Visual * vis = 0;
  Colormap cmap = 0;
  XSetWindowAttributes attrs;
  Status status = 0;
  Atom actual;
  int format;
  unsigned long left, n;
  unsigned char * data;

  /*signal( SIGINT, mySignalHandler );
  signal( SIGINT, SIG_IGN );*/
  /*XSetErrorHandler(myXErrorHandler);*/
  XSetErrorHandler( XmuSimpleErrorHandler );

  c->display = XOpenDisplay(NULL);
  if(!c->display)
  {
    printf( "could not open display %s\n", display_name?display_name:"???" );
    exit (1);
  }

  c->screen = DefaultScreen( c->display );
  c->root = XRootWindow( c->display, c->screen );
  vis = DefaultVisual( c->display, c->screen );
  cmap = XCreateColormap( c->display, c->root, vis, AllocNone );

  /* define the observers interesst */
  c->aProfile = XInternAtom( c->display, "_NET_COLOR_PROFILES", False );
  c->aTarget = XInternAtom( c->display, "_NET_COLOR_TARGET", False );
  c->aCM = XInternAtom( c->display, "_NET_COLOR_MANAGEMENT", False );
  c->aRegion = XInternAtom( c->display, "_NET_COLOR_REGIONS", False );
  c->aDesktop = XInternAtom( c->display, "_NET_COLOR_DESKTOP", False );

  attrs.colormap = cmap;
  attrs.border_pixel = 0;
  attrs.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask |
                     PropertyChangeMask;

  c->w = XCreateWindow( c->display, c->root, 0, 0, 300, 300, 5, 24,
                        InputOutput, DefaultVisual( c->display, c->screen ),
                        CWBorderPixel | CWColormap | CWEventMask, &attrs);

  /*XMapWindow( display, w );*/

  /* check if we can see other clients */
  XGetWindowProperty( c->display, RootWindow(c->display,0),
                      XInternAtom(c->display, "_NET_CLIENT_LIST", False),
                      0, ~0, False, XA_CARDINAL,
                      &actual,&format, &n, &left, &data );
  if(!data && !n)
    printf("\nThe extented ICCCM hint _NET_CLIENT_LIST atom is missed\n"
             "!!! xcmsevents will work limited !!!\n\n");

  XGetWindowProperty( c->display, RootWindow(c->display,0),
                      c->aDesktop, 0, ~0, False, XA_CARDINAL,
                      &actual,&format, &n, &left, &data );
  n += left;
  if(n && data)
    c->old_pid = *((pid_t*)data);

  /* print some general information */
  printf( "xcmsevents - observes X11 colour management system events\n"
          "  (c) 2009-2010 - Kai-Uwe Behrmann  License: newBSD\n" );
  printf( "atom: \"_NET_COLOR_PROFILES\": %d\n", (int)c->aProfile );
  printf( "atom: \"_NET_COLOR_TARGET\": %d\n", (int)c->aTarget );
  printf( "atom: \"_NET_COLOR_MANAGEMENT\": %d\n", (int)c->aCM );
  printf( "atom: \"_NET_COLOR_REGIONS\": %d\n", (int)c->aRegion );
  printf( "atom: \"_NET_COLOR_DESKTOP\": %d %d\n", (int)c->aDesktop,
                                                   (int)c->old_pid );
  printf( "root window ID: %d\n", (int)c->root );
  printf( "running \"oyranos-monitor -l\":\n" );
  system( "oyranos-monitor -l" );
  printf( "\n" );

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

int      xcmseStop                   ( xcmseContext_s * c )
{
  XDestroyWindow( c->display, c->w );
  XCloseDisplay( c->display );
  return 0;
}

int      xcmseInLoop                 ( xcmseContext_s * c )
{
  /* observe events */
  {
    Atom actual;
    int format;
    unsigned long left, n;
    unsigned char * data;

    XEvent event;
    XNextEvent( c->display, &event);

    if( event.xany.window == c->w && event.type == Expose )
    {
      /* draw something */
      XFillRectangle( c->display, c->w, DefaultGC( c->display, c->screen),
                      0, 0, 300, 300);

    } else if( event.type == PropertyNotify )
    {
      int i,j = 0, r;

      actual = 0;
      format = 0;
      left = 0; n = 0;
      data = 0;

      /* --- report --- */
      if(c->w != event.xany.window)
      {
        if(event.xproperty.atom == c->aProfile ||
           event.xproperty.atom == c->aCM ||
           event.xproperty.atom == c->aRegion ||
           event.xproperty.atom == c->aDesktop ||
           strstr( XGetAtomName( event.xany.display, event.xproperty.atom ), 
                   "_ICC_PROFILE") != 0 ||
           strstr( XGetAtomName( event.xany.display, event.xproperty.atom ), 
                   "EDID") != 0)
        r = XGetWindowProperty( c->display, event.xany.window,
                event.xproperty.atom, 0, ~0, False, XA_CARDINAL,&actual,&format,
                &n, &left, &data );
        n += left;

        if       ( event.xproperty.atom == c->aTarget )
        {
          char * text;

          r = XGetWindowProperty( c->display, event.xany.window,
                event.xproperty.atom, 0, ~0, False, XA_STRING, &actual, &format,
                &n, &left, &data );
          n += left;
          text = (char*)data;
          printf("PropertyNotify : %s     \"%s\"  %s\n",
               XGetAtomName( event.xany.display, event.xproperty.atom ),
               text, printWindowName( c->display, event.xany.window ) );

        } else if( event.xproperty.atom == c->aProfile )
        {
          unsigned long count = XcolorProfileCount(data, n);
          printf("PropertyNotify : %s   %d         %s\n",
               XGetAtomName( event.xany.display, event.xproperty.atom ),
               (int)count, printWindowName( c->display, event.xany.window ) );

        } else if( event.xproperty.atom == c->aCM )
        {
          /* should not happen */

        } else if( event.xproperty.atom == c->aDesktop )
        {
          if(n && data)
          {
            if(*((pid_t*)data) && c->old_pid)
              printf("!!! Found old _NET_COLOR_DESKTOP pid: %d.\n"
                     "Eigther there was a previous crash or your setup can be double colour corrected.", c->old_pid );
            c->old_pid = *((pid_t*)data);
          } else
            c->old_pid = 0;
          printf("PropertyNotify : %s    %d          %s\n",
               XGetAtomName( event.xany.display, event.xproperty.atom ),
               c->old_pid, printWindowName( c->display, event.xany.window ) );

        } else if( event.xproperty.atom == c->aRegion )
        {
          printWindowRegions( event.xany.display, event.xany.window, 1 );

        } else if(
           strstr( XGetAtomName( event.xany.display, event.xproperty.atom ), 
                   "_ICC_PROFILE") != 0 ||
           strstr( XGetAtomName( event.xany.display, event.xproperty.atom ),
                   "EDID") != 0)
        {
          const char * name = 0,
                     * an = XGetAtomName( event.xany.display,
                                          event.xproperty.atom );
          char * tmp = 0;
          double colours[9] = {0,0,0,0,0,0,0,0,0};
          oyProfile_s * p = 0;

          if(strcmp( "_ICC_PROFILE", an ) == 0)
            an = "_ICC_PROFILE  ";

          if(n &&
             strstr( XGetAtomName( event.xany.display, event.xproperty.atom ),
                     "_ICC_PROFILE") != 0)
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
             strstr( XGetAtomName( event.xany.display, event.xproperty.atom ),
                     "EDID") != 0)
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
          printf("PropertyNotify : %s    \"%s\"[%d]  %s\n",
                 an, name?name:(tmp?"set":"removed"),(int)n,
                 printWindowName( c->display, event.xany.window ) );

          if(tmp)
          {
            printf("  %s", tmp);
            printf("  {{%.03f,%.03f}{%.03f,%.03f}{%.03f,%.03f}{%.03f,%.03f}%.03f}\n",
                   colours[0], colours[1], colours[2], colours[3],
                   colours[4], colours[5], colours[6], colours[7], colours[8] );
            free(tmp); tmp = 0;
          }
          oyProfile_Release( &p );
        }

        if(data) XFree(data);
      }


      /* claim interesst in other windows events */
      if( c->w != event.xany.window &&
          XInternAtom( c->display, "_NET_CLIENT_LIST", False) ==
          event.xproperty.atom )
      {
        unsigned long nWindow = 0;
        Window * windows = 0;
        actual = 0;
        format = n = 0;
        left = 0;

        r = XGetWindowProperty( c->display, c->root,
          event.xproperty.atom, 0, ~0, False, XA_WINDOW, &actual, &format,
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
            r = XSelectInput( event.xany.display, windows[i],
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

    } else if( event.type == ClientMessage )
    {
      if(event.xclient.message_type == c->aCM )
      {
        /* --- report --- */
        unsigned long active[2];

        active[0] = event.xclient.data.l[0];
        active[1] = event.xclient.data.l[1];
        printf( "ClientMessage  : %s %ld %ld        %s\n",
                XGetAtomName( event.xany.display, event.xclient.message_type),
                active[0], active[1],
                printWindowName( c->display, event.xclient.window ) );
      }
    }
  }
  return 0;
}



