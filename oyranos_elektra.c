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
 */

/** @file @internal
 *  @brief elektra dependent functions
 */

/* Date:      25. 11. 2004 */

#include <kdb.h>
#include <alloca.h>
#include <sys/stat.h>
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
#include "oyranos_io.h"
#include "oyranos_internal.h"
#include "oyranos_sentinel.h"
#include "oyranos_xml.h"

/* --- Helpers  --- */
#if 1
#define ERR if (rc<=0 && oy_debug) { printf("%s:%d %d\n", __FILE__,__LINE__,rc); perror("Error"); }
#else
#define ERR
#endif

/* --- static variables   --- */

static int oyranos_init = 0;
int oy_warn_ = 1;
#define OY_WEB_RGB "sRGB.icc"

/* --- structs, typedefs, enums --- */


/* --- internal API definition --- */

/* elektra key list handling */
char*   oySearchEmptyKeyname_  (const char* keyParentName,
                                const char* keyBaseName);
KeySet* oyReturnChildrenList_  (const char* keyParentName,int* rc);



/* separate from the external functions */


#define oyDEVICE_PROFILE oyDEFAULT_PROFILE_END
static KDBHandle oy_handle_;

void oyOpen_ (void)
{
  if(!oyranos_init) {
    kdbOpen( &oy_handle_ );
    oyranos_init = 1;
  }
  kdbOpen( &oy_handle_ );
}
void oyClose_() { /*kdbClose( &oy_handle_ );*/ }
/* @todo make oyOpen unnecessary */
void oyOpen  (void) { oyOpen_(); }
void oyClose (void) { oyClose_(); }


/* oyranos part */


oyComp_t_* oyGetDeviceProfile_sList          (const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3,
                                           KeySet* profilesList,
                                           int   rc);



/* small helpers */
#define OY_FREE( ptr ) if(ptr) { free(ptr); ptr = 0; }

  /* ksNext uses the same entry twice in a 1 component KeySet, we avoid this */
#define FOR_EACH_IN_KDBKEYSET( current_, list ) \
   ksRewind( list );  \
   for( current_ = ksNext( list ); current_; current_ = ksNext( list )  )



/* --- function definitions --- */

KeySet*
oyReturnChildrenList_ (const char* keyParentName, int* rc)
{ DBG_PROG_START
  int user_sys = oyUSER_SYS;
  KeySet*list_user = 0;
  KeySet*list_sys = 0;
  KeySet*list = ksNew();
  char  *list_name_user = (char*)alloca(MAX_PATH);
  char  *list_name_sys = (char*)alloca(MAX_PATH);

  if( user_sys == oyUSER_SYS || user_sys == oyUSER ) {
    list_user = ksNew();
    sprintf(           list_name_user, "%s%s", OY_USER, keyParentName);
    *rc =
      kdbGetChildKeys( oy_handle_, list_name_user, list_user, KDB_O_RECURSIVE | KDB_O_SORT);
  }
  if( user_sys == oyUSER_SYS || user_sys == oySYS ) {
    list_sys = ksNew();
    sprintf(           list_name_sys, "%s%s", OY_SYS, keyParentName);
    *rc =
      kdbGetChildKeys( oy_handle_, list_name_sys, list_sys, KDB_O_RECURSIVE | KDB_O_SORT);
  }

  if(list_user)
    ksAppendKeys(list, list_user);
  if(list_sys)
    ksAppendKeys(list, list_sys);

  DBG_PROG_V(( (intptr_t)keyParentName ))
  DBG_PROG_S(( keyParentName ))
  DBG_PROG_V(( (intptr_t)ksGetSize(list) ))

  DBG_PROG_ENDE
  return list;
}

