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
 */

/** @file @internal
 *  @brief pure text and string handling functions
 */

/* Date:      25. 11. 2004 */

#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_cmms.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_xml.h"

/* --- Helpers  --- */

/* --- static variables   --- */

/* --- structs, typedefs, enums --- */

/* --- internal API definition --- */

void        oyOptionStringsTranslate_     ();
void        oyOptionStringsTranslateCheck_();


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
/** @internal
 *  in the future groups may have more properties so we need a struct to hold
 *  them all ... frame group, tab group, advanced, hidden ...
 */
const char ***oy_groups_description_ = NULL;

/** @} */

oyWIDGET_TYPE
oyWidgetTypeGet_(oyWIDGET         type)
{
  const oyOption_t_ *opt = oyOptionGet_(type);
  return opt->type;
}

void
oyTextsTranslate_          (void)
{
  oyOptionStringsTranslate_ ();
}

void
oyTextsCheck_              (void)
{
  oyOptionStringsTranslateCheck_ ();
}

void
oyOptionStringsTranslateCheck_ ()
{
  if(oy_option_)
    return;

  oyOptionStringsTranslate_();
}

void
oyOptionStringsTranslate_ ()
{
  int pos;
  oyOption_t_ *opt = NULL;

  if(oy_option_)
    oyFree_m_(oy_option_);

  oyAllocHelper_m_( oy_option_, oyOption_t_, OY_STATIC_OPTS_, oyAllocateFunc_,);
  opt = oy_option_;

  {
#   define oySET_OPTIONS_M_( t_, id_, ca_n, ca1, ca2, ca3, labl, desc, \
                             ch_n, ch0, ch1, ch2, ch3, \
                             conf, xml, \
                             val, def_name) { \
      pos = id_; \
      opt[pos]. type = t_; \
      opt[pos]. id = id_; \
      opt[pos]. category[0] = ca_n; \
      opt[pos]. category[1] = ca1; \
      opt[pos]. category[2] = ca2; \
      opt[pos]. category[3] = ca3; \
      opt[pos]. flags = 0; \
      opt[pos]. name = labl; \
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
      opt[pos]. default_value = val; \
      opt[pos]. default_string = def_name; \
    }

    /* The following defaults corespond to the office.policy.xml */

    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_START, 0, 
      0, 0, 0,
      _("Start"),
      _("Oyranos Settings Group"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_START", 0,NULL)
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_DEFAULT_PROFILES, 0, 
      0, 0, 0,
      _("Default Profiles"),
      _("Source and Target Profiles for various situations"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES", 0, NULL)
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_DEFAULT_PROFILES_EDIT, 0, 
      0, 0, 0,
      _("Editing Colour Space"),
      _("Well behaving Colour Space for Editing"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES_EDIT", 0,0)
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_DEFAULT_PROFILES_ASSUMED, 0, 
      0, 0, 0,
      _("Assumed Colour Space"),
      _("Assumed Colour Space for untagged colours"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES_ASSUMED", 0,0)
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_DEFAULT_PROFILES_PROOF, 0, 
      0, 0, 0,
      _("Proofing Colour Space"),
      _("Colour Space for Simulating real devices"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES_PROOF", 0,0)
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_PATHS, 0, 
      0, 0, 0,
      _("Paths"),
      _("Paths where ICC Profiles can be found"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_PATHS", 0,0)
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_POLICY, 0, 
      0, 0, 0,
      _("Policy"),
      _("Collections of settings in Oyranos"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_POLICY", 0,0)
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_BEHAVIOUR, 0, 
      0, 0, 0,
      _("Behaviour"),
      _("Settings affecting the Behaviour in various situations"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR", 0,0)
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_BEHAVIOUR_RENDERING, 0, 
      0, 0, 0,
      _("Rendering"),
      _("The kind of ICC standard gamut mapping for transforming colours between differently sized colour spaces"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_RENDERING", 0,0)
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, 0, 
      0, 0, 0,
      _("Save Mixed colour space Documents"),
      _("Default Handling of Mixed Colour Spaces inside one single Document"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS", 0,0)
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_BEHAVIOUR_MISSMATCH, 0, 
      0, 0, 0,
      _("Mismatching"),
      _("Decide what to do when the default colour spaces dont match the current ones."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_MISSMATCH", 0,0)
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_BEHAVIOUR_PROOF, 0, 
      0, 0, 0,
      _("Proofing"),
      _("Default Proofing Settings"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_PROOF", 0,0)
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_ALL, 0, 
      0, 0, 0,
      _("All"),
      _("Oyranos Settings"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_ALL", 0,0)


    oySET_OPTIONS_M_( oyTYPE_CHOICE, oyWIDGET_POLICY, 1, 
      oyGROUP_POLICY, 0, 0,
      _("Policy"),
      _("Collections of settings in Oyranos"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyWIDGET_POLICY", 0,0)
    oy_option_[oyWIDGET_POLICY].flags = oyLAYOUT_MIDDLE;


    oySET_OPTIONS_M_( oyTYPE_LIST, oyWIDGET_PATHS, 1, 
      oyGROUP_PATHS, 0, 0,
      _("Paths"),
      _("Paths where ICC Profiles can be found"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyWIDGET_PATHS", 0,0)
    oy_option_[oyWIDGET_PATHS].flags = oyLAYOUT_PATH_SELECTOR;

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_RGB, 1, 
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Editing Rgb"),
      _("Prefered Rgb Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_RGB_PROFILE,
      "oyEDITING_RGB", 0,"sRGB.icc")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_CMYK, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Editing Cmyk"),
      _("Prefered Cmyk Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_CMYK_PROFILE,
      "oyEDITING_CMYK", 0,"ISOcoated.icc")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_XYZ, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Editing XYZ"),
      _("Prefered XYZ Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_XYZ_PROFILE,
      "oyEDITING_XYZ", 0,"XYZ.icc")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_LAB, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Editing Lab"),
      _("Prefered CIE*Lab Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_LAB_PROFILE,
      "oyEDITING_LAB", 0,"Lab.icc")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_GRAY, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Editing Gray"),
      _("Prefered Gray Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_GRAY_PROFILE,
      "oyEDITING_GRAY", 0,"Gray.icc")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_XYZ, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed XYZ source"),
      _("Assigning an untagged XYZ Image an colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_XYZ_PROFILE,
      "oyASSUMED_XYZ", 0,"XYZ.icc")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_LAB, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed Lab source"),
      _("Assigning an untagged CIE*Lab Image an colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_LAB_PROFILE,
      "oyASSUMED_LAB", 0,"Lab.icc")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_RGB, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed Rgb source"),
      _("Assigning an untagged Rgb Image an colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_RGB_PROFILE,
      "oyASSUMED_RGB", 0,"sRGB.icc")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_WEB, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed Web source"),
      _("Assigning an untagged Rgb Image with source from the WWW an colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_WEB_PROFILE,
      "oyASSUMED_WEB", 0,"sRGB.icc")
    oy_option_[oyWIDGET_ASSUMED_WEB].flags = oyLAYOUT_NO_CHOICES;

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_CMYK, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed Cmyk source"),
      _("Assigning an untagged Cmyk Image this colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_CMYK_PROFILE,
      "oyASSUMED_CMYK" , 0,"ISOcoated.icc")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_GRAY, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed Gray source"),
      _("Assigning an untagged Gray Image this colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_GRAY_PROFILE,
      "oyASSUMED_GRAY" , 0,"Gray.icc")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_PROFILE_PROOF, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("Proofing"),
      _("Colour space for Simulating an Output Device"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_PROOF_PROFILE,
      "oyPROFILE_PROOF" , 0,"ISOcoated.icc")


    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_ACTION_UNTAGGED_ASSIGN, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("No Image profile"),
      _("Image has no colour space embedded. What default action shall be performed?"),
      3, /* choices */
      _("Assign No Profile"),_("Assign Assumed Profile"),_("Promt"), NULL,
      OY_ACTION_UNTAGGED_ASSIGN,
      "oyBEHAVIOUR_ACTION_UNTAGGED_ASSIGN" , 1,0)

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_ACTION_OPEN_MISMATCH_RGB, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("On Rgb Mismatch"),
      _("Action for Image profile and Editing profile mismatches."),
      3, /* choices */
      _("Preserve Numbers"),_("Convert automatically"),_("Promt"), NULL,
      OY_ACTION_MISMATCH_RGB,
      "oyBEHAVIOUR_ACTION_MISMATCH_RGB" , 1,0)

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_ACTION_OPEN_MISMATCH_CMYK, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("On Cmyk Mismatch"),
      _("Action for Image profile and Editing profile mismatches."),
      3, /* choices */
      _("Preserve Numbers"),_("Convert automatically"),_("Promt"), NULL,
      OY_ACTION_MISMATCH_CMYK,
      "oyBEHAVIOUR_ACTION_MISMATCH_CMYK", 1,0)

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_MIXED_MOD_DOCUMENTS_PRINT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, 0,
      _("For Print"),
      _("Handle Mixed colour spaces in Preparing a document for Print output."),
      4, /* choices */
      _("Preserve Numbers"),_("Convert to Default Cmyk Editing Space"),_("Convert to untagged Cmyk, preserving Cmyk numbers"),_("Promt"),
      OY_CONVERT_MIXED_COLOUR_SPACE_PRINT_DOCUMENT,
      "oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_PRINT", 1,0)

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_MIXED_MOD_DOCUMENTS_SCREEN, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, 0,
      _("For Screen"),
      _("Handle Mixed colour spaces in Preparing a document for Screen output."),
      4, /* choices */
      _("Preserve Numbers"),_("Convert to Default Rgb Editing Space"),_("Convert to WWW (sRGB)"),_("Promt"),
      OY_CONVERT_MIXED_COLOUR_SPACE_SCREEN_DOCUMENT,
      "oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_SCREEN", 2,0)

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_RENDERING_INTENT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_RENDERING, 0,
      _("Default Rendering Intent"),
      _("Rendering Intent for colour space Transformations."),
      4, /* choices */
      _("Perceptual"),_("Relative Colorimetric"),_("Saturation"),_("Absolute Colorimetric"),
      OY_DEFAULT_RENDERING_INTENT,
      "oyBEHAVIOUR_RENDERING_INTENT", 1,0)

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_RENDERING_BPC, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_RENDERING, 0,
      _("Use Black Point Compensation"),
      _("BPC affects only the Relative Colorimetric Rendering Intent."),
      2, /* choices */
      _("No"),_("Yes"), NULL, NULL,
      OY_DEFAULT_RENDERING_BPC,
      "oyBEHAVIOUR_RENDERING_BPC", 1,0)

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_RENDERING_INTENT_PROOF, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("Proofing Rendering Intent"),
      _("Behaviour of colour space transformation for proofing"),
      2, /* choices */
      _("Relative Colorimetric"),_("Absolute Colorimetric"),NULL,NULL,
      OY_DEFAULT_RENDERING_INTENT_PROOF,
      "oyBEHAVIOUR_RENDERING_INTENT_PROOF", 0,0)

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_PROOF_SOFT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("SoftProof by Default"),
      _("Behaviour for Softproofing view at application startup"),
      2, /* choices */
      _("No"),_("Yes"),NULL,NULL,
      OY_DEFAULT_PROOF_SOFT,
      "oyBEHAVIOUR_PROOF_SOFT", 0,0)

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_PROOF_HARD, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("Hardproof by Default"),
      _("Behaviour for preselecting Hardproofing with Standard Proofing Profile at print time"),
      2, /* choices */
      _("No"),_("Yes"),NULL,NULL,
      OY_DEFAULT_PROOF_HARD,
      "oyBEHAVIOUR_PROOF_HARD", 0,0)

/*#   undef oySET_OPTIONS_M_*/
  }
}


const oyOption_t_*
oyOptionGet_                        (oyWIDGET          type)
{
  /* we provide allways a oyOption_t_ block at return to avoid further checks */
  static const oyOption_t_ default_opt;
  const oyOption_t_ *result = &default_opt;

  DBG_PROG_START

  oyOptionStringsTranslateCheck_ ();

  if(type < OY_STATIC_OPTS_)
  {
    DBG_PROG_ENDE
    return &oy_option_[type];
  }

  result = oyModulsUIOptionSearch_( type );

  DBG_PROG_ENDE
  return result;
}

oyWIDGET_TYPE oyWidgetTitleGet_        (oyWIDGET          type,
                                        const oyGROUP  ** categories,
                                        const char     ** name,
                                        const char     ** tooltip,
                                        int             * flags )
{
  const oyOption_t_ *t = oyOptionGet_(type);
  DBG_PROG_START

  {
    if( categories )
    *categories           = t->category;
    if( name )
    *name                 = t->name;
    if( tooltip )
    *tooltip              = t->description;
    if( flags )
    *flags                = t->flags;
  }

  DBG_PROG_ENDE
  return t->type;
}

char **
oyProfilePathsGet_    (int             * count,
                       oyAllocFunc_t     allocate_func)
{
  char ** path_names = NULL;
  int i,
      n = 0;
  int c = oyPathsCount_();

  oyAllocHelper_m_ (path_names, char*, c, allocate_func, return NULL);

  for( i = 0; i < c; ++i)
  {
    char * test = oyPathName_( i, oyAllocateFunc_ );
    char * checked = oyMakeFullFileDirName_( test );

    if( checked )
    {
      path_names[n] = allocate_func( oyStrblen_(checked) + 1 );
      oySprintf_( path_names[n], "%s", checked );
      oyFree_m_( checked );
      ++n;
    }
    oyFree_m_( test );
  }

  *count = n;
  return path_names;
}

const char**
oyConfigPathsGet_     (int             * count,
                       const char      * subdir )
{
  static char ** static_paths = NULL;
  static int ndp = 0;
  static int init = 0;

#define TestAndSetDefaultPATH( path_, subdir ) \
    oyAllocHelper_m_( static_paths[ndp], char, MAX_PATH, \
                      oyAllocateFunc_, return 0); \
    if(subdir) \
      snprintf( static_paths[ndp++], MAX_PATH, \
                "%s%s%s", path_, OY_SLASH, subdir ); \
    else \
      snprintf( static_paths[ndp++], MAX_PATH, \
                "%s", path_ ); \
    if(!oyIsDir_( static_paths[ndp-1] )) { \
      --ndp; \
    }

  /* the OpenICC agreed upon *nix default paths */
  if(!init)
  {
    init = 1;
    oyAllocHelper_m_( static_paths, char*, 3, oyAllocateFunc_, return 0);

    TestAndSetDefaultPATH( "/usr/share/color", subdir );
    TestAndSetDefaultPATH( OY_USERCOLORDIR, subdir );
    TestAndSetDefaultPATH( OY_SYSCOLORDIR, subdir );
  }

  *count = ndp;

  return (const char**) static_paths;
}

oyWIDGET*
oyPolicyWidgetListGet_( oyGROUP       group,
                     int         * count )
{
  oyWIDGET * list = NULL;
  int behaviour_count = 0,
      default_profiles_count = 0,
      n = 0;

  DBG_PROG_START

  /* which group is to save ? */
  switch (group)
  {
    case oyGROUP_DEFAULT_PROFILES:
           list = oyWidgetListGet_( oyGROUP_DEFAULT_PROFILES, &n,
                                             oyAllocateFunc_ );
         break;
    case oyGROUP_BEHAVIOUR_RENDERING:
           list = oyWidgetListGet_( oyGROUP_BEHAVIOUR_RENDERING,&n,
                                             oyAllocateFunc_ );
         break;
    case oyGROUP_BEHAVIOUR_PROOF:
           list = oyWidgetListGet_( oyGROUP_BEHAVIOUR_PROOF, &n,
                                             oyAllocateFunc_ );
         break;
    case oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS:
           list = oyWidgetListGet_( oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, &n,
                                             oyAllocateFunc_ );
         break;
    case oyGROUP_BEHAVIOUR_MISSMATCH:
           list = oyWidgetListGet_( oyGROUP_BEHAVIOUR_MISSMATCH, &n,
                                             oyAllocateFunc_ );
         break;
    case oyGROUP_ALL:
         /* travel through the group of settings and call the func itself */
         {
           int pos = 0, i;
           oyWIDGET *lw = NULL;
           oyWIDGET *tmp = NULL;

           oyAllocHelper_m_( lw, oyWIDGET, oyWIDGET_CMM_START,
                             oyAllocateFunc_, return 0);

           for(i = oyGROUP_START + 1; i < oyGROUP_ALL; ++i)
           {
             int j;
             tmp = oyPolicyWidgetListGet_( i, &n );
             for(j = 0; j < n; ++j)
               lw[pos++] = tmp[j];
             if(tmp)
               oyFree_m_( tmp );
           }

           n = pos;
           list = lw;
         }
         break;
    default:
         break;
  }

  *count = n;

  DBG_PROG_ENDE
  return list;
}

char*
oyPolicyNameGet_()
{
  int count = 0, i;
  oyString** policy_list = oyPolicyListGet_( &count );
  oyString *name = _("[none]");
  oyString *xml = NULL;

  DBG_PROG_START

  oyI18NSet(0,0);
  xml = oyPolicyToXML_ (oyGROUP_ALL, 0, oyAllocateFunc_);
  oyI18NSet(1,0);
  xml[oyStrlen_(xml)-2] = 0;

  for( i = 0; i < count; ++i )
  {
    oyString * data = NULL,
             * xml_g = NULL;   /* settings, generated from data file */
    size_t size = 0;

    const oyString* fname = policy_list[i];
    data = oyReadFileToMem_( fname, &size, oyAllocateFunc_ );

    // TODO: und als nächstes werden alle relevanten xml Schlüssel ausgelesen,
    // in der selben Form wie für oyPolicyToXML_
    {
      int   err = 0;
      int n = 0;
      oyWIDGET * list = NULL;
      const oyString * key = NULL;
      oyString       * value = NULL;

      list = oyPolicyWidgetListGet_( oyGROUP_ALL, &n );
      for(i = 0; i < n; ++i)
      {
        oyWIDGET oywid = list[i];
        oyWIDGET_TYPE opt_type = oyWidgetTypeGet_( oywid );

        if(opt_type == oyTYPE_DEFAULT_PROFILE)
        {
          const oyOption_t_ *t = oyOptionGet_( oywid );
          key = t->config_string_xml;

          /* read the value for the key */
          value = oyXMLgetValue_(xml, key);

          /* set the key */
          if(value && strlen(value))
          {
            err = oySetDefaultProfile_( oywid, value);
            if(err)
            {
               WARN_S(( "Could not set default profile %s:%s", t->name ,
                        value?value:"--" ));
            }
            oyFree_m_(value);
          }
        } else if(opt_type == oyTYPE_BEHAVIOUR)
        {
          const oyOption_t_ *t = oyOptionGet_( oywid );
          int val = -1;

          key = t->config_string_xml;

          /* read the value for the key */
          value = oyXMLgetValue_(xml, key);

          /* convert value from string to int */
          val = atoi(value);

          /* set the key */
          if( val != -1 && value )
            err = oySetBehaviour_(oywid, val);

          if(err)
          {
            WARN_S(( "Could not set behaviour %s:%s .", t->name ,
                      value?value:"--" ));
            return NULL;
          }

          if(value) oyFree_m_(value);
        }
      }

      if( list ) oyFree_m_( list );
    }

    if( !xml )
    {
      WARN_S(( "no policy data available??" ));
    }
    else if( !data )
    {
      WARN_S(( "no policy file available??" ));
    } else if( oyStrstr_( data, xml ) )
      name = policy_list[i];

    oyFree_m_( data );
  }
  oyFree_m_( xml );
  oyOptionChoicesFree_( oyWIDGET_POLICY, &policy_list, count );

  DBG_PROG_ENDE
  return name;
}

int         oyPolicySet_               (const oyString  * policy_file,
                                        const oyString  * full_name )
{
  int err = 0;
  oyString  * file_name = NULL;
  oyString  * xml = NULL;

  DBG_PROG_START

  oyAllocString_m_( file_name, MAX_PATH, oyAllocateFunc_, return 1 );
  if(full_name)
  {
    if(oyStrlen_( full_name ))
      oySnprintf_( file_name, MAX_PATH, "%s", full_name );
  }

  if( !oyStrlen_( file_name ) )
  {
    int count = 0, i;
    oyString ** policy_list = NULL;

    policy_list = oyPolicyListGet_( &count );

    for( i = 0; i < count; ++i )
    {
      if( oyStrstr_( policy_list[i], policy_file ) != 0 ||
          ( oyStrlen_( policy_file ) >= 3 &&
            oyStrstr_( policy_list[i], &policy_file[1] ) != 0 ) )
      {
        if( oyStrlen_( file_name ) )
        {
          WARN_S(( "ambiguous policy %s selection from policy identifier %s",
                   policy_list[i], policy_file ));
        }

        oySnprintf_( file_name, MAX_PATH, "%s", policy_list[i] );
      }
    }

    oyOptionChoicesFree_( oyWIDGET_POLICY, &policy_list, count );
  }

  {
    size_t size = 0;
    char * data = oyReadFileToMem_( file_name, &size, oyAllocateFunc_ );

    if(data && size)
    {
      xml = data;
      printf( "Opened file: %s", file_name );
    }
  }

  if(xml) {
    err = oyReadXMLPolicy_( oyGROUP_ALL, xml );
    oyFree_m_( xml );
  } else
    WARN_S(( "No policy file found: \"%s\"", file_name ));


  DBG_PROG_ENDE
  return err;
}

int           oyOptionChoicesGet_      (oyWIDGET          type,
                                        int             * choices,
                                        const oyString*** choices_string_list,
                                        int             * current)
{
  int error = 0;
  const oyOption_t_ *t = oyOptionGet_(type);

  DBG_PROG_START

  if( oyWIDGET_BEHAVIOUR_START < type && type < oyWIDGET_BEHAVIOUR_END )
  {
    if( choices )
      *choices              = t->choices;
    if( choices_string_list )
      *choices_string_list  = (const oyString**) t->choice_list;
    if( current )
      *current              = oyGetBehaviour_( (oyBEHAVIOUR) type );
  }
  else
  if( oyWIDGET_DEFAULT_PROFILE_START < type &&
      type < oyWIDGET_DEFAULT_PROFILE_END )
  {
    oyString * default_p =
           oyGetDefaultProfileName( (oyDEFAULT_PROFILE)type, oyAllocateFunc_);
    int i, val = -1, occurence = 0, count = 0;
    oyString** names = /*(const char**)*/ oyProfileListGet_ ( NULL, &count );
    oyString** dup = NULL;
    int dup_count = 0;

    oyAllocHelper_m_( dup, char*, count, oyAllocateFunc_, return 1 );

    {
      int dup_pos = 0;
      oyString * name = NULL;

      for (i = 0; i < count; ++i)
      {
        int k;
        int double_name = 0;

        name = names[i];
        for(k = 0; k < i; ++k)
          if(oyStrcmp_( name, names[k]) == 0 && 
             oyStrlen_( name ) )
          {
            double_name = 1;
            break;
          }

        if(!double_name)
        {
          oyAllocString_m_( dup[dup_pos], oyStrblen_(name), 
                            oyAllocateFunc_, return 1 );
          oySprintf_( dup[dup_pos], "%s", name);
          ++dup_pos;
        }

        if(default_p)
        if(oyStrstr_( name, default_p) &&
           oyStrlen_( name ) == oyStrlen_(default_p))
        {
          if(val >= 0) {
            ++occurence;
          } else {
            val = dup_pos-1;
            ++occurence;
          }
        }
        dup_count = dup_pos;
      }
      if(occurence > 1)
        WARN_S((_("multiple occurencies of default %s profile: %d times\n  Did you install multiple times?"),
                  default_p, occurence))
    }
    if( choices )
      *choices              = dup_count;

    if( choices_string_list )
      *choices_string_list = (const char **)dup; 
    else
      oyOptionChoicesFree_( t->type, &dup,
                            dup_count );
    oyOptionChoicesFree_( t->type, &names, count );

    if(current)
      *current          = val;
    oyFree_m_( default_p );
  }
  else
  if( type == oyWIDGET_POLICY )
  {
    int count = 0;
    oyString * currentP = oyPolicyNameGet_ ();
    oyString ** list = oyPolicyListGet_( &count );
    int c = -1, i;

    if( !list )
      return 1;

    if( currentP )
      for( i = 0; i < count; ++i )
      {
        if( oyStrcmp_( currentP, list[i] ) == 0 )
          c = i;
      }

    if( choices )
      *choices              = count;
    if( choices_string_list )
    {
      char ** zl = NULL;
      char * pn = NULL, * t; 

      oyAllocHelper_m_( zl, oyString*, count,
                        oyAllocateFunc_, return 0 );
      oyAllocString_m_( pn, MAX_PATH,
                        oyAllocateFunc_, return 0 );
      for( i = 0; i < count; ++i )
      {
        oyString * filename = list[i];

        sprintf( pn, filename );

        if(oyStrstr_(pn,"policy.xml"))
        {
          t = oyStrstr_(pn,".policy.xml");
          *t = 0;
        } else {
          t = oyStrrchr_(pn,'.');
          *t = 0;
        }

        oyAllocString_m_( zl[i], oyStrblen_(pn), oyAllocateFunc_, return 0 );

        oyStrcpy_(zl[i], oyStrrchr_(pn,OY_SLASH_C) + 1);
        zl[i][0] = oyToupper_(zl[i][0]);
      }
      oyFree_m_( pn );

      *choices_string_list  = (const char**) zl;
    }
    if( current )
      *current              = c;
  }
  else
  if( type == oyWIDGET_PATHS )
  {
    int count = 0;
    oyString ** list = oyProfilePathsGet_( &count, oyAllocateFunc_ );
    int c = -1;

    if( !list )
      return 1;

    if( choices )
      *choices              = count;
    if( choices_string_list )
      *choices_string_list  = (const oyString**) list;
    if( current )
      *current              = c;
  }

  DBG_PROG_ENDE
  return error;
}


void          oyOptionChoicesFree_     (oyWIDGET_TYPE     option,
                                        char          *** l,
                                        int               size)
{
  char *** list = (char***)l;

  DBG_PROG_START

  if( (oyWIDGET_DEFAULT_PROFILE_START < option &&
       option < oyWIDGET_DEFAULT_PROFILE_END ) ||
      (option == oyWIDGET_POLICY) )
  {
    size_t i;

    for(i = 0; i < size; ++i)
      oyFree_m_( (*list)[i] );
    oyFree_m_( *list );
    *list = NULL;
  }

  DBG_PROG_ENDE
}

oyWIDGET    * oyWidgetListGet_         (oyGROUP           group,
                                        int             * count,
                                        oyAllocFunc_t     allocate_func)
{
#define oyGROUP_DEFAULT_PROFILES_LEN oyWIDGET_DEFAULT_PROFILE_END - oyWIDGET_DEFAULT_PROFILE_START
#define oyGROUP_BEHAVIOUR_RENDERING_LEN 2
#define oyGROUP_BEHAVIOUR_PROOF_LEN 4
#define oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS_LEN 6
#define oyGROUP_BEHAVIOUR_MISSMATCH_LEN 6

#define oyGROUP_BEHAVIOUR_LEN oyGROUP_BEHAVIOUR_RENDERING_LEN + oyGROUP_BEHAVIOUR_PROOF_LEN + oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS_LEN + oyGROUP_BEHAVIOUR_MISSMATCH_LEN

#define oyGROUP_ALL_LEN oyGROUP_DEFAULT_PROFILES_LEN + oyGROUP_BEHAVIOUR_LEN

  oyWIDGET *w = NULL;
  oyWIDGET *lw = NULL;
  oyWIDGET *tmp = NULL;

  oyWIDGET oywid;
  int i,
      pos = 0,
      n = 0;

  DBG_PROG_START

  if(!count)
    return NULL;

  *count = 0;


  switch(group)
  {
     case oyGROUP_POLICY:
          {
            oyAllocHelper_m_( w, oyWIDGET , 1,
                              allocate_func, return NULL);
            for ( oywid = oyWIDGET_POLICY; oywid <= oyWIDGET_POLICY; ++oywid )
              w[pos++] = oywid;

           *count = pos;
          }
          break;
     case oyGROUP_PATHS:
          {
            oyAllocHelper_m_( w, oyWIDGET , 1,
                              allocate_func, return NULL);
            for ( oywid = oyWIDGET_PATHS; oywid <= oyWIDGET_PATHS; ++oywid )
              w[pos++] = oywid;

           *count = pos;
          }
          break;
     case oyGROUP_DEFAULT_PROFILES_EDIT:
          {
            oyAllocHelper_m_( w, oyWIDGET , oyEDITING_GRAY - oyEDITING_XYZ + 1,
                              allocate_func, return NULL);
            for ( oywid = oyEDITING_XYZ; oywid <= oyEDITING_GRAY; ++oywid )
              w[pos++] = oywid;

           *count = pos;
          }
          break;
     case oyGROUP_DEFAULT_PROFILES_ASSUMED:
          {
            oyAllocHelper_m_( w, oyWIDGET , oyASSUMED_GRAY - oyASSUMED_XYZ + 1,
                              allocate_func, return NULL);
            for ( oywid = oyASSUMED_XYZ; oywid <= oyASSUMED_GRAY; ++oywid )
              w[pos++] = oywid;

           *count = pos;
          }
          break;
     case oyGROUP_DEFAULT_PROFILES_PROOF:
          {
            oyAllocHelper_m_( w, oyWIDGET , 1,
                              allocate_func, return NULL);
            for ( oywid = oyPROFILE_PROOF; oywid <= oyPROFILE_PROOF; ++oywid )
              w[pos++] = oywid;

           *count = pos;
          }
          break;
     case oyGROUP_DEFAULT_PROFILES:
         {
           oyAllocHelper_m_( lw, oyWIDGET , oyGROUP_DEFAULT_PROFILES_LEN,
                             allocate_func, return NULL);
           tmp = oyWidgetListGet_( oyGROUP_DEFAULT_PROFILES_EDIT, &n,
                                   oyAllocateFunc_);
           for(i = 0; i < n; ++i)
             lw[pos++] = tmp[i];
           oyFree_m_( tmp );
           tmp = oyWidgetListGet_( oyGROUP_DEFAULT_PROFILES_ASSUMED, &n,
                                   oyAllocateFunc_);
           for(i = 0; i < n; ++i)
             lw[pos++] = tmp[i];
           oyFree_m_( tmp );

           *count = pos;
           w = lw;
         }
         break;
    case oyGROUP_BEHAVIOUR_RENDERING:
         {
           oyAllocHelper_m_( lw, oyWIDGET , oyGROUP_BEHAVIOUR_RENDERING_LEN,
                             allocate_func, return NULL);

           lw[pos++] = oyWIDGET_RENDERING_INTENT;
           lw[pos++] = oyWIDGET_RENDERING_BPC;

           *count = pos;
           w = lw;
         }
         break;
    case oyGROUP_BEHAVIOUR_PROOF:
         {
           oyAllocHelper_m_( lw, oyWIDGET , oyGROUP_BEHAVIOUR_PROOF_LEN,
                             allocate_func, return NULL);

           lw[pos++] = oyWIDGET_PROFILE_PROOF;
           lw[pos++] = oyWIDGET_RENDERING_INTENT_PROOF;
           lw[pos++] = oyWIDGET_PROOF_SOFT;
           lw[pos++] = oyWIDGET_PROOF_HARD;

           *count = pos;
           w = lw;
         }
         break;
    case oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS:
         {
           oyAllocHelper_m_( lw, oyWIDGET ,oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS_LEN ,
                             allocate_func, return NULL);

           lw[pos++] = oyWIDGET_MIXED_MOD_DOCUMENTS_SCREEN;
           lw[pos++] = oyWIDGET_MIXED_MOD_DOCUMENTS_PRINT;

           *count = pos;
           w = lw;
         }
         break;
    case oyGROUP_BEHAVIOUR_MISSMATCH:
         {
           oyAllocHelper_m_( lw, oyWIDGET, oyGROUP_BEHAVIOUR_MISSMATCH_LEN,
                             allocate_func, return NULL);

           lw[pos++] = oyWIDGET_ACTION_UNTAGGED_ASSIGN;
           lw[pos++] = oyWIDGET_ACTION_OPEN_MISMATCH_RGB;
           lw[pos++] = oyWIDGET_ACTION_OPEN_MISMATCH_CMYK;

           *count = pos;
           w = lw;
         }
         break;
    case oyGROUP_BEHAVIOUR:
         {
           oyAllocHelper_m_( lw, oyWIDGET, oyGROUP_BEHAVIOUR_LEN,
                             allocate_func, return NULL);
           tmp = oyWidgetListGet_( oyGROUP_BEHAVIOUR_RENDERING, &n,
                                   oyAllocateFunc_);
           for(i = 0; i < n; ++i)
             lw[pos++] = tmp[i];
           oyFree_m_( tmp );
           tmp = oyWidgetListGet_( oyGROUP_BEHAVIOUR_MISSMATCH, &n,
                                   oyAllocateFunc_);
           for(i = 0; i < n; ++i)
             lw[pos++] = tmp[i];
           oyFree_m_( tmp );
           tmp = oyWidgetListGet_( oyGROUP_BEHAVIOUR_PROOF, &n,
                                   oyAllocateFunc_);
           for(i = 0; i < n; ++i)
             lw[pos++] = tmp[i];
           oyFree_m_( tmp );
           tmp = oyWidgetListGet_( oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, &n,
                                   oyAllocateFunc_);
           for(i = 0; i < n; ++i)
             lw[pos++] = tmp[i];
           oyFree_m_( tmp );

           *count = pos;
           w = lw;
         }
         break;
    case oyGROUP_ALL:
         {
           oyAllocHelper_m_( lw, oyWIDGET, oyGROUP_ALL_LEN,
                             allocate_func, return NULL);

           tmp = oyWidgetListGet_( oyGROUP_POLICY, &n,
                                   oyAllocateFunc_);
           for(i = 0; i < n; ++i)
             lw[pos++] = tmp[i];
           oyFree_m_( tmp );

           tmp = oyWidgetListGet_( oyGROUP_PATHS, &n,
                                   oyAllocateFunc_);
           for(i = 0; i < n; ++i)
             lw[pos++] = tmp[i];
           oyFree_m_( tmp );

           tmp = oyWidgetListGet_( oyGROUP_DEFAULT_PROFILES, &n,
                                   oyAllocateFunc_);
           for(i = 0; i < n; ++i)
             lw[pos++] = tmp[i];
           oyFree_m_( tmp );

           tmp = oyWidgetListGet_( oyGROUP_BEHAVIOUR, &n,
                                   oyAllocateFunc_);
           for(i = 0; i < n; ++i)
             lw[pos++] = tmp[i];
           oyFree_m_( tmp );

           *count = pos;
           w = lw;
         }
         break;
    default:
         break;
  }

  DBG_PROG_ENDE
  return w;
}



oyGROUP
oyGroupAdd_                (const char *cmm,
                            const char *id,
                            const char *name,
                            const char *tooltips)
{
  const char ***ptr = NULL;
  const char **desc = NULL;
  int i;

  DBG_PROG_START
  ptr = calloc ( sizeof(char***), ++oy_groups_descriptions_ );
  desc = calloc( sizeof(char**), 3 );

  oyTextsCheck_ ();

  desc[0] = id;
  desc[1] = name;
  desc[2] = tooltips;

  for(i = 0; i < oy_groups_descriptions_ - 1; ++i)
      ptr[i] = oy_groups_description_[i]; 
  oyFree_m_(oy_groups_description_);
  i = oy_groups_descriptions_ - 1; 
  ptr[i] = (const char**)desc;
  oy_groups_description_ = ptr;

  DBG_PROG_ENDE
  return oy_groups_descriptions_-1;
}

int
oyTestInsideBehaviourOptions_ (oyBEHAVIOUR type, int choice)
{
  int r = 0;
  const oyOption_t_ *t = oyOptionGet_(type);
 
  DBG_PROG_START

  DBG_PROG_S( ("type = %d behaviour %d", type, choice) )

  if ( oyWidgetTypeGet_( type ) == oyTYPE_BEHAVIOUR ||
       oyWidgetTypeGet_( type ) == oyTYPE_CHOICE )
  {
    if ( choice >= 0 &&
         choice < t->choices )
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
{
  const oyOption_t_ *t = oyOptionGet_(type);

  DBG_PROG_START

  if (choices) *choices = t->choices;

  if ( oyTestInsideBehaviourOptions_(type, choice) )
  {
    *option_string = t->choice_list[ choice ];
    *category = "API is broken";
    *tooltip =  t->description;
    DBG_PROG_ENDE
    return t->name;
  }
  DBG_PROG_ENDE
  return NULL;
}

/** @internal returns the binary lenth of a string */
size_t oyStrblen_            (const char * start)
{
  char *end = NULL;

  if( start )
  {
    end = strchr( start, 0 );

    if( end )
      return end - start + 1;
  }

  return 0;
}

