/** @file oyranos_cmm_lraw.cpp
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2009-2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    libraw filter for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/06/14
 */

#include "oyranos_config_internal.h"

#include "oyCMMapi4_s.h"
#include "oyCMMapi4_s_.h"
#include "oyCMMapi7_s.h"
#include "oyCMMapi7_s_.h"
#include "oyCMMinfo_s_.h"
#include "oyCMMui_s_.h"
#include "oyConnectorImaging_s_.h"
#include "oyFilterNode_s_.h"         /* for oyFilterNode_TextToInfo_ */

#include "oyranos_cmm.h"
#include "oyranos_debug.h"
#include "oyranos_devices.h"
#include "oyranos_generic.h"
#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_definitions.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"

#include <cmath>
#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <libraw.h>
#include <libraw_types.h>

/* --- internal definitions --- */

#define CMM_NICK "lraw"
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}
#define OY_LIBRAW_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "file_read._" CMM_NICK "._CPU._NOACCEL"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <iconv.h>

int lrawCMMWarnFunc( int code, const oyPointer context, const char * format, ... );
oyMessage_f message = lrawCMMWarnFunc;

extern oyCMMapi4_s_   lraw_api4_image_input_libraw;
extern oyCMMapi7_s_   lraw_api7_image_input_libraw;

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


/* --- implementations --- */

/** Function lrawCMMInit
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 *  @date    2009/12/17
 */
int                lrawCMMInit       ( oyStruct_s        * filter )
{
  int error = 0;
  return error;
}



/** Function lrawIconv
 *  @brief convert between codesets
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
/*int                lrawIconv         ( const char        * input,
                                       size_t              len,
                                       char              * output,
                                       const char        * from_codeset )
{
  return oyIconv(input, len, output, from_codeset, 0);
}*/





/** Function lrawCMMWarnFunc
 *  @brief message handling
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
extern "C" {
int lrawCMMWarnFunc( int code, const oyPointer context, const char * format, ... )
{
  char* text = (char*)calloc(sizeof(char), 4096);
  va_list list;
  const char * type_name = "";
  int id = -1;
  oyStruct_s * c = (oyStruct_s*) context;

  if(c && oyOBJECT_NONE < c->type_)
  {
    type_name = oyStructTypeToText( c->type_ );
    id = oyObject_GetId( c->oy_ );
  }

  va_start( list, format);
  vsprintf( text, format, list);
  va_end  ( list );

  switch(code)
  {
    case oyMSG_WARN:
         fprintf( stderr, "WARNING"); fprintf( stderr, ": " );
         break;
    case oyMSG_ERROR:
         fprintf( stderr, "!!! ERROR"); fprintf( stderr, ": " );
         break;
  }

  fprintf( stderr, "%s[%d] ", type_name, id );

  fputs( text, stderr );
  free( text );

  return 0;
}
}

/** Function lrawCMMMessageFuncSet
 *  @brief API requirement
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
int            lrawCMMMessageFuncSet ( oyMessage_f         message_func )
{
  message = message_func;
  return 0;
}




const char * lrawWidget_GetDummy     ( const char        * func_name,
                                       uint32_t          * result )
{return 0;}
oyWIDGET_EVENT_e lrawWidget_EventDummy
                                     ( const char        * wid,
                                       oyWIDGET_EVENT_e    type )
{return (oyWIDGET_EVENT_e)0;}


oyWIDGET_EVENT_e   lrawWidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event )
{return (oyWIDGET_EVENT_e)0;}


/** Function lrawFilterNode_LibrawContextToMem
 *  @brief   implement oyCMMFilter_ContextToMem_f()
 *
 *  Serialise into a Oyranos specific ICC profile containers "Info" tag.
 *  We do not have any binary context to include.
 *  Thus oyFilterNode_TextToInfo_() is fine.
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
oyPointer  lrawFilterNode_LibrawContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  return oyFilterNode_TextToInfo_( (oyFilterNode_s_*)node, size, allocateFunc );
}


/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
const char * lrawGetText             ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return _(CMM_NICK);
    else if(type == oyNAME_NAME)
      return _("libraw input filter");
    else
      return _("libraw input filter");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return _("Kai-Uwe");
    else if(type == oyNAME_NAME)
      return _("Kai-Uwe Behrmann");
    else
      return _("Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/wiki/index.php?title=Oyranos/Download");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return _("newBSD");
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 2009 Kai-Uwe Behrmann; newBSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/bsd-license.php");
  }
  return 0;
}
const char *lraw_texts[4] = {"name","copyright","manufacturer",0};


/** @instance lraw_cmm_module
 *  @brief    lraw module infos
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
oyCMMinfo_s_ lraw_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  (char*)"0.1.8",
  lrawGetText,
  (char**)lraw_texts, /* texts; list of arguments to getText */
  OYRANOS_VERSION,

  (oyCMMapi_s*) & lraw_api4_image_input_libraw,

  {oyOBJECT_ICON_S, 0,0,0, 0,0,0, (char*)"oyranos_logo.png"},
};


