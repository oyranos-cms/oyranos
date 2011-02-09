/** @file oyOptions_s_.c

   [Template file inheritance graph]
   +-> Options_s_.template.c
   |
   +-> BaseList_s_.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @since    2011/02/09
 */



  
#include "oyOptions_s.h"
#include "oyOptions_s_.h"

#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  





/** @internal
 *  Function oyOptions_New_
 *  @memberof oyOptions_s_
 *  @brief   allocate a new oyOptions_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyOptions_s_ * oyOptions_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_OPTIONS_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyOptions_s_ * s = 0;

  if(s_obj)
    s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyOptions_s_) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyOptions_Copy;
  s->release = (oyStruct_Release_f) oyOptions_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Options constructor ----- */
  error = !oyObject_SetParent( s_obj, oyOBJECT_OPTIONS_S, s );
  /* ---- end of custom Options constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */


  
  s->list_ = oyStructList_Create( s->type_, 0, 0 );


  
  /* ---- start of custom Options constructor ----- */
  error = oyOptions_Init__Members( s );
  /* ---- end of custom Options constructor ------- */
  
  
  
  
  return s;
}

/** @internal
 *  Function oyOptions_Copy__
 *  @memberof oyOptions_s_
 *  @brief   real copy a Options object
 *
 *  @param[in]     options                 Options struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyOptions_s_ * oyOptions_Copy__ ( oyOptions_s_ *options, oyObject_s object )
{
  oyOptions_s_ *s = 0;
  int error = 0;

  if(!options || !object)
    return s;

  s = oyOptions_New_( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Options copy constructor ----- */
    error = oyOptions_Copy__Members( s, options );
    /* ---- end of custom Options copy constructor ------- */
    
    
    
    
    
    s->list_ = oyStructList_Copy( options->list_, s->oy_ );

  }

  if(error)
    oyOptions_Release_( &s );

  return s;
}

/** @internal
 *  Function oyOptions_Copy_
 *  @memberof oyOptions_s_
 *  @brief   copy or reference a Options object
 *
 *  @param[in]     options                 Options struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyOptions_s_ * oyOptions_Copy_ ( oyOptions_s_ *options, oyObject_s object )
{
  oyOptions_s_ *s = options;

  if(!options)
    return 0;

  if(options && !object)
  {
    s = options;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyOptions_Copy__( options, object );

  return s;
}
 
/** @internal
 *  Function oyOptions_Release_
 *  @memberof oyOptions_s_
 *  @brief   release and possibly deallocate a Options list
 *
 *  @param[in,out] options                 Options struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyOptions_Release_( oyOptions_s_ **options )
{
  /* ---- start of common object destructor ----- */
  oyOptions_s_ *s = 0;

  if(!options || !*options)
    return 0;

  s = *options;

  *options = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom Options destructor ----- */
  oyOptions_Release__Members( s );
  /* ---- end of custom Options destructor ------- */
  
  
  
  

  oyStructList_Release( &s->list_ );


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Options.private_methods_definitions.c" { */
int            oyOptions_Init__Members(oyOptions_s_      * s )
{
  return 0;
}
int            oyOptions_Copy__Members(oyOptions_s_      * dest,
                                       oyOptions_s_      * src )
{
  return 0;
}
int            oyOptions_Release__Members (
                                       oyOptions_s_      * s )
{
  return 0;
}


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
      oyStringListAddStaticString_( texts, texts_n, (const char*)cur->name,
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

/* } Include "Options.private_methods_definitions.c" */

