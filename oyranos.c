/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2006  Kai-Uwe Behrmann
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
 * sorting
 * 
 */

/* Date:      25. 11. 2004 */

#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_check.h"
#include "oyranos_cmms.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_texts.h"
#include "oyranos_xml.h"

/* --- Helpers  --- */
#if 1
#define ERR if (rc<=0 && oy_debug) { printf("%s:%d %d\n", __FILE__,__LINE__,rc); perror("Error"); }
#else
#define ERR
#endif

/* --- static variables   --- */

/* --- structs, typedefs, enums --- */

/* --- internal API definition --- */

/* separate from the external functions */
int   oyPathsCount_             (void);
char* oyPathName_               (int           number,
                                 oyAllocFunc_t allocate_func);
int   oyPathAdd_                (const char* pathname);
void  oyPathRemove_             (const char* pathname);
void  oyPathSleep_              (const char* pathname);
void  oyPathActivate_           (const char* pathname);
char* oyGetPathFromProfileName_ (const char*   profilename,
                                 oyAllocFunc_t allocate_func);


/* oyranos part */
/* check for the global and the users directory */
void oyCheckDefaultDirectories_ ();
/* search in profile path and in current path */
char* oyFindProfile_ (const char* name);



/* small helpers */
#define OY_FREE( ptr ) if(ptr) { free(ptr); ptr = 0; }


/* --- function definitions --- */






int
oyGetPathFromProfileNameCb_ (void* data, const char* full_name,
                                         const char* filename)
{
  char* search = (char*) data;
  int success = 0;
  DBG_S(( search ))
  if(strcmp(filename,search)==0) {
    size_t size = 128;
    char* header = oyReadFileToMem_ (full_name, &size, oyAllocateFunc_);
    success = !oyCheckProfile_Mem (header, size, 0);
    OY_FREE (header)
    if (success) {
      DBG_S((full_name))
      DBG_V((strlen(full_name)))
      if (strlen(full_name) < MAX_PATH) {
        sprintf(search,full_name);
        search[strlen(full_name)] = 0;
      } else
        search[0] = 0;
    } else
      WARN_S(( _("not a profile %s"), oyNoEmptyName_m_(full_name) ))
  }
  /* break on success */
  DBG_V((success))
  return success;
}

