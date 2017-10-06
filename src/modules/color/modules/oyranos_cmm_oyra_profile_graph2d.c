/** @file oyranos_cmm_oyra_profile_graph2d.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2017 (C) Kai-Uwe Behrmann
 *
 *  @brief    ICC profile graph 2D module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2017/04/12
 */

#include "oyCMMapi10_s_.h"
#include "oyCMMui_s.h"

#include "oyranos_alpha_internal.h"
#include "oyranos_cmm.h"
#include "oyranos_cmm_oyra.h"
#include "oyranos_db.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_string.h"


#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_POSIX
#include <stdint.h>  /* UINT32_MAX */
#endif
#include <locale.h>

/* OY_PROFILE_GRAPH2D_SATURATION_REGISTRATION */
#define SAT_L_COMMAND "saturation_line"
int          oyraMOptions_HandleSatL ( oyOptions_s       * options,
                                       oyOptions_s      ** result );



/* OY_PROFILE_GRAPH2D_SATURATION_LINE_REGISTRATION ---------------------------*/


#define OY_PROFILE_GRAPH2D_SATURATION_LINE_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "graph2d." SAT_L_COMMAND

float * oyraCreateLabGradient_( int steps, size_t * size )
{
    int i, k = 3;
    double schritte = (double)steps,
           max = 1.;
    float * block;

    *size = (int)schritte*4 + 1;

    block = (float*) calloc( *size*k, sizeof(float) );
    for(i = 0; i < (int)*size; ++i) {
      /*  CIE*L  */
      block[k*i+0] = 0.5;
      /*  CIE*a  */
      if(i >= schritte * 1 && i < schritte * 2)
        block[k*i+1] = (max/schritte*(i-1*schritte));
      if(i >= schritte * 2 && i < schritte * 3)
        block[k*i+1] = max;
      if(i >= schritte * 3 && i < schritte * 4)
        block[k*i+1] = (max/schritte*(4*schritte-i));
      /*  CIE*b  */
      if(i >= schritte * 2 && i < schritte * 3)
        block[k*i+2] = (max/schritte*(i-2*schritte));
      if(i >= schritte * 3 && i < schritte * 4)
        block[k*i+2] = max;
      if(i >= schritte * 0 && i < schritte * 1)
        block[k*i+2] = (max/schritte*(1*schritte-i));
    }

    block[*size*3-3+0] = (max/schritte*(schritte-1/schritte));
    block[*size*3-3+1] = 0;
    block[*size*3-3+2] = max;


  return block;
}

float * oyraCreateRGBGradient_( int steps, size_t * size )
{
    int i, k = 3;
    double schritte = (double)steps,
           max = 1.;
    float * block;

    *size = (int)schritte*k*2 + 1;

    block = (float*) calloc( *size*k, sizeof(float) );
    for(i = 0; i < (int)*size; ++i) {
      /*  red  */
      if(i >= schritte * 5 && i < schritte * 6)
        block[k*i+0] = (max/schritte*(i-5*schritte));
      if(i >= schritte * 0 && i < schritte * 2)
        block[k*i+0] = max;
      if(i >= schritte * 2 && i < schritte * 3)
        block[k*i+0] = (max/schritte*(3*schritte-i));
      /*  green  */
      if(i >= schritte * 1 && i < schritte * 2)
        block[k*i+1] = (max/schritte*(i-1*schritte));
      if(i >= schritte * 2 && i < schritte * 4)
        block[k*i+1] = max;
      if(i >= schritte * 4 && i < schritte * 5)
        block[k*i+1] = (max/schritte*(5*schritte-i));
      /*  blue  */
      if(i >= schritte * 3 && i < schritte * 4)
        block[k*i+2] = (max/schritte*(i-3*schritte));
      if(i >= schritte * 4 && i < schritte * 6)
        block[k*i+2] = max;
      if(i >= schritte * 0 && i < schritte * 1)
        block[k*i+2] = (max/schritte*(1*schritte-i));
    }

    block[*size*3-3+0] = (max/schritte*(schritte-1/schritte));
    block[*size*3-3+1] = 0;
    block[*size*3-3+2] = max;

  return block;
}

