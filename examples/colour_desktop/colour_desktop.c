/**
 *  @file     colour_desktop.c
 *
 *  @brief    a compiz desktop colour management plug-in
 *
 *  @author   Kai-Uwe Behrmann, based on Tomas' color filter, based on Gerhard
 *            Fürnkranz' GLSL ppm_viewer
 *  @par Copyright:
 *            2008 (C) Gerhard Fürnkranz, 2008 (C) Tomas Carnecky,
              2009 (C) Kai-Uwe Behrmann
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/02/23
 */


#include <assert.h>

#define GL_GLEXT_PROTOTYPES
#define _BSD_SOURCE

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <X11/extensions/Xfixes.h>

#define HAVE_XRANDR
#ifdef HAVE_XRANDR
#include <X11/extensions/Xrandr.h>
#endif

#include <compiz-core.h>

#include <assert.h>
#include <math.h>     // floor()
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>   // getpid()

#include <stdarg.h>
#include <icc34.h>
#include <oyranos_alpha.h>

#include <Xcolor.h>

#define OY_COMPIZ_VERSION COMPIZ_VERSION_MAJOR * 10000 + COMPIZ_VERSION_MINOR * 100 + COMPIZ_VERSION_MICRO
#if OY_COMPIZ_VERSION < 800
#define oyCompLogMessage(disp_, plug_in_name, debug_level, format_, ... ) \
        compLogMessage( disp_, plug_in_name, debug_level, format_, __VA_ARGS__ )
#else
#define oyCompLogMessage(disp_, plug_in_name, debug_level, format_, ... ) \
        compLogMessage( plug_in_name, debug_level, format_, __VA_ARGS__ )
#endif

/* Uncomment the following line if you want to enable debugging output */
/*#define PLUGIN_DEBUG 1*/

/**
 * The 3D lookup texture has 64 points in each dimension, using 16 bit integers.
 * That means each active region will use 1.5MiB of texture memory.
 */
#define GRIDPOINTS 64

#define STENCIL_ID (pw->stencil_id*ps->nCcontexts + i + 1)


#define DBG_STRING "\n  %s:%d %s() %.02f "
#define DBG_ARGS __FILE__,__LINE__,__func__,(double)clock()/CLOCKS_PER_SEC
#if defined(PLUGIN_DEBUG) || defined(DEBUG)
#define START_CLOCK(text) printf( DBG_STRING text " - ", DBG_ARGS );
#define END_CLOCK         printf("%.02f\n", (double)clock()/CLOCKS_PER_SEC );
#else
#define START_CLOCK(text)
#define END_CLOCK
#endif



typedef CompBool (*dispatchObjectProc) (CompPlugin *plugin, CompObject *object, void *privateData);


/**
 * When a profile is uploaded into the root window, the plugin fetches the 
 * property and creates a lcms profile object. Each profile has a reference
 * count to allow clients to share profiles. When the ref-count drops to zero,
 * the profile is released.
 */
typedef struct {
  uint8_t md5[16];
  oyProfile_s * oy_profile;

  unsigned long refCount;
} PrivColorProfile;

/**
 * The XserverRegion is dereferenced only when the client sends a
 * _NET_COLOR_MANAGEMENT ClientMessage to its window. This allows clients to
 * change the region as the window is resized and later send _N_C_M to tell the
 * plugin to re-fetch the region from the server.
 * The profile is resolved as soon as the client uploads the regions into the
 * window. That means clients need to upload profiles first and then the 
 * regions. Otherwise the plugin won't be able to find the profile and no color
 * transformation will be done.
 */
typedef struct {
  /* These members are only valid when this region is part of the
   * active stack range. */
  Region xRegion;
} PrivColorRegion;

/**
 * Output profiles are currently only fetched using XRandR. For backwards 
 * compatibility the code should fall back to root window properties 
 * (_ICC_PROFILE).
 */
typedef struct {
  char name[32];
  oyProfile_s * oy_profile;
  oyConversion_s * cc;
  GLushort clut[GRIDPOINTS][GRIDPOINTS][GRIDPOINTS][3];
  GLuint glTexture;
  GLfloat scale, offset;
  XRectangle xRect;
} PrivColorOutput;


static CompMetadata pluginMetadata;

static int corePrivateIndex;

typedef struct {
  int childPrivateIndex;

  ObjectAddProc objectAdd;
} PrivCore;

typedef struct {
  int childPrivateIndex;

  HandleEventProc handleEvent;

  /* ClientMessage sent by the application */
  Atom netColorManagement;

  /* Window properties */
  Atom netColorProfiles;
  Atom netColorRegions;
  Atom netColorTarget;
  Atom netColorDesktop;
} PrivDisplay;

typedef struct {
  int childPrivateIndex;

  /* hooked functions */
  DrawWindowProc drawWindow;
  DrawWindowTextureProc drawWindowTexture;

  /* profiles attached to the screen */
  unsigned long nProfiles;
  PrivColorProfile *profile;

  /* compiz fragement function */
  int function, param, unit;

  /* XRandR outputs and the associated profiles */
  unsigned long nCcontexts;
  PrivColorOutput *ccontexts;
} PrivScreen;

typedef struct {
  /* stencil buffer id */
  unsigned long stencil_id;

  /* regions attached to the window */
  unsigned long nRegions;
  PrivColorRegion *pRegion;

  /* old absolute region */
  oyRectangle_s * absoluteWindowRectangleOld;

  /* active stack range */
  unsigned long active;

  /* active XRandR output */
  char *output;
} PrivWindow;

static Region absoluteRegion(CompWindow *w, Region region);
static void damageWindow(CompWindow *w, void *closure);

