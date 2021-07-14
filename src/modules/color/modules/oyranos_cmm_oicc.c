/** @file oyranos_cmm_oicc.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2008-2018 (C) Kai-Uwe Behrmann
 *
 *  @brief    color management policy module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2008/12/16
 */

/** \addtogroup oicc_policy oicc Module
 *
 *  The handled options are listed in @ref oicc_default_color_icc_options .
 *  Descriptions for the options can be found in @ref defaults_apis and
 *  in the @ref oyranos_definitions.h header file.
 *  The registration string for this module is "//" OY_TYPE_STD "/icc_color".
 *  @{ */

#include "oyranos_config_internal.h"
#include "oyranos_oicc.h"

#include "oyCMM_s.h"
#include "oyCMMapi9_s_.h"
#include "oyCMMobjectType_s_.h"
#include "oyProfiles_s.h"

#include "oyranos_cmm.h"
#include "oyranos_definitions.h"
#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_json.h"
#include "oyranos_module_internal.h"
#include "oyranos_monitor_effect.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if !defined(_WIN32)
#include <dlfcn.h>
#endif

oyMessage_f oicc_msg = oyMessageFunc;
int            oiccFilterMessageFuncSet( oyMessage_f       message_func );
int                oiccFilterInit    ( oyStruct_s        * filter );
int                oiccFilterReset   ( oyStruct_s        * filter );
oyWIDGET_EVENT_e   oiccWidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event );

/** Function oiccFilterMessageFuncSet
 *  @brief
 *
 *  @version Oyranos: 0.1.10
 *  @date    2007/11/00
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 */
int          oiccFilterMessageFuncSet( oyMessage_f         message_func )
{
  oicc_msg = message_func;
  return 0;
}


/** Function oiccFilterInit
 *  @brief   API requirement
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/24 (Oyranos: 0.1.10)
 *  @date    2009/12/17
 */
int                oiccFilterInit    ( oyStruct_s        * filter OY_UNUSED )
{
  int error = 0;
  return error;
}
int                oiccFilterReset   ( oyStruct_s        * filter OY_UNUSED ) { int error = 0; return error; }

/** Function oiccInit
 *  @brief   API requirement
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/12/11 (Oyranos: 0.9.5)
 *  @date    2013/12/11
 */
int                oiccInit          ( oyStruct_s        * filter OY_UNUSED )
{
  int error = 0;
  return error;
}

/** Function oicc_defaultICCValidateOptions
 *  @brief   dummy
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/13 (Oyranos: 0.1.9)
 *  @date    2008/11/13
 */
oyOptions_s* oicc_defaultICCValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate OY_UNUSED,
                                       int                 statical OY_UNUSED,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  *result = error;

  return 0;
}

#define jADD( value_, key_, ... ) \
{ \
  oyjl_val v = oyjlTreeGetValueF( group, OYJL_CREATE_NEW, key_, __VA_ARGS__ ); \
  oyjlValueSetString( v, value_ ); \
}

/** @showinitializer
 <xf:model> <xf:instance> - must be added in Oyranos to make the model complete
 */
char oicc_default_color_icc_options[] = {
"{\n\
  \"" OY_TOP_SHARED "\": {\n\
   \"" OY_DOMAIN_STD "\": {\n\
    \"" OY_TYPE_STD "\": {\n\
     \"" "profile" "\": {\n\
      \"editing_rgb.front\": \"eciRGB_v2.icc\",\n\
      \"editing_gray.front\": \"Gray.icc\",\n\
      \"editing_lab.front\": \"LCMSLABI.ICM\",\n\
      \"editing_xyz.front\": \"LCMSXYZI.ICM\",\n\
      \"assumed_rgb.front\": \"eciRGB_v2.icc\",\n\
      \"assumed_cmyk.front\": \"ISOcoated_v2_bas.ICC\",\n\
      \"assumed_gray.front\": \"Gray.icc\",\n\
      \"assumed_lab.front\": \"LCMSLABI.ICM\",\n\
      \"assumed_web.front\": \"sRGB.icc\",\n\
      \"assumed_xyz.front\": \"LCMSXYZI.ICM\",\n\
      \"proof.advanced.front\": \"0\",\n\
      \"effect.advanced.front\": \"0\"\n\
     },\n\
     \"" "behaviour" "\": {\n\
      \"action_untagged_assign.front\": \"1\",\n\
      \"action_missmatch_cmyk.front\": \"1\",\n\
      \"action_missmatch_rgb.front\": \"1\",\n\
      \"mixed_color_spaces_print_doc_convert.front\": \"1\",\n\
      \"mixed_color_spaces_screen_doc_convert.front\": \"2\",\n\
      \"proof_hard.advanced\": \"0\",\n\
      \"proof_soft.advanced\": \"0\",\n\
      \"effect_switch\": \"0\",\n\
      \"display_white_point\": \"0\",\n\
      \"rendering_intent\": \"0\",\n\
      \"rendering_bpc\": \"1\",\n\
      \"rendering_intent_proof.advanced\": \"0\",\n\
      \"rendering_gamut_warning.advanced\": \"0\"\n\
     },\n\
     \"" "icc_color" "\": {\n\
      \"context.advanced\": \"///icc_color.lcm2\",\n\
      \"renderer.advanced\": \"///icc_color.lcm2\"\n\
     }\n\
    }\n\
   }\n\
  }\n\
}\n"
};

#define A(long_text) STRING_ADD( tmp, long_text)