char*
oySearchEmptyKeyname_ (const char* keyParentName, const char* keyBaseName)
{ DBG_PROG_START
  char* keyName = (char*)     calloc (strlen(keyParentName)
                                    + strlen(keyBaseName) + 24, sizeof(char));
  char* pathkeyName = (char*) alloca (strlen(keyBaseName) + 24);
  int nth = 0, i = 1, rc=0;
  Key *key;
  char *name = (char*)alloca(MAX_PATH);
  sprintf(name, "%s%s", oySelectUserSys_(), keyParentName);

  key = keyNew( KEY_SWITCH_END );
  keySetName( key, keyBaseName );

  if(keyParentName)
    DBG_PROG_S((keyParentName));
  if(keyBaseName)
    DBG_PROG_S((keyBaseName));
  if(name)
    DBG_PROG_S((name));

    /* search for empty keyname */
    while (!nth)
    { sprintf (pathkeyName , "%s%d", keyBaseName, i);
      rc=kdbGetKeyByParent (oy_handle_, name, pathkeyName, key);
      if (rc != KDB_RET_OK)
        nth = i;
      i++;
    }
    sprintf (keyName, ("%s/%s"), OY_PATHS, pathkeyName);

  if(keyName)
    DBG_PROG_S((keyName));

  DBG_PROG_ENDE
  return keyName;
} 

int
oyKeySetHasValue_     (const char* keyParentName, const char* ask_value)
{ DBG_PROG_START
  int result = 0;
  int rc=0;
  char* value = (char*) calloc (sizeof(char), MAX_PATH);
  KeySet *myKeySet = oyReturnChildrenList_( keyParentName, &rc ); ERR
  Key *current;

        if(!myKeySet)
        {
          FOR_EACH_IN_KDBKEYSET( current, myKeySet )
          {
            keyGetName(current, value, MAX_PATH);
            DBG_NUM_S(( value ))
            if(strstr(value, ask_value) != 0 &&
               strlen(value) == strlen(ask_value))
            {
              DBG_PROG_S((value))
              result = 1;
              break;
            }
          }
        }
  ksClose (myKeySet);
  oyClose_();
  OY_FREE(myKeySet)

  DBG_PROG_ENDE
  return result;
}

int
oyAddKey_valueComment_ (const char* keyName,
                        const char* value,
                        const char* comment)
{ DBG_PROG_START
  int rc=0;
  Key *key;
  char *name = (char*)alloca(MAX_PATH);
  sprintf(name, "%s%s", oySelectUserSys_(), keyName);

  if (keyName)
    DBG_PROG_S(( keyName ));
  if (value)
    DBG_PROG_S(( value ));
  if (comment)
    DBG_PROG_S(( comment ));
  if (!keyName || !strlen(keyName))
    WARN_S( ("%s:%d !!! ERROR no keyName given",__FILE__,__LINE__));

  key = keyNew( KEY_SWITCH_END );
  keySetName( key, name );

  //rc=keyInit(key); ERR
  //rc=keySetName (key, keyName);
  rc=kdbGetKey( oy_handle_, key );
  rc=keySetString (key, value);
  rc=keySetComment (key, comment);
  //TODO debug
  oyOpen_();
  rc=kdbSetKey( oy_handle_, key );
  oyClose_();

  DBG_PROG_ENDE
  return rc;
}

const char*
oySelectUserSys_()
{
  /* enable system wide keys for user root */
  if(geteuid() == 0)
    return OY_SYS;
  else
    return OY_USER;
}


int
oySetBehaviour_      (oyBEHAVIOUR type, int choice)
{ DBG_PROG_START
  int r = 1;

  DBG_PROG_S( ("type = %d behaviour %d", type, choice) )

  if ( (r=oyTestInsideBehaviourOptions_(type, choice)) == 1 )
  {
    const char *keyName = 0;

    keyName = oyOptionGet_(type)-> config_string;

      if(keyName)
      {
        char val[12];
        const char *com =
            oyOptionGet_(type)-> choice_list[ choice ];
        snprintf(val, 12, "%d", choice);
        r = oyAddKey_valueComment_ (keyName, val, com);
        DBG_PROG_S(( "%s %d %s %s", keyName, type, val, com?com:"" ))
      }
      else
        WARN_S( ("%s:%d !!! ERROR type %d behaviour not possible",__FILE__,__LINE__, type));
  }

  DBG_PROG_ENDE
  return r;
}

