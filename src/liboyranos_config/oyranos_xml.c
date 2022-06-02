/** @file oyranos_xml.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2006-2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    xml handling
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2006/02/10
 */


#include <stddef.h>           /* size_t ptrdiff_t */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_alpha.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_json.h"
#include "oyranos_string.h"

#include <oyjl_version.h>

#ifdef OYJL_HAVE_LIBXML2
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#endif


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

#ifdef OYJL_HAVE_LIBXML2
void               oyParseXMLDoc_    ( xmlDocPtr           doc,
                                       xmlNodePtr          cur,
                                       oyUiHandler_s    ** ui_handlers,
                                       oyPointer           ui_handlers_context);
#endif
void               oyParseJsonDoc_   ( oyjl_val            root,
                                       oyjl_val            val,
                                       oyUiHandler_s    ** ui_handlers,
                                       oyPointer           ui_handlers_context);

/* miscellaneous */

/* small helpers */

/* --- function definitions --- */



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
        return 0;
      }
    }
  }
  len = value2 - len2 - value1;
  oyFree_m_(key1); oyFree_m_(key2);
# endif
  if(len > 0 && (ptrdiff_t)value1 - len1 > 0)
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
  ptrdiff_t l = 0;
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
  ptrdiff_t l = 0;
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
    for( j = 0; pices && j < pices_n; ++j )
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
char*
oyWriteOptionsToXML_    ( oyOption_t_ ** opts,
                          char        ** values OY_UNUSED,
                          int            n,
                          char         * text )
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
           oyStringAddPrintf_( &text,  oyAllocateFunc_, oyDeAllocateFunc_,
                                             "<%s>\n",
                               opt->config_string_xml );
           oyStringAddPrintf_( &text,  oyAllocateFunc_, oyDeAllocateFunc_,
                                             "<!-- %s \n"
                                             "     %s -->\n\n",
                       opt->name,
                       opt->tooltip );
           oyStringAddPrintf_( &text,  oyAllocateFunc_, oyDeAllocateFunc_,
                                             "<!-- %s \n"
                                             "     %s -->\n\n",
                       opt->name,
                       opt->tooltip );

           break;
      case oyWIDGETTYPE_DEFAULT_PROFILE:
           break;
      case oyWIDGETTYPE_BEHAVIOUR:
           break;
    }
  }

  if( group_opt )
  {
    oyStringAddPrintf_( &text,  oyAllocateFunc_, oyDeAllocateFunc_,
                                   "</%s>\n\n\n", group_opt->config_string_xml);
  }

  DBG_PROG_ENDE
  return text;
}
                          

/**
 * write option range to text, allocating memory on demand
 *
 * @param[in]      flags               - 1: add group start
 *                                     - 2: add group end
 */
