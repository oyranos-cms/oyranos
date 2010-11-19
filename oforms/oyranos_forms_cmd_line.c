/** @file oyranos_forms_cmd_line.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    forms handling for command line applications
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/10/04
 */



#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_forms.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_alpha_internal.h"
#include "oyranos_string.h"

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
                                       oyOptions_s       * collected_elements,
                                       oyPointer           user_data )
{
  oyOption_s * o  = 0, * o2, *o3;
  int n = oyOptions_Count( collected_elements ),
      i,j,j_n,k,k_n,
      is_default, default_pos = -1,
      choices_n = 0;
  oyOptions_s * opts = 0, * opts2;
  const char * default_value = 0,
             * tmp,
             * label,
             * value;
  char * default_key = 0, *key = 0, * t = 0;
  char * choices = 0;
  oyFormsArgs_s * cmd_line_args = user_data;
  int print = cmd_line_args ? !cmd_line_args->silent : 1;

  default_value = oyOptions_FindString( collected_elements, "xf:select1", 0 );
  o = oyOptions_Find( collected_elements, "xf:select1" );
  key = oyStringCopy_( o->registration, oyAllocateFunc_ );
  t = oyStrrchr_( key, '/' );
  t = oyStrchr_( t, '.' ); 
  t[0] = 0;   

  if(oy_debug && default_value && print)
    printf( "found default: \"%s\"\n", default_value );

  for(i = 0; i < n; ++i)
  {
    o = oyOptions_Get( collected_elements, i );
    opts = (oyOptions_s*) oyOption_StructGet( o, oyOBJECT_OPTIONS_S );
    
    if(!opts && oyFilterRegistrationMatch( o->registration,"xf:label", 0 ) &&
       print)
      printf( " %s:\n", o->value->string );

    if(opts && oyFilterRegistrationMatch( o->registration,"xf:choices", 0 ))
    {
      j_n = oyOptions_Count( opts);
      for(j = 0; j < j_n; ++j)
      {
        o2 = oyOptions_Get( opts, j );
        opts2 = (oyOptions_s*) oyOption_StructGet( o2, oyOBJECT_OPTIONS_S );

        if(!opts2 && oyFilterRegistrationMatch(o2->registration,"xf:label", 0 )
           && print)
          printf( "  %s:\n", o2->value->string );

        if(opts2 && oyFilterRegistrationMatch( o2->registration,"xf:item", 0 ))
        {
          label = tmp = value = 0;
          is_default = 0;

          k_n = oyOptions_Count( opts2);
          for(k = 0; k < k_n; ++k)
          {
            o3 = oyOptions_Get( opts2, k );
            if(oy_debug && print)
              printf( "    found option: 0x%lx  \"%s\" %s\n",
                (long)(intptr_t)o3, oyOption_GetText(o3, oyNAME_NICK),
                oyStruct_TypeToText((oyStruct_s*)o3) );

            oyOption_Release( &o3 );
          }

          /* collect the understood elements */
          tmp = oyOptions_FindString( opts2, "xf:label", 0 );
          if(tmp)
            label = tmp;
          tmp = oyOptions_FindString( opts2, "xf:value", 0 );
          if(tmp)
            value = tmp;

          if(!value && !label)
            continue;

          if(value && default_value &&
             oyStrcmp_(default_value,value) == 0)
          {
            is_default = 1;
            default_pos = choices_n;
          }

          if(is_default)
            STRING_ADD( choices, "[" );
          STRING_ADD( choices, value );
          if(is_default)
            STRING_ADD( choices, "]" );
          if(label)
          {
            STRING_ADD( choices, " - \"" );
            STRING_ADD( choices, label );
          }
          STRING_ADD( choices, "\"" );
          STRING_ADD( choices, "\n" );

          ++choices_n;
        }
        else if(oy_debug && print)
          printf( "  found option: 0x%lx  \"%s\" %s\n",
                (long)(intptr_t)o2, oyOption_GetText(o2, oyNAME_NICK),
                oyStruct_TypeToText((oyStruct_s*)o2) );

        oyOptions_Release( &opts2 );
        oyOption_Release( &o2 );
      }
    }
    else if(oy_debug && print)
      printf( "found option: 0x%lx  \"%s\" %s\n",
              (long)(intptr_t)o, oyOption_GetText(o, oyNAME_NICK),
              oyStruct_TypeToText((oyStruct_s*)o) );

    oyOptions_Release( &opts );
    oyOption_Release( &o );
  }

  o = oyOptions_Find( collected_elements, "xf:select1" );
  if(o)
  {
    STRING_ADD( default_key, o->registration );
    t = oyStrstr_( default_key, ".xf:select1" );
    t[0] = 0;

    if(print)
    {
      printf("  ");
      /* the option follows */
      printf(_("Option"));
      printf(":\n");
      printf("    --%s=[%s]\n    ", default_key, default_value);
    }
    i = 0;
    if(cmd_line_args)
      oyOptions_SetFromText( (oyOptions_s**)&cmd_line_args->xforms_data_model_,
                             key, default_value, OY_CREATE_NEW );
    /* the choices follow */
    if(print)
    {
      printf(_("with following choices"));

      printf(":\n");
      i = -1;
      if(choices_n <= 10)
        printf("%s", choices );
      else
      {
        while(choices[++i])
          if(choices[i] != '\n')
            putc( choices[i], stdout );
          else
          {
            putc( ';', stdout );
            putc( ' ', stdout );
          }
        printf("\n");
      }
      printf("\n");
    }

    oyOption_Release( &o );
  }

  if(choices)
    oyFree_m_( choices );
  oyFree_m_( default_key );
  if(key)
    oyFree_m_( key );

  /*printf("collected:\n%s", oyOptions_GetText( collected_elements, oyNAME_NICK));*/
  return 0;
}