int
oyGetBehaviour_      (oyBEHAVIOUR type)
{ DBG_PROG_START
  char* name = 0;
  const char* key_name = 0;
  int c = -1;

  DBG_PROG_S( ("type = %d behaviour", type) )

  if( oyTestInsideBehaviourOptions_(type, 0) )
  {
    key_name = oyOptionGet_(type)-> config_string;

    if(key_name)
    {
      name = oyGetKeyValue_( key_name, oyAllocateFunc_ );
    }
    else
      WARN_S( ("%s:%d !!! ERROR type %d behaviour not possible",__FILE__,__LINE__, type));
  }
  else
    WARN_S( ("%s:%d !!! ERROR type %d behaviour not possible",__FILE__,__LINE__, type));

  if(name)
    c = atoi(name);
  OY_FREE( name )

  DBG_PROG_ENDE
  return c;
}



int
oySetProfile_      (const char* name, oyDEFAULT_PROFILE type, const char* comment)
{ DBG_PROG_START
  int r = 1;
  const char *fileName = 0, *com = comment;

  /* extract filename */
  if (name && strrchr(name , OY_SLASH_C))
  {
    fileName = strrchr(name , OY_SLASH_C);
    fileName++;
  } else
    fileName = name;

  DBG_PROG_S( ("name = %s type %d", name, type) )

  if ( name == 0 || !oyCheckProfile_ (fileName, 0) )
  {
    const char* config_name = 0;
    DBG_PROG_S(("set fileName = %s as %d profile\n",fileName, type))
    if ( type < 0 )
      WARN_S( (_("default profile type %d; type does not exist"), type ) );

    if(oyWidgetTitleGet_( type, 0,0,0,0 ) == oyTYPE_DEFAULT_PROFILE)
      config_name = oyOptionGet_(type)-> config_string;
    else if(type == oyDEVICE_PROFILE)
      {
        int len = strlen(OY_REGISTRED_PROFILES)
                  + strlen(fileName);
        char* keyName = (char*) calloc (len +10, sizeof(char)); DBG_PROG
        sprintf (keyName, "%s%s", OY_REGISTRED_PROFILES OY_SLASH, fileName); DBG_PROG
        r = oyAddKey_valueComment_ (keyName, com, 0); DBG_PROG
        DBG_PROG_S(( "%s %d", keyName, len ))
        OY_FREE (keyName)
      }
      else
        WARN_S( (_("default profile type %d; type does not exist"), type ) );
      
    
    if(config_name)
    {
      if(name) {
        r = oyAddKey_valueComment_ (config_name, fileName, com);
        DBG_PROG_S(( "%s %s %s",config_name,fileName,com?com:"" ))
      } else {
        KeySet* list;
        Key *current;
        char* value = (char*) calloc (sizeof(char), MAX_PATH);
        int rc = 0;

        DBG_PROG

        list = oyReturnChildrenList_(OY_KEY OY_SLASH "default", &rc ); ERR
        if(!list)
        {
          FOR_EACH_IN_KDBKEYSET( current, list )
          {
            keyGetName(current, value, MAX_PATH);
            DBG_NUM_S(( value ))
            if(strstr(value, config_name) != 0 &&
               strlen(value) == strlen(config_name))
            {
              DBG_PROG_S((value))
              kdbRemove ( oy_handle_, value );
              break;
            }
          }
        }

        DBG_NUM_S(( value ))

        OY_FREE (list) DBG_PROG
        OY_FREE (value) DBG_PROG
        oyClose_(); DBG_PROG
        r = rc;
      }
      DBG_PROG_V( r )
    }
  }

  DBG_PROG_ENDE
  return r;
}


/* public API implementation */

/* path names API */


int
oyPathsCount_ ()
{ DBG_PROG_START
  int rc=0;
  ssize_t n = 0;

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_PATHS, &rc ); ERR
  if(!myKeySet)
  {
    oyClose_();
    DBG_PROG
    DBG_PROG_ENDE
    return n;
  }

  //if(!rc)
    n = ksGetSize(myKeySet);

  ksClose (myKeySet);
  oyClose_();
  OY_FREE(myKeySet)

  DBG_PROG_ENDE
  return (int)n;
}