int  oiccGetDefaultColorIccOptionsUI ( oyCMMapiFilter_s  * module OY_UNUSED,
                                       oyOptions_s       * options,
                                       int                 aflags,
                                       char             ** ui_text,
                                       oyAlloc_f           allocateFunc )
{
  char * tmp = 0;
  oyjl_val root = NULL;
  oyjl_val group = NULL;
  int jgroup = -1, jgroup2 = -1, jopt = -1, jchoice = -1;
  oyOptions_s * os = options,
              * defaults = oyOptions_FromText( oicc_default_color_icc_options, 0, NULL );
  const oyOption_t_ * t = 0;
  int n, i = 0,j;
#if 0
  const char * policy_name = 0;
#endif
  const char * value;
  oyWIDGET_e *wl = oyWidgetListGet( oyGROUP_ALL, &n, oyAllocateFunc_ );

  /* Sections or headline are placed as needed. They come before the options.
   * We have to remember in the below variables for lets say three levels.
   */
  int section_group[3] = {0,0,0};
  const char * section_names[3] = {0,0,0},
             * section_tooltips[3] = {0,0,0},
             * section_description[3] = {0,0,0};

  char num[12];
  oyProfile_s * p = 0;
  int32_t icc_profile_flags = 0;

  oyOptions_FindInt( options, "icc_profile_flags", 0, &icc_profile_flags );

  if(aflags & oyNAME_JSON)
    root = oyjlTreeNew(OY_STD);

  /* fill in all the options */
  for( i = 0 ; i < n ; ++i )
  {
        const oyGROUP_e * groups;
        int             count = 0,
                        current = 0,
                        flags = 0;
        const char    * tooltip = NULL,
                      * description = NULL;
        const char   ** names = NULL;
        const char    * name = NULL;
        oyWIDGET_e        oywid = wl[i];
        oyWIDGET_TYPE_e   type = oyWidgetTitleGet(  oywid,
                                                    &groups, 0, 0,
                                                    &flags );

        if(type == oyWIDGETTYPE_CHOICE ||
           type == oyWIDGETTYPE_BEHAVIOUR ||
           type == oyWIDGETTYPE_DEFAULT_PROFILE ||
           type == oyWIDGETTYPE_PROFILE ||
           type == oyWIDGETTYPE_LIST)
        {
          oyOptionChoicesGet2( oywid, icc_profile_flags, oyNAME_NAME, &count, &names, &current );
          type = oyWidgetTitleGet(  oywid, &groups, &name, &tooltip, &flags );
          oyWidgetDescriptionGet( oywid, &description, 0 );

          /* TODO: When and how to display policy informations? */
#if 0
          if(oywid == oyWIDGET_POLICY)
          {
            if(count && names && current >= 0)
              policy_name =  names[current];
            else
              policy_name =  _("----");
          }
#endif

          t = oyOptionGet_( oywid );
          value = oyOptions_FindString(os, t->config_string, 0);

          /* print the headlines; currently for two levels */
          if(section_names[groups[0] - 1] && value)
          {
            if(groups[0] > 0 && section_names[0])
            {
              if(section_group[0])
              {
                if(!(aflags & oyNAME_JSON))
                   A("\
 </xf:group>\n");
                section_group[0] = 0;
              }
              if(aflags & oyNAME_JSON)
              {
                char * t = NULL;
                oyjlStringAdd( &t, 0,0, "Oyranos %s", section_names[0] );
                ++jgroup;
                group = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, "org/freedesktop/oyjl/modules/[0]/groups/[%d]", jgroup );
                jADD( t, "%s", "name" )
                free(t); t = NULL;
                jADD( section_tooltips[0], "%s", "description" )
                jADD( section_description[0], "%s", "help" )
                jADD( "frame.h3", "%s", "properties" )
                jopt = -1;
                if(oy_debug)
                  fprintf(stderr, "add group1[%d]: %s\n", jgroup, section_names[0]);
              }
              else
              {
              A("\
 <xf:group type=\"frame\">\n");
              A("\
  <xf:label>Oyranos " );
              A(section_names[0]);
              A(             ":</xf:label>\n\
  <xf:hint>" );
              A( section_tooltips[0] );
              A(      "</xf:hint>\n\
  <xf:help>" );
              A( section_description[0] );
              A(      "</xf:help>\n");
              }
              /* The headline is in place. The reference shall be removed. */
              section_names[0] = 0;
              /* Remember to later close that group */
              section_group[0] = 1;
            }
            if(groups[0] > 1 && section_names[1])
            {
              if(groups[0] > 1 && section_group[1])
              {
                if(!(aflags & oyNAME_JSON))
                A("\
  </xf:group>\n");
                section_group[1] = 0;
              }
              if(aflags & oyNAME_JSON)
              {
                ++jgroup2;
                group = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OY_STD "/modules/[0]/groups/[%d]/groups/[%d]", jgroup, jgroup2 );
                jADD( section_names[1], "%s", "name" )
                //jADD( section_tooltips[1], OY_STD "/modules/[0]/groups/[0]/groups/[%d]/%s", i, "description" )
                jADD( section_description[1], "%s", "help" )
                jopt = -1;
                if(oy_debug)
                  fprintf(stderr, "   add group2[%d]: %s\n", jgroup2, section_names[1]);
              }
              else
              {
              A("\
  <xf:group type=\"frame\">\n");
   /*<h4>");
              A(section_names[1]);
              A(              "</h4>\n");*/
              A("\
   <xf:label>" );
              A( section_names[1] );
              A(      "</xf:label>\n\
   <xf:hint>" );
              A( section_tooltips[1] );
              A(      "</xf:hint>\n\
   <xf:help>" );
              A( section_description[1] );
              A(      "</xf:help>\n");
              }
              section_names[1] = 0;
              section_group[1] = 1;
            }
          }

          /* show the option */
          if(value != NULL)
          {
            if(aflags & oyNAME_JSON)
            {
              const char * d = oyOptions_FindString( defaults, t->config_string, NULL );
              ++jopt;
              jADD( t->config_string, "options/[%d]/%s", jopt, "key" )
              jADD( d, "options/[%d]/%s", jopt, "default" )
              jADD( "choice", "options/[%d]/%s", jopt, "type" )
              jADD( name, "options/[%d]/%s", jopt, "name" )
              jADD( tooltip, "options/[%d]/%s", jopt, "description" )
              jADD( description, "options/[%d]/%s", jopt, "help" )
              jchoice = -1;
              if(oy_debug)
                fprintf(stderr, "      add option[%d]: %s\n%s\n", jopt, name, oyJsonPrint(group) );
            }
            else
            {
            A("\
     <xf:select1 ref=\"/");
            A(         t->config_string );
            A(                        "\">\n\
      <xf:label>" );
            A( name );
            A(                        "</xf:label>\n\
      <xf:hint>" );
            A( tooltip );
            A(        "</xf:hint>\n\
      <xf:help>" );
            A( description );
            A(        "</xf:help>\n\
      <xf:choices>\n");
            }

            /* add selection items */
            for(j = 0; j < count; ++j)
            {
              if(type == oyWIDGETTYPE_DEFAULT_PROFILE ||
                 type == oyWIDGETTYPE_PROFILE)
              {
                const char * profile_text,
                           * file_name;

                p = oyProfile_FromFile( names[j], 0, 0 );
                /* add the internal name ... */
                profile_text = oyProfile_GetText( p, oyNAME_DESCRIPTION );

                file_name = oyProfile_GetFileName( p, 0 );
                if(aflags & oyNAME_JSON)
                {
                  ++jchoice;
                  if(file_name)
                  {
                    char * t = NULL; oyStringAddPrintf( &t, oyAllocateFunc_, oyDeAllocateFunc_, "%s (%s)", profile_text, file_name );
                    jADD( t, "options/[%d]/choices/[%d]/name", jopt, jchoice )
                    oyFree_m_( t );
                  }
                  else
                  jADD( profile_text, "options/[%d]/choices/[%d]/name", jopt, jchoice )
                  jADD( names[j], "options/[%d]/choices/[%d]/nick", jopt, jchoice )
                  if(oy_debug)
                  fprintf(stderr, "         add choice[%d]: %s\n", jchoice, names[j]);
                }
                else
                {
                A("\
       <xf:item>\n\
        <xf:label>");
                A( profile_text );
                /* ... and the full file name */
                if(file_name)
                {
                  A(         " (");
                  A(            file_name);
                  A(                    ")");
                }
                A(                    "</xf:label>\n\
        <xf:value>");
                A( names[j] );
                A(                    "</xf:value>\n\
       </xf:item>\n");
                }
                oyProfile_Release( &p );

              } else if(oyWIDGET_CMM_START < oywid && oywid < oyWIDGET_CMM_END)
              {
                char * reg = oyCMMNameToRegistration( names[j], (oyCMM_e)oywid, oyNAME_NAME, 0, oyAllocateFunc_ );
                char * t = NULL;
                const char * node_pattern = NULL;

                if(oywid == oyWIDGET_CMM_CONTEXT)
                  node_pattern = oyOptions_FindString( options, OY_DEFAULT_CMM_CONTEXT, NULL );
                else if(oywid == oyWIDGET_CMM_CONTEXT_FALLBACK)
                  node_pattern = oyOptions_FindString( options, OY_DEFAULT_CMM_CONTEXT_FALLBACK, NULL );
                else if(oywid == oyWIDGET_CMM_RENDERER)
                  node_pattern = oyOptions_FindString( options, OY_DEFAULT_CMM_RENDERER, NULL );
                else if(oywid == oyWIDGET_CMM_RENDERER_FALLBACK)
                  node_pattern = oyOptions_FindString( options, OY_DEFAULT_CMM_RENDERER_FALLBACK, NULL );

                if(node_pattern && oyFilterRegistrationMatch( reg, node_pattern, 0 ))
                  t = oyStringCopy( node_pattern, oyAllocateFunc_ );
                else if(j == current)
                  t = oyGetCMMPattern( (oyCMM_e)oywid, 0, oyAllocateFunc_ );
                else
                  t = oyCMMRegistrationToName( reg, (oyCMM_e)oywid, oyNAME_PATTERN, 0, oyAllocateFunc_ );

                if(aflags & oyNAME_JSON)
                {
                  ++jchoice;
                  jADD( names[j], "options/[%d]/choices/[%d]/name", jopt, jchoice )
                  jADD( t, "options/[%d]/choices/[%d]/nick", jopt, jchoice )
                  if(oy_debug)
                    fprintf(stderr, "         add choice[%d]: %s\n", jchoice, names[j]);
                }
                else
                {
                A("\
       <xf:item>\n\
        <xf:label>");
                A( names[j] );
                A(                    "</xf:label>\n\
        <xf:value>");
                /* take the item position as value */
                A( t );
                A(                    "</xf:value>\n\
       </xf:item>\n");
                oyFree_m_( reg );
                oyFree_m_( t );
                }
              } else
              {
                sprintf( num, "%d", j );
                if(aflags & oyNAME_JSON)
                {
                  ++jchoice;
                  jADD( names[j], "options/[%d]/choices/[%d]/name", jopt, jchoice )
                  jADD( num, "options/[%d]/choices/[%d]/nick", jopt, jchoice )
                  if(oy_debug)
                    fprintf(stderr, "         add choice[%d]: %s\n", jchoice, names[j]);
                }
                else
                {
                A("\
       <xf:item>\n\
        <xf:label>");
                A( names[j] );
                A(                    "</xf:label>\n\
        <xf:value>");
                /* take the item position as value */
                A( num );
                A(                    "</xf:value>\n\
       </xf:item>\n");
                }
              }
            }
            if(!(aflags & oyNAME_JSON))
            A("\
      </xf:choices>\n\
     </xf:select1>\n");

          } else if(oy_debug)
            /* tell about missed options */
            fprintf( stderr, "  %d: %s %s\n", __LINE__,
                  t->config_string?t->config_string:"-", name );

        /* remember the section headlines */
        } else if(type == oyWIDGETTYPE_GROUP_TREE)
        {
          type = oyWidgetTitleGet(  oywid, &groups, &name, &tooltip, &flags );
          oyWidgetDescriptionGet( oywid, &description, 0 );
          section_names[groups[0]] = name;
          section_tooltips[groups[0]] = tooltip;
          section_description[groups[0]] = description;
        }
  }

  /* close open sections */
  if(!(aflags & oyNAME_JSON))
  {
  if(section_group[1])
            {
              A("\
  </xf:group>\n");
              section_group[1] = 0;
            }
  if(section_group[0])
            {
              A("\
 </xf:group>\n");
              section_group[0] = 0;
            }
  }

  if(aflags & oyNAME_JSON)
  {
    int level = 0;
    oyjlTreeToJson( root, &level, &tmp );
  }

  if(allocateFunc && tmp)
  {
    char * t = oyStringCopy_( tmp, allocateFunc );
    oyFree_m_( tmp );
    tmp = t; t = 0;
  } else
    return 1;

  *ui_text = tmp;
  oyOptions_Release( &defaults );

  return 0;
}

