/** @file oyranos_texts.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2017 (C) Kai-Uwe Behrmann
 *
 *  @brief    pure text handling functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2004/11/25
 */

#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "oyProfiles_s.h"
#include "oyCMMapi4_s.h"
#include "oyCMMapiFilter_s_.h"
#include "oyOptions_s_.h"

#include "oyranos_db.h"
#include "oyranos_cache.h"
#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_devices.h"
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

char *             oyGetCMMName_     ( oyCMMapiFilter_s  * cmm,
                                       oyCMM_e             type,
                                       int                 name_type,
                                       oyAlloc_f           allocate_func );


/* separate from the external functions */

/** \addtogroup defaults_apis
 *  @{ *//* defaults_apis */
/** \addtogroup cmm_handling

 *  @{ *//* cmm_handling */

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

/** @} *//* cmm_handling */
/** @} *//* defaults_apis */

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
      _("Color content can sometimes have no ICC profile assigned. This is a critical situation as the system can not properly convert these colors. Therefore the color processing parts need to pick some profile as a guess. These settings allow to change the picked ICC profile to guide the processing components and allow proper conversion and compositing."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES_ASSUMED", 0,0)
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
      _("The conversion between color spaces of different shape and size can happen in various ways. The Perceptual rendering intent is usually the best choice for photographs and artistic imagery. It is used in many automatic systems. The Relative Colorimetric rendering intent provides a well-defined standard, one-to-one color mapping, but without applying white point adaption. This can lead to color clipping in case of colors falling outside the target gamut as a price for the otherwise colorimetric correctness. The Relative Colorimetric intent is often used in combination with Black Point Compensation. The Saturation rendering intent shall provide an optimal use of saturated device colors. The Absolute Colorimetric rendering intent works like the relative colorimetric one except the white point is adapted. All rendering intents beside the colorimetric ones rely on the color tables designed by the profile vendor."),
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
      _("Decide what to do when the default color spaces don't match the current ones."),
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
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_BEHAVIOUR_EFFECT, 1,
      oyGROUP_BEHAVIOUR, 0, 0,
      _("Effect"),
      _("Effect Settings"),
      _("Effect Settings allows one to decide about a abstract effect color space."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_EFFECT", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_BEHAVIOUR_DISPLAY, 1,
      oyGROUP_BEHAVIOUR, 0, 0,
      _("Display"),
      _("Display Settings"),
      _("Display Settings enable handling of viewing conditions."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_DISPLAY", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_CMM, 0,
      0, 0, 0,
      _("CMM"),
      _("Select a Color Matching Module (CMM)"),
      _("A Color Matching Module (CMM) does the computational work to transform colors. It can provide alternative features regarding appearance, security, speed or resource overhead."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_CMM", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_ALL, 0,
      0, 0, 0,
      _("Settings"),
      _("Oyranos Settings"),
      _("Oyranos allows to fine tune handling of ICC color management."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_ALL", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_DEVICES, 0,
      0, 0, 0,
      _("Devices"),
      _("Color Managed Devices"),
      _("Set ICC profiles in Oyranos for device Color Management."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_ALL", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_DEVICES_PROFILES_TAXI, 0,
      0, 0, 0,
      _("Taxi DB Profiles"),
      _("Available Device Profiles from Taxi DB"),
      _("Show available Device Profiles from remote Taxi DB for a selected device. The metadata from Taxi DB and from the device must fit."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_ALL", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_TAXI_PROFILE_INSTALL, 0,
      0, 0, 0,
      _("Install profile"),
      _("Install selected profile"),
      _("Install the selected profile."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_ALL", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_DEVICES_PROFILES, 0,
      0, 0, 0,
      _("Available Device Profiles:"),
      _("Available Device Profiles:"),
      _("Available Device Profiles:"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_ALL", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_DEVICES_RELATED, 0,
      0, 0, 0,
      _("Device Related"),
      _("Show only device related ICC profiles"),
      _("Show only ICC profiles, which have metadata matching a given device."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_ALL", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_INFORMATION, 0,
      0, 0, 0,
      _("Information"),
      _("Profile Information"),
      _("Analyze ICC profile information on your system."),
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
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_POLICY_ACTIVE, 0,
      0, 0, 0,
      _("Active Policy:"),
      _("Active Policy:"),
      _("Show which policy matches by combining all involved settings."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_ALL", 0,0)



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
      _("Preferred Rgb Editing Color Space"),
      _("The preferred Rgb Editing Color Space should represent a well behaving color space like sRGB."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_RGB_PROFILE,
      "oyEDITING_RGB", 0,"sRGB.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_CMYK, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_EDIT, 0,
      _("Editing Cmyk"),
      _("Preferred Cmyk Editing Color Space"),
      _("The preferred Cmyk Editing Color Space should represent a color space that complies to well defined printing conditions like FOGRA or SWOP."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_CMYK_PROFILE,
      "oyEDITING_CMYK", 0,"ISOcoated_v2_bas.ICC")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_XYZ, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_EDIT, 0,
      _("Editing XYZ"),
      _("Preferred XYZ Editing Color Space"),
      _("The preferred XYZ Editing Color Space shall describe CIE*XYZ."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_XYZ_PROFILE,
      "oyEDITING_XYZ", 0,"LCMSXYZI.ICM")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_LAB, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_EDIT, 0,
      _("Editing Lab"),
      _("Preferred CIE*Lab Editing Color Space"),
      _("The preferred CIE*Lab Editing Color Space shall describe the CIE*Lab."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_LAB_PROFILE,
      "oyEDITING_LAB", 0,"LCMSLABI.ICM")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_GRAY, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_EDIT, 0,
      _("Editing Gray"),
      _("Preferred Gray Editing Color Space"),
      _("The preferred Gray Editing Color Space shall describe a single lightness channel color space for grayscale images."),
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
      "oyASSUMED_XYZ", 0,"LCMSXYZI.ICM")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_LAB, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_ASSUMED, 0,
      _("Assumed Lab source"),
      _("Assigns an untagged CIE*Lab Image this color space"),
      _("The selected color space will be assigned to untagged CIE*Lab color content and define colors for further processing."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_LAB_PROFILE,
      "oyASSUMED_LAB", 0,"LCMSLABI.ICM")

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

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_DISPLAY_WHITE_POINT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("Display White Point"),
      _("Display White Point for all screens"),
      _("Select a white point target for all monitors."),
      7, /* choices */
      /* order is none, automatic, static ones, first monitor, second monitor, ... , last monitor */
      _("No"), _("Automatic"), _("Illuminant D50"), _("Illuminant D55"),
      OY_DEFAULT_DISPLAY_WHITE_POINT,
      "oyDISPLAY_WHITE_POINT" , 0,0)
      opt[oyWIDGET_DISPLAY_WHITE_POINT].choice_list[4] = _("Illuminant D65");
      opt[oyWIDGET_DISPLAY_WHITE_POINT].choice_list[5] = _("Illuminant D75");
      opt[oyWIDGET_DISPLAY_WHITE_POINT].choice_list[6] = _("Illuminant D93");

    oySET_OPTIONS_M_( oyWIDGETTYPE_CHOICE, oyWIDGET_DISPLAY_WHITE_POINT_DAEMON, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("Display White Point Daemon"),
      _("Display White Point Daemon for setting white point depending on geographical location and time."),
      _("Select a daemon or set manually."),
      2, /* choices */
      /* order is none/manual or automatic/daemon */
      _("No"), "oyranos-monitor-white-point", NULL, NULL,
      OY_DEFAULT_DISPLAY_WHITE_POINT_DAEMON,
      "oyDISPLAY_WHITE_POINT_DAEMON" , 0,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_PROFILE_EFFECT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_EFFECT, 0,
      _("Effect"),
      _("Color space for showing a effect"),
      _("Select a color profile for adding a effect."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EFFECT_PROFILE,
      "oyPROFILE_EFFECT" , 0,0)


    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_ACTION_UNTAGGED_ASSIGN, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("No Image profile"),
      _("Image has no color space embedded. What default action shall be performed?"),
      _("This setting decides what to do in the case that colors have no color space assigned. Typically the according assumed ICC profile should be assigned."),
      3, /* choices */
      _("Assign No Profile"),_("Assign Assumed Profile"),_("Prompt"), NULL,
      OY_ACTION_UNTAGGED_ASSIGN,
      "oyBEHAVIOUR_ACTION_UNTAGGED_ASSIGN" , 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_ACTION_OPEN_MISMATCH_RGB, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("On Rgb Mismatch"),
      _("Action for Image profile and Editing profile mismatches."),
      _("Incoming color spaces can differ from the default editing color space. For most users, an automatic conversion is fine. However, for more demanding work, the numbers need to be preserved."),
      3, /* choices */
      _("Preserve Numbers"),_("Convert automatically"),_("Prompt"), NULL,
      OY_ACTION_MISMATCH_RGB,
      "oyBEHAVIOUR_ACTION_MISMATCH_RGB" , 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_ACTION_OPEN_MISMATCH_CMYK, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("On Cmyk Mismatch"),
      _("Action for Image profile and Editing profile mismatches."),
      _("Incoming color spaces can differ from the default editing color space. For most users, an automatic conversion is fine. However, for more demanding work, the numbers need to be preserved."),
      3, /* choices */
      _("Preserve Numbers"),_("Convert automatically"),_("Prompt"), NULL,
      OY_ACTION_MISMATCH_CMYK,
      "oyBEHAVIOUR_ACTION_MISMATCH_CMYK", 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_MIXED_MOD_DOCUMENTS_PRINT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, 0,
      _("For Print"),
      _("Handle mixed color spaces in preparing a document for print output."),
      _("Handle mixed color spaces in preparing a document for print output. A conversion to the default editing color space is typically what most users want."),
      4, /* choices */
      _("Preserve Numbers"),_("Convert to Default Cmyk Editing Space"),_("Convert to untagged Cmyk, preserving Cmyk numbers"),_("Prompt"),
      OY_CONVERT_MIXED_COLOR_SPACE_PRINT_DOCUMENT,
      "oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_PRINT", 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_MIXED_MOD_DOCUMENTS_SCREEN, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, 0,
      _("For Screen"),
      _("Handle mixed color spaces in preparing a document for screen output."),
      _("Handle mixed color spaces in preparing a document for screen output. A conversion to sRGB helps in easy data exchange and is what most users want."),
      4, /* choices */
      _("Preserve Numbers"),_("Convert to Default Rgb Editing Space"),_("Convert to WWW (sRGB)"),_("Prompt"),
      OY_CONVERT_MIXED_COLOR_SPACE_SCREEN_DOCUMENT,
      "oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_SCREEN", 2,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_RENDERING_INTENT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_RENDERING, 0,
      _("Rendering Intent"),
      _("Rendering intent for color space transformations."),
      _("Rendering intent for color space transformations is typically the Relative Colorimetric intent plus Black Point Compensation or the Perceptual intent."),
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
      _("Behaviour of color space transformation for proofing. Most people want a preview on screen only. The Relative Colorimetric intent is right for that. The Absolute Colorimetric intent needs a very careful profiling and non-trivial setup, but allows for side-by-side comparisons."),
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

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_EFFECT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_EFFECT, 0,
      _("Use Effect"),
      _("Enable effect profile"),
      _("Switch for enabling the effect profile."),
      2, /* choices */
      _("No"),_("Yes"),NULL,NULL,
      OY_DEFAULT_EFFECT,
      "oyBEHAVIOUR_EFFECT", 0,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_CHOICE, oyWIDGET_CMM_CONTEXT, 1,
      oyGROUP_CMM, 0, 0,
      _("CMM Core"),
      _("Select the core CMM"),
      _("The core Color Matching Module (CMM) takes individual profiles and options and backes a device link. It can provide alternative features regarding appearance, security, speed or resource overhead."),
      0, /* choices */
      NULL,NULL,NULL,NULL,
      OY_DEFAULT_CMM_CONTEXT,
      "oyWIDGET_CMM_CONTEXT", 0, "//" OY_TYPE_STD "/icc_color.lcm2")

    oySET_OPTIONS_M_( oyWIDGETTYPE_CHOICE, oyWIDGET_CMM_RENDERER, 1,
      oyGROUP_CMM, 0, 0,
      _("CMM Renderer"),
      _("Select the processing CMM"),
      _("The processing Color Matching Module (CMM) takes one device link as argument. It can provide alternative features regarding security, speed or resource overhead."),
      0, /* choices */
      NULL,NULL,NULL,NULL,
      OY_DEFAULT_CMM_RENDERER,
      "oyWIDGET_CMM_RENDERER", 0, "//" OY_TYPE_STD "/icc_color.lcm2")

    oySET_OPTIONS_M_( oyWIDGETTYPE_CHOICE, oyWIDGET_CMM_CONTEXT_FALLBACK, 1,
      oyGROUP_CMM, 0, 0,
      _("CMM Core Fallback"),
      _("Select the core CMM"),
      _("The core Color Matching Module (CMM) takes individual profiles and options and backes a device link. It can provide alternative features regarding appearance, security, speed or resource overhead."),
      0, /* choices */
      NULL,NULL,NULL,NULL,
      OY_DEFAULT_CMM_CONTEXT_FALLBACK,
      "oyWIDGET_CMM_CONTEXT_FALLBACK", 0, "//" OY_TYPE_STD "/icc_color.lcm2")

    oySET_OPTIONS_M_( oyWIDGETTYPE_CHOICE, oyWIDGET_CMM_RENDERER_FALLBACK, 1,
      oyGROUP_CMM, 0, 0,
      _("CMM Renderer Fallback"),
      _("Select the processing CMM"),
      _("The processing Color Matching Module (CMM) takes one device link as argument. It can provide alternative features regarding security, speed or resource overhead."),
      0, /* choices */
      NULL,NULL,NULL,NULL,
      OY_DEFAULT_CMM_RENDERER_FALLBACK,
      "oyWIDGET_CMM_RENDERER_FALLBACK", 0, "//" OY_TYPE_STD "/icc_color.lcm2")

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
    {
    *flags                = t->flags;
      if(type == oyWIDGET_DISPLAY_WHITE_POINT)
      {
        int active = 1;
        char * value =
              oyGetPersistentString(OY_DEFAULT_DISPLAY_WHITE_POINT_DAEMON, 0,
                                oySCOPE_USER_SYS, oyAllocateFunc_);
        if(!value || !value[0] || strcmp(value,"oyranos-monitor-white-point") != 0)
          active = 0;
        if(value)
        { oyFree_m_(value);
        }
        if(active)
          *flags |= OY_LAYOUT_INACTIVE;
      }
    }
  }

  DBG_PROG_ENDE
  return t->type;
}

oyWIDGET_TYPE_e  oyWidgetDescriptionGet_(
                                       oyWIDGET_e          type,
                                       uint32_t            flags,
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

       oyOptionChoicesGet_( type, flags, oyNAME_DESCRIPTION, &choices, &choices_string_list, &current );
       if(choice <= choices)
         *description     = choices_string_list[choice-1];
       oyOptionChoicesFree_( type, &choices_string_list, choices );
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
    case oyGROUP_BEHAVIOUR_EFFECT:
           list = oyWidgetListGet_( oyGROUP_BEHAVIOUR_EFFECT, &n,
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

/** Function oyPoliciesEqual
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

        /* skip temporary value, as it can easily be changed by
         * oyranos-monitor-white-point --daemon=1  */
        if(oywid == oyWIDGET_DISPLAY_WHITE_POINT)
          continue;

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

            oyProfile_Release( &p_policyA );
            oyProfile_Release( &p_policyB );
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

/** @internal
 *  Function oyPolicyNameGet_
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
  if( !xml )
  {
    WARNc_S( "no policy data available??" );
    return name;
  }

  xml[oyStrlen_(xml)-2] = 0;

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
 *  Function oyPolicyFileNameGet_
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
    char ** policy_list = NULL;

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

  if(oyStrstr_(pn,".policy.xml"))
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

      oyAllocHelper_m_( zl, char*, count,
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

/** \addtogroup defaults_apis
 *  @{ *//* defaults_apis */
/** \addtogroup cmm_handling

 *  @{ *//* cmm_handling */

/** Function oyGetCMMs
 *  @brief   Get a CMM list as strings
 *
 *  Useful for displaying UI strings.
 *
 *  @param         type                the CMM type to set
 *  @param         name_type           oyNAME_e or oyNAME_REGISTRATION or oyNAME_MODULE
 *  @param         flags               unused
 *  @param         allocate_func       user allocator
 *  @return                            a UI string
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/06/11
 *  @since   2014/06/11 (Oyranos: 0.9.6)
 */
char **            oyGetCMMs         ( oyCMM_e             type,
                                       int                 name_type,
                                       uint32_t            flags OY_UNUSED,
                                       oyAlloc_f           allocate_func )
{
  oyOBJECT_e otype = oyOBJECT_CMM_API4_S;
  char ** texts = NULL,
       ** cmms = NULL;
  int texts_n = 0;

  if(type == oyCMM_CONTEXT ||
     type == oyCMM_CONTEXT_FALLBACK)
    otype = oyOBJECT_CMM_API4_S;
  else
    otype = oyOBJECT_CMM_API7_S;

  {
    uint32_t * rank_list = 0;
    uint32_t apis_n = 0;
    oyCMMapiFilters_s * apis = oyCMMsGetFilterApis_( "///icc_color",
                                                     otype, 0,
                                                     &rank_list, &apis_n );
    int n = oyCMMapiFilters_Count( apis ), i;
    for(i = 0; i < n; ++i)
    {
      oyCMMapiFilter_s_ * f = (oyCMMapiFilter_s_*) oyCMMapiFilters_Get( apis, i );
      if(f)
      {
        char * t = oyGetCMMName_( (oyCMMapiFilter_s *) f, type, name_type, allocate_func );

        oyStringListAddStaticString( &texts, &texts_n, t,
                                      oyAllocateFunc_, oyDeAllocateFunc_);

        if(oy_debug > 1)
        fprintf( stderr,  "[%s]:\t\"%s\"\t%s \"%s\"\n",
                            oyStructTypeToText(f->type_),
                            f->registration,
                            f->id_,
                            t );

        oyFree_m_( t );
        if(f->release)
          f->release( (oyStruct_s**)&f );

      } else
        WARNc1_S( "      no api obtained %d",i);
    }
    oyCMMapiFilters_Release( &apis );
  }

  cmms = oyStringListAppend_( 0, 0, (const char**)texts, texts_n, &texts_n,
                              allocate_func ? allocate_func : oyAllocateFunc_ );
  oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );

  return cmms;
}

/** @} *//* cmm_handling */
/** @} *//* defaults_apis */

oyConfigs_s *      oyGetMonitors     ( oyOptions_s      ** options )
{
  oyConfigs_s * devices = oy_monitors_cache_;
  if(devices) return devices;
  /* get XCM_ICC_COLOR_SERVER_TARGET_PROFILE_IN_X_BASE */
  oyOptions_SetFromString( options,
              "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
              "yes", OY_CREATE_NEW );
  oyOptions_SetFromString( options, "//" OY_TYPE_STD "/config/command",
                                  "properties", OY_CREATE_NEW );
  oyDevicesGet( 0, "monitor._native", *options, &devices );
  oy_monitors_cache_ = devices;

  return devices;
}

int          oyOptionChoicesGetWtPtN_( )
{
  /* order is none, automatic, static ones, first monitor, second monitor, ... , last monitor */
  int choice_list_n = 1+1+5;
  oyOptions_s * options = 0;
  oyConfigs_s * devices = oyGetMonitors( &options );
  int devices_n = oyConfigs_Count( devices );
  choice_list_n += devices_n;

  oyOptions_Release( &options );

  return choice_list_n;
}

char **      oyOptionChoicesGetWtPt_ ( int               * choices )
{
  const oyOption_t_ *t = oyOptionGet_(oyWIDGET_DISPLAY_WHITE_POINT);

  /* order is none, automatic, static ones, first monitor, second monitor, ... , last monitor */
  int choice_list_n = 1+1+5,
      i;
  char ** list;

  oyOptions_s * options = 0;
  oyConfigs_s * devices = oyGetMonitors( &options );
  int devices_n = oyConfigs_Count( devices );

  choice_list_n = 1+1+5;
  list = (char**) calloc( sizeof(char*), choice_list_n + devices_n + 1 );
  for(i = 0; i < t->choices; ++i)
    list[i] = oyStringCopy( t->choice_list[i], 0 );

  choice_list_n += devices_n;
  for(i = 0; i < devices_n; ++i)
  {
    oyConfig_s * c = oyConfigs_Get( devices, i );
    char * text = NULL;
    const char * mnft = oyConfig_FindString( c, "manufacturer", 0 );
    const char * model = oyConfig_FindString( c, "model", 0 );
    oyDeviceGetInfo( c, oyNAME_NAME, options, &text, oyAllocateFunc_ );
    oyStringAddPrintf( &list[t->choices + i], 0,0, "%s %s %s",
                       oyNoEmptyString_m_(mnft), oyNoEmptyString_m_(model),
                       oyNoEmptyString_m_(text) );
    oyDeAllocateFunc_(text);
    oyConfig_Release( &c );
  }

  oyOptions_Release( &options );

  if( choices )
    *choices              = choice_list_n;

  return list;
}

void         oyOptionChoicesGetWtPtD_( int               * choices,
                                       char            *** choices_string_list,
                                       int               * current )
{
  const oyOption_t_ *t = oyOptionGet_(oyWIDGET_DISPLAY_WHITE_POINT_DAEMON);
  int choice_list_n = t->choices;
  int count = 0;
  char ** texts = NULL;
  char * value = 
      oyGetPersistentString( OY_DEFAULT_DISPLAY_WHITE_POINT_DAEMON, 0,
		             oySCOPE_USER_SYS, oyAllocateFunc_);

  oyStringListAddStaticString( &texts, &count, t->choice_list[0],
                               oyAllocateFunc_, oyDeAllocateFunc_);
  if(value && value[0])
    oyStringListAddStaticString( &texts, &count, value,
                                 oyAllocateFunc_, oyDeAllocateFunc_);
  else
    oyStringListAddStaticString( &texts, &count, t->choice_list[1],
                                 oyAllocateFunc_, oyDeAllocateFunc_);

  if( choices )
    *choices              = choice_list_n;
  if( choices_string_list )
    *choices_string_list  = texts; 
  if( current )
  {
    if(value == NULL || !value[0])
    *current              = 0;
    else
    *current              = 1;
  }
 
  if(value)
    oyFree_m_(value)
}

int          oyOptionChoicesGet_     ( oyWIDGET_e          type,
                                       uint32_t            flags,
                                       int                 name_type,
                                       int               * choices,
                                       const char      *** choices_string_list,
                                       int               * current)
{
  int error = 0;
  const oyOption_t_ *t = oyOptionGet_(type);

  DBG_PROG_START

  if( oyWIDGET_BEHAVIOUR_START < type && type < oyWIDGET_BEHAVIOUR_END )
  {
    int choice_list_n = t->choices;
    const char ** choice_list = (const char**) t->choice_list;

    /* create dynamic choices */
    if(type == oyWIDGET_DISPLAY_WHITE_POINT)
      choice_list = (const char**)oyOptionChoicesGetWtPt_( &choice_list_n );
    else if(type == oyWIDGET_DISPLAY_WHITE_POINT_DAEMON)
      oyOptionChoicesGetWtPtD_( &choice_list_n, (char ***)&choice_list,
                                current );

    if( choices )
      *choices              = choice_list_n;
    if( choices_string_list )
      *choices_string_list  = (const char**) choice_list;
    else
      oyOptionChoicesFree_( type, &choice_list, choice_list_n );
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

    iccs = oyProfiles_ForStd( (oyPROFILE_e)type, flags, current, 0 );
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

      oyStringListAddStaticString( &texts, &count, temp,
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
      *choices_string_list  = (const char**) list;
    if( current )
      *current              = c;
  }
  else
  if( type == oyWIDGET_CMM_CONTEXT ||
      type == oyWIDGET_CMM_RENDERER)
  {
    int count = 0;
    char ** list = oyGetCMMs( (oyCMM_e)type, name_type, flags, oyAllocateFunc_ );
    int c = -1;
    char * cu = oyGetCMMPattern( (oyCMM_e)type, 0, oyAllocateFunc_ );

    if( !list )
      return 1;

    while(list[count])
    {
      char * t = oyCMMNameToRegistration( list[count], (oyCMM_e)type, name_type, 0,
                                          oyAllocateFunc_ );
      if(oyFilterRegistrationMatch( t, cu, 0))
        c = count;
      oyFree_m_( t );

      ++count;
    }

    if( choices )
      *choices              = count;
    if( choices_string_list )
      *choices_string_list  = (const char**) list;
    if( current )
      *current              = c;
  }

  DBG_PROG_ENDE
  return error;
}


void          oyOptionChoicesFree_     (oyWIDGET_e        option,
                                        const char    *** l,
                                        int               size)
{
  char *** list = (char***)l;

  DBG_PROG_START

  if( (oyWIDGET_DEFAULT_PROFILE_START < option &&
       option < oyWIDGET_DEFAULT_PROFILE_END ) ||
      option == oyWIDGET_POLICY ||
      option == oyWIDGET_DISPLAY_WHITE_POINT ||
      option == oyWIDGET_DISPLAY_WHITE_POINT_DAEMON )
  {
    size_t i;

    for(i = 0; i < (size_t)size; ++i)
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
#define oyGROUP_BEHAVIOUR_PROOF_LEN 6+1
#define oyGROUP_BEHAVIOUR_EFFECT_LEN 2+1
#define oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS_LEN 6+1
#define oyGROUP_BEHAVIOUR_MISSMATCH_LEN 6+1
#define oyGROUP_CMM_LEN 2+1

#define oyGROUP_BEHAVIOUR_LEN oyGROUP_BEHAVIOUR_RENDERING_LEN + oyGROUP_BEHAVIOUR_PROOF_LEN + oyGROUP_BEHAVIOUR_EFFECT_LEN + oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS_LEN + oyGROUP_BEHAVIOUR_MISSMATCH_LEN + 1

#define oyGROUP_ALL_LEN oyGROUP_DEFAULT_PROFILES_LEN + oyGROUP_BEHAVIOUR_LEN + oyGROUP_CMM_LEN

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
           lw[pos++] = oyWIDGET_DISPLAY_WHITE_POINT;
           lw[pos++] = oyWIDGET_DISPLAY_WHITE_POINT_DAEMON;
           lw[pos++] = oyWIDGET_PROFILE_PROOF;
           lw[pos++] = oyWIDGET_RENDERING_INTENT_PROOF;
           lw[pos++] = oyWIDGET_PROOF_SOFT;
           lw[pos++] = oyWIDGET_PROOF_HARD;

           *count = pos;
           w = lw;
         }
         break;
    case oyGROUP_BEHAVIOUR_EFFECT:
         {
           oyAllocHelper_m_( lw, oyWIDGET_e , oyGROUP_BEHAVIOUR_EFFECT_LEN,
                             allocate_func, return NULL);

           lw[pos++] = oyWIDGET_GROUP_BEHAVIOUR_EFFECT;
           lw[pos++] = oyWIDGET_PROFILE_EFFECT;
           lw[pos++] = oyWIDGET_EFFECT;

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
           tmp = oyWidgetListGet_( oyGROUP_BEHAVIOUR_EFFECT, &n,
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
    case oyGROUP_CMM:
         {
           oyAllocHelper_m_( lw, oyWIDGET_e, oyGROUP_CMM_LEN,
                             allocate_func, return NULL);

           lw[pos++] = oyWIDGET_GROUP_CMM;
           lw[pos++] = oyWIDGET_CMM_CONTEXT;
           lw[pos++] = oyWIDGET_CMM_RENDERER;

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

           tmp = oyWidgetListGet_( oyGROUP_CMM, &n,
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
oyGroupAdd_                (const char *cmm OY_UNUSED,
                            const char *id,
                            const char *name,
                            const char *tooltips)
{
  const char ***ptr = NULL;
  const char **desc = NULL;
  int i;

  DBG_PROG_START
  ptr = calloc ( sizeof(char**), ++oy_groups_descriptions_ );
  if(!ptr) return oyGROUP_START;
  desc = calloc( sizeof(char*), 3 );
  if(!desc) { free(ptr); return oyGROUP_START; }

  oyTextsCheck_ ();

  desc[0] = id;
  desc[1] = name;
  desc[2] = tooltips;

  if(oy_groups_description_)
  {
    for(i = 0; i < (int)oy_groups_descriptions_ - 1; ++i)
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
    if(type == oyBEHAVIOUR_DISPLAY_WHITE_POINT)
    {
      int choice_list_n = oyOptionChoicesGetWtPtN_( );
      if(choice >= 0 &&
         choice < choice_list_n)
        r = 1;
      else
        WARNc2_S( "choice %d is invalid (count: %d)", choice, choice_list_n );
    } else
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

/** \addtogroup defaults_apis
 *  @{ *//* defaults_apis */

/** \addtogroup cmm_handling Default CMMs
 *  @brief Provide logical and UI support for Color Matching Module selection

 *  CMMs are modules, which do color calculations. The core part is 
 *  responsible to do profile concatenation into one single 
 *  color transform according to the provided options. It is loaded
 *  as a oyCMMapi4_s module.
 *  A policy module, implemented as a oyCMMapi9_s module,
 *  ensures the options follow the user and system settings.
 *  The renderer CMM (oyCMMapi7_s) module does the actual color transform, which is
 *  expressed as a device link for data exchange. So different core and
 *  renderer modules can easily be combined.

 *  @{ *//* cmm_handling */

char *             oyGetCMMName_     ( oyCMMapiFilter_s  * cmm,
                                       oyCMM_e             type,
                                       int                 name_type,
                                       oyAlloc_f           allocate_func )
{
  oyOBJECT_e otype = oyOBJECT_CMM_API4_S;
  oyCMMapiFilter_s_ * f = (oyCMMapiFilter_s_ *) cmm;
  char * name = NULL;

  if(type == oyCMM_CONTEXT ||
     type == oyCMM_CONTEXT_FALLBACK)
    otype = oyOBJECT_CMM_API4_S;
  else
    otype = oyOBJECT_CMM_API7_S;

  if(f)
  {
    oyCMMui_s * ui = NULL;

    if(name_type <= oyNAME_DESCRIPTION)
    {
      if(otype == oyOBJECT_CMM_API4_S)
      {
        ui = oyCMMapi4_GetUi((oyCMMapi4_s*)f);
        name = oyStringCopy( oyCMMui_GetTextF(ui)("name", name_type, (oyStruct_s*)ui),
                             allocate_func );
        
      } else
      {
        char * x = strstr(f->registration, "icc_color");
        if(x)
        {
          char * d;
          x = oyStringCopy(x+strlen("icc_color."), oyAllocateFunc_);
          d = strchr(x,'.');
          if(d)
            d[0] = '\000';
          name = oyStringCopy( x + 1, allocate_func );
          oyFree_m_( x );
        }
      }
    } else
    if(name_type == oyNAME_MODULE)
      name = oyStringCopy( f->id_, allocate_func );
    else
    if(name_type == oyNAME_REGISTRATION)
      name = oyStringCopy( f->registration, allocate_func );
    else
    if(name_type == oyNAME_PATTERN)
    {
      char * t = NULL,
           * nick = oyGetCMMName_( cmm, type, oyNAME_NICK, oyAllocateFunc_ );
      oyStringAddPrintf( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                         "///icc_color.%s", nick );
      oyFree_m_( nick );
      name = oyStringCopy( t, allocate_func );
      oyFree_m_( t );
    }
  }

  return name;
}

oyCMMapiFilter_s * oyGetCMM_         ( oyCMM_e             type,
                                       int                 name_type,
                                       const char        * name )
{
  uint32_t * rank_list = 0;
  uint32_t apis_n = 0;
  oyCMMapiFilters_s * apis;
  oyCMMapiFilter_s * filter = NULL;
  int i, n;

  oyOBJECT_e otype = oyOBJECT_CMM_API4_S;

  if(type == oyCMM_CONTEXT ||
     type == oyCMM_CONTEXT_FALLBACK)
    otype = oyOBJECT_CMM_API4_S;
  else
    otype = oyOBJECT_CMM_API7_S;

  apis = oyCMMsGetFilterApis_( "///icc_color", otype, 0,
                               &rank_list, &apis_n );
  n = oyCMMapiFilters_Count( apis );
  for(i = 0; i < n; ++i)
  {
    oyCMMapiFilter_s * f = oyCMMapiFilters_Get( apis, i );
    if(f)
    {
      char * t = NULL;
      int found = 0;

      if(name_type == oyNAME_REGISTRATION)
      {
        const char * r = ((oyCMMapiFilter_s_*) f)->registration;
        if(strcmp(r, name) == 0 ||
           (!strchr( name, '_' ) && oyFilterRegistrationMatch(r, name, 0)))
          found = 1;
      }
      else
      {
        t = oyGetCMMName_( f, type, name_type, oyAllocateFunc_ );
        if(strcmp(t, name) == 0)
          found = 1;
      }

      if(t)
        oyFree_m_( t );

      if(found)
      {
        oyCMMapiFilters_Release( &apis );
        filter = f;
        goto clean_oyGetCMM_;
      }

      if(f->release)
        f->release( (oyStruct_s**)&f );

    } else
      WARNc1_S( "      no api obtained %d",i);
  }

clean_oyGetCMM_:
  if(rank_list)
    oyFree_m_(rank_list);
  oyCMMapiFilters_Release( &apis );

  return filter;
}


/** Function oyCMMRegistrationToName
 *  @brief   Get a CMM string
 *
 *  Useful for displaying UI strings.
 *
 *  @param         registration        a registration string to match a existing CMM
 *  @param         type                the CMM type to set
 *  @param         name_type           oyNAME_e or oyNAME_PATTERN or oyNAME_REGISTRATION or oyNAME_MODULE
 *  @param         flags               unused
 *  @param         allocate_func       user allocator
 *  @return                            a UI string
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/06/11
 *  @since   2014/06/11 (Oyranos: 0.9.6)
 */
char *       oyCMMRegistrationToName ( const char        * registration,
                                       oyCMM_e             type,
                                       int                 name_type,
                                       uint32_t            flags OY_UNUSED,
                                       oyAlloc_f           allocate_func )
{
  char * name = NULL;

  DBG_PROG_START

  DBG_PROG1_S( "type = %d CMM", type )

  if(oyCMM_START < type && type < oyCMM_END)
  {
    oyCMMapiFilter_s * f = oyGetCMM_( type, oyNAME_REGISTRATION, registration );
    if(f)
    {
      name = oyGetCMMName_( f, type, name_type, allocate_func );

      if(f->release)
        f->release( (oyStruct_s**)&f );
    }

  } else
      WARNc1_S( "type %d not supported", type);

  DBG_PROG_ENDE
  return name;
}

/** Function oyCMMNameToRegistration
 *  @brief   Get a CMM registration from UI string
 *
 *  Useful for matching UI strings to registration in e.g. oySetCMMRegistration().
 *
 *  @param         name                a UI string
 *  @param         type                the CMM type to set
 *  @param         name_type           oyNAME_e or oyNAME_REGISTRATION or oyNAME_MODULE or oyNAME_PATTERN
 *  @param         flags               unused
 *  @param         allocate_func       user allocator
 *  @return                            a registration string to match a existing CMM
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/06/11
 *  @since   2014/06/11 (Oyranos: 0.9.6)
 */
char *       oyCMMNameToRegistration ( const char        * name,
                                       oyCMM_e             type,
                                       int                 name_type,
                                       uint32_t            flags OY_UNUSED,
                                       oyAlloc_f           allocate_func )
{
  char * reg = NULL;

  DBG_PROG_START

  DBG_PROG1_S( "type = %d CMM", type )

  if(oyCMM_START < type && type < oyCMM_END)
  {
    oyCMMapiFilter_s * f = oyGetCMM_( type, name_type, name );
    if(f)
    {
      reg = oyGetCMMName_( f, type, oyNAME_REGISTRATION, allocate_func );

      if(f->release)
        f->release( (oyStruct_s**)&f );
    }

  } else
      WARNc1_S( "type %d not supported", type);

  DBG_PROG_ENDE
  return reg;
}

/** Function oyGetCMMPattern
 *  @brief   Get a default CMM
 *
 *  The stored value will contain a registration pattern string.
 *
 *  @param         type                the CMM type to set
 *  @param         flags               oySOURCE_DATA for persistent DB only settings, oySOURCE_FILTER for Oyranos inbuild default
 *  @param         allocate_func       user allocator
 *  @return                            a registration pattern to match a CMM registration string
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/06/11
 *  @since   2014/06/11 (Oyranos: 0.9.6)
 */
char *       oyGetCMMPattern         ( oyCMM_e             type,
                                       uint32_t            flags,
                                       oyAlloc_f           allocate_func )
{
  const char* key_name = NULL;
  char * name = NULL;

  DBG_PROG_START

  DBG_PROG1_S( "type = %d CMM", type )

  if(oyCMM_START < type && type < oyCMM_END)
  {
    const oyOption_t_ * t = oyOptionGet_((oyWIDGET_e)type);

    if(t)
      key_name = t->config_string;

    if(key_name &&
       (!flags || flags & oySOURCE_DATA))
      name = oyGetPersistentString( key_name, flags, oySCOPE_USER_SYS,
                                    allocate_func );
    else if(!key_name)
      WARNc1_S( "type %d not supported", type);

    if((!name || !name[0]) &&
       (!flags || flags & oySOURCE_FILTER))
      name = oyStringCopy( oyOptionGet_((oyWIDGET_e)type)->default_string, allocate_func );

  } else
      WARNc1_S( "type %d not supported", type);

  DBG_PROG_ENDE
  return name;
}

/** Function oySetCMMPattern
 *  @brief   set a CMM as default
 *
 *  The stored value will contain a registration pattern string, which 
 *  shall fit a installed module registration string. However the caller has to 
 *  check the existence of the CMM itself.
 *
 *  @param         type                the CMM type to set
 *  @param         flags               unused
 *  @param         scope               oySCOPE_USER and oySCOPE_SYS are possible
 *  @param         pattern             a registration pattern to match a installed CMM registration
 *  @return                            -1 in case of an issue, 0 for proper operation, 1 for error
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/06/10
 *  @since   2014/06/10 (Oyranos: 0.9.6)
 */
int          oySetCMMPattern         ( oyCMM_e             type,
                                       uint32_t            flags OY_UNUSED,
                                       oySCOPE_e           scope,
                                       const char        * pattern )
{
  int r = 1;

  DBG_PROG_START

  if(oyCMM_START < type && type < oyCMM_END)
  {
    const char *key_name = 0;

    key_name = oyOptionGet_((oyWIDGET_e)type)-> config_string;

    if(key_name)
      r = oySetPersistentString (key_name, scope, pattern, NULL);
    else
      WARNc1_S( "type %d setting CMM not possible", type);
  } else
      WARNc1_S( "type %d not supported", type);

  DBG_PROG_ENDE
  return r;
}

/**
 *  @brief Get flags for oyProfile_FromFile() and friends
 *
 *  supported are "icc_version_2" - OY_ICC_VERSION_2 and
 *  "icc_version_4" - OY_ICC_VERSION_4 .
 *
 *  @param       registration    plain module registration; oyNAME_REGISTRATION
 *  @return                      profile selection flags
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/04/08
 *  @since   2014/04/08 (Oyranos: 0.9.6)
 */
uint32_t oyICCProfileSelectionFlagsFromRegistration (
                                       const char        * registration )
{
  uint32_t profile_flags = 0;

  if(!registration)
    return profile_flags;

  if(strstr( registration, "icc_version_2") != NULL)
    profile_flags |= OY_ICC_VERSION_2;
  if(strstr( registration, "icc_version_4") != NULL)
    profile_flags |= OY_ICC_VERSION_4;

  return profile_flags;
}


/** @} *//* cmm_handling */
/** @} *//* defaults_apis */

/** \addtogroup objects_value
 *  @{ *//* objects_value */
int      oyOptions_SetRegFromText    ( oyOptions_s      ** options,
                                       const char        * registration,
                                       const char        * value,
                                       uint32_t            flags OY_UNUSED )
{
  oyOption_s * o;
  int found = 0;
  int error = 0;

  if(!*options)
    *options = oyOptions_New( NULL );

  o = oyOptions_Find( *options, registration, oyNAME_REGISTRATION );
  if(o)
  {
    found = 1;
    DBG_PROG2_S("key found in cache: %s -> %s", registration, value ? value : "");
  } else
    o = oyOption_FromRegistration( registration, NULL );

  error = oyOption_SetFromString( o,
    /* cache the searched for value,
     * or mark with empty string if nothing was found */
                          value ? value : "",
                          0 );
  if(found)
    oyOption_Release( &o );
  else
    oyOptions_MoveIn( *options, &o, -1 );

  return error;
}

static int oy_db_cache_init_ = 0;
int * get_oy_db_cache_init_() { return &oy_db_cache_init_; };

/** Function oyGetPersistentStrings
 *  @brief   cache strings from DB
 *
 *  @param[in]     top_key_name        the DB root key, zero clears the 
 *                                     DB cache; use for example OY_STD
 *  @return                            error
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/03/02
 *  @since   2015/02/26 (Oyranos: 0.9.6)
 */
int          oyGetPersistentStrings  ( const char        * top_key_name )
{
  char * value = NULL;
  oyDB_s * db = NULL;
  int i;
  int error = 0;
  char ** key_names = NULL;
  int     key_names_n = 0;
  int init = !oy_db_cache_;

  if(!top_key_name)
  {
    oyOptions_Release( &oy_db_cache_ );
    oyConfigs_Release( &oy_monitors_cache_ );
    oy_db_cache_init_ = 0;
  }
  else
  {
    db = oyDB_newFrom( top_key_name, oySCOPE_USER_SYS, oyAllocateFunc_, oyDeAllocateFunc_ );

    key_names = oyDB_getKeyNames( db, top_key_name, &key_names_n );

    for(i = 0; i < key_names_n; ++i)
    {
      const char * key_name = key_names[i];
      value = oyDB_getString(db, key_name);

      error = oyOptions_SetRegFromText( &oy_db_cache_,
                                        key_name,
      /* cache the searched for value,
       * or mark with empty string if nothing was found */
                                        value ? value : "",
                                        OY_CREATE_NEW );

      if(value)
        oyFree_m_( value );
    }
    if(init && oy_db_cache_)
      oyObject_SetNames( ((oyOptions_s_*)oy_db_cache_)->list_->oy_,
                         "oy_db_cache_","oy_db_cache_","oy_db_cache_" );

    oyDB_release( &db );
    oyStringListRelease_( &key_names, key_names_n, oyDeAllocateFunc_ );
  }

  return error;
}

/** Function oyGetPersistentString
 *  @brief   get a cached string from DB
 *
 *  @param         key_name            the DB key name
 *  @param         flags               
 *                                     - 0 for cached string or
 *                                     - oySOURCE_DATA for a likely expensive DB lookup
 *  @param         scope               user/system or both, works together with
 *                                     flags |= oySOURCE_DATA
 *  @param         alloc_func          the user allocator
 *  @return                            the cached value
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/02/06
 *  @since   2015/02/06 (Oyranos: 0.9.6)
 */
char *       oyGetPersistentString   ( const char        * key_name,
                                       uint32_t            flags,
                                       oySCOPE_e           scope,
                                       oyAlloc_f           alloc_func )
{
  char * value = NULL;
  const char * return_value = NULL;

  if(flags & oySOURCE_DATA)
  {
    if(oy_db_cache_init_ == 0)
    {
      oy_db_cache_init_ = 1;
      /* cache in one go */
      oyGetPersistentStrings( OY_STD );
      value = oyGetPersistentString( key_name, 0, scope, alloc_func );
    } else
      /* fallback */
    {
      oyDB_s * db = oyDB_newFrom( key_name, scope, oyAllocateFunc_, oyDeAllocateFunc_ );
      value = oyDB_getString(db, key_name);
      oyDB_release( &db );
      oyOptions_SetRegFromText( &oy_db_cache_, key_name,
      /* cache the searched for value,
       * or mark with empty string if nothing was found */
                             value ? value : "",
                             OY_CREATE_NEW );
      DBG_PROG2_S("single DB request to key_name %s (cached:%d)", key_name, oyOptions_Count(oy_db_cache_));
    }
  } else
  {
    oyOption_s * o = oyOptions_Find( oy_db_cache_, key_name, oyNAME_REGISTRATION );
    return_value = oyOption_GetValueString( o, 0 );
    if(!return_value)
    {
      DBG_PROG1_S("no value for key_name %s", key_name);
      value = oyGetPersistentString( key_name, oySOURCE_DATA,scope, alloc_func);
    } else
      value = oyStringCopy( return_value, alloc_func );

    oyOption_Release( &o );
  }

  return value;
}

/** Function oyExistPersistentString
 *  @brief   check a cached string from DB
 *
 *  @param         key_name            the DB key name
 *  @param         flags               
 *                                     - 0 for cached string or
 *                                     - oySOURCE_DATA for a likely expensive DB lookup
 *  @param         scope               user/system or both, works together with
 *                                     flags |= oySOURCE_DATA
 *  @param         value               the expected value
 *  @return                            found : number of occurences, otherwise : false
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/05/15
 *  @since   2018/05/15 (Oyranos: 0.9.7)
 */
int          oyExistPersistentString ( const char        * key_name,
                                       const char        * value,
                                       uint32_t            flags,
                                       oySCOPE_e           scope )
{
  int found = 0;
  char * value_ = oyGetPersistentString( key_name, flags, scope, oyAllocateFunc_ );

  if(value && value_ && strcmp(value, value_) == 0)
    ++found;
  else if(value == NULL && value_)
    ++found;

  if(value_)
    oyFree_m_(value_);

  return found;
}

int          oyCanSetPersistent      ( oySCOPE_e           scope )
{
  int available = 1;
  if(scope == oySCOPE_USER || scope == oySCOPE_USER_SYS)
  {
    char * path = oyGetInstallPath( oyPATH_POLICY, oySCOPE_USER, oyAllocateFunc_ );
    oyjlStringAdd( &path, oyAllocateFunc_, oyDeAllocateFunc_, "/openicc.json" );
    size_t size = oyIsFile_(path) ? oyReadFileSize_(path) : 0;
    if(size)
    {
#ifdef HAVE_POSIX
      {
        /* get local free disk space the POSIX way */
        char * num = oyReadCmdToMemf_( &size, "r", oyAllocateFunc_, "df -P %s | awk '{ print $4 }' | awk '{ sum += $1 }; END { print sum }'", path );
        long space = 0;
        if(oy_debug)
          fprintf(stderr, "empty disk space: %s\n", num );
        if(num && oyjlStringToLong(num, &space) == 0)
        {
          if(space < 10)
          {
            WARNc1_S( "not enough disk space: %ld", space );
            return 0;
          }
        }
        if(num) free(num);
      }
#else
      {
        /* check file size and possibly send warning */
        char * data = oyReadFileToMem_( path, &size, oyAllocateFunc_);
        oyjlStringAdd( &data, oyAllocateFunc_, oyDeAllocateFunc_, "testtesttesttesttest" );
        char * output = NULL;
        int error = oyWriteMemToFile2_( path, data, strlen(data), OY_FILE_NAME_SEARCH, &output, oyAllocateFunc_ );
        size_t output_size = oyReadFileSize_(output);
        if(error || output_size < strlen(data))
        {
          WARNc3_S( "output_size: %d|%d error: %d\n", (int)output_size, (int)strlen(data), error );
          return 0;
        }
        error = remove(output);
        if(error)
           WARNc3_S( "remove error: %d %s %s\n", error, strerror(errno), output );
        if(data) oyDeAllocateFunc_(data);
        if(output) oyDeAllocateFunc_(output);
      }
#endif
    }
    if(path) oyDeAllocateFunc_(path);
  }
  return available;
}

/** Function oySetPersistentString
 *  @brief   set string into DB and cache
 *
 *  @param         key_name            the DB key name
 *  @param         scope               possible values are:
 *                                     - oySCOPE_USER
 *                                     - oySCOPE_SYS
 *                                     - oySCOPE_USER_SYS means, you are your own with prefixing
 *  @param         value               the value string; NULL means erase
 *  @param         comment             the comment string
 *  @return                            DB specific return code
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/02/06
 *  @since   2015/02/06 (Oyranos: 0.9.6)
 */
int          oySetPersistentString   ( const char        * key_name,
                                       oySCOPE_e           scope,
                                       const char        * value,
                                       const char        * comment )
{
  int rc = -1;
  const char * key = key_name;
  int error = 0;

  if(!oyCanSetPersistent(scope))
    return rc;

  if(value)
    rc = oyDBSetString( key_name, scope, value, comment );
  else
    rc = oyDBEraseKey( key_name, scope );
  if(scope == oySCOPE_USER_SYS)
  {
    if(strchr( key_name, '/' ))
      key = strchr( key_name, '/' ) + 1;
  }
  error = oyOptions_SetRegFromText( &oy_db_cache_, key, value, OY_ADD_ALWAYS );
  if(error)
    WARNc3_S( "Could not set key: %d %s -> %s",
              error, key_name, value ? value : "" );

  return rc;
}

/** @} *//* objects_value */


int      oySetBehaviour_             ( oyBEHAVIOUR_e       type,
                                       oySCOPE_e           scope,
                                       int                 choice)
{
  int error = 0;

  DBG_PROG_START

  DBG_PROG2_S( "type = %d behaviour %d", type, choice )

  if ( (error=!oyTestInsideBehaviourOptions_(type, choice)) == 0 )
  {
    const char *key_name = 0;

    key_name = oyOptionGet_((oyWIDGET_e)type)-> config_string;

    if(key_name)
    {
      char val[64];
      char * com = NULL;
      if(type == oyBEHAVIOUR_DISPLAY_WHITE_POINT)
      {
        int choice_list_n = 0;
        char ** choice_list = oyOptionChoicesGetWtPt_( &choice_list_n );
        if(choice < choice_list_n)
          com = oyStringCopy( choice_list[choice], 0 );
	oyStringListRelease( &choice_list, choice_list_n, 0 );
	
      } else
      if(type == oyBEHAVIOUR_DISPLAY_WHITE_POINT_DAEMON)
      {
        int choice_list_n = 0;
        char ** choice_list = NULL;
        if(choice == 0)
        {
          error = oySetPersistentString (key_name, scope, NULL, NULL);
          DBG_PROG_ENDE
          return error;
        }
        oyOptionChoicesGetWtPtD_( &choice_list_n, &choice_list,
                                  NULL );
        if(choice < choice_list_n)
          com = oyStringCopy( choice_list[choice], 0 );
        snprintf(val, 64, "%s", com);
        oyStringListRelease( &choice_list, choice_list_n, 0 );

      } else
        com = oyStringCopy( oyOptionGet_((oyWIDGET_e)type)-> choice_list[ choice ], 0 );

      if(type != oyBEHAVIOUR_DISPLAY_WHITE_POINT_DAEMON)
        snprintf(val, 64, "%d", choice);
      error = oySetPersistentString (key_name, scope, val, com);
      DBG_PROG4_S( "%s %d %s %s", key_name, type, val, com?com:"" )
      if(com)
        oyFree_m_(com);
    }
    else
      WARNc1_S( "type %d behaviour not possible", type);
  }

  DBG_PROG_ENDE
  return error;
}

int oyGetBehaviour_      (oyBEHAVIOUR_e type)
{
  char* name = 0;
  const char* key_name = 0;
  int c = -1;

  DBG_PROG_START

  DBG_PROG1_S( "type = %d behaviour", type )

  if( oyTestInsideBehaviourOptions_(type, 0) )
  {
    key_name = oyOptionGet_((oyWIDGET_e)type)-> config_string;

    if(key_name)
      name = oyGetPersistentString( key_name, 0, oySCOPE_USER_SYS,
                                    oyAllocateFunc_ );
    else
      WARNc1_S( "type %d behaviour not possible", type);
  }
  else
    WARNc1_S( "type %d behaviour not possible", type);

  if(type == oyBEHAVIOUR_DISPLAY_WHITE_POINT_DAEMON)
  {
    if(name && name[0])
      c = 1;
  } else if(name)
  {
    c = atoi(name);
  }

  if(name)
    oyFree_m_( name );

  if(c < 0)
    c = oyOptionGet_((oyWIDGET_e)type)-> default_value;

  DBG_PROG_ENDE
  return c;
}



/* not  */
void
oyI18NSet_             ( int active,
                         int reserved OY_UNUSED )
{
  DBG_PROG_START

  if(active)
    oy_domain = OY_TEXTDOMAIN;
  else
    oy_domain = "";


  oyTextsTranslate_ ();

  DBG_PROG_ENDE
}