char*
oyPathName_ (int number, oyAllocFunc_t allocate_func)
{ DBG_PROG_START
  int rc=0, n = 0;
  Key *current;
  char* value;

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_PATHS, &rc ); ERR
  if(!myKeySet)
  {
    oyClose_();
    DBG_PROG_ENDE
    return 0;
  }

  value = (char*) allocate_func( MAX_PATH );

  if (number <= (int)ksGetSize(myKeySet))
    FOR_EACH_IN_KDBKEYSET( current, myKeySet )
    {
      if (number == n) {
        keyGetComment (current, value, MAX_PATH);
        if (strstr(value, OY_SLEEP) == 0)
          keyGetString(current, value, MAX_PATH);
      }
      n++;
    }

  ksClose (myKeySet);
  oyClose_();
  OY_FREE(myKeySet)

  DBG_PROG_ENDE
  return value;
}

int
oyPathAdd_ (const char* pfad)
{
  DBG_PROG_START
  int rc=0, n = 0;
  Key *current, *checker;
  int current_pos = 0, checker_pos = 0;
  char* keyName = NULL;
  char* value = NULL, *check = NULL;
  int has_local_path = 0, has_global_path = 0,
      has_config_local_path = 0, has_config_global_path = 0;
  KeySet* myKeySet = NULL;
  KeySet* checkKeySet = NULL;
  int *remove_keys = NULL;

  const char *config_user_path = OY_PROFILE_PATH_USER_DEFAULT;
  const char *config_system_path = OY_PROFILE_PATH_SYSTEM_DEFAULT;
  const char *config_local_path = USERCOLORDIR OY_SLASH ICCDIRNAME;
  const char *config_global_path = SYSCOLORDIR OY_SLASH ICCDIRNAME;

  /* add path */
  /* search for empty keyname */
  keyName = oySearchEmptyKeyname_ (OY_PATHS, OY_PATH);

  /* write key */
  rc = oyAddKey_valueComment_ (keyName, pfad, "");

  /* take all keys in the paths directory */
  myKeySet = oyReturnChildrenList_(OY_PATHS, &rc ); ERR
  checkKeySet = oyReturnChildrenList_(OY_PATHS, &rc ); ERR

  n = ksGetSize(myKeySet);
  remove_keys = (int*) calloc(sizeof(int), n);

  if(!myKeySet)
    goto finish;

  if(pfad)
    DBG_PROG_S(( pfad ));

  keyName = (char*) calloc (sizeof(char), MAX_PATH);
  value = (char*) calloc (sizeof(char), MAX_PATH);
  check = (char*) calloc (sizeof(char), MAX_PATH);

  /* search for allready included path */
  DBG_PROG_S(( "path items: %d", (int)ksGetSize(myKeySet) ))
  FOR_EACH_IN_KDBKEYSET( current, myKeySet )
  {
    keyGetString(current, value, MAX_PATH);
    keyGetFullName(current, keyName, MAX_PATH);
    if (value) DBG_PROG_S(( value ));

    /* Are the default paths allready there? */
    if (strcmp(value, config_user_path) == 0) has_local_path = 1;
    if (strcmp(value, config_system_path) == 0) has_global_path = 1;
    if (strcmp(value, config_local_path) == 0) has_config_local_path = 1;
    if (strcmp(value, config_global_path) == 0) has_config_global_path = 1;

    checker_pos = 0;
    /* erase double occurencies of this path */
    FOR_EACH_IN_KDBKEYSET( checker, checkKeySet )
    {
      rc=keyGetString(checker,check, MAX_PATH); ERR

      if (current_pos < checker_pos &&
          strcmp (value, check) == 0 )
        remove_keys[checker_pos] = 1;

      ++checker_pos;
    }
    ++current_pos;
  }

  /* remove double keys */
  current_pos = 0;
  FOR_EACH_IN_KDBKEYSET( current, myKeySet )
  {
    if(remove_keys[current_pos])
    {
      rc=keyGetFullName(current,keyName, MAX_PATH); ERR
      rc=kdbRemove( oy_handle_, keyName ); ERR
      DBG_PROG_S(( "erase path key : %s %s", value, keyName ));
    }
    ++current_pos;
  }

  finish:
  if (myKeySet) ksClose (myKeySet);
  if (myKeySet) ksDel (myKeySet);
  if (checkKeySet) ksClose (checkKeySet);
  if (checkKeySet) ksDel (checkKeySet);

  if (!has_global_path)
  {
    keyName = oySearchEmptyKeyname_ (OY_PATHS, OY_PATH);
    rc = oyAddKey_valueComment_ (keyName, config_system_path, "");
  }
  if (!has_local_path)
  {
    keyName = oySearchEmptyKeyname_ (OY_PATHS, OY_PATH);
    rc = oyAddKey_valueComment_ (keyName, config_user_path, "");
  }
  if (!has_config_local_path &&
      !oyKeySetHasValue_( OY_PATHS, config_local_path) )
  {
    keyName = oySearchEmptyKeyname_ (OY_PATHS, OY_PATH);
    rc = oyAddKey_valueComment_ (keyName, config_local_path, "");
  }
  if (!has_config_global_path &&
      !oyKeySetHasValue_( OY_PATHS, config_global_path) )
  {
    keyName = oySearchEmptyKeyname_ (OY_PATHS, OY_PATH);
    rc = oyAddKey_valueComment_ (keyName, config_global_path, "");
  }


  oyClose_();
  OY_FREE (keyName)
  OY_FREE (value)
  OY_FREE (check)
  OY_FREE (remove_keys)

  oyCheckDefaultDirectories_();

  DBG_PROG_ENDE
  return rc;
}