/* OY_LIBRAW_REGISTRATION ----------------------------------------------------*/


oyOptions_s* lrawFilter_ImageInputRAWValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = !oyOptions_FindString( validate, "filename", 0 );

  *result = error;

  return 0;
}

// The oyVEC3, oyMAT3, oyMAT3inverse, _oyVEC3init and _oyMAT3per definitions 
// origin from lcms2' cmsmtrx.c written by Marti Maria www.littlecms.com 
// and is MIT licensed there
// Vectors
typedef struct {
  double n[3];                                                  
} oyVEC3;
 
// 3x3 Matrix                                                               
typedef struct {
  oyVEC3 v[3];                                                           
 
} oyMAT3; 

#define MATRIX_DET_TOLERANCE    0.0001
// Inverse of a matrix b = a^(-1)
int _oyMAT3inverse(const oyMAT3* a, oyMAT3* b)
{
   double det, c0, c1, c2;

   c0 =  a -> v[1].n[1]*a -> v[2].n[2] - a -> v[1].n[2]*a -> v[2].n[1];
   c1 = -a -> v[1].n[0]*a -> v[2].n[2] + a -> v[1].n[2]*a -> v[2].n[0];
   c2 =  a -> v[1].n[0]*a -> v[2].n[1] - a -> v[1].n[1]*a -> v[2].n[0];

   det = a -> v[0].n[0]*c0 + a -> v[0].n[1]*c1 + a -> v[0].n[2]*c2;

   if (fabs(det) < MATRIX_DET_TOLERANCE) return FALSE;  // singular matrix; can't invert

   b -> v[0].n[0] = c0/det;
   b -> v[0].n[1] = (a -> v[0].n[2]*a -> v[2].n[1] - a -> v[0].n[1]*a -> v[2].n[2])/det;
   b -> v[0].n[2] = (a -> v[0].n[1]*a -> v[1].n[2] - a -> v[0].n[2]*a -> v[1].n[1])/det;
   b -> v[1].n[0] = c1/det;
   b -> v[1].n[1] = (a -> v[0].n[0]*a -> v[2].n[2] - a -> v[0].n[2]*a -> v[2].n[0])/det;
   b -> v[1].n[2] = (a -> v[0].n[2]*a -> v[1].n[0] - a -> v[0].n[0]*a -> v[1].n[2])/det;
   b -> v[2].n[0] = c2/det;
   b -> v[2].n[1] = (a -> v[0].n[1]*a -> v[2].n[0] - a -> v[0].n[0]*a -> v[2].n[1])/det;
   b -> v[2].n[2] = (a -> v[0].n[0]*a -> v[1].n[1] - a -> v[0].n[1]*a -> v[1].n[0])/det;

   return TRUE;
}
// Axis of the matrix/array. No specific meaning at all.
#define VX      0
#define VY      1
#define VZ      2
// Initiate a vector
void _oyVEC3init(oyVEC3* r, double x, double y, double z)
{
    r -> n[VX] = x;
    r -> n[VY] = y;
    r -> n[VZ] = z;
}
// Multiply two matrices
void _oyMAT3per(oyMAT3* r, const oyMAT3* a, const oyMAT3* b)
{
#define ROWCOL(i, j) \
    a->v[i].n[0]*b->v[0].n[j] + a->v[i].n[1]*b->v[1].n[j] + a->v[i].n[2]*b->v[2].n[j]

    _oyVEC3init(&r-> v[0], ROWCOL(0,0), ROWCOL(0,1), ROWCOL(0,2));
    _oyVEC3init(&r-> v[1], ROWCOL(1,0), ROWCOL(1,1), ROWCOL(1,2));
    _oyVEC3init(&r-> v[2], ROWCOL(2,0), ROWCOL(2,1), ROWCOL(2,2));

#undef ROWCOL //(i, j)
}

// End of lcms code

typedef struct {
  double xy[2];
} oyCIExyY;
typedef struct {
  oyCIExyY v[3];
} oyCIExyYTriple;

