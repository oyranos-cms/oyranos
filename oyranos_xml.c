/** @file oyranos_xml.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2006-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    xml handling
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2006/02/10
 */



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"


/* memory handling for text parsing and writing */
/* mem with old_leng will be stretched if add dont fits inside */
int         oyMemBlockExtent_  (char **mem, int old_len, int add);
/* gives string bordered by a xml style keyword */
char*       oyXMLgetValue_     (const char       *xml,
                    const char       *key);
/* gives the position and length of a string bordered by a xml style keyword */
char* oyXMLgetField_  (const char       *xml,
                 const char       *key,
                 int              *len);
/* gives all strings bordered by a xml style keyword from xml */
char** oyXMLgetArray_  (const char       *xml,
                 const char       *key,
                 int              *count);
/* write option range to mem, allocating memory on demand */
char*       oyWriteOptionToXML_(oyGROUP_e           group,
                    oyWIDGET_e          start,
                    oyWIDGET_e          end, 
                    char             *mem,
                    int               oytmplen);

/* form for write modules */
typedef oyChar* (*oyOptionWrite_f)     ( oyOption_t_ ** opts,
                          oyChar    ** values,
                          int            n,
                          oyChar     * mem,
                          int            oytmplen );

/* miscellaneous */

/* small helpers */

/* --- function definitions --- */


int
oyMemBlockExtent_(char **mem, int old_len, int add)
{
  int new_len = old_len;
  DBG_PROG3_S("len1: %d %d %d",(int) strlen(*mem), (int)old_len, add);
  if( add > (old_len - strlen(*mem)) )
  {
    int len = add + strlen(*mem) + ((add > 120) ? add + 50 : 120);
    char *tmp = NULL;

    oyAllocHelper_m_( tmp, char, len, oyAllocateFunc_, return 0 );
    DBG_PROG1_S("len2: %d\n",len);
    memcpy( tmp, *mem, old_len  );
    oyFree_m_ (*mem);
    *mem = tmp;
    new_len = len;
  }
  return new_len;
}


/* sscanf is not  useable as it ignores after an empty space sign
   We get a allocated pure value string. */
oyChar*
oyXMLgetValue_  (const oyChar  * xml,
                 const oyChar  * key)
{
  const oyChar * val_pos = 0;
  oyChar *value1 = 0, *value2 = 0, *value = 0;
  int   len1 = oyStrlen_( key ) + 2,
        len2 = oyStrlen_( key ) + 3;
  oyChar * key1 = NULL,
           * key2 = NULL;
  int   open = 0;

  int len = 0;

  oyAllocString_m_( key1, len1+1, oyAllocateFunc_, return NULL );
  oyAllocString_m_( key2, len2+1, oyAllocateFunc_, return NULL );

# if 0
  if(xml && key)
    value1 = strstr(xml, key);
  if(value1)
  if (value1 > xml &&
      value1[-1] == '<' &&
      value1[ oyStrlen_(key) ] == '>')
  { value2 = oyStrstr_(value1+1, key);
    if(value2)
    if (value2[ -2 ] == '<' &&
        value2[ -1 ] == '/' &&
        value2[ oyStrlen_(key) ] == '>')
    {
      val_pos = value1 + oyStrlen_(key) + 1;
      len = (int)(value2 - val_pos - 1);
      /*/TODO char txt[128];
      //snprintf(txt,len,val_pos);*/
    }
  }
# else
  oySprintf_(key1, "<%s>", key);
  oySprintf_(key2, "</%s>", key);

  val_pos = value1 = strstr( xml, key1 ) + len1;

  if(value1-len1)
    ++open;

  while(open)
  {
    value2 =  oyStrstr_( val_pos, key2 ) + len2;
    val_pos = oyStrstr_( val_pos, key1 ) + len1;
    if(val_pos - len1 &&
       val_pos < value2)
      ++open;
    else
    {
      if(value2)
        --open;
      else
      {
        WARNc1_S("key: %s is not complete.", key)
        len = 0;
        return 0;
      }
    }
  }
  len = value2 - len2 - value1;
  oyFree_m_(key1); oyFree_m_(key2);
# endif
  if(len > 0 && (intptr_t)value1 - len1 > 0)
  {
    oyAllocHelper_m_( value, char, len+1, oyAllocateFunc_, return NULL );
    oySnprintf1_(value, len+1, "%s", value1);
  }

  return value;
}