void
oyPathRemove_ (const char* pfad)
{ DBG_PROG_START
  int rc=0;
  Key *current;
  char* value;
  char* keyName;

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_PATHS, &rc ); ERR
  if(!myKeySet)
  {
    oyPathAdd_ (OY_PROFILE_PATH_USER_DEFAULT);
    oyClose_();
    DBG_PROG_ENDE
    return;
  }

  value = (char*) calloc (sizeof(char), MAX_PATH);
  keyName = (char*) calloc (sizeof(char), MAX_PATH);

  /* compare and erase if matches */
  FOR_EACH_IN_KDBKEYSET( current, myKeySet )
  {
    keyGetString(current, value, MAX_PATH);
    if (strcmp (value, pfad) == 0)
    {
      keyGetFullName(current,keyName, MAX_PATH); ERR
      kdbRemove ( oy_handle_, keyName );
      DBG_NUM_S(( "remove" ))
    }
  }

  ksClose (myKeySet);

  /* after remove blindly add seeing */
  oyPathAdd_ (OY_PROFILE_PATH_USER_DEFAULT);

  oyClose_();
  OY_FREE(myKeySet)
  OY_FREE(keyName)
  OY_FREE(value)


  DBG_PROG_ENDE
}

void
oyPathSleep_ (const char* pfad)
{ DBG_PROG_START
  int rc=0;
  Key *current;
  char* value;

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_PATHS, &rc ); ERR
  if(!myKeySet) {
    oyClose_();

    DBG_PROG_ENDE
    return;
  }

  value = (char*) calloc (sizeof(char), MAX_PATH);

  /* set "SLEEP" in comment */
  FOR_EACH_IN_KDBKEYSET( current, myKeySet )
  {
    keyGetString(current, value, MAX_PATH);
    if (strcmp (value, pfad) == 0)
    {
      keySetComment (current, OY_SLEEP);
      kdbSetKey ( oy_handle_, current );
      DBG_NUM_S(( "sleep" ))
    }
  }

  ksClose (myKeySet);
  oyClose_();
  OY_FREE(myKeySet)
  OY_FREE (value);
  DBG_PROG_ENDE
}

void
oyPathActivate_ (const char* pfad)
{ DBG_PROG_START
  int rc=0;
  Key *current;
  char* value;

  /* take all keys in the paths directory */
  KeySet* myKeySet = oyReturnChildrenList_(OY_PATHS, &rc ); ERR
  if(!myKeySet) {
    oyClose_();

    DBG_PROG_ENDE
    return;
  }

  value = (char*) calloc (sizeof(char), MAX_PATH);

  /* erase "SLEEP" from comment */
  FOR_EACH_IN_KDBKEYSET( current, myKeySet )
  {
    keyGetString(current, value, MAX_PATH);
    if (strcmp (value, pfad) == 0)
    {
      keySetComment (current, "");
      kdbSetKey ( oy_handle_, current );
      DBG_NUM_S(( "wake up" ))
    }
  }

  ksClose (myKeySet);
  oyClose_();
  OY_FREE(myKeySet)
  OY_FREE (value);
  DBG_PROG_ENDE
}

