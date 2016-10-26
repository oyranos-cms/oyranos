/** @file oyranos_db.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2015 (C) Kai-Uwe Behrmann
 *
 *  @brief    elektra dependent functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2004/11/25
 */

#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <kdb.h>

#include "oyOptions_s.h"
#include "oyranos_config.h"
#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_check.h"
#include "oyranos_db.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_io.h"
#include "oyranos_internal.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_types.h"
#include "oyranos_xml.h"


/* --- Helpers  --- */

#define AD oyAllocateFunc_, oyDeAllocateFunc_
#define oyDEVICE_PROFILE oyDEFAULT_PROFILE_END

int      oySetProfile_               ( const char        * name,
                                       oySCOPE_e           scope,
                                       oyPROFILE_e         type,
                                       const char        * comment )
{
  int r = 1;
  const char *fileName = 0, *com = comment;

  DBG_PROG_START

  /* extract filename */
  if (name && strrchr(name , OY_SLASH_C))
  {
    fileName = strrchr(name , OY_SLASH_C);
    fileName++;
  } else
    fileName = name;

  DBG_PROG2_S( "name = %s type %d", name, type )

  if ( name == 0 || !oyCheckProfile_ (fileName, 0, 0) )
  {
    const char* config_name = 0;
    DBG_PROG2_S("set fileName = %s as %d profile\n",fileName, type)
    if ( type < oyDEFAULT_PROFILE_START || oyDEFAULT_PROFILE_END < type )
      WARNc2_S( "%s %d", _("default profile type does not exist:"), type  );

    if(oyWidgetTitleGet_( (oyWIDGET_e)type, 0,0,0,0 ) ==
       oyWIDGETTYPE_DEFAULT_PROFILE)
    {
      config_name = oyOptionGet_((oyWIDGET_e)type)-> config_string;

    } else if(type == oyDEVICE_PROFILE)
      {
        int len = strlen(OY_REGISTRED_PROFILES)
                  + strlen(fileName);
        char* key_name = NULL;
        oyStringAddPrintf( &key_name, AD, "%s%s", OY_REGISTRED_PROFILES OY_SLASH, fileName );
        r = oySetPersistentString (key_name, scope, com, 0);
        DBG_PROG2_S( "%s %d", key_name, len )
        oyFree_m_ (key_name)
      }
      else
        WARNc2_S( "%s %d", _("default profile type does not exist:"), type );
      
    
    if(config_name)
    {
      if(name)
      {
        r = oySetPersistentString (config_name, scope, fileName, com);
        DBG_PROG3_S( "%s %s %s",config_name,fileName,com?com:"" )
      }
      DBG_PROG_V( r )
    }
  }

  DBG_PROG_ENDE
  return r;
}

/* public API implementation */

static int oy_db_handling_ready_ = 0;

/**
 *  @brief tell if DB handling is ready
 *  @ingroup objects_value
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/10/23
 *  @since   2016/10/23 (Oyranos: 0.9.6)
 */
int                oyDbHandlingReady ( void )
{
  return oy_db_handling_ready_;
}

/** @fn      oyDbHandlingSet
 *  @ingroup objects_value
 *  @brief set DB functions for persistent settings
 *
 *  Be careful to set the APIs only before any call to the oyDB_s functions.
 *  Otherwise the behaviour is undefined.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/10/23
 *  @since   2016/10/23 (Oyranos: 0.9.6)
 */
int                oyDbHandlingSet   ( const oyDbAPI_s   * db_api )
{
  if(oy_db_handling_ready_)
  {
    WARNc1_S( "%s", _("DB handling is already set") );
    return 1;
  } else
  {
                 oyDB_newFrom = db_api->newFrom;
                 oyDB_release = db_api->release;
               oyDB_getString = db_api->getString;
              oyDB_getStrings = db_api->getStrings;
             oyDB_getKeyNames = db_api->getKeyNames;
     oyDB_getKeyNamesOneLevel = db_api->getKeyNamesOneLevel;
                oyDBSetString = db_api->setString;
       oyDBSearchEmptyKeyname = db_api->searchEmptyKeyname;
                 oyDBEraseKey = db_api->eraseKey;

    oy_db_handling_ready_ = 1;
    return 0;
  }
}

/* private helper */

oyDB_s * oyDB_newFromInit            ( const char        * top_key_name,
                                       oySCOPE_e           scope,
                                       oyAlloc_f           allocFunc,
                                       oyDeAlloc_f         deAllocFunc );