oyWIDGET_EVENT_e   oiccWidgetEvent   ( oyOptions_s       * options OY_UNUSED,
                                       oyWIDGET_EVENT_e    type OY_UNUSED,
                                       oyStruct_s        * event OY_UNUSED )
{return 0;}


const char * oiccProfileGetText      ( oyStruct_s        * obj,
                                       oyNAME_e            type,
                                       int                 flags OY_UNUSED )
{
  const char * text = 0;

  if(!obj)
  {
    if(type == oyNAME_NAME)
      text = _("ICC profile");
    else if(type == oyNAME_DESCRIPTION)
      text = _("ICC color profile for color transformations");
    else
      text = "oyProfile_s";
  } else
  {
    if(obj &&
       obj->type_ == oyOBJECT_PROFILE_S)
      text = oyProfile_GetText( (oyProfile_s*)obj, type );
  }

  return text;
}

/** @brief   get a presentable name
 *  @memberof oyProfiles_s
 *
 *  The type argument should select the following string in return: \n
 *  - oyNAME_NAME - a readable XML element
 *  - oyNAME_NICK - the hash ID
 *  - oyNAME_DESCRIPTION - profiles internal name (icSigProfileDescriptionTag)
 *  - oyNAME_JSON - JSON formatted string supporting nested JSON
 *
 *  @version Oyranos: 0.9.7
 *  @date    2020/03/20
 *  @since   2011/10/31 (Oyranos: 0.3.3)
 */
