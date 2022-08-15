/** @file oyranos_monitor.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2005-2018 (C) Kai-Uwe Behrmann
 *
 *  @brief    monitor device detection
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2005/01/31
 */

#ifndef OY_UNUSED
#if   (__GNUC__*100 + __GNUC_MINOR__) >= 406
#define OY_UNUSED                      __attribute__ ((unused))
#elif defined(_MSC_VER)
#define OY_UNUSED                      __declspec(unused)
#else
#define OY_UNUSED
#endif
#endif

#ifndef OY_DBG_FORMAT_
#define DBG_UHR_ (double)clock()/(double)CLOCKS_PER_SEC
#if defined(__GNUC__)
# define  OY_DBG_FORMAT_ " %.06g %s:%d %s() "
# define  OY_DBG_ARGS_   DBG_UHR_, strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__,__func__
#else
# define  OY_DBG_FORMAT_ " %.06g %s:%d "
# define  OY_DBG_ARGS_   DBG_UHR_, strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__
#endif
#endif

#ifndef oyNoEmptyString_m_
#define oyNoEmptyString_m_(x) x?x:"---"
#endif

#include "oyranos_monitor_hooks.h"
#include "oyranos_monitor_hooks_x11.h"

#include <ctype.h> /* isdigit() */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <locale.h>
#include <errno.h>
#include <time.h>

#ifdef PRINT_TIME_INCLUDE
#include PRINT_TIME_INCLUDE
#endif

/* ---  Helpers  --- */

#define noE( string ) ((string)?(string):"")
extern int oy_debug; /* print debug infos on stderr */
enum {
  oyOBJECT_MONITOR_S = 2,
  oyOBJECT_MONITOR_HOOKS_S = 120,
  oyOBJECT_MONITOR_HOOKS2_S = 122,
  oyOBJECT_MONITOR_HOOKS3_S = 123,
};

#ifndef MAX_PATH
#define MAX_PATH 2048
#endif

#define oyX1Alloc( ptr_, size_, action ) { ptr_ = malloc(size_); \
  if(!ptr_) { fprintf( stderr, OY_DBG_FORMAT_ "ERROR: malloc failed %d\n", OY_DBG_ARGS_, (int)size_ ); action } }


/* --- internal API definition --- */

/** @internal Display functions */
const char* oyX1Monitor_name_( oyMonitor_s *disp ) { return disp->name; }
const char* oyX1Monitor_hostName_( oyMonitor_s *disp ) { return disp->host; }
const char* oyX1Monitor_identifier_( oyMonitor_s *disp ) { return disp->identifier; }
/** @internal the screen appendment for the root window properties */
char*       oyX1Monitor_screenIdentifier_( oyMonitor_s *disp )
{ char *number = 0;

  oyX1Alloc( number, 24, return ""; );
  number[0] = 0;
  if( disp->geo[1] >= 1 && !disp->screen ) sprintf( number,"_%d", disp->geo[1]);
  return number;
}
int oyX1Monitor_deviceScreen_( oyMonitor_s *disp ) { return disp->screen; }
int oyX1Monitor_number_( oyMonitor_s *disp ) { return disp->geo[0]; }
int oyX1Monitor_screen_( oyMonitor_s *disp ) { return disp->geo[1]; }
int oyX1Monitor_x_( oyMonitor_s *disp ) { return disp->geo[2]; }
int oyX1Monitor_y_( oyMonitor_s *disp ) { return disp->geo[3]; }
int oyX1Monitor_width_( oyMonitor_s *disp ) { return disp->geo[4]; }
int oyX1Monitor_height_( oyMonitor_s *disp ) { return disp->geo[5]; }
int   oyX1Monitor_getGeometryIdentifier_(oyMonitor_s         * disp );
Display* oyX1Monitor_device_( oyMonitor_s *disp ) { return disp->display; }
const char* oyX1Monitor_systemPort_( oyMonitor_s *disp ) { return disp->system_port; }

oyX11INFO_SOURCE_e
    oyX1Monitor_infoSource_( oyMonitor_s *disp ) { return disp->info_source; }
# if defined(HAVE_XRANDR)
XRRScreenResources *
    oyX1Monitor_xrrResource_( oyMonitor_s * disp ) { return disp->res; }
RROutput
    oyX1Monitor_xrrOutput_( oyMonitor_s * disp ) { return disp->output; }
XRROutputInfo *
    oyX1Monitor_xrrOutputInfo_( oyMonitor_s * disp ) { return disp->output_info; }
int oyX1Monitor_activeOutputs_( oyMonitor_s * disp ) { return disp->active_outputs; }
int oyX1Monitor_rrVersion_    ( oyMonitor_s * disp ) { return disp->rr_version; }
int oyX1Monitor_rrScreen_     ( oyMonitor_s * disp ) { return disp->rr_screen; }


#endif

char* oyX1Monitor_getScreenAtomName_ ( oyMonitor_s       * disp,
                                       const char        * base );
const char *xrandr_edids[] = {"EDID","EDID_DATA",0};



/** @internal
 *  Function oyX1Monitor_getProperty_
 *  @brief   obtain X property
 *
 *  The returned property is owned by the caller.
 *
 *  @version Oyranos: 0.9.7
 *  @date    2021/11/24
 *  @since   2009/01/17 (Oyranos: 0.1.10)
 */
char *   oyX1Monitor_getProperty_    ( oyMonitor_s       * disp,
                                       const char        * prop_name,
                                       const char       ** prop_name_xrandr,
                                       size_t            * prop_size,
                                       oyX11INFO_SOURCE_e* source,
                                       char             ** atom_name )
{
  char * prop = 0;
  Display *display = 0;
  Window w = 0;
  Atom atom = 0, a;
  int actual_format_return;
  unsigned long nitems_return=0, bytes_after_return=0;
  unsigned char* prop_return=0;
  int error = !disp;

  if(!error)
  {
    display = oyX1Monitor_device_( disp );
# if defined(HAVE_XRANDR)
    if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR )
    {
      int i = 0;
      if(prop_name_xrandr)
        while(!atom && prop_name_xrandr[i])
          atom = XInternAtom( display,
                              prop_name_xrandr[i++],
                              True );
      else
        atom = XInternAtom( display, prop_name, True );

      if(atom)
      {
        error =
        XRRGetOutputProperty ( display, oyX1Monitor_xrrOutput_( disp ),
                      atom, 0, INT_MAX,
                      False, False, AnyPropertyType, &a,
                      &actual_format_return, &nitems_return,
                      &bytes_after_return, &prop_return );

        if(error != Success)
          fprintf( stderr,"%s nitems_return: %lu, bytes_after_return: %lu %d\n",
                   "found issues", nitems_return, bytes_after_return,
                   error );
        else
          *source = oyX11INFO_SOURCE_XRANDR;
        *atom_name = XGetAtomName(display, atom);
        if(oy_debug) fprintf( stderr, "root: %d atom: %ld atom_name: %s prop_name: %s %lu %lu\n",
                  (int)w, atom, *atom_name, prop_name, nitems_return,bytes_after_return );
      }
    }
#else
    if(oy_debug) fprintf( stderr,"!HAVE_XRANDR\n");
# endif
    atom = 0;

    if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XINERAMA ||
        oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_SCREEN ||
        (oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR &&
          !nitems_return) )
    {
      *atom_name = oyX1Monitor_getScreenAtomName_( disp, prop_name );
      if(*atom_name)
        atom = XInternAtom(display, *atom_name, True);
      if(atom)
        w = RootWindow( display, oyX1Monitor_deviceScreen_( disp ) );
      if(w)
        /* AnyPropertyType does not work for XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE ---vvvvvvvvvv */
        XGetWindowProperty( display, w, atom, 0, INT_MAX, False,
                     AnyPropertyType,
                     &a, &actual_format_return, &nitems_return, 
                     &bytes_after_return, &prop_return );
      if(bytes_after_return != 0) fprintf( stderr,"%s bytes_after_return: %lu\n",
                                          "found issues",bytes_after_return);
      if(oy_debug) fprintf( stderr, "root: %d atom: %ld atom_name: %s prop_name: %s %lu %lu\n",
                  (int)w, atom, *atom_name, prop_name, nitems_return,bytes_after_return );
      *source = oyX11INFO_SOURCE_XINERAMA;
    }
  }

  if(nitems_return && prop_return)
  {
    oyX1Alloc( prop, nitems_return, return prop; )
    memcpy( prop, prop_return, nitems_return );
    *prop_size = nitems_return;
    XFree( prop_return ); prop_return = 0;
  }

  return prop;
}

int      oyX1Monitor_setProperty_    ( oyMonitor_s       * disp,
                                       const char        * prop_name,
                                       char              * prop,
                                       size_t              prop_size )
{
  Display *display = 0;
  Window w = 0;
  Atom atom = 0;
  char *atom_name = 0;
  int error = !disp;

  if(!error)
  {
    display = oyX1Monitor_device_( disp );
# if defined(HAVE_XRANDR)
    if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR )
    {
      atom = XInternAtom( display, prop_name, True );

      if(atom)
      {
        if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR )
        {
          XRRChangeOutputProperty( display, oyX1Monitor_xrrOutput_( disp ),
			           atom, XA_CARDINAL, 8, PropModeReplace,
			           (unsigned char *)prop, (int)prop_size );
          if(oy_debug) fprintf( stderr,"XRRChangeOutputProperty[%s] = %lu\n", prop_name, prop_size);
          if(prop_size == 0)
          XRRDeleteOutputProperty( display, oyX1Monitor_xrrOutput_( disp ), atom );
        }
      }
    }
#else
    if(oy_debug) fprintf( stderr,"!HAVE_XRANDR\n");
