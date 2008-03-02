/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2006  Kai-Uwe Behrmann
 *
 * Autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * -----------------------------------------------------------------------------
 *
 * pure text and string handling functions
 * 
 */

/* Date:      25. 11. 2004 */

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_cmms.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_sentinel.h"
#include "oyranos_xml.h"

/* --- Helpers  --- */

/* --- static variables   --- */

#define OY_STATIC_OPTS_  400

/* --- structs, typedefs, enums --- */

/* --- internal API definition --- */

void        oyGroupStringsTranslate_      ();
void        oyOptionStringsTranslate_     ();

/* separate from the external functions */

/** \addtogroup cmm_handling

 *  @{
 */

/** @brief \internal UI strings for various behaviour options 
 *
 *  This Text array is an internal only variable.<br>
 *  The content is  available through the oyGetOptionUITitle funcion.
 */
oyOption_t_ *oy_option_= NULL; 

oyGROUP oy_groups_descriptions_ = oyGROUP_ALL + 1;
const char ***oy_groups_description_ = NULL;

/** @} */

oyOPTION_TYPE
oyGetOptionType_(oyOPTION         type)
{
  const oyOption_t_ *opt = oyGetOption_(type);

  if( oyOPTION_BEHAVIOUR_START < type && type < oyOPTION_BEHAVIOUR_END )
  {
    if( opt->type != oyTYPE_BEHAVIOUR )
       WARN_S(("internal type: %d does not fit to option %d\n", oyGetOption_(type)->type , type));
  }

  return opt->type;
}

void
oyTexteCheck_              (void)
{
  oyGroupStringsTranslate_ ();
  oyOptionStringsTranslate_ ();
}