const char * oiccProfilesGetText     ( oyStruct_s        * obj,
                                       oyNAME_e            type,
                                       int                 flags OY_UNUSED )
{
  const char * text = 0, * t = 0;
  char * tmp = 0;
  int i, n,
      error = 0;
  oyProfiles_s * profiles = 0;

  if(!obj)
  {
    if(type == oyNAME_NAME)
      text = _("ICC profiles");
    else if(type == oyNAME_DESCRIPTION)
      text = _("ICC color profiles for color transformations");
    else
      text = "oyProfiles_s";
  } else
  {
    if(obj &&
       obj->type_ == oyOBJECT_PROFILES_S)
    {
      if(type == oyNAME_NAME)
        STRING_ADD( tmp, "<profiles>\n" );
      else if((int)type == oyNAME_JSON)
        STRING_ADD( tmp, "{ \"profiles\": [\n" );
      profiles = (oyProfiles_s*)obj;
      n = oyProfiles_Count( profiles );
      for(i = 0; i < n; ++i)
      {
        oyProfile_s * p = oyProfiles_Get( profiles, i );
        t = oyProfile_GetText( p, type );
        if(t)
          oyStringAddPrintf_( &tmp, oyAllocateFunc_, oyDeAllocateFunc_,
                              "  %s\n", t );
        else
          STRING_ADD( tmp,    "  no info available\n" );
        if((int)type == oyNAME_JSON && i < n-1)
          oyStringAddPrintf_( &tmp, oyAllocateFunc_, oyDeAllocateFunc_,
                              ",\n", t );
        oyProfile_Release( &p );
      }
      if(type == oyNAME_NAME)
        STRING_ADD( tmp, "</profiles>" );
      else if( (int)type == oyNAME_JSON)
        STRING_ADD( tmp, "\n] }" );

      if(tmp && error <= 0)
        error = oyObject_SetName( profiles->oy_, tmp, type );

      if(tmp)
        oyFree_m_( tmp );

      if(error <= 0)
        text = oyObject_GetName( profiles->oy_, type );
    }
  }

  return text;
}

/** Function oiccObjectLoadFromMem
 *  @brief   load a ICC profile from a in memory data blob
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/23 (Oyranos: 0.1.9)
 *  @date    2009/09/14
 */
oyStruct_s * oiccProfileLoadFromMem   ( size_t              buf_size,
                                       const oyPointer     buf,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  return (oyStruct_s*) oyProfile_FromMem( buf_size, buf, flags, object );
}

/** Function oiccObjectScan
 *  @brief   load ICC profile informations from a in memory data blob
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/23 (Oyranos: 0.1.9)
 *  @date    2009/09/14
 */
int          oiccObjectScan          ( oyPointer           buf,
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

  oyProfile_Release( &temp_prof );

  return error;
}

