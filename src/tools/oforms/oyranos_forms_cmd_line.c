/** @file oyranos_forms_cmd_line.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2009-2018 (C) Kai-Uwe Behrmann
 *
 *  @brief    forms handling for command line applications
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2009/10/04
 */



#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_db.h"
#include "oyranos_debug.h"
#include "oyranos_forms.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_json.h"
#include "oyranos_string.h"
#include "oyranos_xml.h"


/** @internal
 *  Function oyJSON2XFORMsCmdLineHtmlHeadlineHandler
 *  @brief   build a UI from a 'groups' Json array
 *
 *  This function is a simple demonstration.
 *
 *  @param[in]     cur                 json node
 *  @param[in]     collected_elements  parsed and requested elements
 *  @param[in]     user_data           toolkit context
 *  @return                            error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/01/02
 *  @since   2009/08/29 (Oyranos: 0.1.10)
 */
int        oyJSON2XFORMsCmdLineHtmlHeadlineHandler (
                                       oyjl_val            value,
                                       oyOptions_s       * collected_elements OY_UNUSED,
                                       oyPointer           user_data )
{
  const char * label = 0,
             * help = 0;
  oyFormsArgs_s * forms_args = (oyFormsArgs_s *)user_data;
  int print = forms_args ? forms_args->print : 1;

  if(!print || !value)
    return 0;

  oyjl_val v = oyjlTreeGetValue( value, 0, "name" );
  if (OYJL_IS_STRING(v))
    label = v->u.string;
  v = oyjlTreeGetValue( value, 0, "help" );
  if(!v)
    v = oyjlTreeGetValue( value, 0, "description" );
  if (OYJL_IS_STRING(v))
    help = v->u.string;

  if(label && label[0])
    printf( "%s\n", label );
  if(help && help[0])
    printf( " [%s]\n", help );

  return 0;
}


const char * oy_ui_cmd_line_handler_json_headline_element_searches_[] = {
"/////groups/", 0};
oyUiHandler_s oy_ui_cmd_line_handler_json_headline_ =
  {oyOBJECT_UI_HANDLER_S,0,0,0,        /**< oyStruct_s members */
   "oyJSON",                           /**< dialect */
   "oyjl",                             /**< parser_type */
   (oyUiHandler_f)oyJSON2XFORMsCmdLineHtmlHeadlineHandler, /**<oyUiHandler_f handler*/
   "oyjl node",                        /**< handler_type */
   (char**)oy_ui_cmd_line_handler_json_headline_element_searches_ /**< element_searches */
  };


#ifdef HAVE_LIBXML2
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

/* ------------- cmd line XFORMS UI handlers ------------------*/

/** @internal
 *  Function oyXML2XFORMsCmdLineSelect1Handler
 *  @brief   build a UI for a xf:select1 XFORMS sequence
 *
 *  This function is a simple demonstration.
 *
 *  @param[in]     cur                 libxml2 node
 *  @param[in]     collected_elements  parsed and requested elements
 *  @param[in]     user_data           toolkit context
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/29 (Oyranos: 0.1.10)
 *  @date    2009/09/04
 */
int        oyXML2XFORMsCmdLineSelect1Handler( xmlNodePtr          cur,
                                       oyOptions_s       * collected_elements OY_UNUSED,
                                       oyPointer           user_data )
{
  int is_default,
      choices_n = 0;
  const char * default_value = 0,
             * tmp,
             * label,
             * value,
             * help,
             * xpath = 0;
  char * default_key = 0, *key = 0;
  oyFormsArgs_s * forms_args = user_data;
  int print = forms_args ? forms_args->print : 1;

  xmlNodePtr select1, choices = 0, items;

  if(cur)
  {
    if(oyXMLNodeNameIs(cur, "xf:select1"))
    {
      select1 = cur->children;
      default_value = oyXFORMsModelGetXPathValue( cur, "ref", &xpath );

      if(oy_debug && default_value && print)
        printf( "found default: \"%s\"\n", default_value );
    }
    else
      select1 = 0;

    while(select1)
    {
      if(oyXMLNodeNameIs( select1, "xf:label") && print)
          printf( "    %s:\n", oyXML2NodeValue(select1) );
      else
      if(oyXMLNodeNameIs( select1, "xf:help") && print & 0x02)
      {
          printf( "      [%s]\n", oyXML2NodeValue(select1) );
      }
      else
      {
        if(oyXMLNodeNameIs(select1, "xf:choices"))
          choices = select1->children;
        else
          choices = 0;
      }

      while(choices)
      {
        label = tmp = value = help = 0;
        is_default = 0;

        if(oyXMLNodeNameIs( choices, "xf:item"))
          items = choices->children;
        else
          items = 0;
        while(items)
        {
          if(oyXMLNodeNameIs( items, "xf:label") && print)
            label = oyXML2NodeValue( items );
          if(oyXMLNodeNameIs( items, "xf:value") && print)
            value = oyXML2NodeValue( items );
          if(oyXMLNodeNameIs( items, "xf:help") && print)
            help = oyXML2NodeValue( items );

          items = items->next;
        }
        if(value || label)
        {
            /* detect default */
            if(value && default_value &&
               oyStrcmp_(default_value,value) == 0)
            {
              is_default = 1;
            }

            if(!value) value = label;
            if(!label) label = value;

            /* append the choice
             * store the label and value in user_data() for evaluating results*/
            if(print & 0x04)
            {
              tmp = 0;
              if(print & 0x02)
                tmp = label;
              help = tmp?help? (strstr(help,tmp) ? &help[strlen(tmp)] : help):"":"";
              printf( "      --%s=\"%s\"%s%s%s%s%s%s\n",
                      xpath+1, oyNoEmptyString_m_(value), is_default ? " *":"",
                      tmp ? " [" : "", tmp?tmp:"", (help && strlen(help))?": ":"", help, tmp?"]":"" );
            }

            if( !(print & 0x02) && !(print & 0x04) &&
                is_default )
              printf( "      --%s=\"%s\"\n",
                      xpath+1, oyNoEmptyString_m_(value) );

            ++choices_n;
        }
        choices = choices->next;
      }
      select1 = select1->next;
    }
  }

  /* collect results */
  if(xpath && forms_args)
    oyOptions_SetFromString( (oyOptions_s**)&forms_args->xforms_data_model_,
                           xpath+1, default_value, OY_CREATE_NEW );
  if(default_key)
    oyFree_m_( default_key );
  if(key)
    oyFree_m_( key );

  /*printf("collected:\n%s", oyOptions_GetText( collected_elements, oyNAME_NICK));*/
  return 0;
}

