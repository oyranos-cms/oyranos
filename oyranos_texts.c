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

  if( oyWIDGET_BEHAVIOUR_START < type && type < oyWIDGET_BEHAVIOUR_END )
  {
    if( opt->type != oyTYPE_BEHAVIOUR )
       WARN_S(("internal type: %d does not fit to option %d\n", oyOptionGet_(type)->type , type));
  }

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
  oyOption_t_ *opt;

  if(oy_option_)
    free(oy_option_);

  oy_option_= calloc(sizeof(oyOption_t_), OY_STATIC_OPTS_);
  opt = oy_option_;

  {
#   define oySET_OPTIONS_M_( t_, id_, ca_n, ca1, ca2, ca3, labl, desc, \
                             ch_n, ch0, ch1, ch2, ch3, \
                             conf, xml) { \
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
    }

    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_START, 0, 
      0, 0, 0,
      _("Start"),
      _("Oyranos Settings Group"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_START")
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_DEFAULT_PROFILES, 0, 
      0, 0, 0,
      _("Default Profiles"),
      _("Source and Target Profiles for various situations"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES")
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_DEFAULT_PROFILES_EDIT, 0, 
      0, 0, 0,
      _("Editing Colour Space"),
      _("Well behaving Colour Space for Editing"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES_EDIT")
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_DEFAULT_PROFILES_ASSUMED, 0, 
      0, 0, 0,
      _("Assumed Colour Space"),
      _("Assumed Colour Space for untagged colours"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES_ASSUMED")
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_DEFAULT_PROFILES_PROOF, 0, 
      0, 0, 0,
      _("Proofing Colour Space"),
      _("Colour Space for Simulating real devices"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_DEFAULT_PROFILES_PROOF")
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_PATHS, 0, 
      0, 0, 0,
      _("Paths"),
      _("Paths where ICC Profiles can be found"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_PATHS")
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_POLICY, 0, 
      0, 0, 0,
      _("Policy"),
      _("Collections of settings in Oyranos"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_POLICY")
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_BEHAVIOUR, 0, 
      0, 0, 0,
      _("Behaviour"),
      _("Settings affecting the Behaviour in various situations"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR")
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_BEHAVIOUR_RENDERING, 0, 
      0, 0, 0,
      _("Rendering"),
      _("The kind of ICC standard gamut mapping for transforming colours between differently sized colour spaces"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_RENDERING")
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, 0, 
      0, 0, 0,
      _("Save Mixed colour space Documents"),
      _("Default Handling of Mixed Colour Spaces inside one single Document"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS")
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_BEHAVIOUR_MISSMATCH, 0, 
      0, 0, 0,
      _("Mismatching"),
      _("Decide what to do when the default colour spaces dont match the current ones."),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_MISSMATCH")
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_BEHAVIOUR_PROOF, 0, 
      0, 0, 0,
      _("Proofing"),
      _("Default Proofing Settings"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_BEHAVIOUR_PROOF")
    oySET_OPTIONS_M_( oyTYPE_GROUP_TREE, oyGROUP_ALL, 0, 
      0, 0, 0,
      _("All"),
      _("Oyranos Settings"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      "oyGROUP_ALL")




    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_RGB, 1, 
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Editing Rgb"),
      _("Prefered Rgb Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_RGB_PROFILE,
      "oyEDITING_RGB")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_CMYK, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Editing Cmyk"),
      _("Prefered Cmyk Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_CMYK_PROFILE,
      "oyEDITING_CMYK")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_XYZ, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Editing XYZ"),
      _("Prefered XYZ Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_XYZ_PROFILE,
      "oyEDITING_XYZ")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_EDITING_LAB, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Editing Lab"),
      _("Prefered CIE*Lab Editing Colour Space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_EDITING_LAB_PROFILE,
      "oyEDITING_LAB")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_XYZ, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed XYZ source"),
      _("Assigning an untagged XYZ Image an colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_XYZ_PROFILE,
      "oyASSUMED_CMYK")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_LAB, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed Lab source"),
      _("Assigning an untagged CIE*Lab Image an colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_LAB_PROFILE,
      "oyASSUMED_LAB")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_RGB, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed Rgb source"),
      _("Assigning an untagged Rgb Image an colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_RGB_PROFILE,
      "oyASSUMED_RGB")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_WEB, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed Web source"),
      _("Assigning an untagged Rgb Image with source from the WWW an colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_WEB_PROFILE,
      "oyASSUMED_WEB")

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_ASSUMED_CMYK, 1,
      oyGROUP_DEFAULT_PROFILES, 0, 0,
      _("Assumed Cmyk source"),
      _("Assigning an untagged Cmyk Image this colour space"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_ASSUMED_CMYK_PROFILE,
      "oyASSUMED_CMYK" )

    oySET_OPTIONS_M_( oyTYPE_DEFAULT_PROFILE, oyWIDGET_PROFILE_PROOF, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("Proofing"),
      _("Colour space for Simulating an Output Device"),
      0, /* choices */
      NULL, NULL, NULL, NULL,
      OY_DEFAULT_PROOF_PROFILE,
      "oyPROFILE_PROOF" )


    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_ACTION_UNTAGGED_ASSIGN, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("No Image profile"),
      _("Image has no colour space embedded. What default action shall be performed?"),
      3, /* choices */
      _("Assign No Profile"),_("Assign Assumed Profile"),_("Promt"), NULL,
      OY_ACTION_UNTAGGED_ASSIGN,
      "oyBEHAVIOUR_ACTION_UNTAGGED_ASSIGN" )

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_ACTION_OPEN_MISMATCH_RGB, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("On Rgb Mismatch"),
      _("Action for Image profile and Editing profile mismatches."),
      3, /* choices */
      _("Preserve Numbers"),_("Convert automatically"),_("Promt"), NULL,
      OY_ACTION_MISMATCH_RGB,
      "oyBEHAVIOUR_ACTION_MISMATCH_RGB" )

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_ACTION_OPEN_MISMATCH_CMYK, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MISSMATCH, 0,
      _("On Cmyk Mismatch"),
      _("Action for Image profile and Editing profile mismatches."),
      3, /* choices */
      _("Preserve Numbers"),_("Convert automatically"),_("Promt"), NULL,
      OY_ACTION_MISMATCH_CMYK,
      "oyBEHAVIOUR_ACTION_MISMATCH_CMYK")

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_MIXED_MOD_DOCUMENTS_PRINT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, 0,
      _("For Print"),
      _("Handle Mixed colour spaces in Preparing a document for Print output."),
      4, /* choices */
      _("Preserve Numbers"),_("Convert to Default Cmyk Editing Space"),_("Convert to untagged Cmyk, preserving Cmyk numbers"),_("Promt"),
      OY_CONVERT_MIXED_COLOUR_SPACE_PRINT_DOCUMENT,
      "oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_PRINT")

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_MIXED_MOD_DOCUMENTS_SCREEN, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, 0,
      _("For Screen"),
      _("Handle Mixed colour spaces in Preparing a document for Screen output."),
      4, /* choices */
      _("Preserve Numbers"),_("Convert to Default Rgb Editing Space"),_("Convert to WWW (sRGB)"),_("Promt"),
      OY_CONVERT_MIXED_COLOUR_SPACE_SCREEN_DOCUMENT,
      "oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_SCREEN")

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_RENDERING_INTENT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_RENDERING, 0,
      _("Default Rendering Intent"),
      _("Rendering Intent for colour space Transformations."),
      4, /* choices */
      _("Perceptual"),_("Relative Colorimetric"),_("Saturation"),_("Absolute Colorimetric"),
      OY_DEFAULT_RENDERING_INTENT,
      "oyBEHAVIOUR_RENDERING_INTENT")

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_RENDERING_BPC, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_RENDERING, 0,
      _("Use Black Point Compensation"),
      _("BPC affects only the Relative Colorimetric Rendering Intent."),
      2, /* choices */
      _("No"),_("Yes"), NULL, NULL,
      OY_DEFAULT_RENDERING_BPC,
      "oyBEHAVIOUR_RENDERING_BPC")

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_RENDERING_INTENT_PROOF, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("Proofing Rendering Intent"),
      _("Behaviour of colour space transformation for proofing"),
      2, /* choices */
      _("Relative Colorimetric"),_("Absolute Colorimetric"),NULL,NULL,
      OY_DEFAULT_RENDERING_INTENT_PROOF,
      "oyBEHAVIOUR_RENDERING_INTENT_PROOF")

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_PROOF_SOFT, 2,
      oyGROUP_BEHAVIOUR, oyGROUP_BEHAVIOUR_PROOF, 0,
      _("SoftProof by Default"),
      _("Behaviour for Softproofing view at application startup"),
      2, /* choices */
      _("No"),_("Yes"),NULL,NULL,
      OY_DEFAULT_PROOF_SOFT,
      "oyBEHAVIOUR_PROOF_SOFT")

    oySET_OPTIONS_M_( oyTYPE_BEHAVIOUR, oyWIDGET_PROOF_HARD, 2,
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


const oyOption_t_*
oyOptionGet_                        (oyWIDGET          type)
{
  /* we provide allways a options at return to avoid further checks */
  static const oyOption_t_ default_opt;
  const oyOption_t_ *result = &default_opt;

  DBG_PROG_START

  oyOptionStringsTranslateCheck_ ();

  if(type < OY_STATIC_OPTS_)
    return &oy_option_[type];

  result = oyModulsUIOptionSearch_( type );

  DBG_PROG_ENDE
  return result;
}

oyWIDGET_TYPE oyWidgetTitleGet_        (oyWIDGET          option,
                                        const oyGROUP  ** categories,
                                        const char     ** name,
                                        const char     ** tooltip,
                                        int             * flags )
{
  DBG_PROG_START

  {
    if( categories )
    *categories           = oyOptionGet_(option)-> category;
    if( name )
    *name                 = oyOptionGet_(option)-> name;
    if( tooltip )
    *tooltip              = oyOptionGet_(option)-> description;
    if( flags )
    *flags                = oyOptionGet_(option)-> flags;
  }

  DBG_PROG_ENDE
  return oyOptionGet_(option)-> type;
}

int           oyOptionChoicesGet_      (oyWIDGET          option,
                                        int             * choices,
                                        const char    *** choices_string_list,
                                        int             * current)
{
  int error = 0;

  DBG_PROG_START

  if( oyWIDGET_BEHAVIOUR_START < option && option < oyWIDGET_BEHAVIOUR_END )
  {
    if( choices )
      *choices              = oyOptionGet_(option)-> choices;
    if( choices_string_list )
      *choices_string_list  = (const char**) oyOptionGet_(option)-> choice_list;
    if( current )
      *current              = oyGetBehaviour_( (oyBEHAVIOUR) option );
  }
  else
  if( oyWIDGET_DEFAULT_PROFILE_START < option &&
      option < oyWIDGET_DEFAULT_PROFILE_END )
  {
    char * default_p =
           oyGetDefaultProfileName( (oyDEFAULT_PROFILE)option, oyAllocateFunc_);
    int i, val = -1, occurence = 0, count = 0;
    char** names = /*(const char**)*/ oyProfileListGet_ ( NULL, &count );
    char** dup = (char**) oyAllocateFunc_( count*sizeof(char*) );
    int dup_count = 0;

    if(default_p)
    {
      int dup_pos = 0;
      for (i = 0; i < count; ++i)
      {
        int k;
        int double_name = 0;
        for(k = 0; k < count; ++k)
          if(strcmp( names[i], names[k]) == 0 && 
             strlen( names[i] ) &&
             i < k )
            double_name = 1;

        if(!double_name)
        {
          dup[dup_pos] = oyAllocateFunc_(strlen(names[i])+1);
          sprintf( dup[dup_pos], "%s", names[i]);
          ++dup_pos;
        }

        if(strstr( names[i], default_p) &&
           strlen( names[i] ) == strlen(default_p))
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
        WARN_S((_("multiple occurencies of default %s profile: %d times"),
                  default_p, occurence))
    }
    if( choices )
      *choices              = dup_count;

    if( choices_string_list )
      *choices_string_list = (const char **)dup; 
    else
      oyOptionChoicesFree( oyOptionGet_(option)-> type, &dup,
                           dup_count );
    oyOptionChoicesFree( oyOptionGet_(option)-> type, &names, count );

    if(current)
      *current          = val;
    oyDeAllocateFunc_( default_p );
  }
  else
  if( option == oyWIDGET_POLICY )
  { //@todo TODO
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
      if ( (*list)[i] )
        free( (*list)[i] );
    free( *list );
    *list = NULL;
  }

  DBG_PROG_ENDE
}

oyWIDGET    * oyWidgetListGet_         (oyGROUP           group,
                                        int             * count)
{
#define oyGROUP_DEFAULT_PROFILES_LEN oyWIDGET_DEFAULT_PROFILE_END - oyWIDGET_DEFAULT_PROFILE_START
#define oyGROUP_BEHAVIOUR_RENDERING_LEN 2
#define oyGROUP_BEHAVIOUR_PROOF_LEN 4
#define oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS_LEN 6
#define oyGROUP_BEHAVIOUR_MISSMATCH_LEN 6

#define oyGROUP_BEHAVIOUR_LEN oyGROUP_BEHAVIOUR_RENDERING_LEN + oyGROUP_BEHAVIOUR_PROOF_LEN + oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS_LEN + oyGROUP_BEHAVIOUR_MISSMATCH_LEN

#define oyGROUP_ALL_LEN oyGROUP_DEFAULT_PROFILES_LEN + oyGROUP_BEHAVIOUR_LEN

  oyWIDGET *w = NULL;
  oyWIDGET i;

  DBG_PROG_START

  if(!count)
    return NULL;

  *count = 0;


  switch(group)
  { case oyGROUP_DEFAULT_PROFILES:
         { int pos = 0;
           static oyWIDGET lw[oyGROUP_DEFAULT_PROFILES_LEN];

           for( i = oyWIDGET_DEFAULT_PROFILE_START + 1;
                  i < oyWIDGET_PROFILE_PROOF; ++i)
           {
             lw[pos++] = i;
           }
           *count = oyWIDGET_DEFAULT_PROFILE_END
                              -oyWIDGET_DEFAULT_PROFILE_START - 1;
           w = lw;
         }
         break;
    case oyGROUP_BEHAVIOUR_RENDERING:
         { int pos = 0;
           static oyWIDGET lw[oyGROUP_BEHAVIOUR_RENDERING_LEN];

           lw[pos++] = oyWIDGET_RENDERING_INTENT;
           lw[pos++] = oyWIDGET_RENDERING_BPC;

           *count = pos;
           w = lw;
         }
         break;
    case oyGROUP_BEHAVIOUR_PROOF:
         { int pos = 0;
           static oyWIDGET lw[oyGROUP_BEHAVIOUR_PROOF_LEN];

           lw[pos++] = oyWIDGET_PROFILE_PROOF;
           lw[pos++] = oyWIDGET_RENDERING_INTENT_PROOF;
           lw[pos++] = oyWIDGET_PROOF_SOFT;
           lw[pos++] = oyWIDGET_PROOF_HARD;

           *count = pos;
           w = lw;
         }
         break;
    case oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS:
         { int pos = 0;
           static oyWIDGET lw[oyGROUP_BEHAVIOUR_PROOF_LEN];

           lw[pos++] = oyWIDGET_MIXED_MOD_DOCUMENTS_SCREEN;
           lw[pos++] = oyWIDGET_MIXED_MOD_DOCUMENTS_PRINT;

           *count = pos;
           w = lw;
         }
         break;
    case oyGROUP_BEHAVIOUR_MISSMATCH:
         { int pos = 0;
           static oyWIDGET lw[oyGROUP_BEHAVIOUR_PROOF_LEN];

           lw[pos++] = oyWIDGET_ACTION_UNTAGGED_ASSIGN;
           lw[pos++] = oyWIDGET_ACTION_OPEN_MISMATCH_RGB;
           lw[pos++] = oyWIDGET_ACTION_OPEN_MISMATCH_CMYK;

           *count = pos;
           w = lw;
         }
         break;
    case oyGROUP_BEHAVIOUR:
         { int pos = 0;
           static oyWIDGET lw[oyGROUP_BEHAVIOUR_LEN];
           int n;
           oyWIDGET *tmp;

           tmp = oyWidgetListGet_(oyGROUP_BEHAVIOUR_RENDERING, &n);
           for(i = 0; i < n; ++i)
             lw[pos++] = tmp[i];
           tmp = oyWidgetListGet_(oyGROUP_BEHAVIOUR_MISSMATCH, &n);
           for(i = 0; i < n; ++i)
             lw[pos++] = tmp[i];
           tmp = oyWidgetListGet_(oyGROUP_BEHAVIOUR_PROOF, &n);
           for(i = 0; i < n; ++i)
             lw[pos++] = tmp[i];
           tmp = oyWidgetListGet_(oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS, &n);
           for(i = 0; i < n; ++i)
             lw[pos++] = tmp[i];

           *count = pos;
           w = lw;
         }
         break;
    case oyGROUP_ALL:
         { int pos = 0;
           static oyWIDGET lw[oyGROUP_ALL_LEN];
           int n;
           oyWIDGET *tmp;

           tmp = oyWidgetListGet_(oyGROUP_DEFAULT_PROFILES, &n);
           for(i = 0; i < n; ++i)
             lw[pos++] = tmp[i];
           tmp = oyWidgetListGet_(oyGROUP_BEHAVIOUR, &n);
           for(i = 0; i < n; ++i)
             lw[pos++] = tmp[i];

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
  const char ***ptr = calloc(sizeof(char***), ++oy_groups_descriptions_);
  const char **desc = calloc(sizeof(char**), 3);
  int i;

  DBG_PROG_START

  oyTextsCheck_ ();

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
oyTestInsideBehaviourOptions_ (oyBEHAVIOUR type, int choice)
{
  int r = 0;

  DBG_PROG_START

  DBG_PROG_S( ("type = %d behaviour %d", type, choice) )

  if ( oyWidgetTypeGet_( type ) == oyTYPE_BEHAVIOUR )
  {
    if ( choice >= 0 &&
         choice < oyOptionGet_(type)-> choices )
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
  if (choices) *choices = oyOptionGet_(type)-> choices;

  if ( oyTestInsideBehaviourOptions_(type, choice) )
  { *option_string = oyOptionGet_(type)-> choice_list[ choice ];
    *category = "API is broken";
    *tooltip =  oyOptionGet_(type)-> description;
    DBG_PROG_ENDE
    return oyOptionGet_(type)-> name;
  }
  DBG_PROG_ENDE
  return NULL;
}