# endif
    atom = 0;

    if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XINERAMA ||
        oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_SCREEN ||
        oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR )
    {
      atom_name = oyX1Monitor_getScreenAtomName_( disp, prop_name );
      if(atom_name)
        atom = XInternAtom(display, atom_name, True);
      if(atom)
        w = RootWindow( display, oyX1Monitor_deviceScreen_( disp ) );
      if(w)
        /* AnyPropertyType does not work for XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE ---vvvvvvvvvv */
        error = XChangeProperty( display, w, atom, XA_CARDINAL,
                       8, PropModeReplace, (unsigned char*)prop, (int)prop_size );
      if(1||oy_debug) fprintf( stderr,"XChangeProperty[%s] = %lu\n", atom_name, prop_size);
      if(atom_name)
        free( atom_name );
    }
  }

  return error;
}

/*#define IGNORE_EDID 1*/

int      oyX1GetMonitorEdid          ( oyMonitor_s       * disp,
                                       char             ** edid,
                                       size_t            * edid_size,
                                       int                 refresh_edid,
                                       oyX11INFO_SOURCE_e* source,
                                       char             ** atom_name )
{
  char * prop = 0;
  size_t prop_size = 0;
  int error = 0;
 
#if !defined(IGNORE_EDID)
  prop = oyX1Monitor_getProperty_( disp, "XFree86_DDC_EDID1_RAWDATA",
                                   xrandr_edids, &prop_size,
                                   source, atom_name );
#else
    if(oy_debug) fprintf( stderr,OY_DBG_FORMAT_ "IGNORE_EDID\n", OY_DBG_ARGS_ );
#endif

  if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XINERAMA &&
      ((!prop || (prop && prop_size%128)) ||
       refresh_edid ) )
  {
#if !defined(IGNORE_EDID)
    prop = oyX1Monitor_getProperty_( disp, "XFree86_DDC_EDID1_RAWDATA",
                                     xrandr_edids, &prop_size,
                                     source, atom_name );
#else
    if(oy_debug) fprintf( stderr,OY_DBG_FORMAT_ "IGNORE_EDID\n", OY_DBG_ARGS_ );
#endif
  }

  if( prop )
  {
    if( prop_size%128 )
    {
      fprintf( stderr, OY_DBG_FORMAT_ "\n\t  %s %d; %s %s\n",OY_DBG_ARGS_, "unexpected EDID lenght",
               (int)prop_size,
               "\"XFree86_DDC_EDID1_RAWDATA\"/\"EDID_DATA\"",
               "Cant read hardware information from device.");
      error = -1;
    }
  }

  if(edid)
  {
    *edid = prop;
    *edid_size = prop_size;
    prop = 0;
  }

  if(prop) free( prop );

  return error;
}

char * oyX1OpenFile( const char * file_name,
                        size_t   * size_ptr )
{
  FILE * fp = NULL;
  size_t size = 0, s = 0;
  char * text = NULL;

  if(file_name)
  {
    fp = fopen(file_name,"rb");
    if(fp)
    {
      fseek(fp,0L,SEEK_END); 
      size = ftell (fp);
      if(size == (size_t)-1)
      {
        switch(errno)
        {
          case EBADF:        fprintf(stderr, OY_DBG_FORMAT_ "Not a seekable stream", OY_DBG_ARGS_ ); break;
          case EINVAL:       fprintf(stderr, OY_DBG_FORMAT_ "Wrong argument", OY_DBG_ARGS_); break;
          default:           fprintf(stderr, OY_DBG_FORMAT_ "%s", OY_DBG_ARGS_, strerror(errno)); break;
        }
        if(size_ptr)
          *size_ptr = size;
        fclose( fp );
        return NULL;
      }
      rewind(fp);
      oyX1Alloc(text, size+1, fclose( fp ); return NULL;)
      s = fread(text, sizeof(char), size, fp);
      text[size] = '\000';
      if(s != size)
        fprintf( stderr, OY_DBG_FORMAT_ "Error: fread %lu but should read %lu", OY_DBG_ARGS_,
                (long unsigned int) s, (long unsigned int)size);
      fclose( fp );
    } else
    {
      fprintf( stderr, OY_DBG_FORMAT_ "Error: Could not open file - \"%s\"", OY_DBG_ARGS_, file_name);
    }
  }

  if(size_ptr)
    *size_ptr = size;

  return text;
}

#define CASE_RETURN_ENUM( text_ ) case text_: return #text_;
static const char * oyX11INFO_SOURCE_eToString( oyX11INFO_SOURCE_e type )
{
  switch( type )
  {
    CASE_RETURN_ENUM(oyX11INFO_SOURCE_XINERAMA)
    CASE_RETURN_ENUM(oyX11INFO_SOURCE_SCREEN)
    CASE_RETURN_ENUM(oyX11INFO_SOURCE_XRANDR)
  }
  return NULL;
}

typedef struct {
    char                sig[4];
    uint32_t            offset;
    uint32_t            size;
} iccTag;

int           UTF16toASCII           ( const char        * input,
                                       size_t              len,
                                       char              * output,
                                       const char        * from_codeset )
{
  int error = 0;
  error = !memcpy(output, input, sizeof(char) * len);
  output[len] = 0;

  /* cheap fallback for UTF-16 to ASCII */
  {
    int i;
    int low_byte = strcmp(from_codeset,"UTF-16BE") == 0;

    for(i = 0 ; i < (int)len; i += 2)
      output[i/2] = input[i+low_byte];

    output[i/2] = 0;
  }

  return error;
}

char * icProfileName( const char * p, int psize, int verbose )
{
  int moff = 4+4+4+4+4+4+12; /* magic offset */
  if(psize < 132)
  {
    fprintf( stderr, "Size too small for a ICC profile: %d\n", psize );
    return NULL;
  }
  if(verbose)
    fprintf( stderr, "ICC profile: %d\n", psize );
  if(verbose)
    fprintf( stderr, "ICC_magic:%c%c%c%c\n", p[moff],p[moff+1],p[moff+2],p[moff+3]);
  int tag_count = htonl(*(uint32_t*)&p[128]);
  if(verbose)
    fprintf( stderr, "ICC_tag_count:%d\n", tag_count);
  if(128 + tag_count*12 > psize)
    fprintf( stderr, "profile data exceeds memory:%d/%d\n", 128+tag_count*12, psize);
  char * pname = NULL;
  int i;
  for(i = 0; i < tag_count; ++i)
  {
    const iccTag * ttag = (const iccTag*) &p[128+4 + 12*i]; /* table tag */
    const char * s = ttag->sig;
    int toff = htonl(ttag->offset);
    int tag_size = htonl(ttag->size);
    int pass = 0;
    if(toff + tag_size > psize)
      fprintf( stderr, "profile tag data exceeds memory:%d/%d\n",
          128+toff+tag_size, psize);
    else
      ++pass;
    if(verbose)
      fprintf( stderr, "ICC_tag[%d]:%c%c%c%c %d+%d\n",
          i, s[0],s[1],s[2],s[3], toff, tag_size );
    if(pass)
    {
      const char * tag = &p[toff];
      const char * ttype = tag;
      int is_desc = memcmp(s,"desc",4) == 0 || memcmp(s,"dscm",4) == 0;
      if( memcmp(ttype,"desc", 4) == 0 ||
          is_desc )
      {
        if(memcmp(s,"desc",4) == 0 && memcmp(ttype,"desc", 4) == 0)
        {
          int tlen = htonl(*(uint32_t*)&tag[4+4]);
          s = ttype;
          if(verbose)
            fprintf( stderr, "ICC_tag[%d]type:%c%c%c%c %d\n",
                i, s[0],s[1],s[2],s[3], tlen );
          if(tlen <= tag_size - 20)
          {
            pname = (char*) calloc( tlen + 8, sizeof(char) );
            memcpy( pname, &tag[4+4+4], tlen );
            break;
          }
        } else
        if(memcmp(ttype,"mluc",4) == 0)
        {
          int count = htonl( *(uint32_t*)&tag[8] );
          int size = htonl( *(uint32_t*)&tag[12] ); /* 12 */
          int j, error = 0, len;
          char * t = NULL;
          s = ttype;
          if(verbose)
            fprintf( stderr, "ICC_tag[%d]type:%c%c%c%c %d %d\n", i,
                s[0],s[1],s[2],s[3], count, size );
          if(!error)
          for (j = 0; j < count; j++)
          {
            char lang[4] = {0,0,0,0};
            int  g = 0,
                 offset = 0;

            error = tag_size < 20 + j * size;
            if(!error)
              g = htonl( *(uint32_t*)&tag[20+ j*size] );

            lang[0] = tag[16+ j*size];
            lang[1] = tag[17+ j*size];
            if(verbose)
              fprintf( stderr, "ICC_tag[%d]lang[%d]:%c%c\n", i,j, 
                  lang[0],lang[1] );
            error = tag_size < 20 + j * size + g + 4;
            if(!error)
            {
              len = (g > 1) ? g : 8;
              t = (char*) malloc(len*4);
              error = !t;
            }
            if(!error)
              t[0] = 0;

            if(!error)
              error = (24 + j*size + 4) > tag_size;

            if(!error)
              offset = htonl( *(uint32_t*)&tag[24+ j*size] );
            if(!error)
            {
              /* ICC says UTF-16BE */
              error = UTF16toASCII( &tag[offset], len, t, "UTF-16BE" );

              /* eigther text or we have a non translatable string */
              if(!error && strlen(t))
              {
                if(verbose)
                  fprintf( stderr, "ICC_tag[%d]lang_country[%d]:%s_%c%c %s\n", i,j, 
                              lang, tag[18+ j*size], tag[19+ j*size], t);

              }
              if( is_desc &&
                  t && t[0] && strcasecmp(lang,"en") == 0)
              {
                pname = strdup(t);
                i = tag_count;
                break;
              }
            }
            if(t) { free(t); t = NULL; }
          }
        } else
        {
          s = ttype;
          if(verbose)
            fprintf( stderr, "ICC_tag[%d]type:%c%c%c%c\n", i, s[0],s[1],s[2],s[3] );
        }
      }
    }
  }
  return pname;
}