/* First try Elektra then any other "db_handler".  */
int                oyDbInitialise_  ( void )
{
  oyOptions_s * opts = 0,
              * result_opts = 0;

  opts = oyOptions_New(0);
  int error = oyOptions_Handle( "//"OY_TYPE_STD"/db_handler.elDB",
                                opts,"db_handler",
                                &result_opts );
  if(error || oyDB_newFrom == oyDB_newFromInit)
  {
    oyMessageFunc_p( oyMSG_DBG, NULL, OY_DBG_FORMAT_
                     " can't initialise Elektra \"db_handler\"",OY_DBG_ARGS_);

    error = oyOptions_Handle( "//"OY_TYPE_STD"/db_handler",
                              opts,"db_handler",
                              &result_opts );
    if(error || oyDB_newFrom == oyDB_newFromInit)
    {
      oyMessageFunc_p( oyMSG_WARN, NULL, OY_DBG_FORMAT_
                       " can't properly call \"db_handler\"",OY_DBG_ARGS_);
      if(oyDB_newFrom == oyDB_newFromInit)
        error = 1;
    }
  }

  oyOptions_Release( &opts );
  return error;
}
int                oyDbHandlingInit  ( void )
{
  if(oy_db_handling_ready_)
  {
    WARNc1_S( "%s", _("DB handling is already set") );
    return 1;
  } else
  {
    return oyDbInitialise_();
  }
}

/* DB hooks */
oyDB_s * oyDB_newFromInit            ( const char        * top_key_name,
                                       oySCOPE_e           scope,
                                       oyAlloc_f           allocFunc,
                                       oyDeAlloc_f         deAllocFunc )
{ 
  if(oyDbHandlingInit() <= 0)
    return oyDB_newFrom(top_key_name,scope,allocFunc,deAllocFunc);
  else
    return 0;
}
void     oyDB_releaseInit            ( oyDB_s           ** db )
{
  if(oyDbHandlingInit() <= 0)
    oyDB_release( db );
}
char *   oyDB_getStringInit          ( oyDB_s            * db,
                                       const char        * key_name )
{ 
  if(oyDbHandlingInit() <= 0)
    return oyDB_getString(db,key_name);
  else
    return 0;
}
int      oyDB_getStringsInit         ( oyDB_s            * db,
                                       oyOptions_s      ** options,
                                       const char       ** key_names,
                                       int                 key_names_n )
{ 
  if(oyDbHandlingInit() <= 0)
    return oyDB_getStrings(db,options,key_names,key_names_n);
  else
    return 1;
}
char **  oyDB_getKeyNamesInit        ( oyDB_s            * db,
                                       const char        * key_name,
                                       int               * n )
{ 
  if(oyDbHandlingInit() <= 0)
    return oyDB_getKeyNames(db,key_name,n);
  else
    return 0;
}
char **  oyDB_getKeyNamesOneLevelInit (oyDB_s            * db,
                                       const char        * key_name,
                                       int               * n )
{ 
  if(oyDbHandlingInit() <= 0)
    return oyDB_getKeyNamesOneLevel(db,key_name,n);
  else
    return 0;
}

int      oyDBSetStringInit           ( const char        * keyName,
                                       oySCOPE_e           scope,
                                       const char        * value,
                                       const char        * comment )
{ 
  if(oyDbHandlingInit() <= 0)
    return oyDBSetString(keyName,scope,value,comment);
  else
    return 1;
}
char*    oyDBSearchEmptyKeynameInit    ( const char      * key_parent_name,
                                         oySCOPE_e         scope )
{ 
  if(oyDbHandlingInit() <= 0)
    return oyDBSearchEmptyKeyname(key_parent_name,scope);
  else
    return 0;
}
int      oyDBEraseKeyInit            ( const char        * key_name,
                                       oySCOPE_e           scope )
{ 
  if(oyDbHandlingInit() <= 0)
    return oyDBEraseKey(key_name,scope);
  else
    return 1;
}

oyDB_newFrom_f             oyDB_newFrom = oyDB_newFromInit;
oyDB_release_f             oyDB_release = oyDB_releaseInit;
oyDB_getString_f           oyDB_getString = oyDB_getStringInit;
oyDB_getStrings_f          oyDB_getStrings = oyDB_getStringsInit;
oyDB_getKeyNames_f         oyDB_getKeyNames = oyDB_getKeyNamesInit;
oyDB_getKeyNamesOneLevel_f oyDB_getKeyNamesOneLevel = oyDB_getKeyNamesOneLevelInit;
oyDBSetString_f            oyDBSetString = oyDBSetStringInit;
oyDBSearchEmptyKeyname_f   oyDBSearchEmptyKeyname = oyDBSearchEmptyKeynameInit;
oyDBEraseKey_f             oyDBEraseKey = oyDBEraseKeyInit;


#define oyjl_string_add                oyStringAddPrintf
#define oyjl_string_copy               oyStringCopy
#define oyjl_string_split              oyStringSplit
#define oyjl_string_list_add_list      oyStringListAdd
#define oyjl_string_list_cat_list      oyStringListCat
#define oyjl_string_list_release       oyStringListRelease_
#define oyjl_string_list_free_doubles  oyStringListFreeDoubles
#define oyjl_string_list_add_static_string oyStringListAddStaticString
#include "../oyjl/oyjl_tree.c"