float * oyraCreateCMYKGradient_( int steps, size_t * size )
{
  int i, k = 4;
  double schritte = (double) steps,
         max = 1.;
  float * block;

  *size = (int)schritte*(k-1)*2 + 1;

  block = (float*) calloc( *size*k, sizeof(float) );

  for(i = 0; i < (int)*size*k; ++i) {
    block[i] = 0;
  }

  for(i = 0; i < (int)*size; ++i) {
    /*  cyan  */
    if(i >= schritte * 5 && i < schritte * 6)
      block[k*i+0] = (max/schritte*(i-5*schritte));
    if(i >= schritte * 0 && i < schritte * 2)
      block[k*i+0] = max;
    if(i >= schritte * 2 && i < schritte * 3)
      block[k*i+0] = (max/schritte*(3*schritte-i));
    /*  magenta  */
    if(i >= schritte * 1 && i < schritte * 2)
      block[k*i+1] = (max/schritte*(i-1*schritte));
    if(i >= schritte * 2 && i < schritte * 4)
      block[k*i+1] = max;
    if(i >= schritte * 4 && i < schritte * 5)
      block[k*i+1] = (max/schritte*(5*schritte-i));
    /*  yellow  */
    if(i >= schritte * 3 && i < schritte * 4)
      block[k*i+2] = (max/schritte*(i-3*schritte));
    if(i >= schritte * 4 && i < schritte * 6)
      block[k*i+2] = max;
    if(i >= schritte * 0 && i < schritte * 1)
      block[k*i+2] = (max/schritte*(1*schritte-i));
  }

  block[*size*k-k+0] = (max/schritte*(schritte-1/schritte));
  block[*size*k-k+1] = 0;
  block[*size*k-k+2] = max;

  return block;
}

/** @brief creates a linie around the saturated colors of Cmyk and Rgb profiles */
double * oyraGetSaturationLine_(oyProfile_s * profile, int intent, int precision, size_t * size_, oyProfile_s * outspace)
{
  int i;
  double *lab_erg = 0;

  icColorSpaceSignature csp = (icColorSpaceSignature)
                              oyProfile_GetSignature( profile,
                                                      oySIGNATURE_COLOR_SPACE);
  if(oy_debug)
    oyra_msg( oyMSG_DBG, (oyStruct_s*)profile, OY_DBG_FORMAT_ "%s",
              OY_DBG_ARGS_, oyICCColorSpaceGetName(csp) );

  if(csp == icSigRgbData ||
     csp == icSigXYZData ||
     csp == icSigYCbCrData ||
     csp == icSigCmykData ||
     csp == icSigLabData)
  {
    float *block = 0;
    float *lab_block = 0;

    /* scan here the color space border */
    {
      size_t size = 0;
      oyOptions_s * options = NULL;
      char num[24];

      if(csp == icSigRgbData || csp == icSigXYZData || csp == icSigYCbCrData)
        block = oyraCreateRGBGradient_( precision, &size );
      else if(csp == icSigCmykData)
        block = oyraCreateCMYKGradient_( precision, &size );
      else if(csp == icSigLabData)
        block = oyraCreateLabGradient_( precision, &size );

      if(oy_debug)
        oyra_msg( oyMSG_DBG, 0, OY_DBG_FORMAT_ "precision: %d size: %d",
                                OY_DBG_ARGS_, precision, size );
      lab_block = (float*) malloc(size*4*sizeof(float));

      if(!(block && lab_block))
        return NULL;

      sprintf(num,"%d", intent);
      oyOptions_SetFromText( &options, OY_BEHAVIOUR_STD OY_SLASH "rendering_intent",
                            num, OY_CREATE_NEW);

      oyColorConvert_( profile, outspace, block, lab_block,
                       oyFLOAT, oyFLOAT, options, size );
      *size_ = size;
      lab_erg =  (double*) calloc( sizeof(double), *size_ * 3);
      for(i = 0; i < (int)(*size_ * 3); ++i) {
        lab_erg[i] = lab_block[i];
      }
    }
    if(block) free (block);
    if(lab_block) free (lab_block);
  }
  return lab_erg;
}
int          oyraMOptions_HandleSatL ( oyOptions_s       * options,
                                       oyOptions_s      ** result )
{
  int error = 0;
  oyProfile_s * in = (oyProfile_s*) oyOptions_GetType( options, -1,
                             "icc_profile.input", oyOBJECT_PROFILE_S );
  oyProfile_s * outspace = (oyProfile_s*) oyOptions_GetType( options, -1,
                             "icc_profile.output", oyOBJECT_PROFILE_S );
  size_t size = 0;
  double * values;
  int32_t precision = 0;
  int32_t intent = 0; /* default to preceptual */


  if(oyOptions_FindInt( options, "precision", 0, &precision ))
  {
    oyra_msg( oyMSG_WARN, 0, OY_DBG_FORMAT_ "missed \"precision\" option:\n%s", OY_DBG_ARGS_,
	      oyOptions_GetText( options, oyNAME_NICK ) );
    return 1;
  }
  error = oyOptions_FindInt( options, "rendering_intent", 0, &intent );
  if(!outspace)
    outspace = oyProfile_FromName( "lab", 0, NULL );

  if(oy_debug)
    oyra_msg( oyMSG_DBG, 0, OY_DBG_FORMAT_ "options:\n%s\n%d %d", OY_DBG_ARGS_,
	      oyOptions_GetText( options, oyNAME_NICK ), precision, intent );
  values = oyraGetSaturationLine_( in, intent, precision, &size, outspace );
  {
    oyOption_s * o = oyOption_FromRegistration( OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH SAT_L_COMMAND ".output.double._" CMM_NICK,
                        0 );
    int pos;
    for(pos = 0; (size_t)pos < size*3; ++pos)
      oyOption_SetFromDouble( o, values[pos], pos, 0 );
    if(!*result)
      *result = oyOptions_New(0);
    oyOptions_MoveIn( *result, &o, -1 );
    if(oy_debug)
      oyra_msg( oyMSG_DBG, 0, OY_DBG_FORMAT_ "result: %d\n%s", OY_DBG_ARGS_,
                size, oyOptions_GetText( *result, oyNAME_NICK ) );
  }
  return error;
}