oyCMMobjectType_s_ icc_profile = {
  oyOBJECT_CMM_DATA_TYPES_S, /* oyStruct_s::type; */
  0,0,0, /* unused oyStruct_s fields in static data; keep to zero */
  oyOBJECT_PROFILE_S, /* id; */
  "color/icc", /* paths; sub paths */
  0, /* pathsGet */
  "icc:icm", /* exts; file name extensions */
  "profile", /* element_name; in XML documents */
  oiccProfileGetText, /* oyCMMobjectGetText; */
  oiccProfileLoadFromMem, /* oyCMMobjectLoadFromMem; */
  oiccObjectScan /* oyCMMobjectScan; */
};
oyCMMobjectType_s_ icc_profiles = {
  oyOBJECT_CMM_DATA_TYPES_S, /* oyStruct_s::type; */
  0,0,0, /* unused oyStruct_s fields in static data; keep to zero */
  oyOBJECT_PROFILES_S, /* id; */
  "color/icc", /* paths; sub paths */
  0, /* pathsGet */
  NULL, /* exts; file name extensions */
  "profiles", /* element_name; in XML documents */
  oiccProfilesGetText, /* oyCMMobjectGetText; */
  0, /* oyCMMobjectLoadFromMem; */
  0 /* oyCMMobjectScan; */
};
oyCMMobjectType_s_ * icc_objects[] = {
  &icc_profile,
  &icc_profiles,
  0
};


void             oiccChangeNodeOption( oyOptions_s       * f_options,
                                       oyOptions_s       * db_options,
                                       const char        * key,
                                       oyConversion_s    * cc OY_UNUSED,
                                       int                 verbose,
                                       int                 flags,
                                       int                 type )
{
  oyOption_s * o, * db_o;
  const char * tmp = 0;
  char * text = NULL, * db_text = NULL;

  o = oyOptions_Find( f_options, key, oyNAME_PATTERN );
  /** Set missing options and overwrite filter inbuild fallbacks.
   *  Do not touch edits. */
              if((o &&
                  oyOption_GetSource(o) == oyOPTIONSOURCE_FILTER &&
                  !(oyOption_GetFlags(o) & oyOPTIONATTRIBUTE_EDIT)) ||
                 !o)
              {
                db_o = oyOptions_Find( db_options, key, oyNAME_PATTERN );
                if(db_o)
                {
                  if(!o)
                  {
                    oyOption_SetFlags(db_o, oyOption_GetFlags(db_o) | oyOPTIONATTRIBUTE_AUTOMATIC);
                    oyOptions_MoveIn( f_options, &db_o, -1 );
                  }
                  else
                  {
                    db_text = oyOption_GetValueText( db_o, oyAllocateFunc_ );
                    text = oyOption_GetValueText( o, oyAllocateFunc_ );
                    if(db_text && !(text && strcmp(text,db_text) == 0))
                    {
                      oyOption_SetFromString( o, db_text, 0 );
                      oyOption_SetFlags(o, oyOption_GetFlags(o) | oyOPTIONATTRIBUTE_AUTOMATIC);
                    }
                    oyFree_m_( db_text );
                    oyFree_m_( text );
                  }

                  if(oy_debug > 2 || verbose)
                    oicc_msg( oyMSG_DBG,(oyStruct_s*)f_options,
                              OY_DBG_FORMAT_"set %s: %s", OY_DBG_ARGS_, key,
                              oyOptions_FindString(f_options,
                                                  key, 0) );

                } else if((flags & oyOPTIONATTRIBUTE_ADVANCED &&
                           type & oyOPTIONATTRIBUTE_ADVANCED) ||
                          !(type & oyOPTIONATTRIBUTE_ADVANCED))
                  WARNc1_S("no in filter defaults \"%s\" found.", key);

              } else if(oy_debug > 2)
              {
                tmp = oyOptions_FindString(f_options, key, 0);
                oicc_msg( oyMSG_DBG,(oyStruct_s*)f_options,
                         "%s:%d \"%s\" is already set = %s",
                         strrchr(__FILE__,'/') ?
                                 strrchr(__FILE__,'/') + 1 : __FILE__ ,__LINE__,
                         key, tmp?tmp:"????");
              }
  oyOption_Release( &o );
}

void     oiccFilterGraph_CountNodes  ( oyFilterGraph_s   * g,
                                       int32_t           * display_mode,
                                       int               * icc_nodes_n,
                                       int                 verbose )
{
  int i,n;
  oyFilterNode_s * node = 0;

  n = oyFilterGraph_CountNodes( g, "", NULL );
  for(i = 0; i < n; ++i)
  {
    node = oyFilterGraph_GetNode( g, i, "", NULL );
    if(oyFilterRegistrationMatch( oyFilterNode_GetRegistration(node),
                                  "//" OY_TYPE_STD "/icc_color", 0 ))
    {
      if(verbose)
        oicc_msg( oyMSG_DBG,(oyStruct_s*)node, OY_DBG_FORMAT_
                  "node: %s[%d]", OY_DBG_ARGS_,
                  oyFilterNode_GetRegistration( node ), oyFilterNode_GetId( node ));
      ++(*icc_nodes_n);
    }
    if(oyFilterRegistrationMatch( oyFilterNode_GetRegistration( node ),
                                  "//" OY_TYPE_STD "/display", 0 ))
    {
      if(verbose)
        oicc_msg( oyMSG_DBG,(oyStruct_s*)node, OY_DBG_FORMAT_
                  "node: %s[%d] - display mode", OY_DBG_ARGS_,
                  oyFilterNode_GetRegistration( node ), oyFilterNode_GetId( node ));
      ++(*display_mode);
    }
    oyFilterNode_Release( &node );
  }

}

