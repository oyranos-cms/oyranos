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

void               oyParseXMLNode_   ( xmlDocPtr           doc,
                                       xmlNodePtr          cur,
                                       oyOptions_s       * wid_data );
const char *       oyXFORMsModelGetXPathValue_
                                     ( xmlDocPtr           doc,
                                       const char        * reference );
char *             oyXML2NodeName_   ( xmlNodePtr          cur );

/** @internal
 *  @typedef oyUiHandler_t
 *  @brief   handle parser output and build the UI
 *
 *  @param[in]     cur                 libxml2 node
 *  @param[in]     collected_elements  from libxml2
 *  @return                            ns + ':' + name
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/30 (Oyranos: 0.1.10)
 *  @date    2009/08/30
 */
typedef int  (*oyUiHandler_t)        ( oyPointer           cur,
                                       oyOptions_s       * collected_elements,
                                       oyPointer           user_data );

/** @internal
 *  @typedef oyUiHandler_s
 *  @brief   provide a list of handlers to build the UI
 *
 *  A parser will read out the XFORMS elements and collect those a UI handler
 *  claims interesst in. The handler is then called to process the collected 
 *  elements and to build the UI.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/30 (Oyranos: 0.1.10)
 *  @date    2009/08/30
 */
typedef struct {
  oyOBJECT_e           type;           /**< oyOBJECT_UI_HANDLER_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private features name and hash */

  char               * dialect;        /**< currently only "oyFORMS",
                                            a subset of W3C XFORMS */
  char               * parser_type;    /**< currently only "libxml2" */
  char               * element_type;   /**< a valid XFORMS element,
                                            e.g. "xf:select1" */
  oyUiHandler_t        handler;        /**< The handler which obtains the parsed
                                            results and a context to construct
                                            the UI. */
  /** The elements to collect by the parser, e.g.
   *  "xf:choices/xf:item/xf:label.xf:value".
   *  The list shall be terminated by zero. */
  char               * element_search[];
} oyUiHandler_s;

int        oyXML2XFORMsSelect1Handler( xmlNodePtr          cur,
                                       oyOptions_s       * collected_elements,
                                       oyPointer           user_data );

oyUiHandler_s oy_ui_handler_xf_select1_ =
  {oyOBJECT_UI_HANDLER_S,0,0,0,        /**< oyStruct_s members */
   "libxml2",                          /**< parser_type */
   "xf:select1",                       /**< Wanted XML element. */
   (oyUiHandler_t)oyXML2XFORMsSelect1Handler, /**< oyXFORMsUiHandler_t handler*/
   {"xf:choices/xf:item/xf:label.xf:value",0} /**< element_search */
  };

oyUiHandler_s * ui_handlers[] = {
  &oy_ui_handler_xf_select1_,
  0
};

int main (int argc, char ** argv)
{
  oyFilterNode_s * node = oyFilterNode_NewWith( "//imaging/icc.lcms", 0,0 );
  char * ui_text = 0, * text = 0;
  const char * data = 0;
  int error = oyFilterNode_UiGet( node, &ui_text, malloc );
  xmlDocPtr doc = 0;
  xmlNodePtr cur = 0;
  oyOptions_s * opts = 0;

  oyOptions_Release( &node->core->options_ );
  opts = oyFilterNode_OptionsGet( node, 
                            OY_SELECT_FILTER | oyOPTIONATTRIBUTE_ADVANCED );

  data = oyOptions_GetText( opts, oyNAME_NAME );
  text = malloc( strlen(ui_text) + strlen( data ) + 1024 );
  sprintf( text,
   "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
   "xmlns:xf=\"http://www.w3.org/2002/xforms\">\n"
   "<head>\n"
   "  <title>lcms options</title>\n"
   "  <xf:model>\n"
   "    <data>\n"
   "      %s\n"
   "    </data>\n"
   "  </xf:model>\n"
   "</head>\n"
   "<body>\n%s\n"
   "</body></html>", data, ui_text );
  printf("%s\n", text);

  /*printf("%s\n", oyFilterNode_GetText( node, oyNAME_NICK ));*/

  doc = xmlParseMemory( text, strlen(text) );
  cur = xmlDocGetRootElement(doc);

  oyParseXMLNode_( doc, cur, 0 );

  oyOptions_Release( &opts );

  /* xmlParseMemory sollte der Ebenen gewahr werden wie oyOptions_FromText. */
  opts = oyOptions_FromText( data, 0,0 );

  if(ui_text) free(ui_text); ui_text = 0;
  if(text) free(text); text = 0;
  oyFilterNode_Release( &node );
  xmlFreeDoc( doc );

  return error;
}