void _oyMAT3toCIExyYTriple ( const oyMAT3* a,oyCIExyYTriple * triple )
{
  int i,j,
      fail=0;
  double sum;
    for(i = 0; i < 3; ++i)
    {
      for(j = 0; j < 3; ++j)
      {
        if(i < 3 && a->v[i].n[j] == 0)
          fail = 1;
      }
      sum = a->v[i].n[0]+a->v[i].n[1]+a->v[i].n[2];
      if(sum != 0)
      {
        triple->v[i].xy[0] = a->v[i].n[0]/sum;
        triple->v[i].xy[1] = a->v[i].n[1]/sum;
      } else
      {
        triple->v[i].xy[0] = 1;
        triple->v[i].xy[1] = 1;
      }
    }
}
const char * _oyMAT3show ( const oyMAT3* a )
{
  static char * t = (char*) malloc(1024);
  int i,j;
  t[0] = 0;
  for(i = 0; i < 3; ++i)
  {
    for(j = 0; j < 3; ++j)
      sprintf( &t[strlen(t)], " %g", a->v[i].n[j]);
    sprintf( &t[strlen(t)], "\n" );
  }
  return t;
}
const char * _oyMat34show ( const float a[3][4] )
{
  static char * t = (char*) malloc(1024);
  int i,j;
  t[0] = 0;
  for(i = 0; i < 3; ++i)
  {
    for(j = 0; j < 4; ++j)
      sprintf( &t[strlen(t)], " %g", a[i][j]);
    sprintf( &t[strlen(t)], "\n" );
  }
  return t;
}
const char * _oyMat4show ( const float a[4] )
{
  static char * t = (char*) malloc(1024);
  int i;
  t[0] = 0;
  for(i = 0; i < 4; ++i)
    sprintf( &t[strlen(t)], " %g", a[i]);
  sprintf( &t[strlen(t)], "\n" );
  return t;
}
const char * _oyMat43show ( const float a[4][3] )
{
  static char * t = (char*) malloc(1024);
  int i,j;
  t[0] = 0;
  for(i = 0; i < 4; ++i)
  {
    for(j = 0; j < 3; ++j)
      sprintf( &t[strlen(t)], " %g", a[i][j]);
    sprintf( &t[strlen(t)], "\n" );
  }
  return t;
}
const char * _oyCIExyYTriple_Show( oyCIExyYTriple * triple )
{
  static char * t = (char*) malloc(1024);
  int i;
  t[0] = 0;
  for(i = 0; i < 3; ++i)
  {
    sprintf( &t[strlen(t)], " x:%g y:%g", triple->v[i].xy[0],
                                          triple->v[i].xy[1]);
    sprintf( &t[strlen(t)], "\n" );
  }
  return t;
}

