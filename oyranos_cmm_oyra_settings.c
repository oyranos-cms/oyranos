/** @file oyranos_cmm_oyra_settings.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    backends for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2008/11/13
 */

#include "config.h"
#include "oyranos_alpha.h"
#include "oyranos_cmm.h"
#include "oyranos_cmm_oyra.h"
#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_definitions.h"
#include "oyranos_texts.h"
#include <iconv.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/** Function oyraFilter_CanHandle
 *  @brief   dummy
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/13 (Oyranos: 0.1.9)
 *  @date    2008/11/13
 */
int    oyraFilter_CanHandle          ( oyCMMQUERY_e      type,
                                       uint32_t          value )
{
  int ret = -1;

  return ret;
}

/** Function oyraFilter_defaultICCValidateOptions
 *  @brief   dummy
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/13 (Oyranos: 0.1.9)
 *  @date    2008/11/13
 */
oyOptions_s* oyraFilter_defaultICCValidateOptions
                                     ( oyFilter_s        * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = filter->type_ != oyOBJECT_FILTER_S;

  *result = error;

  return 0;
}

#if 0

oyValue_u oy_oyra_default_colour_ed_rgb_val = {
  "sRGB.icc"  /* dont works, as char* is not he first type in union */
};

oyOption_s oy_oyra_default_colour_ed_rgb = {

  oyOBJECT_STRUCT_LIST_S, /* oyStruct_s::type oyOBJECT_STRUCT_LIST_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */

  1, /* id */
  {oyOBJECT_NAME_S, 0,0,0, "editing_rgb", _("Editing Colour Space"),
  _("Well behaving Colour Space for Editing")}, /* name; translatable, eg "scale" "image scaling" "..." */
  OY_PROFILE_STD, /* registration */

  {0,0,1}, /* version[3] */
  oyVAL_STRING, /* value_type */
  &oy_oyra_default_colour_ed_rgb_val, /* value */
  oyOPTIONSOURCE_FILTER, /* source */
  0 /* flags */
};

#define OYRA_DEFAULT_COLOUR_SETTINGS_OPTIONS 6

oyOption_s * oyra_default_colour_settings_options[OYRA_DEFAULT_COLOUR_SETTINGS_OPTIONS] = {
  0,0,0,0,0
};

oyStructList_s oyra_default_colour_settings_struct_list = {

  oyOBJECT_STRUCT_LIST_S, /* oyStruct_s::type oyOBJECT_STRUCT_LIST_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */

  (oyStruct_s**)oyra_default_colour_settings_options, /* ptr_ */
  OYRA_DEFAULT_COLOUR_SETTINGS_OPTIONS, /* n_ */
  0, /* n_reserved_ */
  "oyra_default_colour_settings", /* list_name */
  oyOBJECT_OPTIONS_S, /* parent_type_ */
};

/** @instance oyra_default_colour_settings
 *  @brief   oyra static default colour CMM settings
 *
 *  default settings for ICC CMM's
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/13 (Oyranos: 0.1.8)
 *  @date    2008/11/13
 */
oyOptions_s  oyra_default_colour_settings = {
  
  oyOBJECT_OPTIONS_S, /* oyStruct_s::type oyOBJECT_OPTIONS_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */

  &oyra_default_colour_settings_struct_list
};

#endif

char oyra_default_colour_options[] = {
 " \n\
  <xf:model> \n\
   <xf:instance> \n\
    <" OY_DOMAIN_STD " xmlns=\"\"> \n\
     <" OY_TYPE_STD "> \n\
      <profile> \n\
       <editing_cmyk>coated_FOGRA39L_argl.icc</editing_cmyk> \n\
       <editing_gray>Gray.icc</editing_gray> \n\
       <editing_lab>Lab.icc</editing_lab> \n\
       <editing_rgb>eciRGB_v2.icc</editing_rgb> \n\
       <editing_xyz>XYZ.icc</editing_xyz> \n\
      </profile> \n\
     </" OY_TYPE_STD "> \n\
    </" OY_DOMAIN_STD "> \n\
   </xf:instance> \n\
  </xf:model> \n\
 "
};