/* We dont get the starting point and length of the found value. */
char*
oyXMLgetField2_ (const char       *xml,
                 const char       *start_key,
                 const char       *end_key,
                 int              *len)
{
  const char* val_pos = 0;
  char *value1 = 0, *value2 = 0;
  intptr_t l = 0;
  int   len1 = strlen( start_key ) + 2,
        len2 = strlen( start_key ) + 3;
  const char *key1 = start_key,
             *key2 = end_key;
  int   open = 0;

  *len = 0;

  val_pos = value1 = strstr( xml, key1 ) + len1;

  if(value1)
    ++open;

  if(val_pos - len1)
  while(open)
  {
    value2 =  strstr( val_pos, key2 ) + len2;
    val_pos = strstr( val_pos, key1 ) + len1;
    if(val_pos - len1 &&
       val_pos < value2)
      ++open;
    else
    {
      if(value2)
        --open;
      else
      {
        WARNc1_S("key: %s is not complete.", key1)
        l = 0;
        return 0;
      }
    }
  }
  l = value2 - len2 - value1;
  if(l < 0)
  {
    l = 0;
    value1 = NULL;
  }

  *len = l;

  return value1;
}

char*
oyXMLgetField_  (const char       *xml,
                 const char       *key,
                 int              *len)
{
  char *value1 = 0;
  intptr_t l = 0;
  int   len1 = strlen( key ) + 2,
        len2 = strlen( key ) + 3;
  char *key1 = NULL,
       *key2 = NULL;

  oyAllocHelper_m_( key1, char, len1+1, oyAllocateFunc_, return NULL );
  oyAllocHelper_m_( key2, char, len2+1, oyAllocateFunc_, return NULL );

  *len = 0;

  if(!xml) goto clean;

  /* we search for a <key>...</key> pair */
  oySprintf_(key1, "<%s>", key);
  oySprintf_(key2, "</%s>", key);

  value1 = oyXMLgetField2_( xml, key1, key2, len );

  /* we try to find a <key ...>...</key> pattern */
  if(!value1 && *len <= 0)
  {
    oySprintf_(key1, "<%s ", key);

    value1 = oyXMLgetField2_( xml, key1, key2, len );
  }


  clean:
  oyFree_m_(key1); oyFree_m_(key2);

  *len = l;

  return value1;
}


/**
 *  @internal
 *  Function oyXMLgetElement_
 *  @brief   parse a text snippet for a XPATH expression
 *
 *  @param         xml                 the XML text
 *  @param         key                 the element name
 *  @return                            the element's start position
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/14 (Oyranos: 0.1.9)
 *  @date    2008/11/14
 */
char*        oyXMLgetElement_        ( const char        * xml,
                                       const char        * xpath,
                                       const char        * key )
{
  char * text = 0;
  const char * start = xml;
  int xpaths_n = -1, pices_n = -1, i, j;
  char ** xpaths = oyStringSplit_( xpath, '/', &xpaths_n, oyAllocateFunc_ ),
       ** pices = 0;

  for( i = 0; i < xpaths_n; ++i )
  {
    pices_n = -1;
    pices = oyXMLgetArray_( start, xpaths[i], &pices_n );
    for( j = 0; j < pices_n; ++j )
    {
      text = oyXMLgetElement_( pices[j], oyStrchr_( xpath, '/' ), key );
      if(text) break;
    }
    if(text) break;
  }

  if(xpaths && xpaths_n)
    oyStringListRelease_( &xpaths, xpaths_n, oyDeAllocateFunc_ );

  if(!xpath)
  {
    pices = oyXMLgetArray_( xml, key, &pices_n );
    if(pices)
      text = oyStringCopy_( pices[0], oyAllocateFunc_ );
  }

  if(pices)
    oyStringListRelease_( &pices, pices_n, oyDeAllocateFunc_ );

  return text;
}