/** @internal
 *  Function oyXML2NodeName_
 *  @brief   join namespace and node name
 *
 *  @param[in]     pattern             libxml2 node
 *  @return                            ns + ':' + name
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/28 (Oyranos: 0.1.10)
 *  @date    2009/08/28
 */
char *             oyXML2NodeName_   ( xmlNodePtr          cur )
{
  char * name = 0;
  const xmlChar * prefix = cur->ns && cur->ns->prefix ? cur->ns->prefix : 0;
  if(prefix)
  {
    STRING_ADD( name, (char*)prefix );
    STRING_ADD( name, ":" );
  }
  STRING_ADD( name, (char*)cur->name );
  return name;
}

void               oyParseXMLNode_   ( xmlDocPtr           doc,
                                       xmlNodePtr          cur,
                                       oyOptions_s       * wid_data )
{
  xmlChar *key = 0;

  while(cur != NULL)
  {
    oyOptions_s * old_wid_data = 0;
    int collect = 0;
    char * name = 0;
    char * p_name = 0;
    const char * search = 0;
    char * tmp = 0;

    if(cur->parent->type == XML_ELEMENT_NODE)
      p_name = oyXML2NodeName_(cur->parent);
    else
      STRING_ADD( p_name, "root" );

    if(cur->type == XML_ELEMENT_NODE)
    {
      name = oyXML2NodeName_(cur);
      printf(" name: (%s)->%s\n", p_name, name);

      if(strcmp( name, "xf:select1" ) == 0)
      {
        old_wid_data = wid_data;
        wid_data = 0;
        oyOptions_SetFromText( &wid_data, "////search",
                               "xf:choices/xf:item", OY_CREATE_NEW );
        collect = 1;
      }

      if(strcmp( name, "xf:item" ) == 0)
      {
        old_wid_data = wid_data;
        wid_data = 0;
        oyOptions_SetFromText( &wid_data, "////search",
                               "xf:choices/xf:label.xf:value", OY_CREATE_NEW );
        collect = 1;
      }

      xmlAttrPtr attr = cur->properties;
      while(attr && attr->name)
      {
        printf(" attr: %s=", attr->name );
        if(attr->children && attr->children->content)
          printf("%s\n", attr->children->content);
        else
          printf("\n");

        if( strcmp((char*)attr->name,"ref") == 0 &&
            attr->children->content )
        {
          const char * v;

          v = oyXFORMsModelGetXPathValue_( doc, (char*)attr->children->content);
          if(v)
            printf( "Found: %s=\"%s\"\n", attr->children->content, v );


          if(wid_data && oyOptions_FindString(wid_data, "search", 0))
          {
            STRING_ADD( tmp, "////" );
            STRING_ADD( tmp, name );
            oyOptions_SetFromText( &wid_data, tmp, v, OY_CREATE_NEW );
            oyFree_m_( tmp )
          }
        }

        attr = attr->next;
      }
    }

    if(cur->xmlChildrenNode)
      oyParseXMLNode_(doc, cur->xmlChildrenNode, wid_data);

    if(cur->type == XML_TEXT_NODE && !cur->children &&
       cur->content && cur->content[0] &&
       cur->content[0] != '\n')
    {
      key = xmlNodeListGetString(doc, cur, 1);
      printf("  key: %s\n", key);
    }

    search = oyOptions_FindString(wid_data, "search", 0);

    if(p_name && wid_data && key &&
       search &&
       strstr(search, p_name) != 0)
    {
      STRING_ADD( tmp, "////" );
      STRING_ADD( tmp, p_name );
      oyOptions_SetFromText( &wid_data, tmp, (char*)key, OY_CREATE_NEW );
      oyFree_m_( tmp )
    }


    /* clean old search context */
    if(collect)
    {
      printf("collected:\n%s", oyOptions_GetText(wid_data, oyNAME_NICK));

      if(old_wid_data)
      {
        STRING_ADD( tmp, "////" );
        STRING_ADD( tmp, (name?name:p_name) );
        oyOptions_MoveInStruct( &old_wid_data, tmp,
                                (oyStruct_s**)&wid_data, -1 );
        oyFree_m_( tmp )
      }
      else
        oyOptions_Release( &wid_data );

      wid_data = old_wid_data;
      old_wid_data = 0;
    }

    if(name)
      oyFree_m_( name )
    if(p_name)
      oyFree_m_( p_name )

    cur = cur->next;
  }
}