oyProfile_s * createMatrixProfile      ( libraw_colordata_t & color )
{
  static oyProfile_s * p = NULL;

  if(color.profile_length)
    p = oyProfile_FromMem( color.profile_length, color.profile, 0,0);

  if(!p)
  {
    oyOption_s *matrix = oyOption_FromRegistration("///colour_matrix."
              "from_primaries."
              "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma", NULL );

    int fail = 0;
    for(int i = 0; i < 3; ++i)
    {
      for(int j = 0; j < 3; ++j)
      {
        if(i < 3 && color.cam_xyz[i][j] == 0)
          fail = 1;
      }
    }
    oyMAT3 cam_zyx, pre_mul, ab_cm, ab_cm_inverse;
    oyCIExyYTriple ab_cm_inverse_xyY;
    
    // Convert camera matrix to ICC profile
    // In theory that should perform the same conversion like dcraw/libraw do.

    memset(&pre_mul,0,sizeof(oyMAT3));
    for(int i = 0; i < 3; ++i)
      pre_mul.v[i].n[i] = color.pre_mul[i];
    for(int i = 0; i < 3; ++i)
      for(int j = 0; j < 3; ++j)
        // mirror diagonal
        cam_zyx.v[j].n[i] = color.cam_xyz[i][j];

    // DNG-1.3 says in Mapping Camera Color Space to CIE XYZ Space
    // XYZtoCamera = AB (AnalogBalance:pre_mul?) * CC (CameraCalibration2:?)
    //               * CM (ColorMatrix2:cam_xyz)

    // multiply AB * CM
    _oyMAT3per( &ab_cm, &cam_zyx, &pre_mul );
    if(_oyMAT3inverse( &ab_cm, &ab_cm_inverse ))
      // convert to CIE*xyY
      _oyMAT3toCIExyYTriple( &ab_cm_inverse, &ab_cm_inverse_xyY );
    else
    {
      fail = 1;
      message( oyMSG_WARN, (oyStruct_s*)0,
             OY_DBG_FORMAT_ "ab_cm is singular",
             OY_DBG_ARGS_ );
    }

    if(oy_debug)
    {
      printf("color.cam_xyz:\n%s",_oyMat43show( color.cam_xyz ));
      printf("color.cam_mul:\n%s",_oyMat4show( color.cam_mul ));
      printf("color.pre_mul:\n%s",_oyMat4show( color.pre_mul ));
      printf("pre_mul:\n%s",_oyMAT3show( const_cast<oyMAT3*>(&pre_mul) ));
      printf("color.rgb_cam:\n%s",_oyMat34show( color.rgb_cam ));
      printf("color.cmatrix:\n%s",_oyMat34show( color.cmatrix ));
      printf("ab*cm|pre_mul*cam_xyz:\n%s",_oyMAT3show( const_cast<oyMAT3*>(&ab_cm) ));
      printf("ab_cm_inverse:\n%s",_oyMAT3show( const_cast<oyMAT3*>(&ab_cm_inverse) ));
      if(!fail)
      printf("=> ");
      printf("ab_cm_inverse_xyY:\n%s", _oyCIExyYTriple_Show(const_cast<oyCIExyYTriple*>(&ab_cm_inverse_xyY)));
    }
    if(!fail)
    {
      oyCIExyYTriple * use = &ab_cm_inverse_xyY;
      oyOption_SetFromDouble( matrix, use->v[0].xy[0], 0, 0);
      oyOption_SetFromDouble( matrix, use->v[0].xy[1], 1, 0);
      oyOption_SetFromDouble( matrix, use->v[1].xy[0], 2, 0);
      oyOption_SetFromDouble( matrix, use->v[1].xy[1], 3, 0);
      oyOption_SetFromDouble( matrix, use->v[2].xy[0], 4, 0);
      oyOption_SetFromDouble( matrix, use->v[2].xy[1], 5, 0);

      /* D65 */
      oyOption_SetFromDouble( matrix, 0.31271, 6, 0);
      oyOption_SetFromDouble( matrix, 0.32902, 7, 0);
    } else
    // fall back
    {
    /* http://www.color.org/chardata/rgb/rommrgb.xalter
     * original gamma is 1.8, we adapt to typical cameraRAW gamma of 1.0 */
      oyOption_SetFromDouble( matrix, 0.7347, 0, 0);
      oyOption_SetFromDouble( matrix, 0.2653, 1, 0);
      oyOption_SetFromDouble( matrix, 0.1596, 2, 0);
      oyOption_SetFromDouble( matrix, 0.8404, 3, 0);
      oyOption_SetFromDouble( matrix, 0.0366, 4, 0);
      oyOption_SetFromDouble( matrix, 0.0001, 5, 0);
      oyOption_SetFromDouble( matrix, 0.3457, 6, 0);
      oyOption_SetFromDouble( matrix, 0.3585, 7, 0);
      fail = 1;
    }
    oyOption_SetFromDouble( matrix, 1.0, 8, 0);

    oyOptions_s * opts = oyOptions_New(0),
                * result = 0;

    oyOptions_MoveIn( opts, &matrix, -1 );
    const char * reg = "//"OY_TYPE_STD"/create_profile.colour_matrix.icc";
    oyOptions_Handle( reg,opts,"create_profile.icc_profile.colour_matrix",
                      &result );

    p = (oyProfile_s*)oyOptions_GetType( result, -1, "icc_profile",
                                               oyOBJECT_PROFILE_S );
    oyOptions_Release( &result );
    if(!p)
      message(oyMSG_DBG, (oyStruct_s*)0,
          OY_DBG_FORMAT_ " profile creation failed by \"%s\"",
          OY_DBG_ARGS_, reg);

    if(!fail)
      oyProfile_AddTagText( p, icSigProfileDescriptionTag,
                                            "cam_xyz gamma 1.0" );
    else
      oyProfile_AddTagText( p, icSigProfileDescriptionTag,
                                            "ICC Examin ROMM gamma 1.0" );

    if(oy_debug)
    {
      size_t size = 0;
      char * data = (char*) oyProfile_GetMem( p, &size, 0, malloc );
      if(!fail)
        oyWriteMemToFile_( "cam_xyz gamma 1.0.icc", data, size );
      else
        oyWriteMemToFile_( "ICC Examin ROMM gamma 1.0.icc", data, size );
    }
  }

  return p;
}


oyConfig_s * oyREgetColorInfo        ( const char        * filename,
                                       libraw_output_params_t * device_context,
                                       oyOptions_s       * options )
{
   oyConfig_s * device = NULL;
   //2.  Get the relevant color information from Oyranos
   //    This is the "command" -> "properties" call
   //Request the properties call
   oyOptions_SetFromText(&options, OY_LIBRAW_REGISTRATION OY_SLASH "command", "properties", OY_CREATE_NEW);
   oyOptions_SetFromText(&options, OY_LIBRAW_REGISTRATION OY_SLASH "device_name", "dummy", OY_CREATE_NEW);
   //Pass in the filename
   oyOptions_SetFromText(&options, OY_LIBRAW_REGISTRATION OY_SLASH "device_handle", filename, OY_CREATE_NEW);
   //Pass in the libraw object with the raw image rendering options
   oyOption_s *context_opt = oyOption_FromRegistration(
                           OY_LIBRAW_REGISTRATION OY_SLASH "device_context", 0);
   oyOption_SetFromData(context_opt, (oyPointer)&device_context, sizeof(libraw_output_params_t*));
   oyOptions_MoveIn(options, &context_opt, -1);

   /*Call Oyranos*/
   oyDeviceGet(OY_TYPE_STD, "raw-image", "dummy", options, &device);

  return device;
}