void     oiccFilterNode_OptionsPrint ( oyFilterNode_s    * node,
                                       oyOptions_s       * f_options,
                                       oyOptions_s       * db_options )
{
  int k,os_n;
  oyOption_s * o = 0;
  char * tmp = 0;

              os_n = oyOptions_Count(f_options);
              {
                oicc_msg( oyMSG_DBG,(oyStruct_s*)node,OY_DBG_FORMAT_
                           "filter node options %s[%d]",OY_DBG_ARGS_,
                           oyStruct_GetInfo( (oyStruct_s*)f_options, oyNAME_NAME, 0 ),
                           oyObject_GetId( f_options->oy_ ));
                for(k = 0; k < os_n; k++)
                {
                  o = oyOptions_Get( f_options, k );
                  tmp  = oyOption_GetValueText(o, oyAllocateFunc_);
                  fprintf( stderr, "%d: \"%s\": \"%s\" %s %d\n", k, 
                       oyOption_GetText( o, oyNAME_DESCRIPTION ),
                       tmp,
           oyFilterRegistrationToText( oyOption_GetText( o, oyNAME_DESCRIPTION),
                                       oyFILTER_REG_OPTION, 0 ),
                  oyOption_GetFlags(o) );

                  oyFree_m_( tmp );
                  oyOption_Release( &o );
                }
              }
              os_n = oyOptions_Count(db_options);
              {
                oicc_msg( oyMSG_DBG,(oyStruct_s*)node,OY_DBG_FORMAT_
                           "DB options for filter %s[%d]",OY_DBG_ARGS_,
                           oyStruct_GetInfo( (oyStruct_s*)db_options, oyNAME_NAME, 0 ),
                           oyObject_GetId( db_options->oy_ ));
                for(k = 0; k < os_n; k++)
                {
                  o = oyOptions_Get( db_options, k );
                  tmp  = oyOption_GetValueText(o, oyAllocateFunc_);
                  fprintf( stderr, "%d: \"%s\": \"%s\" %s %d\n", k, 
                       oyOption_GetText( o, oyNAME_DESCRIPTION ),
                       tmp,
           oyFilterRegistrationToText( oyOption_GetText( o, oyNAME_DESCRIPTION),
                                       oyFILTER_REG_OPTION, 0 ),
                  oyOption_GetFlags(o) );

                  oyFree_m_( tmp );
                  oyOption_Release( &o );
                }
              }
}