/* OY_PROFILE_GRAPH2D_SATURATION_LINE_REGISTRATION ---------------------------*/

/**
 *  This function implements oyMOptions_Handle_f.
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/04/12
 *  @since   2017/04/12 (Oyranos: 0.9.7)
 */
int          oyraMOptions_Handle     ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  int error = 0;

  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    oyra_msg( oyMSG_DBG, 0, "called %s()::can_handle", __func__ );
    return error;
  }
  else if(oyFilterRegistrationMatch(command, SAT_L_COMMAND, 0))
  {
    error = oyraMOptions_HandleSatL(options, result);
    oyra_msg( error?oyMSG_WARN:oyMSG_DBG, 0, "called %s()::db_handler", __func__ );
  }

  return error;
}

const char *oyra_texts_graph2d_create[4] = {"can_handle", SAT_L_COMMAND, "help",0};

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/04/12
 *  @since   2017/04/12 (Oyranos: 0.9.7)
 */
const char * oyraInfoGetTextGraph2D  ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, SAT_L_COMMAND)==0)
  {
         if(type == oyNAME_NICK)
      return SAT_L_COMMAND;
    else if(type == oyNAME_NAME)
      return _("Compute a saturation line.");
    else
      return _("The Oyranos \""SAT_L_COMMAND"\" command will return a saturation curve.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Compute a saturation line.");
    else
      return _("The Oyranos \""SAT_L_COMMAND"\" command will return a saturation curve. Expected are following options: \"icc_profile.input\" for the saturation color space as oyProfile_s; \"icc_profile.output\" as projection color space, typical lab as oyProfile_s, \"precision\" as integer option for computing the size and \"rendering_intent\" as integer. The result will contain a \"saturation_line.output.double._oyra\" option as doubles.");
  }
  return 0;
}

                 
/** @instance oyra_api10_profile_graph2d_SaturationLine
 *  @brief    oyra oyCMMapi10_s implementation
 *
 *  supplier for black body temperature curve
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/04/12
 *  @since   2017/04/12 (Oyranos: 0.9.7)
 */
oyCMMapi10_s_    oyra_api10_profile_graph2d_SaturationLine = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  (oyCMMapi_s*) NULL,

  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  OY_PROFILE_GRAPH2D_SATURATION_LINE_REGISTRATION,

  CMM_VERSION, /* int32_t version[3] */
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */
 
  oyraInfoGetTextGraph2D,             /**< getText */
  (char**)oyra_texts_graph2d_create,   /**<texts; list of arguments to getText*/
 
  oyraMOptions_Handle                  /**< oyMOptions_Handle_f oyMOptions_Handle */
};

/* ---------------------------------------------------------------------------*/



