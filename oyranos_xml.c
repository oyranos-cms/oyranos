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
 *
 * xml handling
 * 
 */

/* Date:      10. 02. 2006 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_debug.h"
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
char*       oyWriteOptionToXML_(oyGROUP           group,
                    oyOPTION          start,
                    oyOPTION          end, 
                    char             *mem,
                    int               oytmplen);

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
    char *tmp = oyAllocateFunc_( len );
    DBG_PROG_S(("len2: %d\n",len));
    memcpy( tmp, *mem, old_len  );
    DBG_PROG_S(("%s // %s", *mem, tmp));
    free (*mem);
    *mem = tmp;
    new_len = len;
  }
  return new_len;
}


/* sscanf is not  useable as it ignores after an empty space sign
   We get a allocated pure value string. */
char*
oyXMLgetValue_  (const char       *xml,
                 const char       *key)
{
  const char* val_pos = 0;
  char *value1 = 0, *value2 = 0, *value = 0;
  int   len1 = strlen( key ) + 2,
        len2 = strlen( key ) + 3;
  char *key1 = calloc(sizeof(char), len1 + 1),
       *key2 = calloc(sizeof(char), len2 + 1);
  int   open = 0;

  int len = 0;
# if 0
  if(xml && key)
    value1 = strstr(xml, key);
  if(value1)
  if (value1 > xml &&
      value1[-1] == '<' &&
      value1[ strlen(key) ] == '>')
  { value2 = strstr(value1+1, key);
    if(value2)
    if (value2[ -2 ] == '<' &&
        value2[ -1 ] == '/' &&
        value2[ strlen(key) ] == '>')
    {
      val_pos = value1 + strlen(key) + 1;
      len = (int)(value2 - val_pos - 1);
      //TODO char txt[128];
      //snprintf(txt,len,val_pos);
    }
  }
# else
  sprintf(key1, "<%s>", key);
  sprintf(key2, "</%s>", key);

  val_pos = value1 = strstr( xml, key1 ) + len1;

  if(value1-len1)
    ++open;

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
        WARN_S(("key: %s is not complete.", key))
        len = 0;
        return 0;
      }
    }
  }
  len = value2 - len2 - value1;
  free(key1); free(key2);
# endif
  if(len > 0 && value1-len1 > 0)
  {
    value = calloc(sizeof(char), len+1);
    snprintf(value, len+1, value1);
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
  char *key1 = calloc(sizeof(char), len1 + 1),
       *key2 = calloc(sizeof(char), len2 + 1);
  int   open = 0;

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
        WARN_S(("key: %s is not complete.", key))
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
  free(key1); free(key2);

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
      free(values);
    values = ptr;
    oyAllocHelper_m_( ptr[n], char, len+1, oyAllocateFunc_, return NULL )
    snprintf( ptr[n], len+1, val_pos);
    val_pos += len;
    ++n;
  }
  *count = n;

  return values;
}