char oyra_default_colour_options_ui[] = {
 " \n\
  <h3>Oyranos Default Profiles:</h3> \n\
  <table> \n\
   <tr> \n\
    <td>Editing Rgb:</td> \n\
    <td> \n\
     <xf:select1 ref=\"/" OY_DOMAIN_STD "/default/profile/editing_rgb\"> \n\
      <xf:choices label=\"Editing RGB\"> \n\
       <sta:profiles cspace1=\"RGB\" class1=\"prtr\" class2=\"mntr\" class3=\"scnr\"/> \n\
       <xf:item> \n\
        <xf:label>sRGB.icc</xf:label> \n\
        <xf:value>sRGB.icc</xf:value> \n\
       </xf:item> \n\
       <xf:item> \n\
        <xf:label>eciRGB_v2.icc</xf:label> \n\
        <xf:value>eciRGB_v2.icc</xf:value> \n\
       </xf:item> \n\
      </xf:choices> \n\
     </xf:select1> \n\
    </td> \n\
   </tr> \n\
   <tr> \n\
    <td>Editing Cmyk:</td> \n\
    <td> \n\
     <xf:select1 ref=\"/" OY_DOMAIN_STD "/default/profile/editing_cmyk\"> \n\
      <xf:choices> \n\
       <sta:profiles cspace1=\"CMYK\" class1=\"prtr\"/> \n\
       <xf:item> \n\
        <xf:label>coated_FOGRA39L_argl.icc</xf:label> \n\
        <xf:value>coated_FOGRA39L_argl.icc</xf:value> \n\
       </xf:item> \n\
      </xf:choices> \n\
     </xf:select1> \n\
    </td> \n\
   </tr> \n\
   <tr> \n\
    <td>Editing Lab:</td> \n\
    <td> \n\
     <xf:select1 ref=\"/" OY_DOMAIN_STD "/default/profile/editing_lab\"> \n\
      <xf:choices xml:lang=\"en\" label=\"Editing Lab\"> \n\
       <sta:profiles cspace1=\"Lab\" class1=\"prtr\" class2=\"mntr\" class3=\"scnr\"/> \n\
       <xf:item> \n\
        <xf:label>Lab.icc</xf:label> \n\
        <xf:value>Lab.icc</xf:value> \n\
       </xf:item> \n\
       <xf:item> \n\
        <xf:label>CIELab.icc</xf:label> \n\
        <xf:value>CIELab.icc</xf:value> \n\
       </xf:item> \n\
      </xf:choices> \n\
     </xf:select1> \n\
    </td> \n\
   </tr> \n\
   <tr> \n\
    <td>Editing XYZ:</td> \n\
    <td> \n\
     <xf:select1 ref=\"/" OY_DOMAIN_STD "/" OY_TYPE_STD "/profile/editing_xyz\"> \n\
      <xf:choices> \n\
       <sta:profiles cspace1=\"XYZ\" class1=\"prtr\" class2=\"mntr\" class3=\"scnr\"/> \n\
       <xf:item> \n\
        <xf:label>XYZ.icc</xf:label> \n\
        <xf:value>XYZ.icc</xf:value> \n\
       </xf:item> \n\
       <xf:item> \n\
        <xf:label>CIEXYZ.icc</xf:label> \n\
        <xf:value>CIEXYZ.icc</xf:value> \n\
       </xf:item> \n\
      </xf:choices> \n\
     </xf:select1> \n\
    </td> \n\
   </tr> \n\
   <tr> \n\
    <td>Editing Gray:</td> \n\
    <td> \n\
     <xf:select1 ref=\"/" OY_DOMAIN_STD "/" OY_TYPE_STD "/profile/editing_gray\"> \n\
      <xf:choices> \n\
       <sta:profiles cspace1=\"Gray\" class1=\"prtr\" class2=\"mntr\" class3=\"scnr\"/> \n\
       <xf:item> \n\
        <xf:label>Grau.icc</xf:label> \n\
        <xf:value>Grau.icc</xf:value> \n\
       </xf:item> \n\
       <xf:item> \n\
        <xf:label>Gray.icc</xf:label> \n\
        <xf:value>Gray.icc</xf:value> \n\
       </xf:item> \n\
      </xf:choices> \n\
     </xf:select1> \n\
    </td> \n\
   </tr> \n\
  </table> \n\
"
}; 

/** @instance oyra_api4
 *  @brief    oyra oyCMMapi4_s implementation
 *
 *  a filter providing default settings for ICC CMM's
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/13 (Oyranos: 0.1.8)
 *  @date    2008/11/13
 */
oyCMMapi4_s  oyra_api4_defaults_colour_settings = {

  oyOBJECT_CMM_API4_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api4_image_root, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */
  oyraFilter_CanHandle, /* oyCMMCanHandle_f */

  /* registration */
  OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH "colour_icc/oyra",

  {0,0,1}, /* int32_t version[3] */

  0, /* uint32_t * cache_data_types */

  oyraFilter_defaultICCValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyraWidgetEvent, /* oyWidgetEvent_f */

  0, /* oyCMMDataOpen_f */
  0, /* oyCMMFilterNode_CreateContext_f */
  0, /* oyCMMFilterNode_ContextToMem_f */
  0, /* oyCMMFilterNode_ContextFromMem_f */
  0, /* oyCMMFilterPlug_Run_f */

  {oyOBJECT_NAME_S, 0,0,0, "colour_icc", "Colour", "ICC static default options"}, /* name; translatable, eg "scale" "image scaling" "..." */
  "Colour/CMM/Oyranos", /* category : @todo better zero ? */
  oyra_default_colour_options,   /* options */
  oyra_default_colour_options_ui,   /* opts_ui_ */

  0,   /* plugs */
  0,   /* plugs_n */
  0,   /* plugs_last_add */
  0,   /* sockets */
  0,   /* sockets_n */
  0    /* sockets_last_add */
};