/**
 *    Private Data Allocation
 *
 * These are helper functions that really should be part of compiz. The private
 * data setup and handling currently requires macros and duplicates code all 
 * over the place. These functions, along with the object setup code (at the 
 * very bottom of this source file) make it much simpler.
 */

static void *compObjectGetPrivate(CompObject *o)
{
  if (o == NULL)
    return &corePrivateIndex;

  int *privateIndex = compObjectGetPrivate(o->parent);
  if (privateIndex == NULL)
    return NULL;

  return o->privates[*privateIndex].ptr;
}

static void *compObjectAllocPrivate(CompObject *parent, CompObject *object, int size)
{
  int *privateIndex = compObjectGetPrivate(parent);
  if (privateIndex == NULL)
    return NULL;

  int *privateData = malloc(size);
  if (privateData == NULL)
    return NULL;

  /* allocate an index for child objects */
  if (object->type < 3) {
    *privateData = compObjectAllocatePrivateIndex(object, object->type + 1);
    if (*privateData == -1) {
      free(privateData);
      return NULL;
    }
  }

  object->privates[*privateIndex].ptr = privateData;

  return privateData;
}

static void compObjectFreePrivate(CompObject *parent, CompObject *object)
{
  int *privateIndex = compObjectGetPrivate(parent);
  if (privateIndex == NULL)
    return;

  int *privateData = object->privates[*privateIndex].ptr;
  if (privateData == NULL)
    return;

  /* free index of child objects */
  if (object->type < 3)
    compObjectFreePrivateIndex(object, object->type + 1, *privateData);

  object->privates[*privateIndex].ptr = NULL;

  free(privateData);
}

/**
 * Xcolor helper functions. I didn't really want to put them into the Xcolor
 * library.
 * Other window managers are free to copy those when needed.
 */

static inline XcolorRegion *XcolorRegionNext(XcolorRegion *region)
{
  unsigned char *ptr = (unsigned char *) region;
  return (XcolorRegion *) (ptr + sizeof(XcolorRegion));
}

static inline unsigned long XcolorRegionCount(void *data, unsigned long nBytes)
{
  return nBytes / sizeof(XcolorRegion);
}


/**
 * Here begins the real code
 */

static int getFetchTarget(CompTexture *texture)
{
  if (texture->target == GL_TEXTURE_2D) {
    return COMP_FETCH_TARGET_2D;
  } else {
    return COMP_FETCH_TARGET_RECT;
  }
}

/**
 * The shader is the same for all windows and profiles. It only depends on the
 * 3D texture and two environment variables.
 */
static int getProfileShader(CompScreen *s, CompTexture *texture, int param, int unit)
{
  PrivScreen *ps = compObjectGetPrivate((CompObject *) s);

  if (ps->function && ps->param == param && ps->unit == unit)
    return ps->function;

  if (ps->function)
    destroyFragmentFunction(s, ps->function);

  CompFunctionData *data = createFunctionData();

  addFetchOpToFunctionData(data, "output", NULL, getFetchTarget(texture));

  addDataOpToFunctionData(data, "MAD output, output, program.env[%d], program.env[%d];", param, param + 1);
  addDataOpToFunctionData(data, "TEX output, output, texture[%d], 3D;", unit);
  addColorOpToFunctionData (data, "output", "output");

  ps->function = createFragmentFunction(s, "colour_desktop", data);
  ps->param = param;
  ps->unit = unit;

#if defined(PLUGIN_DEBUG)
  oyCompLogMessage( s->display, "colour_desktop", CompLogLevelDebug,
                  DBG_STRING "Shader compiled: %d/%d/%d", DBG_ARGS,
                  ps->function, param, unit);
#endif

  return ps->function;
}

/**
 * Converts a server-side region to a client-side region.
 */
static Region convertRegion(Display *dpy, XserverRegion src)
{
  Region ret = XCreateRegion();

  int nRects = 0;
  XRectangle *rect = XFixesFetchRegion(dpy, src, &nRects);

  for (int i = 0; i < nRects; ++i) {
    XUnionRectWithRegion(&rect[i], ret, ret);
  }

  XFree(rect);

  return ret;
}

static Region windowRegion( CompWindow * w )
{
  Region r = XCreateRegion();
  XRectangle rect = {0,0,w->width, w->height};
  XUnionRectWithRegion( &rect, r, r );
 return r;
}

/**
 * Generic function to fetch a window property.
 */
static void *fetchProperty(Display *dpy, Window w, Atom prop, Atom type, unsigned long *n, Bool delete)
{
  Atom actual;
  int format;
  unsigned long left;
  unsigned char *data;

  int result = XGetWindowProperty(dpy, w, prop, 0, ~0, delete, type, &actual, &format, n, &left, &data);
#if defined(PLUGIN_DEBUG)
  printf( "%s:%d %s delete: %d %s %lu\n", __FILE__,__LINE__,
                XGetAtomName( dpy, prop ), delete,
                (result == Success) ? "fine" : "err", *n );
#endif
  if (result == Success)
    return (void *) data;

  return NULL;
}

static unsigned long colour_desktop_stencil_id_pool = 0;

/**
 * Called when new regions have been attached to a window. Fetches these and
 * saves them in the local list.
 */
