/** xcmsevents.c
 *
 *  A small X11 colour management event observer.
 *
 *  License: newBSD
 *  Copyright: 2009 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 *  compile: gcc -Wall -g -o xcmsevents xcmsevents.c `pkg-config --cflags --libs x11 xfixes oyranos` -DHAVE_X11 -I../ -L./ -lXcolor
 */

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xmu/WinUtil.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h> /* signal */

#include "Xcolor.h"
#include <X11/extensions/Xfixes.h>
#include <X11/Xmu/Error.h> /* XmuSimpleErrorHandler */
#include <oyranos_alpha.h> /* use Oyranos to obtain profile names */

void
oyUnrollEdid1_                    (oyPointer   edi,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                       double            * c,
                                   oyAlloc_f     allocate_func);
#define STRING_ADD(t, txt) oyStringAdd_( &t, txt, malloc, free )
void               oyStringAdd_      ( char             ** text,
                                       const char        * append,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocFunc );


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

  if(!text) text = malloc(80);

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
          for(i = 0; i < n; ++i)
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

  for (ptr = data; (intptr_t)ptr < (intptr_t)data + nBytes; ptr = XcolorProfileNext(ptr))
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

int main(int argc, char *argv[])
{
  /* Open the display and create our window. */
  Display * display = 0;
  int screen = -1;
  Window root = 0;
  Visual * vis = 0;
  Colormap cmap = 0;
  XSetWindowAttributes attrs;
  Window w;
  static int nWindows = 0;
  static Window * Windows = 0;
  Atom actual = 0;
  int format = 0;
  unsigned long left = 0, n = 0;
  unsigned char * data = 0;
  static pid_t old_pid = 0;
  const char * display_name = getenv("DISPLAY");
  Atom aProfile, aTarget, aCM, aRegion, aDesktop;
  Status status = 0;

  /*signal( SIGINT, mySignalHandler );
  signal( SIGINT, SIG_IGN );*/
  /*XSetErrorHandler(myXErrorHandler);*/
  XSetErrorHandler( XmuSimpleErrorHandler );

  display = XOpenDisplay(NULL);
  if(!display)
  {
    printf( "could not open display %s\n", display_name?display_name:"???" );
    exit (1);
  }

  screen = DefaultScreen( display );
  root = XRootWindow( display, screen );
  vis = DefaultVisual( display, screen );
  cmap = XCreateColormap( display, root, vis, AllocNone );

  /* define the observers interesst */
  aProfile = XInternAtom( display, "_NET_COLOR_PROFILES", False );
  aTarget = XInternAtom( display, "_NET_COLOR_TARGET", False );
  aCM = XInternAtom( display, "_NET_COLOR_MANAGEMENT", False );
  aRegion = XInternAtom( display, "_NET_COLOR_REGIONS", False );
  aDesktop = XInternAtom( display, "_NET_COLOR_DESKTOP", False );

  attrs.colormap = cmap;
  attrs.border_pixel = 0;
  attrs.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask |
                     PropertyChangeMask;

  w = XCreateWindow( display, root, 0, 0, 300, 300, 5, 24,
                     InputOutput, DefaultVisual( display, screen ),
                     CWBorderPixel | CWColormap | CWEventMask, &attrs);

  /*XMapWindow( display, w );*/

  /* check if we can see other clients */
  XGetWindowProperty( display, RootWindow(display,0),
                      XInternAtom(display, "_NET_CLIENT_LIST", False),
                      0, ~0, False, XA_CARDINAL,
                      &actual,&format, &n, &left, &data );
  if(!data && !n)
    printf("\nThe extented ICCCM hint _NET_CLIENT_LIST atom is missed\n"
             "!!! xcmsevents will work limited !!!\n\n");

  XGetWindowProperty( display, RootWindow(display,0),
                      aDesktop, 0, ~0, False, XA_CARDINAL,
                      &actual,&format, &n, &left, &data );
  n += left;
  if(n && data)
    old_pid = *((pid_t*)data);

  /* print some general information */
  printf( "xcmsevents - observes X11 colour management system events\n"
          "  (c) 2009 - Kai-Uwe Behrmann  license: newBSD\n" );
  printf( "atom: \"_NET_COLOR_PROFILES\": %d\n", (int)aProfile );
  printf( "atom: \"_NET_COLOR_TARGET\": %d\n", (int)aTarget );
  printf( "atom: \"_NET_COLOR_MANAGEMENT\": %d\n", (int)aCM );
  printf( "atom: \"_NET_COLOR_REGIONS\": %d\n", (int)aRegion );
  printf( "atom: \"_NET_COLOR_DESKTOP\": %d %d\n", (int)aDesktop,(int)old_pid );
  printf( "root window ID: %d\n", (int)root );
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

    XSync( display, 0 );
    status = XQueryTree( display, root,
                         &root_return, &parent_return,
                         &children_return, &nchildren_return );
    wins = malloc(sizeof(Window) * nchildren_return );
    memcpy( wins, children_return, sizeof(Window) * nchildren_return );
    XFree( children_return );
    children_return = wins; wins = 0;

    for(i = nchildren_return - 1; i >= 0; --i)
    {
      root_return = 0;
      status = XQueryTree( display, children_return[i],
                           &root_return, &parent_return,
                           &wins, &wins_n );
      status = XGetWindowAttributes( display, children_return[i],
                                     &window_attributes_return );
      if(window_attributes_return.map_state == IsViewable &&
         parent_return == root)
      {
        printWindowRegions( display, children_return[i], 0 );
        /*printf( "[%d] \"%s\"\n", (int)children_return[i],
                     printWindowName(display, children_return[i]) );*/

      }

      XFree( wins );
    }

    free( children_return );
  }

  /* observe the root window as well for newly appearing windows */
  XSelectInput( display, root,
                PropertyChangeMask |   /* _NET_COLOR_PROFILES */
                ExposureMask );        /* _NET_COLOR_MANAGEMENT */

  /* observe events */
  for(;;)
  {
    XEvent event;
    XNextEvent(display, &event);

    if( event.xany.window == w && event.type == Expose )
    {
      /* draw something */
      XFillRectangle( display, w, DefaultGC(display, screen), 0, 0, 300, 300);

    } else if( event.type == PropertyNotify )
    {
      int i,j = 0, r;

      actual = 0;
      format = 0;
      left = 0; n = 0;
      data = 0;

      /* --- report --- */
      if(w != event.xany.window)
      {
        if(event.xproperty.atom == aProfile ||
           event.xproperty.atom == aCM ||
           event.xproperty.atom == aRegion ||
           event.xproperty.atom == aDesktop ||
           strstr( XGetAtomName( event.xany.display, event.xproperty.atom ), 
                   "_ICC_PROFILE") != 0 ||
           strstr( XGetAtomName( event.xany.display, event.xproperty.atom ), 
                   "EDID") != 0)
        r = XGetWindowProperty( display, event.xany.window,
                event.xproperty.atom, 0, ~0, False, XA_CARDINAL,&actual,&format,
                &n, &left, &data );
        n += left;

        if       ( event.xproperty.atom == aTarget )
        {
          char * text;

          r = XGetWindowProperty( display, event.xany.window,
                event.xproperty.atom, 0, ~0, False, XA_STRING, &actual, &format,
                &n, &left, &data );
          n += left;
          text = (char*)data;
          printf("PropertyNotify : %s     \"%s\"  %s\n",
               XGetAtomName( event.xany.display, event.xproperty.atom ),
               text, printWindowName( display, event.xany.window ) );

        } else if( event.xproperty.atom == aProfile )
        {
          unsigned long count = XcolorProfileCount(data, n);
          printf("PropertyNotify : %s   %d         %s\n",
               XGetAtomName( event.xany.display, event.xproperty.atom ),
               (int)count, printWindowName( display, event.xany.window ) );

        } else if( event.xproperty.atom == aCM )
        {
          /* should not happen */

        } else if( event.xproperty.atom == aDesktop )
        {
          if(n && data)
          {
            if(*((pid_t*)data) && old_pid)
              printf("!!! Found old _NET_COLOR_DESKTOP pid: %d.\n"
                     "Eigther there was a previous crash or your setup can be double colour corrected.", old_pid );
            old_pid = *((pid_t*)data);
          } else
            old_pid = 0;
          printf("PropertyNotify : %s    %d          %s\n",
               XGetAtomName( event.xany.display, event.xproperty.atom ),
               old_pid, printWindowName( display, event.xany.window ) );

        } else if( event.xproperty.atom == aRegion )
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

          if(strcmp( "_ICC_PROFILE", an ) == 0)
            an = "_ICC_PROFILE  ";

          if(n &&
             strstr( XGetAtomName( event.xany.display, event.xproperty.atom ),
                     "_ICC_PROFILE") != 0)
          {
            oyProfile_s * p = oyProfile_FromMem( n, data, 0, 0 );
            name = oyProfile_GetFileName( p, 0 );
            if(name && strchr(name, '/'))
              name = strrchr( name, '/' ) + 1;
          }
          if(n &&
             strstr( XGetAtomName( event.xany.display, event.xproperty.atom ),
                     "EDID") != 0)
          {
            char * manufacturer = 0,
                 * model = 0,
                 * serial = 0;

            oyUnrollEdid1_( data, &manufacturer, &model, &serial, colours,
                            malloc );
            STRING_ADD( tmp, manufacturer ); STRING_ADD( tmp, " - " );
            STRING_ADD( tmp, model ); STRING_ADD( tmp, " - " );
            STRING_ADD( tmp, serial ); STRING_ADD( tmp, "\n  " );
          }
          printf("PropertyNotify : %s    \"%s\"[%d]  %s\n",
                 an, name?name:(tmp?"set":"removed"),(int)n,
                 printWindowName( display, event.xany.window ) );

          if(tmp)
          {
            printf("  %s", tmp);
            printf("  {{%.03f,%.03f}{%.03f,%.03f}{%.03f,%.03f}{%.03f,%.03f}%.03f}\n",
                   colours[0], colours[1], colours[2], colours[3],
                   colours[4], colours[5], colours[6], colours[7], colours[8] );
            free(tmp); tmp = 0;
          }
        }

        if(data) XFree(data);
      }


      /* claim interesst in other windows events */
      if( w != event.xany.window &&
          XInternAtom(display, "_NET_CLIENT_LIST", False) == event.xproperty.atom )
      {
        unsigned long nWindow = 0;
        Window * windows = 0;
        actual = 0;
        format = n = 0;
        left = 0;

        r = XGetWindowProperty( display, root,
          event.xproperty.atom, 0, ~0, False, XA_WINDOW, &actual, &format,
          &nWindow, &left, (unsigned char**)&windows );
        n = (int)(nWindow + left);

        for(i = 0; i < n; ++i)
        {
          /* search of a previous observation of a particular window */
          int found = 0;
          for(j = 0; j < nWindows; ++j)
          {
            if(windows[i] == Windows[j])
              found = 1;
          }

          /* other new windows but not own */
          if( w != windows[i] &&
              !found )
          {
            /* observe other windows */
            r = XSelectInput( event.xany.display, windows[i],
                       PropertyChangeMask |  /* Xcolor properties */
                       ExposureMask );       /* Xcolor client messages */
          }
        }

        if(n > nWindows)
        {
          if(Windows) free(Windows);
          Windows = malloc( sizeof(Window) * n );
        }
        memcpy( Windows, windows, sizeof(Window) * n );
        nWindows = n;

      }

    } else if( event.type == ClientMessage )
    {
      if(event.xclient.message_type == aCM )
      {
        /* --- report --- */
        unsigned long active[2];

        active[0] = event.xclient.data.l[0];
        active[1] = event.xclient.data.l[1];
        printf( "ClientMessage  : %s %ld %ld        %s\n",
                XGetAtomName( event.xany.display, event.xclient.message_type),
                active[0], active[1],
                printWindowName( display, event.xclient.window ) );
      }
    }
  }

  XDestroyWindow(display, w);
  XCloseDisplay(display);

  return 0;
}

