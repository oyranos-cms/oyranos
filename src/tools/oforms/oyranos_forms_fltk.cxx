/** @file oyranos_forms_fltk.cxx
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2009-2023 (C) Kai-Uwe Behrmann
 *
 *  @brief    forms handling for the FLTK toolkit
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
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

#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_internal.h"
#include "oyranos_json.h"
#include "oyranos_forms.h"
#include "oyranos_string.h"
#include "oyranos_xml.h"
#include "../fl_i18n/fl_i18n.H"


using namespace oyranos;
using namespace oyranos::forms;

typedef struct {
  char * label;
  char * value;
  char * key;
  oyOptions_s ** callback_data;
} fltk_cb_data;

float oy_forms_fltk_scale = 1.0;

void fltkCallback                    ( Fl_Widget         * widget,
                                       void              * user_data )
{
  fltk_cb_data * cd = (fltk_cb_data*) user_data;
  Fl_Choice * ch = dynamic_cast<Fl_Choice*> (widget);
  oyOptions_s ** opts = cd->callback_data;
  char num[24];

  if(ch)
  {
    sprintf(num, "%d", ch->value() );
    oyOptions_SetFromString( opts, cd->key, cd->value, 0 );
    printf("Fl_Choice %s(%s)=%s/%s\n", cd->key, cd->label, cd->value, num );

  } else
    printf("no Fl_Choice %s(%s)=%s/%s\n", cd->key, cd->label, cd->value, num );
}

void fltkHelpViewCallback            ( Fl_Widget         * widget,
                                       void              * user_data )
{
  fltk_cb_data * cd = (fltk_cb_data*) user_data;
  Fl_Choice * ch = dynamic_cast<Fl_Choice*> (widget);
  oyOptions_s ** opts = cd->callback_data;
  char num[24];

  if(ch)
  {
    sprintf(num, "%d", ch->value() );
    oyOptions_SetFromString( opts, cd->key, cd->value, 0 );
    printf("Fl_Choice %s(%s)=%s/%s\n", cd->key, cd->label, cd->value, num );

  } else
    printf("no Fl_Choice %s(%s)=%s/%s\n", cd->key, cd->label, cd->value, num );
}

class formsFltkChoice : public Fl_Choice
{
  public:
  formsFltkChoice(int X,int Y,int W,int H) : Fl_Choice(X,Y,W,H) 
  { hint_callback= 0; };
  oyCallback_s * hint_callback;
  int handle(int event)
  {
    int result = Fl_Choice::handle(event);
    switch (event)
    {
      case FL_ENTER:
           if(hint_callback)
           {
             oyFormsFltkHelpViewCallback_f userCallback = 0;
             userCallback =(oyFormsFltkHelpViewCallback_f)
                                                        hint_callback->callback;
             userCallback( hint_callback->data, (const char*)user_data() );
           }
           redraw();
           break;

      case FL_LEAVE:
           if(hint_callback)
           {
             oyFormsFltkHelpViewCallback_f userCallback = 0;
             userCallback =(oyFormsFltkHelpViewCallback_f)
                                                        hint_callback->callback;
             userCallback( hint_callback->data, 0 );
           }
           redraw();
           break;
    }
    return result;
  }
};

/** @internal
 *  Function oyXML2XFORMsFLTKSelect1Handler
 *  @brief   build a UI for a xf:select1 XFORMS sequence
 *
 *  This function is a handler for a Oyranos style xforms.
 *
 *  @param[in]     cur                 libxml2 node
 *  @param[in]     collected_elements  unused
 *  @param[in]     user_data           toolkit context
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/29 (Oyranos: 0.1.10)
 *  @date    2009/11/12
 */