/**@brief read Key value
 *
 *  1. ask user
 *  2. if user has no setting ask system
 */
char*
oyGetKeyValue_ ( const char       *key_name,
                 oyAllocFunc_t     allocFunc )
{
  char* name = 0;
  char* full_key_name = 0;
  int rc = 0;

  if( !key_name || strlen( key_name ) > MAX_PATH-1 )
  { WARN_S(("wrong string format given"));
    return 0;
  }

  name = (char*) allocFunc (MAX_PATH);
  full_key_name = (char*) oyAllocateFunc_ (MAX_PATH);

  if( !name || !full_key_name )
    return 0;

  sprintf( full_key_name, "%s%s", OY_USER, key_name );

  name[0] = 0;
  rc = kdbGetValue ( oy_handle_, full_key_name, name, MAX_PATH );

  if( rc != KDB_RET_OK || !strlen( name ))
  {
    sprintf( full_key_name, "%s%s", OY_SYS, key_name );
    rc = kdbGetValue ( oy_handle_, full_key_name, name, MAX_PATH );
  }

  free( full_key_name );

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  if(rc == KDB_RET_OK)
    return name;
  else
    return 0;
}


/* device profiles API */

char*
oyGetDeviceProfile_                (const char* manufacturer,
                                    const char* model,
                                    const char* product_id,
                                    const char* host,
                                    const char* port,
                                    const char* attrib1,
                                    const char* attrib2,
                                    const char* attrib3,
                                    oyAllocFunc_t allocate_func)
{ DBG_PROG_START
  DBG_PROG
  char* profileName = 0;
  int rc=0;

  oyComp_t_ *matchList = 0,
         *testEntry = 0,
         *foundEntry = 0;
  KeySet *profilesList;

  profilesList = 
   oyReturnChildrenList_(OY_REGISTRED_PROFILES, &rc ); ERR

  if(!profilesList) {
    oyClose_();

    DBG_PROG_ENDE
    return profileName;
  }

  matchList = oyGetDeviceProfile_sList (manufacturer, model, product_id,
                                        host, port, attrib1, attrib2, attrib3,
                                        profilesList, rc);

  /* 6. select the profile from the match list with the most hits */
  if (matchList)
  {
    int max_hits = 0;
    int count = 0;
    foundEntry = 0;
    DBG_PROG_S(( "matchList->begin->next: %d\n", (int)(intptr_t)matchList->begin->next ))
    for (testEntry=matchList->begin; testEntry; testEntry=testEntry->next)
    {
      DBG_PROG_S(( "testEntry %d count: %d\n", (int)(intptr_t)testEntry, count++ ))
      if (testEntry->hits > max_hits)
      {
        foundEntry = testEntry;
        max_hits = testEntry->hits;
      }
    }
    if(foundEntry) DBG_PROG_S ((printComp (foundEntry)));

    /* 7. tell about the profile and its hits */
    if(foundEntry)
    {
      char *fileName = 0;
      if (foundEntry->name)
        DBG_PROG_S(("%s\n",foundEntry->name) );
      if (foundEntry->name &&
          strlen(foundEntry->name) &&
          strrchr(foundEntry->name , OY_SLASH_C))
      {
        fileName = strrchr(foundEntry->name , OY_SLASH_C);
        fileName++;
      }
      else
        fileName = foundEntry->name;

      int len = strlen (fileName)+1;
      profileName = (char*) allocate_func (len);
      sprintf (profileName, fileName);

      DBG_PROG_S((foundEntry->name))
      DBG_PROG_S((profileName))
      oyDestroyCompList_ (matchList);
    }
  }

  ksClose (profilesList);
  oyClose_();
  OY_FREE(profilesList)

  DBG_PROG_ENDE
  return profileName;
}