int           oiccConversion_Correct ( oyConversion_s    * conversion,
                                       uint32_t            flags,
                                       oyOptions_s       * options )
{
  int error = 0, j,m,
      icc_nodes_n = 0;
  int verbose = oyOptions_FindString( options, "verbose", 0 ) ? 1:0;
  oyFilterGraph_s * g = 0;
  oyFilterNode_s * node = 0;
  oyFilterPlug_s * edge = 0;
  oyConversion_s * s = conversion;
  oyProfiles_s * proofs =  0,
               * effps = 0;
  oyProfile_s * proof =  0,
              * effp = 0;
  oyOptions_s * db_options = 0,
              * f_options = 0;
  oyOption_s * o = 0;
  const char * val = 0;
  int32_t proofing = 0,
          effect_switch = 0,
          display_mode = 0,
          rendering_gamut_warning = 0;
  int display_white_point = 0;

  if(oy_debug == 1)
    verbose = 1;

  if( oyOptions_FindString( options, "display_mode", "1" ) )
    ++display_mode;

  if(verbose || oy_debug)
    oicc_msg( oyMSG_DBG,(oyStruct_s*)node, OY_DBG_FORMAT_
              "display_mode option %sfound %s", OY_DBG_ARGS_,
              display_mode?"":"not ",
              oyOptions_FindString( options, "display_mode", 0)?
              oyOptions_FindString( options, "display_mode", 0):"");


  node = oyConversion_GetNode( s, OY_INPUT );
  if(!node)
    node = oyConversion_GetNode( s, OY_OUTPUT );

  g = oyFilterGraph_FromNode( node, 0 );
  oyFilterNode_Release( &node );

  oiccFilterGraph_CountNodes( g, &display_mode, &icc_nodes_n, verbose );

  /* How far is this ICC node from the output node? */
  if(verbose && oy_debug == 5)
  {
    node = oyConversion_GetNode( s, OY_INPUT );
    oyShowGraph_( node, 0 );
    oyFilterNode_Release( &node );
  }

  m = oyFilterGraph_CountEdges( g );
  /* start from out_ and search all ICC CMMs */
  for(j = 0; j < m; ++j)
  {

        edge = oyFilterGraph_GetEdge( g, j );
        node = oyFilterPlug_GetNode( edge );
        {
          oyConnector_s * edge_pattern = oyFilterPlug_GetPattern( edge );
          oyFilterSocket_s * edge_remote = oyFilterPlug_GetSocket( edge );
          oyConnector_s * edge_remote_pattern = oyFilterSocket_GetPattern( edge_remote );
          /* select only application level "data" coonectors; follow the data */
          if(oyFilterRegistrationMatch( oyConnector_GetReg( edge_pattern ),
                                        "//" OY_TYPE_STD "/data", 0) &&
             oyFilterRegistrationMatch( oyConnector_GetReg( edge_remote_pattern ),
                                        "//" OY_TYPE_STD "/data", 0))
          {
            /* stop at the first hit if "icc" */
            if( oyFilterRegistrationMatch( oyFilterNode_GetRegistration( node ),
                                           "//" OY_TYPE_STD "/icc_color", 0))
            {
              const char * reg = oyFilterNode_GetRegistration( node );
              uint32_t icc_profile_flags = oyICCProfileSelectionFlagsFromRegistration( reg );

              /* apply the found policy settings */
              db_options = oyOptions_ForFilter( reg, flags, 0 );
              f_options = oyFilterNode_GetOptions( node, flags );

              if(oy_debug > 2 || verbose)
                oiccFilterNode_OptionsPrint( node, f_options, db_options );

              oiccChangeNodeOption( f_options, db_options,
                                    "proof_soft", s, verbose, flags, oyOPTIONATTRIBUTE_ADVANCED );
              oiccChangeNodeOption( f_options, db_options,
                                    "proof_hard", s, verbose, flags, oyOPTIONATTRIBUTE_ADVANCED);
              oiccChangeNodeOption( f_options, db_options,
                                    "rendering_intent", s, verbose, flags, 0);
              oiccChangeNodeOption( f_options, db_options,
                                    "rendering_bpc", s, verbose, flags, 0);
              oiccChangeNodeOption( f_options, db_options,
                                    "rendering_intent_proof", s, verbose, flags, oyOPTIONATTRIBUTE_ADVANCED);
              oiccChangeNodeOption( f_options, db_options,
                                    "rendering_gamut_warning", s, verbose, flags, oyOPTIONATTRIBUTE_ADVANCED);
              oiccChangeNodeOption( f_options, db_options,
                                    "effect_switch", s, verbose, flags, 0 );
              oiccChangeNodeOption( f_options, db_options,
                                    "display_white_point", s, verbose, flags, 0 );
              if(display_mode)
                proofing = oyOptions_FindString( f_options, "proof_soft", "1" )
                           ? 1 : 0;
              else
                proofing = oyOptions_FindString( f_options, "proof_hard", "1" )
                           ? 1 : 0;
              rendering_gamut_warning = oyOptions_FindString( f_options,
                                        "rendering_gamut_warning", "1" ) ? 1:0;
              effect_switch = oyOptions_FindString( f_options,
                                        "effect_switch", "1" ) ? 1:0;

              o = oyOptions_Find( f_options, "profiles_effect", oyNAME_PATTERN );
              if(!o && (effect_switch))
              {
                effp = oyProfile_FromStd( oyPROFILE_EFFECT, icc_profile_flags, 0 );
                effps = oyProfiles_New(0);
                val = oyProfile_GetText( effp, oyNAME_NAME );
                oyProfiles_MoveIn( effps, &effp, -1 );
                oyOptions_MoveInStruct( &f_options,
                                        OY_PROFILES_EFFECT,
                                        (oyStruct_s**)& effps,
                                        OY_CREATE_NEW );
                if(verbose)
                  oicc_msg( oyMSG_DBG,(oyStruct_s*)node,
                           "%s:%d set \"profiles_effect\": %s %s in %s[%d]",
                           strrchr(__FILE__,'/') ?
                                 strrchr(__FILE__,'/') + 1 : __FILE__ ,__LINE__,
                           val?val:"empty profile text", 
                           display_mode ? "for displaying" : "for hard copy",
                           oyStruct_GetInfo( (oyStruct_s*)f_options, oyNAME_NAME, 0 ),
                           oyObject_GetId( f_options->oy_ ));
              } else if(verbose)
                oicc_msg( oyMSG_DBG,(oyStruct_s*)node,
                         "%s:%d \"profiles_effect\" %s, %s",
                         strrchr(__FILE__,'/') ?
                                 strrchr(__FILE__,'/') + 1 : __FILE__ ,__LINE__,
                         o ? "is already set" : "no profile",
                         effect_switch ? "effect_switch is set" :"effect_switch is not set" );

              /* TODO @todo add proofing profile */
              o = oyOptions_Find( f_options, "profiles_simulation", oyNAME_PATTERN );
              if(!o && (proofing || rendering_gamut_warning))
              {
                proof = oyProfile_FromStd( oyPROFILE_PROOF, icc_profile_flags, 0 );
                proofs = oyProfiles_New(0);
                val = oyProfile_GetText( proof, oyNAME_NAME );
                oyProfiles_MoveIn( proofs, &proof, -1 );
                oyOptions_MoveInStruct( &f_options,
                                        OY_PROFILES_SIMULATION,
                                        (oyStruct_s**)& proofs,
                                        OY_CREATE_NEW );
                if(verbose)
                  oicc_msg( oyMSG_DBG,(oyStruct_s*)node,
                           "%s:%d set \"profiles_simulation\": %s %s in %s[%d]",
                           strrchr(__FILE__,'/') ?
                                 strrchr(__FILE__,'/') + 1 : __FILE__ ,__LINE__,
                           val?val:"empty profile text", 
                           display_mode ? "for displaying" : "for hard copy",
                           oyStruct_GetInfo( (oyStruct_s*)f_options, oyNAME_JSON, 0 ),
                           oyObject_GetId( f_options->oy_ ));
              } else if(verbose)
                oicc_msg( oyMSG_DBG,(oyStruct_s*)node,
                         "%s:%d \"profiles_simulation\" %s, %s",
                         strrchr(__FILE__,'/') ?
                                 strrchr(__FILE__,'/') + 1 : __FILE__ ,__LINE__,
                         o ? "is already set" : "no profile",
                         proofing ? "proofing is set" :"proofing is not set" );

              if(display_mode)
                oyAddDisplayEffects( &f_options );

              if(display_mode)
              {
                oyImage_s * image = (oyImage_s*)oyFilterNode_GetData( node, 0 );
                oyProfile_s* image_profile = oyImage_GetProfile( image );

                error = oyAddMonitorEffects( image_profile, &f_options );
                if(error || oy_debug)
                  oicc_msg( oyMSG_WARN, (oyStruct_s*)conversion, 
                    OY_DBG_FORMAT_"display_white_point: %d %s", OY_DBG_ARGS_, display_white_point, oyProfile_GetText( image_profile, oyNAME_DESCRIPTION ));

                oyProfile_Release( &image_profile );
                oyImage_Release( &image );
              }

              if(oy_debug)
                oicc_msg( oyMSG_DBG, (oyStruct_s*)conversion, 
                  OY_DBG_FORMAT_"display_white_point: %d", OY_DBG_ARGS_, display_white_point);

              oyOption_Release( &o );
              oyOptions_Release( &db_options );
              oyOptions_Release( &f_options );

              j = m;
            }
          }
          oyFilterSocket_Release( &edge_remote );
          oyConnector_Release( &edge_pattern );
          oyConnector_Release( &edge_remote_pattern );
        }
        oyFilterPlug_Release( &edge );
        oyFilterNode_Release( &node );
  }

  oyFilterGraph_Release( &g );

  return error;
}