int        oyXML2XFORMsFLTKSelect1Handler (
                                       xmlNodePtr          cur,
                                       oyOptions_s       * collected_elements OY_UNUSED,
                                       oyPointer           user_data )
{
  int k,
      is_default, default_pos = -1,
      choices_n = 0;
  const char * default_value = 0,
             * tmp,
             * label,
             * value,
             * xpath = 0;
  char * default_string = 0;
  oyFormsArgs_s * forms_args = (oyFormsArgs_s *)user_data;
  int print = forms_args ? forms_args->print : 1;

  xmlNodePtr select1, choices, items;

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

    new Fl_Box( 0,0,H_SPACING,BUTTON_HEIGHT);
    OyFl_Box_c * box = new OyFl_Box_c( 2*H_SPACING,0,w-BOX_WIDTH-4*H_SPACING,BUTTON_HEIGHT);
    box->align( FL_ALIGN_LEFT | FL_ALIGN_INSIDE );

    formsFltkChoice * c = new formsFltkChoice( w-BOX_WIDTH-H_SPACING,0,BOX_WIDTH,BUTTON_HEIGHT );

  if(cur)
  {
    if(oyXMLNodeNameIs(cur, "xf:select1"))
    {
      select1 = cur->children;
      default_value = oyXFORMsModelGetXPathValue( cur, "ref", &xpath );
    }
    else
      select1 = 0;

    while(select1)
    {
      if(oyXMLNodeNameIs( select1, "xf:label") && print)
      {
        box->copy_label( (const char *)oyXML2NodeValue(select1) );
      }
      else
      if(oyXMLNodeNameIs( select1, "xf:help") && print)
      {
          oyCallback_s * cb = 0;
          oyOptions_FindData( (oyOptions_s*)forms_args->data_,
                                      OYFORMS_FLTK_HELP_VIEW_REG,
                                      (oyPointer*)&cb, 0, 0);
          if(cb)
            c->hint_callback = cb;
          c->user_data( (void*)oyXML2NodeValue(select1) );
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
        label = tmp = value = 0;
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

          items = items->next;
        }
        if(value || label)
        {
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
             * store the label and value in user_data() for evaluating results*/
            if(print)
            {
              fltk_cb_data *cb_data=(fltk_cb_data*)malloc(sizeof(fltk_cb_data));
              int len = strlen(label), pos = 0;
              memset(cb_data, 0, sizeof(fltk_cb_data) );
              cb_data->label = (char*) malloc(strlen(label)*2);
              cb_data->value = strdup(value);
              cb_data->key = xpath ? strdup(xpath+1):0;
              cb_data->callback_data = (oyOptions_s**)
                                                &forms_args->xforms_data_model_;
              for(k = 0; k <= len; ++k)
              {
                if(label[k] == '/')
                  cb_data->label[pos++] = '\\';
                cb_data->label[pos++] = label[k];
              }
              c->add( (const char *) cb_data->label, 0,
                      fltkCallback,
                      (void*)cb_data, 0 );

              if(is_default)
                default_string = oyStringCopy_( label, malloc );
            }

            ++choices_n;
        }
        choices = choices->next;
      }
      select1 = select1->next;
    }

    if(default_string && default_pos >= 0)
    {
      c->value( c->find_item( (char*)default_string ) );
      free( default_string ); default_string = 0;
    } else
      c->value( -1 );
  }

  pack->end();
  pack->resizable( box );

  /* collect results */
  if(xpath && forms_args)
    oyOptions_SetFromString( (oyOptions_s**)&forms_args->xforms_data_model_,
                           xpath+1, default_value, OY_CREATE_NEW );

  return 0;
}

const char * oy_ui_fltk_handler_xf_select1_element_searches_[] = {
 "xf:select1/xf:choices/xf:item/xf:label.xf:value",
 "xf:select1/xf:label",
 "xf:select1/xf:help",
 "xf:select1/xf:hint",
 0
};

oyUiHandler_s oy_ui_fltk_handler_xf_select1_ =
  {oyOBJECT_UI_HANDLER_S,0,0,0,        /**< oyStruct_s members */
   (char*)"oyFORMS",                   /**< dialect */
   (char*)"libxml2",                   /**< parser_type */
   (oyUiHandler_f)oyXML2XFORMsFLTKSelect1Handler, /**<oyUiHandler_f handler*/
   (char*)"dummy",                     /**< handler_type */
   (char**)oy_ui_fltk_handler_xf_select1_element_searches_ /**< element_searches */
  };

/** @internal
 *  Function oyXML2XFORMsFLTKHtmlHeadlineHandler
 *  @brief   build a UI for a html:h3 element
 *
 *  This function is a handler for a Oyranos style xforms.
 *
 *  @param[in]     cur                 libxml2 node
 *  @param[in]     collected_elements  unused
 *  @param[in]     user_data           toolkit context
 *  @return                            error
 *
 *  @version Oyranos: 0.3.2
 *  @since   2009/08/29 (Oyranos: 0.1.10)
 *  @date    2011/07/31
 */