/* There is no check for using the same key on a lower hirarchy. */
char**
oyXMLgetArray_  (const char       *xml,
                 const char       *key,
                 int              *count)
{
  char       **values = 0;
  const char  *val_pos = xml;

  int n = 0, i, len=0;

  *count = 0;
  while((val_pos = oyXMLgetField_(val_pos, key, &len)) != NULL)
  {
    char     **ptr = NULL;

    oyAllocHelper_m_( ptr, char*, n+1, oyAllocateFunc_, return NULL )
    for(i = 0; i < n; ++i)
      ptr[i] = values[i];
    if(values)
      oyFree_m_(values);
    values = ptr;
    oyAllocHelper_m_( ptr[n], char, len+1, oyAllocateFunc_, return NULL )
    snprintf( ptr[n], len+1, "%s", val_pos);
    val_pos += len;
    ++n;
  }
  *count = n;

  return values;
}

/* The function expects one single group to be present, usually the first opt.
   The caller must do sorting for this internal function itself. */
oyChar*
oyWriteOptionsToXML_    ( oyOption_t_ ** opts,
                          oyChar    ** values,
                          int            n,
                          oyChar     * mem,
                          int            oytmplen )
{
  int i;
  oyOption_t_ * group_opt = NULL;

  DBG_PROG_START

  for( i = 0; i < n; ++i )
  {
    oyOption_t_ * opt = opts[i];
    oyWIDGET_TYPE_e opt_type = opt->type;

    switch(opt_type)
    {
      case oyWIDGETTYPE_START:
      case oyWIDGETTYPE_PROFILE:
      case oyWIDGETTYPE_INT:
      case oyWIDGETTYPE_FLOAT:
      case oyWIDGETTYPE_CHOICE:
      case oyWIDGETTYPE_LIST:
      case oyWIDGETTYPE_VOID:
      case oyWIDGETTYPE_END:
           break;
      case oyWIDGETTYPE_GROUP_TREE:
           group_opt = opt;

           /* allocate new mem if needed */
           oytmplen = oyMemBlockExtent_(&mem, oytmplen, 360);
           oySprintf_( &mem[oyStrlen_(mem)], "<%s>\n",
                       opt->config_string_xml );
           oySprintf_( &mem[oyStrlen_(mem)], "<!-- %s \n"
                                             "     %s -->\n\n",
                       opt->name,
                       opt->description );
           oySprintf_( &mem[oyStrlen_(mem)], "<!-- %s \n"
                                             "     %s -->\n\n",
                       opt->name,
                       opt->description );

           break;
      case oyWIDGETTYPE_DEFAULT_PROFILE:
           break;
      case oyWIDGETTYPE_BEHAVIOUR:
           break;
    }
  }

  if( group_opt )
  {
       oytmplen = oyMemBlockExtent_(&mem, oytmplen, 160);
    oySprintf_( &mem[oyStrlen_(mem)], "</%s>\n\n\n", group_opt->config_string_xml);
  }

  DBG_PROG_ENDE
  return mem;
}
                          