#if 0
char**
oyGetDeviceProfile_s               (const char* manufacturer,
                                    const char* model,
                                    const char* product_id,
                                    const char* host,
                                    const char* port,
                                    const char* attrib1,
                                    const char* attrib2,
                                    const char* attrib3,
                                    int** number)
{ DBG_PROG_START
  char** profileNames = 0;
  char*  profileName = 0;
  int    rc;

  oyComp_t_ *matchList = 0,
         *testEntry = 0,
         *foundEntry = 0;
  KeySet* profilesList;

  kdbOpen();

  // TODO merge User and System KeySets in oyReturnChildrenList_
  profilesList = oyReturnChildrenList_(OY_USER OY_REGISTRED_PROFILES, &rc ); ERR
  if(!profilesList) {
    oyClose_();

    DBG_PROG_ENDE
    return profileNames;
  }

  matchList = oyGetDeviceProfile_sList (manufacturer, model, product_id,
                                        host, port, attrib1, attrib2, attrib3,
                                        profilesList, rc);

  /* 6. select the profile from the match list with the most hits */
  if (matchList)
  {
    int max_hits = 0;
    foundEntry = 0;
    for (testEntry=matchList->begin; testEntry; testEntry=testEntry->next)
    {
      if (testEntry->hits > max_hits)
      {
        foundEntry = testEntry;
        max_hits = testEntry->hits;
      }
    }
    if(foundEntry) DBG_PROG_S ((printComp (foundEntry)))

    /* 7. tell about the profile and its hits */
    {
      char *fileName = 0;

      if (strrchr(foundEntry->name , OY_SLASH_C))
      {
        fileName = strrchr(foundEntry->name , OY_SLASH_C);
        fileName++;
      }
      else
        fileName = foundEntry->name;

      profileName = (char*) calloc (strlen (fileName)+1, sizeof(char));
      sprintf (profileName, fileName);
      // @TODO add profileName to profileNames

      DBG_PROG_S((foundEntry->name))
      DBG_PROG_S((profileName))
      oyDestroyCompList_ (matchList);
    }
  }

  ksClose (profilesList);
  kdbClose();
  OY_FREE(profilesList)

  DBG_PROG_ENDE
  return profileNames;
}
#endif

  /**
   * @internal
   * Search description
   *
   * This routine describes the A approach
   *   - registred profiles with assigned devices
   *
   * -# take all arguments and walk through the named devices list \n
   *    //  named devices consist of an key with the profile name + attributes\n
   *    //  it is not allowed to have two profiles with the same name\n
   *    //  it is allowed to have different profiles for the same attribute :(\n
   *    //  specify more attributes to make an decission presumable\n
   *    //   or maintain profiles, erasing older and invalid ones
   * -# test if attributes matches the value of the key, count the hits
   * -# search the profile in an match list
   * -# add the profile to the match list if not found
   * -# increase the hits counter in the macht list for that profile
   * -# select the profile from the match list with the most hits
   * -# tell about the profile and its hits
   *
   * @todo approach B:\n
   * no attributes are assigned beside certain keyword ("monitor", "scanner")\n
   * scan profile tags for manufacturer, device descriptions ... \n<ul>
   * <li> When to start an automatic registration run?</li>\n
   * <li> include profile tag editing?</li>
   * </ul>
   * @todo other things:\n
   * <ul>
   * <li> spread weighting? 3 degrees are sufficient How to merge in the on
   *       string approach?</li></ul>
   *
   */