char*
oyGetPathFromProfileName_       (const char*   fileName,
                                 oyAllocFunc_t allocate_func)
{
  DBG_PROG_START
  char  *fullFileName = 0;
  char  *pathName = 0;
  int    success = 0;
  char  *header = 0;
  size_t    size;

  //DBG_NUM_S((fileName))
  /* test for pure file without dir; search in configured paths */
  if (fileName && !strchr(fileName, OY_SLASH_C))
  { DBG_PROG
    char search[MAX_PATH];
    if(strlen(fileName) < MAX_PATH)
      sprintf(search, fileName);
    else {
      WARN_S((_("name longer than %d"), MAX_PATH));
      DBG_PROG_ENDE
      return 0;
    }
    success = oyRecursivePaths_(oyGetPathFromProfileNameCb_,(void*)search);
#   if 0
    int   n_paths = oyPathsCount_ (),
          i;

    DBG_PROG_S(("pure filename found"))
    DBG_PROG_S(("n_paths = %d", n_paths ))
    fullFileName = (char*) allocate_func( MAX_PATH );

    for (i = 0; i < n_paths; i++)
    { /* test profile */
      char* ptr = oyPathName_ (i, oyAllocateFunc_);
      pathName = oyMakeFullFileDirName_ (ptr);
      sprintf (fullFileName, "%s%s%s", pathName, OY_SLASH, fileName);

      DBG_PROG_S((pathName))
      DBG_PROG_S((fullFileName))

      if (oyIsFileFull_(fullFileName))
      { DBG_PROG
        size = 128;
        header = oyReadFileToMem_ (fullFileName, &size, allocate_func);
        success = !oyCheckProfile_Mem (header, size);
      }

      OY_FREE (ptr)
      OY_FREE (header)
#     endif
      if (success) { /* found */
        int len = 0;
        DBG_S((search))
        if(search[0] != 0) len = strlen(search);
        if(len) {
          char *ptr = 0;
          pathName = (char*) allocate_func( len+1 );
          sprintf(pathName, search);
          ptr = strrchr(pathName , OY_SLASH_C);
          if(ptr)
            ptr[0] = 0;
        }
        DBG_PROG_S(( pathName ))
        DBG_PROG_ENDE
        return pathName;
      }
    //}

    if (!success) {
      if(oy_warn_)
        WARN_S( (_("profile %s not found in colour path\n"),
                 oyNoEmptyName_m_(fileName)));
      DBG_PROG_ENDE
      return 0;
    }

  } else
  {/* else use fileName as an full qualified name, check name and test profile*/
    DBG_PROG_S(("dir/filename found"))
    fullFileName = oyMakeFullFileDirName_ (fileName);

    if (oyIsFileFull_(fullFileName))
    {
      size = 128;
      header = oyReadFileToMem_ (fullFileName, &size, allocate_func);

      if (size >= 128)
        success = !oyCheckProfile_Mem (header, 128, 0);
    }

    if (!success) {
      WARN_S ((_("profile %s not found\n"), oyNoEmptyName_m_(fileName)))
      DBG_PROG_ENDE
      return 0;
    }

    pathName = oyExtractPathFromFileName_(fullFileName);

    OY_FREE (header)
  }

  if (!success)
  { OY_FREE (pathName)
    pathName = 0;
  }

  OY_FREE (fullFileName)

  DBG_PROG_ENDE
  return pathName;
}


/* public API implementation */

/* path names API */

/* default profiles API */
int
oySetDefaultProfile_       (oyDEFAULT_PROFILE type,
                            const char*       file_name)
{ DBG_PROG_START
  int r = 0;
  if( type == oyASSUMED_WEB &&
      !strstr( file_name,"sRGB" ) )
  {
    WARN_S((_("wrong profile for static web colour space selected, need sRGB")))
    return 1;
  }
  r = oySetProfile_ (file_name, type, 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultProfileBlock_  (oyDEFAULT_PROFILE type,
                            const char* file_name, void* mem, size_t size)
{ DBG_PROG_START
  int r = oySetProfile_Block (file_name, mem, size, type, 0);
  DBG_PROG_ENDE
  return r;
}


char*
oyGetDefaultProfileName_   (oyDEFAULT_PROFILE type,
                            oyAllocFunc_t     alloc_func)
{ DBG_PROG_START
  char* name = 0;
  
  DBG_PROG_S(( "%d",type ))

  /* a static_profile */
  if(type == oyASSUMED_WEB) {
    name = (char*) alloc_func (MAX_PATH);
    if( !name ) return 0;
    sprintf(name, OY_WEB_RGB);
    DBG_PROG_S(( name ))
    return name;
  }

  name = oyGetKeyValue_( oyOptionGet_(type)-> config_string, alloc_func );

  if(name) DBG_PROG_S((name));

  DBG_PROG_ENDE
  return name;
}




/* profile lists API */

#define MAX_DEPTH 64
struct OyProfileList_s_ {
  int hopp;
  const char* coloursig;
  int mem_count;
  int count_files;
  char** names;
};


/* profile handling API */

int
oySetProfile_Block (const char* name, void* mem, size_t size,
                    oyDEFAULT_PROFILE type, const char* comnt)
{ DBG_PROG_START
  int r = 0;
  char *fullFileName, *resolvedFN;
  const char *fileName;

  if (strrchr (name, OY_SLASH_C))
    fileName = strrchr (name, OY_SLASH_C);
  else
    fileName = name;

  fullFileName = (char*) calloc (sizeof(char),
                  strlen(OY_PROFILE_PATH_USER_DEFAULT) + strlen (fileName) + 4);

  sprintf (fullFileName, "%s%s%s",
           OY_PROFILE_PATH_USER_DEFAULT, OY_SLASH, fileName);

  resolvedFN = oyResolveDirFileName_ (fullFileName);
  OY_FREE(fullFileName)
  fullFileName = resolvedFN;

  if (!oyCheckProfile_Mem( mem, size, 0))
  {
    DBG_PROG_S((fullFileName))
    if ( oyIsFile_(fullFileName) ) {
      WARN_S((_("file %s exist , please remove befor installing new profile\n"),
              fullFileName))
    } else
    { r = oyWriteMemToFile_ (fullFileName, mem, size);
      oySetProfile_ ( name, type, comnt);
    }
  }

  DBG_PROG_S(("%s", name))
  DBG_PROG_S(("%s", fileName))
  DBG_PROG_S(("%ld %d", (long int)&((char*)mem)[0] , (int)size))
  OY_FREE(fullFileName)

  DBG_PROG_ENDE
  return r;
}

/* small search engine */

oyComp_t_*
oyInitComp_ (oyComp_t_ *list, oyComp_t_ *top)
{ DBG_PROG_START
  if (!list)
    list = (oyComp_t_*) calloc (1, sizeof(oyComp_t_));

  list->next = 0;

  if (top)
    list->begin = top;
  else
    list->begin = list;
  list->name = 0;
  list->val = 0;
  list->hits = 0;
  DBG_PROG_ENDE
  return list;
}

oyComp_t_*
oyAppendComp_ (oyComp_t_ *list, oyComp_t_ *new)
{ DBG_PROG_START

  /* no list yet => first and only entry */
  if (!list) {
    list = oyInitComp_(list,0);
    DBG_PROG_ENDE
    return list;
  }

  list = list->begin;
  while (list->next)
    list = list->next;

  /* no new => add */
  if (!new)
    new = oyInitComp_(new, list->begin);

  list->next = new;

  DBG_PROG_ENDE
  return new;
}

void
oySetComp_         (oyComp_t_ *compare, const char* keyName,
                    const char* value, int hits )
{
  DBG_PROG_START
  compare->name = (char*) calloc( strlen(keyName)+1, sizeof(char) );
  memcpy (compare->name, keyName, strlen(keyName)+1); 
  compare->val = (char*) calloc( strlen(value)+1, sizeof(char) );;
  memcpy (compare->val, value, strlen(value)+1); 
  compare->hits = hits;
  DBG_PROG_ENDE
}

void
oyDestroyCompList_ (oyComp_t_ *list)
{ DBG_PROG_START
  oyComp_t_ *before;

  list = list->begin;
  while (list->next)
  {
    before = list;
    list = list->next;
    OY_FREE(before)
  }
  OY_FREE(list)

  DBG_PROG_ENDE
}

char*
printComp (oyComp_t_* entry)
{ DBG_PROG_START
# ifdef DEBUG
  static char text[MAX_PATH] = {0};
  DBG_PROG_S(("%d", (int)(intptr_t)entry))
  sprintf( text, "%s:%d %s() begin %d next %d\n",
           __FILE__,__LINE__,__func__,
           (int)(intptr_t)entry->begin, (int)(intptr_t)entry->next );

  if(entry->name)
    sprintf( &text[strlen(text)], " name %s %d", entry->name,
                                  (int)(intptr_t)entry->name);
  if(entry->val)
    sprintf( &text[strlen(text)], " val %s %d", entry->val,
                                  (int)(intptr_t)entry->val);
  sprintf( &text[strlen(text)], " hits %d\n", entry->hits);

  DBG_PROG_ENDE
  return text;
# else
  DBG_PROG_ENDE
  return 0;
# endif
}



int
oySetDeviceProfile_                (const char* manufacturer,
                                    const char* model,
                                    const char* product_id,
                                    const char* host,
                                    const char* port,
                                    const char* attrib1,
                                    const char* attrib2,
                                    const char* attrib3,
                                    const char* profileName,
                                    const void* mem,
                                    size_t size)
{ DBG_PROG_START
  int rc = 0;
  char* comment = 0;

  if (mem && size && profileName)
  {
    rc = oyCheckProfile_Mem (mem, size, 0); ERR
  }

  if (!rc)
  { DBG_PROG
    oyEraseDeviceProfile_          ( manufacturer, model, product_id,
                                     host, port,
                                     attrib1, attrib2, attrib3);

    if (manufacturer || model || product_id || host || port || attrib1
        || attrib2 || attrib3)
    { int len = 0;
      DBG_PROG
      if (manufacturer) len += strlen(manufacturer);
      if (model) len += strlen(model);
      if (product_id) len += strlen(product_id);
      if (host) len += strlen(host);
      if (port) len += strlen(port);
      if (attrib1) len += strlen(attrib1);
      if (attrib2) len += strlen(attrib2);
      if (attrib3) len += strlen(attrib3);
      comment = (char*) calloc (len+10, sizeof(char)); DBG_PROG
      if (manufacturer) sprintf (comment, "%s", manufacturer); DBG_PROG
      if (model) sprintf (&comment[strlen(comment)], "%s", model); DBG_PROG
      if (product_id) sprintf (&comment[strlen(comment)], "%s", product_id);
      if (host) sprintf (&comment[strlen(comment)], "%s", host);
      if (port) sprintf (&comment[strlen(comment)], "%s", port);
      if (attrib1) sprintf (&comment[strlen(comment)], "%s", attrib1);
      if (attrib2) sprintf (&comment[strlen(comment)], "%s", attrib2);
      if (attrib3) sprintf (&comment[strlen(comment)], "%s", attrib3);
    } DBG_PROG

    rc =  oySetProfile_ (profileName, oyDEVICE_PROFILE, comment); ERR
  }

  DBG_PROG_ENDE
  return rc;
}



/* --- internal API decoupling --- */

#include "oyranos.h"

/** \addtogroup options Options API
 *  Functions to set and query for Options layout and UI strings in Oyranos.

 *  @{
 */


/** @brief all widgets of a group
 *
 *  param       group           specify which group dialog to build
 *  param[out]  count           number of widgets contained in list
 *
 *  return                      list of widgets to create in correct order
 */
oyWIDGET    * oyWidgetListGet          (oyGROUP           group,
                                        int             * count)
{
  oyWIDGET *list = NULL;

  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  list = oyWidgetListGet_                  ( group, count);

  oyExportEnd_();
  DBG_PROG_ENDE
  return list;
}

/** @brief Get a special Options UI strings.
 *
 *  param       option          merges oyBEHAVIOUR and oyDEFAULT_PROFILE
 *  param[out]  categories      enums list, to place into the right group widget
 *                              { n, first category, second c., ... , n'th c. } 
 *                              for widget of type oyGROUP this is empty
 *  param[out]  name            transated widget title
 *  param[out]  tooltip         transated tooltip
 *
 *  return                      widget type, gives a hint to further properties,
 *                              { like choices or int/float value ranges ... }
 */
oyWIDGET_TYPE oyWidgetTitleGet         (oyWIDGET          option,
                                        const oyGROUP  ** categories,
                                        const char     ** name,
                                        const char     ** tooltip,
                                        int             * flags )
{
  oyWIDGET_TYPE type = oyTYPE_START;

  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  type = oyWidgetTitleGet_                 ( option,
                                             categories,
                                             name, tooltip,
                                             flags );
  oyExportEnd_();
  DBG_PROG_ENDE
  return type;
}

/** @brief Get a Options choices UI strings.
 *
 *  This function is checked for double occuring profiles. Such are sorted out.
 *  @todo In the future a second choices_string_list may appear for displaying.
 *
 *  param       option          merge oyBEHAVIOUR and oyDEFAULT_PROFILE
 *  param[out]  choices         n choices; if choices is zero then you need to
 *                              optain the choices otherwise, like for profiles
 *  param[out]  choices_strings translated list of n choices
 *  param[out]  current         the actual setting
 *
 *  return                      success
 */
int         oyOptionChoicesGet         (oyWIDGET          option,
                                        int             * choices,
                                        const char    *** choices_string_list,
                                        int             * current )
{
  int error = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  error =   oyOptionChoicesGet_            ( option,
                                             choices, choices_string_list,
                                             current );
  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}

/** @brief delete list of choices from a option
 *  @param list     string list
 *  @param size     number of strings in the list to free
 */
void
oyOptionChoicesFree                  (oyWIDGET_TYPE     option,
                                      char          *** list,
                                      int               size)
{ DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  oyOptionChoicesFree_ (option, list, size);

  oyExportEnd_();
  DBG_PROG_ENDE
}

/*  @} */


/** \addtogroup behaviour Behaviour API
 *  Functions to set and query for behaviour on various actions in Oyranos.

 *  @todo is the missing description of available choices<br>

 *  Take a look at the Oyranos settings xml file. Options are described <a href="../../../color/settings/office.policy.xml">there</a>.
 
 *  Otherwise look at the \b oyranos-config-fltk application
    or the description on

 *  <a href="http://www.oyranos.com/wiki/index.php?title=Oyranos_Use_Cases">ColourWiki</a>.

 *  @{
 */

/** Set a special behaviour. Usual in control panel in Oyranos.\n 
 *
 *  @param  type      the type of behaviour
 *  @param  choice    the selected option
 *  @return error
 */
int
oySetBehaviour         (oyBEHAVIOUR       type,
                        int               choice)
{
  int error = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_SETTING);

  error = oySetBehaviour_(type, choice);

  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}

/** Get a special behaviour.\n 
 *
 *  @param  type      the type of behaviour
 *  @return           the behaviour option (-1 if not available or not set)
 */
int
oyGetBehaviour         (oyBEHAVIOUR       type)
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_SETTING);

  n = oyGetBehaviour_(type);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/*  @} */

/** \addtogroup policy Policy API
 *  Functions to set and export policies in Oyranos.
 *
 *  @todo define some default policies internally
 *
 *  @{
 */

/** Save a group of policy settings.\n
 *  Write only such variables, which are available and ignore unknown ones.
 *  This currently produces pseudo xml configuration files.
 *
 *  @param  group     the policy group
 *  @param  add_header     add description
 *  @param allocate_func user provided function for allocating the strings memory
 *  @return           the configuration as XML to save to file
 */
char*
oyPolicyToXML          (oyGROUP           group,
                        int               add_header,
                        oyAllocFunc_t     allocate_func)
{
  char* text = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_SETTING);

  text = oyPolicyToXML_(group, add_header, allocate_func);

  oyExportEnd_();
  DBG_PROG_ENDE
  return text;
}