/** @brief pick up monitor information with Xlib
 *  @deprecated because sometimes is no ddc information available
 *  @todo include connection information - grafic cart
 *
 *  @param      display_name  the display string
 *  @param[out] manufacturer  the manufacturer of the monitor device
 *  @param[out] model         the model of the monitor device
 *  @param[out] serial        the serial number of the monitor device
 *  @return     error
 *
 */
int      oyX1GetMonitorInfo          ( oyMonitor_s       * disp,
                                       char             ** manufacturer OY_UNUSED,
                                       char             ** mnft,
                                       char             ** model OY_UNUSED,
                                       char             ** serial OY_UNUSED,
                                       char             ** vendor OY_UNUSED,
                                       char             ** display_geometry,
                                       char             ** system_port,
                                       char             ** host,
                                       int               * week,
                                       int               * year,
                                       int               * mnft_id OY_UNUSED,
                                       int               * model_id,
                                       double            * colors,
                                       char             ** edid,
                                       size_t            * edid_size,
                                       int                 refresh_edid,
                                       char             ** debug_info )
{
  int len;
  char *t, * port = 0, * geo = 0;
  const char * display_name = oyX1Monitor_name_(disp);
  char * prop = 0;
  size_t prop_size = 0;
  int error = 0;
  oyX11INFO_SOURCE_e source;
  char * atom_name = NULL;

  if(display_name)
    if(oy_debug) fprintf( stderr, OY_DBG_FORMAT_ "display_name %s\n", OY_DBG_ARGS_, display_name);

  {
    t = 0;
    if( oyX1Monitor_systemPort_( disp ) &&
        strlen(oyX1Monitor_systemPort_( disp )) )
    {
      len = strlen(oyX1Monitor_systemPort_( disp ));
      ++len;
      oyX1Alloc( t, len, )
      strcpy(t, oyX1Monitor_systemPort_( disp ));
    }
    port = t;
    if( system_port ) 
      *system_port = port;
    t = 0;
  }

  if( display_geometry )
    *display_geometry = strdup( oyX1Monitor_identifier_( disp ) );
  else
    geo = strdup( oyX1Monitor_identifier_( disp ) );
  if( host )
    *host = strdup( oyX1Monitor_hostName_( disp ) );

  error = oyX1GetMonitorEdid( disp, &prop, &prop_size, refresh_edid,
                                    &source, &atom_name );
  if(debug_info)
  {
    char * icc;
    size_t icc_size = 0;
    char * pname = NULL;
    oyX1Alloc(t, 4096,)
    t[0] = '\000';
    sprintf( &t[strlen(t)],"EDID_source:%s\n", oyX11INFO_SOURCE_eToString(source) );
    if(atom_name)
      sprintf( &t[strlen(t)],"EDID_atom_name:%s\n", atom_name );
    if(atom_name) { free(atom_name); atom_name = NULL; }
    sprintf( &t[strlen(t)],"EDID_size:%ld\n", (long)prop_size );

#if defined(HAVE_XRANDR)
    sprintf( &t[strlen(t)],"XRR_version:%d\n", oyX1Monitor_rrVersion_(disp) );
    sprintf( &t[strlen(t)],"XRR_screen:%d(%d)\n", oyX1Monitor_rrScreen_(disp), oyX1Monitor_activeOutputs_(disp) );
    sprintf( &t[strlen(t)],"XRR_size:%dmmx%dmm\n", disp->mm_width, disp->mm_height );
#endif

#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
  /* support the color server device profile */
    icc_size = 0;
    icc = oyX1Monitor_getProperty_( disp,
                             XCM_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE, 0,
                                    &icc_size,
                                    &source, &atom_name );
    sprintf( &t[strlen(t)],"ICC_server_source:%s\n", oyX11INFO_SOURCE_eToString(source) );
    if(atom_name)
      sprintf( &t[strlen(t)],"ICC_server_atom_name:%s\n", atom_name );
    sprintf( &t[strlen(t)],"ICC_server_size:%ld\n", (long)icc_size );
    if((pname = icProfileName(icc, icc_size, 0)) != NULL)
    {
      sprintf( &t[strlen(t)],"ICC_server_name:%s\n", pname );
      free(pname); pname = NULL;
    }
    if(atom_name) { free(atom_name); atom_name = NULL; }
    if(icc) free(icc);
    icc_size = 0;
    icc = oyX1Monitor_getProperty_( disp, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE, 0, &icc_size,
                                    &source, &atom_name );
    sprintf( &t[strlen(t)],"ICC_v0_3_target_source:%s\n", oyX11INFO_SOURCE_eToString(source) );
    if(atom_name)
      sprintf( &t[strlen(t)],"ICC_v0_3_target_atom_name:%s\n", atom_name );
    sprintf( &t[strlen(t)],"ICC_v0_3_target_size:%ld\n", (long)icc_size );
    if((pname = icProfileName(icc, icc_size, 0)) != NULL)
    {
      sprintf( &t[strlen(t)],"ICC_v0_3_target_name:%s\n", pname );
      free(pname); pname = NULL;
    }
    if(atom_name) { free(atom_name); atom_name = NULL; }
    if(icc) free(icc);
    icc_size = 0;
    icc = oyX1Monitor_getProperty_( disp, "_ICC_PROFILE", 0, &icc_size,
                                    &source, &atom_name );
    sprintf( &t[strlen(t)],"ICC_v0_3_source:%s\n", oyX11INFO_SOURCE_eToString(source) );
    if(atom_name)
      sprintf( &t[strlen(t)],"ICC_v0_3_atom_name:%s\n", atom_name );
    sprintf( &t[strlen(t)],"ICC_v0_3_size:%ld\n", (long)icc_size );
    if((pname = icProfileName(icc, icc_size, 0)) != NULL)
    {
      sprintf( &t[strlen(t)],"ICC_v0_3_name:%s\n", pname );
      free(pname); pname = NULL;
    }
    if(atom_name) { free(atom_name); atom_name = NULL; }
    if(icc) free(icc);
#endif

    *debug_info = t;
    t = NULL;
  }
  if(atom_name) { free(atom_name); atom_name = NULL; }


  if( !prop )
  /* as a last means try Xorg.log for at least some informations */
  {
    char * log_text = 0;
    int screen = oyX1Monitor_screen_( disp ), i;
    char * log_file;
   
    oyX1Alloc(log_file, 256,)

    if(log_file)
    {
      size_t size = 0;

#define X_LOG_PATH  "/var/log/"
      sprintf( log_file, X_LOG_PATH "Xorg.%d.log", oyX1Monitor_number_(disp) );

      log_text = oyX1OpenFile( log_file, &size );

      if(!log_text)
      {
        free(log_file);
        error = 1;
        goto cleanInfoMem;
      } else
        log_text[size] = '\000';
    }

    if(log_text)
    {
      float rx=0,ry=0,gx=0,gy=0,bx=0,by=0,wx=0,wy=0,g=0;
      int year_ = 0, week_ = 0;
      const char * t;
      char mnft_[80] = {0};
      unsigned int model_id_ = 0;

      char * save_locale = 0;
      /* sensible parsing */
      save_locale = strdup( setlocale( LC_NUMERIC, 0 ) );
      setlocale( LC_NUMERIC, "C" );

      t = strstr( log_text, "Connected Display" );
      if(!t) t = log_text;
      if(port)
        t = strstr( t, port );
      if(!t)
      {
        t = log_text;
        if(t && t[0])
        for(i = 0; i < screen; ++i)
        {
          ++t;
          t = strstr( t, "redX:" );
          if(!t) break;
        }
      }

      if(t && (t = strstr( t, "Manufacturer:" )) != 0)
      {
        sscanf( t,"Manufacturer: %s", mnft_ );
      }

      if(t && (t = strstr( t, "Model:" )) != 0)
      {
        sscanf( t,"Model: %x ", &model_id_ );
      }

      if(t && (t = strstr( t, "Year:" )) != 0)
      {
        sscanf( t,"Year: %d  Week: %d", &year_, &week_ );
      } 

      if(t && (t = strstr( t, "Gamma:" )) != 0)
        sscanf( t,"Gamma: %g", &g );
      if(t && (t = strstr( t, "redX:" )) != 0)
        sscanf( t,"redX: %g redY: %g", &rx,&ry );
      if(t && (t = strstr( t, "greenX:" )) != 0)
        sscanf( t,"greenX: %g greenY: %g", &gx,&gy );
      if(t && (t = strstr( t, "blueX:" )) != 0)
        sscanf( t,"blueX: %g blueY: %g", &bx,&by );
      if(t && (t = strstr( t, "whiteX:" )) != 0)
        sscanf( t,"whiteX: %g whiteY: %g", &wx,&wy );

      if(mnft_[0])
      {
        *mnft = strdup( mnft_ );
        *model_id = model_id_;
        colors[0] = rx;
        colors[1] = ry;
        colors[2] = gx;
        colors[3] = gy;
        colors[4] = bx;
        colors[5] = by;
        colors[6] = wx;
        colors[7] = wy;
        colors[8] = g;
        *year = year_;
        *week = week_;
        fprintf( stderr, OY_DBG_FORMAT_ "found %s in \"%s\": %s %d %s\n",
                         OY_DBG_ARGS_,
                   log_file, display_name, mnft_, model_id_,
                   display_geometry?noE(*display_geometry):geo);
      }

      setlocale(LC_NUMERIC, save_locale);
      if(save_locale)
        free( save_locale );
      free(log_text);
    }
    free(log_file);
  }

cleanInfoMem:
  if(edid)
  {
    *edid = prop;
    *edid_size = prop_size;
    prop = 0;
  }

  if(geo) free(geo);
  if( port && !system_port ) free( port );

  if(prop || (edid && *edid))
  {
    if(prop) free( prop );
    return error;
  } else {
    const char * log = "Can not read hardware information from device.";
    int r = -1;

    if(*mnft && (*mnft)[0])
    {
      log = "using Xorg log fallback.";
      r = 0;
    }

    fprintf( stderr, OY_DBG_FORMAT_ "\n  %s:\n  %s\n  %s\n", OY_DBG_ARGS_,
               "no EDID available from X properties",
               "\"XFree86_DDC_EDID1_RAWDATA\"/\"EDID_DATA\"",
               noE(log));
    return r;
  }
}