static void updateWindowRegions(CompWindow *w)
{
  PrivWindow *pw = compObjectGetPrivate((CompObject *) w);

  CompDisplay *d = w->screen->display;
  PrivDisplay *pd = compObjectGetPrivate((CompObject *) d);

  /* free existing data structures */
  for (unsigned long i = 0; i < pw->nRegions; ++i) {
    if (pw->pRegion[i].xRegion != 0) {
      XDestroyRegion(pw->pRegion[i].xRegion);
    }
  }
  if (pw->nRegions)
    free(pw->pRegion);
  pw->nRegions = 0;
  oyRectangle_Release( &pw->absoluteWindowRectangleOld );


  /* fetch the regions */
  unsigned long nBytes;
  void *data = fetchProperty(d->display, w->id, pd->netColorRegions, XA_CARDINAL, &nBytes, False);

  /* allocate the list */
  unsigned long count = 1;
  if(data)
    count += XcolorRegionCount(data, nBytes + 1);

  pw->pRegion = malloc(count * sizeof(PrivColorRegion));
  if (pw->pRegion == NULL)
    goto out;

  memset(pw->pRegion, 0, count * sizeof(PrivColorRegion));

  /* get the complete windows region and put it at the end */
  pw->pRegion[count-1].xRegion = windowRegion( w );


  /* fill in the possible application region(s) */
  XcolorRegion *region = data;
  Region wRegion = pw->pRegion[count-1].xRegion;
  for (unsigned long i = 0; i < (count - 1); ++i)
  {
    pw->pRegion[i].xRegion = convertRegion( d->display, region->region );

#if defined(PLUGIN_DEBUG)
    BOX * b = &pw->pRegion[i].xRegion->extents;
    if(b)
    printf( DBG_STRING "\n  substract region[%d] %dx%d+%d+%d\n",DBG_ARGS,(int)i,
            b->x2 - b->x1, b->y2 - b->y1, b->x1, b->y1 );
#endif

    /* substract a application region from the window region */
    XSubtractRegion( wRegion, pw->pRegion[i].xRegion, wRegion );

    region = XcolorRegionNext(region);
  }

  pw->nRegions = count;
  pw->active = 1;
  if(pw->nRegions > 1)
    pw->stencil_id = colour_desktop_stencil_id_pool++;
  else
    pw->stencil_id = 0;

#if defined(PLUGIN_DEBUG)
  oyCompLogMessage(d, "colour_desktop", CompLogLevelDebug, "\n  Updated window regions, %d total now; id:%d", count, pw->stencil_id);
#endif

  pw->absoluteWindowRectangleOld = oyRectangle_NewWith( 0,0, w->width, w->height, 0 );

  addWindowDamage(w);

out:
  XFree(data);
}


/**
 * Called when the window target (_NET_COLOR_TARGET) has been changed.
 */
static void updateWindowOutput(CompWindow *w)
{
  PrivWindow *pw = compObjectGetPrivate((CompObject *) w);

  CompDisplay *d = w->screen->display;
  PrivDisplay *pd = compObjectGetPrivate((CompObject *) d);

  if (pw->output)
    XFree(pw->output);

  unsigned long nBytes;
  pw->output = fetchProperty(d->display, w->id, pd->netColorTarget, XA_STRING, &nBytes, False);

#if defined(_NET_COLOR_DEBUG)
  oyCompLogMessage(d, "colour_desktop", CompLogLevelDebug, "Updated window output, target is %s", pw->output);
#endif

  if(!pw->nRegions)
    addWindowDamage(w);
}



static void cdCreateTexture( PrivColorOutput *ccontext )
{
    glBindTexture(GL_TEXTURE_3D, ccontext->glTexture);

    ccontext->scale = (GLfloat) (GRIDPOINTS - 1) / GRIDPOINTS;
    ccontext->offset = (GLfloat) 1.0 / (2 * GRIDPOINTS);

#if defined(PLUGIN_DEBUG)
    printf( DBG_STRING "\n", DBG_ARGS );
#endif

    glGenTextures(1, &ccontext->glTexture);
    glBindTexture(GL_TEXTURE_3D, ccontext->glTexture);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage3D( GL_TEXTURE_3D, 0, GL_RGB16, GRIDPOINTS,GRIDPOINTS,GRIDPOINTS,
                  0, GL_RGB, GL_UNSIGNED_SHORT, ccontext->clut);
}

static void freeOutput( PrivScreen *ps )
{
  if (ps->nCcontexts > 0)
  {
    for (unsigned long i = 0; i < ps->nCcontexts; ++i)
    {
      if(ps->ccontexts[i].oy_profile)
        oyProfile_Release( &ps->ccontexts[i].oy_profile );
      if(ps->ccontexts[i].cc)
         oyConversion_Release( &ps->ccontexts[i].cc );
      if(ps->ccontexts[i].glTexture)
        glDeleteTextures( 1, &ps->ccontexts[i].glTexture );
      ps->ccontexts[i].glTexture = 0;
    }
    free(ps->ccontexts);
  }
}

/**
 * Called when XRandR output configuration (or properties) change. Fetch
 * output profiles (if available) or fall back to sRGB.
 */
static void updateOutputConfiguration(CompScreen *s, CompBool updateWindows)
{
  PrivScreen *ps = compObjectGetPrivate((CompObject *) s);
  int error = 0,
      n;
  oyOptions_s * options = 0;
  oyOption_s * o = 0;
  oyRectangle_s * r = 0;
  oyConfigs_s * devices = 0;
  oyConfig_s * device = 0;
  const char * device_name = 0;
  char num[12];

  START_CLOCK("freeOutput:")
  /* clean memory */
  freeOutput(ps); END_CLOCK

  /* obtain device informations, including geometry and ICC profiles
     from the according Oyranos module */
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                 "list", OY_CREATE_NEW );
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/device_rectangle",
                                 "true", OY_CREATE_NEW );
  /*error = oyOptions_SetFromText( &options,
                                 "//" OY_TYPE_STD "/config/display_name",
                                 DisplayString( s->display->display ),
                                 OY_CREATE_NEW );*/
  error = oyDevicesGet( OY_TYPE_STD, "monitor", options, &devices );
  n = oyOptions_Count( options );
  //printf( DBG_STRING "options: %d devices: %d %s\n", DBG_ARGS, n, oyConfigs_Count( devices ) );
  oyOptions_Release( &options );

  n = oyConfigs_Count( devices );