extern "C" {
int              oyArray2d_ToPPM_    ( oyArray2d_s       * array,
                                       const char        * file_name ); }

/** Function lrawFilterPlug_ImageInputRAWRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
int      lrawFilterPlug_ImageInputRAWRun (
                                       oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  oyFilterSocket_s * socket = 0;
  oyFilterNode_s * node = 0;
  int error = 0;
  const char * filename = 0;
  oyDATATYPE_e data_type = oyUINT8;
  oyPROFILE_e profile_type = oyEDITING_RGB;
  oyProfile_s * prof = 0;
  oyImage_s * image_in = 0;
  oyPixel_t pixel_type = 0;
  uint8_t * buf = 0;
  size_t  mem_n = 0;   /* needed memory in bytes */

  int info_good = 1;

  int type = 0;        /* PNM type */
  int width = 0;
  int height = 0;
  int spp = 0;         /* samples per pixel */
  int byteps = 1;      /* byte per sample */
  double maxval = 255; 
  oyConfig_s * device = 0;
  oyOptions_s * options = 0;

  LibRaw rip;
  libraw_processed_image_t * image_rgb = 0;

  libraw_output_params_t * params = rip.output_params_ptr();

  if(requestor_plug->type_ == oyOBJECT_FILTER_PLUG_S)
    socket = oyFilterPlug_GetSocket( requestor_plug );
  else if(requestor_plug->type_ == oyOBJECT_FILTER_SOCKET_S)
    socket = (oyFilterSocket_s*) requestor_plug;

  node = oyFilterSocket_GetNode( socket );

  message(oyMSG_DBG, (oyStruct_s*)node,
          OY_DBG_FORMAT_ " output_color was: %d  output_bps: %d no_auto_bright: %d\ng[0] %g g[1] %g",
          OY_DBG_ARGS_,
          params->output_color, params->output_bps, params->no_auto_bright,
          params->gamm[0], params->gamm[1]);

  /* render at half size */
  params->half_size = 0;
  params->four_color_rgb = 0;

  oyImage_s * image = (oyImage_s*)oyFilterSocket_GetData( socket );
  /* passing through the data reading */
  if(requestor_plug->type_ == oyOBJECT_FILTER_PLUG_S &&
     image)
  {
    error = oyFilterPlug_ImageRootRun( requestor_plug, ticket );
    oyImage_Release( &image );
    oyFilterSocket_Release( &socket );
    oyFilterNode_Release( &node );

    return error;

  }

  if(error <= 0)
    {
      oyOptions_s * opts = oyFilterNode_GetOptions( node, 0 );
      filename = oyOptions_FindString( opts, "filename", 0 );
      oyOptions_Release( &opts );
    }

  if(filename)
    error = rip.open_file( filename );

  if(error)
  {
    message( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_ " could not open: %s",
             OY_DBG_ARGS_, oyNoEmptyString_m_( filename ) );
    return 1;
  }

  double clck = oyClock();
  error = rip.unpack();
  clck = oyClock() - clck;
  DBG_NUM1_S("rip.unpack(): %g", clck/1000000.0 );

  params->output_color = 0;    /* raw_color */
  params->output_bps = 16;     /* linear space */
  params->gamm[0] = 1.0; params->gamm[1] = 1.0;
  params->user_qual = 3;
  params->use_camera_wb = 1;
  params->no_auto_bright = 1;

  oyOptions_s * node_options = oyFilterNode_GetOptions( node, 0 );
  int render = oyOptions_FindString( node_options, "render", "0" ) == NULL ? 1 : 0;

  if(render)
  {
    clck = oyClock();
    error = rip.dcraw_process();
    clck = oyClock() - clck;
    DBG_NUM1_S("rip.dcraw_process(): %g", clck/1000000.0 );
  }

  if(render)
  {
    clck = oyClock();
    image_rgb = rip.dcraw_make_mem_image();
    clck = oyClock() - clck;
    DBG_NUM1_S("rip.dcraw_make_mem_image(): %g", clck/1000000.0 );
  }

  if(image_rgb)
  {
    width = image_rgb->width;
    height = image_rgb->height;
    spp = image_rgb->colors;

    if(image_rgb->bits == 16)
    {
      data_type = oyUINT16;
      byteps = 2;
      maxval = 65535;
      message( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_ " maxval: %g",
             OY_DBG_ARGS_, maxval );
    }

  } else
  {
    info_good = 0;
  }

    switch(spp)
    {
      case 1:
           profile_type = oyASSUMED_GRAY;
           type = 5;
           break;
      case 2:
           profile_type = oyASSUMED_GRAY;
           type = 5;
           break;
      case 3:
           profile_type = oyASSUMED_RGB;
           type = 6;
           break;
      case 4:
           profile_type = oyASSUMED_RGB;
           type = 6;
           break;
    }

  if( !info_good &&
      render)
  {
    message( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_ "failed to get info of %s",
             OY_DBG_ARGS_, oyNoEmptyString_m_( filename ));
    return FALSE;
  }

  /* check if the file can hold the expected data (for raw only) */
  mem_n = width*height*byteps*spp;
  if( image_rgb )
    error = mem_n != image_rgb->data_size;

  if(info_good)
    oyAllocHelper_m_( buf, uint8_t, mem_n, 0, return 1);

  /* the following code is almost completely taken from ku.b's ppm CP plug-in */
  /* ... and then copied from the input_ppm Oyranos filter */
  if(info_good)
  {
    int h, j_h = 0, p, n_samples, n_bytes;
    int byte_swap = 0; /*!oyBigEndian();*/
    unsigned char *d_8 = 0;
    unsigned char *src = image_rgb->data;

    uint16_t *d_16;
    float  *d_f;

#pragma omp parallel for
    for(h = 0; h < height; ++h)
    {
        n_samples = 1 * width * spp;
        n_bytes = n_samples * byteps;

        d_8  = buf;
        d_16 = (uint16_t*)buf;
        d_f  = (float*)buf;

        /*  TODO 1 bit raw and ascii */
        if (type == 1 || type == 4) {

        /*  TODO ascii  */
        } else if (type == 2 || type == 3) {


        /*  raw and floats */
        } else if (type == 5 || type == 6 ||
                   type == -5 || type == -6 ||
                   type == 7 )
        {
          if(byteps == 1) {
            d_8 = &src[ h * width * spp * byteps ];
          } else if(byteps == 2) {
            d_16 = (uint16_t*)& src[ h * width * spp * byteps ];
          } else if(byteps == 4) {
            d_f = (float*)&src[ h * width * spp * byteps ];
          }
          memcpy (&buf[ h * width * spp * byteps ],
                  &src[ (j_h + h) * width * spp * byteps ],
                  1 * width * spp * byteps);
        }

        /* normalise and byteswap */
        if( byte_swap )
        {
          unsigned char *c_buf = &buf[ h * width * spp * byteps ];
          char  tmp;
          if (byteps == 2) {         /* 16 bit */
            for (p = 0; p < n_bytes; p += 2)
            {
              tmp = c_buf[p];
              c_buf[p] = c_buf[p+1];
              c_buf[p+1] = tmp;
            }
          } else if (byteps == 4) {  /* float */
            for (p = 0; p < n_bytes; p += 4)
            {
              tmp = c_buf[p];
              c_buf[p] = c_buf[p+3];
              c_buf[p+3] = tmp;
              tmp = c_buf[p+1];
              c_buf[p+1] = c_buf[p+2];
              c_buf[p+2] = tmp;
            }
          }
        }

        if (byteps == 1 && maxval < 255) {         /*  8 bit */
          for (p = 0; p < n_samples; ++p)
            d_8[p] = (d_8[p] * 255) / maxval;
        } else if (byteps == 2 && maxval < 65535) {/* 16 bit */
          for (p = 0; p < n_samples; ++p)
            d_16 [p] = (d_16[p] * 65535) / maxval;
        } else if (byteps == 4 && maxval != 1.0) {  /* float */
          for (p = 0; p < n_samples; ++p)
            d_f[p] = d_f[p] * maxval;
        }
    }
  }

  pixel_type = oyChannels_m(spp) | oyDataType_m(data_type);
  int32_t n = 0;
  error = oyOptions_Filter( &options, &n, 0,
                      oyBOOLEAN_INTERSECTION, "///config",node_options);
  device = oyREgetColorInfo( filename, params, options );
  error = oyDeviceGetProfile( device, 0, &prof );
  if(!prof || error != 0)
  {
    if(prof)
      oyProfile_Release( &prof );
    if(profile_type == oyASSUMED_RGB)
      prof = createMatrixProfile( rip.imgdata.color );
    else
      prof = oyProfile_FromStd( profile_type, 0 );
  }

  if(oy_debug)
  {
    const char * t = oyProfile_GetText( prof, oyNAME_NAME );
    message( oyMSG_DBG, (oyStruct_s*)node,
             OY_DBG_FORMAT_ "image profile %s",
             OY_DBG_ARGS_,  t?t:"---" );
  }

  image_in = oyImage_Create( width, height, buf, pixel_type, prof, 0 );
  buf = 0;


  oyOptions_s * image_in_tags = oyImage_GetTags( image_in );
  if(oyOptions_FindString( node_options, "device", "1" ) != NULL)
  {
    oyOptions_MoveInStruct( &image_in_tags,
                            "//" OY_TYPE_STD OY_SLASH CMM_NICK "/device",
                            (oyStruct_s**)&device, OY_CREATE_NEW );
  }
  oyConfig_Release( &device );

  if(oy_debug && image_in)
  {
    oyArray2d_s * a = (oyArray2d_s*)oyImage_GetPixelData( image_in );
    oyArray2d_ToPPM_( a, "test_oy_dbg_lraw.ppm" );
    oyArray2d_Release( &a );
  }

  oyProfile_Release( &prof );

  if (!image_in)
  {
      message( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_ "libraw can't create a new image\n%dx%d %d",
             OY_DBG_ARGS_,  width, height, pixel_type );
      oyFree_m_ (buf)
    return FALSE;
  }

  
  error = oyOptions_SetFromText( &image_in_tags,
                              "//" OY_TYPE_STD OY_SLASH CMM_NICK "/filename",
                                 filename, OY_CREATE_NEW );

  if(error <= 0)
  {
    oyFilterSocket_SetData( socket, (oyStruct_s*)image_in );
  }

  oyImage_s * output_image = oyPixelAccess_GetOutputImage( ticket );
  if(ticket &&
     output_image &&
     oyImage_GetWidth( output_image ) == 0 &&
     oyImage_GetHeight( output_image ) == 0)
  {
    oyImage_SetCritical( output_image, oyImage_GetPixelLayout( image_in,
                                                               oyLAYOUT ),
                         0,0,
                         oyImage_GetWidth( image_in ),
                         oyImage_GetHeight( image_in ) );
  }

  oyImage_Release( &image_in );
  oyOptions_Release( &image_in_tags );
  oyOptions_Release( &node_options );
  oyImage_Release( &image );
  oyImage_Release( &output_image );
  oyFilterSocket_Release( &socket );
  oyFilterNode_Release( &node );

  /* return an error to cause the graph to retry */
  return 1;
}