oyChar*
oyWriteOptionToXML_(oyGROUP_e           group,
                    oyWIDGET_e          start,
                    oyWIDGET_e          end, 
                    char             *mem,
                    int               oytmplen)
{
  int   i = 0;
  const char  * key = 0;
  const oyOption_t_ * opt = 0;
  int n = 0;
  oyWIDGET_e *tmp = NULL;
  DBG_PROG_START

         /* allocate new mem if needed */
         oytmplen = oyMemBlockExtent_(&mem, oytmplen, 360);
         opt = oyOptionGet_( (oyWIDGET_e)group );
         oySprintf_( &mem[strlen(mem)], "<%s>\n",
                  opt->config_string_xml );
         oySprintf_( &mem[strlen(mem)], "<!-- %s \n"
                                     "     %s -->\n\n",
                  opt->name,
                  opt->description );

  tmp = oyWidgetListGet_( group, &n, oyAllocateFunc_ );

       for(i = 0; i < n; ++i)
       {
         int wt = tmp[i];
         if(start <= wt && wt <= end)
         {
           char *value = 0;
           int opt_type = oyWidgetTypeGet_( wt );
           int j;
           const oyOption_t_ *t = oyOptionGet_( wt );
           int n = t->choices;
           int group_level = t->category[0];
           char intent[24] = {""};
 
           for( j = 0; j < group_level; ++j )
             oySprintf_( &intent[strlen(intent)], "  " );
 
           if( (opt_type == oyWIDGETTYPE_BEHAVIOUR) ||
               (opt_type == oyWIDGETTYPE_DEFAULT_PROFILE))
           {
             key = t->config_string_xml;
             /* allocate new mem if needed */
             oytmplen = oyMemBlockExtent_(&mem, oytmplen, 256 + 12+2*strlen(key)+8);
             /* write a short description */
             oySprintf_( &mem[strlen(mem)], "%s<!-- %s\n", intent,
                       t->name );
             oySprintf_( &mem[strlen(mem)], "%s     %s\n", intent,
                       t->description);
             /* write the profile name */
             if(opt_type == oyWIDGETTYPE_DEFAULT_PROFILE)
             {
               value = oyGetDefaultProfileName_( wt, oyAllocateFunc_ );
               if( value && strlen( value ) )
               {
                 key = t->config_string_xml;
                 /* allocate new mem if needed */
                 oytmplen = oyMemBlockExtent_(&mem, oytmplen,
                                              strlen(value) + 2*strlen(key) + 8 );
                 DBG_PROG3_S("pos: %d + %d oytmplen: %d\n",
                             (int)strlen(mem),(int)strlen(value),oytmplen);
                 oySprintf_( &mem[strlen(mem)-1], " -->\n");
 
                 /* append xml keys and value */
                 oySprintf_( &mem[strlen(mem)], "%s<%s>%s</%s>\n\n", intent,
                          key, value, key);
                 DBG_PROG1_S( "%s\n", mem);
               } else
                 oySprintf_( &mem[strlen(mem)-1], " -->\n" );
               if(value) oyFree_m_(value);
             }
             else if( opt_type == oyWIDGETTYPE_BEHAVIOUR ) 
             {
               int val = oyGetBehaviour_( wt );
               /* write a per choice description */
               for( j = 0; j < n; ++j )
                 oySprintf_( &mem[strlen(mem)], "%s %d %s\n", intent, j,
                          t->choice_list[j] );
               oySprintf_( &mem[strlen(mem)-1], " -->\n");
               /* write the key value */
               oySprintf_( &mem[strlen(mem)], "%s<%s>%d</%s>\n\n", intent,
                        key, val, key );
             }
           }
         }
       }
       oytmplen = oyMemBlockExtent_(&mem, oytmplen, 160);
       oySprintf_( &mem[strlen(mem)], "</%s>\n\n\n", opt->config_string_xml );


  DBG_PROG_ENDE
  return mem;
}

