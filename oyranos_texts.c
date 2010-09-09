/** @file oyranos_texts.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    pure text and string handling functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */

#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iconv.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_alpha.h"
#include "oyranos_cmms.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_xml.h"
#include "oyranos_alpha.h"

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
#   define oySET_OPTIONS_M_( t_, id_, ca_n, ca1, ca2, ca3, labl, desc, \
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
      _("Start"),
      _("Oyranos Settings Group"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_START", 0,NULL)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_DEFAULT_PROFILES, 0,
      0, 0, 0,
      _("Default Profiles"),
      _("Source and Target Profiles for various situations"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES", 0, NULL)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_DEFAULT_PROFILES_EDIT, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Editing Colour Space"),
      _("Well behaving Colour Space for Editing"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES_EDIT", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_DEFAULT_PROFILES_ASSUMED, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed Colour Space"),
      _("Assumed Colour Space for untagged colours"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES_ASSUMED", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_DEFAULT_PROFILES_PROOF, 0,
      0, 0, 0,
      _("Proofing Colour Space"),
      _("Colour Space for Simulating real devices"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES_PROOF", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_PATHS, 0,
      0, 0, 0,
      _("Paths"),
      _("Paths where ICC Profiles can be found"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_PATHS", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_POLICY, 0,
      0, 0, 0,
      _("Policy"),
      _("Collections of settings in Oyranos"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_POLICY", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_BEHAVIOUR, 0,
      0, 0, 0,
      _("Behaviour"),
      _("Settings affecting the Behaviour in various situations"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_BEHAVIOUR_RENDERING, 1,
      oyGROUP_BEHAVIOUR, 0, 0,
      _("Rendering"),
      _("The kind of ICC gamut mapping for transforming colours between differently sized colour spaces"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_RENDERING", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, 1,
      oyGROUP_BEHAVIOUR, 0, 0,
      _("Save Mixed colour space Documents"),
      _("Handling of Mixed Colour Spaces inside one single Document"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_BEHAVIOUR_MISSMATCH, 1, 
      oyGROUP_BEHAVIOUR, 0, 0,
      _("Mismatching"),
      _("Decide what to do when the default colour spaces dont match the current ones."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_MISSMATCH", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_BEHAVIOUR_PROOF, 1,
      oyGROUP_BEHAVIOUR, 0, 0,
      _("Proofing"),
      _("Proofing Settings"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_PROOF", 0,0)
    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, oyWIDGET_GROUP_ALL, 0,
      0, 0, 0,
      _("All"),
      _("Oyranos Settings"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_ALL", 0,0)


    oySET_OPTIONS_M_( oyWIDGETTYPE_CHOICE, oyWIDGET_POLICY, 1,
      oyGROUP_POLICY, 0, 0,
      _("Policy"),
      _("Collections of settings in Oyranos"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyWIDGET_POLICY", 0,0)
    oy_option_[oyWIDGET_POLICY].flags = OY_LAYOUT_MIDDLE;


    oySET_OPTIONS_M_( oyWIDGETTYPE_LIST, oyWIDGET_PATHS, 1,
      oyGROUP_PATHS, 0, 0,
      _("Paths"),
      _("Paths where ICC Profiles can be found"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyWIDGET_PATHS", 0,0)
    oy_option_[oyWIDGET_PATHS].flags = OY_LAYOUT_PATH_SELECTOR;

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_RGB, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_EDIT, 0,
      _("Editing Rgb"),
      _("Prefered Rgb Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_RGB_PROFILE,
      "oyEDITING_RGB", 0,"sRGB.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_CMYK, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_EDIT, 0,
      _("Editing Cmyk"),
      _("Prefered Cmyk Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_CMYK_PROFILE,
      "oyEDITING_CMYK", 0,"coated_FOGRA39L_argl.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_XYZ, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_EDIT, 0,
      _("Editing XYZ"),
      _("Prefered XYZ Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_XYZ_PROFILE,
      "oyEDITING_XYZ", 0,"XYZ.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_LAB, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_EDIT, 0,
      _("Editing Lab"),
      _("Prefered CIE*Lab Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_LAB_PROFILE,
      "oyEDITING_LAB", 0,"Lab.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_GRAY, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_EDIT, 0,
      _("Editing Gray"),
      _("Prefered Gray Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_GRAY_PROFILE,
      "oyEDITING_GRAY", 0,"Gray-CIE_L.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_XYZ, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_ASSUMED, 0,
      _("Assumed XYZ source"),
      _("Assigning an untagged XYZ Image an colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_XYZ_PROFILE,
      "oyASSUMED_XYZ", 0,"XYZ.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_LAB, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_ASSUMED, 0,
      _("Assumed Lab source"),
      _("Assigning an untagged CIE*Lab Image an colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_LAB_PROFILE,
      "oyASSUMED_LAB", 0,"Lab.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_RGB, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_ASSUMED, 0,
      _("Assumed Rgb source"),
      _("Assigning an untagged Rgb Image an colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_RGB_PROFILE,
      "oyASSUMED_RGB", 0,"sRGB.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_WEB, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_ASSUMED, 0,
      _("Assumed Web source"),
      _("Assigning an untagged Rgb Image with source from the WWW an colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_WEB_PROFILE,
      "oyASSUMED_WEB", 0,"sRGB.icc")
    oy_option_[oyWIDGET_ASSUMED_WEB].flags = OY_LAYOUT_NO_CHOICES;

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_CMYK, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_ASSUMED, 0,
      _("Assumed Cmyk source"),
      _("Assigning an untagged Cmyk Image this colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_CMYK_PROFILE,
      "oyASSUMED_CMYK" , 0,"coated_FOGRA39L_argl.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_GRAY, 2,
      oyGROUP_DEFAULT_PROFILES, oyGROUP_DEFAULT_PROFILES_ASSUMED, 0,
      _("Assumed Gray source"),
      _("Assigning an untagged Gray Image this colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_GRAY_PROFILE,
      "oyASSUMED_GRAY" , 0,"Gray-CIE_L.icc")

    oySET_OPTIONS_M_( oyWIDGETTYPE_DEFAULT_PROFILE, oyWIDGET_PROFILE_PROOF, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("Proofing"),
      _("Colour space for Simulating an Output Device"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_PROOF_PROFILE,
      "oyPROFILE_PROOF" , 0,"coated_FOGRA39L_argl.icc")


    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_ACTION_UNTAGGED_ASSIGN, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("No Image profile"),
      _("Image has no colour space embedded. What default action shall be performed?"),
      3, /* choices */
      _("Assign No Profile"),_("Assign Assumed Profile"),_("Promt"), NULL,
      OY_ACTION_UNTAGGED_ASSIGN,
      "oyBEHAVIOUR_ACTION_UNTAGGED_ASSIGN" , 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_ACTION_OPEN_MISMATCH_RGB, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("On Rgb Mismatch"),
      _("Action for Image profile and Editing profile mismatches."),
      3, /* choices */
      _("Preserve Numbers"),_("Convert automatically"),_("Promt"), NULL,
      OY_ACTION_MISMATCH_RGB,
      "oyBEHAVIOUR_ACTION_MISMATCH_RGB" , 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_ACTION_OPEN_MISMATCH_CMYK, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("On Cmyk Mismatch"),
      _("Action for Image profile and Editing profile mismatches."),
      3, /* choices */
      _("Preserve Numbers"),_("Convert automatically"),_("Promt"), NULL,
      OY_ACTION_MISMATCH_CMYK,
      "oyBEHAVIOUR_ACTION_MISMATCH_CMYK", 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_MIXED_MOD_DOCUMENTS_PRINT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, 0,
      _("For Print"),
      _("Handle Mixed colour spaces in Preparing a document for Print output."),
      4, /* choices */
      _("Preserve Numbers"),_("Convert to Default Cmyk Editing Space"),_("Convert to untagged Cmyk, preserving Cmyk numbers"),_("Promt"),
      OY_CONVERT_MIXED_COLOUR_SPACE_PRINT_DOCUMENT,
      "oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_PRINT", 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_MIXED_MOD_DOCUMENTS_SCREEN, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, 0,
      _("For Screen"),
      _("Handle Mixed colour spaces in Preparing a document for Screen output."),
      4, /* choices */
      _("Preserve Numbers"),_("Convert to Default Rgb Editing Space"),_("Convert to WWW (sRGB)"),_("Promt"),
      OY_CONVERT_MIXED_COLOUR_SPACE_SCREEN_DOCUMENT,
      "oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_SCREEN", 2,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_RENDERING_INTENT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_RENDERING, 0,
      _("Rendering Intent"),
      _("Rendering Intent for colour space Transformations."),
      4, /* choices */
      _("Perceptual"),_("Relative Colorimetric"),_("Saturation"),_("Absolute Colorimetric"),
      OY_DEFAULT_RENDERING_INTENT,
      "oyBEHAVIOUR_RENDERING_INTENT", 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_RENDERING_BPC, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_RENDERING, 0,
      _("Use Black Point Compensation"),
      _("BPC affects often only the Relative Colorimetric Rendering Intent."),
      2, /* choices */
      _("No"),_("Yes"), NULL, NULL,
      OY_DEFAULT_RENDERING_BPC,
      "oyBEHAVIOUR_RENDERING_BPC", 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_RENDERING_INTENT_PROOF, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("Proofing Rendering Intent"),
      _("Behaviour of colour space transformation for proofing"),
      2, /* choices */
      _("Relative Colorimetric"),_("Absolute Colorimetric"),NULL,NULL,
      OY_DEFAULT_RENDERING_INTENT_PROOF,
      "oyBEHAVIOUR_RENDERING_INTENT_PROOF", 0,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_PROOF_SOFT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("SoftProof"),
      _("Simulate the output print on the monitor"),
      2, /* choices */
      _("No"),_("Yes"),NULL,NULL,
      OY_DEFAULT_PROOF_SOFT,
      "oyBEHAVIOUR_PROOF_SOFT", 0,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_PROOF_HARD, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("Hardproof"),
      _("Behaviour for preselecting Hardproofing with a Proofing Profile at print time"),
      2, /* choices */
      _("No"),_("Yes"),NULL,NULL,
      OY_DEFAULT_PROOF_HARD,
      "oyBEHAVIOUR_PROOF_HARD", 0,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_RENDERING_GAMUT_WARNING, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_RENDERING, 0,
      _("Gamut Warning"),
      _("Mark Out Of Gamut colours"),
      2, /* choices */
      _("No"),_("Yes"),NULL,NULL,
      OY_DEFAULT_RENDERING_GAMUT_WARNING,
      "oyBEHAVIOUR_RENDERING_GAMUT_WARNING", 1,0)

    oySET_OPTIONS_M_( oyWIDGETTYPE_BEHAVIOUR, oyWIDGET_RENDERING_HIGH_PRECISSION, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_RENDERING, 0,
      _("High Precission"),
      _("Rendering with High Precission in opposite to fast processing"),
      2, /* choices */
      _("No"),_("Yes"),NULL,NULL,
      OY_DEFAULT_RENDERING_HIGH_PRECISSION,
      "oyBEHAVIOUR_RENDERING_HIGH_PRECISSION", 1,0)

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

  result = oyModulsUIOptionSearch_( type );

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
    *tooltip              = t->description;
    if( flags )
    *flags                = t->flags;
  }

  DBG_PROG_ENDE
  return t->type;
}


char **
oyProfilePathsGet_    (int             * count,
                       oyAlloc_f         allocateFunc)
{
  char ** path_names = NULL;
#if 1
  char ** tmp = NULL;
  int tmp_n = -1;

  path_names = oyDataPathsGet_( count, "color/icc", oyALL, oyUSER_SYS,
                                          oyAllocateFunc_ );
# if defined(__APPLE__)
# define TestAndSetDefaultPATH( path ) \
  if(oyIsDir_( path )) \
    oyStringListAddStaticString_ ( &path_names, count, path, oyAllocateFunc_, \
                                   oyDeAllocateFunc_ );

  /* Apples ColorSync default paths */

# define CSSystemPATH        "/System/Library/ColorSync/Profiles"
# define CSGlobalInstallPATH "/Library/ColorSync/Profiles"
# define CSUserPATH          "~/Library/ColorSync/Profiles"
# define CSNetworkPath       "/Network/Library/ColorSync/Profiles"
  TestAndSetDefaultPATH( CSSystemPATH );
  TestAndSetDefaultPATH( CSGlobalInstallPATH );
  TestAndSetDefaultPATH( CSUserPATH );
  TestAndSetDefaultPATH( CSNetworkPath );

# undef TestAndSetDefaultPATH
# endif
  tmp = oyStringListAppend_( 0, 0, (const char**)path_names, *count,
                             &tmp_n, allocateFunc );
  oyStringListRelease_( &path_names, *count,  oyDeAllocateFunc_ );
  path_names = tmp; tmp = 0;
  *count = tmp_n;

#else
  int i,
      n = 0;
  int c = oyPathsCount_();

  oyAllocHelper_m_ (path_names, char*, c, allocateFunc, return NULL);

  for( i = 0; i < c; ++i)
  {
    char * test = oyPathName_( i, oyAllocateFunc_ );
    char * checked = oyMakeFullFileDirName_( test );

    if( checked )
    {
      path_names[n] = allocateFunc( oyStrblen_(checked) + 1 );
      oySprintf_( path_names[n], "%s", checked );
      oyFree_m_( checked );
      ++n;
    }
    oyFree_m_( test );
  }

  *count = n;
#endif
  return path_names;
}


/* string manipulation */

char*
oyStringCopy_      ( const char    * text,
                     oyAlloc_f       allocateFunc )
{
  char * text_copy = NULL;
    
  if(text)
  {
      oyAllocHelper_m_( text_copy, char, oyStrlen_(text) + 1,
                        allocateFunc, return 0 );
      oyStrcpy_( text_copy, text );
  }
  return text_copy;
}

/** @internal 
 *  @brief append a string and care about allocation
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
char*              oyStringAppend_   ( const char        * text,
                                       const char        * append,
                                       oyAlloc_f           allocateFunc )
{
  char * text_copy = NULL;
  int text_len = 0, append_len = 0;

  if(text)
    text_len = oyStrlen_(text);

  if(append)
    append_len = oyStrlen_(append);

  if(text_len || append_len)
  {
    oyAllocHelper_m_( text_copy, oyChar,
                      text_len + append_len + 1,
                      allocateFunc, return 0 );

    if(text_len)
      oySprintf_( text_copy, "%s", text );
      
    if(append_len)
      oySprintf_( &text_copy[text_len], "%s", append );
  }

  return text_copy;
}

/** @internal 
 *  @brief   search in a data pointer for text
 *
 *  Extract only the visible text. Stop for any other non print or space sign. 
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/06 (Oyranos: 0.1.10)
 *  @date    2009/01/06
 */
int                oyStringFromData_ ( const oyPointer     ptr,
                                       size_t              size,
                                       char             ** text_new,
                                       size_t            * text_new_size,
                                       oyAlloc_f           allocateFunc )
{
  const char * text = (const char*) ptr;
  char * text_tmp = 0;
  int j,
      error = 0;

  if(ptr && size)
  {
    j = 0;

    while(j < size)
      if(!isprint( text[j] ) && !isspace( text[j] ))
      {
        text = 0;
        break;
      } else
        ++j;

    if(text)
    {
      text_tmp = allocateFunc( j );
      error = !text_tmp;
      if(!error)
      {
        error = !memcpy( text_tmp, text, j-1 );
        if(error)
          text_tmp[j-1] = 0;
      }
    }
  }

  if(!error && text)
  {
    *text_new = text_tmp;
    *text_new_size = j - 1;
  }

  return error;
}

/** @internal 
 *  @brief add a string and care about de-/allocation
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
void               oyStringAdd_      ( char             ** text,
                                       const char        * append,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocFunc )
{
  char * text_copy = NULL;

  text_copy = oyStringAppend_(*text, append, allocateFunc);

  if(text && *text && deallocFunc)
    deallocFunc(*text);

  *text = text_copy;

  return;
}

/** @internal 
 *  @brief   printf style string add
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/07 (Oyranos: 0.1.10)
 *  @date    2009/02/07
 */
int                oyStringAddPrintf_( char             ** string,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocFunc,
                                       const char        * format,
                                                           ... )
{
  char * text_copy = NULL;
  char * text = 0;
  va_list list;
  int len;
  size_t sz = 256;

  text = malloc( sz );
  if(!text)
  {
    fprintf(stderr,
     "oyranos.c:80 oyMessageFunc_() Could not allocate 256 byte of memory.\n");
    return 1;
  }

  text[0] = 0;

  va_start( list, format);
  len = vsnprintf( text, sz, format, list);
  va_end  ( list );

  if (len >= sz)
  {
    text = realloc( text, (len+1)*sizeof(char) );
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list);
    va_end  ( list );
  }


  text_copy = oyStringAppend_(*string, text, allocateFunc);

  if(string && *string && deallocFunc)
    deallocFunc(*string);

  *string = text_copy;

  free(text);

  return 0;
}


char**             oyStringSplit     ( const char    * text,
                                       const char      delimiter,
                                       int           * count,
                                       oyAlloc_f       allocateFunc )
{
  return oyStringSplit_(text, delimiter,count,allocateFunc);
}

char**             oyStringSplit_    ( const char    * text,
                                       const char      delimiter,
                                       int           * count,
                                       oyAlloc_f       allocateFunc )
{
  char ** list = 0;
  int n = 0;

  if(text && text[0] && delimiter)
  {
    int i;
    const char * tmp = text;

    if(!allocateFunc)
      allocateFunc = oyAllocateFunc_;

    if(tmp[0] == delimiter) ++n;
    do { ++n;
    } while( (tmp = oyStrchr_(tmp + 1, delimiter)) );

    tmp = 0;

    if((list = allocateFunc( (n+1) * sizeof(char*) )) == 0) return 0;

    {
      const char * start = text;
      for(i = 0; i < n; ++i)
      {
        intptr_t len = 0;
        char * end = oyStrchr_(start, delimiter);

        if(end > start)
          len = end - start;
        else if (end == start)
          len = 0;
        else
          len = oyStrlen_(start);

        if((list[i] = allocateFunc( len+1 )) == 0) return 0;

        memcpy( list[i], start, len );
        list[i][len] = 0;
        start += len + 1;
      }
    }
  }

  if(count)
    *count = n;

  return list;
}


char *             oyStrnchr_        ( char              * text,
                                       char                delimiter,
                                       int                 len )
{
  int i = 0;

  while(i < len)
  {
    if(text[i] == delimiter)
      return &text[i];
    else if(text[i] == 0)
      return 0;
    ++i;
  }

  return 0;
}

char *             oyStrnchrN_       ( char              * text,
                                       char                delimiter,
                                       int                 len,
                                       int               * end )
{
  *end = 0;

  while(*end < len)
  {
    if(text[*end] == delimiter)
      return &text[*end];
    else if(text[*end] == 0)
      return 0;
    ++ (*end);
  }

  return 0;
}

int                oyStringSegments_ ( const char        * text,
                                       char                delimiter )
{
  int n = 0;

  if(text[0] == delimiter) ++n;
  do { ++n;
  } while( (text = oyStrchr_(text + 1, delimiter)) );

  return n;
}

int                oyStringSegmentsN_( const char        * text,
                                       int                 len,
                                       char                delimiter )
{
  int n = 0;
  int end = 0;

  if(text[0] == delimiter) ++n;
  do {
    ++n;
    len -= end;
  } while( (text = oyStrnchrN_((char*)text + 1, delimiter, len, &end)) &&
           len-end-1 > 0 );

  return n;
}

char *             oyStringSegment_  ( char              * text,
                                       char                delimiter,
                                       int                 segment,
                                       int               * end )
{
  intptr_t end_pos = 0;
  int i = 0;
  char * t = text;

  for(; i < segment; ++i)
  {
    t = oyStrchr_( t, delimiter );
    ++t;
  }

  end_pos = (intptr_t) oyStrchr_(t, delimiter);

  if(end_pos == 0)
  {
    *end = oyStrlen_(t);
    return t;
  }

  *end = (int) (end_pos - (intptr_t) t);

  return t;
}

char *             oyStringSegmentN_ ( char              * text,
                                       int                 len,
                                       char                delimiter,
                                       int                 segment,
                                       int               * end )
{
  intptr_t end_pos = (intptr_t)text;
  int i = 0;
  char * t = text;

  for(; i < segment; ++i)
  {
    t = oyStrchr_( t, delimiter );
    ++t;
  }

  if(len)
  {
    end_pos = (intptr_t) oyStrnchr_(t, delimiter, text + len - t);

    if(end_pos == 0)
    {
      *end = text + len - t;
      return t;
    }
  }

  *end = (int) (end_pos - (intptr_t) t);

  return t;
}

int    oyStringSegmentX_             ( const char        * text,
                                       char                delimiter,
                                       int               * count,
                                       int              ** pos,
                                       char             ** max_segment )
{
  int n = 0, i, max_segment_len = 0;
  const char * tmp = text;
  static int int_size = sizeof(int);
  const char * start = text;

  if(tmp[0] == delimiter) ++n;
  do { ++n;
  } while( (tmp = oyStrchr_(tmp + 1, delimiter)) );

  if(n > *pos[0])
  {
    oyDeAllocateFunc_( *pos );
    *pos = oyAllocateFunc_( int_size * (n+1) );
    if(!*pos) return 1;
    *pos[0] = n;
  }

  for(i = 0; i < n; ++i)
  {
        intptr_t len = 0;
        char * end = oyStrchr_(start, delimiter);

        if(end > start)
          len = end - start;
        else if (end == start)
          len = 0;
        else
          len = oyStrlen_(start);

        *pos[i] = (int)((intptr_t)(start - text + len));
        start += len + 1;

    if(len > max_segment_len)
      max_segment_len = len;
  }

  if(!max_segment ||
     ((int)*((uint8_t*)&max_segment[0]) < max_segment_len &&
      (int)*((uint8_t*)&max_segment[0]) != 255))
  {
    *((uint8_t*)&max_segment[0]) = OY_MIN(255, max_segment_len);
    *max_segment = oyAllocateFunc_( *((uint8_t*)&max_segment[0]) );
    if(!*max_segment) return 1;
  }

  return 0;
}


void               oyStringListAdd_  ( char            *** list,
                                       int               * n,
                                       const char       ** append,
                                       int                 n_app,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc )
{
  int alt_n = *n;
  char ** tmp = oyStringListAppend_((const char**)*list, alt_n, append, n_app,
                                     n, allocateFunc);

  oyStringListRelease_(list, alt_n, deallocateFunc);

  *list = tmp;
}

void               oyStringListAddStaticString_ ( char *** list,
                                       int               * n,
                                       const char        * string,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc )
{
  int alt_n = *n;
  char ** tmp = oyStringListAppend_((const char**)*list, alt_n,
                                    (const char**)&string, 1,
                                     n, allocateFunc);

  oyStringListRelease_(list, alt_n, deallocateFunc);

  *list = tmp;
}

void               oyStringListAddString_ ( char       *** list,
                                       int               * n,
                                       char             ** string,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc )
{
  int alt_n = *n;
  char ** tmp = oyStringListAppend_((const char**)*list, alt_n,
                                    (const char**)string, 1,
                                     n, allocateFunc);

  deallocateFunc(*string); *string = 0;
  oyStringListRelease_(list, alt_n, deallocateFunc);

  *list = tmp;
}

char**             oyStringListAppend_( const char   ** list,
                                        int             n_alt,
                                        const char   ** append,
                                        int             n_app,
                                        int           * count,
                                        oyAlloc_f       allocateFunc )
{
  char ** nlist = 0;

  {
    int i = 0;
    int n = 0;

    if(n_alt || n_app)
      oyAllocHelper_m_(nlist, char*, n_alt + n_app +1, allocateFunc, return NULL);

    for(i = 0; i < n_alt; ++i)
    {
      if(list[i] /*&& oyStrlen_(list[i])*/)
        nlist[n] = oyStringCopy_( list[i], allocateFunc );
      n++;
    }

    for(i = 0; i < n_app; ++i)
    {
      if(1/*oyStrlen_( append[i] )*/)
        nlist[n] = oyStringCopy_( append[i], allocateFunc );
      n++;
    }

    if(count)
      *count = n;
  }

  return nlist;
}


/** @internal
 *  @brief reducing filter
 *
 *  In order to collect all possible matches chain this filter.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  december 2007 (API 0.1.8)
 */
char**             oyStringListFilter_(const char   ** list,
                                       int             list_n,
                                       const char    * dir_string,
                                       const char    * string,
                                       const char    * suffix,
                                       int           * count,
                                       oyAlloc_f       allocateFunc )
{
  char ** nl = 0;
  int nl_n = 0;
  int i;

  for(i = 0; i < list_n; ++i)
  {
    int b = 1;

    if(list[i] && list[i][0])
    {
      if(dir_string && dir_string[0])
      {
        char * t = oyStrstr_(list[i], dir_string);
        if(t)
          b = (oyStrstr_(t+oyStrlen_(dir_string), OY_SLASH)) ? 1:0;
        else
          b = 0;
        if(!b) continue;
      }

      if(b && string && string[0])
      {
        b = (oyStrstr_(list[i], string))?1:0;
        if(!b) continue;
      }

      if(b && suffix && suffix[0])
      {
        const char * fn = list[i],
                   * f1 = fn + oyStrlen_(fn) - oyStrlen_(suffix);
        b = oyStrcmp_(f1, suffix) == 0;
        if(!b) continue;
      }
    }

    if(b)
    {
      if(!nl)
        oyAllocHelper_m_( nl, char*, list_n + 1, allocateFunc, return 0);

      nl[nl_n] = oyStringCopy_( list[i], allocateFunc );
      if(nl[nl_n])
        ++nl_n;
    }
  }

  if(count)
    *count = nl_n;

  return nl;
}

void          oyStringListRelease_    ( char          *** l,
                                        int               size,
                                        oyDeAlloc_f       deallocFunc )
{
  char *** list = l;

  DBG_MEM_START

  if( l )
  {
    size_t i;

    for(i = 0; i < size; ++i)
      if((*list)[i])
        deallocFunc( (*list)[i] );
    if(*list)
      deallocFunc( *list );
    *list = NULL;
  }

  DBG_MEM_ENDE
}

#ifdef DEBUG
int     oyStrlen_( const char * str_ ) { return strlen(str_); }
void    oyStrcpy_( char * targ_, const char * src_ ) { strcpy(targ_,src_); }
char *  oyStrchr_( const char * str_, char c_ ) { return strchr(str_,c_); }
char *  oyStrrchr_( const char * str_, char c_ ) { return strrchr(str_,c_); }
char *  oyStrstr_( const char * str1_, const char * str2_ ) { return strstr(str1_,str2_); }
int     oyStrcmp_( const char * str1_, const char * str2_ ) { return strcmp(str1_,str2_); }
char    oyToupper_( char c_ ) { return toupper(c_); }
#endif

/** @func  oyIconv
 *  @brief convert between codesets
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/05/27
 *  @since   2008/07/23 (Oyranos: 0.1.8)
 */
int                oyIconv           ( const char        * input,
                                       size_t              len,
                                       char              * output,
                                       const char        * from_codeset,
                                       const char        * to_codeset )
{
  int error = 0;

  char * out_txt = output;
  char * in_txt = (char*)input;
  const char * loc_env = 
# ifdef USE_GETTEXT
  setlocale( LC_MESSAGES, 0 )
# else
  0
# endif
  , *loc = to_codeset;
  iconv_t cd;
  size_t size, in_left = len, out_left = len;

  /* application codeset */
  if(!loc && oy_domain_codeset)
    loc = oy_domain_codeset;
  /* environment codeset */
  if(!loc && loc_env)
  {
    char * loc_tmp = strchr(loc_env, '.');
    if(loc_tmp && strlen(loc_tmp) > 2)
      loc = loc_tmp + 1;
  }
  /* fallback codeset */
  if(!loc)
    loc = "UTF-8";

  if(!from_codeset && !oy_domain_codeset)
  {
    error = !memcpy(output, input, sizeof(char) * len);
    output[len] = 0;
    return error;
  }

  cd = iconv_open( loc, from_codeset ? from_codeset : oy_domain_codeset );
  size = iconv( cd, &in_txt, &in_left, &out_txt, &out_left);
  iconv_close( cd );
  *out_txt = 0;

  if(size == (size_t)-1)
    error = -1;
  else
    error = size;

  return error;
}





/* Oyranos text handling */

/** @internal
 *  @brief query library paths
 *
 *  @param[out]    count       number of paths found
 *  @param[in]     owner       oyUSER/oySYS/oyUSER_SYS
 *  @return                    a array to write the found paths into
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
char**  oyLibPathsGet_( int             * count,
                        const char      * subdir,
                        int               owner,
                        oyAlloc_f         allocateFunc )
{
  char ** paths = 0, ** tmp;
  int     n = 0, tmp_n = 0;
  char *  vars[] = {"OY_MODULE_PATHS"};
  int     vars_n = 1;
  int     i;
  char  * fix_paths[3] = {0,0,0};
  int     fix_paths_n = 2;

  if(!subdir)
  {
    fix_paths[0] = OY_LIBDIR OY_SLASH OY_METASUBPATH;
    fix_paths[1] = OY_USER_PATH OY_SLASH "lib" OY_SLASH OY_METASUBPATH;
  } else {
    fix_paths[0] = oyStringCopy_(OY_LIBDIR OY_SLASH, oyAllocateFunc_);
    oyStringAdd_( &fix_paths[0], subdir, oyAllocateFunc_, oyDeAllocateFunc_);
    fix_paths[1] = oyStringCopy_(OY_USER_PATH OY_SLASH, oyAllocateFunc_);
    oyStringAdd_( &fix_paths[1], subdir, oyAllocateFunc_, oyDeAllocateFunc_);
  }

  oyStringListAdd_( &paths, &n, (const char**)fix_paths, fix_paths_n,
                    oyAllocateFunc_, oyDeAllocateFunc_ );

  for(i = 0; i < vars_n; ++i)
  {
    if( (i >= 2 && (owner != oyUSER)) ||
        (i < 2 && (owner != oySYS)))
    {
      if(getenv(vars[i]))
      {
        const char * var = getenv(vars[i]);

        if(strlen(var))
        {
          char **tmp_neu;
          int  tmp_neu_n;

          tmp = oyStringSplit_( var, ':', &tmp_n, oyAllocateFunc_ );

          tmp_neu = oyStringListAppend_( (const char**)paths, n, 
                                         (const char**)tmp, tmp_n,
                                         &tmp_neu_n, oyAllocateFunc_ );
          oyStringListRelease_( &paths, n, oyDeAllocateFunc_ );
          paths = tmp_neu;
          n = tmp_neu_n;
        }
      }
    }
  }

  tmp = oyStringListAppend_( 0, 0, (const char**)paths, n,
                             &tmp_n, allocateFunc );
  oyStringListRelease_( &paths, n, oyDeAllocateFunc_ );
  paths = tmp;
  n = tmp_n;

  if(fix_paths[0])
    oyFree_m_(fix_paths[0]);
  if(fix_paths[1])
    oyFree_m_(fix_paths[1]);

  if(count)
    *count = n;

  if(!n)
  {
    if(paths)
      oyDeAllocateFunc_(paths);
    paths = 0;
  }

  return paths;
}

/** @internal
 *  @brief query valid XDG paths
 *
 *  @see Directory paths for colour profiles and other configuration data
 *       http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal
 *
 *  @param[out]    count       number of paths found
 *  @param[in]     data        oyYES/oyNO/oyALL for data or config text
 *  @param[in]     owner       oyUSER/oySYS/oyUSER_SYS
 *  @return                    a array to write the found paths into
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
char**  oyXDGPathsGet_( int             * count,
                        int               data,
                        int               owner,
                        oyAlloc_f         allocateFunc )
{
  char ** paths = 0, ** tmp;
  int     n = 0, tmp_n = 0;
  char *  vars[] = {"XDG_DATA_HOME", "XDG_CONFIG_HOME", "XDG_DATA_DIRS", 
                    "XDG_CONFIG_DIRS"};
  int     vars_n = 4;
  int     i, j;

  for(i = 0; i < vars_n; ++i)
  {
    if( (i >= 2 && (owner != oyUSER)) ||
        (i < 2 && (owner != oySYS)))
      if( ((i == 0 || i == 2) && data != oyNO) ||
          ((i == 1 || i == 3) && data != oyYES) )
    {
      if(getenv(vars[i]))
      {
        const char * var = getenv(vars[i]);

        if(strlen(var))
        {
          char **tmp_neu;
          int  tmp_neu_n;


          tmp = oyStringSplit_( var, ':', &tmp_n, oyAllocateFunc_ );

          /* remove slash */
          for(j = 0; j < tmp_n; ++j)
          {
            char slash = 0;
            int len = 0;
            if(tmp[j])
              len = oyStrlen_(tmp[j]);
            if(len > 1)
              slash = tmp[j][len-1];
            if(slash == OY_SLASH_C)
              tmp[j][oyStrlen_(tmp[j])-1] = 0;
          }

          tmp_neu = oyStringListAppend_( (const char**)paths, n, 
                                         (const char**)tmp, tmp_n,
                                         &tmp_neu_n, oyAllocateFunc_ );
          oyStringListRelease_( &paths, n, oyDeAllocateFunc_ );
          oyStringListRelease_( &tmp, tmp_n, oyDeAllocateFunc_ );
          paths = tmp_neu;
          n = tmp_neu_n;
        }
      } else
      {
        if(i == 0)
          oyStringListAddStaticString_( &paths, &n, "~/.local/share",
                                        oyAllocateFunc_, oyDeAllocateFunc_ );
        if(i == 1)
          oyStringListAddStaticString_( &paths, &n, "~/.config",
                                        oyAllocateFunc_, oyDeAllocateFunc_ );
        if(i == 2)
        {
          oyStringListAddStaticString_( &paths, &n, "/usr/share",
                                        oyAllocateFunc_, oyDeAllocateFunc_ );
          oyStringListAddStaticString_( &paths, &n, "/usr/local/share",
                                        oyAllocateFunc_, oyDeAllocateFunc_ );
        }
        if(i == 4)
          oyStringListAddStaticString_( &paths, &n, "/etc/xdg",
                                        oyAllocateFunc_, oyDeAllocateFunc_ );
      }
    }
  }

  tmp = oyStringListAppend_( 0, 0, (const char**)paths, n,
                             &tmp_n, allocateFunc );
  oyStringListRelease_( &paths, n, oyDeAllocateFunc_ );
  paths = tmp;
  n = tmp_n;

  if(count)
    *count = n;

  if(!n)
  {
    if(paths)
      oyDeAllocateFunc_(paths);
    paths = 0;
  }

  return paths;
}

char * oyPathContructAndTest_(char * path_, const char * subdir)
{
  char * text = 0, * tmp = 0;
  int subdir_len = 0;

  if(!path_)
    return 0;

  if(subdir)
    subdir_len = oyStrlen_(subdir);

  oyAllocHelper_m_( text, char, MAX_PATH,
                    oyAllocateFunc_, return 0);
  if(subdir && (oyStrlen_(path_) + subdir_len + 10) < MAX_PATH)
     oySprintf_( text,
              "%s%s%s", path_, OY_SLASH, subdir );
  else if((oyStrlen_(path_) + subdir_len + 10) < MAX_PATH)
    oySprintf_( text,
                "%s", path_ );

  tmp = oyResolveDirFileName_( text );
  oyDeAllocateFunc_(text); text = tmp; tmp = 0;

  if(!oyIsDir_( text ))
  {
    oyDeAllocateFunc_(text);
    text = 0;
  }

  return text; 
}

/** @internal
 *  @brief query valid Oyranos paths
 *
 *  @param[out]    count       number of paths found
 *  @param[in]     subdir      the data directories sub path, a string 
 *                             containing one path, e.g. "color/icc""
 *  @param[in]     data        oyYES/oyNO/oyALL data or config text
 *  @param[in]     owner       oyUSER/oySYS/oyUSER_SYS
 *
 *  @version Oyranos: 0.1.11
 *  @date    2010/08/18
 *  @since   2007/11/00 (Oyranos: 0.1.x)
 */
char**
oyDataPathsGet_       (int             * count,
                       const char      * subdir,
                       int               data,
                       int               owner,
                       oyAlloc_f         allocateFunc )
{
  char ** paths = NULL;
  int ndp = 0;        /* number of default paths */
  static int init = 0;

  /* the OpenICC agreed upon *nix default paths */
  {
    int xdg_n = 0, oy_n = 0, tmp_n = 0, i,j,has;
    char ** oy_paths = 0;
    char ** xdg_paths = 0;
    char ** tmp_paths = 0;
    char * text = 0;
    char * xdg_sub = 0;

    init = 1;

    oyAllocHelper_m_( xdg_sub, char, MAX_PATH, oyAllocateFunc_, return 0);
    oySprintf_( xdg_sub, "%s", subdir );

    oyAllocHelper_m_( oy_paths, char*, 4, oyAllocateFunc_, return 0);

    xdg_paths = oyXDGPathsGet_(&xdg_n, data, owner, oyAllocateFunc_);

    if(xdg_n)
      oyAllocHelper_m_( tmp_paths, char*, xdg_n, oyAllocateFunc_, return 0);

    for(i = 0; i < xdg_n; ++i)
    {
      text = oyPathContructAndTest_(xdg_paths[i], xdg_sub);
      if(text)
        tmp_paths[tmp_n++] = text;
    }

    oyStringListRelease_(&xdg_paths, xdg_n, oyDeAllocateFunc_);
    xdg_paths = tmp_paths; tmp_paths = 0;
    xdg_n = tmp_n; tmp_n = 0;
    oyFree_m_(xdg_sub);

    ndp += xdg_n;

    text = oyPathContructAndTest_( "/usr/share", subdir );
    if(text) oy_paths[oy_n++] = text;
    text = oyPathContructAndTest_( "/usr/local/share", subdir );
    if(text) oy_paths[oy_n++] = text;
    text = 0;
    if(subdir && strlen(subdir) > 6 && memcmp( subdir,"color/", 6 ) == 0)
    text = oyPathContructAndTest_( OY_USERCOLORDIR, &subdir[6] );
    if(text) oy_paths[oy_n++] = text;
    text = oyPathContructAndTest_( OY_DATADIR, subdir );
    if(text) oy_paths[oy_n++] = text;

    paths = oyStringListAppend_(0,0, (const char**)oy_paths, oy_n,
                                &ndp, allocateFunc);
    for(i = 0; i < xdg_n; ++i)
    {
      has = 0;
      text = xdg_paths[i];

      for(j = 0; j < oy_n; ++j)
        if(text && oyStrcmp_( text, oy_paths[j] ) == 0)
        {
          has = 1;
          break;
        }

      if(!has)
        oyStringListAddStaticString_( &paths, &ndp, text,
                                      oyAllocateFunc_, oyDeAllocateFunc_ );
    }

    oyStringListRelease_(&oy_paths, oy_n, oyDeAllocateFunc_);
    oyStringListRelease_(&xdg_paths, xdg_n, oyDeAllocateFunc_);

  }

  *count = ndp;

  return paths;
}

char **     oyDataFilesGet_          ( int             * count,
                                       const char      * subdir,
                                       int               data,
                                       int               owner,
                                       const char      * dir_string,
                                       const char      * string,
                                       const char      * suffix,
                                       oyAlloc_f         allocateFunc )
{
  int l_n = 0;
  char** l = oyFileListGet_(subdir, &l_n, data, owner);
  int filt_n = 0;
  char ** filt = oyStringListFilter_( (const char**)l, l_n, 
                                      dir_string, string, suffix, &filt_n,
                                      allocateFunc );

  if(l && *l)
    oyStringListRelease_(&l, l_n, oyDeAllocateFunc_);

  if(count)
    *count = filt_n;
  return filt;
}

char **     oyLibFilesGet_           ( int             * count,
                                       const char      * subdir,
                                       int               owner,
                                       const char      * dir_string,
                                       const char      * string,
                                       const char      * suffix,
                                       oyAlloc_f         allocateFunc )
{
  int l_n = 0;
  char** l = oyLibListGet_(subdir, &l_n, owner);
  int filt_n = 0;
  char ** filt = oyStringListFilter_( (const char**)l, l_n, 
                                      dir_string, string, suffix, &filt_n,
                                      allocateFunc );

  if(l && *l)
    oyStringListRelease_(&l, l_n, oyDeAllocateFunc_);

  if(count)
    *count = filt_n;
  return filt;
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
    int count = 0;
    char * currentP = oyPolicyNameGet_ ();
    char ** list = oyPolicyListGet_( &count );
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

    if(!count)
    {
      WARNc_S( "\n  No policy found. Installation incomplete or path missing?" );
    } else
    if( choices_string_list )
    {
      char ** zl = NULL;
      char * pn = NULL, * t; 

      oyAllocHelper_m_( zl, oyChar*, count,
                        oyAllocateFunc_, return 0 );
      oyAllocString_m_( pn, MAX_PATH,
                        oyAllocateFunc_, return 0 );
      for( i = 0; i < count; ++i )
      {
        oyChar * filename = list[i];

        oySprintf_( pn, "%s", filename );

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
                    (oyWIDGET_e)oywid <= oyPROFILE_PROOF; ++oywid )
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
           lw[pos++] = oyWIDGET_RENDERING_HIGH_PRECISSION;

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