void         oyWriteOptionToXML_     ( oyGROUP_e           group,
                                       oyWIDGET_e          start,
                                       oyWIDGET_e          end, 
                                       char             ** text,
                                       uint32_t            flags )
{
  int   i = 0;
  const char  * key = 0;
  const oyOption_t_ * opt = 0;
  int n = 0;
  oyWIDGET_e *tmp = NULL;
  DBG_PROG_START

  
         opt = oyOptionGet_( (oyWIDGET_e)group );

  if(flags&1)
  {
         oyStringAddPrintf_( text,  oyAllocateFunc_, oyDeAllocateFunc_,
                                        "<%s>\n",
                  opt->config_string_xml );
         oyStringAddPrintf_( text,  oyAllocateFunc_, oyDeAllocateFunc_,
                                        "<!-- %s \n"
                                     "     %s -->\n\n",
                  opt->name,
                  opt->tooltip );
  }

  tmp = oyWidgetListGet_( group, &n, oyAllocateFunc_ );

       for(i = 0; i < n; ++i)
       {
         int wt = tmp[i];
         if(start <= (size_t)wt && (size_t)wt <= end)
         {
           char *value = 0;
           int opt_type = oyWidgetTypeGet_( wt );
           int j;
           const oyOption_t_ *t = oyOptionGet_( wt );
           int n = t->choices;
           int group_level = t->category[0];
           char indent[24] = {""};
 
           for( j = 0; j < group_level; ++j )
             oySprintf_( &indent[strlen(indent)], "  " );
 
           if( (opt_type == oyWIDGETTYPE_BEHAVIOUR) ||
               (opt_type == oyWIDGETTYPE_DEFAULT_PROFILE))
           {
             key = t->config_string_xml;
             /* write a short description */
             oyStringAddPrintf_( text,  oyAllocateFunc_, oyDeAllocateFunc_,
                                            "%s<!-- %s\n", indent,
                       t->name );
             oyStringAddPrintf_( text,  oyAllocateFunc_, oyDeAllocateFunc_,
                                            "%s     %s\n", indent,
                       t->tooltip);
             /* write the profile name */
             if(opt_type == oyWIDGETTYPE_DEFAULT_PROFILE)
             {
               value = oyGetDefaultProfileName_( wt, oyAllocateFunc_ );
               if( value )
               {
                 key = t->config_string_xml;
                 /* end the coment */
                 (*text)[strlen(*text)-1] = '\000';
                 oyStringAdd_( text, " -->\n", oyAllocateFunc_,oyDeAllocateFunc_);
 
                 /* append xml keys and value */
                 oyStringAddPrintf_( text,  oyAllocateFunc_, oyDeAllocateFunc_,
                                                "%s<%s>%s</%s>\n\n", indent,
                          key, value, key);
               } else
                 oySprintf_( text[strlen(*text)-1], " -->\n" );
               if(value) oyFree_m_(value);
             }
             else if( opt_type == oyWIDGETTYPE_BEHAVIOUR ) 
             {
               int val = oyGetBehaviour_( wt );
               /* write a per choice description */
               for( j = 0; j < n; ++j )
                 oyStringAddPrintf_( text,  oyAllocateFunc_, oyDeAllocateFunc_,
                                     "%s %d %s\n", indent, j,
                          t->choice_list[j] );
               /* end the coment */
               (*text)[strlen(*text)-1] = '\000';
               oyStringAdd_( text, " -->\n", oyAllocateFunc_,oyDeAllocateFunc_);
               /* write the key value */
               oyStringAddPrintf_( text,  oyAllocateFunc_, oyDeAllocateFunc_,
                                              "%s<%s>%d</%s>\n\n", indent,
                        key, val, key );
             }
           }
         }
       }
  if(flags&2)
  {
       oyStringAddPrintf_( text,  oyAllocateFunc_, oyDeAllocateFunc_,
                                      "</%s>\n\n\n", opt->config_string_xml );
  }

  if(tmp) oyFree_m_(tmp);

  DBG_PROG_ENDE
}

