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
                                       oyPointer           handler_context );

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
  char               * handler_type;   /**< informational handler context type*/
  /** The elements to collect by the parser, e.g.
   *  "xf:choices/xf:item/xf:label.xf:value".
   *  The list shall be terminated by zero. */
  char               * element_search[];
} oyUiHandler_s;

void               oyParseXMLNode_   ( xmlDocPtr           doc,
                                       xmlNodePtr          cur,
                                       oyOptions_s       * wid_data,
                                       oyUiHandler_s    ** ui_handlers,
                                       oyPointer           ui_handlers_context);
const char *       oyXFORMsModelGetXPathValue_
                                     ( xmlDocPtr           doc,
                                       const char        * reference );
char *             oyXML2NodeName_   ( xmlNodePtr          cur );





int        oyXML2XFORMsSelect1Handler( xmlNodePtr          cur,
                                       oyOptions_s       * collected_elements,
                                       oyPointer           user_data );

oyUiHandler_s oy_ui_handler_xf_select1_ =
  {oyOBJECT_UI_HANDLER_S,0,0,0,        /**< oyStruct_s members */
   "oyFORMS",                          /**< dialect */
   "libxml2",                          /**< parser_type */
   "xf:select1",                       /**< element_type; Wanted XML element. */
   (oyUiHandler_t)oyXML2XFORMsSelect1Handler, /**< oyUiHandler_t handler*/
   "dummy",                            /**< handler_type */
   {"xf:choices/xf:item/xf:label.xf:value",0} /**< element_search */
  };