char *       oyX1GetMonitorProfile   ( oyMonitor_s       * disp,
                                       int                 flags,
                                       size_t            * size )
{
  const char * device_name = oyX1Monitor_name_(disp);
  char * moni_profile = NULL;
  oyX11INFO_SOURCE_e source = 0;
  char * atom_name = NULL;

  char * prop = 0;
  size_t prop_size = 0;

  if(device_name)
    if(oy_debug) fprintf( stderr,OY_DBG_FORMAT_ "device_name %s\n",OY_DBG_ARGS_,device_name);

#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
  /* support the color server device profile */
  if(flags & 0x01)
    prop = oyX1Monitor_getProperty_( disp,
                             XCM_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE, 0, &prop_size,
                                     &source, &atom_name );
  if(atom_name) { free( atom_name ); atom_name = NULL; }
#endif

  /* alternatively fall back to the non color server or pre v0.4 atom */
  if(!prop)
#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
    prop = oyX1Monitor_getProperty_( disp, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE, 0, &prop_size,
                                     &source, &atom_name );
#else
    prop = oyX1Monitor_getProperty_( disp, "_ICC_PROFILE", 0, &prop_size,
                                     &source, &atom_name );
#endif

  if(prop)
  {
    moni_profile = prop;
    if(moni_profile)
    {
      *size = prop_size;
    }
  } /*else
    fprintf( stderr,"\n  %s",
         _("Could not get Xatom, probably your monitor profile is not set:"));*/
  if(atom_name) { free( atom_name ); atom_name = NULL; }

  return moni_profile;
}


int      oyX1GetAllScreenNames       ( const char        * display_name,
                                       char            *** display_names )
{
  int i = 0;

  *display_names = oyX1GetAllScreenNames_( display_name, &i );

  return i; 
}

char**
oyX1GetAllScreenNames_          (const char *display_name,
                                 int *n_scr)
{
  int i = 0;
  char** list = 0;

  Display *display = 0;
  int len = 0;
  oyMonitor_s * disp = 0;

  *n_scr = 0;

  if(!display_name || !display_name[0])
  {
    fprintf( stderr, OY_DBG_FORMAT_ "No display_name\n", OY_DBG_ARGS_ );
    return 0;
  }

  disp = oyX1Monitor_newFrom_( display_name, 0 );
  if(!disp)
  {
#ifdef __APPLE__
    if(oy_debug)
#endif
    fprintf( stderr, OY_DBG_FORMAT_ "No disp object\n", OY_DBG_ARGS_ );
    return 0;
  }

  display = oyX1Monitor_device_( disp );

  if( !display || (len = ScreenCount( display )) == 0 )
  {
    if(!display)
      fprintf( stderr, OY_DBG_FORMAT_ "No display struct\n", OY_DBG_ARGS_ );
    else
      fprintf( stderr, OY_DBG_FORMAT_ "No ScreenCount %d\n", OY_DBG_ARGS_, len );
    return 0;
  }

# if defined(HAVE_XRANDR)
  if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR)
    len = disp->active_outputs;
# endif

# if defined(HAVE_XINERAMA)
  /* test for Xinerama screens */
  if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XINERAMA)
    {
      int n_scr_info = 0;
      XineramaScreenInfo *scr_info = XineramaQueryScreens( display, &n_scr_info );
      if(!scr_info || !n_scr_info) return 0;

      if( n_scr_info >= 1 )
        len = n_scr_info;

      XFree( scr_info );
    }
# endif

  oyX1Alloc( list, sizeof(char*) * len, return NULL; )

  for (i = 0; i < len; ++i)
    if( (list[i] = oyX1ChangeScreenName_( display_name, i )) == 0 )
    {
      fprintf( stderr, OY_DBG_FORMAT_ "oyChangeScreenName_failed %s %d\n", OY_DBG_ARGS_,
               noE(display_name), i );
      free( list );
      return NULL;
    }

  *n_scr = len;
  oyX1Monitor_release_( &disp );

  if(oy_debug || !len) fprintf( stderr, OY_DBG_FORMAT_ "display_name = %s  count = %d\n", OY_DBG_ARGS_, oyNoEmptyString_m_(display_name), len );

  return list;
}


/** @internal
 *  Function oyX1GetRectangleFromMonitor
 *  @memberof monitor_api
 *  @brief   value filled a oyStruct_s object
 *
 *  @param         device_name         the Oyranos specific device name
 *  @return                            the rectangle the device projects to
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/07/22
 *  @since   2009/01/28 (Oyranos: 0.1.10)
 */
int      oyX1GetRectangleFromMonitor ( oyMonitor_s       * disp,
                                       double            * x,
                                       double            * y,
                                       double            * width,
                                       double            * height )
{
  *x = oyX1Monitor_x_(disp);
  *y = oyX1Monitor_y_(disp);
  *width = oyX1Monitor_width_(disp);
  *height = oyX1Monitor_height_(disp);

  return 0;
}


/** @internal
    takes a chaked display name and point as argument and
    gives a string back for search in the db
 */
int
oyX1Monitor_getGeometryIdentifier_         (oyMonitor_s  *disp)
{
  int len = 64;

  oyX1Alloc(disp->identifier, len, return 1;)

  snprintf( disp->identifier, len, "%dx%d+%d+%d", 
            oyX1Monitor_width_(disp), oyX1Monitor_height_(disp),
            oyX1Monitor_x_(disp), oyX1Monitor_y_(disp) );

  return 0;
}

char* oyX1Monitor_getScreenAtomName_ ( oyMonitor_s     * disp,
                                       const char        * base )
{
  int len = 64;
  char *screen_number = oyX1Monitor_screenIdentifier_( disp );
  char *atom_name;
 
  oyX1Alloc(atom_name, len,)

  if(!screen_number) return 0;
  if(!atom_name) { free( screen_number ); return 0; }

  snprintf( atom_name, len, "%s%s", base, screen_number );

  free( screen_number );

  return atom_name;
}

void  oyX1Monitor_setCompatibility   ( oyMonitor_s       * disp,
                                       const char        * profile_name )
{
  char * prop = 0;
  size_t prop_size = 0;
  int refresh_edid = 1;
  char * command;
  oyX11INFO_SOURCE_e source;
  char * atom_name = NULL;
 
  oyX1Alloc(command, 4096, return;)

  oyX1GetMonitorEdid( disp, &prop, &prop_size, refresh_edid, &source, &atom_name );
  if(atom_name) { free(atom_name); atom_name = NULL; }

  sprintf( command, "oyranos-compat-gnome -q %s -i -", profile_name?"-a":"-e" );
  if(profile_name)
    sprintf( &command[strlen(command)], " -p \"%s\"", profile_name );

  if(oy_debug)
    fprintf( stderr, OY_DBG_FORMAT_ "%s\n", OY_DBG_ARGS_, command );
  if(prop && prop_size)
  {
    FILE * s = popen( command, "w" );

    if(s)
    {
      fwrite( prop, sizeof(char), prop_size, s );

      pclose(s); s = 0;
    } else
      fprintf( stderr, OY_DBG_FORMAT_ "fwrite(%s) : %s\n", OY_DBG_ARGS_, command, strerror(errno));

  }

  if(prop) free( prop );
  free( command );
}

