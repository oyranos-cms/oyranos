#ifdef HAVE_LIBXML2
/**
 *  @internal
 *  Function oyOptions_ParseXML_
 *  @memberof oyOptions_s
 *  @brief   deserialise a text file to oyOptions_s data
 *
 *  This function is parsing libxml2 structures.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/17 (Oyranos: 0.1.9)
 *  @date    2009/09/01
 */
void           oyOptions_ParseXML_   ( oyOptions_s_      * s,
                                       char            *** texts,
                                       int               * texts_n,
                                       xmlDocPtr           doc,
                                       xmlNodePtr          cur )
{
  oyOption_s * o = 0;
  char * tmp = 0;
  int i;
  xmlChar *key = 0;

  while (cur != NULL)
  {
    if(cur->type == XML_ELEMENT_NODE)
      oyStringListAddStaticString( texts, texts_n, (const char*)cur->name,
                                    oyAllocateFunc_, oyDeAllocateFunc_ );

    if(cur->xmlChildrenNode)
    {
      oyOptions_ParseXML_( s, texts, texts_n, doc, cur->xmlChildrenNode );
      *texts_n -= 1;
      oyDeAllocateFunc_( (*texts)[*texts_n] );
    }

    if(cur->type == XML_TEXT_NODE && !cur->children &&
       cur->content && cur->content[0] &&
       cur->content[0] != '\n')
    {
      for( i = 0; i < *texts_n; ++i )
      {
        if(i)
          STRING_ADD( tmp, "/" );
        STRING_ADD( tmp, (*texts)[i] );
      }

      if(tmp)
        o = (oyOption_s*)oyOption_FromRegistration( tmp, 0 );

      if(!o)
        goto clean_stage;

      key = xmlNodeListGetString(doc, cur, 1);
      oyOption_SetFromText( o, (char*)key, 0 );
      xmlFree(key);

      oyOption_SetSource( o, oyOPTIONSOURCE_DATA );

      oyOptions_MoveIn( (oyOptions_s*)s, &o, -1 );

      clean_stage:
      if(tmp)
        oyFree_m_( tmp );
    }
    cur = cur->next;
  }
}
#endif