char*
oyPolicyToXML_  (oyGROUP_e           group,
                 int               add_header,
                 oyAlloc_f         allocate_func)
{
# define OYTMPLEN_ 80 /*/ TODO handle memory in more a secure way */
  /* allocate memory */
  int   oytmplen = OYTMPLEN_;
  char *mem = NULL;
  int   i = 0;

  DBG_PROG_START

  oyAllocHelper_m_( mem, char, oytmplen, oyAllocateFunc_, return NULL );

  /* initialise */
  oyOptionGet_( oyWIDGET_BEHAVIOUR_START );
  mem[0] = 0;

  /* create a XML structure and store there the keys for exporting */
  if( add_header )
  {
         char head[] = { OY_POLICY_HEADER }; 
         oytmplen = oyMemBlockExtent_( &mem, oytmplen, strlen(head)+24 );

         oySprintf_( mem, "%s\n<body>\n\n\n", head );
  }


  /* which group is to save ? */
  switch (group)
  { case oyGROUP_DEFAULT_PROFILES:
         mem = oyWriteOptionToXML_( group,
                                    oyWIDGET_DEFAULT_PROFILE_START + 1,
                                    oyWIDGET_DEFAULT_PROFILE_END - 1,
                                    mem, oytmplen );
         break;
    case oyGROUP_BEHAVIOUR_RENDERING:
         mem = oyWriteOptionToXML_( group,
                                    oyWIDGET_RENDERING_INTENT,
                                    oyWIDGET_RENDERING_HIGH_PRECISSION,
                                    mem, oytmplen );
         break;
    case oyGROUP_BEHAVIOUR_PROOF:
         mem = oyWriteOptionToXML_( group,
                                    oyWIDGET_RENDERING_INTENT_PROOF,
                                    oyWIDGET_BEHAVIOUR_END - 1,
                                    mem, oytmplen );
         break;
    case oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS:
         mem = oyWriteOptionToXML_( group,
                                    oyWIDGET_MIXED_MOD_DOCUMENTS_PRINT,
                                    oyWIDGET_MIXED_MOD_DOCUMENTS_SCREEN,
                                    mem, oytmplen );
         break;
    case oyGROUP_BEHAVIOUR_MISSMATCH:
         mem = oyWriteOptionToXML_( group,
                                    oyWIDGET_ACTION_UNTAGGED_ASSIGN,
                                    oyWIDGET_ACTION_OPEN_MISMATCH_CMYK,
                                    mem, oytmplen );
         break;
    case oyGROUP_ALL:

         /* travel through the group of settings and call the func itself */
         for(i = oyGROUP_START + 1; i < oyGROUP_ALL; ++i)
         { int   pos = strlen(mem);
           char *value = oyPolicyToXML_(i, 0, oyAllocateFunc_);
           if(value)
           {
             /* allocate new mem if needed */
             oytmplen = oyMemBlockExtent_(&mem, oytmplen, strlen(value)+1);

             oySprintf_(&mem[pos], "%s", value);
             oyFree_m_(value);
           }
         }
         break;
    default:
         /* error */
         /*oytmplen = oyMemBlockExtent_(&mem, oytmplen, 48);
         oySprintf_( mem, "<!-- Group: %d does not exist -->", group );*/
         break;
  }

  if( add_header )
  {
         const char *end = "\n</body>\n";
         int   pos = strlen(mem);

         oytmplen = oyMemBlockExtent_( &mem, oytmplen, strlen( end )+1 );
         oySprintf_( &mem[pos], "%s", end );
  }

  { int len = strlen( mem );
    char *tmp = NULL;

    if(len)
    {
      oyAllocHelper_m_( tmp, char, len + 1, allocate_func, return NULL );
      memcpy( tmp, mem, len + 1 );
      oyFree_m_( mem );
    }
    mem = tmp;
  }
  DBG_PROG_ENDE
  return mem;
}