char*
oyWriteOptionToXML_(oyGROUP           group,
                    oyOPTION          start,
                    oyOPTION          end, 
                    char             *mem,
                    int               oytmplen)
{ DBG_PROG_START

  int   i = 0;
  const char *key = 0;
  const char **strings;

         /* allocate new mem if needed */
         oytmplen = oyMemBlockExtent_(&mem, oytmplen, 360);
         oyGroupGet_( group, &strings );
         sprintf( &mem[strlen(mem)], "<%s>\n",
                  strings[0] );
         sprintf( &mem[strlen(mem)], "<!-- %s \n"
                                     "     %s -->\n\n",
                  strings[1],
                  strings[2] );
         for(i = start; i <= end; ++i)
         {
           char *value = 0;
           int opt_type = oyGetOptionType_( i );
           int j;
           int n = oyGetOption_(i)-> choices;
           int group_level = oyGetOption_(i)-> categories[0];
           char intent[24] = {""};
 
           for( j = 0; j < group_level; ++j )
             sprintf( &intent[strlen(intent)], "  " );
 
           if( (opt_type == oyTYPE_BEHAVIOUR) ||
               (opt_type == oyTYPE_DEFAULT_PROFILE))
           {
             key = oyGetOption_(i)-> config_string_xml;
             /* allocate new mem if needed */
             oytmplen = oyMemBlockExtent_(&mem, oytmplen, 256 + 12+2*strlen(key)+8);
             /* write a short description */
             sprintf( &mem[strlen(mem)], "%s<!-- %s\n", intent,
                       oyGetOption_(i)-> label );
             sprintf( &mem[strlen(mem)], "%s     %s\n", intent,
                       oyGetOption_(i)-> description);
             /* write the profile name */
             if(opt_type == oyTYPE_DEFAULT_PROFILE)
             {
               value = oyGetDefaultProfileName_(i, oyAllocateFunc_);
               if( value && strlen( value ) )
               {
                 key = oyGetOption_(i)->
                       config_string_xml;
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
               if(value) free(value);
             }
             else if( opt_type == oyTYPE_BEHAVIOUR ) 
             {
               int val = oyGetBehaviour_(i);
               /* write a per choice description */
               for( j = 0; j < n; ++j )
                 sprintf( &mem[strlen(mem)], "%s %d %s\n", intent, j,
                          oyGetOption_(i)-> choice_list[j] );
               sprintf( &mem[strlen(mem)-1], " -->\n");
               /* write the key value */
               sprintf( &mem[strlen(mem)], "%s<%s>%d</%s>\n\n", intent,
                        key, val, key );
             }
           }
         }
         oytmplen = oyMemBlockExtent_(&mem, oytmplen, 160);
         sprintf( &mem[strlen(mem)], "</%s>\n\n\n", strings[0] );


  DBG_PROG_ENDE
  return mem;
}

char*
oyPolicyToXML_  (oyGROUP           group,
                 int               add_header,
                 oyAllocFunc_t     allocate_func)
{ DBG_PROG_START

# define OYTMPLEN_ 80 // TODO handle memory in more a secure way
  /* allocate memory */
  int   oytmplen = OYTMPLEN_;
  char *mem = oyAllocateFunc_(oytmplen);
  int   i = 0;

  /* initialise */
  oyGetOption_( oyOPTION_BEHAVIOUR_START );
  mem[0] = 0;

  /* create a XML structure and store there the keys for exporting */
  if( add_header )
  {
          char head[] = { 
          "<!--?xml version=\"1.0\" encoding=\"UTF-8\"? -->\n\
<!-- Oyranos policy format 1.0 -->\n\n\n\n" };
         oytmplen = oyMemBlockExtent_( &mem, oytmplen, strlen(head) );

         sprintf( mem, "%s", head );
  }


  /* which group is to save ? */
  switch (group)
  { case oyGROUP_DEFAULT_PROFILES:
         mem = oyWriteOptionToXML_( group,
                                    oyOPTION_DEFAULT_PROFILE_START + 1,
                                    oyOPTION_DEFAULT_PROFILE_END - 1,
                                    mem, oytmplen );
         break;
    case oyGROUP_BEHAVIOUR_RENDERING:
         mem = oyWriteOptionToXML_( group,
                                    oyOPTION_RENDERING_INTENT,
                                    oyOPTION_RENDERING_BPC,
                                    mem, oytmplen );
         break;
    case oyGROUP_BEHAVIOUR_PROOF:
         mem = oyWriteOptionToXML_( group,
                                    oyOPTION_RENDERING_INTENT_PROOF,
                                    oyOPTION_BEHAVIOUR_END - 1,
                                    mem, oytmplen );
         break;
    case oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS:
         mem = oyWriteOptionToXML_( group,
                                    oyOPTION_MIXED_MOD_DOCUMENTS_PRINT,
                                    oyOPTION_MIXED_MOD_DOCUMENTS_SCREEN,
                                    mem, oytmplen );
         break;
    case oyGROUP_BEHAVIOUR_MISSMATCH:
         mem = oyWriteOptionToXML_( group,
                                    oyOPTION_ACTION_UNTAGGED_ASSIGN,
                                    oyOPTION_ACTION_OPEN_MISMATCH_CMYK,
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
             oytmplen = oyMemBlockExtent_(&mem, oytmplen, strlen(value));

             sprintf(&mem[pos], "%s", value);
             free(value);
           }
         }
         break;
    default:
         /* error */
         /*oytmplen = oyMemBlockExtent_(&mem, oytmplen, 48);
         sprintf( mem, "<!-- Group: %d does not exist -->", group );*/
         break;
  }
  { int len = strlen( mem );
    char *tmp = allocate_func( len + 1 );
    memcpy( tmp, mem, len + 1 );
    free( mem );
    mem = tmp;
  }
  DBG_PROG_ENDE
  return mem;
}