int      oyX1SetupMonitorProfile     ( oyMonitor_s       * disp,
                                       const char        * profile_name,
                                       const char        * profile_data,
                                       size_t              profile_data_size )
{
  int error = 0;
  const char * display_name = oyX1Monitor_name_(disp);
  char *text = 0;

  if(oy_debug) fprintf( stderr, OY_DBG_FORMAT_ "profile_name = %s\n", OY_DBG_ARGS_, profile_name?profile_name:"" );

  if( profile_name && profile_name[0] )
  {
    /* set XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE atom in X */
    {
      Display *display;
      Atom atom = 0;
      int screen = 0;
      Window w;

      char       *atom_name=0;
      int         result = 0;

      if(display_name)
        if(oy_debug) fprintf( stderr,OY_DBG_FORMAT_ "display_name %s\n", OY_DBG_ARGS_,display_name);

      display = oyX1Monitor_device_( disp );

      screen = oyX1Monitor_deviceScreen_( disp );
      if(oy_debug) fprintf( stderr, OY_DBG_FORMAT_ "screen: %d\n", OY_DBG_ARGS_, screen);
      w = RootWindow(display, screen); if(oy_debug) fprintf( stderr,OY_DBG_FORMAT_ "w: %ld\n", OY_DBG_ARGS_, w);

      if(!profile_data_size || !profile_data)
        fprintf( stderr,OY_DBG_FORMAT_ "Error obtaining profile\n", OY_DBG_ARGS_);

#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
      atom_name = oyX1Monitor_getScreenAtomName_( disp, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE );
#else
      atom_name = oyX1Monitor_getScreenAtomName_( disp, "_ICC_PROFILE" );
#endif
      if( atom_name )
      {
        atom = XInternAtom (display, atom_name, False);
        if (atom == None) {
          fprintf( stderr,OY_DBG_FORMAT_ "%s \"%s\"\n", OY_DBG_ARGS_, "Error setting up atom", atom_name);
        }
      } else fprintf( stderr,OY_DBG_FORMAT_ "Error setting up atom\n", OY_DBG_ARGS_);

      if( atom && profile_data)
      result = XChangeProperty( display, w, atom, XA_CARDINAL,
                       8, PropModeReplace, (unsigned char*)profile_data, (int)profile_data_size );
      if(result == 0) fprintf( stderr,OY_DBG_FORMAT_ "%s %d\n", OY_DBG_ARGS_, "found issues",result);

# if defined(HAVE_XRANDR)
      if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR )
      {
#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
        atom = XInternAtom( display, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE, True );
#else   
        atom = XInternAtom( display, "_ICC_PROFILE", True );
#endif

        if(atom)
        {
          XRRChangeOutputProperty( display, oyX1Monitor_xrrOutput_( disp ),
			           atom, XA_CARDINAL, 8, PropModeReplace,
			           (unsigned char *)profile_data, (int)profile_data_size );

          if(oy_debug)
            atom_name = XGetAtomName(display, atom);
          if(oy_debug) fprintf( stderr, OY_DBG_FORMAT_ "output: \"%s\" crtc: %d atom_name: %s\n", OY_DBG_ARGS_,
               noE(oyX1Monitor_xrrOutputInfo_(disp)->name),
               (int)oyX1Monitor_xrrOutputInfo_(disp)->crtc, atom_name );
        }
      }
#else
      if(oy_debug) fprintf( stderr,OY_DBG_FORMAT_ "!HAVE_XRANDR\n", OY_DBG_ARGS_);
# endif

      /* claim to be compatible with 0.4 
       * http://www.freedesktop.org/wiki/OpenIcc/ICC_Profiles_in_X_Specification_0.4
       */
      atom = XInternAtom( display, "_ICC_PROFILE_IN_X_VERSION", False );
      if(atom)
      {
        Atom a;
        /* 0.4 == 100*0 + 1*4 = 4 */
        const unsigned char * value = (const unsigned char*)"4";
        int actual_format_return;
        unsigned long nitems_return=0, bytes_after_return=0;
        unsigned char* prop_return=0;

        XGetWindowProperty( display, w, atom, 0, INT_MAX, False, XA_STRING,
                     &a, &actual_format_return, &nitems_return, 
                     &bytes_after_return, &prop_return );
        if(bytes_after_return != 0) fprintf( stderr,OY_DBG_FORMAT_ "%s bytes_after_return: %lu\n", OY_DBG_ARGS_,
                                          "found issues",bytes_after_return);
        /* check if the old value is the same as our intented */
        if(actual_format_return != XA_STRING ||
           nitems_return == 0)
        {
          if(!prop_return || strcmp( (char*)prop_return, (char*)value ) != 0)
          result = XChangeProperty( display, w, atom, XA_STRING,
                                    8, PropModeReplace,
                                    value, 4 );
          if(result == 0) fprintf( stderr,OY_DBG_FORMAT_ "%s %d\n", OY_DBG_ARGS_, "found issues",result);
        }
      }

      free( atom_name );

      oyX1Monitor_setCompatibility( disp, profile_name );

      XFlush( display );
    }

    free( text );
  }

  return error;
}

int           oyX1FreeMonitor        ( oyMonitor_s      ** disp )
{
  return oyX1Monitor_release_( disp );
}

oyMonitor_s * oyX1GetMonitor         ( const char        * display_name )
{
  return oyX1Monitor_newFrom_( display_name, 1 );
}

int      oyX1SetupMonitorCalibration ( oyMonitor_s       * display,
                                       const char        * profile_name,
                                       const char        * profile_data OY_UNUSED,
                                       size_t              profile_data_size OY_UNUSED )
{
  int status = 0x00, error = 0;
  oyMonitor_s * disp = display;
  const char * display_name = oyX1Monitor_name_(disp);
  char       *dpy_name = NULL;
  char *text = 0;
  char *clear = 0;

  dpy_name = calloc( sizeof(char), MAX_PATH );
  if(!dpy_name)
  { fprintf(stderr, OY_DBG_FORMAT_ "ERROR: calloc() failed", OY_DBG_ARGS_);
    goto Clean;
  }
  if( display_name && !strstr( disp->host, display_name ) )
    snprintf( dpy_name, MAX_PATH, ":%d", disp->geo[0] );
  else
    snprintf( dpy_name, MAX_PATH, "%s:%d", disp->host, disp->geo[0] );

  if(oy_debug) fprintf( stderr, OY_DBG_FORMAT_ "profile_name = %s\n", OY_DBG_ARGS_, profile_name?profile_name:"" );

  int version = 0; char * t = NULL;
  const char * xs = "-s";
  FILE * fp = popen("xcalib -version", "r");
  if(fp)
  {
    int n;
    oyX1Alloc( t, 48, pclose(fp); free(dpy_name); return 1;)
    n = fread( t, sizeof(char), 48, fp );
    if(0 <= n && n < 48) t[n] = '\000';
    else t[47] = '\000';
    pclose(fp);
  } else
  {
    fprintf( stderr, OY_DBG_FORMAT_ "xcalib not found for setting with %s\n", OY_DBG_ARGS_, profile_name );
    status |= OY_CALIB_ERROR;
  }
  if(t && strstr(t, "xcalib "))
  {
    int major = -1, minor = -1, micro = 0;
    char * tmp = strstr(t, "xcalib ");

    tmp = &tmp[7];
    sscanf( tmp, "%d.%d.%d", &major, &minor, &micro );
    version = major*10000 + minor * 100 + micro;

    if(version < 0 || oy_debug)
      fprintf( stderr, OY_DBG_FORMAT_ "xcalib version string: \"%s\" \"%s\" version: %d\n", OY_DBG_ARGS_, t, tmp, version );
  }
  if(t) { free(t); t = NULL; }

  if(version >= 1000)
    xs = "-o";

  oyX1Alloc(text, MAX_PATH, goto Clean;)
  oyX1Alloc(clear, MAX_PATH, goto Clean;)

  sprintf(clear,"xcalib -d %s %s %d %s -c", dpy_name, xs, disp->geo[1],
               oy_debug?"-v":"");
  if(oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR)
  {
#if defined(HAVE_XRANDR)
    sprintf(clear,"xcalib -d %s %s %d %s -c", dpy_name, xs, oyX1Monitor_rrScreen_(disp),
            oy_debug?"-v":"");
#else
    sprintf(clear,"xcalib -d %s %s %d %s -c", dpy_name, xs, 0,
            oy_debug?"-v":"");
#endif
  }
  else
  {
    sprintf(clear,"xcalib -d %s %s %d %s -c", dpy_name, xs, disp->geo[1],
            oy_debug?"-v":"");
  }

  if( profile_name && profile_name[0] )
  {

    /** set vcgt tag with xcalib
       not useable with multihead Xinerama at one screen

       @todo TODO xcalib should be configurable as a module
     */
    sprintf(text,"xcalib -d %s %s %d %s \'%s\'", dpy_name, xs, disp->geo[1],
                 oy_debug?"-v":"", profile_name);
    if(oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR)
    {
#if defined(HAVE_XRANDR)
      sprintf(text,"xcalib -d %s %s %d %s \'%s\'", dpy_name, xs, oyX1Monitor_rrScreen_(disp),
              oy_debug?"-v":"", profile_name);
#else
      sprintf(text,"xcalib -d %s %s %d %s \'%s\'", dpy_name, xs, 0,
              oy_debug?"-v":"", profile_name);
#endif
    }
    else
    {
      sprintf(text,"xcalib -d %s %s %d %s \'%s\'", dpy_name, xs, disp->geo[1],
              oy_debug?"-v":"", profile_name);
    }

    {
      Display * display = oyX1Monitor_device_( disp );
      int effective_screen = oyX1Monitor_screen_( disp );
      int screen = oyX1Monitor_deviceScreen_( disp );
      int size = 0,
          can_gamma = 0;

      if(!display)
      {
        fprintf( stderr,OY_DBG_FORMAT_ "ERROR: %s %s %s\n", OY_DBG_ARGS_, "open X Display failed", dpy_name, display_name);
        goto Clean;
      }

#ifdef HAVE_XXF86VM
      if(effective_screen == screen)
      {
        XF86VidModeGamma gamma;
        /* check for gamma capabiliteis on the given screen */
        if (XF86VidModeGetGamma(display, effective_screen, &gamma))
          can_gamma = 1;
        else
        if (XF86VidModeGetGammaRampSize(display, effective_screen, &size))
        {
          if(size)
            can_gamma = 1;
        }
      }
#endif

      /* Check for incapabilities of X gamma table access */
      if(can_gamma || oyX1Monitor_screen_( disp ) == 0 || oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR )
      {
      /* OS X handles VGCT fine, no need for xcalib */
#if !defined(__APPLE__)
        FILE * fp = popen(text, "r");
        t = NULL;
        if(fp)
        {
          int n;
          oyX1Alloc( t, 48, pclose(fp); free(dpy_name); free(text); free(clear); return 1;)
          n = fread( t, sizeof(char), 48, fp );
          if(0 <= n && n < 48) t[n] = '\000';
          else t[47] = '\000';
          pclose(fp);
        }
        else
        {
          fprintf( stderr,OY_DBG_FORMAT_ "ERROR: %s: %s\n", OY_DBG_ARGS_, "calling failed", text);
          goto Clean;
        }

        if(strstr(t,"No calibration data in ICC profile"))
        {
          fprintf( stderr,OY_DBG_FORMAT_ "%s %s %d\n", OY_DBG_ARGS_, "No monitor gamma curves by profile:",
                   noE(profile_name), error );
          status |= OY_CALIB_VCGT_NOT_CONTAINED;
        }
        if(strstr(t, "Unable to read file"))
        {
          fprintf( stderr,OY_DBG_FORMAT_ "%s %s %d\n", OY_DBG_ARGS_, "Not found profile:",
                   noE(profile_name), error );
          status |= OY_CALIB_VCGT_NOT_CONTAINED;
        }
        printf("%s : %s\n", text, t);
        if(status & OY_CALIB_VCGT_NOT_CONTAINED)
        {
#ifdef PRINT_TIME
          PRINT_TIME;
#endif
          fprintf( stderr,OY_DBG_FORMAT_ "Clearing VCGT\n", OY_DBG_ARGS_ );
          error = system(clear); // causes flicker, but profile without VCGT tag will not change any curves.
        }
        if(t) { free(t); t = NULL; }
#endif
      }
      if(!can_gamma)
        status |= OY_CALIB_DEVICE_NOT_SUPPORTED;
      if(error &&
         error != 65280)
      { /* hack */
        fprintf( stderr,OY_DBG_FORMAT_ "%s %s %d\n", OY_DBG_ARGS_, "found issue",
                 noE(profile_name), error );
      } else
      {
        /* take xcalib error not serious, turn into a issue */
        if(oy_debug) fprintf( stderr,OY_DBG_FORMAT_ "xcalib returned %d\n", OY_DBG_ARGS_, error);
        error = -1;
      }
    }

    if(oy_debug) fprintf( stderr, OY_DBG_FORMAT_ "system: %s\n", OY_DBG_ARGS_, clear );
    if(oy_debug) fprintf( stderr, OY_DBG_FORMAT_ "system: %s\n", OY_DBG_ARGS_, text );
  }
  else
  {
#ifdef PRINT_TIME
        PRINT_TIME;
#endif
        fprintf( stderr, OY_DBG_FORMAT_ "Clearing VCGT\n", OY_DBG_ARGS_ );
        error = system(clear); // causes flicker, but profile without VCGT tag will not change any curves.
  }

  Clean:
  if(dpy_name) free( dpy_name );
  if(text) free( text );
  if(clear) free( clear );

  return status;
}