int        oyXML2XFORMsFLTKHtmlHeadlineHandler (
                                       xmlNodePtr          cur,
                                       oyOptions_s       * collected_elements OY_UNUSED,
                                       oyPointer           user_data )
{
  const char * label = 0,
             * help = 0,
             * type = 0;
  oyFormsArgs_s * forms_args = (oyFormsArgs_s *)user_data;
  int print = forms_args ? forms_args->print : 1;
  OyFl_Box_c * box = 0;
  xmlNodePtr group;


  if(cur && print)
  {
    Fl_Group *parent = Fl_Group::current(); // parent tab

    group = cur->children;

    if( !parent )
    {
      WARNc_S( "wrong widget" );
      return 1;
    }

    if(cur->next)
      type = oyXFORMsModelGetAttrValue( cur, "type" );

    if(type)
    while(group)
    {
      if(oyXMLNodeNameIs( group, "xf:label") && print && !label)
      {
        Fl_Widget *wid = (Fl_Widget*)0; //parent->user_data();
        if( !wid ) wid = parent;

        int x = parent->x(),
            y = parent->y(),
            w = parent->w();
        box = new OyFl_Box_c( x,y,w,BUTTON_HEIGHT );
        label = oyXML2NodeValue(group);
        box->copy_label( (const char *)label );
        if(strcmp(type,"h3") ==  0)
          box->labelfont( FL_BOLD );
        else
          box->labelfont( FL_ITALIC );
        box->align( FL_ALIGN_LEFT | FL_ALIGN_INSIDE );
      }
      else
      if(oyXMLNodeNameIs( group, "xf:help") && print && !help)
      {
        oyCallback_s * cb = 0;
        help = oyXML2NodeValue(group);
        oyOptions_FindData( (oyOptions_s*)forms_args->data_,
                                      OYFORMS_FLTK_HELP_VIEW_REG,
                                      (oyPointer*)&cb, 0, 0);
        if(cb && box)
          box->hint_callback = cb;
        if(box)
          box->user_data( (void*)help );
      }
      group = group->next;
    }
  }

  return 0;
}


const char * oy_ui_fltk_handler_html_headline_element_searches_[] = {
 "xf:group",
 0
};
oyUiHandler_s oy_ui_fltk_handler_html_headline_ =
  {oyOBJECT_UI_HANDLER_S,0,0,0,        /**< oyStruct_s members */
   (char*)"oyFORMS",                   /**< dialect */
   (char*)"libxml2",                   /**< parser_type */
   (oyUiHandler_f)oyXML2XFORMsFLTKHtmlHeadlineHandler, /**<oyUiHandler_f handler*/
   (char*)"dummy",                     /**< handler_type */
   (char**)oy_ui_fltk_handler_html_headline_element_searches_ /**< element_searches */
  };

int        oyJSON2XFORMsFltkHtmlHeadlineHandler2 (
                                       oyjl_val            value,
                                       oyOptions_s       * collected_elements OY_UNUSED,
                                       oyPointer           user_data )
{
  const char * name = NULL,
             * desc = NULL,
             * help = NULL,
             * properties = NULL;
  oyFormsArgs_s * forms_args = (oyFormsArgs_s *)user_data;

  oyjl_val v;
  v = oyjlTreeGetValue( value, 0, "name" );
  if (OYJL_IS_STRING(v))              name = v->u.string;
  v = oyjlTreeGetValue( value, 0, "description" );
  if (OYJL_IS_STRING(v))              desc = v->u.string;
  v = oyjlTreeGetValue( value, 0, "help" );
  if (OYJL_IS_STRING(v))              help = v->u.string;
  v = oyjlTreeGetValue( value, 0, "properties" );
  if (OYJL_IS_STRING(v))              properties = v->u.string;

  OyFl_Box_c * box = 0;

  if(name)
  {
    Fl_Group *parent = Fl_Group::current(); // parent tab

    if( !parent )
    {
      WARNc_S( "wrong widget" );
      return 1;
    }

    Fl_Widget *wid = (Fl_Widget*)0; //parent->user_data();
    if( !wid ) wid = parent;

    int x = parent->x(),
        y = parent->y(),
        w = parent->w();
    box = new OyFl_Box_c( x,y,w,BUTTON_HEIGHT );
    char * label = NULL;
    oyjlStringAdd( &label, malloc, free, "%s %s", name, desc?desc:"" );
    if(label)
    {
      box->copy_label( (const char *)label );
      free(label);
    }
    if( properties &&
        ( strstr(properties,"h3") ||
          strstr(properties,"frame")
        )
      )
      box->labelfont( FL_BOLD );
    else
      box->labelfont( FL_ITALIC );
    box->align( FL_ALIGN_LEFT | FL_ALIGN_INSIDE );

    if(help)
    {
      oyCallback_s * cb = 0;
      oyOptions_FindData( (oyOptions_s*)forms_args->data_,
                                      OYFORMS_FLTK_HELP_VIEW_REG,
                                      (oyPointer*)&cb, 0, 0);
      if(cb && box)
        box->hint_callback = cb;
      if(box)
        box->user_data( (void*)strdup(help) );
    }
  }

  return 0;
}

