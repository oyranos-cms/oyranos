/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2006  Kai-Uwe Behrmann
 *
 * Autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * -----------------------------------------------------------------------------
 */

/** @file @internal
 *  @brief xml handling
 */

/* Date:      10. 02. 2006 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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

/* form for write backends */
typedef oyChar* (*oyOptionWriteFunc_t)     ( oyOption_t_ ** opts,
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
  DBG_PROG_S(("len1: %d %d %d\n",(int) strlen(*mem), (int)old_len, add));
  if( add > (old_len - strlen(*mem)) )
  {
    int len = add + strlen(*mem) + ((add > 120) ? add + 50 : 120);
    char *tmp = NULL;

    oyAllocHelper_m_( tmp, char, len, oyAllocateFunc_, return 0 );
    DBG_PROG_S(("len2: %d\n",len));
    memcpy( tmp, *mem, old_len  );
    DBG_PROG_S(("%s // %s", *mem, tmp));
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
        WARNc_S(("key: %s is not complete.", key))
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
    oySnprintf_(value, len+1, "%s", value1);
  }

  return value;
}

/* We dont get the starting point and length of the found value. */
char*
oyXMLgetField_  (const char       *xml,
                 const char       *key,
                 int              *len)
{
  const char* val_pos = 0;
  char *value1 = 0, *value2 = 0;
  intptr_t l = 0;
  int   len1 = strlen( key ) + 2,
        len2 = strlen( key ) + 3;
  char *key1 = NULL,
       *key2 = NULL;
  int   open = 0;

  oyAllocHelper_m_( key1, char, len1+1, oyAllocateFunc_, return NULL );
  oyAllocHelper_m_( key2, char, len2+1, oyAllocateFunc_, return NULL );

  *len = 0;

  sprintf(key1, "<%s>", key);
  sprintf(key2, "</%s>", key);

  if(!xml) goto clean;

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
        WARNc_S(("key: %s is not complete.", key))
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

  clean:
  oyFree_m_(key1); oyFree_m_(key2);

  *len = l;

  return value1;
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
    snprintf( ptr[n], len+1, val_pos);
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
         opt = oyOptionGet_( group );
         sprintf( &mem[strlen(mem)], "<%s>\n",
                  opt->config_string_xml );
         sprintf( &mem[strlen(mem)], "<!-- %s \n"
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
             sprintf( &intent[strlen(intent)], "  " );
 
           if( (opt_type == oyWIDGETTYPE_BEHAVIOUR) ||
               (opt_type == oyWIDGETTYPE_DEFAULT_PROFILE))
           {
             key = t->config_string_xml;
             /* allocate new mem if needed */
             oytmplen = oyMemBlockExtent_(&mem, oytmplen, 256 + 12+2*strlen(key)+8);
             /* write a short description */
             sprintf( &mem[strlen(mem)], "%s<!-- %s\n", intent,
                       t->name );
             sprintf( &mem[strlen(mem)], "%s     %s\n", intent,
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
                 DBG_PROG_S(("pos: %d + %d oytmplen: %d\n",
                             (int)strlen(mem),(int)strlen(value),oytmplen));
                 sprintf( &mem[strlen(mem)-1], " -->\n");
 
                 /* append xml keys and value */
                 sprintf( &mem[strlen(mem)], "%s<%s>%s</%s>\n\n", intent,
                          key, value, key);
                 DBG_PROG_S((mem));
               } else
                 sprintf( &mem[strlen(mem)-1], " -->\n" );
               if(value) oyFree_m_(value);
             }
             else if( opt_type == oyWIDGETTYPE_BEHAVIOUR ) 
             {
               int val = oyGetBehaviour_( wt );
               /* write a per choice description */
               for( j = 0; j < n; ++j )
                 sprintf( &mem[strlen(mem)], "%s %d %s\n", intent, j,
                          t->choice_list[j] );
               sprintf( &mem[strlen(mem)-1], " -->\n");
               /* write the key value */
               sprintf( &mem[strlen(mem)], "%s<%s>%d</%s>\n\n", intent,
                        key, val, key );
             }
           }
         }
       }
       oytmplen = oyMemBlockExtent_(&mem, oytmplen, 160);
       sprintf( &mem[strlen(mem)], "</%s>\n\n\n", opt->config_string_xml );


  DBG_PROG_ENDE
  return mem;
}

char*
oyPolicyToXML_  (oyGROUP_e           group,
                 int               add_header,
                 oyAllocFunc_t     allocate_func)
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

         sprintf( mem, "%s\n<body>\n\n\n", head );
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
                                    oyWIDGET_RENDERING_BPC,
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

             sprintf(&mem[pos], "%s", value);
             oyFree_m_(value);
           }
         }
         break;
    default:
         /* error */
         /*oytmplen = oyMemBlockExtent_(&mem, oytmplen, 48);
         sprintf( mem, "<!-- Group: %d does not exist -->", group );*/
         break;
  }

  if( add_header )
  {
         const char *end = "\n</body>\n";
         int   pos = strlen(mem);

         oytmplen = oyMemBlockExtent_( &mem, oytmplen, strlen( end )+1 );
         sprintf( &mem[pos], "%s", end );
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
        err = oySetDefaultProfile_( oywid, value);
        if(err)
        {
          WARNc_S(( "Could not set default profile %s:%s", t->name ,
                    value?value:"--" ));
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
      val = atoi(value);

      /* set the key */
      if( val != -1 && value )
        err = oySetBehaviour_(oywid, val);

      if(err)
        {
          WARNc_S(( "Could not set behaviour %s:%s .", t->name ,
                    value?value:"--" ));
          return err;
        }

      if(value) oyFree_m_(value);
    }
  }

  if( list ) oyFree_m_( list );

  DBG_PROG_ENDE
  return err;
}