oyComp_t_*
oyGetDeviceProfile_sList           (const char* manufacturer,
                                    const char* model,
                                    const char* product_id,
                                    const char* host,
                                    const char* port,
                                    const char* attrib1,
                                    const char* attrib2,
                                    const char* attrib3,
                                    KeySet *profilesList,
                                    int   rc)
{ DBG_PROG_START
  /* 1. take all arguments and walk through the named devices list */
  int i = 0, n = 0;
  char* name  = (char*) calloc (MAX_PATH, sizeof(char));
  char* value = (char*) calloc (MAX_PATH, sizeof(char));
  const char **attributs = (const char**) alloca (8 * sizeof (const char*));
  Key *current;
  oyComp_t_ *matchList = 0,
         *testEntry = 0;

  attributs[0] = manufacturer;
  attributs[1] = model;
  attributs[2] = product_id;
  attributs[3] = host;
  attributs[4] = port;
  attributs[5] = attrib1;
  attributs[6] = attrib2;
  attributs[7] = attrib3;

# if 1
  for (i = 0; i <= 7; ++i)
    DBG_PROG_S (("%ld %ld", (long int)attributs[i], (long int)model));
# endif


  if (profilesList)
  {
    FOR_EACH_IN_KDBKEYSET( current, profilesList )
    {
      n = 0;
      keyGetString (current, value, MAX_PATH);
      keyGetName   (current, name,  MAX_PATH);

      /* 2. test if attributes matches the value of the key, count the hits */
      for (i = 0; i < 8; i++)
      {
        DBG_PROG_S (("%d: %s", i, attributs[i]))
        if (value && attributs[i] &&
            (strstr(value, attributs[i]) != 0))
        {
          if      (i == 0) n += 2;
          else if (i == 1) n += 2;
          else if (i == 2) n += 5;
          else             ++n;
          DBG_PROG_S(( "attribute count n = %d", n ));
        }
      }

      if (n >= 5)
      { /* 3. search the profile in an match list */
        int found = 0; DBG_PROG
        if (matchList)
        {
          for (testEntry=matchList->begin; testEntry; testEntry=testEntry->next)
          {
            DBG_PROG_S(( "%s %s", testEntry->name, name ))
            if (testEntry->name && strlen(name) &&
                strstr(testEntry->name, name) != 0)
            { DBG_PROG_S(( "%s", strstr(testEntry->name, name) ))
              found = 1;
              WARN_S(("double occurency of profile %s", testEntry->name))
              /* anyway increase the hits counter if attributes fits better */
              if (testEntry->hits < n)
                testEntry->hits = n;
            }
          }
        }
        /* 4. add the profile to the match list if not found (normal case) */
        if (!found)
        {
          DBG_PROG_S(( "new matching profile found %s", name ))
          matchList = oyAppendComp_ (matchList, 0);
          DBG_PROG_S ((printComp (matchList)))
          /* 5. increase the hits counter in the match list for that profile */
          oySetComp_ ( matchList, name, value, n );
          DBG_PROG_S ((printComp (matchList)))
        }
      }
    }
  } else
    WARN_S (("No profiles yet registred to devices"))

  DBG_PROG_ENDE
  return matchList;
}


int
oyEraseDeviceProfile_              (const char* manufacturer,
                                    const char* model,
                                    const char* product_id,
                                    const char* host,
                                    const char* port,
                                    const char* attrib1,
                                    const char* attrib2,
                                    const char* attrib3)
{ DBG_PROG_START
  DBG_PROG
  char* profile_name = 0;
  int rc=0;
  KeySet* profilesList = 0;
  Key *current;
  char* value;

  DBG_PROG

  profilesList =
   oyReturnChildrenList_(OY_REGISTRED_PROFILES, &rc ); ERR
  if(!profilesList)
  {
    oyClose_();

    DBG_PROG_ENDE
    return rc;
  }

  value = (char*) calloc (sizeof(char), MAX_PATH);
  profile_name = oyGetDeviceProfile_ (manufacturer, model, product_id,
                                      host, port, attrib1, attrib2, attrib3,
                                      oyAllocateFunc_);

  DBG_PROG_S(("profile_name %s", profile_name ))

  FOR_EACH_IN_KDBKEYSET( current, profilesList )
  {
    keyGetName(current, value, MAX_PATH);
    DBG_NUM_S(( value ))
    if(profile_name &&
       strstr(value, profile_name) != 0) {
      DBG_PROG_S((value))
      kdbRemove ( oy_handle_, value ); 
      break;
    }
  }

  DBG_NUM_S(( value ))

  if(profilesList) ksClose(profilesList); DBG_PROG
  OY_FREE (value) DBG_PROG
  OY_FREE (profile_name) DBG_PROG
  oyClose_(); DBG_PROG

  DBG_PROG_ENDE
  return rc;
}