int      oyX1UnsetMonitorProfile     ( oyMonitor_s       * disp )
{
  const char * display_name = oyX1Monitor_name_(disp);
  int error = 0;
  {
    /* unset the XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE atom in X */
      Display *display;
      Atom atom;
      int screen = 0;
      Window w;
      char *atom_name = 0;
      char * command = 0;

      if(display_name && oy_debug)
        if(oy_debug) fprintf( stderr,OY_DBG_FORMAT_ "display_name %s\n", OY_DBG_ARGS_,display_name);

      display = oyX1Monitor_device_( disp );

      screen = oyX1Monitor_deviceScreen_( disp );
      if(oy_debug) fprintf( stderr, OY_DBG_FORMAT_ "screen: %d\n", OY_DBG_ARGS_, screen);
      w = RootWindow(display, screen); if(oy_debug) fprintf( stderr,"w: %ld\n", w);


#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
      atom_name = oyX1Monitor_getScreenAtomName_( disp, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE );
#else
      atom_name = oyX1Monitor_getScreenAtomName_( disp, "_ICC_PROFILE" );
#endif
      atom = XInternAtom (display, atom_name, True);
      if (atom != None)
        XDeleteProperty( display, w, atom );
      else
      {
        fprintf( stderr,OY_DBG_FORMAT_ "%s \"%s\"\n", OY_DBG_ARGS_, "Error getting atom", atom_name);
        error = -1;
      }
# if defined(HAVE_XRANDR)
      if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR )
      {
#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
        atom = XInternAtom( display, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE, True );
#else   
        atom = XInternAtom( display, "_ICC_PROFILE", True );
#endif
        if(atom != None)
        {
          /* need a existing property to remove; sorry for the noice */
          XRRChangeOutputProperty( display, oyX1Monitor_xrrOutput_( disp ),
			           atom, XA_CARDINAL, 8, PropModeReplace,
			           (unsigned char *)NULL, (int)0 );
          XRRDeleteOutputProperty( display, oyX1Monitor_xrrOutput_( disp ), atom );
          if(oy_debug)
            atom_name = XGetAtomName(display, atom);
          if(oy_debug) fprintf( stderr, OY_DBG_FORMAT_ "output: \"%s\" crtc: %d atom_name: %s\n", OY_DBG_ARGS_,
               noE(oyX1Monitor_xrrOutputInfo_(disp)->name),
               (int)oyX1Monitor_xrrOutputInfo_(disp)->crtc, atom_name );
        }
      }
#else
      if(oy_debug) fprintf(stderr,OY_DBG_FORMAT_ "!HAVE_XRANDR\n", OY_DBG_ARGS_);
# endif

      {
        char *dpy_name = strdup( noE(display_name) );
        char *ptr = NULL;
        int r = 0;

        if(!dpy_name) goto finish;
        oyX1Alloc(command, 1048, free(dpy_name); goto finish;)

        if( (ptr = strchr(dpy_name,':')) != 0 )
          if( (ptr = strchr(ptr,'.')) != 0 )
            ptr[0] = '\000';

        if(oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR)
          snprintf(command, 1024, "xrandr -display %s --output %s --gamma .999999:.999999:.999999",
                 dpy_name, oyX1Monitor_systemPort_(disp));
        else
          snprintf(command, 1024, "xgamma -gamma 1.0 -screen %d -display %s",
                 disp->geo[1], dpy_name);

        if(oy_debug)
          if(oy_debug) fprintf( stderr, OY_DBG_FORMAT_ "%d %d system: %s\n", OY_DBG_ARGS_, screen, disp->geo[1], command );
        if(screen == disp->geo[1] || oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR)
          r = system( command );
        if(r) fprintf( stderr,OY_DBG_FORMAT_ "%s %d\n", OY_DBG_ARGS_, "found issues",r);

        free( dpy_name );
        free( command );
      }

      oyX1Monitor_setCompatibility( disp, NULL );

      XFlush( display );

      free( atom_name );

      goto finish;
  }


  finish:

  return error;
}

int
oyGetDisplayNumber_        (oyMonitor_s *disp)
{
  int dpy_nummer = 0;
  const char *display_name = oyX1Monitor_name_(disp);


  if( display_name )
  {
    char ds[8];             /* display.screen*/
    const char *txt = strchr( display_name, ':' );
    int l;
    
    memset( ds, 0, 8 );

    if( !txt )
    { fprintf( stderr, OY_DBG_FORMAT_ "invalid display name: %s\n", OY_DBG_ARGS_, display_name );
      return -1;
    }

    if(txt[0])
      ++txt;
    l = strlen(txt) > 8 ? 8 : strlen(txt);
    memcpy( ds, txt, l );
    ds[7] = '\000';
    if( strchr( ds, '.' ) )
    {
      char *end = strchr( ds, '.' );
      if( end )
        *end = 0;
    }
    dpy_nummer = atoi( ds );
  }

  return dpy_nummer;
}

int   oyX1Monitor_getScreenFromDisplayName_( oyMonitor_s     * disp )
{
  int scr_nummer = 0;
  const char *display_name = oyX1Monitor_name_(disp);

  if( display_name )
  {
    char ds[8];             /* display.screen*/
    const char *txt = strchr( display_name, ':' );
    
    memset( ds, 0, 8 );

    if( !txt )
    { fprintf( stderr, OY_DBG_FORMAT_ "invalid display name: %s\n", OY_DBG_ARGS_, display_name );
      return -1;
    }

    memcpy( ds, txt, strlen(txt) > 8 ? 8 : strlen(txt) );
    ds[7] = '\000';
    if( strchr( display_name, '.' ) )
    {
      char *nummer_text = strchr( ds, '.' );
      if( nummer_text )
        scr_nummer = atoi( &nummer_text[1] );
    }
  }

  return scr_nummer;
}

char * oyX1Hostname()
{
  char * t = getenv ("HOSTNAME");
  FILE * fp;
  if(!t) t = getenv("HOST");
  if(!t) t = getenv("XAUTHLOCALHOSTNAME");

  if(t)
    t = strdup(t);
  else
  {
    fp = popen("uname -n", "r");
    if(fp)
    {
      oyX1Alloc( t, 48, pclose(fp); return strdup("dummy");)
      if( fread( t, sizeof(char), 48, fp ) > 0 )
      {
        t[47] = '\000';
        pclose(fp);
        return t;
      }
      else if(t) free(t);
      pclose(fp);
    }
    t = strdup("dummy");
  }

  return t;
}

/**  @internal
 *  extract the host name or get from environment
 */
char*
oyExtractHostName_           (const char* display_name)
{
  char* host_name = NULL;

  /* Is this X server identifyable? */
  if(!display_name)
  {
    host_name = oyX1Hostname();
  } else if (strchr(display_name,':') == display_name ||
             !strchr( display_name, ':' ) )
  {
    host_name = oyX1Hostname();
  } else if ( strchr( display_name, ':' ) )
  {
    char* ptr = 0;
    oyX1Alloc(host_name, strlen( display_name ) + 48, return NULL;)
    strcpy( host_name, display_name );
    ptr = strchr( host_name, ':' );
    if(ptr) ptr[0] = 0;
  } else
    host_name = strdup( "" );

  if(oy_debug || !host_name) fprintf( stderr, OY_DBG_FORMAT_ "host_name = %s  display_name = %s\n", OY_DBG_ARGS_, oyNoEmptyString_m_(host_name), oyNoEmptyString_m_(display_name) );

  return host_name;
}