#if defined(PLUGIN_DEBUG)
  oyCompLogMessage( s->display, "colour_desktop", CompLogLevelDebug,
                  DBG_STRING "Oyranos monitor \"%s\" devices found: %d",
                  DBG_ARGS, DisplayString( s->display->display ), n);
#endif

  ps->nCcontexts = n;
  ps->ccontexts = malloc(ps->nCcontexts * sizeof(PrivColorOutput));
  for (unsigned long i = 0; i < ps->nCcontexts; ++i)
  {
    device = oyConfigs_Get( devices, i );

    o = oyConfig_Find( device, "device_rectangle" );
    if( !o )
    {
      oyCompLogMessage( s->display, "colour_desktop", CompLogLevelWarn,
                      DBG_STRING"monitor rectangle request failed", DBG_ARGS);
      return;
    }
    r = (oyRectangle_s*) oyOption_StructGet( o, oyOBJECT_RECTANGLE_S );
    if( !r )
    {
      oyCompLogMessage( s->display, "colour_desktop", CompLogLevelWarn,
                      DBG_STRING"monitor rectangle request failed", DBG_ARGS);
      return;
    }
    oyOption_Release( &o );

    ps->ccontexts[i].xRect.x = r->x;
    ps->ccontexts[i].xRect.y = r->y;
    ps->ccontexts[i].xRect.width = r->width;
    ps->ccontexts[i].xRect.height = r->height;

    device_name = oyConfig_FindString( device, "device_name", 0 );
    if(device_name && device_name[0])
    {
      strcpy( ps->ccontexts[i].name, device_name );

#if defined(PLUGIN_DEBUG)
      oyCompLogMessage( s->display, "colour_desktop", CompLogLevelDebug,
                      DBG_STRING "  screen output found %s %s",
                      DBG_ARGS, ps->ccontexts[i].name, oyRectangle_Show(r) );
#endif

    } else
    {
       oyCompLogMessage( s->display, "colour_desktop", CompLogLevelWarn,
       DBG_STRING "oyDevicesGet list answere included no device_name",DBG_ARGS);

       sprintf( num, "%d", (int)i );
       strcpy( ps->ccontexts[i].name, num );
    }

    o = oyConfig_Find( device, "icc_profile" );

    ps->ccontexts[i].oy_profile = (oyProfile_s*) 
                                  oyOption_StructGet( o, oyOBJECT_PROFILE_S );

    if(!ps->ccontexts[i].oy_profile)
      oyDeviceGetProfile( device, &ps->ccontexts[i].oy_profile );

    if (ps->ccontexts[i].oy_profile)
    {
#ifdef DEBUG  /* expensive lookup */
      const char * tmp = oyProfile_GetFileName( ps->ccontexts[i].oy_profile, 0 );
      
      oyCompLogMessage(s->display, "colour_desktop", CompLogLevelInfo,
             DBG_STRING "Output %s: extracted profile from Oyranos: %s",
             DBG_ARGS, ps->ccontexts[i].name,
             (strrchr(tmp, OY_SLASH_C)) ? strrchr(tmp, OY_SLASH_C) + 1 : tmp );
#endif

      START_CLOCK("create images")
      oyProfile_s * src_profile = oyProfile_FromStd( oyASSUMED_WEB, 0 );
      oyProfile_s * dst_profile = ps->ccontexts[i].oy_profile;

      oyPixel_t pixel_layout = OY_TYPE_123_16;

      oyImage_s * image_in = oyImage_Create( GRIDPOINTS,GRIDPOINTS*GRIDPOINTS,
                                             ps->ccontexts[i].clut, 
                                             pixel_layout, src_profile, 0 );
      oyProfile_Release( &src_profile );
      oyImage_s * image_out= oyImage_Create( GRIDPOINTS,GRIDPOINTS*GRIDPOINTS,
                                             ps->ccontexts[i].clut,
                                             pixel_layout, dst_profile, 0 );
      oyOptions_s * options = 0;
      /* rendering_high_precission maps to lcms' cmsFLAGS_NOTPRECALC */
      error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/icc/rendering_high_precission",
                                     "1", OY_CREATE_NEW );
      END_CLOCK

      START_CLOCK("oyConversion_CreateBasicPixels: ")
      ps->ccontexts[i].cc = oyConversion_CreateBasicPixels( image_in, image_out,
                                                      options, 0 ); END_CLOCK

      if (ps->ccontexts[i].cc == NULL)
      {
        oyCompLogMessage( s->display, "colour_desktop", CompLogLevelWarn,
                      DBG_STRING "no conversion created for %s",
                      DBG_ARGS, ps->ccontexts[i].name);
        continue;
      }

      START_CLOCK("fill array: ")
      uint16_t in[3];
      for (int r = 0; r < GRIDPOINTS; ++r)
      {
        in[0] = floor((double) r / (GRIDPOINTS - 1) * 65535.0 + 0.5);
        for (int g = 0; g < GRIDPOINTS; ++g) {
          in[1] = floor((double) g / (GRIDPOINTS - 1) * 65535.0 + 0.5);
          for (int b = 0; b < GRIDPOINTS; ++b)
          {
            in[2] = floor((double) b / (GRIDPOINTS - 1) * 65535.0 + 0.5);
            for(int j = 0; j < 3; ++j)
              /* BGR */
              ps->ccontexts[i].clut[b][g][r][j] = in[j];
          }
        }
      } END_CLOCK

      START_CLOCK("oyConversion_RunPixels: ")
      oyConversion_RunPixels( ps->ccontexts[i].cc, 0 ); END_CLOCK

      START_CLOCK("cdCreateTexture: ")
      cdCreateTexture( &ps->ccontexts[i] ); END_CLOCK

      oyOptions_Release( &options );
      oyImage_Release( &image_in );
      oyImage_Release( &image_out );

    } else {
      oyCompLogMessage( s->display, "colour_desktop", CompLogLevelInfo,
                      DBG_STRING "Output %s: omitting sRGB->sRGB conversion",
                      DBG_ARGS, ps->ccontexts[i].name);
      ps->ccontexts[i].oy_profile = 0; /*cmsCreate_sRGBProfile();*/
    }

    oyConfig_Release( &device );
  }