oyUiHandler_s * oy_ui_handlers[] = {
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
   "    <xf:instance xmlns=\"\">\n"
   "      %s"
   "    </xf:instance>\n"
   "  </xf:model>\n"
   "</head>\n"
   "<body>\n%s\n"
   "</body></html>", data, ui_text );
  printf("%s\n", text);

  /*printf("%s\n", oyFilterNode_GetText( node, oyNAME_NICK ));*/

  doc = xmlParseMemory( text, strlen(text) );
  cur = xmlDocGetRootElement(doc);

  oyParseXMLNode_( doc, cur, 0, oy_ui_handlers, 0 );

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
                                       oyOptions_s       * wid_data,
                                       oyUiHandler_s    ** ui_handlers,
                                       oyPointer           ui_handlers_context )
{
  xmlChar *val = 0;

  while(cur != NULL)
  {
    oyOptions_s * old_wid_data = 0;
    int collect = 0;
    char * name = 0;
    char * p_name = 0;
    const char * search = 0;
    char * tmp = 0;
    int pos, len, error = 0;

    if(cur->parent->type == XML_ELEMENT_NODE)
      p_name = oyXML2NodeName_(cur->parent);
    else
      STRING_ADD( p_name, "root" );

    if(cur->type == XML_ELEMENT_NODE)
    {
      name = oyXML2NodeName_(cur);
      if(oy_debug)
        printf(" name: (%s)->%s\n", p_name, name);

      pos = 0;
      while(ui_handlers[pos])
      {
        STRING_ADD( tmp, ui_handlers[pos]->element_search[0] );
        len = (int)(oyStrrchr_(tmp, '/') - tmp);
        if(oyStrchr_(tmp, '/'))
          tmp[len] = 0;

        if(oyStrstr_( ui_handlers[pos]->element_type, name ) != 0 ||
           oyStrstr_( tmp, name ) != 0)
        {
          old_wid_data = wid_data;
          wid_data = 0;
          search = ui_handlers[pos]->element_search[0];
          if(oyStrstr_( search, name ))
            search = oyStrstr_( search, name ) + oyStrlen_(name) + 1;

          error = oyOptions_SetFromText( &wid_data, "////search",
                                         search, OY_CREATE_NEW );
          if(error) printf("%s:%d error\n\n", __FILE__,__LINE__);
          collect = 1;
        }

        oyFree_m_( tmp )

        ++pos;
      }

      xmlAttrPtr attr = cur->properties;
      while(attr && attr->name)
      {
        if(oy_debug)
        {
          printf(" attr: %s=", attr->name );
          if(attr->children && attr->children->content)
            printf("%s\n", attr->children->content);
          else
            printf("\n");
        }

        if( strcmp((char*)attr->name,"ref") == 0 &&
            attr->children->content )
        {
          const char * v;

          v = oyXFORMsModelGetXPathValue_( doc, (char*)attr->children->content);
          if(v && oy_debug)
            printf( "Found: %s=\"%s\"\n", attr->children->content, v );


          if(wid_data && oyOptions_FindString(wid_data, "search", 0))
          {
            STRING_ADD( tmp, "////" );
            STRING_ADD( tmp, name );
            error = oyOptions_SetFromText( &wid_data, tmp, v, OY_CREATE_NEW );
            if(error) printf("%s:%d error\n\n", __FILE__,__LINE__);
            oyFree_m_( tmp )
          }
        }

        attr = attr->next;
      }
    }

    if(cur->xmlChildrenNode)
      oyParseXMLNode_( doc, cur->xmlChildrenNode, wid_data,
                       ui_handlers, ui_handlers_context );

    if(cur->type == XML_TEXT_NODE && !cur->children &&
       cur->content && cur->content[0] &&
       cur->content[0] != '\n')
    {
      val = xmlNodeListGetString(doc, cur, 1);
      if(oy_debug)
        printf("  val: %s\n", val);
    }

    search = oyOptions_FindString(wid_data, "search", 0);

    if(p_name && wid_data && val &&
       search &&
       oyStrstr_(search, p_name) != 0)
    {
      STRING_ADD( tmp, "////" );
      STRING_ADD( tmp, p_name );
      error = oyOptions_SetFromText( &wid_data, tmp, (char*)val, OY_CREATE_NEW);
      if(error) printf("%s:%d error\n\n", __FILE__,__LINE__);
      /*printf("  set: %s-%s to %x\n", tmp, val, (int)wid_data );*/
      oyFree_m_( tmp )
    }


    /* clean old search context */
    if(collect)
    {
      pos = 0;
      while(ui_handlers[pos])
      {
        if(strcmp( name, ui_handlers[pos]->element_type ) == 0)
        {
          ui_handlers[pos]->handler( cur, wid_data, ui_handlers_context );
        }
        ++pos;
      }


      if(old_wid_data)
      {
        STRING_ADD( tmp, "////" );
        STRING_ADD( tmp, (name?name:p_name) );
        error = oyOptions_MoveInStruct( &old_wid_data, tmp,
                                (oyStruct_s**)&wid_data, OY_ADD_ALWAYS );
        if(error) printf("%s:%d error\n\n", __FILE__,__LINE__);
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
  const char * ref, * url;
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
    sprintf( xpath, "/xmlns:html/xmlns:head/xf:model/xf:instance" );

#if 0
    /* inject "xmlns:", but that can be done easier with a xmlns="" attribute */
    const char *t, *t2;
    int len;

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
#else
    STRING_ADD( xpath, reference );
#endif
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


/** @internal
 *  Function oyXML2XFORMsSelect1Handler
 *  @brief   build a UI for a xf:select1 XFORMS sequence
 *
 *  @param[in]     cur                 libxml2 node
 *  @param[in]     collected_elements  parsed and requested elements
 *  @param[in]     user_data           toolkit context
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/29 (Oyranos: 0.1.10)
 *  @date    2009/08/29
 */
int        oyXML2XFORMsSelect1Handler( xmlNodePtr          cur,
                                       oyOptions_s       * collected_elements,
                                       oyPointer           user_data )
{
  oyOption_s * o  = 0, * o2, *o3;
  int n = oyOptions_Count( collected_elements ),
      i,j,j_n,k,k_n,
      is_default,
      choices_n = 0;
  oyOptions_s * opts = 0, * opts2;
  const char * default_value = 0,
             * tmp,
             * label,
             * value;

  tmp = oyOptions_FindString( collected_elements, "xf:select1", 0 );
  if(tmp)
  {
    default_value = tmp;

    if(oy_debug)
      printf( "found default: \"%s\"\n", default_value );
  }

  for(i = 0; i < n; ++i)
  {
    o = oyOptions_Get( collected_elements, i );
    opts = (oyOptions_s*) oyOption_StructGet( o, oyOBJECT_OPTIONS_S );
    
    if(opts && oyFilterRegistrationMatch( o->registration,"xf:choices", 0 ))
    {
      printf( "Select \"\": " );

      j_n = oyOptions_Count( opts);
      for(j = 0; j < j_n; ++j)
      {
        o2 = oyOptions_Get( opts, j );
        opts2 = (oyOptions_s*) oyOption_StructGet( o2, oyOBJECT_OPTIONS_S );

        if(opts2 && oyFilterRegistrationMatch( o2->registration,"xf:item", 0 ))
        {
          label = tmp = value = 0;
          is_default = 0;

          k_n = oyOptions_Count( opts2);
          for(k = 0; k < k_n; ++k)
          {
            o3 = oyOptions_Get( opts2, k );
            if(oy_debug)
              printf( "    found option: 0x%x  \"%s\" %s\n",
                (int)o3, oyOption_GetText(o3, oyNAME_NICK),
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
            is_default = 1;

          if(choices_n)
            printf( "; " );
          printf( "%c%s - \"%s\"%c",
                  is_default?'<':' ', value, label, is_default?'>':' ' );
          ++choices_n;
        }
        else if(oy_debug)
          printf( "  found option: 0x%x  \"%s\" %s\n",
                (int)o2, oyOption_GetText(o2, oyNAME_NICK),
                oyStruct_TypeToText((oyStruct_s*)o2) );

        oyOptions_Release( &opts2 );
        oyOption_Release( &o2 );
      }
      printf( "\n" );
    }
    else if(oy_debug)
      printf( "found option: 0x%x  \"%s\" %s\n",
              (int)o, oyOption_GetText(o, oyNAME_NICK),
              oyStruct_TypeToText((oyStruct_s*)o) );

    oyOptions_Release( &opts );
    oyOption_Release( &o );
  }

  /*printf("collected:\n%s", oyOptions_GetText( collected_elements, oyNAME_NICK));*/
  return 0;
}