/** Load a group of policy settings.\n
 *  use xml-ish input produced by oyPolicyToXML()
 *
 *  @param  group     the policy group
 *  @param  xml       xml configuration string
 *  @return           errors
 */
int
oyReadXMLPolicy        (oyGROUP           group,
                        const char       *xml)
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_SETTING);

  n = oyReadXMLPolicy_(group, xml);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/*  @} */

/** \addtogroup path_names Path Names API
 *  Functions to handle path configuration for Oyranos.
 *
 *  Paths include operating system standard paths. For linux these are:
 *  <ul><li>  /usr/share/color/icc - for global system profiles</li>
 *  <li>    ~/.color/icc - for user profiles</li>
 *  <li>    /usr/local/share/color/icc - for local system profiles</li>
 *  </ul>
 *  Paths are scanned recursively excluding symbolic links.

 *  @see @ref profile_lists

 *  @{
 */

/** Determin the count of configured search paths.\n */
int
oyPathsCount         (void)
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  n = oyPathsCount_();
  if(!n)
    oyPathAdd_ (OY_PROFILE_PATH_USER_DEFAULT);
  n = oyPathsCount_();

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/** Get the path name at a certain position in the Oyranos search path list.
 *
 *  @param number        number in list - dont rely on through sessions
 *  @param allocate_func user provided function for allocating the string memory
 *  @return              the pathname at position \e number
 */