int
oyReadXMLPolicy_(oyGROUP           group,
                 const char       *xml)
{ DBG_PROG_START

  /* allocate memory */
  const char *key = 0;
  char *value = 0;
  int   i = 0;
  int   err = 0;

  /* which group is to save ? */
  switch (group)
  { case oyGROUP_DEFAULT_PROFILES:
         for(i = oyDEFAULT_PROFILE_START + 1; i < oyDEFAULT_PROFILE_END; ++i)
         {
           key = oyGetOption_(i)-> config_string_xml;

           /* read the value for the key */
           value = oyXMLgetValue_(xml, key);

           /* set the key */
           if(value && strlen(value))
           {
             oySetDefaultProfile_(i, value);
             free(value);
           }
         }
         break;
    case oyGROUP_BEHAVIOUR_RENDERING:
         for(i = oyBEHAVIOUR_RENDERING_INTENT; i <= oyOPTION_RENDERING_BPC; ++i)
         {
           int val = -1;
           key = oyGetOption_(i)-> config_string_xml;

           /* read the value for the key */
           value = oyXMLgetValue_(xml, key);

           /* convert value from string to int */
           val = atoi(value);

           /* set the key */
           if( val != -1 && value )
             oySetBehaviour_(i, val);
           if(value) free(value);
         }
         break;
    case oyGROUP_BEHAVIOUR_PROOF:
         for(i = oyBEHAVIOUR_RENDERING_INTENT_PROOF; i < oyBEHAVIOUR_END; ++i)
         {
           int val = -1;
           key = oyGetOption_(i)-> config_string_xml;

           /* read the value for the key */
           value = oyXMLgetValue_(xml, key);

           /* convert value from string to int */
           val = atoi(value);

           /* set the key */
           if( val != -1 && value )
             oySetBehaviour_(i, val);
           if(value) free(value);
         }
         break;
    case oyGROUP_BEHAVIOUR_MIXED_MODE_DOCUMENTS:
         for(i = oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_PRINT;
               i <= oyBEHAVIOUR_MIXED_MOD_DOCUMENTS_SCREEN; ++i)
         {
           int val = -1;
           key = oyGetOption_(i)-> config_string_xml;
           value = oyXMLgetValue_(xml, key);
           val = atoi(value);
           if( val != -1 && value )
             oySetBehaviour_(i, val);
           if(value) free(value);
         }
         break;
    case oyGROUP_BEHAVIOUR_MISSMATCH:
         for(i = oyBEHAVIOUR_ACTION_UNTAGGED_ASSIGN;
               i <= oyBEHAVIOUR_ACTION_OPEN_MISMATCH_CMYK; ++i)
         {
           int val = -1;
           key = oyGetOption_(i)-> config_string_xml;
           value = oyXMLgetValue_(xml, key);
           val = atoi(value);
           if( val != -1 && value )
             oySetBehaviour_(i, val);
           if(value) free(value);
         }
         break;
    case oyGROUP_ALL:
         /* travel through the group of settings and call the func itself */
         for(i = oyGROUP_START + 1; i < oyGROUP_ALL; ++i)
           err = oyReadXMLPolicy_(i, xml);
         break;
    default:
         /* error */
         /*WARN_S(( "Group: %d does not exist", group ));*/
         break;
  }


  DBG_PROG_ENDE
  return err;
}