char*
oyPolicyToXML_  (oyGROUP_e           group,
                 int               add_xml_header,
                 oyAlloc_f         allocate_func)
{
  /* allocate memory */
  char * text = NULL;
  int   i = 0;

  DBG_PROG_START

  /* initialise */
  oyOptionGet_( oyWIDGET_BEHAVIOUR_START );

  /* create a XML structure and store there the keys for exporting */
  if( add_xml_header )
  {
         char head[] = { OY_POLICY_HEADER }; 
         oyStringAddPrintf_( &text,  oyAllocateFunc_, oyDeAllocateFunc_,
                          "%s\n<body>\n\n\n", head );
  }


  /* which group is to save ? */
  switch (group)
  { case oyGROUP_DEFAULT_PROFILES:
         oyWriteOptionToXML_( group,
                                    oyWIDGET_DEFAULT_PROFILE_START + 1,
                                    oyWIDGET_DEFAULT_PROFILE_END - 1,
                                    &text, 3 );
         break;
    case oyGROUP_BEHAVIOUR_RENDERING:
         oyWriteOptionToXML_( group,
                                    oyWIDGET_RENDERING_INTENT,
                                    oyWIDGET_BEHAVIOUR_END,
                                    &text, 3 );
         break;
    case oyGROUP_BEHAVIOUR_PROOF:
         oyWriteOptionToXML_( group,
                                    oyWIDGET_DEFAULT_PROFILE_START + 1,
                                    oyWIDGET_DEFAULT_PROFILE_END - 1,
                                    &text, 1 );
         oyWriteOptionToXML_( group,
                                    oyWIDGET_RENDERING_INTENT_PROOF,
                                    /* skip the two display settings */
                                    oyWIDGET_BEHAVIOUR_END - 3,
                                    &text, 2 );
         break;
    case oyGROUP_BEHAVIOUR_EFFECT:
         oyWriteOptionToXML_( group,
                                    oyWIDGET_DEFAULT_PROFILE_START + 1,
                                    oyWIDGET_DEFAULT_PROFILE_END - 1,
                                    &text, 1 );
         oyWriteOptionToXML_( group,
                                    oyWIDGET_RENDERING_INTENT_PROOF,
                                    oyWIDGET_BEHAVIOUR_END - 1,
                                    &text, 2 );
         break;
    case oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS:
         oyWriteOptionToXML_( group,
                                    oyWIDGET_MIXED_MOD_DOCUMENTS_PRINT,
                                    oyWIDGET_MIXED_MOD_DOCUMENTS_SCREEN,
                                    &text, 3 );
         break;
    case oyGROUP_BEHAVIOUR_MISSMATCH:
         oyWriteOptionToXML_( group,
                                    oyWIDGET_ACTION_UNTAGGED_ASSIGN,
                                    oyWIDGET_ACTION_OPEN_MISMATCH_CMYK,
                                    &text, 3 );
         break;
    case oyGROUP_ALL:

         /* travel through the group of settings and call the func itself */
         for(i = oyGROUP_START + 1; i < oyGROUP_ALL; ++i)
         {
           char *value = oyPolicyToXML_(i, 0, oyAllocateFunc_);
           if(value)
           {
             oyStringAdd_( &text, value, oyAllocateFunc_, oyDeAllocateFunc_ );
             oyFree_m_(value);
           }
         }
         break;
    default:
         /* error */
         /*oytmplen = oyTextExtent_(&mem, oytmplen, 48);
         oySprintf_( mem, "<!-- Group: %d does not exist -->", group );*/
         break;
  }

  if( add_xml_header )
  {
         const char *end = "\n</body>\n";
         oyStringAdd_( &text, end, oyAllocateFunc_, oyDeAllocateFunc_ );
         
  }

  if(text)
  { int len = strlen( text );
    char *tmp = NULL;

    if(len)
    {
      oyAllocHelper_m_( tmp, char, len + 1, allocate_func, return NULL );
      memcpy( tmp, text, len + 1 );
      oyFree_m_( text );
    }
    text = tmp;
  }
  DBG_PROG_ENDE
  return text;
}