int
oyReadXMLPolicy_(oyGROUP_e           group,
                 const char      * xml)
{
  /* allocate memory */
  const char *key = 0;
  char *value = 0;
  int   i = 0;
  int   err = 0;
  int n = 0;
  oyWIDGET_e * list = NULL;

  DBG_PROG_START

  list = oyPolicyWidgetListGet_( oyGROUP_ALL, &n );
  for(i = 0; i < n; ++i)
  {
    oyWIDGET_e oywid = list[i];
    oyWIDGET_TYPE_e opt_type = oyWidgetTypeGet_( oywid );

    if(opt_type == oyWIDGETTYPE_DEFAULT_PROFILE)
    {
      const oyOption_t_ *t = oyOptionGet_( oywid );
      key = t->config_string_xml;

      /* read the value for the key */
      value = oyXMLgetValue_(xml, key);

      /* set the key */
      if(value && strlen(value))
      {
        err = oySetDefaultProfile_( (oyPROFILE_e)oywid, value);
        if(err)
        {
          WARNc2_S( "Could not set default profile %s:%s", t->name ,
                    value?value:"--" );
        }
        oyFree_m_(value);
      }
    } else if(opt_type == oyWIDGETTYPE_BEHAVIOUR)
    {
      const oyOption_t_ *t = oyOptionGet_( oywid );
      int val = -1;

      key = t->config_string_xml;

      /* read the value for the key */
      value = oyXMLgetValue_(xml, key);

      /* convert value from string to int */
      if(value)
        val = atoi(value);

      /* set the key */
      if( val != -1 && value )
        err = oySetBehaviour_((oyBEHAVIOUR_e)oywid, val);

      if(err)
        {
          WARNc2_S( "Could not set behaviour %s:%s .", t->name ,
                    value?value:"--" );
          return err;
        }

      if(value) oyFree_m_(value);
    }
  }

  if( list ) oyFree_m_( list );

  DBG_PROG_ENDE
  return err;
}









/**
 *  Function oyXFORMsRenderUi
 *  @brief   render the UI by a selected toolkit
 *
 *  Parse the XFORMS XML and render with the selected toolkit UI handlers.
 *
 *  @param[in]     xforms              XFORMS text
 *  @param[in]     ui_handlers         the toolkit specific XFORMS handlers
 *  @param[in,out] user_data           user context passed to ui_handlers
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/31 (Oyranos: 0.1.10)
 *  @date    2009/08/31
 */
int          oyXFORMsRenderUi        ( const char        * xforms,
                                       oyUiHandler_s    ** ui_handlers,
                                       oyPointer           user_data )
{
  int error = !xforms || !ui_handlers;
  xmlDocPtr doc = 0;
  xmlNodePtr cur = 0;
  const char * text = xforms;

  if(error)
    return error;

  doc = xmlParseMemory( text, strlen(text) );
  cur = xmlDocGetRootElement(doc);

  if(doc && cur)
  {
    oyParseXMLDoc_( doc, cur, ui_handlers, user_data );
    xmlFreeDoc( doc );
  }
  else
    error = 1;

  return error;
}

/**
 *  @brief   default CSS
 *
 *  The short stylesheet should cover the Oyranos subset of XFORMS elements.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/04 (Oyranos: 0.1.10)
 *  @date    2009/10/04
 */
#define OY_XFORMS_CSS "\
  <style type=\"text/css\"> \n\
  @namespace xf url(\"http://www.w3.org/2002/xforms\");\n\
  xf|label {\n\
   font-family: Helvetica, Geneva, Lucida, sans-serif;\n\
   width: 24ex;\n\
   text-align: right;\n\
   padding-right: 1em;\n\
  }\n\
  xf|select1 { display: table-row; }\n\
  xf|select1 xf|label, xf|choices xf|label  { display: table-cell; }\n\
  </style>"

/**
 *  Function oyXFORMsFromModelAndUi
 *  @brief   join data and UI
 *
 *  @param[in]     data                data text
 *  @param[in]     ui_text             ui text
 *  @param[in]     namespaces          url style namespaces
 *  @param[in]     head_injection      html text to include in the HEAD section
 *                                     With a NULL pointer Oyranos will implicit
 *                                     insert a default CSS.
 *  @param[in]     allocate_func       user allocator
 *  @return                            XFORMS text
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/31 (Oyranos: 0.1.10)
 *  @date    2009/08/31
 */