#if defined(PLUGIN_DEBUG)
  oyCompLogMessage( s->display, "colour_desktop", CompLogLevelDebug,
                  DBG_STRING "Updated screen outputs, %d total now %d",
                  DBG_ARGS, ps->nCcontexts, updateWindows);
#endif
  START_CLOCK("damageWindow(s)")
  if(updateWindows)
  {
    int all = 1;
    forEachWindowOnScreen( s, damageWindow, &all );
  } END_CLOCK
}

/**
 * CompDisplay::handleEvent
 */
static void pluginHandleEvent(CompDisplay *d, XEvent *event)
{
  PrivDisplay *pd = compObjectGetPrivate((CompObject *) d);

  UNWRAP(pd, d, handleEvent);
  (*d->handleEvent) (d, event);
  WRAP(pd, d, handleEvent, pluginHandleEvent);

  switch (event->type)
  {
  case PropertyNotify:
#if defined(PLUGIN_DEBUG)
    if (event->xproperty.atom == pd->netColorProfiles ||
        event->xproperty.atom == pd->netColorRegions ||
        event->xproperty.atom == pd->netColorTarget ||
        event->xproperty.atom == pd->netColorDesktop);
      printf( "%s:%d PropertyNotify: %s\n", __FILE__,__LINE__,
               XGetAtomName( event->xany.display, event->xproperty.atom ) );
#endif
    if (event->xproperty.atom == pd->netColorRegions) {
      CompWindow *w = findWindowAtDisplay(d, event->xproperty.window);
      updateWindowRegions(w);
    } else if (event->xproperty.atom == pd->netColorTarget) {
      CompWindow *w = findWindowAtDisplay(d, event->xproperty.window);
      updateWindowOutput(w);

    /* update for a changing monitor profile */
    } else if(
           strstr( XGetAtomName( event->xany.display, event->xproperty.atom ),
                   "_ICC_PROFILE") != 0)
    {
      const char * an = XGetAtomName( event->xany.display,
                                      event->xproperty.atom );
      oyPointer data = 0;
      unsigned long n = 0;

      if(strcmp( "_ICC_PROFILE", an ) == 0)
        an = "_ICC_PROFILE  ";

      data = fetchProperty( d->display, event->xany.window,
                            event->xproperty.atom, XA_CARDINAL, &n, False);
#ifdef DEBUG /* expensive lookup */
      if(n)
      {
        const char * name = 0;
        oyProfile_s * p = oyProfile_FromMem( n, data, 0, 0 );
        name = oyProfile_GetFileName( p, 0 );
        if(name && strchr(name, '/'))
          name = strrchr( name, '/' ) + 1;
        oyProfile_Release( &p );
      }
      printf(" PropertyNotify : %s    \"%s\"[%d]\n",
             an, name?name:"removed",(int)n );
#endif

      if(n)
      {
        int screen = 0;
        if(strlen(an) > strlen("_ICC_PROFILE_"))
          an += strlen("_ICC_PROFILE_");
        screen = atoi( an );

        printf(":%d Profile changed on screen: %d\n", __LINE__, screen );

        updateOutputConfiguration( findScreenAtDisplay(d, event->xany.window),
                                   TRUE);
      }

      if(data) XFree(data);
    }
    break;
  case ClientMessage:
    if (event->xclient.message_type == pd->netColorManagement)
    {
#if defined(PLUGIN_DEBUG)
      printf( "%s:%d ClientMessage: %s\n", __FILE__,__LINE__,
               XGetAtomName( event->xany.display, event->xclient.message_type) );
#endif
      CompWindow *w = findWindowAtDisplay (d, event->xclient.window);
      PrivWindow *pw = compObjectGetPrivate((CompObject *) w);

      pw->active = 1;
    }
    break;
  default:
#ifdef HAVE_XRANDR
    if (event->type == d->randrEvent + RRNotify) {
      XRRNotifyEvent *rrn = (XRRNotifyEvent *) event;
      CompScreen *s = findScreenAtDisplay(d, rrn->window);
#if defined(PLUGIN_DEBUG)
      printf( "%s:%d XRRNotifyEvent\n", __FILE__,__LINE__ );
#endif
      updateOutputConfiguration(s, TRUE);
    }
#endif
    break;
  }
}

/**
 * Make region relative to the window. 
 * Uses static variables to prevent
 * allocating and freeing the Region in pluginDrawWindow().
 */
static Region absoluteRegion(CompWindow *w, Region region)
{
  Region r = XCreateRegion();
  XUnionRegion( region, r, r );

  for (int i = 0; i < r->numRects; ++i) {
    r->rects[i].x1 += w->attrib.x;
    r->rects[i].x2 += w->attrib.x;
    r->rects[i].y1 += w->attrib.y;
    r->rects[i].y2 += w->attrib.y;

    EXTENTS(&r->rects[i], r);
  }

  return r;
}