const char lraw_extra_options[] = {
 "\n\
  <" OY_TOP_SHARED ">\n\
   <" OY_DOMAIN_INTERNAL ">\n\
    <" OY_TYPE_STD ">\n\
     <" CMM_NICK ".file_read" ">\n\
      <filename>----</filename>\n\
     </" CMM_NICK ".file_read" ">\n\
     <" CMM_NICK ">\n\
      <output_bps>16</output_bps>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <use_auto_wb.advanced>0</use_auto_wb.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <use_camera_wb.advanced>1</use_camera_wb.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <four_color_rgb.advanced>0</four_color_rgb.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <half_size.advanced>0</half_size.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <highlight.advanced>0</highlight.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <gamma_16bit.advanced>0</gamma_16bit.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <auto_bright.advanced>0.01</auto_bright.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <no_auto_bright.advanced>0</no_auto_bright.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <use_fujy_rotate.advanced>1</use_fujy_rotate.advanced>\n\
     </" CMM_NICK ">\n\
     <" CMM_NICK ">\n\
      <filtering_mode.advanced>0</filtering_mode.advanced>\n\
     </" CMM_NICK ">\n\
    </" OY_TYPE_STD ">\n\
   </" OY_DOMAIN_INTERNAL ">\n\
  </" OY_TOP_SHARED ">\n"
};