void
oyOptionStringsTranslate_ ()
{
  int pos;
  oyOption_t_ *opt;

  if(oy_option_)
    return;
  else
    oy_option_= calloc(sizeof(oyOption_t_), OY_STATIC_OPTS_);
  opt = oy_option_;

  {
#   define oySET_OPTIONS_M_( t_, id_, ca_n, ca1, ca2, ca3, labl, desc, \
                             ch_n, ch0, ch1, ch2, ch3, \
                             conf, xml) { \
      pos = id_; \
      opt[pos]. id = id_; \
      opt[pos]. type = t_; \
      opt[pos]. categories[0] = ca_n; \
      opt[pos]. categories[1] = ca1; \
      opt[pos]. categories[2] = ca2; \
      opt[pos]. categories[3] = ca3; \
      opt[pos]. label = labl; \
      opt[pos]. description = desc; \
      opt[pos]. choices = ch_n; \
      opt[pos]. choice_list[0] = ch0; \
      opt[pos]. choice_list[1] = ch1; \
      opt[pos]. choice_list[2] = ch2; \
      opt[pos]. choice_list[3] = ch3; \
/*      opt[pos]. choice_desc[0] = cd0; \
      opt[pos]. choice_desc[1] = cd1; \
      opt[pos]. choice_desc[2] = cd2; \
      opt[pos]. choice_desc[3] = cd3;*/ \
      opt[pos]. config_string = conf; \
      opt[pos]. config_string_xml = xml; \
    }

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyOPTION_EDITING_RGB, 1, 
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Editing Rgb"),
      _("Prefered Rgb Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_RGB_PROFILE,
      "oyEDITING_RGB")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyOPTION_EDITING_CMYK, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Editing Cmyk"),
      _("Prefered Cmyk Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_CMYK_PROFILE,
      "oyEDITING_CMYK")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyOPTION_EDITING_XYZ, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Editing XYZ"),
      _("Prefered XYZ Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_XYZ_PROFILE,
      "oyEDITING_XYZ")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyOPTION_EDITING_LAB, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Editing Lab"),
      _("Prefered CIE*Lab Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_LAB_PROFILE,
      "oyEDITING_LAB")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyOPTION_ASSUMED_XYZ, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed XYZ source"),
      _("Assigning an untagged XYZ Image an colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_XYZ_PROFILE,
      "oyASSUMED_CMYK")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyOPTION_ASSUMED_LAB, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed Lab source"),
      _("Assigning an untagged CIE*Lab Image an colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_LAB_PROFILE,
      "oyASSUMED_LAB")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyOPTION_ASSUMED_RGB, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed Rgb source"),
      _("Assigning an untagged Rgb Image an colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_RGB_PROFILE,
      "oyASSUMED_RGB")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyOPTION_ASSUMED_WEB, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed Web source"),
      _("Assigning an untagged Rgb Image with source from the WWW an colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_WEB_PROFILE,
      "oyASSUMED_WEB")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyOPTION_ASSUMED_CMYK, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed Cmyk source"),
      _("Assigning an untagged Cmyk Image this colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_CMYK_PROFILE,
      "oyASSUMED_CMYK" )

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyOPTION_PROFILE_PROOF, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("Proofing"),
      _("Colour space for Simulating an Output Device"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_PROOF_PROFILE,
      "oyPROFILE_PROOF" )


    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyOPTION_ACTION_UNTAGGED_ASSIGN, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("No Image profile"),
      _("Image has no colour space embedded. What default action shall be performed?"),
      3, /* choices */
      _("Assign No Profile"),_("Assign Assumed Profile"),_("Promt"), NULL,
      OY_ACTION_UNTAGGED_ASSIGN,
      "oyBEHAVIOUR_ACTION_UNTAGGED_ASSIGN" )

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyOPTION_ACTION_OPEN_MISMATCH_RGB, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("On Rgb Mismatch"),
      _("Action for Image profile and Editing profile mismatches."),
      3, /* choices */
      _("Preserve Numbers"),_("Convert automatically"),_("Promt"), NULL,
      OY_ACTION_MISMATCH_RGB,
      "oyBEHAVIOUR_ACTION_MISMATCH_RGB" )

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyOPTION_ACTION_OPEN_MISMATCH_CMYK, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("On Cmyk Mismatch"),
      _("Action for Image profile and Editing profile mismatches."),
      3, /* choices */
      _("Preserve Numbers"),_("Convert automatically"),_("Promt"), NULL,
      OY_ACTION_MISMATCH_CMYK,
      "oyBEHAVIOUR_ACTION_MISMATCH_CMYK")

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyOPTION_MIXED_MOD_DOCUMENTS_PRINT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, 0,
      _("For Print"),
      _("Handle Mixed colour spaces in Preparing a document for Print output."),
      4, /* choices */
      _("Preserve Numbers"),_("Convert to Default Cmyk Editing Space"),_("Convert to untagged Cmyk, preserving Cmyk numbers"),_("Promt"),
      OY_CONVERT_MIXED_COLOUR_SPACE_PRINT_DOCUMENT,
      "oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_PRINT")

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyOPTION_MIXED_MOD_DOCUMENTS_SCREEN, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, 0,
      _("For Screen"),
      _("Handle Mixed colour spaces in Preparing a document for Screen output."),
      4, /* choices */
      _("Preserve Numbers"),_("Convert to Default Rgb Editing Space"),_("Convert to WWW (sRGB)"),_("Promt"),
      OY_CONVERT_MIXED_COLOUR_SPACE_SCREEN_DOCUMENT,
      "oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_SCREEN")

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyOPTION_RENDERING_INTENT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_RENDERING, 0,
      _("Default Rendering Intent"),
      _("Rendering Intent for colour space Transformations."),
      4, /* choices */
      _("Perceptual"),_("Relative Colorimetric"),_("Saturation"),_("Absolute Colorimetric"),
      OY_DEFAULT_RENDERING_INTENT,
      "oyBEHAVIOUR_RENDERING_INTENT")

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyOPTION_RENDERING_BPC, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_RENDERING, 0,
      _("Use Black Point Compensation"),
      _("BPC affects only the Relative Colorimetric Rendering Intent."),
      2, /* choices */
      _("No"),_("Yes"), NULL, NULL,
      OY_DEFAULT_RENDERING_BPC,
      "oyBEHAVIOUR_RENDERING_BPC")

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyOPTION_RENDERING_INTENT_PROOF, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("Proofing Rendering Intent"),
      _("Behaviour of colour space transformation for proofing"),
      2, /* choices */
      _("Relative Colorimetric"),_("Absolute Colorimetric"),NULL,NULL,
      OY_DEFAULT_RENDERING_INTENT_PROOF,
      "oyBEHAVIOUR_RENDERING_INTENT_PROOF")

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyOPTION_PROOF_SOFT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("SoftProof by Default"),
      _("Behaviour for Softproofing view at application startup"),
      2, /* choices */
      _("No"),_("Yes"),NULL,NULL,
      OY_DEFAULT_PROOF_SOFT,
      "oyBEHAVIOUR_PROOF_SOFT")

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyOPTION_PROOF_HARD, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("Hardproof by Default"),
      _("Behaviour for preselecting Hardproofing with Standard Proofing Profile at print time"),
      2, /* choices */
      _("No"),_("Yes"),NULL,NULL,
      OY_DEFAULT_PROOF_HARD,
      "oyBEHAVIOUR_PROOF_HARD")

/*#   undef oySET_OPTIONS_M_*/
  }
}