char * oiccCMMGetFallback            ( oyFilterNode_s    * node OY_UNUSED,
                                       uint32_t            flags,
                                       int                 select_core,
                                       oyAlloc_f           allocate_func )
{
  if(select_core)
    return oyGetCMMPattern( oyCMM_CONTEXT_FALLBACK, flags, allocate_func );
  else
    return oyGetCMMPattern( oyCMM_RENDERER_FALLBACK, flags, allocate_func );
}

char * oiccCMMRegistrationToName     ( const char        * registration,
                                       int                 name_type,
                                       uint32_t            flags,
                                       int                 select_core,
                                       oyAlloc_f           allocate_func )
{
  if(select_core)
    return oyCMMRegistrationToName( registration, oyCMM_CONTEXT, name_type, flags, allocate_func );
  else
    return oyCMMRegistrationToName( registration, oyCMM_RENDERER, name_type, flags, allocate_func );
}

char * oiccCMMGetDefaultPattern      ( const char        * base_pattern OY_UNUSED,
                                       uint32_t            flags OY_UNUSED,
                                       int                 select_core OY_UNUSED,
                                       oyAlloc_f           allocate_func )
{
  return oyStringCopy( "//" OY_TYPE_STD "/icc_color.lcm2", allocate_func );
}

/** @brief    oicc policy oyCMMapi9_s implementation
 *
 *  A module to match ICC CMM default settings to Oyranos policy settings.
 *  Per object user settings have priority and are not touched.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/13 (Oyranos: 0.1.9)
 *  @date    2009/07/23
 */
oyCMMapi9_s_  oicc_api9 = {

  oyOBJECT_CMM_API9_S, /* oyStruct_s::type */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  0, /* oyCMMapi_s * next */
  
  oiccFilterInit, /* oyCMMInit_f */
  oiccFilterReset, /* oyCMMReset_f */
  oiccFilterMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* registration */
  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "icc_color." CMM_NICK,

  {0,9,6}, /* int32_t version[3] */
  CMM_API_VERSION, /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* oyCMMapi5_s    * api5_; keep empty */
  0,   /* runtime_context */

  oicc_defaultICCValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  oiccWidgetEvent, /* oyWidgetEvent_f */

  oicc_default_color_icc_options,   /* options */
  oiccGetDefaultColorIccOptionsUI,  /* oyCMMuiGet_f oyCMMuiGet */
  (char*)CMM_NICK "=\"http://www.oyranos.org/2009/oyranos_icc\"", /* xml_namespace */

  (oyCMMobjectType_s**)icc_objects,  /* object_types */

  0,  /* getText */
  0,  /* texts */

  /** oyConversion_Correct_f oyConversion_Correct; check a graph */
  oiccConversion_Correct,

  /** const char * pattern; a pattern supported by oiccConversion_Correct */
  "//" OY_TYPE_STD "/icc_color",

  /** const char * key_base; a key base used in oicc_default_color_icc_options */
  OY_CMM_STD,

  /** oyCMMGetFallback_f oyCMMGetFallback; get pattern specific module fallback */
  oiccCMMGetFallback,

  /** oyCMMRegistrationToName_f oyCMMRegistrationToName; get pattern from registration */
  oiccCMMRegistrationToName,

  /** oyCMMGetDefaultPattern_f oyCMMGetDefaultPattern; get the default pattern for a module group */
  oiccCMMGetDefaultPattern
};


/**
 *  This function implements oyCMMGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2008/12/30
 */
const char * oiccInfoGetText         ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return CMM_NICK;
    else if(type == oyNAME_NAME)
      return _("Oyranos ICC policy");
    else
      return _("Oyranos ICC policy module");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return "Kai-Uwe";
    else if(type == oyNAME_NAME)
      return "Kai-Uwe Behrmann";
    else
      return _("Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/wiki/index.php?title=Oyranos/Download");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return "newBSD";
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 2005-2016 Kai-Uwe Behrmann; newBSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/BSD-3-Clause");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("The filter provides policy settings. These settings can be applied to a graph through the user function oyConversion_Correct().");
    else
      return _("The module is responsible for many settings in the Oyranos color management settings panel. If applied the module care about rendering intents, simulation, mixed color documents and default profiles.");
  }
  return 0;
}
const char *oicc_texts[5] = {"name","copyright","manufacturer","help",0};

oyIcon_s oicc_icon = {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"};

/** @brief    oicc module infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/23 (Oyranos: 0.1.10)
 *  @date    2009/07/23
 */
oyCMM_s oicc_cmm_module = {

  oyOBJECT_CMM_INFO_S,
  0,0,0,
  CMM_NICK,
  "0.9.6",
  oiccInfoGetText,                     /**< oyCMMGetText_f getText */
  (char**)oicc_texts,                  /**<texts; list of arguments to getText*/
  OYRANOS_VERSION,

  (oyCMMapi_s*) & oicc_api9,

  &oicc_icon,

  oiccInit,
  NULL                                 /**< reset() */
};

/**  @} *//* oicc_policy */
