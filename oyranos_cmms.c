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
 *  CMM's framework
 * 
 */

/* Date:      30. 07. 2006 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_cmms.h"
#include "oyranos_internal.h"
#include "oyranos_helper.h"
#include "oyranos_debug.h"
#include "oyranos_xml.h"

/* --- Helpers  --- */

/* --- static variables   --- */

oyCMM_t__ oyCMM_ = {0,NULL,0};

/* --- structs, typedefs, enums --- */

/* --- internal API definition --- */

/* separate from the external functions */

/* small helpers */
#define OY_FREE( ptr ) if(ptr) { free(ptr); ptr = 0; }

/* --- function definitions --- */


/* CMM support */


oyCMM_t*
oyCmmGet_        (const char *id)
{
  int i;
  if(oyCMM_.cmms && oyCMM_.n)
    for(i = 0; i < oyCMM_.n; ++i)
    {
      if(memcmp( id, oyCMM_.cmms[i].id, 4 ) == 0)
      {
        return &oyCMM_.cmms[i];
      }
    }
  return NULL;
}

int
oyCmmRemove_     (const char *id)
{
  int i,
      error = 0,
      pos = 0;
  oyCMM_t *ptr = NULL;

  oyCMM_.looked = 1;

  oyAllocHelper_m_(ptr, oyCMM_t, oyCMM_.n-1, oyAllocateFunc_, return 1)
  for(i = 0; i < oyCMM_.n; ++i)
  {
    if(strcmp(oyCMM_.cmms[i].id, id) == 0)
      ++i;
    else
      ptr[pos] = oyCMM_.cmms[i];
    ++pos;
  }

  if(oyCMM_.cmms) oyDeAllocateFunc_(oyCMM_.cmms);
  oyCMM_.cmms = ptr;
  --oyCMM_.n;
  oyCMM_.looked = 0;

  return error;
}

int
oyCmmAdd_        (oyCMM_t *cmm)
{
  int i,
      error = 0;
  oyCMM_t *ptr = NULL;

  oyCMM_.looked = 1;

  oyAllocHelper_m_(ptr, oyCMM_t, oyCMM_.n+1, oyAllocateFunc_, return 1)
  for(i = 0; i < oyCMM_.n; ++i)
    ptr[i] = oyCMM_.cmms[i];
  ptr[oyCMM_.n] = *cmm;
  if(oyCMM_.cmms) oyDeAllocateFunc_(oyCMM_.cmms);
  oyCMM_.cmms = ptr;
  ++oyCMM_.n;
  oyCMM_.looked = 0;

  return error;
}

char**
oyCmmGetCmmNames_( int        *count,
                   oyAllocFunc_t alloc_func )
{
  char **ids = NULL;
  int    i;

  *count = 0;
  oyAllocHelper_m_( ids, char*, oyCMM_.n, alloc_func, return NULL)
  WARN_S(("oyCMM_.n %d",oyCMM_.n))
  for( i = 0; i < oyCMM_.n; ++i)
  {
    oyAllocHelper_m_( ids[i], char, 5, alloc_func, return NULL);

    snprintf( ids[i], 5, oyCMM_.cmms[i].id );
  }
  *count = oyCMM_.n;
  return ids;
}

oyGROUP
oyRegisterGroups_(char *cmm, char **desc)
{
  const char ***ptr = calloc(sizeof(char***), ++oy_groups_descriptions_);
  int i;

  oyGetOption_(0);

  for(i = 0; i < oy_groups_descriptions_ - 1; ++i)
      ptr[i] = oy_groups_description_[i];
  if(oy_groups_description_)
    free(oy_groups_description_);
  i = oy_groups_descriptions_ - 1;
  ptr[i] = (const char**)desc;
  oy_groups_description_ = ptr;

  return oy_groups_descriptions_-1;
}


