/** @file oyranos_texts.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    pure text handling functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */

#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iconv.h>

#include "oyProfiles_s.h"

#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_xml.h"
#include "oyranos_string.h"

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
 *  The content is  available through the oyGetOptionUITitle function.
 */
oyOption_t_ *oy_option_= NULL; 

oyGROUP_e oy_groups_descriptions_ = oyGROUP_ALL + 1;
/** @internal
 *  in the future groups may have more properties so we need a struct to hold
 *  them all ... frame group, tab group, advanced, hidden ...
 */
const char ***oy_groups_description_ = NULL;

/** @} */

oyWIDGET_TYPE_e
oyWidgetTypeGet_(oyWIDGET_e         type)
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

  oyAllocHelper_m_( oy_option_, oyOption_t_, OY_STATIC_OPTS_,oyAllocateFunc_,;);
  opt = oy_option_;

  {
#   define oySET_OPTIONS_M_( t_, id_, ca_n, ca1, ca2, ca3, labl, tip, desc, \
                             ch_n, ch0, ch1, ch2, ch3, \
                             conf, xml, \
                             val, def_name) { \
      pos = (int)id_; \
      opt[pos]. type = t_; \
      opt[pos]. id = id_; \
      opt[pos]. category[0] = (oyGROUP_e)ca_n; \
      opt[pos]. category[1] = (oyGROUP_e)ca1; \
      opt[pos]. category[2] = (oyGROUP_e)ca2; \
      opt[pos]. category[3] = (oyGROUP_e)ca3; \
      opt[pos]. flags = 0; \
      opt[pos]. name = labl; \
      opt[pos]. tooltip = tip; \
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

    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_START, 0,
      0, 0, 0,
      _("Start"),                  /* name */
      _("Oyranos Settings Group"), /* tooltip */
      _("Oyranos settings let you configure ICC color management behaviour for a local computer. The settings can be per user and per system."), /* description */
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_START", 0,NULL)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_DEFAULT_PROFILES, 0,
      0, 0, 0,
      _("Default Profiles"),
      _("Source and Target Profiles for various situations"),
      _("ICC profiles should always be attached to all colors. In the case that colors are undefined, users can set various default ICC profiles in Oyranos settings."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES", 0, NULL)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_DEFAULT_PROFILES_EDIT, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Editing Color Space"),
      _("Well behaving Color Space for Editing"),
      _("The Editing Color Spaces shall be well behaving in order to allow editing of per channel curves. Well behaving means for Rgb, that editing of all three curves maintains gray neutrality. Hint: editing color spaces are sometimes called a working space."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES_EDIT", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_DEFAULT_PROFILES_ASSUMED, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed Color Space"),
      _("Assumed Color Space for untagged colors"),
      _("Color content can sometimes have no ICC profile assigned. This is a critical situation as the system can not properly convert these colors. Therefor the color processing parts need to pick some profile as a guess. These settings allow to change the picked ICC profile to guide the processing components and allow proper conversion and compositing."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES_ASSUMED", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_DEFAULT_PROFILES_PROOF, 0,
      0, 0, 0,
      _("Proofing Color Space"),
      _("Color Space for Simulating real devices"),
      _("The proofing color space represents a real color device for simulation. Possible uses cases are to simulate a print machine, a viewing environment in a theater or a expected small monitor gamut."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES_PROOF", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_PATHS, 0,
      0, 0, 0,
      _("Paths"),
      _("Paths where ICC Profiles can be found"),
      _("ICC profiles reside in known paths. They are defined by OpenICC and can be used by every complying system or application."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_PATHS", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_POLICY, 0,
      0, 0, 0,
      _("Policy"),
      _("Collections of settings in Oyranos"),
      _("Policies summarise common settings for different user groups. Such settings make it easy to switch back to simple defaults. Other groups can define own settings and share them accross Oyranos enabled systems."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_POLICY", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_BEHAVIOUR, 0,
      0, 0, 0,
      _("Behaviour"),
      _("Settings affecting the Behaviour in various situations"),
      _("Color management has in some situations to resolve unclear situations. This can be done by automatic applying preselected choices or on a case to case base. In unclear situations users can get alerted if they wish and perform manual control."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_BEHAVIOUR_RENDERING, 1,
      oyGROUP_BEHAVIOUR, 0, 0,
      _("Rendering"),
      _("The kind of ICC gamut mapping for transforming colors between differently sized color spaces"),
      _("The conversion between color spaces of different shape and size can happen in various ways. The Perceptual rendering intent is usual the best choice for photographs and artistic imagery. It is used in many automatic systems. The Relative Colorimetric rendering intent provides a well-defined standard, one-to-one color mapping, but without applying white point adaption. This can lead to color clipping in case of colors falling outside the target gamut as a price for the otherwise colorimetric correctness. The Relative Colorimetric intent is often used in combination with Black Point Compensation. The Saturation rendering intent shall provide an optimal use of saturated device colors. The Absolute Colorimetric rendering intent works like the relative colorimetric one except the white point is adapted. All rendering intents beside the colorimetric ones rely on the color tables designed by the profile vendor."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_RENDERING", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, 1,
      oyGROUP_BEHAVIOUR, 0, 0,
      _("Save Mixed Color Space Documents"),
      _("Handling of Mixed Color Spaces inside one single Document"),
      _("Mixed color spaces inside one single document can be difficult to handle for some systems. The settings can give the option to flatten document color spaces."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_BEHAVIOUR_MISSMATCH, 1, 
      oyGROUP_BEHAVIOUR, 0, 0,
      _("Mismatching"),
      _("Decide what to do when the default color spaces dont match the current ones."),
      _("Decide how to resolve conflicts with input color spaces and current settings."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_MISSMATCH", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_BEHAVIOUR_PROOF, 1,
      oyGROUP_BEHAVIOUR, 0, 0,
      _("Proofing"),
      _("Proofing Settings"),
      _("Proofing Settings allows one to decide about the simulation color space."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_PROOF", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_ALL, 0,
      0, 0, 0,
      _("All"),
      _("Oyranos Settings"),
      _("Oyranos Settings allow to fine tune handling of color management."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_ALL", 0,0)


    oySET_OPTIONS_M_( oyWIDGETTYPE_CHOICE, oyWIDGET_POLICY, 1,
      oyGROUP_POLICY, 0, 0,
      _("Policy"),
      _("Collections of settings in Oyranos"),
      _("Settings can be grouped in Oyranos policies for easier adaption to demands, better remembering and synchronisation."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyWIDGET_POLICY", 0,0)
    oy_option_[oyWIDGET_POLICY].flags = OY_LAYOUT_MIDDLE;


    oySET_OPTIONS_M_( oyWIDGETTYPE_LIST, oyWIDGET_PATHS, 1,
      oyGROUP_PATHS, 0, 0,
      _("Paths"),
      _("Paths where ICC Profiles can be found"),
      _("ICC profiles reside in known paths. They are defined by OpenICC and can be used by every complying system or application. XDG_DATA_HOME and XDG_DATA_DIRS environment variables route Oyranos to top directories containing resources. The derived paths for ICC profiles have a \"color/icc\" appended."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyWIDGET_PATHS", 0,0)
    oy_option_[oyWIDGET_PATHS].flags = OY_LAYOUT_PATH_SELECTOR;

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_RGB, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_EDIT, 0,
      _("Editing Rgb"),
      _("Prefered Rgb Editing Color Space"),
      _("The preferred Rgb Editing Color Space should represent a well behaving color space like sRGB."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_RGB_PROFILE,
      "oyEDITING_RGB", 0,"sRGB.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_CMYK, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_EDIT, 0,
      _("Editing Cmyk"),
      _("Prefered Cmyk Editing Color Space"),
      _("The preferred Cmyk Editing Color Space should represent a color space that complies to well defined printing conditions like FOGRA or SWOP."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_CMYK_PROFILE,
      "oyEDITING_CMYK", 0,"ISOcoated_v2_bas.ICC")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_XYZ, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_EDIT, 0,
      _("Editing XYZ"),
      _("Prefered XYZ Editing Color Space"),
      _("The preferred XYZ Editing Color Space shall decribe CIE*XYZ."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_XYZ_PROFILE,
      "oyEDITING_XYZ", 0,"XYZ.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_LAB, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_EDIT, 0,
      _("Editing Lab"),
      _("Prefered CIE*Lab Editing Color Space"),
      _("The preferred CIE*Lab Editing Color Space shall describe the CIE*Lab."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_LAB_PROFILE,
      "oyEDITING_LAB", 0,"Lab.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_GRAY, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_EDIT, 0,
      _("Editing Gray"),
      _("Prefered Gray Editing Color Space"),
      _("The preferred Gray Editing Color Space shall describe a single lighness channel color space for grayscale images."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_GRAY_PROFILE,
      "oyEDITING_GRAY", 0,"Gray-CIE_L.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_XYZ, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_ASSUMED, 0,
      _("Assumed XYZ source"),
      _("Assigns an untagged XYZ Image this color space"),
      _("The selected color space will be assigned to untagged XYZ color content and define colors for further processing."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_XYZ_PROFILE,
      "oyASSUMED_XYZ", 0,"XYZ.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_LAB, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_ASSUMED, 0,
      _("Assumed Lab source"),
      _("Assigns an untagged CIE*Lab Image this color space"),
      _("The selected color space will be assigned to untagged CIE*Lab color content and define colors for further processing."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_LAB_PROFILE,
      "oyASSUMED_LAB", 0,"Lab.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_RGB, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_ASSUMED, 0,
      _("Assumed Rgb source"),
      _("Assigns an untagged Rgb Image this color space"),
      _("The selected color space will be assigned to untagged Rgb color content and define colors for further processing. Typical sRGB should be assumed."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_RGB_PROFILE,
      "oyASSUMED_RGB", 0,"sRGB.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_WEB, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_ASSUMED, 0,
      _("Assumed Web source"),
      _("Assigns an untagged Rgb Image with source from the WWW this color space"),
      _("Assigns an untagged Rgb Image with source from the WWW this color space. This will always be sRGB as defined by W3C."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_WEB_PROFILE,
      "oyASSUMED_WEB", 0,"sRGB.icc")
    oy_option_[oyWIDGET_ASSUMED_WEB].flags = OY_LAYOUT_NO_CHOICES;

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_CMYK, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_ASSUMED, 0,
      _("Assumed Cmyk source"),
      _("Assigns an untagged Cmyk Image this color space"),
      _("The selected color space will be assigned to untagged Cmyk color content and define colors for further processing."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_CMYK_PROFILE,
      "oyASSUMED_CMYK" , 0,"ISOcoated_v2_bas.ICC")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_GRAY, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_ASSUMED, 0,
      _("Assumed Gray source"),
      _("Assigns an untagged Gray Image this color space"),
      _("The selected color space will be assigned to untagged Gray color content and define colors for further processing."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_GRAY_PROFILE,
      "oyASSUMED_GRAY" , 0,"Gray-CIE_L.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_PROFILE_PROOF, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("Proofing"),
      _("Color space for Simulating an Output Device"),
      _("Select a color profile representing a device to be simulated. This profile will only be applied if proofing is enabled."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_PROOF_PROFILE,
      "oyPROFILE_PROOF" , 0,"ISOcoated_v2_bas.ICC")


    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_ACTION_UNTAGGED_ASSIGN, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("No Image profile"),
      _("Image has no color space embedded. What default action shall be performed?"),
      _("This setting decides what to do in the case that colors have no color space assigned. Typical the according assumed ICC profile should be assigned."),
      3, /* choices */
      _("Assign No Profile"),_("Assign Assumed Profile"),_("Promt"), NULL,
      OY_ACTION_UNTAGGED_ASSIGN,
      "oyBEHAVIOUR_ACTION_UNTAGGED_ASSIGN" , 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_ACTION_OPEN_MISMATCH_RGB, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("On Rgb Mismatch"),
      _("Action for Image profile and Editing profile mismatches."),
      _("Incoming color spaces can differ from the default editing color space. For most users, an automatic conversion is fine. However, for more demanding work, the numbers need to be preserved."),
      3, /* choices */
      _("Preserve Numbers"),_("Convert automatically"),_("Promt"), NULL,
      OY_ACTION_MISMATCH_RGB,
      "oyBEHAVIOUR_ACTION_MISMATCH_RGB" , 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_ACTION_OPEN_MISMATCH_CMYK, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("On Cmyk Mismatch"),
      _("Action for Image profile and Editing profile mismatches."),
      _("Incoming color spaces can differ from the default editing color space. For most users, an automatic conversion is fine. However, for more demanding work, the numbers need to be preserved."),
      3, /* choices */
      _("Preserve Numbers"),_("Convert automatically"),_("Promt"), NULL,
      OY_ACTION_MISMATCH_CMYK,
      "oyBEHAVIOUR_ACTION_MISMATCH_CMYK", 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_MIXED_MOD_DOCUMENTS_PRINT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, 0,
      _("For Print"),
      _("Handle mixed color spaces in preparing a document for print output."),
      _("Handle mixed color spaces in preparing a document for print output. A conversion to the default editing color space is typically what most users want."),
      4, /* choices */
      _("Preserve Numbers"),_("Convert to Default Cmyk Editing Space"),_("Convert to untagged Cmyk, preserving Cmyk numbers"),_("Promt"),
      OY_CONVERT_MIXED_COLOUR_SPACE_PRINT_DOCUMENT,
      "oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_PRINT", 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_MIXED_MOD_DOCUMENTS_SCREEN, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, 0,
      _("For Screen"),
      _("Handle mixed color spaces in preparing a document for screen output."),
      _("Handle mixed color spaces in preparing a document for screen output. A conversion to sRGB helps in easy data exchange and is what most users want."),
      4, /* choices */
      _("Preserve Numbers"),_("Convert to Default Rgb Editing Space"),_("Convert to WWW (sRGB)"),_("Promt"),
      OY_CONVERT_MIXED_COLOUR_SPACE_SCREEN_DOCUMENT,
      "oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_SCREEN", 2,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_RENDERING_INTENT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_RENDERING, 0,
      _("Rendering Intent"),
      _("Rendering intent for color space transformations."),
      _("Rendering intent for color space transformations is typical the Relative Colorimetric intent plus Black Point Compensation or the Perceptual intent."),
      4, /* choices */
      _("Perceptual"),_("Relative Colorimetric"),_("Saturation"),_("Absolute Colorimetric"),
      OY_DEFAULT_RENDERING_INTENT,
      "oyBEHAVIOUR_RENDERING_INTENT", 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_RENDERING_BPC, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_RENDERING, 0,
      _("Use Black Point Compensation"),
      _("BPC affects often only the Relative Colorimetric Rendering intent."),
      _("BPC affects often only the Relative Colorimetric Rendering intent."),
      2, /* choices */
      _("No"),_("Yes"), NULL, NULL,
      OY_DEFAULT_RENDERING_BPC,
      "oyBEHAVIOUR_RENDERING_BPC", 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_RENDERING_INTENT_PROOF, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("Proofing Rendering Intent"),
      _("Behaviour of color space transformation for proofing"),
      _("Behaviour of color space transformation for proofing. Most people want a preview on screen only. The Relative Colorimetric intent is right for that. The Absolute Colorimetric intent needs a very careful profiling and non-trivial setup, but allowes for side-by-side comparisions."),
      2, /* choices */
      _("Relative Colorimetric"),_("Absolute Colorimetric"),NULL,NULL,
      OY_DEFAULT_RENDERING_INTENT_PROOF,
      "oyBEHAVIOUR_RENDERING_INTENT_PROOF", 0,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_PROOF_SOFT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("SoftProof"),
      _("Simulate the output print on the monitor"),
      _("Enable simulation of output print on the monitor. Most users do not work for a certain media and leave color handling to the system. They want no simulation."),
      2, /* choices */
      _("No"),_("Yes"),NULL,NULL,
      OY_DEFAULT_PROOF_SOFT,
      "oyBEHAVIOUR_PROOF_SOFT", 0,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_PROOF_HARD, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("Hardproof"),
      _("Behaviour for preselecting Hardproofing with a Proofing Profile at print time"),
      _("Enable simulation of an output color space on a local printer. Most users do not work for a certain media and leave color handling to the system. They want no simulation."),
      2, /* choices */
      _("No"),_("Yes"),NULL,NULL,
      OY_DEFAULT_PROOF_HARD,
      "oyBEHAVIOUR_PROOF_HARD", 0,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_RENDERING_GAMUT_WARNING, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_RENDERING, 0,
      _("Gamut Warning"),
      _("Mark Out Of Gamut colors"),
      _("Most users want a simple display of colors and will not check if colors match the simulation color space."),
      2, /* choices */
      _("No"),_("Yes"),NULL,NULL,
      OY_DEFAULT_RENDERING_GAMUT_WARNING,
      "oyBEHAVIOUR_RENDERING_GAMUT_WARNING", 1,0)

/*#   undef oySET_OPTIONS_M_*/
  }
}


const oyOption_t_*
oyOptionGet_                        (oyWIDGET_e          type)
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

  DBG_PROG_ENDE
  return result;
}

oyWIDGET_TYPE_e oyWidgetTitleGet_      (oyWIDGET_e        type,
                                        const oyGROUP_e** categories,
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
    *tooltip              = t->tooltip;
    if( flags )
    *flags                = t->flags;
  }

  DBG_PROG_ENDE
  return t->type;
}

oyWIDGET_TYPE_e  oyWidgetDescriptionGet_(
                                       oyWIDGET_e          type,
                                       const char       ** description,
                                       int                 choice )
{
  const oyOption_t_ *t = oyOptionGet_(type);
  DBG_PROG_START

  {
    if( description && choice == 0)
    *description          = t->description;
    if( description && choice > 0 )
    {
       int choices = 0, current = -1;
       const char ** choices_string_list = 0;

       oyOptionChoicesGet_( type, &choices, &choices_string_list, &current );
       if(choice <= choices)
         *description     = choices_string_list[choice-1];
    }
  }

  DBG_PROG_ENDE
  return t->type;
}



oyWIDGET_e*
oyPolicyWidgetListGet_( oyGROUP_e       group,
                     int         * count )
{
  oyWIDGET_e * list = NULL;
  /*int behaviour_count = 0,
      default_profiles_count = 0,*/
  int  n = 0;

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
           oyWIDGET_e *lw = NULL;
           oyWIDGET_e *tmp = NULL;

           oyAllocHelper_m_( lw, oyWIDGET_e, oyWIDGET_CMM_START,
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

/** @func    oyPoliciesEqual
 *  @brief   compare between two policy texts
 *
 *  Do the comparision according to policy typical (oyGROUP_ALL) xml keys.
 *
 *  @return                            -1 in case of an error, 0 for not matching keys, 1 for all keys, which are common in policyA and policyB are equal
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/07/23
 *  @since   2008/07/23 (Oyranos: 0.1.8)
 */
int                oyPoliciesEqual   ( const char        * policyA,
                                       const char        * policyB )
{
  int j;
  int n = 0;
  oyWIDGET_e * list = NULL;
  oyProfile_s * p_policyA = 0,
              * p_policyB = 0;
  int is_equal = 1;

  DBG_PROG_START

  list = oyPolicyWidgetListGet_( oyGROUP_ALL, &n );

  if( !policyA || !policyB )
    return -1;

    {
      const char * key = NULL;
      char       * value = NULL,
                 * value2 = NULL;

      for(j = 0; j < n && is_equal ; ++j)
      {
        oyWIDGET_e oywid = list[j];
        oyWIDGET_TYPE_e opt_type = oyWidgetTypeGet_( oywid );

        const oyOption_t_ *t = oyOptionGet_( oywid );

        key = t->config_string_xml;

        /* read the value for the key */
        value = oyXMLgetValue_(policyA, key);
        value2 = oyXMLgetValue_(policyB, key);

        if(opt_type == oyWIDGETTYPE_DEFAULT_PROFILE)
        {
          /* compare the keys */
          if(value && strlen(value) && value2 && strlen(value2))
          {
            p_policyA = oyProfile_FromFile( value, 0, 0);
            p_policyB = oyProfile_FromFile( value2, 0, 0);
            if(!oyProfile_Equal( p_policyA, p_policyB))
              is_equal = 0;
          }

        } else if(opt_type == oyWIDGETTYPE_BEHAVIOUR)
        {
          int val = -1,
              val2 = -1;

          /* convert value from string to int */
          if(value)
            val = atoi(value);
          if(value2)
            val2 = atoi(value2);

          /* compare the keys */
          if(value && val != val2)
            is_equal = 0;

        }

        if(value) oyFree_m_(value);
        if(value2) oyFree_m_(value2);

      }
    }

  if( list ) oyFree_m_( list );

  DBG_PROG_ENDE
  return is_equal;
}

/** @func    oyPolicyNameGet_
 *  @brief   get the name of a actual policy file
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/00/00
 *  @since   2008/07/23 (Oyranos: 0.1.x)
 */
char*
oyPolicyNameGet_()
{
  int count = 0, i;
  char** policy_list = oyPolicyListGet_( &count );
  char *name = 0;/*_("[none]");*/
  char *xml = NULL;

  DBG_PROG_START

  oyI18NSet(0,0);
  xml = oyPolicyToXML_ (oyGROUP_ALL, 0, oyAllocateFunc_);
  oyI18NSet(1,0);
  xml[oyStrlen_(xml)-2] = 0;

  if( !xml )
  {
    WARNc_S( "no policy data available??" );
    return name;
  }

  for( i = 0; i < count; ++i )
  {
    char * compare = NULL;   /* settings from policy file */
    size_t size = 0;
    const char * fname = policy_list[i];
    int is_policy = 1;

    compare = oyReadFileToMem_( fname, &size, oyAllocateFunc_ );

    if( !compare || !size)
    {
      WARNc1_S( "no policy file available?? %s", fname );
      is_policy = 0;
    }

    if(is_policy && oyPoliciesEqual( xml, compare ) == 1)
    {
      name = oyStringCopy_( policy_list[i], oyAllocateFunc_ );

      oyFree_m_( compare );
    }
  }

  oyFree_m_( xml );
  oyStringListRelease_( &policy_list, count, oyDeAllocateFunc_ );

  DBG_PROG_ENDE
  return name;
}

/** @internal
 *  @func    oyPolicyFileNameGet_
 *  @brief   resolve the file name of a policy
 *
 *  @param[in]     policy_name         the selected policy
 *  @param[out]    full_name           the full file name of policy_name
 *  @param[in]     allocateFunc        user allocator
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/02/28 (Oyranos: 0.1.10)
 *  @date    2010/02/28
 */
int        oyPolicyFileNameGet_      ( const char        * policy_name,
                                       char             ** full_name,
                                       oyAlloc_f           allocateFunc )
{
  char  * file_name = NULL;
  int error = !full_name || !allocateFunc || !policy_name;

  DBG_PROG_START

  if(!error)
  {
    int count = 0, i;
    oyChar ** policy_list = NULL;

    policy_list = oyPolicyListGet_( &count );

    for( i = 0; i < count; ++i )
    {
      if( oyStrstr_( policy_list[i], policy_name ) != 0 ||
          ( oyStrlen_( policy_name ) >= 3 &&
            oyStrstr_( policy_list[i], &policy_name[1] ) != 0 ) )
      {
        if( file_name )
        {
          WARNc2_S( "ambiguous policy %s selection from policy identifier %s",
                   policy_list[i], policy_name );
        }

        file_name = oyStringCopy_( policy_list[i], oyAllocateFunc_ );
      }
    }

    oyStringListRelease_( &policy_list, count, oyDeAllocateFunc_ );

    if(file_name)
    {
      *full_name = oyStringCopy_( file_name, allocateFunc );
      oyDeAllocateFunc_( file_name );
    }
  }

  DBG_PROG_ENDE
  return error;
}

int         oyPolicySet_               (const char    * policy_name,
                                        const char    * full_name )
{
  int err = 0;
  char  * file_name = NULL;
  char  * xml = NULL;

  DBG_PROG_START

  if(full_name)
    file_name = oyStringCopy_( full_name, oyAllocateFunc_ );
  else
    err = oyPolicyFileNameGet_( policy_name, &file_name, oyAllocateFunc_ );

  if(!err && file_name)
  {
    size_t size = 0;
    char * data = oyReadFileToMem_( file_name, &size, oyAllocateFunc_ );

    if(data && size)
    {
      xml = data;
      DBG_PROG1_S( "Opened file: %s", file_name );
    }
  }

  if(xml) {
    err = oyReadXMLPolicy_( oyGROUP_ALL, xml );
    oyFree_m_( xml );
  } else
    WARNc1_S( "No policy file found: \"%s\"",
              file_name ? file_name : (full_name ? full_name : 
              (policy_name ? policy_name : "????")) );


  DBG_PROG_ENDE
  return err;
}

char *       oyPolicyFileNameToDisplay(const char        * file_name )
{
  char * pn = NULL,
       * t = NULL,
       * text = NULL;
  if(!file_name)
    return pn;

  oyAllocString_m_( pn, oyStrlen_(file_name) + 1,
                        oyAllocateFunc_, return 0 );

  oySprintf_( pn, "%s", file_name );

  if(oyStrstr_(pn,"policy.xml"))
  {
    t = oyStrstr_(pn,".policy.xml");
    *t = 0;
  } else {
    t = oyStrrchr_(pn,'.');
    *t = 0;
  }

  oyAllocString_m_( text, oyStrblen_(pn), oyAllocateFunc_, return 0 );

  oyStrcpy_(text, oyStrrchr_(pn,OY_SLASH_C) + 1);
  text[0] = oyToupper_(text[0]);

  oyFree_m_( pn );

  return text;
}

int          oyPolicyFilesToDisplay  ( int               * choices,
                                       const char      *** choices_string_list,
                                       int               * current)
{
  int error = 0;

  {
    int count = 0;
    char * currentP = oyPolicyNameGet_ ();
    char ** list = oyPolicyListGet_( &count );
    int c = -1, i;

    if( !list )
      return 1;

    if( choices )
      *choices              = count;

    if(!count)
    {
      WARNc_S( "\n  No policy found. Installation incomplete or path missing?" );
    } else
    if( choices_string_list )
    {
      char ** zl = NULL;
      char ** sort = NULL;
      int default_policy[4] = {-1,-1,-1,-1}, pos;

      oyAllocHelper_m_( zl, oyChar*, count,
                        oyAllocateFunc_, return 0 );
      for( i = 0; i < count; ++i )
        zl[i] = oyPolicyFileNameToDisplay( list[i] );

      /* sort policies, the default ones first */
      for(i = 0; i < count; ++i)
      {
             if(strcmp( zl[i], "Office") == 0)
          default_policy[0] = i;
        else if(strcmp( zl[i], "Photographer") == 0)
          default_policy[1] = i;
        else if(strcmp( zl[i], "Designer") == 0)
          default_policy[2] = i;
        else if(strcmp( zl[i], "Prepress") == 0)
          default_policy[3] = i;
      }
      oyAllocHelper_m_( sort, char*, count+1, oyAllocateFunc_, return 0 );

      pos = 0;
      for(i = 0; i < 4; ++i)
        if(default_policy[i] >= 0)
          sort[pos++] = zl[default_policy[i]];

      for(i = 0; i < count; ++i)
      {
        if(i != default_policy[0] &&
           i != default_policy[1] &&
           i != default_policy[2] &&
           i != default_policy[3])
          sort[pos++] = zl[i];
      }

      if( currentP )
      {
        char * name = oyPolicyFileNameToDisplay(currentP);
        for( i = 0; i < count; ++i )
        {
          if( oyStrcmp_( name, sort[i] ) == 0 )
            c = i;
        }
        oyFree_m_( name );
      }

      oyDeAllocateFunc_(zl);
      zl = sort; sort = 0;


      *choices_string_list  = (const char**) zl;

    }
    if( current )
      *current              = c;

  }
  return error;
}

int          oyOptionChoicesGet_     ( oyWIDGET_e          type,
                                       int               * choices,
                                       const char      *** choices_string_list,
                                       int               * current)
{
  int error = 0;
  const oyOption_t_ *t = oyOptionGet_(type);

  DBG_PROG_START

  if( oyWIDGET_BEHAVIOUR_START < type && type < oyWIDGET_BEHAVIOUR_END )
  {
    if( choices )
      *choices              = t->choices;
    if( choices_string_list )
      *choices_string_list  = (const oyChar**) t->choice_list;
    if( current )
      *current              = oyGetBehaviour_( (oyBEHAVIOUR_e) type );
  }
  else
  if( oyWIDGET_DEFAULT_PROFILE_START < type &&
      type < oyWIDGET_DEFAULT_PROFILE_END )
  {
    int i = 0, n = 0, count = 0;
    char ** texts = 0,
          * text = 0,
          * temp = 0;
    oyProfiles_s * iccs = 0;
    oyProfile_s * temp_prof = 0;

    iccs = oyProfiles_ForStd( (oyPROFILE_e)type, current, 0 );
    n = oyProfiles_Count( iccs );

    for(i = 0; i < n; ++i)
    {
      temp_prof = oyProfiles_Get( iccs, i );
      text = oyStringCopy_( oyProfile_GetFileName(temp_prof, -1),
                            oyAllocateFunc_ );
      temp = oyStrrchr_( text, '/' );
      if(temp)
        ++temp;
      else
        temp = text;

      oyStringListAddStaticString_( &texts, &count, temp,
                                    oyAllocateFunc_, oyDeAllocateFunc_);

      oyProfile_Release( &temp_prof );
      oyDeAllocateFunc_( text );
    }
    if( choices )
      *choices              = count;

    if( choices_string_list )
      *choices_string_list = (const char **)texts; 
    else
      oyStringListRelease_( &texts, count, oyDeAllocateFunc_ );

    oyProfiles_Release( &iccs );
  }
  else
  if( type == oyWIDGET_POLICY )
  {
    error = oyPolicyFilesToDisplay( choices, choices_string_list, current );
  }
  else
  if( type == oyWIDGET_PATHS )
  {
    int count = 0;
    char ** list = oyProfilePathsGet_( &count, oyAllocateFunc_ );
    int c = -1;

    if( !list )
      return 1;

    if( choices )
      *choices              = count;
    if( choices_string_list )
      *choices_string_list  = (const oyChar**) list;
    if( current )
      *current              = c;
  }

  DBG_PROG_ENDE
  return error;
}


void          oyOptionChoicesFree_     (oyWIDGET_e        option,
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

oyWIDGET_e    * oyWidgetListGet_         (oyGROUP_e           group,
                                        int             * count,
                                        oyAlloc_f         allocate_func)
{
#define oyGROUP_DEFAULT_PROFILES_LEN oyWIDGET_DEFAULT_PROFILE_END - oyWIDGET_DEFAULT_PROFILE_START + 1
#define oyGROUP_BEHAVIOUR_RENDERING_LEN 4+1
#define oyGROUP_BEHAVIOUR_PROOF_LEN 4+1
#define oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS_LEN 6+1
#define oyGROUP_BEHAVIOUR_MISSMATCH_LEN 6+1

#define oyGROUP_BEHAVIOUR_LEN oyGROUP_BEHAVIOUR_RENDERING_LEN + oyGROUP_BEHAVIOUR_PROOF_LEN + oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS_LEN + oyGROUP_BEHAVIOUR_MISSMATCH_LEN + 1

#define oyGROUP_ALL_LEN oyGROUP_DEFAULT_PROFILES_LEN + oyGROUP_BEHAVIOUR_LEN

  oyWIDGET_e *w = NULL;
  oyWIDGET_e *lw = NULL;
  oyWIDGET_e *tmp = NULL;

  oyWIDGET_e oywid;
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
            oyAllocHelper_m_( w, oyWIDGET_e , 1 + 1,
                              allocate_func, return NULL);
            w[pos++] = oyWIDGET_GROUP_POLICY;
            for ( oywid = oyWIDGET_POLICY; oywid <= oyWIDGET_POLICY; ++oywid )
              w[pos++] = oywid;

           *count = pos;
          }
          break;
     case oyGROUP_PATHS:
          {
            oyAllocHelper_m_( w, oyWIDGET_e , 1 + 1,
                              allocate_func, return NULL);
            w[pos++] = oyWIDGET_GROUP_PATHS;
            for ( oywid = oyWIDGET_PATHS; oywid <= oyWIDGET_PATHS; ++oywid )
              w[pos++] = oywid;

           *count = pos;
          }
          break;
     case oyGROUP_DEFAULT_PROFILES_EDIT:
          {
            oyAllocHelper_m_( w, oyWIDGET_e , oyEDITING_GRAY - oyEDITING_XYZ + 1 + 1,
                              allocate_func, return NULL);
            w[pos++] = oyWIDGET_GROUP_DEFAULT_PROFILES_EDIT;
            for ( oywid = (oyWIDGET_e)oyEDITING_XYZ;
                    oywid <= (oyWIDGET_e)oyEDITING_GRAY; ++oywid )
              w[pos++] = oywid;

           *count = pos;
          }
          break;
     case oyGROUP_DEFAULT_PROFILES_ASSUMED:
          {
            oyAllocHelper_m_( w, oyWIDGET_e , oyASSUMED_GRAY - oyASSUMED_XYZ + 1 + 1,
                              allocate_func, return NULL);
            w[pos++] = oyWIDGET_GROUP_DEFAULT_PROFILES_ASSUMED;
            for ( oywid = (oyWIDGET_e)oyASSUMED_XYZ;
                    oywid <= (oyWIDGET_e)oyASSUMED_GRAY; ++oywid )
              w[pos++] = oywid;

           *count = pos;
          }
          break;
     case oyGROUP_DEFAULT_PROFILES_PROOF:
          {
            oyAllocHelper_m_( w, oyWIDGET_e , 1 + 1,
                              allocate_func, return NULL);
            w[pos++] = oyWIDGET_GROUP_DEFAULT_PROFILES_PROOF;
            for ( oywid = (oyWIDGET_e)oyPROFILE_PROOF;
                    oywid <= (oyWIDGET_e)oyPROFILE_PROOF; ++oywid )
              w[pos++] = oywid;

           *count = pos;
          }
          break;
     case oyGROUP_DEFAULT_PROFILES:
         {
           oyAllocHelper_m_( lw, oyWIDGET_e , oyGROUP_DEFAULT_PROFILES_LEN,
                             allocate_func, return NULL);
           tmp = oyWidgetListGet_( oyGROUP_DEFAULT_PROFILES_EDIT, &n,
                                   oyAllocateFunc_);
           lw[pos++] = oyWIDGET_GROUP_DEFAULT_PROFILES;
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
           oyAllocHelper_m_( lw, oyWIDGET_e , oyGROUP_BEHAVIOUR_RENDERING_LEN,
                             allocate_func, return NULL);

           lw[pos++] = oyWIDGET_GROUP_BEHAVIOUR_RENDERING;
           lw[pos++] = oyWIDGET_RENDERING_INTENT;
           lw[pos++] = oyWIDGET_RENDERING_BPC;
           lw[pos++] = oyWIDGET_RENDERING_GAMUT_WARNING;

           *count = pos;
           w = lw;
         }
         break;
    case oyGROUP_BEHAVIOUR_PROOF:
         {
           oyAllocHelper_m_( lw, oyWIDGET_e , oyGROUP_BEHAVIOUR_PROOF_LEN,
                             allocate_func, return NULL);

           lw[pos++] = oyWIDGET_GROUP_BEHAVIOUR_PROOF;
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
           oyAllocHelper_m_( lw, oyWIDGET_e ,oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS_LEN,
                             allocate_func, return NULL);

           lw[pos++] = oyWIDGET_GROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS;
           lw[pos++] = oyWIDGET_MIXED_MOD_DOCUMENTS_SCREEN;
           lw[pos++] = oyWIDGET_MIXED_MOD_DOCUMENTS_PRINT;

           *count = pos;
           w = lw;
         }
         break;
    case oyGROUP_BEHAVIOUR_MISSMATCH:
         {
           oyAllocHelper_m_( lw, oyWIDGET_e, oyGROUP_BEHAVIOUR_MISSMATCH_LEN,
                             allocate_func, return NULL);

           lw[pos++] = oyWIDGET_GROUP_BEHAVIOUR_MISSMATCH;
           lw[pos++] = oyWIDGET_ACTION_UNTAGGED_ASSIGN;
           lw[pos++] = oyWIDGET_ACTION_OPEN_MISMATCH_RGB;
           lw[pos++] = oyWIDGET_ACTION_OPEN_MISMATCH_CMYK;

           *count = pos;
           w = lw;
         }
         break;
    case oyGROUP_BEHAVIOUR:
         {
           oyAllocHelper_m_( lw, oyWIDGET_e, oyGROUP_BEHAVIOUR_LEN,
                             allocate_func, return NULL);
           tmp = oyWidgetListGet_( oyGROUP_BEHAVIOUR_RENDERING, &n,
                                   oyAllocateFunc_);
           lw[pos++] = oyWIDGET_GROUP_BEHAVIOUR;
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
           oyAllocHelper_m_( lw, oyWIDGET_e, oyGROUP_ALL_LEN,
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



oyGROUP_e
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

  if(oy_groups_description_)
  {
    for(i = 0; i < oy_groups_descriptions_ - 1; ++i)
      ptr[i] = oy_groups_description_[i]; 
    oyFree_m_(oy_groups_description_);
  }
  i = oy_groups_descriptions_ - 1; 
  ptr[i] = (const char**)desc;
  oy_groups_description_ = ptr;

  DBG_PROG_ENDE
  return oy_groups_descriptions_-1;
}

int
oyTestInsideBehaviourOptions_ (oyBEHAVIOUR_e type, int choice)
{
  int r = 0;
  const oyOption_t_ *t = oyOptionGet_((oyWIDGET_e)type);
 
  DBG_PROG_START

  DBG_PROG2_S( "type = %d behaviour %d", type, choice )

  if ( oyWidgetTypeGet_( (oyWIDGET_e)type ) == oyWIDGETTYPE_BEHAVIOUR ||
       oyWidgetTypeGet_( (oyWIDGET_e)type ) == oyWIDGETTYPE_CHOICE )
  {
    if ( choice >= 0 &&
         choice < t->choices )
      r = 1;
    else
      WARNc2_S( "type %d option %d does not exist for behaviour", type, choice);
  }
  else
    WARNc1_S( "type %d type does not exist for behaviour", type);

  DBG_PROG_ENDE
  return r;
}


const char*
oyGetBehaviourUITitle_     (oyBEHAVIOUR_e       type,
                            int               choice,
                            int              *choices,
                            const char      **category,
                            const char      **option_string,
                            const char      **tooltip)
{
  const oyOption_t_ *t = oyOptionGet_((oyWIDGET_e)type);

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

/* not  */
void
oyI18NSet_             ( int active,
                         int reserved )
{
  DBG_PROG_START

  if(active)
    oy_domain = OY_TEXTDOMAIN;
  else
    oy_domain = "";


  oyTextsTranslate_ ();

  DBG_PROG_ENDE
}