/** @internal
 *  Function oyJSON2XFORMsFltkHtmlHeadlineHandler
 *  @brief   build a UI from a 'groups' Json array
 *
 *  This function is a handler for a OpenICC style JSON ui.
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
int        oyJSON2XFORMsFltkHtmlHeadlineHandler (
                                       oyjl_val            value,
                                       oyOptions_s       * collected_elements,
                                       oyPointer           user_data )
{
  const char * name = NULL,
             * desc = NULL,
             * help = NULL,
             * properties = NULL;
  oyFormsArgs_s * forms_args = (oyFormsArgs_s *)user_data;
  int print = forms_args ? forms_args->print : 1;

  if(!print || !value)
    return 0;

  oyJSON2XFORMsFltkHtmlHeadlineHandler2( value, collected_elements, user_data );

  return 0;
}


const char * oy_ui_fltk_handler_json_headline_element_searches_[] = {
 "org/freedesktop/oyjl/modules/[]/groups/[]",
 0
};
oyUiHandler_s oy_ui_fltk_handler_json_headline_ =
  {oyOBJECT_UI_HANDLER_S,0,0,0,         /**< oyStruct_s members */
   (char*)"oyJSON",                     /**< dialect */
   (char*)"oyjl",                       /**< parser_type */
   (oyUiHandler_f)oyJSON2XFORMsFltkHtmlHeadlineHandler, /**<oyUiHandler_f handler*/
   (char*)"oyjl node",                  /**< handler_type */
   (char**)oy_ui_fltk_handler_json_headline_element_searches_ /**< element_searches */
  };

/** @internal
 *  Function oyJSON2XFORMsFLTKSelect1Handler
 *  @brief   build a UI for a xf:select1 XFORMS sequence
 *
 *  This function is a handler for a OpenICC style JSON ui.
 *
 *  @param[in]     cur                 libxml2 node
 *  @param[in]     collected_elements  unused
 *  @param[in]     user_data           toolkit context
 *  @return                            error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/01/03
 *  @since   2009/08/29 (Oyranos: 0.1.10)
 */