void
oyGroupStringsTranslate_ ()
{
  oyGROUP group;

  if( oy_groups_description_ )
    return;
  else
    oy_groups_description_ = calloc(sizeof(char***), oy_groups_descriptions_*3);


  for( group = oyGROUP_START; group < oyGROUP_ALL + 1; ++group)
  {
  oy_groups_description_[group] = calloc(sizeof(char*), 3);
  switch( group )
  {
  case oyGROUP_START:
  oy_groups_description_[group][0] = "oyGROUP_START";
  oy_groups_description_[group][1] = _("Start");
  oy_groups_description_[group][2] = _("Oyranos Settings Group"); break;
  case oyGROUP_DEFAULT_PROFILES:
  oy_groups_description_[group][0] = "oyGROUP_DEFAULT_PROFILES";
  oy_groups_description_[group][1] = _("Default Profiles");
  oy_groups_description_[group][2] = _("Source and Target Profiles for various situations"); break;
  case oyGROUP_DEFAULT_PROFILES_EDIT:
  oy_groups_description_[group][0] = "oyGROUP_DEFAULT_PROFILES_EDIT";
  oy_groups_description_[group][1] = _("Editing Colour Space");
  oy_groups_description_[group][2] = _("Well behaving Colour Space for Editing");
 break;
  case oyGROUP_DEFAULT_PROFILES_ASSUMED:
  oy_groups_description_[group][0] = "oyGROUP_DEFAULT_PROFILES_ASSUMED";
  oy_groups_description_[group][1] = _("Assumed Colour Space");
  oy_groups_description_[group][2] = _("Assumed Colour Space for untagged colours"); break;
  case oyGROUP_DEFAULT_PROFILES_PROOF:
  oy_groups_description_[group][0] = "oyGROUP_DEFAULT_PROFILES_PROOF";
  oy_groups_description_[group][1] = _("Proofing");
  oy_groups_description_[group][2] = _("Colour Space for Simulating real devices"); break;
  case oyGROUP_PATHS:
  oy_groups_description_[group][0] = "oyGROUP_PATHS";
  oy_groups_description_[group][1] = _("Paths");
  oy_groups_description_[group][2] = _("Paths where ICC Profiles can be found"); break;
  case oyGROUP_POLICY:
  oy_groups_description_[group][0] = "oyGROUP_POLICY";
  oy_groups_description_[group][1] = _("Policy");
  oy_groups_description_[group][2] = _("Collections of settings in Oyranos"); break;
  case oyGROUP_BEHAVIOUR:
  oy_groups_description_[group][0] = "oyGROUP_BEHAVIOUR";
  oy_groups_description_[group][1] = _("Behaviour");
  oy_groups_description_[group][2] = _("Settings affecting the Behaviour in various situations"); break;
  case oyGROUP_BEHAVIOUR_RENDERING:
  oy_groups_description_[group][0] = "oyGROUP_BEHAVIOUR_RENDERING";
  oy_groups_description_[group][1] = _("Rendering");
  oy_groups_description_[group][2] = _("The kind of ICC standard gamut mapping for transforming colours between differently sized colour spaces"); break;
  case oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS:
  oy_groups_description_[group][0] = "oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS";
  oy_groups_description_[group][1] = _("Save Mixed colour space Documents");
  oy_groups_description_[group][2] = _("Default Handling of Mixed Colour Spaces inside one single Document"); break;
  case oyGROUP_BEHAVIOUR_MISSMATCH:
  oy_groups_description_[group][0] = "oyGROUP_BEHAVIOUR_MISSMATCH";
  oy_groups_description_[group][1] = _("Mismatching");
  oy_groups_description_[group][2] = _("Decide what to do when the default colour spaces dont match the current ones."); break;
  case oyGROUP_BEHAVIOUR_PROOF:
  oy_groups_description_[group][0] = "oyGROUP_BEHAVIOUR_PROOF";
  oy_groups_description_[group][1] = _("Proofing");
  oy_groups_description_[group][2] = _("Default Proofing Settings"); break;
  case oyGROUP_ALL:
  oy_groups_description_[group][0] = "oyGROUP_ALL";
  oy_groups_description_[group][1] = _("All");
  oy_groups_description_[group][2] = _("Oyranos Settings"); break;
  }
  }

}

const oyOption_t_*
oyGetOption_                        (oyOPTION          type)
{
  /* we provide allways a options at return to avoid further checks */
  static const oyOption_t_ default_opt;
  const oyOption_t_ *result = &default_opt;

  DBG_PROG_START
# if 0
  if( oyGetOptionType_( type ) == oyTYPE_BEHAVIOUR )
    pos = type - oyOPTION_BEHAVIOUR_START - 1;

  if( oyGetOptionType_( type ) == oyTYPE_DEFAULT_PROFILE )
    pos = type - oyOPTION_DEFAULT_PROFILE_START - 1 +
          oyOPTION_BEHAVIOUR_END - oyOPTION_BEHAVIOUR_START - 1;
# endif

  oyOptionStringsTranslate_ ();

  if(type < OY_STATIC_OPTS_)
    return &oy_option_[type];

  result = oyCmmsUIOptionSearch_( type );

  DBG_PROG_ENDE
  return result;
}