char*
oyPathName           (int           number,
                      oyAllocFunc_t allocate_func)
{
  char* name = NULL;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  name = oyPathName_ (number, allocate_func);

  oyExportEnd_();
  DBG_PROG_ENDE
  return name;
}

/** Add a path name statically to allready configured ones.
 *
 *  @param  pathname new pathname to use by Oyranos for profile search
 *  @return success
 */
int
oyPathAdd            (const char* pathname)
{ int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  n = oyPathAdd_ (pathname);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/** Remove a path name statically from the list of configured ones.
 *
 *  @param  pathname the pathname to remove from the Oyranos search path list
 */
void
oyPathRemove         (const char* pathname)
{ DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  oyPathRemove_ (pathname);

  oyExportEnd_();
  DBG_PROG_ENDE
}

void
oyPathSleep          (const char* pathname)
{ DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  oyPathSleep_ (pathname);

  oyExportEnd_();
  DBG_PROG_ENDE
}

void
oyPathActivate       (const char* pathname)
{ DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  oyPathActivate_ (pathname);

  oyExportEnd_();
  DBG_PROG_ENDE
}

/** Find out where in the Oyranos search path the specified profile resides.
 *
 *  @param  profile_name  the filename find in the Oyranos search path
 *  @param  allocate_func user provided function for allocating the string
                          memory
 *  @return the path name where the profile was found in the oyranos search path
 */
char*
oyGetPathFromProfileName (const char* profile_name, oyAllocFunc_t allocate_func)
{
  char* path_name = NULL;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  path_name = oyGetPathFromProfileName_ (profile_name, allocate_func);

  oyExportEnd_();
  DBG_PROG_ENDE
  return path_name;
}

/*  @} */

/** \addtogroup default_profiles Default Profiles API
 *  Functions to handle and configure default profiles.

 *  @see @ref path_names

 *  @{
 */

/** Sets a profile, which is available in the current configured path.
 *
 *  @param  type      the kind of default profile
 *  @param  file_name the profile which shall become the default for the above
 *                    specified profile type
 *  @return success
 */
int
oySetDefaultProfile        (oyDEFAULT_PROFILE type,
                            const char*       file_name)
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  n = oySetDefaultProfile_ (type, file_name);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/** Sets a profile, which is available in the current configured path.
 *
 *  @param  type      the kind of default profile
 *  @param  file_name the profile which shall become the default for the above
 *          specified profile type
 *  @param  mem       the memory containing the profile
 *  @param  size      the profile size in mem
 *  @return success
 */
int
oySetDefaultProfileBlock   (oyDEFAULT_PROFILE type,
                            const char*       file_name,
                            void*             mem,
                            size_t            size)
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  n = oySetDefaultProfileBlock_ (type, file_name, mem, size);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/** Gets a default profile filename.
 *
 *  @param  type          the kind of default profile
 *  @param  allocate_func the allocation function needed to reserve memory for
                          the filename string
 *  @return filename
 */
char*
oyGetDefaultProfileName    (oyDEFAULT_PROFILE type,
                            oyAllocFunc_t     allocate_func)
{
  char* name = NULL;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  name = oyGetDefaultProfileName_ (type, allocate_func);

  oyExportEnd_();
  DBG_PROG_ENDE
  return name;
}
/*  @} */


/** \addtogroup profile_lists Profile Lists API
 *  Functions to handle profile name lists.

 *  @see @ref path_names

 *  @{
 */

/** @brief get a list of profile filenames
 *  @param coloursig filter as ICC 4 byte string
 *  @param[out] size profile filenames count
 *  @return the profiles filename list allocated within Oyranos
 *
 *  @see oyProfileListFree to free lists memory after usage is completed.
 *  @see @ref path_names
 *
 *  @todo use coloursig variable; extract such information from profiles
 */
char**
oyProfileListGet                   (const char* coloursig, int *size)
{
  char **names = NULL;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  names = oyProfileListGet_(coloursig, size);

  oyExportEnd_();
  DBG_PROG_ENDE
  return names;
}

/** @} */


/** \addtogroup profile_handling Profile Handling API
 *  Functions to handle profiles.

 *  @{
 */

/** @brief check an file if it is a profile
 *
 *  @param  filename  the filename to check
 *  @param  coloursig colour space signature as described in the
                      <a href="http://www.color.org">ICC specification</a>
 *  @return true/false
 */
int
oyCheckProfile (const char* filename, const char* coloursig)
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  /* coloursig is currently ignored */
  n = oyCheckProfile_ (filename, coloursig);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/** @brief check an memory block if it is a profile
 *
 *  @param  mem  the memory containing the profile
 *  @param  size the size of the memory block
 *  @param  coloursig currently ignored
 *  @return true/false
 */
int
oyCheckProfileMem (const void* mem, size_t size, const char* coloursig)
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  /* coloursig is currently ignored */
  n = oyCheckProfile_Mem (mem, size, coloursig);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/** @brief get the profiles size
 *
 *  @param  profilename  specifies the profile
 *  @return size
 */
size_t
oyGetProfileSize                  (const char* profilename)
{
  size_t size = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  size = oyGetProfileSize_ (profilename);

  oyExportEnd_();
  DBG_PROG_ENDE
  return size;
}

/** @brief obtain an memory block in the responsibility of the user
 *
 *  Please note the size parameter is sensible. Eighter set it to zero or to  
 *  the size obtained with oyGetProfileSize to get the full data or
 *  specify the size you want to get a partitial profile. \n
 *  This feature is useful for instance to get only the headers of many profiles.
 *
 *  @param  profilename  specifies the profile
 *  @param[in]  size     desired size, or zero for a complete copy
    @param[out] size     the size of the returned block
 *  @param      allocate_func the users memory allocation function
 *  @return             the profile block in memory allocated by oyAllocFunc_t
 */
void*
oyGetProfileBlock                 (const char* profilename, size_t *size,
                                   oyAllocFunc_t allocate_func)
{
  char* block = NULL;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  block = oyGetProfileBlock_ (profilename, size, allocate_func);
  DBG_PROG_S( ("%s %hd %d", profilename, (int)(intptr_t)block, (int)(intptr_t)*size) )
  DBG_PROG

  oyExportEnd_();
  DBG_PROG_ENDE
  return block;
}

/** @} */



/**
 *  @internal 
 *  \addtogroup device_profiles Device Profiles API
 * 
 * There different approaches to select an (mostly) fitting profile
 *
 * A: search and compare all available profiles by \n
 *    - ICC profile class
 *    - Manufacturer / Model (as written in profile tags)
 *    - other hints\n
 *    .
 * B: install an profile and tell Oyranos about the belonging device and the
 *    invalidating period\n
 * C: look for similarities of devices of allready installed profiles\n
 * D: use the md5 checksum stored in the profile

 * @see @ref path_names

 * @{
 */

/** @brief ask for a profile name by specifying device attributes
 *
 *  @param typ            kind of device
 *  @param manufacturer   the device manufacturer (EIZO)
 *  @param model          the model (LCD2100)
 *  @param product_id     the ID reported during connection (ID_701200xx)
 *  @param host           useful for monitor identification (grafic:0.0)
 *  @param port           kind of connection (Matrox G650)
 *  @param attrib1        additional attribute
 *  @param attrib2        additional attribute
 *  @param attrib3        additional attribute
 *  @param allocate_func  the users memory allocation function
 * 
 *  simply pass 0 for not specified properties<br>

   \code
   char* profile_name = oyGetDeviceProfile ("EIZO", "LCD2100",
                                            "ID 87-135.19",
                                            "grafic:0.0", "Matrox G650",
                                             "100lux", 0,
                                            "", allocate_func);
   if (profile_name)
   { char* ptr = NULL;
     ptr = oyGetProfileBlock (profile_name, allocate_func);
       // do something
     free (ptr);
   }
   \endcode

   \return allocated by oyAllocFunc_t
 */
char*
oyGetDeviceProfile                (oyDEVICETYP typ,
                                   const char* manufacturer,
                                   const char* model,
                                   const char* product_id,
                                   const char* host,
                                   const char* port,
                                   const char* attrib1,
                                   const char* attrib2,
                                   const char* attrib3,
                                   oyAllocFunc_t allocate_func)
{
  char* profile_name = NULL;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  profile_name = oyGetDeviceProfile_ (manufacturer, model, product_id,
                                    host, port, attrib1, attrib2, attrib3,
                                    allocate_func);
  if(profile_name)
    DBG_PROG_S( (profile_name) );

  oyExportEnd_();
  DBG_PROG_ENDE
  return profile_name;
}

/** @brief set a profile name with specifying device attributes
 *  @param mem remains in the users domain
 */
int
oySetDeviceProfile                (oyDEVICETYP typ,
                                   const char* manufacturer,
                                   const char* model,
                                   const char* product_id,
                                   const char* host,
                                   const char* port,
                                   const char* attrib1,
                                   const char* attrib2,
                                   const char* attrib3,
                                   const char* profileName,
                                   const void* mem,
                                   size_t size)
{
  int rc = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  rc =         oySetDeviceProfile_ (manufacturer, model, product_id,
                                    host, port, attrib1, attrib2, attrib3,
                                    profileName, mem, size);

  oyExportEnd_();
  DBG_PROG_ENDE
  return rc;
}

/** @brief remove or deinstall the profile from the current path */
int
oyEraseDeviceProfile              (oyDEVICETYP typ,
                                   const char* manufacturer,
                                   const char* model,
                                   const char* product_id,
                                   const char* host,
                                   const char* port,
                                   const char* attrib1,
                                   const char* attrib2,
                                   const char* attrib3)
{
  int rc = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  rc =       oyEraseDeviceProfile_ (manufacturer, model, product_id,
                                    host, port, attrib1, attrib2, attrib3);

  oyExportEnd_();
  DBG_PROG_ENDE
  return rc;
}


/** @} */



/** \addtogroup cmm_handling CMM Handling API
 *  Functions to handle ColorMatchingModules.

 *  @{
 */

/** @brief  read in the declarations of available options and functions 
 *
 *  @param  group        the policy group
 *  @param  xml          xml configuration string
 *  @param  domain       i18n gettext domain
 *  @param  domain_path  i18n gettext domain path 
 *  @return              errors
 */
int
oyModulRegisterXML       (oyGROUP           group,
                        const char       *xml)
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  n = oyModulRegisterXML_(group, xml);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/** @brief  get the user allocated CMM 4 char ID's
 *
 *  @param  count          the number of CMM's available
 *  @param  allocate_func  the users memory allocation function
 *  @return allocated by oyAllocFunc_t
 * 
   \code
   int    count, i;
   char** ids = oyModulsGetNames ( &count, allocate_func);
   for (i = 0; i < count; ++i)
   { printf( "CMM short name: %s\n", ids[i]);
     free (ids[i]);
   }
   if(count && ids)
     free (ids);
   \endcode

 */
char**
oyModulsGetNames       ( int        *count,
                         oyAllocFunc_t allocate_func )
{
  char** ids = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  ids = oyModulsGetNames_(count, allocate_func);

  oyExportEnd_();
  DBG_PROG_ENDE
  return ids;
}
/** @} */

/** \addtogroup misc Miscellaneous
 *  Miscellaneous stuff.

 *  @{
 */

/** @brief  switch internationalisation of strings on or off
 *
 *  @param  active         bool
 *  @param  reserved       for future use
 */
void 
oyI18NSet              ( int active,
                         int reserved )
{
  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  oyI18NSet_(active, reserved);

  oyExportEnd_();
  DBG_PROG_ENDE
}

/** @brief  calculate a md5 digest beginning after the header offset
 *
 *  The md5 implementation is written by L. Peter Deutsch
 *
 *  @param[in]  buffer         complete profiles buffer
 *  @param[in]  size           over all profile size
 *  @param[out] md5_return     buffer to write in the md5 digest (128 bytes)
 *
 *  @return                    error
 */
int
oyProfileGetMD5        ( void       *buffer,
                         size_t      size,
                         char       *md5_return )
{
  int error = 0;
  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  error = oyProfileGetMD5_(buffer, size, md5_return);

  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}
/** @} */


