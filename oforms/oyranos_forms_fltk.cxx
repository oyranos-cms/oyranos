/** @file oyranos_forms_fltk.cxx
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    forms handling for the FLTK toolkit
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/10/04
 */



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "oyranos_widgets_fltk.h"
#include <FL/Fl.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Scroll.H>
//#include "../fl_i18n/fl_i18n.H"
#include <FL/Flmm_Tabs.H>

#include "config.h"
#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"


using namespace oyranos;



/** @internal
 *  Function oyXML2XFORMsFLTKSelect1Handler
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
 *  @date    2009/09/10
 */
int        oyXML2XFORMsFLTKSelect1Handler (
                                       xmlNodePtr          cur,
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
  char * default_key = 0, * t = 0;
  char * choices = 0;
  fltk_args_s * fltk_args = (fltk_args_s *)user_data;
  int print = fltk_args ? !fltk_args->silent : 1;

  default_value = oyOptions_FindString( collected_elements, "xf:select1", 0 );

  if(oy_debug && default_value && print)
    printf( "found default: \"%s\"\n", default_value );

  Fl_Group *parent = Fl_Group::current();

  if(!parent)
    return 1;

  int x = parent->x(),
            y = parent->y(),
            w = parent->w()/*,
            h = parent->h()*/;
  Fl_Pack * pack = new Fl_Pack(x,y,w,BUTTON_HEIGHT);
  pack->type( FL_HORIZONTAL );
  pack->spacing(H_SPACING);

    OyFl_Box_c * box = new OyFl_Box_c( 0,0,w-BOX_WIDTH-H_SPACING,BUTTON_HEIGHT);
    //box->labelfont( FL_BOLD );
    box->align( FL_ALIGN_LEFT | FL_ALIGN_INSIDE );

    Fl_Choice * c = new Fl_Choice( 0,0,BOX_WIDTH,BUTTON_HEIGHT );

  for(i = 0; i < n; ++i)
  {
    o = oyOptions_Get( collected_elements, i );
    opts = (oyOptions_s*) oyOption_StructGet( o, oyOBJECT_OPTIONS_S );
    
    if(!opts && oyFilterRegistrationMatch( o->registration,"xf:label",
                                           oyOBJECT_NONE ) &&
       print)
      box->copy_label( o->value->string );

    if(opts && oyFilterRegistrationMatch( o->registration,"xf:choices",
                                          oyOBJECT_NONE ))
    {
      j_n = oyOptions_Count( opts);
      for(j = 0; j < j_n; ++j)
      {
        o2 = oyOptions_Get( opts, j );
        opts2 = (oyOptions_s*) oyOption_StructGet( o2, oyOBJECT_OPTIONS_S );

        if(!opts2 && oyFilterRegistrationMatch(o2->registration,"xf:label",
                                               oyOBJECT_NONE )
           && print)
          box->copy_label( o2->value->string );

        if(opts2 && oyFilterRegistrationMatch( o2->registration,"xf:item",
                                               oyOBJECT_NONE ))
        {
          label = tmp = value = 0;
          is_default = 0;

          if(oy_debug && print)
          {
            k_n = oyOptions_Count( opts2);
            for(k = 0; k < k_n; ++k)
            {
              o3 = oyOptions_Get( opts2, k );
              if(oy_debug && print)
                printf( "    found option: 0x%x  \"%s\" %s\n",
                  (int)o3, oyOption_GetText(o3, oyNAME_NICK),
                  oyStruct_TypeToText((oyStruct_s*)o3) );

              oyOption_Release( &o3 );
            }
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

          /* detect default */
          if(value && default_value &&
             oyStrcmp_(default_value,value) == 0)
          {
            is_default = 1;
            default_pos = choices_n;
          }

          if(!value) value = label;
          if(!label) label = value;

          /* append the choice
           * store the label and value in user_data() for evaluating results */
          if(print)
          {
            char ** texts = (char**)malloc(sizeof(char*)*3);
            int len = strlen(label), pos = 0;
            memset(texts, 0, sizeof(char*)*3 );
            texts[0] = (char*) malloc(strlen(label)*2);
            texts[1] = strdup(value);
            for(k = 0; k <= len; ++k)
            {
              if(label[k] == '/')
                texts[0][pos++] = '\\';
              texts[0][pos++] = label[k];
            }
            c->add( (const char *) texts[0], 0, 0, (void*)texts, 0 );
          }

          ++choices_n;
        }
        else if(oy_debug && print)
          printf( "  found option: 0x%x  \"%s\" %s\n",
                (int)o2, oyOption_GetText(o2, oyNAME_NICK),
                oyStruct_TypeToText((oyStruct_s*)o2) );

        oyOptions_Release( &opts2 );
        oyOption_Release( &o2 );
      }
    }
    else if(oy_debug && print)
      printf( "found option: 0x%x  \"%s\" %s\n",
              (int)o, oyOption_GetText(o, oyNAME_NICK),
              oyStruct_TypeToText((oyStruct_s*)o) );

    oyOptions_Release( &opts );
    oyOption_Release( &o );
  }

  pack->end();
  pack->resizable( box );
  if(default_pos >= 0)
    c->value( default_pos );

  /* collect results */
  o = oyOptions_Find( collected_elements, "xf:select1" );
  if(o)
  {
    STRING_ADD( default_key, o->registration );
    t = oyStrstr_( default_key, ".xf:select1" );
    t[0] = 0;

    if(fltk_args)
      oyStringListAddStaticString_( &fltk_args->options,
                                    &fltk_args->n, default_key,
                                    oyAllocateFunc_, oyDeAllocateFunc_ );

    oyOption_Release( &o );
  }

  if(choices)
    oyFree_m_( choices );
  oyFree_m_( default_key );

  /*printf("collected:\n%s", oyOptions_GetText( collected_elements, oyNAME_NICK));*/
  return 0;
}

oyUiHandler_s oy_ui_fltk_handler_xf_select1_ =
  {oyOBJECT_UI_HANDLER_S,0,0,0,        /**< oyStruct_s members */
   (char*)"oyFORMS",                   /**< dialect */
   (char*)"libxml2",                   /**< parser_type */
   (char*)"xf:select1",                /**< element_type; Wanted XML element. */
   (oyUiHandler_f)oyXML2XFORMsFLTKSelect1Handler, /**<oyUiHandler_f handler*/
   (char*)"dummy",                     /**< handler_type */
   (char*)"xf:choices/xf:item/xf:label.xf:value" /**< element_search */
  };

/** @internal
 *  Function oyXML2XFORMsFLTKHtmlHeadlineHandler
 *  @brief   build a UI for a html:h3 element
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
int        oyXML2XFORMsFLTKHtmlHeadlineHandler (
                                       xmlNodePtr          cur,
                                       oyOptions_s       * collected_elements,
                                       oyPointer           user_data )
{
  const char * tmp = 0;
  int size = 0;
  fltk_args_s * fltk_args = (fltk_args_s *)user_data;
  int print = fltk_args ? !fltk_args->silent : 1;
  OyFl_Box_c * box = 0;

  if(!tmp)
  {
    tmp = oyOptions_FindString( collected_elements, "h3", 0 );
    if(tmp)
      size = 3;
  }

  if(tmp && tmp[0] && print)
  {
    Fl_Group *parent = Fl_Group::current(); // parent tab

    if( !parent )
    {
      WARNc_S( "wrong widget" );
      return 1;
    }

    {
        Fl_Widget *wid = (Fl_Widget*)0; //parent->user_data();
        if( !wid ) wid = parent;

        int x = parent->x(),
            y = parent->y(),
            w = parent->w();
        box = new OyFl_Box_c( x,y,w,BUTTON_HEIGHT );
        box->copy_label( tmp );
        box->labelfont( FL_BOLD );
        box->align( FL_ALIGN_LEFT | FL_ALIGN_INSIDE );
    }
  }

  return 0;
}

/** @internal
 *  Function oyXML2XFORMsFLTKHtmlHeadline4Handler
 *  @brief   build a UI for a html:h4 element
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
int        oyXML2XFORMsFLTKHtmlHeadline4Handler (
                                       xmlNodePtr          cur,
                                       oyOptions_s       * collected_elements,
                                       oyPointer           user_data )
{
  const char * tmp = 0;
  int size = 0;
  fltk_args_s * fltk_args = (fltk_args_s *)user_data;
  int print = fltk_args ? !fltk_args->silent : 1;
  OyFl_Box_c * box = 0;

  if(!tmp)
  {
    tmp = oyOptions_FindString( collected_elements, "h4", 0 );
    if(tmp)
      size = 3;
  }

  if(tmp && print)
  {
    Fl_Group *parent = Fl_Group::current(); // parent tab

    if( !parent )
    {
      WARNc_S( "wrong widget" );
      return 1;
    }

    {
        int x = parent->x(),
            y = parent->y(),
            w = parent->w();
        box = new OyFl_Box_c( x,y,w,BUTTON_HEIGHT );
        box->copy_label( tmp );
        box->labelfont( FL_ITALIC );
        box->align( FL_ALIGN_LEFT | FL_ALIGN_INSIDE );
    }
  }

  return 0;
}

oyUiHandler_s oy_ui_fltk_handler_html_headline4_ =
  {oyOBJECT_UI_HANDLER_S,0,0,0,        /**< oyStruct_s members */
   (char*)"oyFORMS",                   /**< dialect */
   (char*)"libxml2",                   /**< parser_type */
   (char*)"h4",                        /**< element_type; Wanted XML elements.*/
   (oyUiHandler_f)oyXML2XFORMsFLTKHtmlHeadline4Handler, /**<oyUiHandler_f handler*/
   (char*)"dummy",                     /**< handler_type */
   (char*)"h4"                         /**< element_search */
  };

oyUiHandler_s oy_ui_fltk_handler_html_headline_ =
  {oyOBJECT_UI_HANDLER_S,0,0,0,        /**< oyStruct_s members */
   (char*)"oyFORMS",                   /**< dialect */
   (char*)"libxml2",                   /**< parser_type */
   (char*)"h3",                        /**< element_type; Wanted XML elements.*/
   (oyUiHandler_f)oyXML2XFORMsFLTKHtmlHeadlineHandler, /**<oyUiHandler_f handler*/
   (char*)"dummy",                     /**< handler_type */
   (char*)"h3"                         /**< element_search */
  };

oyUiHandler_s * oy_ui_fltk_handlers[4] = {
  &oy_ui_fltk_handler_xf_select1_,
  &oy_ui_fltk_handler_html_headline_,
  &oy_ui_fltk_handler_html_headline4_,
  0
};