const char * oy_ui_cmd_line_handler_xf_select1_element_searches_[]
 = {
 "xf:select1/xf:choices/xf:item/xf:label.xf:value.xf:help",
 "xf:select1/xf:label",
 "xf:select1/xf:help",
 0
};
oyUiHandler_s oy_ui_cmd_line_handler_xf_select1_ =
  {oyOBJECT_UI_HANDLER_S,0,0,0,        /**< oyStruct_s members */
   (char*)"oyFORMS",                          /**< dialect */
   (char*)"libxml2",                          /**< parser_type */
   (oyUiHandler_f)oyXML2XFORMsCmdLineSelect1Handler, /**<oyUiHandler_f handler*/
   (char*)"dummy",                            /**< handler_type */
   (char**)oy_ui_cmd_line_handler_xf_select1_element_searches_ /**< element_searches */
  };

/** @internal
 *  Function oyXML2XFORMsCmdLineHtmlHeadlineHandler
 *  @brief   build a UI for a xf:select1 XFORMS sequence
 *
 *  This function is a simple demonstration.
 *
 *  @param[in]     cur                 libxml2 node
 *  @param[in]     collected_elements  parsed and requested elements
 *  @param[in]     user_data           toolkit context
 *  @return                            error
 *
 *  @version Oyranos: 0.3.2
 *  @since   2009/08/29 (Oyranos: 0.1.10)
 *  @date    2011/07/31
 */
int        oyXML2XFORMsCmdLineHtmlHeadlineHandler (
                                       xmlNodePtr          cur,
                                       oyOptions_s       * collected_elements OY_UNUSED,
                                       oyPointer           user_data )
{
  const char * label = 0,
             * help = 0,
             * type = 0;
  oyFormsArgs_s * forms_args = (oyFormsArgs_s *)user_data;
  int print = forms_args ? forms_args->print : 1;

  xmlNodePtr group;


  if(cur && print)
  {
    group = cur->children;

    if(cur->next)
      type = oyXFORMsModelGetAttrValue( cur, "type" );

    if(type)
    while(group)
    {
      if(oyXMLNodeNameIs( group, "xf:label") && print && !label)
        label = oyXML2NodeValue(group);
      else
      if(oyXMLNodeNameIs( group, "xf:help") && print && !help)
        help = oyXML2NodeValue(group);
      group = group->next;
    }
  }

  if(label && label[0] && print)
    printf( "%s\n", label );
  if(help && help[0] && print)
    printf( " [%s]\n", help );

  return 0;
}


const char * oy_ui_cmd_line_handler_html_headline_element_searches_[] = {
"xf:group",0};
oyUiHandler_s oy_ui_cmd_line_handler_html_headline_ =
  {oyOBJECT_UI_HANDLER_S,0,0,0,        /**< oyStruct_s members */
   "oyFORMS",                          /**< dialect */
   "libxml2",                          /**< parser_type */
   (oyUiHandler_f)oyXML2XFORMsCmdLineHtmlHeadlineHandler, /**<oyUiHandler_f handler*/
   "dummy",                            /**< handler_type */
   (char**)oy_ui_cmd_line_handler_html_headline_element_searches_ /**< element_searches */
  };

oyUiHandler_s * oy_ui_cmd_line_handlers[4] = {
  &oy_ui_cmd_line_handler_xf_select1_,
  &oy_ui_cmd_line_handler_html_headline_,
  &oy_ui_cmd_line_handler_json_headline_,
  0
};
#else /* HAVE_LIBXML2 */

oyUiHandler_s * oy_ui_cmd_line_handlers[4] = {
  &oy_ui_cmd_line_handler_json_headline_,
  0
};

#endif /* HAVE_LIBXML2 */