int oyReadXMLPolicy_(oyGROUP_e           group OY_UNUSED,
                 const char      * xml)
{
  /* allocate memory */
  const char *key = 0;
  char *value = 0;
  int   i = 0;
  int   err = 0;
  int n = 0;
  oyWIDGET_e * list = NULL;
  oySCOPE_e scope = oySCOPE_USER;

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
        err = oySetDefaultProfile_( (oyPROFILE_e)oywid, scope, value);
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
        err = oySetBehaviour_((oyBEHAVIOUR_e)oywid, scope, val);

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
 *  Function oyGetDocHeader
 *  @brief   write a page header
 *
 *  @param         options             zero terminated paired key/value strings
 *  @return                            the string
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/04/05
 *  @since   2011/08/09 (Oyranos: 0.3.2)
 */
char * oyGetDocHeader                ( const char       ** options )
{
  char * text = NULL;

  const char * SF = "#cccccc";
         /* HTML */
  const char * title = _("About Oyranos"),
             * version = NULL;
  int i = 0,
      format_type = 1;

  while(options && options[i])
  {
    if( strcmp( options[i], "bgcolor" ) == 0 )
    { if(options[i+1])
       SF = options[i+1];
    }
    if( strcmp( options[i], "title" ) == 0 )
    { if(options[i+1])
       title = options[i+1];
    }
    if( strcmp( options[i], "format" ) == 0 && options[i+1])
      format_type = atoi(options[i+1]);
    if( strcmp( options[i], "version" ) == 0 && options[i+1])
      version = options[i+1];

    if(options[i+1]) i += 2;
    else i++;
  }

  if(!version)
    version = oyVersionString(1);

  if(format_type == 1) /* HTML */
  {
    oyStringAddPrintf_( &text,  oyAllocateFunc_, oyDeAllocateFunc_,
    "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">"
    "<meta http-equiv=\"Content-Type\" content=\"text/html\">\n"
    "<html>\n<head>\n"
    "<title>%s</title>\n"
    "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n"
    "<meta name=\"description\" content=\"Oyranos ICC compatible color management system\">\n"
    "<meta name=\"author\" content=\"automatic generated by Oyranos-%s\">\n"
    "</head>\n<body bgcolor=\"%s\" text=\"#000000\" link=\"blue\">\n\n",
    oyNoEmptyString_m_(title),
    version,
    SF );
  } else if(format_type == 2) /* DocBook */
  {
    oyStringAddPrintf_( &text,  oyAllocateFunc_, oyDeAllocateFunc_,
    "<?xml version=\"1.0\" ?>\n"
    "<!DOCTYPE article PUBLIC \"-//KDE//DTD DocBook XML V4.5-Based Variant V1.1//EN\" \"dtd/kdedbx45.dtd\" [\n"
    "<!ENTITY title \"<application>%s</application>\">\n"
    "<!ENTITY version \"%s\">\n"
    "<!ENTITY %% addindex \"IGNORE\">\n"
    "<!ENTITY %% English \"INCLUDE\">\n"
    "]>",
    oyNoEmptyString_m_(title),
    version/*,
    SF*/ );
  }

  return text;
}

void   oyGetDocTitle                 ( const char       ** options,
                                       char             ** html )
{
  int i = 0;
  int add_oyranos_copyright = 0,
      add_oyranos_title = 0,
      format_type = 1;
  const char * SF = "#cccccc",
             * title = NULL,
             * copyright = NULL;
  char * oyranos_title = NULL,
       * oyranos_copyright = NULL;

  while(options && options[i])
  {
    if( strcmp( options[i], "bgcolor" ) == 0 )
    { if(options[i+1])
       SF = options[i+1];
    }
    if( strcmp( options[i], "format" ) == 0 && options[i+1])
      format_type = atoi(options[i+1]);
    if( strcmp( options[i], "add_oyranos_title" ) == 0 && options[i+1])
      add_oyranos_title = atoi(options[i+1]);
    if( strcmp( options[i], "title" ) == 0 && options[i+1])
      title = oyStringCopy_(options[i+1],0);
    if( strcmp( options[i], "add_oyranos_copyright" ) == 0 && options[i+1])
      add_oyranos_copyright = atoi(options[i+1]);
    if( strcmp( options[i], "copyright" ) == 0 && options[i+1])
      copyright = oyStringCopy_(options[i+1],0);

    if(options[i+1]) i += 2;
    else i++;
  }

  if(format_type == 1) /* HTML */
  {
    if( add_oyranos_title )
    {
         oyStringAddPrintf_( &oyranos_title, oyAllocateFunc_, oyDeAllocateFunc_,
                             "%s %s",
         /* HTML */
                             _("Oyranos Version"), OYRANOS_VERSION_NAME );
         title = oyranos_title;
    }
    if( add_oyranos_copyright )
    {
         /* HTML */
         oyStringAddPrintf_( &oyranos_copyright,
                             oyAllocateFunc_, oyDeAllocateFunc_,
        "%s 2004-%d Kai-Uwe Behrmann<br>2009-%d Joseph Simon<br>2009-%d Yiannis Belias",
         /* HTML */
                             _("Copyright &copy;"), OYRANOS_DEVEL_YEAR,
                             OYRANOS_DEVEL_YEAR, OYRANOS_DEVEL_YEAR );
         copyright = oyranos_copyright;
    }

    /* general informations */
      if(title || copyright)
         oyStringAddPrintf_( html,  oyAllocateFunc_, oyDeAllocateFunc_,
"<table cellspacing=3 cellpadding=4 width=\"100%%\"> \n");
      if(title)
         oyStringAddPrintf_( html,  oyAllocateFunc_, oyDeAllocateFunc_, " \
  <tr> \n\
    <td bgcolor=\"%s\"> \n\
    <center> \n\
    <h2>%s\
    </h2> \n\
    </center><br><br><br>. \n\
    </td> \n\
  </tr> \n", SF, title );
      if(copyright)
         oyStringAddPrintf_( html,  oyAllocateFunc_, oyDeAllocateFunc_, " \
  <tr> \n\
    <td bgcolor=\"#9f9f9f\"> \n\
    <b>%s\n\
    </b> \n\
  </tr> \n", copyright );
      if(title || copyright)
         oyStringAddPrintf_( html,  oyAllocateFunc_, oyDeAllocateFunc_, " \
</table> \n\
<br>" );
         oyStringAddPrintf_( html,  oyAllocateFunc_, oyDeAllocateFunc_,
" \n\
<br> \n\
<p>");
  } else if(format_type == 2)
  {
         oyStringAddPrintf_( html,  oyAllocateFunc_, oyDeAllocateFunc_, "\n\
<article id=\"oyranos_settings\" lang=\"&language;\"> \n\
    <articleinfo> \n\
    <title>%s</title> \n\
 \n\
    <authorgroup> \n\
        <author> \n\
            <firstname>Kai-Uwe</firstname> \n\
            <surname>Behrmann</surname> \n\
            <affiliation> \n\
                <address><email>ku.b@gmx.de</email></address> \n\
            </affiliation> \n\
        </author> \n\
        <!-- TRANS:ROLES_OF_TRANSLATORS --> \n\
    </authorgroup> \n\
    <copyright> \n\
        <year>%d</year> \n\
        <holder>Kai-Uwe Behrmann</holder> \n\
    </copyright> \n\
 \n\
    <date>2017-04-05</date><!-- Date of (re)writing, or update.--> \n\
    <releaseinfo>&version;</releaseinfo><!-- Application version number. Use the variable definitions within header to change this value.--> \n\
 \n\
    <abstract> \n\
        <para>%s</para> \n\
    </abstract> \n\
 \n\
    <keywordset> \n\
        <keyword>KDE</keyword> \n\
        <keyword>System Settings</keyword> \n\
        <keyword>Oyranos</keyword> \n\
        <keyword>Color Management</keyword> \n\
        <keyword>KolorManager</keyword> \n\
        <keyword>Synnefo</keyword> \n\
    </keywordset> \n\
 \n\
    </articleinfo>\n",
    _("Oyranos User Manual"),
    OYRANOS_DEVEL_YEAR,
    /* Hint for i18n: do not translate the &title; token. */
    _("This is the documentation for the &title; that configures the Oyranos Color Management System.") );
  }
}

/**
 *  Function oyWriteOptionToHTML_
 *  @brief   write a option to HTML
 *
 * write option range to text, allocating memory on demand
 *
 *  @return                            the string
 *
 *  @version Oyranos: 0.3.2
 *  @since   2011/08/13 (Oyranos: 0.3.2)
 *  @date    2011/08/13
 */
void         oyWriteOptionToHTML_    ( oyGROUP_e           group,
                                       oyWIDGET_e          start,
                                       oyWIDGET_e          end, 
                                       int                 format_type,
                                       char             ** text )
{
  int   i = 0;
  int n = 0;
  oyWIDGET_e *tmp = NULL;
  DBG_PROG_START

  
  tmp = oyWidgetListGet_( group, &n, oyAllocateFunc_ );

       for(i = 0; i < n; ++i)
       {
         int wt = tmp[i];
         if(start <= (size_t)wt && (size_t)wt <= end)
         {
           int opt_type = oyWidgetTypeGet_( wt );
           int j;
           const oyOption_t_ *t = oyOptionGet_( wt );
           int group_level = t->category[0];
           char indent[24] = {""};
 
           for( j = 0; j < group_level; ++j )
             oySprintf_( &indent[strlen(indent)], "  " );
 
           if( (opt_type == oyWIDGETTYPE_BEHAVIOUR) ||
               (opt_type == oyWIDGETTYPE_DEFAULT_PROFILE))
           {
             const char * sect2 = "%s%s<h4>%s</h4>\n",
                        * sect2end = "",
                        * paragraph = "%s     %s\n";
             if(format_type == 2)
             {
               sect2 = "%s<variablelist><varlistentry><term>\n%s<guilabel>%s</guilabel></term> <listitem>\n";
               sect2end = "%s</listitem></varlistentry></variablelist>\n";
               paragraph = "%s<para>%s</para>\n";
             }
             /* write a short description */
             oyStringAddPrintf_( text,  oyAllocateFunc_, oyDeAllocateFunc_,
                                 sect2, indent, format_type == 2 ? indent : "",
                       t->name );
             oyStringAddPrintf_( text,  oyAllocateFunc_, oyDeAllocateFunc_,
                                 paragraph, indent,
                       t->description);
             oyStringAddPrintf_( text,  oyAllocateFunc_, oyDeAllocateFunc_,
                                 sect2end, indent );
             /* write the profile name */
           }
         }
       }

  if(tmp) oyFree_m_(tmp);

  DBG_PROG_ENDE
}

/**
 *  Function oyDescriptionToHTML
 *  @brief   write a documentation page
 *
 *  Available formats are HTML and docbook.
 *
 *  @code
    const char * opts[] = {"add_html_header","1",
                           "add_oyranos_title","1",
                           "add_oyranos_copyright","1",
                           NULL};
    char * html = oyDescriptionToHTML( oyGROUP_ALL, opts, malloc );
    @endcode
 *
 *  @param         group               currently only oyGROUP_ALL
 *  @param         options             zero terminated paired key/value strings
 *                                     - "add_html_header":"1" - adds a document header
 *                                     - "bgcolor":"#cccccc" - background color for HTML
 *                                     - "title":"myPage" - HTML page title or
 *                                       docbook application title
 *                                     - "format": "1" for HTML, "2" for docbook
 *                                     - "version": "1.0" for docbook application version
 *  @param         allocate_func       the user allocator
 *  @return                            the string
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/05/07
 *  @since   2011/08/09 (Oyranos: 0.3.2)
 */
char * oyDescriptionToHTML           ( int                 group,
                                       const char       ** options,
                                       oyAlloc_f           allocateFunc )
{
  char * html = NULL,
       * value = NULL;
  int   i = 0;
  int add_html_header = 0,
      add_oyranos_copyright = 0,
      add_oyranos_title = 0,
      format_type = 1;
  char * oyranos_title = NULL,
       * oyranos_copyright = NULL;

  const oyOption_t_ * opt = 0;
  const char * sect1 =                      "        <h3>%s</h3>\n",
             * sect1end =                   "",
             * paragraph =                  "          <p>\n",
             * paragraphend =               "</p>\n",
             * paragraph1 =                 "          %s\n",
             * break1 =                     "          </ br>\n",
             * link_color = "<a href=\"http://www.color.org\">www.color.org</a>";
  DBG_PROG_START

  while(options && options[i])
  {
    if( strcmp( options[i], "add_html_header" ) == 0 && options[i+1])
      add_html_header = atoi(options[i+1]);
    if( strcmp( options[i], "format" ) == 0 && options[i+1])
      format_type = atoi(options[i+1]);
    if( strcmp( options[i], "add_oyranos_title" ) == 0 && options[i+1])
      add_oyranos_title = atoi(options[i+1]);
    if( strcmp( options[i], "add_oyranos_copyright" ) == 0 && options[i+1])
      add_oyranos_copyright = atoi(options[i+1]);

    if(options[i+1]) i += 2;
    else i++;
  }

  if( add_html_header )
         html = oyGetDocHeader( options ); 

  /* general informations */
  if( add_oyranos_title && add_oyranos_copyright )
    oyGetDocTitle( options, &html );

  if(format_type == 2)
  {
    sect1 =                                 "        <sect1>\n"
                                            "          <title>%s</title>\n";
    sect1end =                              "        </sect1>\n";
    paragraph =                             "          <para>\n",
    paragraphend =                        "\n          </para>\n",
    paragraph1 =                            "          <para>%s</para>\n";
    break1 =                                "",
    link_color = "<ulink url=\"http://www.color.org\">www.color.org</ulink>";
  }


  opt = oyOptionGet_( (oyWIDGET_e)group );
  switch (group)
  {
    case oyGROUP_DEFAULT_PROFILES:
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             sect1,
                             opt->name );
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             paragraph1,
                             opt->description );
         oyWriteOptionToHTML_( group,
                                    oyWIDGET_DEFAULT_PROFILE_START + 1,
                                    oyWIDGET_DEFAULT_PROFILE_END - 1,
                                    format_type, &html );
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             sect1end );
         break;
    case oyGROUP_BEHAVIOUR_RENDERING:
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             sect1,
                             opt->name );
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             paragraph1,
                             opt->description );
         oyWriteOptionToHTML_( group,
                                    oyWIDGET_RENDERING_INTENT,
                                    oyWIDGET_BEHAVIOUR_END,
                                    format_type, &html );
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             sect1end );
         break;
    case oyGROUP_BEHAVIOUR_PROOF:
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             sect1,
                             opt->name );
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             paragraph1,
                             opt->description );
         oyWriteOptionToHTML_( group,
                                    oyWIDGET_DEFAULT_PROFILE_START + 1,
                                    oyWIDGET_DEFAULT_PROFILE_END - 1,
                                    format_type, &html );
         oyWriteOptionToHTML_( group,
                                    oyWIDGET_RENDERING_INTENT_PROOF,
                                    oyWIDGET_BEHAVIOUR_END - 1,
                                    format_type, &html );
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             sect1end );
         break;
    case oyGROUP_BEHAVIOUR_EFFECT:
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             sect1,
                             opt->name );
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             paragraph1,
                             opt->description );
         oyWriteOptionToHTML_( group,
                                    oyWIDGET_DEFAULT_PROFILE_START + 1,
                                    oyWIDGET_DEFAULT_PROFILE_END - 1,
                                    format_type, &html );
         oyWriteOptionToHTML_( group,
                                    oyWIDGET_RENDERING_INTENT_PROOF,
                                    oyWIDGET_BEHAVIOUR_END - 1,
                                    format_type, &html );
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             sect1end );
         break;
    case oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS:
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             sect1,
                             opt->name );
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             paragraph1,
                             opt->description );
         oyWriteOptionToHTML_( group,
                                    oyWIDGET_MIXED_MOD_DOCUMENTS_PRINT,
                                    oyWIDGET_MIXED_MOD_DOCUMENTS_SCREEN,
                                    format_type, &html );
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             sect1end );
         break;
    case oyGROUP_BEHAVIOUR_MISSMATCH:
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             sect1,
                             opt->name );
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             paragraph1,
                             opt->description );
         oyWriteOptionToHTML_( group,
                                    oyWIDGET_ACTION_UNTAGGED_ASSIGN,
                                    oyWIDGET_ACTION_OPEN_MISMATCH_CMYK,
                                    format_type, &html );
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             sect1end );
         break;
    case oyGROUP_ALL:
         {
           const char * opts[] = {"format", format_type == 1 ? "1" : "2",
                           NULL};
           value = oyDescriptionToHTML(oyGROUP_START, opts, oyAllocateFunc_);
           if(value)
           {
             oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                                   "%s", value);
             oyFree_m_(value);
           }

           /* travel through the group of settings and call the func itself */
           for(i = oyGROUP_START + 1; i < oyGROUP_ALL; ++i)
           {
             value = oyDescriptionToHTML(i, opts, oyAllocateFunc_);
             if(value)
             {
               oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                                   "%s", value);
               oyFree_m_(value);
             }
           }
         }
         break;
    case oyGROUP_START:
         /* HTML */
         if(format_type == 1)
         {
           oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_, _(
"Internet: <a href=\"http://www.oyranos.org\">www.oyranos.org</a><br>") );
           oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_, " \
</p>\n \
<p>");
         } else if(format_type == 2)
         {
           oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
"           <sect1>\n"
"             <title>%s</title>\n"
"%s",
           _("Introduction"), paragraph );
           oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_, _(
"Internet: <ulink url=\"http://www.oyranos.org\">www.oyranos.org</ulink>") );
           oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_, " \
%s\n \
%s", paragraphend, paragraph );
         }
  /* goal and means */
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_, _(
         /* HTML */
"Oyranos is a Color Management System (CMS), which relies one the ICC "
"file format standard (%s) "
"for color space definitions. "
"The use of ICC color profiles shall enable a flawless and automated "
"color data exchange between different color spaces and various "
"devices with their respective physical color behaviours."),
           link_color );
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_, " \
%s\n \
%s", paragraphend, paragraph );
  /* color conversion theory for devices */
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_, _(
         /* HTML */
"Color matching between different devices is "
"possible, provided that the color characteristics of the involved devices are "
"known. "
"The quality of a color transform for devices from one color space to an other "
"depends particularly on the quality of the color measurement and the "
"profiling algorithm used during ICC profile creation. "
"Each color space definition happens in reference to a "
"Profile Connection Spaces (PCS). The PCS is a well known color space, based "
"on the average \"human observer\" as defined by CIE.") );
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_, " \
%s\n \
%s", paragraphend, paragraph );
  /* color devices in Oyranos */
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_, _(
         /* HTML */
"Color profiles are often available from manufacturers of imaging devices "
"as digital cameras, monitors and printers. In Oyranos, ICC profiles are "
"assigned to calibration states of color devices, to get close to the device "
"behaviour as was present during ICC profile creation." ) );
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_, " \
%s\n \
%s", paragraphend, paragraph );
  /* other settings */
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_, _(
         /* HTML */
"Oyranos allowes detailed settings like preferred editing color spaces and the "
"behaviour of color conversions or simulation. "
"Oyranos reduces the work involved in all color management related decisions "
"through automation, useful defaults and grouping of settings in selectable "
"policies.") );
         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_, " \
%s%s\n", break1, paragraphend);

         oyStringAddPrintf_( &html,  oyAllocateFunc_, oyDeAllocateFunc_,
                             sect1end );
         break;
    default:
         DBG_NUM1_S("No text for group: %d",group);
         break;
  }

  if( add_html_header )
  {
         const char *end = "</body>";
         if(format_type == 2)
           end = "      </article>";
         oyStringAdd_( &html, end, oyAllocateFunc_, oyDeAllocateFunc_ );
         
  }

  if(oyranos_title)
    oyFree_m_( oyranos_title );
  if(oyranos_copyright)
    oyFree_m_( oyranos_copyright );

  if(html)
  { int len = strlen( html );
    char *tmp = NULL;

    if(len)
    {
      oyAllocHelper_m_( tmp, char, len + 1, allocateFunc, return NULL );
      memcpy( tmp, html, len + 1 );
      oyFree_m_( html );
    }
    html = tmp;
  }
  DBG_PROG_ENDE
  return html;
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
  oyjl_val root = NULL,
           value = NULL;
  int has_xml = 0;