char *       oyXFORMsFromModelAndUi  ( const char        * data,
                                       const char        * ui_text,
                                       const char       ** namespaces,
                                       const char        * head_injection,
                                       oyAlloc_f           allocate_func )
{
  char * text = 0;
  int error = !data || !ui_text,
      pos = 0;

#if USE_GETTEXT
  const char * save_locale = setlocale(LC_NUMERIC, 0 );
  char * tmp = 0;
  int i = 0;
#endif

  if(error)
    return 0;

  STRING_ADD( text,
   "<?xml version=\"1.0\" encoding=\"" );
#if USE_GETTEXT
  if(save_locale && oyStrcmp_(save_locale,"C") != 0 &&
     oyStrchr_(save_locale,'.'))
  {
    STRING_ADD( tmp, oyStrchr_( save_locale, '.' ) + 1 );
    while(tmp[i])
    {
      tmp[i] = tolower(tmp[i]);
      ++i;
    }
    STRING_ADD( text, tmp );
  } else
    STRING_ADD( text, oy_domain_codeset ? oy_domain_codeset :
                                          "UTF-8" /*"ISO-8859-1"*/ );
#else
  STRING_ADD( text,
   "UTF-8" );
#endif
  STRING_ADD( text,
   "\" ?>\n"
   "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\"\n"
   "       \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n"
   "<html xmlns=\"http://www.w3.org/1999/xhtml\"\n"
   "      xmlns:xf=\"http://www.w3.org/2002/xforms\"\n" );
  while(namespaces && namespaces[pos])
  {
    STRING_ADD( text, "      xmlns:" );
    STRING_ADD( text, namespaces[pos] );
    ++pos;
  }
  STRING_ADD( text,
   ">\n"
   "<head>\n");
  STRING_ADD( text,
   "  <title>Filter options</title>\n"
   "  <xf:model>\n"
   "    <xf:instance xmlns=\"\">\n" );
  STRING_ADD( text, data );
  STRING_ADD( text,
   "\n"
   "    </xf:instance>\n"
   "  </xf:model>\n" );
  if(head_injection)
    STRING_ADD( text, head_injection );
  else
    STRING_ADD( text, OY_XFORMS_CSS );
  STRING_ADD( text,
   "</head>\n"
   "<body>\n" );
  STRING_ADD( text, ui_text );
  STRING_ADD( text,
   "\n"
   "</body></html>" );

  if(allocate_func && allocate_func != oyAllocateFunc_)
  {
    char * t = oyStringCopy_( text, allocate_func );
    oyDeAllocateFunc_(text);
    text = t; t = 0;
  }

  return text;
}


/** @internal
 *  Function oyXML2NodeName
 *  @brief   join namespace and node name
 *
 *  @param[in]     pattern             libxml2 node
 *  @return                            ns + ':' + name
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/28 (Oyranos: 0.1.10)
 *  @date    2009/08/28
 */
char *             oyXML2NodeName    ( xmlNodePtr          cur )
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

/** Function oyXMLNodeNameIs
 *  @brief   string compare with a joined namespace and node name
 *
 *  @return                            boolean
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/12 (Oyranos: 0.1.10)
 *  @date    2009/11/12
 */
int                oyXMLNodeNameIs   ( xmlNodePtr          cur,
                                       const char        * node_name )
{
  int found = 0;
  char * name = 0;

  if(cur && cur->type == XML_ELEMENT_NODE)
    name = oyXML2NodeName( cur );

  if(name && node_name &&
     oyStrcmp_(name, node_name) == 0)
    found = 1;

  if(name)
    oyFree_m_( name );

  return found;
}

/** @internal
 *  Function oyXML2PathName_
 *  @brief   tell about the nodes position in a path style
 *
 *  @param[in]     cur                 libxml2 node
 *  @return                            level only path + key
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/09 (Oyranos: 0.1.10)
 *  @date    2009/11/09
 */
char *             oyXML2PathName_   ( xmlNodePtr          cur )
{
  char * path = 0;
  char * t = 0,
       * t2 = 0;
  int len = 0;

  path = (char*) xmlGetNodePath( cur );
  while((t = oyStrchr_( path, '[' )) != 0)
  {
    t2 = oyStrchr_( path, ']' );
    len = t2 + 1 - t;
    memmove( t, t2+1, oyStrlen_(t2+1) );
    path[oyStrlen_(path)-len] = 0;
  };
  return path;
}