static void damageWindow(CompWindow *w, void *closure)
{
  PrivWindow *pw = compObjectGetPrivate((CompObject *) w);
  int * all = closure;

  /* scrissored rects seem to be insensible to artifacts from other windows */
  if((pw->stencil_id || (all && *all == 1)) &&
      pw->absoluteWindowRectangleOld /*&&
      (w->type ==1 || w->type == 128) &&
      w->resName*/)
  {
#if defined(PLUGIN_DEBUG_)
    printf( "damaged - %dx%d+%d+%d  %s\n", 
            w->serverWidth, w->serverHeight,w->serverX, w->serverY,
            w->resName?w->resName:"???" );
#endif
    /* what is so expensive */
    addWindowDamage(w);
  }
#if defined(PLUGIN_DEBUG_)
  else
    printf( "%dx%d+%d+%d  resName %s\n", 
            w->serverWidth, w->serverHeight,w->serverX, w->serverY,
            w->resName?w->resName:"???" );
#endif
}


/**
 * CompScreen::drawWindow
 *  The window's texture is mapped on screen.
 *  As this is the second step of drawing a window it is not best suited to
 *  declare a region for colour conversion.
 *  On the other side here can overlapping regions be found to reduce the 
 *  colour transformed area like:
 *  - draw all windows regions into the stencil buffer
 *  - draw all window textures as needed by the flat desktop
 *  - map all windows to the screen
 *  The inclusion of perspective shifts is not reasonably well done.
 */
static Bool pluginDrawWindow(CompWindow *w, const CompTransform *transform, const FragmentAttrib *attrib, Region region, unsigned int mask)
{
  CompScreen *s = w->screen;
  PrivScreen *ps = compObjectGetPrivate((CompObject *) s);
  int i;

  UNWRAP(ps, s, drawWindow);
  Bool status = (*s->drawWindow) (w, transform, attrib, region, mask);
  WRAP(ps, s, drawWindow, pluginDrawWindow);

  /* If no regions have been enabled, just return as we're done */
  PrivWindow *pw = compObjectGetPrivate((CompObject *) w);

  /* initialise window regions */
  if (pw->active == 0)
    updateWindowRegions( w );

  oyRectangle_s * rect = oyRectangle_NewWith( w->serverX, w->serverY, w->serverWidth, w->serverHeight, 0 );

  /* update to window movements and resizes */
  if( !oyRectangle_IsEqual( rect, pw->absoluteWindowRectangleOld ) )
  {
    forEachWindowOnScreen(s, damageWindow, NULL);

    if(rect->width != pw->absoluteWindowRectangleOld->width ||
       rect->height != pw->absoluteWindowRectangleOld->height )
      updateWindowRegions( w );

    /* Clear the stencil buffer with zero. But we do not know when the loop
     * starts */
    //glClear(GL_STENCIL_BUFFER_BIT);

    oyRectangle_SetByRectangle( pw->absoluteWindowRectangleOld, rect );

#if defined(PLUGIN_DEBUG)
    printf( DBG_STRING "%s\n", DBG_ARGS, oyRectangle_Show(rect) );
#endif
  }

  oyRectangle_Release( &rect );

  /* skip the stencil drawing for to be scissored windows */
  if( !pw->stencil_id )
    return status;

  PrivColorRegion *reg = pw->pRegion + pw->nRegions - 1;
  Region aRegion = absoluteRegion( w, reg->xRegion);

  glEnable(GL_STENCIL_TEST);

  /* Replace the stencil value in places where we'd draw something */
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  /* Disable color mask as we won't want to draw anything */
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  for( i = 0; i < ps->nCcontexts; ++i )
  {
    /* Each region gets its own stencil value */
    glStencilFunc(GL_ALWAYS, STENCIL_ID, ~0);

    /* intersect window with monitor */
    Region screen = XCreateRegion();
    XUnionRectWithRegion( &ps->ccontexts[i].xRect, screen, screen );    
    Region intersection = XCreateRegion();
    XIntersectRegion( screen, aRegion, intersection );
    BOX * b = &intersection->extents;
    if(b->x1 == 0 && b->x2 == 0 && b->y1 == 0 && b->y2 == 0)
      goto cleanDrawWindow;

#if defined(PLUGIN_DEBUG_)
    //if(b->y2 - b->y1 == 190)
    //if((int)pw->stencil_id == 7)
    printf( DBG_STRING "%dx%d+%d+%d  %d[%d] on %d\n", DBG_ARGS,
            b->x2 - b->x1, b->y2 - b->y1, b->x1, b->y1,
            (int)pw->stencil_id, (int)STENCIL_ID, i );
    b = &region->extents;
    printf( DBG_STRING "region: %dx%d+%d+%d\n", DBG_ARGS,
            b->x2 - b->x1, b->y2 - b->y1, b->x1, b->y1 );
#endif

    w->vCount = w->indexCount = 0;
    (*w->screen->addWindowGeometry) (w, &w->matrix, 1, intersection, region);

    /* If the geometry is non-empty, draw the window */
    if (w->vCount > 0)
    {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      (*w->drawWindowGeometry) (w);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    cleanDrawWindow:
    XDestroyRegion( intersection );
    XDestroyRegion( screen );
  }

  /* Reset the color mask */
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  glDisable(GL_STENCIL_TEST);

  XDestroyRegion( aRegion ); aRegion = 0;

#if defined(PLUGIN_DEBUG_)
  printf( DBG_STRING "\n", DBG_ARGS );
#endif

  return status;
}

/**
 * CompScreen::drawWindowTexture
 *  The window's texture or content is drawn here.
 *  Where does we know which monitor we draw on? from
 *  - pluginDrawWindow()
 *  - Oyranos
 */
static void pluginDrawWindowTexture(CompWindow *w, CompTexture *texture, const FragmentAttrib *attrib, unsigned int mask)
{
  CompScreen *s = w->screen;
  PrivScreen *ps = compObjectGetPrivate((CompObject *) s);

  UNWRAP(ps, s, drawWindowTexture);
  (*s->drawWindowTexture) (w, texture, attrib, mask);
  WRAP(ps, s, drawWindowTexture, pluginDrawWindowTexture);

   PrivWindow *pw = compObjectGetPrivate((CompObject *) w);
  if (pw->active == 0)
    return;

  /* Set up the shader */
  FragmentAttrib fa = *attrib;

  int param = allocFragmentParameters(&fa, 2);
  int unit = allocFragmentTextureUnits(&fa, 1);

  int function = getProfileShader(s, texture, param, unit);
  if (function)
    addFragmentFunction(&fa, function);

#if 1
  if( pw->stencil_id )
  {
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_ZERO);
  } else
    glEnable(GL_SCISSOR_TEST);
#endif

  if(w->screen->nOutputDev != ps->nCcontexts)
    oyCompLogMessage( s->display, "colour_desktop", CompLogLevelWarn,
                    DBG_STRING "Need to update screen outputs, %d / %d",
                    DBG_ARGS, ps->nCcontexts, w->screen->nOutputDev );

  for(int i = 0; i < ps->nCcontexts; ++i)
  {
    /* get the window region */
    PrivColorRegion *reg = pw->pRegion + pw->nRegions - 1;
    Region tmp = absoluteRegion( w, reg->xRegion);
    Region screen = XCreateRegion();
    XUnionRectWithRegion( &ps->ccontexts[i].xRect, screen, screen );    
    Region intersection = XCreateRegion();

    /* create intersection of window and monitor */
    XIntersectRegion( screen, tmp, intersection );

    BOX * b = &intersection->extents;

    GLint width =  b->x2 - b->x1,
          height = b->y2 - b->y1;
    glScissor( b->x1, s->height - b->y1 - height, width, height);

    if(WINDOW_INVISIBLE(w))
      goto cleanDrawTexture;

    if(b->x1 == 0 && b->x2 == 0 && b->y1 == 0 && b->y2 == 0)
      goto cleanDrawTexture;

    PrivColorOutput * c = &ps->ccontexts[i];
    /* Set the environment variables */
    glProgramEnvParameter4dARB( GL_FRAGMENT_PROGRAM_ARB, param + 0, 
                                c->scale, c->scale, c->scale, 1.0);
    glProgramEnvParameter4dARB( GL_FRAGMENT_PROGRAM_ARB, param + 1,
                                c->offset, c->offset, c->offset, 0.0);

    /* Activate the 3D texture */
    (*s->activeTexture) (GL_TEXTURE0_ARB + unit);
    glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, c->glTexture);
    (*s->activeTexture) (GL_TEXTURE0_ARB);

    /* Only draw where the stencil value matches the window and output */
    glStencilFunc(GL_EQUAL, STENCIL_ID, ~0);

#if defined(PLUGIN_DEBUG_)
#if 1
    //if(b->y2 - b->y1 == 190)
    //if((int)pw->stencil_id == 7)
    printf( DBG_STRING "%dx%d+%d+%d  %d[%d] on %d\n", DBG_ARGS,
            b->x2 - b->x1, b->y2 - b->y1, b->x1, b->y1,
            (int)pw->stencil_id, (int)STENCIL_ID, i );
#else
    printf( DBG_STRING "%d[%d]\n", DBG_ARGS, (int)pw->stencil_id,
            (int)(pw->stencil_id*ps->nCcontexts + i + 1));
#endif
#endif

    /* Now draw the window texture */
    UNWRAP(ps, s, drawWindowTexture);
    (*s->drawWindowTexture) (w, texture, &fa, mask);
    WRAP(ps, s, drawWindowTexture, pluginDrawWindowTexture);

    cleanDrawTexture:
    XDestroyRegion( intersection );
    XDestroyRegion( tmp );
    XDestroyRegion( screen );
  }

  /* Deactivate the 3D texture */
  (*s->activeTexture) (GL_TEXTURE0_ARB + unit);
  glBindTexture(GL_TEXTURE_3D, 0);
  glDisable(GL_TEXTURE_3D);
  (*s->activeTexture) (GL_TEXTURE0_ARB);

  glDisable(GL_STENCIL_TEST);
  glDisable(GL_SCISSOR_TEST);

