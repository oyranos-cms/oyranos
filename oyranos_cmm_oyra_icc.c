/** @file oyranos_cmm_oyra_icc.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    meta backend for ICC support in Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/bsd-license.php>
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

/** Function oyra_defaultICCValidateOptions
 *  @brief   dummy
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/13 (Oyranos: 0.1.9)
 *  @date    2008/11/13
 */
oyOptions_s* oyra_defaultICCValidateOptions
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


/*
 <xf:model> <xf:instance> - must be added in Oyranos to make the model complete
 */
char oyra_default_colour_icc_options[] = {
 "\n\
  <" OY_TOP_SHARED ">\n\
   <" OY_DOMAIN_STD ">\n\
    <" OY_TYPE_STD ">\n\
     <profile>\n\
      <editing_rgb.front>eciRGB_v2.icc</editing_rgb.front>\n\
      <editing_cmyk.front>coated_FOGRA39L_argl.icc</editing_cmyk.front>\n\
      <editing_gray.front>Gray.icc</editing_gray.front>\n\
      <editing_lab.front>Lab.icc</editing_lab.front>\n\
      <editing_xyz.front>XYZ.icc</editing_xyz.front>\n\
     </profile>\n\
     <behaviour>\n\
      <action_untagged_assign.front>1</action_untagged_assign.front>\n\
      <action_missmatch_cmyk.front>1</action_missmatch_cmyk.front>\n\
      <action_missmatch_rgb.front>1</action_missmatch_rgb.front>\n\
      <mixed_colour_spaces_print_doc_convert.front>1</mixed_colour_spaces_print_doc_convert.front>\n\
      <mixed_colour_spaces_screen_doc_convert.front>2</mixed_colour_spaces_screen_doc_convert.front>\n\
      <proof_hard.advanced.front>0</proof_hard.advanced.front>\n\
      <proof_soft.advanced.front>0</proof_soft.advanced.front>\n\
      <rendering_intent>0</rendering_intent>\n\
      <rendering_bpc>1</rendering_bpc>\n\
      <rendering_intent_proof>0</rendering_intent_proof>\n\
      <rendering_gamut_warning.advanced>0</rendering_gamut_warning.advanced>\n\
     </behaviour>\n\
    </" OY_TYPE_STD ">\n\
   </" OY_DOMAIN_STD ">\n\
  </" OY_TOP_SHARED ">\n"
};

char oyra_default_colour_icc_options_ui[] = {
 "\
  <h3>Oyranos Default Profiles:</h3>\n\
  <table>\n\
   <tr>\n\
    <td>Editing Rgb:</td>\n\
    <td>\n\
     <xf:select1 ref=\"/" OY_TOP_SHARED "/" OY_DOMAIN_STD "/default/profile/editing_rgb\">\n\
      <xf:choices label=\"Editing RGB\">\n\
       <sta:profiles cspace1=\"RGB\" class1=\"prtr\" class2=\"mntr\" class3=\"scnr\"/>\n\
       <xf:item>\n\
        <xf:label>sRGB.icc</xf:label>\n\
        <xf:value>sRGB.icc</xf:value>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:label>eciRGB_v2.icc</xf:label>\n\
        <xf:value>eciRGB_v2.icc</xf:value>\n\
       </xf:item>\n\
      </xf:choices>\n\
     </xf:select1>\n\
    </td>\n\
   </tr>\n\
   <tr>\n\
    <td>Editing Cmyk:</td>\n\
    <td>\n\
     <xf:select1 ref=\"/" OY_TOP_SHARED "/" OY_DOMAIN_STD "/default/profile/editing_cmyk\">\n\
      <xf:choices>\n\
       <sta:profiles cspace1=\"CMYK\" class1=\"prtr\"/>\n\
       <xf:item>\n\
        <xf:label>coated_FOGRA39L_argl.icc</xf:label>\n\
        <xf:value>coated_FOGRA39L_argl.icc</xf:value>\n\
       </xf:item>\n\
      </xf:choices>\n\
     </xf:select1>\n\
    </td>\n\
   </tr>\n\
   <tr>\n\
    <td>Editing Lab:</td>\n\
    <td>\n\
     <xf:select1 ref=\"/" OY_TOP_SHARED "/" OY_DOMAIN_STD "/default/profile/editing_lab\">\n\
      <xf:choices xml:lang=\"en\" label=\"Editing Lab\">\n\
       <sta:profiles cspace1=\"Lab\" class1=\"prtr\" class2=\"mntr\" class3=\"scnr\"/>\n\
       <xf:item>\n\
        <xf:label>Lab.icc</xf:label>\n\
        <xf:value>Lab.icc</xf:value>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:label>CIELab.icc</xf:label>\n\
        <xf:value>CIELab.icc</xf:value>\n\
       </xf:item>\n\
      </xf:choices>\n\
     </xf:select1>\n\
    </td>\n\
   </tr>\n\
   <tr>\n\
    <td>Editing XYZ:</td>\n\
    <td>\n\
     <xf:select1 ref=\"/" OY_TOP_SHARED "/" OY_DOMAIN_STD "/" OY_TYPE_STD "/profile/editing_xyz\">\n\
      <xf:choices>\n\
       <sta:profiles cspace1=\"XYZ\" class1=\"prtr\" class2=\"mntr\" class3=\"scnr\"/>\n\
       <xf:item>\n\
        <xf:label>XYZ.icc</xf:label>\n\
        <xf:value>XYZ.icc</xf:value>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:label>CIEXYZ.icc</xf:label>\n\
        <xf:value>CIEXYZ.icc</xf:value>\n\
       </xf:item>\n\
      </xf:choices>\n\
     </xf:select1>\n\
    </td>\n\
   </tr>\n\
   <tr>\n\
    <td>Editing Gray:</td>\n\
    <td>\n\
     <xf:select1 ref=\"/" OY_TOP_SHARED "/" OY_DOMAIN_STD "/" OY_TYPE_STD "/profile/editing_gray\">\n\
      <xf:choices>\n\
       <sta:profiles cspace1=\"Gray\" class1=\"prtr\" class2=\"mntr\" class3=\"scnr\"/>\n\
       <xf:item>\n\
        <xf:label>Grau.icc</xf:label>\n\
        <xf:value>Grau.icc</xf:value>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:label>Gray.icc</xf:label>\n\
        <xf:value>Gray.icc</xf:value>\n\
       </xf:item>\n\
      </xf:choices>\n\
     </xf:select1>\n\
    </td>\n\
   </tr>\n\
  </table>\n\
"
}; 