int        oyJSON2XFORMsFLTKSelect1Handler (
                                       oyjl_val            ov,
                                       oyOptions_s       * collected_elements OY_UNUSED,
                                       oyPointer           user_data )
{
  int k,
      is_default, default_pos = -1;
  const char * default_value = NULL,
             * label,
             * value,
             * xpath = 0;
  char * default_string = 0;
  oyFormsArgs_s * forms_args = (oyFormsArgs_s *)user_data;
  int print = forms_args ? forms_args->print : 1;

  oyjl_val choices;

  Fl_Group *parent = Fl_Group::current();

  if(!parent || !ov)
    return 1;


  label = NULL;

  oyjl_val v = oyjlTreeGetValue( ov, 0, "name" );
  if( OYJL_IS_STRING( v ) )         label = v->u.string;

  if(!label) return 1;

  int x = parent->x(),
      y = parent->y(),
      w = parent->w()/*,
      h = parent->h()*/;
  Fl_Pack * pack = new Fl_Pack(x,y,w,BUTTON_HEIGHT);
  pack->type( FL_HORIZONTAL );
  pack->spacing(H_SPACING);

  new Fl_Box( 0,0,H_SPACING,BUTTON_HEIGHT);
  OyFl_Box_c * box = new OyFl_Box_c( 2*H_SPACING,0,w-BOX_WIDTH-4*H_SPACING,BUTTON_HEIGHT);
  box->align( FL_ALIGN_LEFT | FL_ALIGN_INSIDE );

  formsFltkChoice * c = new formsFltkChoice( w-BOX_WIDTH-H_SPACING,0,BOX_WIDTH,BUTTON_HEIGHT );

  {
    v = oyjlTreeGetValue( ov, 0, "default" );
    if( OYJL_IS_STRING( v ) )         default_value = v->u.string;
    if(oy_debug && default_value && print)
      printf( "found default: \"%s\"\n", default_value );

    v = oyjlTreeGetValue( ov, 0, "key" );
    if( OYJL_IS_STRING( v ) )         xpath = v->u.string;

    if(print)
      box->copy_label( (const char *)label );

    const char * t = NULL;
    v = oyjlTreeGetValue( ov, 0, "help" );
    if( OYJL_IS_STRING( v ) ) t = v->u.string;
    if(t)
    {
      oyCallback_s * cb = 0;
      oyOptions_FindData( (oyOptions_s*)forms_args->data_,
                                      OYFORMS_FLTK_HELP_VIEW_REG,
                                      (oyPointer*)&cb, 0, 0);
      if(cb)
        c->hint_callback = cb;
      c->user_data( (void*)strdup(t) );
    }

    choices = oyjlTreeGetValue( ov, 0, "choices" );
    int count = oyjlValueCount( choices );
    for(int i = 0; i < count; ++i)
    {
      oyjl_val item = oyjlTreeGetValueF( choices, 0, "[%d]", i );
      oyjl_val v;

      is_default = 0;
      label = value = 0;

      v = oyjlTreeGetValue( item, 0, "name" );
      if( OYJL_IS_STRING( v ) ) label = v->u.string;
      v = oyjlTreeGetValue( item, 0, "nick" );
      if( OYJL_IS_STRING( v ) ) value = v->u.string;

      if(label)
      {
        /* detect default */
        if( value && default_value &&
            oyStrcmp_( default_value, value ) == 0 )
        {
          is_default = 1;
          default_pos = i;
        }

        if(!value) value = label;
        if(!label) label = value;

        /* append the choice
         * store the label and value in user_data() for evaluating results*/
        if(print)
        {
          fltk_cb_data *cb_data=(fltk_cb_data*)malloc(sizeof(fltk_cb_data));
          int len = strlen(label), pos = 0;
          memset(cb_data, 0, sizeof(fltk_cb_data) );
          cb_data->label = (char*) malloc(strlen(label)*2);
          cb_data->value = strdup(value);
          cb_data->key = xpath ? strdup(xpath):0;
          cb_data->callback_data = (oyOptions_s**)
                                              &forms_args->xforms_data_model_;
          for(k = 0; k <= len; ++k)
          {
            if(label[k] == '/')
              cb_data->label[pos++] = '\\';
            cb_data->label[pos++] = label[k];
          }
          c->add( (const char *) cb_data->label, 0,
                    fltkCallback,
                    (void*)cb_data, 0 );

          if(is_default)
            default_string = oyStringCopy_( label, malloc );
        }
      }
    }

    if(default_string && default_pos >= 0)
    {
      c->value( c->find_item( (char*)default_string ) );
      free( default_string ); default_string = 0;
    } else
      c->value( -1 );
  }

  pack->end();
  pack->resizable( box );

  /* collect results */
  if(xpath && forms_args)
    oyOptions_SetFromString( (oyOptions_s**)&forms_args->xforms_data_model_,
                             xpath, default_value, OY_CREATE_NEW );

  return 0;
}

const char * oy_ui_fltk_handler_json_select1_element_searches_[] = {
 "org/freedesktop/oyjl/modules/[]/groups/[]/options/[]",
 0
};

oyUiHandler_s oy_ui_fltk_handler_json_options_ =
  {oyOBJECT_UI_HANDLER_S,0,0,0,        /**< oyStruct_s members */
   (char*)"oyJSON",                   /**< dialect */
   (char*)"oyjl",                   /**< parser_type */
   (oyUiHandler_f)oyJSON2XFORMsFLTKSelect1Handler, /**<oyUiHandler_f handler*/
   (char*)"oyjl node",                     /**< handler_type */
   (char**)oy_ui_fltk_handler_json_select1_element_searches_ /**< element_searches */
  };

oyUiHandler_s * oy_ui_fltk_handlers[5] = {
  &oy_ui_fltk_handler_xf_select1_,
  &oy_ui_fltk_handler_html_headline_,
  &oy_ui_fltk_handler_json_headline_,
  &oy_ui_fltk_handler_json_options_,
  0
};