const char * oy_ui_cmd_line_handler_xf_select1_element_searches_[]
 = {
 "xf:select1/xf:choices/xf:item/xf:label.xf:value",
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
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/29 (Oyranos: 0.1.10)
 *  @date    2009/08/31
 */
int        oyXML2XFORMsCmdLineHtmlHeadlineHandler (
                                       xmlNodePtr          cur,
                                       oyOptions_s       * collected_elements,
                                       oyPointer           user_data )
{
  const char * tmp = 0;
  int size = 0;
  oyFormsArgs_s * cmd_line_args = user_data;
  int print = cmd_line_args ? !cmd_line_args->silent : 1;

  if(!tmp)
  {
    tmp = oyOptions_FindString( collected_elements, "h3", 0 );
    if(tmp)
      size = 3;
  }

  if(tmp && print)
    printf( "%s\n", tmp );

  return 0;
}

/** @internal
 *  Function oyXML2XFORMsCmdLineHtmlHeadline4Handler
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
 *  @since   2009/10/04 (Oyranos: 0.1.10)
 *  @date    2009/10/04
 */
int        oyXML2XFORMsCmdLineHtmlHeadline4Handler (
                                       xmlNodePtr          cur,
                                       oyOptions_s       * collected_elements,
                                       oyPointer           user_data )
{
  const char * tmp = 0;
  int size = 0;
  oyFormsArgs_s * cmd_line_args = user_data;
  int print = cmd_line_args ? !cmd_line_args->silent : 1;

  if(!tmp)
  {
    tmp = oyOptions_FindString( collected_elements, "h4", 0 );
    if(tmp)
      size = 3;
  }

  if(tmp && print)
    printf( "%s\n", tmp );

  return 0;
}

const char * oy_ui_cmd_line_handler_html_headline4_element_searches_[] = {
"h4",0};
oyUiHandler_s oy_ui_cmd_line_handler_html_headline4_ =
  {oyOBJECT_UI_HANDLER_S,0,0,0,        /**< oyStruct_s members */
   "oyFORMS",                          /**< dialect */
   "libxml2",                          /**< parser_type */
   (oyUiHandler_f)oyXML2XFORMsCmdLineHtmlHeadline4Handler, /**<oyUiHandler_f handler*/
   "dummy",                            /**< handler_type */
   (char**)oy_ui_cmd_line_handler_html_headline4_element_searches_ /**< element_searches */
  };

const char * oy_ui_cmd_line_handler_html_headline_element_searches_[] = {
"h3",0};
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
  &oy_ui_cmd_line_handler_html_headline4_,
  0
};