/** @internal Do a full check and change the screen name,
 *  if the screen arg is appropriate. Dont care about the host part
 */
char * oyX1ChangeScreenName_         ( const char        * display_name,
                                       int                 screen )
{
  char* host_name;

  /* Is this X server identifyable? */
  if(!display_name)
    display_name = ":0.0";


  oyX1Alloc(host_name, strlen( display_name ) + 48, return NULL;)

  strcpy( host_name, display_name );

  /* add screen */
  {
    const char *txt = strchr( host_name, ':' );

    /* fail if no display was given */
    if( !txt )
    { fprintf( stderr, OY_DBG_FORMAT_ "invalid display name: %s\n", OY_DBG_ARGS_, display_name );
      host_name[0] = 0;
      return host_name;
    }

    if( !strchr( txt, '.' ) )
    {
      sprintf( &host_name[ strlen(host_name) ], ".%d", screen );
    } else
    if( screen >= 0 )
    {
      char *txt_scr = strchr( txt, '.' );
      sprintf( txt_scr, ".%d", screen );
    }
  }

  if(oy_debug) fprintf( stderr, OY_DBG_FORMAT_ "host_name = %s  display_name = %s screen = %d\n", OY_DBG_ARGS_, oyNoEmptyString_m_(host_name), oyNoEmptyString_m_(display_name), screen );

  return host_name;
}

/** @internal get the geometry of a screen 
 *
 *  @param          disp      display info structure
 *  @return         error
 */
int
oyX1Monitor_getScreenGeometry_            (oyMonitor_s *disp)
{
  int error = 0;
  int screen = 0;


  disp->geo[0] = oyGetDisplayNumber_( disp );
  disp->geo[1] = screen = oyX1Monitor_getScreenFromDisplayName_( disp );

  if(screen < 0)
    return screen;

# if defined(HAVE_XRANDR)
  if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR )
  {
    XRRCrtcInfo * crtc_info = 0;

    crtc_info = XRRGetCrtcInfo( disp->display, disp->res,
                                disp->output_info->crtc );
    if(crtc_info)
    {
      disp->geo[2] = crtc_info->x;
      disp->geo[3] = crtc_info->y;
      disp->geo[4] = crtc_info->width;
      disp->geo[5] = crtc_info->height;

      XRRFreeCrtcInfo( crtc_info );
    } else
    {
      fprintf( stderr, OY_DBG_FORMAT_ "%s output: \"%s\" crtc: %d\n", OY_DBG_ARGS_,
               "XRandR CrtcInfo request failed", 
               noE(disp->output_info->name),
               (int)disp->output_info->crtc);
    }
  }
# endif /* HAVE_XRANDR */

# if defined(HAVE_XINERAMA)
  if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XINERAMA )
  {
    int n_scr_info = 0;

    XineramaScreenInfo *scr_info = XineramaQueryScreens( disp->display,
                                                         &n_scr_info );
    if( !scr_info || !n_scr_info )
    {
      fprintf( stderr,OY_DBG_FORMAT_ "Xinerama request failed\n", OY_DBG_ARGS_);
      return 1;
    }
    if( n_scr_info <= screen )
    {
      fprintf( stderr,OY_DBG_FORMAT_ "Xinerama request failed\n", OY_DBG_ARGS_);
      return -1;
    }
    {
        disp->geo[2] = scr_info[screen].x_org;
        disp->geo[3] = scr_info[screen].y_org;
        disp->geo[4] = scr_info[screen].width;
        disp->geo[5] = scr_info[screen].height;
    }
    XFree( scr_info );
  }
# endif /* HAVE_XINERAMA */

  if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_SCREEN )
  {
    Screen *scr = XScreenOfDisplay( disp->display, screen );
    if(scr == NULL)
    {
      fprintf( stderr,OY_DBG_FORMAT_ "open X Screen failed\n", OY_DBG_ARGS_);
      error = 1;

    } else
    {
        disp->geo[2] = 0;
        disp->geo[3] = 0;
        disp->geo[4] = XWidthOfScreen( scr );
        disp->geo[5] = XHeightOfScreen( scr );
        disp->screen = screen;
    }
  }

  return error;
}

Display *    oyX1OpenDisplay         ( const char        * display_name,
                                       char             ** changed_name )
{
  Display * display = NULL;;
  char * name = NULL;

  if(display_name && 
     (isdigit(display_name[0]) || strchr(display_name, ':') == NULL))
  {
    fprintf(stderr, OY_DBG_FORMAT_ "ERROR: give up display_name: %s\n", OY_DBG_ARGS_, oyNoEmptyString_m_(display_name) );
    return display;
  }
	  
  if( display_name )
  {
    if( display_name[0] )
      name = strdup( display_name );
  } else
  {
    const char * dpyn = getenv("DISPLAY");
    if(dpyn && dpyn[0])
      name = strdup( dpyn );
    else
      name = strdup( ":0" );
  }

  display = XOpenDisplay (name);

  /* switch to Xinerama mode */
  if( !display )
  {
    char *text = oyX1ChangeScreenName_( name, 0 );
    if(!text) return display;

    display = XOpenDisplay( text );

    if( !display )
    {
      fprintf( stderr,OY_DBG_FORMAT_ "ERROR: %s: %s %s %s\n", OY_DBG_ARGS_,
                            "open X Display failed",
                            noE(display_name),
                            noE(name),
                            noE(text));
      free( text );
      return display;
    }
    free( text );
  }

  if(oy_debug || !name) fprintf( stderr, OY_DBG_FORMAT_ "name = %s  display_name = %s\n", OY_DBG_ARGS_, oyNoEmptyString_m_(name), oyNoEmptyString_m_(display_name) );

  if(changed_name)
    *changed_name = name;
  else if(name)
    free(name);

  return display;
}

/** @internal
 *  @brief create a monitor information struct for a given display name
 *
 *  @param   display_name              Oyranos display name
 *  @param   expensive                 probe XRandR even if it causes flickering
 *  @return                            a monitor information structure
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/17 (Oyranos: 0.1.8)
 *  @date    2009/01/13
 */