#ifdef OYJL_HAVE_LIBXML2
  xmlDocPtr doc = NULL;
  xmlNodePtr cur = NULL;
  const char * text = xforms;

  if(error)
    return error;

  if(text[0] == '<')
  {
    doc = xmlParseMemory( text, strlen(text) );
    cur = xmlDocGetRootElement(doc);
  }

  if(doc && cur)
  {
    has_xml = 1;
    oyParseXMLDoc_( doc, cur, ui_handlers, user_data );
    xmlFreeDoc( doc );
  }
  else
#endif
  {
    root = oyJsonParse( text, NULL );
    if(root)
    {
      oyParseJsonDoc_( root, value, ui_handlers, user_data );
      oyjlTreeFree( root );
    }
  }

  if(!root && !has_xml)
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

#ifdef USE_GETTEXT
  const char * save_locale = setlocale(LC_NUMERIC, 0 );
  char * tmp = 0;
  int i = 0;
#endif

  if(error)
    return 0;

  STRING_ADD( text,
   "<?xml version=\"1.0\" encoding=\"" );
#ifdef USE_GETTEXT
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


#ifdef OYJL_HAVE_LIBXML2
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

/** Function oyXFORMsModelGetAttrValue
 *  @brief   get the xml elements attribute value
 *
 *  @param[in]     cur                 the libxml2 node
 *  @param[in]     attr_name           the nodes attribute
 *  @return                            the attributes value
 *
 *  @version Oyranos: 0.3.2
 *  @since   2011/07/31 (Oyranos: 0.3.2)
 *  @date    2011/07/31
 */