#if defined(PLUGIN_DEBUG_)
  printf( DBG_STRING "\n", DBG_ARGS );
#endif
}


/**
 * This is really stupid, object->parent isn't inisialized when 
 * pluginInitObject() is called. So this is a wrapper to get the parent because
 * compObjectAllocPrivate() needs it.
 */
static CompObject *getParent(CompObject *object)
{
  switch (object->type) {
  case 0:
    return NULL;
  case 1:
    return (CompObject *) &core;
  case 2:
    return (CompObject *) ((CompScreen *) object)->display;
  case 3:
    return (CompObject *) ((CompWindow *) object)->screen;
  default:
    return NULL;
  }
}

/**
 *    Object Init Functions
 */

static CompBool pluginInitCore(CompPlugin *plugin, CompObject *object, void *privateData)
{
  return TRUE;
}

static CompBool pluginInitDisplay(CompPlugin *plugin, CompObject *object, void *privateData)
{
  CompDisplay *d = (CompDisplay *) object;
  PrivDisplay *pd = privateData;

  if (d->randrExtension == False)
    return FALSE;

  WRAP(pd, d, handleEvent, pluginHandleEvent);

  pd->netColorManagement = XInternAtom(d->display, "_NET_COLOR_MANAGEMENT", False);

  pd->netColorProfiles = XInternAtom(d->display, "_NET_COLOR_PROFILES", False);
  pd->netColorRegions = XInternAtom(d->display, "_NET_COLOR_REGIONS", False);
  pd->netColorTarget = XInternAtom(d->display, "_NET_COLOR_TARGET", False);
  pd->netColorDesktop = XInternAtom(d->display, "_NET_COLOR_DESKTOP", False);

  unsigned long n = 0;
  char * data = fetchProperty( d->display, RootWindow(d->display,0),
                               pd->netColorDesktop, XA_CARDINAL, &n, False);

  /* set the colour management desktop service activity atom */
  pid_t pid = getpid();
  pid_t old_pid = 0;
  if(n && data)
    old_pid = *((pid_t*)data);
  if(old_pid)
    oyCompLogMessage( d, "colour_desktop", CompLogLevelWarn,
                    DBG_STRING "\n!!! Found old _NET_COLOR_DESKTOP pid: %d.\n"
                    "Eigther there was a previous crash or your setup can be double colour corrected.",
                    DBG_ARGS, old_pid );
  XChangeProperty( d->display, RootWindow(d->display,0),
                                pd->netColorDesktop, XA_CARDINAL,
                                8, PropModeReplace, (unsigned char*)&pid,
                                sizeof(pid_t) );

  return TRUE;
}