const char*
oyGetOptionUITitle_                    (oyOPTION          type,
                                        const oyGROUP   **categories,
                                        int              *choices,
                                        const char     ***choices_string_list,
                                        const char      **tooltip)
{
  DBG_PROG_START

  {
    if( choices )
    *choices              = oyGetOption_(type)-> choices;
    if( choices_string_list )
    *choices_string_list  = oyGetOption_(type)-> choice_list;
    if( categories )
    *categories           = oyGetOption_(type)-> categories;
    if( tooltip )
    *tooltip              = oyGetOption_(type)-> description;
    DBG_PROG_ENDE
    return oyGetOption_(type)-> label;
  }

  DBG_PROG_ENDE
  return NULL;
}

const char*
oyGetGroupUITitle_                     (oyGROUP          type,
                                        const char      **tooltip)
{
  int pos = type;

  DBG_PROG_START

  oyGroupStringsTranslate_( );

  if( pos >= 0 )
  {
    if( tooltip )
      *tooltip =  oy_groups_description_ [pos][2];
    DBG_S(("oy_groups_descriptions_: %d\n", oy_groups_descriptions_))
    DBG_PROG_ENDE
    return oy_groups_description_[pos][1];
  }

  DBG_PROG_ENDE
  return NULL;
}

oyGROUP
oyGroupAdd_                (const char *cmm,
                            const char *id,
                            const char *name,
                            const char *tooltips)
{
  const char ***ptr = calloc(sizeof(char***), ++oy_groups_descriptions_);
  const char **desc = calloc(sizeof(char**), 3);
  int i;

  DBG_PROG_START

  oyGroupStringsTranslate_ ();

  desc[0] = id;
  desc[1] = name;
  desc[2] = tooltips;

  for(i = 0; i < oy_groups_descriptions_ - 1; ++i)
      ptr[i] = oy_groups_description_[i]; 
  if(oy_groups_description_)
    free(oy_groups_description_);
  i = oy_groups_descriptions_ - 1; 
  ptr[i] = (const char**)desc;
  oy_groups_description_ = ptr;

  DBG_PROG_ENDE
  return oy_groups_descriptions_-1;
}

int
oyGroupGet_                (oyGROUP          type,
                            const char    ***strings)
{
  DBG_PROG_START

  *strings = oy_groups_description_[type];

  DBG_PROG_ENDE
  return 3;
}


int
oyTestInsideBehaviourOptions_ (oyBEHAVIOUR type, int choice)
{
  int r = 0;

  DBG_PROG_START

  DBG_PROG_S( ("type = %d behaviour %d", type, choice) )

  if ( oyGetOptionType_( type ) == oyTYPE_BEHAVIOUR )
  {
    if ( choice >= 0 &&
         choice < oyGetOption_(type)-> choices )
      r = 1;
    else
      WARN_S( ("%s:%d !!! ERROR type %d option %d does not exist for behaviour",__FILE__,__LINE__, type, choice));
  }
  else
    WARN_S( ("%s:%d !!! ERROR type %d type does not exist for behaviour",__FILE__,__LINE__, type));

  DBG_PROG_ENDE
  return r;
}


const char*
oyGetBehaviourUITitle_     (oyBEHAVIOUR       type,
                            int               choice,
                            int              *choices,
                            const char      **category,
                            const char      **option_string,
                            const char      **tooltip)
{ DBG_PROG_START
  if (choices) *choices = oyGetOption_(type)-> choices;

  if ( oyTestInsideBehaviourOptions_(type, choice) )
  { *option_string = oyGetOption_(type)-> choice_list[ choice ];
    *category = "API is broken";
    *tooltip =  oyGetOption_(type)-> description;
    DBG_PROG_ENDE
    return oyGetOption_(type)-> label;
  }
  DBG_PROG_ENDE
  return NULL;
}


const char *domain = "oyranos";


void
oyI18NSet_             ( int active,
                         int reserved )
{
  DBG_PROG_START

  if(active)
    domain = "oyranos";
  else
    domain = "";

  oyI18Nrefresh_();

  DBG_PROG_ENDE
}


void
oyI18Nrefresh_()
{
  int    i;


  if(oy_groups_description_ && oy_groups_descriptions_)
    for( i = 0; i < oy_groups_descriptions_ - 1; ++i)
      oyFree_m_( oy_groups_description_[i] );

  oyFree_m_( oy_groups_description_ );
  oyGroupStringsTranslate_ ();

  oyFree_m_( oy_option_ );
  oyOptionStringsTranslate_ ();


  /* refresh CMM's */
  oyCmmsRefreshI18N_();
}
  