const char *       oyXFORMsModelGetAttrValue (
                                       xmlNodePtr          cur,
                                       const char        * attr_name )
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

    if( attr->children && attr->children->content &&
        oyStrcmp_((char*)attr->name, attr_name) == 0 )
    {
      v = (const char*)attr->children->content;
      if(v && oy_debug)
         printf( "Found: %s=\"%s\"\n", attr->children->content, v );
    }

    attr = attr->next;
  }
  return v;
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
  const char * attr = 0;

  /* search a entry node */
  attr = oyXFORMsModelGetAttrValue( cur, attr_name );
  if( attr )
  {
    v = oyXFORMsModelGetXPathValue_( cur->doc,(char*)attr);
    if(xpath)
      *xpath = attr;
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
#endif

void               oyParseJsonDoc_   ( oyjl_val            root,
                                       oyjl_val            value OY_UNUSED,
                                       oyUiHandler_s    ** ui_handlers,
                                       oyPointer           ui_handlers_context)
{
  int count = 0, i;
  char ** paths = oyjlTreeToPaths( root, 1000000, NULL, 0, &count );

  for(i = 0; i < count; ++i)
  {
    char * path = paths[i];
    oyOptions_s * wid_data = 0;
    int pos = 0;

    /* search a entry node */
    while(ui_handlers[pos])
    {
      int pos2 = 0;
      while(ui_handlers[pos]->element_searches[pos2])
      {
        const char * pattern = ui_handlers[pos]->element_searches[pos2];

        if(oyjlPathMatch( path, pattern, OYJL_PATH_MATCH_LAST_ITEMS ))
        {
          oyjl_val v = oyjlTreeGetValue( root, 0, path );

          /* render */
          ui_handlers[pos]->handler( v, wid_data, ui_handlers_context );
        }

        ++pos2;
      }

      ++pos;
    }
  }
}

