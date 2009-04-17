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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Xcolor.h"
#include <X11/extensions/Xfixes.h>
#include <oyranos_alpha.h> /* use Oyranos to obtain profile names */

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
  Window child_return;

  if(!text) text = malloc(80);

  XGetGeometry( display, w, &root_return,
                        &x_return, &y_return, &width_return, &height_return,
                        &border_width_return, &depth_return );

  XTranslateCoordinates( display, w, root, x_return, y_return,
                         &dest_x_return, &dest_y_return, &child_return );

  if( RootWindow( display, DefaultScreen( display ) ) == w )
    sprintf( text, "root window" );
  else
    sprintf( text, "%dx%d%s%d%s%d (id:%d)", width_return, height_return,
             dest_x_return<0?"":"+", dest_x_return,
             dest_y_return<0?"":"+", dest_y_return,
             (int)w );

  return text;
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



int main(int argc, char *argv[])
{
  /* Open the display and create our window. */
  Display *display = XOpenDisplay(NULL);
  int screen = DefaultScreen( display );
  Window root = XRootWindow( display, screen );
  Visual *vis = DefaultVisual( display, screen );
  Colormap cmap = XCreateColormap( display, root, vis, AllocNone );
  XSetWindowAttributes attrs;
  Window w;
  static int nWindows = 0;
  static Window * Windows = 0;
  Atom actual = 0;
  int format = 0;
  unsigned long left = 0, n = 0;
  unsigned char * data = 0;
  static pid_t old_pid = 0;


  /* define the observers interesst */
  Atom aProfile = XInternAtom( display, "_NET_COLOR_PROFILES", False ),
       aTarget = XInternAtom( display, "_NET_COLOR_TARGET", False ),
       aCM = XInternAtom( display, "_NET_COLOR_MANAGEMENT", False ),
       aRegion = XInternAtom( display, "_NET_COLOR_REGIONS", False ),
       aDesktop = XInternAtom( display, "_NET_COLOR_DESKTOP", False );

  attrs.colormap = cmap;
  attrs.border_pixel = 0;
  attrs.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask |
                     PropertyChangeMask;

  w = XCreateWindow( display, root, 0, 0, 300, 300, 5, 24,
                     InputOutput, DefaultVisual( display, screen ),
                     CWBorderPixel | CWColormap | CWEventMask, &attrs);

  /*XMapWindow( display, w );*/

  XGetWindowProperty( display, RootWindow(display,0),
                      aDesktop, 0, ~0, False, XA_CARDINAL,
                      &actual,&format, &n, &left, &data );
  n += left;
  if(n && data)
    old_pid = *((pid_t*)data);

  /* print some general information */
  printf( "xcmsevents - observes X11 colour management system evens\n"
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

  /* observe the root window as well for newly appearing windows */
  XSelectInput( display, root,
                PropertyChangeMask |   /* _NET_COLOR_PROFILES */
                ExposureMask );        /* _NET_COLOR_MANAGEMENT */

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
                   "_ICC_PROFILE") != 0)
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
          /* n seems not to be relyable */
          XcolorRegion * regions = XcolorRegionFetch( display,
                                    event.xany.window, &n );
          printf("PropertyNotify : %s    vvvvv      %s %d\n",
               XGetAtomName( event.xany.display, event.xproperty.atom ),
               printWindowName( display, event.xany.window ), (int)n );
          for(i = 0; i < n; ++i)
          {
            int nRect = 0;
            XRectangle * rect = 0;
            uint32_t * md5 = 0;
            oyProfile_s * p = 0;
            const char * name = 0;

            if(!regions[i].region)
              break;

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
        } else if(
           strstr( XGetAtomName( event.xany.display, event.xproperty.atom ), 
                   "_ICC_PROFILE") != 0)
        {
          const char * name = 0,
                     * an = XGetAtomName( event.xany.display,
                                          event.xproperty.atom );

          if(strcmp( "_ICC_PROFILE", an ) == 0)
            an = "_ICC_PROFILE  ";

          if(n)
          {
            oyProfile_s * p = oyProfile_FromMem( n, data, 0, 0 );
            name = oyProfile_GetFileName( p, 0 );
            if(name && strchr(name, '/'))
              name = strrchr( name, '/' ) + 1;
          }
          printf("PropertyNotify : %s    \"%s\"[%d]  %s\n",
                 an, name?name:"removed",(int)n,
                 printWindowName( display, event.xany.window ) );
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