#define A(long_text) STRING_ADD( tmp, long_text)

/** Function lrawUiGet
 *  @brief   return XFORMS for matching options
 *
 *  @version Oyranos: 0.1.13
 *  @since   2010/11/29 (Oyranos: 0.1.13)
 *  @date    2010/11/29
 */
int  lrawUiGet                       ( oyOptions_s       * options,
                                       char             ** xforms_layout,
                                       oyAlloc_f           allocateFunc )
{
  char * tmp = 0;

  tmp = (char *)oyOptions_FindString( options,
                                      "output_bps", 0 );
  if(tmp == 0)
    return 0;

  tmp = oyStringCopy_( "\
  <h3>libRAW ", oyAllocateFunc_ );

  A(       _("Module Options"));
  A(                         ":</h3>\n");
  A("\
     <xf:select1 ref=\"/" OY_TOP_SHARED "/" OY_DOMAIN_INTERNAL "/" OY_TYPE_STD "/" CMM_NICK "/output_bps\">\n\
      <xf:label>" );
  A(          _("Bits per Sample"));
  A(                              "</xf:label>\n\
      <xf:help>" );
  A(          _("More bits mean more precission for processing and more size."));
  A(                              "</xf:help>\n\
      <xf:choices>\n\
       <xf:item>\n\
        <xf:value>8</xf:value>\n\
        <xf:label>");
  A(             _("8-bit"));
  A(                     "</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>16</xf:value>\n\
        <xf:label>");
  A(             _("16-bit"));
  A(                     "</xf:label>\n\
       </xf:item>\n\
      </xf:choices>\n\
     </xf:select1>\n");

  if(allocateFunc && tmp)
  {
    char * t = oyStringCopy_( tmp, allocateFunc );
    oyFree_m_( tmp );
    tmp = t; t = 0;
  } else
    return 1;

  *xforms_layout = tmp;
  return 0;
};

