/* gcc -Wall -g `pkg-config oyranos libxml-2.0 --libs --cflags` oy_filter_node.c -o oy_filter_node */
#include <oyranos_alpha.h>
#include <oyranos_helper.h>
#include <oyranos_i18n.h>
#include <oyranos_texts.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <stdio.h>
#include <string.h>




int        oyXML2XFORMsSelect1Handler( xmlNodePtr          cur,
                                       oyOptions_s       * collected_elements,
                                       oyPointer           user_data );

oyUiHandler_s oy_ui_handler_xf_select1_ =
  {oyOBJECT_UI_HANDLER_S,0,0,0,        /**< oyStruct_s members */
   "oyFORMS",                          /**< dialect */
   "libxml2",                          /**< parser_type */
   "xf:select1",                       /**< element_type; Wanted XML element. */
   (oyUiHandler_f)oyXML2XFORMsSelect1Handler, /**< oyUiHandler_f handler*/
   "dummy",                            /**< handler_type */
   "xf:choices/xf:item/xf:label.xf:value" /**< element_search */
  };

oyUiHandler_s * oy_ui_handlers[] = {
  &oy_ui_handler_xf_select1_,
  0
};

int main (int argc, char ** argv)
{
  const char * node_name = argc > 1 ? argv[1] : "//imaging/icc.lcms";
  oyFilterNode_s * node = oyFilterNode_NewWith( node_name, 0,0 );
  char * ui_text = 0,
      ** namespaces = 0,
       * text = 0;
  const char * data = 0;
  int error = 0,
      i;
  oyOptions_s * opts = 0;

  oyOptions_Release( &node->core->options_ );
  /* First call for options ... */
  opts = oyFilterNode_OptionsGet( node, 
                                  OY_SELECT_FILTER | OY_SELECT_COMMON |
                                  oyOPTIONATTRIBUTE_ADVANCED |
                                  oyOPTIONATTRIBUTE_FRONT );
  /* ... then get the UI for this filters options. */
  error = oyFilterNode_UiGet( node, &ui_text, &namespaces, malloc );
  oyFilterNode_Release( &node );

  data = oyOptions_GetText( opts, oyNAME_NAME );
  text = oyXFORMsFromModelAndUi( data, ui_text, (const char**)namespaces,
                                 malloc );

  if(namespaces)
  {
    i = 0;
    while(namespaces[i])
      free( namespaces[i++] );
    free(namespaces);
  }
  if(ui_text) free(ui_text); ui_text = 0;
  oyOptions_Release( &opts );

  if(oy_debug)
    printf("%s\n", text);

  error = oyXFORMsRenderUi( text, oy_ui_handlers, 0 );

  /* xmlParseMemory sollte der Ebenen gewahr werden wie oyOptions_FromText. */
  opts = oyOptions_FromText( data, 0,0 );

  if(text) free(text); text = 0;

  return error;
}