const char * oyXFORMsModelGetXPathValue_
                                     ( xmlDocPtr           doc,
                                       const char        * reference )
{
  char * xpath = (char*) malloc(1024);
  xmlXPathContextPtr context = xmlXPathNewContext( doc );
  xmlXPathObjectPtr result = 0;
  int error = 0;
  const char * ref, * url, * t, *t2;
  int len;
  const char * text = 0;

  memset(xpath, 0, 1024);

  if( reference )
  {
    ref = "xmlns";
    url = "http://www.w3.org/1999/xhtml";
    error = xmlXPathRegisterNs( context, (xmlChar*)ref, (xmlChar*)url);
    if(error)
      printf("Could not register %s=%s\n", ref, url);
    ref = "xf";
    url = "http://www.w3.org/2002/xforms";
    error = xmlXPathRegisterNs( context, (xmlChar*)ref, (xmlChar*)url);
    if(error)
      printf("Could not register %s=%s\n", ref, url);

    /* add the static part */
    sprintf( xpath, "/xmlns:html/xmlns:head/xf:model/xmlns:data/" );
    /* add the first dynamic level */
    t = reference;
    if(t[0] != '/')
    {
      t2 = strchr(t,'/');
      len = t2 - t;
      sprintf( &xpath[strlen(xpath)], "xmlns:" );
      memcpy( &xpath[strlen(xpath)], t, len+1 );
    }
    /* add other dynamic levels */
    while((t = strchr(t,'/')) != 0)
    {
      t2 = strchr(t+1,'/');
      len = 0;

      sprintf( &xpath[strlen(xpath)], "xmlns:" );
      if(t2)
      {
        len = t2 - t;
        memcpy( &xpath[strlen(xpath)], t+1, len );
        t = t2;
      }
      else
      {
        sprintf( &xpath[strlen(xpath)], "%s", t+1 );
        break;
      }
    }
    result = xmlXPathEvalExpression( (xmlChar*)xpath, context );

    if( result && !xmlXPathNodeSetIsEmpty( result->nodesetval ) &&
        result->nodesetval &&
        result->nodesetval->nodeTab && result->nodesetval->nodeTab[0] &&
        result->nodesetval->nodeTab[0]->children &&
        strcmp((char*)result->nodesetval->nodeTab[0]->children->name, "text") == 0 &&
        result->nodesetval->nodeTab[0]->children->content)
      text = (char*)result->nodesetval->nodeTab[0]->children->content;
    else
      printf( "No result with %s\n", xpath);

    xmlXPathFreeObject( result );
    xmlXPathFreeContext( context );
  }

  if(xpath)
    free(xpath);

  return text;
}