static CompBool pluginInitScreen(CompPlugin *plugin, CompObject *object, void *privateData)
{
  CompScreen *s = (CompScreen *) object;
  PrivScreen *ps = privateData;

  GLint stencilBits = 0;
  glGetIntegerv(GL_STENCIL_BITS, &stencilBits);
  if (stencilBits == 0)
    return FALSE;

  WRAP(ps, s, drawWindow, pluginDrawWindow);
  WRAP(ps, s, drawWindowTexture, pluginDrawWindowTexture);

  ps->nProfiles = 0;
  ps->profile = NULL;

  ps->function = 0;

  /* XRandR setup code */

#ifdef HAVE_XRANDR
  XRRSelectInput(s->display->display, s->root, RROutputPropertyNotifyMask);
#endif

  ps->nCcontexts = 0;
  updateOutputConfiguration(s, FALSE);

  return TRUE;
}

static CompBool pluginInitWindow(CompPlugin *plugin, CompObject *object, void *privateData)
{
  /* CompWindow *w = (CompWindow *) object; */
  PrivWindow *pw = privateData;

  pw->nRegions = 0;
  pw->pRegion = 0;
  pw->active = 0;

  pw->absoluteWindowRectangleOld = 0;
  pw->output = NULL;

  return TRUE;
}

static dispatchObjectProc dispatchInitObject[] = {
  pluginInitCore, pluginInitDisplay, pluginInitScreen, pluginInitWindow
};

/**
 *    Object Fini Functions
 */


static CompBool pluginFiniCore(CompPlugin *plugin, CompObject *object, void *privateData)
{
  /* Don't crash if something goes wrong inside lcms */
  //cmsErrorAction(LCMS_ERRC_WARNING);

  return TRUE;
}

static CompBool pluginFiniDisplay(CompPlugin *plugin, CompObject *object, void *privateData)
{
  CompDisplay *d = (CompDisplay *) object;
  PrivDisplay *pd = privateData;

  UNWRAP(pd, d, handleEvent);

  /* remove desktop colour management service mark */
  XDeleteProperty( d->display, RootWindow(d->display, 0), pd->netColorDesktop );

  return TRUE;
}

static CompBool pluginFiniScreen(CompPlugin *plugin, CompObject *object, void *privateData)
{
  CompScreen *s = (CompScreen *) object;
  PrivScreen *ps = privateData;

  /* clean memory */
  freeOutput(ps);

  UNWRAP(ps, s, drawWindow);
  UNWRAP(ps, s, drawWindowTexture);

  return TRUE;
}

static CompBool pluginFiniWindow(CompPlugin *plugin, CompObject *object, void *privateData)
{
  return TRUE;
}

static dispatchObjectProc dispatchFiniObject[] = {
  pluginFiniCore, pluginFiniDisplay, pluginFiniScreen, pluginFiniWindow
};


/**
 *    Plugin Interface
 */
static CompBool pluginInit(CompPlugin *p)
{
  corePrivateIndex = allocateCorePrivateIndex();

  if (corePrivateIndex < 0)
    return FALSE;

  return TRUE;
}

static CompBool pluginInitObject(CompPlugin *p, CompObject *o)
{
  static const int privateSizes[] = {
    sizeof(PrivCore), sizeof(PrivDisplay), sizeof(PrivScreen), sizeof(PrivWindow)
  };

  void *privateData = compObjectAllocPrivate(getParent(o), o, privateSizes[o->type]);
  if (privateData == NULL)
    return TRUE;

  if (dispatchInitObject[o->type](p, o, privateData) == FALSE)
    compObjectFreePrivate(getParent(o), o);

  return TRUE;
}

static void pluginFiniObject(CompPlugin *p, CompObject *o)
{
  void *privateData = compObjectGetPrivate(o);
  if (privateData == NULL)
    return;

  dispatchFiniObject[o->type](p, o, privateData);
  compObjectFreePrivate(getParent(o), o);
}

static void pluginFini(CompPlugin *p)
{
  freeCorePrivateIndex(corePrivateIndex);
}

static CompMetadata *pluginGetMetadata(CompPlugin *p)
{
  return &pluginMetadata;
}

CompPluginVTable pluginVTable = {
  "colour_desktop",
  pluginGetMetadata,
  pluginInit,
  pluginFini,
  pluginInitObject,
  pluginFiniObject,
  0,
  0
};

CompPluginVTable *getCompPluginInfo20070830(void)
{
  return &pluginVTable;
}