void               oyParseXMLDoc_    ( xmlDocPtr           doc,
                                       xmlNodePtr          cur,
                                       oyUiHandler_s    ** ui_handlers,
                                       oyPointer           ui_handlers_context )
{
  while(cur != NULL)
  {
    char * name = 0;
    char * tmp = 0,
         * t = 0;
    int pos = 0;
    oyOptions_s * wid_data = 0;

    if(cur->type == XML_ELEMENT_NODE)
    {
      name = oyXML2NodeName(cur);
      if(oy_debug)
        printf(" name: %s\n", name);

      /* search a entry node */
      {

        while(ui_handlers[pos])
        {
          int pos2 = 0;
          STRING_ADD( tmp, ui_handlers[pos]->element_searches[pos2] );
          t = oyStrchr_( tmp, '/' );
          if(t)
            t[0] = 0;

          if(oyStrcmp_( tmp, name ) == 0)
            /* render */
            ui_handlers[pos]->handler( cur, wid_data, ui_handlers_context );

          oyFree_m_( tmp );

          ++pos;
        }
      }
    }

    if(cur->xmlChildrenNode)
      oyParseXMLDoc_( doc, cur->xmlChildrenNode,
                      ui_handlers, ui_handlers_context );

    if(name)
      oyFree_m_( name );

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

    STRING_ADD( xpath, reference );
    result = xmlXPathEvalExpression( (xmlChar*)xpath, context );

    if( result && !xmlXPathNodeSetIsEmpty( result->nodesetval ) &&
        result->nodesetval &&
        result->nodesetval->nodeTab && result->nodesetval->nodeTab[0] &&
        result->nodesetval->nodeTab[0]->children &&
        oyStrcmp_((char*)result->nodesetval->nodeTab[0]->children->name, "text") == 0 &&
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

/** Function oyXFORMsModelGetXPathValue
 *  @brief   get the xforms model value of a corresponding layout node
 *
 *  @param[in]     cur                 the libxml2 node
 *  @param[in]     attr_name           the nodes attribute, only "ref" ?
 *  @param[out]    xpath               the xpath of the referenced key
 *  @return                            the attributes value
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/11 (Oyranos: 0.1.10)
 *  @date    2009/11/11
 */
const char *       oyXFORMsModelGetXPathValue (
                                       xmlNodePtr          cur,
                                       const char        * attr_name,
                                       const char       ** xpath )
{
  const char * v = 0;
  xmlAttrPtr attr = 0;

  /* search a entry node */
  attr = cur->properties;
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

    if( oyStrcmp_((char*)attr->name, attr_name) == 0 &&
        attr->children->content )
    {
      v = oyXFORMsModelGetXPathValue_( cur->doc,(char*)attr->children->content);
      if(v && oy_debug)
         printf( "Found: %s=\"%s\"\n", attr->children->content, v );
      if(xpath)
        *xpath = (const char*) attr->children->content;
    }

    attr = attr->next;
  }
  return v;
}

/** Function oyXML2NodeValue
 *  @brief   get a node childs text value
 *
 *  @param[in]     cur                 the libxml2 node
 *  @return                            the childs text value
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/11 (Oyranos: 0.1.10)
 *  @date    2009/11/11
 */
const char *       oyXML2NodeValue   ( xmlNodePtr          cur )
{
  const char * v = 0;
  xmlChar *val = 0;

  if(cur->type == XML_ELEMENT_NODE)
  {
    {
      if(cur->children && cur->children->type == XML_TEXT_NODE &&
         cur->children->content && cur->children->content[0] &&
         cur->children->content[0] != '\n')
      {
        val = xmlNodeListGetString(cur->doc, cur->children, 1);
        if(oy_debug)
          printf("  val: %s\n", val);

        if(val)
          v = (char*)val;
      }
    }
  }

  return v;
}