const char * oyraICCDataNameGet(oyNAME_e type)
{
  if(type == oyNAME_NAME)
  return _("ICC profile");
  else if(type == oyNAME_DESCRIPTION)
  return _("ICC colour profile for colour transformations");
  else
  return "colour";
}

/** Function oyraICCDataLoadFromMem
 *  @brief   load a ICC profile from a in memory data blob
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/23 (Oyranos: 0.1.9)
 *  @date    2008/11/23
 */
oyStruct_s * oyraICCDataLoadFromMem  ( size_t              buf_size,
                                       const oyPointer     buf,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  return (oyStruct_s*) oyProfile_FromMem( buf_size, buf, flags, object );
}

/** Function oyraICCDataScan
 *  @brief   load ICC profile informations from a in memory data blob
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/23 (Oyranos: 0.1.9)
 *  @date    2008/11/23
 */
int          oyraICCDataScan         ( oyPointer           buf,
                                       size_t              buf_size,
                                       char             ** intern,
                                       char             ** filename,
                                       oyAlloc_f           allocateFunc )
{
  oyProfile_s * temp_prof = oyProfile_FromMem( buf_size, buf, 0, 0 );
  int error = !temp_prof;
  const char * internal = oyProfile_GetText( temp_prof, oyNAME_DESCRIPTION );
  const char * external = oyProfile_GetFileName( temp_prof, 0 );

  if(intern && internal)
    *intern = oyStringCopy_( internal, allocateFunc );

  if(filename && external)
    *filename = oyStringCopy_( external, allocateFunc );

  return error;
}

oyCMMDataTypes_s icc_data[] = {
 {
  oyOBJECT_CMM_API5_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  0, /* id */
  "color/icc", /* paths */
  "icc:icm", /* ext */
  oyraICCDataNameGet, /* oyCMMDataNameGet */
  oyraICCDataLoadFromMem, /* oyCMMDataLoadFromMem */
  oyraICCDataScan /* oyCMMDataScan */
 },{0} /* zero list end */
};

/** @instance oyra_api5
 *  @brief    oyra oyCMMapi5_s implementation
 *
 *  a interpreter for ICC CMM's
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/13 (Oyranos: 0.1.9)
 *  @date    2008/11/23
 */
oyCMMapi5_s  oyra_api5_colour_icc = {

  oyOBJECT_CMM_API5_S, /* oyStruct_s::type oyOBJECT_CMM_API4_S */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  (oyCMMapi_s*) & oyra_api4_image_root, /* oyCMMapi_s * next */
  
  oyraCMMInit, /* oyCMMInit_f */
  oyraCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */
  oyraFilter_CanHandle, /* oyCMMCanHandle_f */

  /* registration */
  OY_TOP_INTERNAL OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH "colour/oyra_meta",

  {0,0,1}, /* int32_t version[3] */

  0, /* paths */
  0, /* ext */

  0, /* oyCMMFilter_LoadFromMem_f */
  0, /* oyCMMFilter_Scan_f */

  oyra_defaultICCValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oyraWidgetEvent, /* oyWidgetEvent_f */

  oyra_default_colour_icc_options,   /* options */
  oyra_default_colour_icc_options_ui,   /* opts_ui_ */

  icc_data, /* data_types */
};