oyMonitor_s* oyX1Monitor_newFrom_      ( const char        * display_name,
                                       int                 expensive )
{
  int error = 0, t_err = 0;
  oyMonitor_s * disp = NULL;
  Display * display;
  int len = 0, i;
  int monitors = 0;

  oyX1Alloc( disp, sizeof(oyMonitor_s), return disp; );

  t_err = !memset( disp, 0, sizeof(oyMonitor_s) );
  if(t_err) error = t_err;

  disp->type_ = oyOBJECT_MONITOR_S;

  disp->display = display = oyX1OpenDisplay( display_name, &disp->name );

  if( error <= 0 &&
      (disp->host = oyExtractHostName_( disp->name )) == 0 )
    error = 1;

  disp->screen = 0;

  if( !display || (len = ScreenCount( display )) <= 0 )
  {
    fprintf( stderr,OY_DBG_FORMAT_ "ERROR: %s: \"%s\"\n", OY_DBG_ARGS_, "no Screen found",
                           noE(disp->name));
    goto dispFailed;
  }

  disp->info_source = oyX11INFO_SOURCE_SCREEN;

  if(len == 1)
  {
# if defined(HAVE_XRANDR)
    int major_versionp = 0;
    int minor_versionp = 0;
    int i, n = 0;
    XRRQueryVersion( display, &major_versionp, &minor_versionp );

    if(oy_debug) fprintf( stderr, OY_DBG_FORMAT_ "XRR version: %d.%d\n", OY_DBG_ARGS_, major_versionp, minor_versionp );
    if((major_versionp*100 + minor_versionp) >= 102)
    {
      /* too expensive
      Time xrr_config_time0 = 0,
           xrr_config_time = XRRTimes( display, oyX1Monitor_screen_(disp),
                                       &xrr_config_time0 );

      // expensive too: XRRConfigTimes()
      */
    }

    if((major_versionp*100 + minor_versionp) >= 102)
    {
      Window w = RootWindow(display, oyX1Monitor_screen_(disp));
      XRRScreenResources * res = 0;
      int selected_screen = oyX1Monitor_getScreenFromDisplayName_( disp );
      int xrand_screen = -1;
      int geo[4] = {-1,-1,-1,-1};
      int geo_monitors = 0;

      if(oy_debug) fprintf( stderr, OY_DBG_FORMAT_ "selected_screen: %d\n", OY_DBG_ARGS_, selected_screen );
      if(selected_screen < 0)
        goto dispFailed;

# if defined(HAVE_XINERAMA)
      /* sync numbering with Xinerama screens */
      if( XineramaIsActive( display ) )
      {
        int n_scr_info = 0;
        XineramaScreenInfo *scr_info = XineramaQueryScreens( display,
                                                             &n_scr_info );
        const char * dn = oyX1Monitor_name_(disp);
        if(oy_debug) fprintf( stderr, OY_DBG_FORMAT_ "Xinerama: n_scr_info: %d selected_screen: %d %s\n", OY_DBG_ARGS_, n_scr_info, selected_screen, dn?dn:"----" );
        if(n_scr_info <= selected_screen)
        {
          XFree( scr_info );
          fprintf( stderr, OY_DBG_FORMAT_ "ERROR: Xinerama: n_scr_info <= selected_screen %d %d %s\n", OY_DBG_ARGS_, n_scr_info, selected_screen, dn?dn:"----" );
          oyX1Monitor_release_( &disp );
          return 0;
        }
        if(!scr_info || !n_scr_info)
        {
          fprintf( stderr, OY_DBG_FORMAT_ "ERROR: Xinerama: n_scr_info: %d  scr_info block: %s\n", OY_DBG_ARGS_, n_scr_info, scr_info?"obtained":"no" );
          goto dispFailed;
        }

        geo[0] = scr_info[selected_screen].x_org;
        geo[1] = scr_info[selected_screen].y_org;
        geo[2] = scr_info[selected_screen].width;
        geo[3] = scr_info[selected_screen].height;

        XFree( scr_info );

        if(n_scr_info > 0)
          disp->info_source = oyX11INFO_SOURCE_XINERAMA;
      }
# endif /* HAVE_XINERAMA */

      if(disp->info_source != oyX11INFO_SOURCE_XINERAMA)
        expensive = 1;


      if(expensive)
      {
        /* a havily expensive call */
        if(oy_debug) fprintf( stderr,OY_DBG_FORMAT_ "going to call XRRGetScreenResources()\n", OY_DBG_ARGS_);
        res = XRRGetScreenResources(display, w);
        if(res)
          n = res->noutput;
        if(oy_debug) fprintf( stderr,OY_DBG_FORMAT_ "end of call XRRGetScreenResources() n = %d\n", OY_DBG_ARGS_, n);
      }
      for(i = 0; i < n; ++i)
      {
        XRRScreenResources * res_temp = res ? res : disp->res;
        XRROutputInfo * output_info = XRRGetOutputInfo( display, res_temp,
                                                        res_temp->outputs[i]);
 
        if(oy_debug) fprintf( stderr,OY_DBG_FORMAT_ "XRR[%d]: %s %s\n", OY_DBG_ARGS_, i, output_info?oyNoEmptyString_m_(output_info->name):"no XRROutputInfo", output_info && output_info->crtc ? "connected":"");
        /* we work on connected outputs */
        if( output_info && output_info->crtc )
        {
          XRRCrtcInfo * crtc_info = 0;

          if(monitors == 0)
          {
            if(!XRRGetCrtcGammaSize( display, output_info->crtc ))
            {
              if(oy_debug) fprintf( stderr,OY_DBG_FORMAT_ "XRR: XRRGetCrtcGammaSize() failed %d %s\n", OY_DBG_ARGS_, i, oyNoEmptyString_m_(output_info->name));
              XRRFreeOutputInfo( output_info );
              break;
            }
          }

          if(XRRGetCrtcGammaSize( display, output_info->crtc ))
            disp->info_source = oyX11INFO_SOURCE_XRANDR;

          crtc_info = XRRGetCrtcInfo( disp->display, res_temp,
                                      output_info->crtc );
          if(crtc_info)
          {
            /* compare with Xinerama geometry */
            if(!(geo[0] != -1 && geo[1] != -1 && geo[2] != -1 && geo[3] != -1)||
               (geo[0] == crtc_info->x &&
                geo[1] == crtc_info->y &&
                geo[2] == (int)crtc_info->width &&
                geo[3] == (int)crtc_info->height ) )
            {
              xrand_screen = monitors;
              ++geo_monitors;
            }
            if(oy_debug) fprintf( stderr,OY_DBG_FORMAT_ "XRR[%d]: %s %dx%d+%d+%d\n", OY_DBG_ARGS_, i, oyNoEmptyString_m_(output_info->name),
                                  (int)crtc_info->width, (int)crtc_info->height, crtc_info->x, crtc_info->y);

            XRRFreeCrtcInfo( crtc_info );
          }

          if(xrand_screen == monitors &&
             oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR &&
             (geo_monitors == 1 || geo_monitors-1 == selected_screen))
          {
            if(output_info)
              disp->output_info = output_info;
            disp->output = res_temp->outputs[i];
            output_info = 0;
            if(res) /* only needed for a second geo matching monitor */
              disp->res = res;
            res = 0;
            if(disp->output_info->name && strlen(disp->output_info->name))
              disp->system_port = strdup( disp->output_info->name );
            disp->rr_version = major_versionp*100 + minor_versionp;
            disp->rr_screen = xrand_screen;
            disp->mm_width = disp->output_info->mm_width;
            disp->mm_height = disp->output_info->mm_height;
            if(oy_debug) fprintf( stderr,OY_DBG_FORMAT_ "XRR[%d]: %s %dmmx%dmm\n", OY_DBG_ARGS_, i, oyNoEmptyString_m_(disp->system_port),
                                  disp->mm_width, disp->mm_height);
          }

          ++ monitors;
        }

        if(output_info)
          XRRFreeOutputInfo( output_info );
      }

      if(res)
      { XRRFreeScreenResources(res); res = 0; }

      if(oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR)
      {
        if(monitors >= len && disp->output_info)
          disp->active_outputs = monitors;
        else
          disp->info_source = oyX11INFO_SOURCE_SCREEN;
      }
    }
# endif /* HAVE_XRANDR */

    if(oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_SCREEN)
    {
      if(oy_debug) fprintf( stderr, OY_DBG_FORMAT_ "enter oyX11INFO_SOURCE_SCREEN section\n", OY_DBG_ARGS_ );
# if defined(HAVE_XINERAMA)
      /* test for Xinerama screens */
      if( XineramaIsActive( display ) )
      {
        int n_scr_info = 0;
        XineramaScreenInfo *scr_info = XineramaQueryScreens( display,
                                                             &n_scr_info );
        if(oy_debug) fprintf( stderr, OY_DBG_FORMAT_ "Xinerama: n_scr_info: %d  scr_info block: %s\n", OY_DBG_ARGS_, n_scr_info, scr_info?"obtained":"no" );
        if(!scr_info || !n_scr_info)
          goto dispFailed;

        if( n_scr_info >= 1 )
          len = n_scr_info;

        if(n_scr_info < monitors)
        {
          fprintf( stderr, OY_DBG_FORMAT_ "%s: %d < %d\n", OY_DBG_ARGS_, "less Xinerama monitors than XRandR ones",
                    n_scr_info, monitors );
        } else
          disp->info_source = oyX11INFO_SOURCE_XINERAMA;

        XFree( scr_info );
      }
# endif /* HAVE_XINERAMA */
    }
  }

  for( i = 0; i < 6; ++i )
    disp->geo[i] = -1;

  if( error <= 0 &&
      (t_err = oyX1Monitor_getScreenGeometry_( disp )) != 0 )
    error = t_err;

  if( error <= 0 )
  {
    t_err = oyX1Monitor_getGeometryIdentifier_( disp );
    if(t_err) error = t_err;
  }

  if( !disp->system_port || !strlen( disp->system_port ) )
  if( 0 <= oyX1Monitor_screen_( disp ) && oyX1Monitor_screen_( disp ) < 10000 )
  {
    oyX1Alloc( disp->system_port, 12, );
    sprintf( disp->system_port, "%d", oyX1Monitor_screen_( disp ) );
  }

  if(oy_debug) fprintf( stderr,OY_DBG_FORMAT_ "error = %d\n", OY_DBG_ARGS_, error);
  if(error > 0)
  {
    dispFailed:
    fprintf( stderr,OY_DBG_FORMAT_ "ERROR: %s: %s %d\n", OY_DBG_ARGS_, "no oyMonitor_s created", display_name, error);
    oyX1Monitor_release_( &disp );
  }

  return disp;
}


/** @internal
 *  @brief release a monitor information stuct
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/17 (Oyranos: 0.1.8)
 *  @date    2009/04/01
 */
int          oyX1Monitor_release_      ( oyMonitor_s        ** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyMonitor_s * s = 0;
  
  if(!obj || !*obj)
    return 0;
  
  s = *obj;
  
  if( s->type_ != oyOBJECT_MONITOR_S)
  { 
    fprintf( stderr,OY_DBG_FORMAT_ "Attempt to release a non oyMonitor_s object.\n", OY_DBG_ARGS_);
    return 1;
  }
  /* ---- end of common object destructor ------- */

  if(s->name) free( s->name );
  if(s->host) free( s->host );
  if(s->identifier) free( s->identifier );
  if(s->system_port) free( s->system_port );


  s->geo[0] = s->geo[1] = -1;

  if( s->display )
  {
#  if defined(HAVE_XRANDR)
    if(s->output_info)
    { XRRFreeOutputInfo( s->output_info ); s->output_info = 0; }
    if(s->res)
    { XRRFreeScreenResources( s->res ); s->res = 0; }
#  endif
    XCloseDisplay( s->display );
    s->display=0;
  }

  free( s );
  s = 0;

  *obj = 0;

  return error; 
}


/* separate from the internal functions */


#define oyX1_help_system_specific \
      " One option \"device_name\" will select the according X display.\n" \
      " If not the module will try to get this information from \n" \
      " your \"DISPLAY\" environment variable or uses what the system\n" \
      " provides. The \"device_name\" should be identical with the one\n" \
      " returned from a \"list\" request.\n" \
      " The \"properties\" call might be a expensive one.\n" \
      " Informations are stored in the returned oyConfig_s::backend_core member."
#include "config.icc_profile.monitor.oyX1.qarz.json.h"
oyMonitorHooks3_s oyX1MonitorHooks3_ = {
  oyOBJECT_MONITOR_HOOKS3_S,
  {"oyX1"},
  10000, /* 1.0.0 */
  oyX1_help_system_specific,
  (const char*)config_icc_profile_monitor_oyX1_qarz_json,
  oyX1GetMonitor,
  oyX1FreeMonitor,
  oyX1SetupMonitorCalibration,
  oyX1SetupMonitorProfile,
  oyX1UnsetMonitorProfile,
  oyX1GetRectangleFromMonitor,
  oyX1GetMonitorProfile,
  oyX1GetAllScreenNames,
  oyX1GetMonitorInfo
};

oyMonitorHooks3_s * oyX1MonitorHooks3 = &oyX1MonitorHooks3_;