oyDATATYPE_e lraw_data_types[3] = {oyUINT8, oyUINT16, (oyDATATYPE_e)0};

oyConnectorImaging_s_ lraw_imageInputRAW_connector = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorSocketText, /* getText */
  oy_image_connector_texts, /* texts */
  (char*)"//" OY_TYPE_STD "/image.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  lraw_data_types,
  2, /* data_types_n; elements in data_types array */
  -1, /* max_colour_offset */
  1, /* min_channels_count; */
  3, /* max_channels_count; */
  1, /* min_colour_count; */
  3, /* max_colour_count; */
  0, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  0, /* can_swap; can swap colour channels (BGR)*/
  0, /* can_swap_bytes; non host byte order */
  0, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  0, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* count in channel_types */
  1, /* id; relative to oyFilter_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s_ * lraw_imageInputRAW_connectors[2] = 
             { &lraw_imageInputRAW_connector, 0 };


const char * oyraApi4UiImageInputLibrawGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  if(strcmp(select,"name"))
  {
    if(type == oyNAME_NICK)
      return CMM_NICK;
    else if(type == oyNAME_NAME)
      return _("Image[lraw]");
    else if(type == oyNAME_DESCRIPTION)
      return _("Input libraw Image Filter Object");
  } else if(strcmp(select,"help"))
  {
    if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("The filter obtains a image from libraw.");
    else if(type == oyNAME_DESCRIPTION)
      return _("The filter is a libraw image reader. The option \"device\" "
               "with value \"1\" requestests a oyConfig_s object containing "
               "typical device informations about the cameraRAW file. "
               "The option \"render\" with value \"0\" will skip some of the "
               "data processing.");
  }
  return 0;
}
const char * oyra_api4_ui_image_input_libraw_texts[] = {"name", "help", 0};

/** @instance lraw_api4_ui_image_input_libraw
 *  @brief    lraw oyCMMapi4_s::ui implementation
 *
 *  The UI for image input libraw.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/09/09
 */
oyCMMui_s_   oraw_api4_ui_image_input_libraw = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  {0,9,0},                            /**< int32_t module_api[3] */

  lrawFilter_ImageInputRAWValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  lrawWidgetEvent, /* oyWidgetEvent_f */

  "Files/Read cameraRAW", /* category */
  lraw_extra_options,   /* const char * options */
  lrawUiGet,      /* oyCMMuiGet_f oyCMMuiGet */

  oyraApi4UiImageInputLibrawGetText, /* oyCMMGetText_f getText */
  oyra_api4_ui_image_input_libraw_texts /* (const char**)texts */
};

/** @instance lraw_api4
 *  @brief    lraw oyCMMapi4_s implementation
 *
 *  A filter for reading a RAW image.
 *
 *  @par Options:
 *  - "filename" - the file name to read from
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
oyCMMapi4_s_ lraw_api4_image_input_libraw = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & lraw_api7_image_input_libraw, /* oyCMMapi_s * next */
  
  lrawCMMInit, /* oyCMMInit_f */
  lrawCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_LIBRAW_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  {0,9,0},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  lrawFilterNode_LibrawContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_GetText_f        oyCMMFilterNode_GetText */
  {0}, /* char context_type[8] */

  &oraw_api4_ui_image_input_libraw     /**< oyCMMui_s *ui */
};

const char * lraw_api7_image_input_raw_properties[] =
{
  "file=read",    /* file load|write */
  "image=pixel",  /* image type, pixel/vector/font */
  "layers=1",     /* layer count, one for plain images */
  "icc=0",        /* image type ICC profile support */
  "ext=cr2,crw,dng,mrf,nef,orf,raf,rw2,sti", /* supported extensions */
  0
};


/** @instance lraw_api7
 *  @brief    lraw oyCMMapi7_s implementation
 *
 *  A filter reading a RAW image.
 *
 *  @par Options:
 *  - "filename" - the file name to write to
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/06/14
 *  @since   2009/06/14 (Oyranos: 0.1.10)
 */
oyCMMapi7_s_ lraw_api7_image_input_libraw = {

  oyOBJECT_CMM_API7_S, /* oyStruct_s::type oyOBJECT_CMM_API7_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) 0, /* oyCMMapi_s * next */
  
  lrawCMMInit, /* oyCMMInit_f */
  lrawCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_LIBRAW_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  {0,9,0},                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */

  lrawFilterPlug_ImageInputRAWRun, /* oyCMMFilterPlug_Run_f */
  {0}, /* char data_type[8] */

  0,   /* plugs */
  0,   /* plugs_n */
  0,   /* plugs_last_add */
  (oyConnector_s**) lraw_imageInputRAW_connectors,   /* sockets */
  1,   /* sockets_n */
  0,   /* sockets_last_add */

  (char**)lraw_api7_image_input_raw_properties /* char * properties */
};


/* OY_LIBRAW_REGISTRATION ----------------------------------------------------*/