int
oyCmmGetFromXML_( oyGROUP           group,
                  const char       *xml,
                  const char       *domain,
                  const char       *domain_path,
                  oyCMM_t          *cmm)
{

  /* allocate memory */
  char *value = 0,
       *cmm_reg = 0,
       *cmm_group = 0,
       *groups = 0,
      **groupa,
       *options,
      **option,
       *choices,
      **choice;
  int   count = 0, count2;
  int   i, j, k;
  int   err = 0;
  int base_complete = 1;
  int first_group_n = 0;

  char *old_td = NULL, *old_bdtd = NULL;

  DBG_PROG_START


  cmm->domain = domain;
  cmm->domain_path = domain_path;
#ifdef USE_GETTEXT
  setlocale(LC_MESSAGES, "");
  WARN_S(("setlocale"))
  {
    char *bdtd = NULL;

    old_td = textdomain( NULL );
    old_bdtd = bindtextdomain( old_td, NULL );

    if(!domain)
      domain = "oyranos";

    if((old_td && (strcmp(old_td, domain) != 0)) ||
       !old_td)
    {
      bdtd = bindtextdomain (domain, domain_path ? domain_path : "");
      textdomain( domain );
    }
    DBG_PROG_S(("bdtd %s", bdtd))
  }
#endif


  cmm_reg = oyXMLgetValue_(xml, "oyCMM_REGISTER");
  cmm_group= oyXMLgetValue_(cmm_reg, "oyCMM_GROUP");
  value = oyXMLgetValue_(cmm_group, "oyID");
  if(value && strlen(value) == 4)
    snprintf( cmm->id, 5, value );
  else
    base_complete = 0;
  value = oyXMLgetValue_(cmm_group, "oyNAME");
  if(value && strlen(value))
    cmm->name = value;
  else
    base_complete = 0;
  value = _( oyXMLgetValue_(cmm_group, "oyDESCRIPTION") );
  if(value && strlen(value))
    cmm->description = value;
  groups = oyXMLgetValue_(cmm_group, "oyGROUPS");
  groupa = oyXMLgetArray_(groups, "oyGROUP", &count);

  
  oy_debug=1;
  for(i = 0; i < count; ++i)
  {
    char **props = calloc(sizeof(char*), 3);
    oyGROUP oy_group;

    props[0] = oyXMLgetValue_(groupa[i], "oyCONFIG_STRING_XML");
    props[1] = _( oyXMLgetValue_(groupa[i], "oyNAME") );
    props[2] = _( oyXMLgetValue_(groupa[i], "oyDESCRIPTION") );

    oy_group = oyRegisterGroups_(cmm->id, props);

    if(i == 0)
      first_group_n = oy_group;
    
    //DBG_S(("oyGROUP[%d]: %s", i, groupa[i]));
    DBG_S(("   [%d]: %s", i, oyXMLgetValue_(groupa[i], "oyCONFIG_STRING_XML")));
    DBG_S(("   [%d]: %s", i, _( oyXMLgetValue_(groupa[i], "oyNAME")) ));
    DBG_S(("   [%d]: %s", i, _( oyXMLgetValue_(groupa[i], "oyDESCRIPTION")) ));
    DBG_S(("   [%d]: %s", i, oyXMLgetValue_(groupa[i], "oyNIX")));
    DBG_S(("   [%d]: %s", i, oyGetGroupUITitle( first_group_n + i, NULL )));
  }
  cmm->groups_start = first_group_n;
  cmm->groups_end   = cmm->groups_start + count - 1;

  options = oyXMLgetValue_(cmm_reg, "oyOPTIONS");
  option = oyXMLgetArray_(options, "oyOPTION", &count);
  for(i = 0; i < count; ++i)
  {
    int group_n = 0;
    char **grs = NULL;
    //DBG_S(("oyOPTION[%d]: %s", i, option[i]));
    DBG_S(("       : %s", oyXMLgetValue_(option[i], "oyID")));
    grs = oyXMLgetArray_(option[i], "oyGROUP", &group_n);
    for( k = 0; k < group_n; ++k)
      DBG_S(("       => %s", _( oyXMLgetValue_(groupa[ atoi(grs[k]) ], "oyNAME") )));
    DBG_S(("       : %s", _( oyXMLgetValue_(option[i], "oyNAME") )));
    DBG_S(("       : %s", _( oyXMLgetValue_(option[i], "oyDESCRIPTION") )));
    choices = oyXMLgetValue_(option[i], "oyCHOICES");
    choice = oyXMLgetArray_(choices, "oyNAME", &count2);
    DBG_S(("       : %s", oyXMLgetValue_(option[i], "oyCONFIG_STRING")));
    DBG_S(("       : %s", oyXMLgetValue_(option[i], "oyCONFIG_STRING_XML")));
    
    for(j = 0; j < count2; ++j)
    {
      DBG_S(("         : %s", _(choice[j])));
    }
  }

  oy_debug=0;

#ifdef USE_GETTEXT
  if(old_td && (strcmp(old_td, domain) != 0))
  {
    if(old_bdtd)
      bindtextdomain( old_td, old_bdtd );
    textdomain( old_td );
    DBG_PROG_S(("Setting back to old textdomain: %s in %s", old_td, old_bdtd))
  }
#endif

  DBG_PROG_ENDE
  return err;
}

int
oyCmmRegisterXML_(oyGROUP           group,
                  const char       *xml,
                  const char       *domain,
                  const char       *domain_path)
{

  int   err = 0;
  oyCMM_t cmm;

  oyCmmGetFromXML_(group, xml, domain, domain_path, &cmm);
  oyCmmAdd_(&cmm);

  DBG_PROG_ENDE
  return err;
}



