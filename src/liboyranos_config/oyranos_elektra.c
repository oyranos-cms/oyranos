/** @file oyranos_elektra.c
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
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */

#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <kdb.h>

#include "oyOptions_s.h"
#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_check.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_io.h"
#include "oyranos_internal.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_types.h"
#include "oyranos_xml.h"

#define DBG_EL1_S DBG_MEM1_S
#define DBG_EL2_S DBG_MEM2_S
#define DBG_EL_S DBG_MEM_S

#ifndef KDB_VERSION_MAJOR
#define KDB_VERSION_MAJOR 0
#endif
#ifndef KDB_VERSION_MINOR
#define KDB_VERSION_MINOR 0
#endif
#define KDB_VERSION_NUM (KDB_VERSION_MAJOR*10000 + KDB_VERSION_MINOR*100)

#if KDB_VERSION_NUM >= 800
#define kdbGetString_m kdbGetString
#define kdbGetChildKeys(a,b,c,d) oyGetByName(c,b)
#define kdbGetKey(a,b) oyGetKey(a,b)
#define kdbSetKey(a,b) oySetKey(b)
#define oyKdbRemove(a,b,c) oyRemoveFromDB(b,c)
#define ksAppendKeys ksAppend
#define keyIsDir(a) 0
#define keyRemove(a) 0
#define keySetComment(a,b) keySetMeta(a,"comment",b)

#elif KDB_VERSION_NUM >= 700
#define kdbGetString_m kdbGetString
#define kdbGetChildKeys(a,b,c,d) kdbGetByName(a,c,b,d)
#define ksAppendKeys ksAppend

#else
#define kdbGetString_m kdbGetValue
#define oyKdbRemove(a,b,c) kdbRemove(a,b)
#endif

char oy_elektra_error_text[24] = {0};

char * oy__kdbStrError(int rc) { sprintf(oy_elektra_error_text, "elektra: %d", rc);
                                 return oy_elektra_error_text; }

#if KDB_VERSION_NUM > 600
#define kdbStrError(t) oy__kdbStrError(t)
#endif

#if KDB_VERSION_NUM >= 800
#define oyERRopen(k) { const Key *meta = NULL; keyRewindMeta(k); \
                   if(!oy_handle_) { \
                     while((meta = keyNextMeta(k)) != 0) { \
                       if(oy_debug || strstr(oyNoEmptyString_m_( keyName(meta) ),"warnings") == 0) \
                         WARNc2_S( "%s:\t%s", \
                                   oyNoEmptyString_m_( keyName(meta) ), \
                                   oyNoEmptyString_m_( keyString(meta) ) ); \
                     } \
                 } }
#define oyERR(k) { const Key *meta = NULL; keyRewindMeta(k); \
                   if(rc < 0) { \
                     while((meta = keyNextMeta(k)) != 0) { \
                       if(oy_debug || strstr(oyNoEmptyString_m_( keyName(meta) ),"warnings") == 0) \
                         WARNc3_S( "rc:%d %s:\t%s", rc, \
                                   oyNoEmptyString_m_( keyName(meta) ), \
                                   oyNoEmptyString_m_( keyString(meta) ) ); \
                     } \
                 } }
#else
#define oyERR(k)
#endif

/* --- Helpers  --- */


/* test2.cpp references variables for testing */
#if DEBUG_NEVER
#define STATIC_IF_NDEBUG static
#else
#define STATIC_IF_NDEBUG
#endif

#define AD oyAllocateFunc_, oyDeAllocateFunc_

/* --- static variables   --- */

STATIC_IF_NDEBUG int oyranos_init = 0;
#if KDB_VERSION_NUM >= 800
#else
STATIC_IF_NDEBUG KeySet * oy_config_ = 0;
STATIC_IF_NDEBUG KDB * oy_handle_ = 0;
#endif

/* --- structs, typedefs, enums --- */


/* --- internal API definition --- */

const char * oyGetScopeString_       ( oySCOPE_e           scope );

char *   oyDBGetStringFast_          ( KDB               * oy_handle_,
                                       const char        * key_name,
                                       oySCOPE_e           scope,
                                       oyAlloc_f           allocate_func );

/* elektra key list handling */
KeySet* oyReturnChildrenList_ (const char* keyParentName, oySCOPE_e scope, int* rc_ptr);



/* separate from the external functions */


#define oyDEVICE_PROFILE oyDEFAULT_PROFILE_END

void oyOpen_ (void)
{
  if(!oyranos_init) {
	  DBG_EL_S ( "______________\n");
	  DBG_EL1_S ("v%d _________\n", KDB_VERSION_NUM);
#if KDB_VERSION_NUM >= 800
#else
    oy_config_ = ksNew(0,NULL);
    oy_handle_ = kdbOpen( /*&oy_handle_*/ );
	  DBG_EL1_S ( "______________ %p\n", oy_handle_);
    DBG_EL1_S("oy_handle_ = kdbOpen() == %s", oy_handle_ ? "good":"NULL");
    if(!oy_handle_)
      WARNc_S("Could not initialise Elektra.");
#endif
    oyranos_init = 1;
  }
}
void oyClose_() { /*kdbClose( &oy_handle_ );*/ }
/* @todo make oyOpen unnecessary */
void oyDBOpen  (void) { oyOpen_(); }
void oyDBClose (void) { oyClose_(); }
void oyCloseReal__() {
#if KDB_VERSION_NUM >= 800
#else
  kdbClose( oy_handle_ ); oyERR(error_key)
  ksDel(oy_config_);
  oy_config_ = 0;
  oy_handle_ = 0;
#endif
  oyranos_init = 0;
}

/* oyranos part */



/* small helpers */

  /* ksNext uses the same entry twice in a 1 component KeySet, we avoid this */
#define FOR_EACH_IN_KDBKEYSET( current_, list ) \
   ksRewind( list );  \
   for( current_ = ksNext( list ); current_; current_ = ksNext( list )  )


#if KDB_VERSION_NUM >= 800
int oyGetByName(KeySet * ks, const char * key_name)
{
  Key * error_key = keyNew(KEY_END);
  KDB * oy_handle_ = kdbOpen(error_key); oyERRopen(error_key)
  Key * top =  keyNew(key_name, KEY_END);

  int rc = kdbGet(oy_handle_, ks, top); oyERR(top)

  keyDel(top);
  kdbClose(oy_handle_, error_key);
  keyDel(error_key);

  return rc;
}

int  oyGetKey                        ( KDB               * oy_handle_,
                                       Key               * key )
{
  Key * result;
  int rc;
  KeySet * oy_config_ = ksNew(0,NULL);

  DBG_EL1_S( "xxxxxxxxxxxx Try to get %s\n", keyName(key));

  DBG_EL1_S( "oy_config_ == %s", oy_config_ ? "good":"NULL" );
  rc = kdbGet( oy_handle_, oy_config_, key ); oyERR(key)
  DBG_EL1_S( "rc = kdbGet( oy_handle_, oy_config_, key ) == %d", rc );

  result = ksLookup( oy_config_, key, KDB_O_NONE);
  DBG_EL1_S( "result = ksLookup( oy_config_, key, KDB_O_NONE) == %s", result ? "good":"NULL" );
  if(rc < 0 && !result)
  {
    WARNc1_S( "keyString(key) == %s", oyNoEmptyString_m_(keyString(key)) );
    oyERR(key)
  }
  else
  {
    rc = 0;
  }
  DBG_EL1_S ( "xxxxxxxxxxxx Got %s\n", keyString(result));
  DBG_EL_S( "keyCopy( key, result )" );
  keyCopy( key, result );

  keyDel( result );
  DBG_EL_S( "keyDel( result )" );
  ksDel(oy_config_);
  return rc;
}

int  oySetKey                        ( Key               * key )
{
  int rc;
  Key * error_key = keyNew(KEY_END);
  KDB * oy_handle_ = kdbOpen(error_key); oyERRopen(error_key)
  KeySet * oy_config_ = ksNew(0, KS_END);
  const char * key_name = keyName(key);
  Key * parent_key = keyNew( key_name, KEY_END );

  DBG_EL1_S( "oy_config_ == %s", oy_config_ ? "good":"NULL" );
  rc = kdbGet( oy_handle_, oy_config_, parent_key ); oyERR(key)
  DBG_EL1_S( "kdbGet( oy_handle_, oy_config_, key ) = %d", rc );
  if(rc >= 0)
  {
    Key * dup = keyDup (key);
    //keyNeedSync(dup);
    ksAppendKey(oy_config_, dup);
    rc = kdbSet( oy_handle_, oy_config_, parent_key ); oyERR(key)
    DBG_EL1_S( "kdbSet( oy_handle_, oy_config_, key ) = %d", rc );
  }
  kdbClose( oy_handle_,error_key ); oyERR(error_key)
  keyDel(error_key);
  keyDel(parent_key);
  ksDel(oy_config_);
  return rc;
}

int      oyRemoveFromDB              ( const char        * name,
                                       oySCOPE_e           scope )
{
  return oyDBEraseKey_(name, scope);
}
#endif /* KDB_VERSION_NUM >= 800 */

/* --- function definitions --- */


struct oyDB_s {
  KDB      * h;
  Key      * error;
  KeySet   * ks;
  char     * top_key_name;
  oyAlloc_f  alloc;
  oySCOPE_e  scope;
};

void     oyDB_printWarn              ( oyDB_s            * db )
{
  const Key * meta = NULL;
  Key * k = db->error;

  keyRewindMeta(k);
  while((meta = keyNextMeta(k)) != 0)
  {
    if(oy_debug || strstr(oyNoEmptyString_m_( keyName(meta) ),"warnings") == 0)
      WARNc2_S( "%s:\t%s",
                oyNoEmptyString_m_( keyName(meta) ),
                oyNoEmptyString_m_( keyString(meta) ) );
  }
}

int      oyDB_GetChildren            ( oyDB_s            * db )
{
  int rc = 0;
  KeySet*list_user = NULL;
  KeySet*list_sys = NULL;
  char  *list_name_user = NULL;
  char  *list_name_sys = NULL;

  DBG_PROG_START

  if(db->ks)
    WARNc_S("please use only one call to oyDB_GetChildren")
  else
    db->ks = ksNew(0,NULL);

  if( db->scope == oySCOPE_USER_SYS || db->scope == oySCOPE_USER )
  {
    list_user = ksNew(0,NULL);
    oyStringAddPrintf( &list_name_user, AD, "%s%s", OY_USER, db->top_key_name );
    if(!db->h)
      return 0;

    rc =
      kdbGetChildKeys( db->h, list_name_user, list_user, KDB_O_SORT);
    if(rc < 0)
      oyDB_printWarn(db);

    if(rc > 0)
      DBG_NUM1_S("kdbGetChildKeys returned with %d", rc);
  }

  if( db->scope == oySCOPE_USER_SYS || db->scope == oySCOPE_SYSTEM )
  {
    list_sys = ksNew(0,NULL);
    oyStringAddPrintf( &list_name_sys, AD, "%s%s", OY_SYS, db->top_key_name );
    if(!db->h)
      return 0;

    rc =
      kdbGetChildKeys( db->h, list_name_sys, list_sys, KDB_O_SORT);
    if(rc < 0)
      oyDB_printWarn(db);

    if(rc > 0)
      DBG_NUM1_S("kdbGetChildKeys returned with %d", rc);
  }

  if(list_user)
  {
    ksAppendKeys( db->ks, list_user );
    ksDel( list_user );
  }
  if(list_sys)
  {
    ksAppendKeys( db->ks, list_sys );
    ksDel( list_sys );
  }

  oyFree_m_( list_name_user )
  oyFree_m_( list_name_sys )

  DBG_PROG_ENDE
  return rc;
}


oyDB_s * oyDB_newFrom                ( const char        * top_key_name,
                                       oySCOPE_e           scope,
                                       oyAlloc_f           allocFunc )
{
  oyDB_s * db = calloc( sizeof(oyDB_s), 1 );

  if(db)
  {
    db->error = keyNew(KEY_END);
    db->h = kdbOpen(db->error);
    if(!db->h) oyDB_printWarn( db );
    db->top_key_name = oyStringCopy( top_key_name, oyAllocateFunc_ );
    db->ks = NULL;
    db->alloc = allocFunc;
    db->scope = scope;
  }

  return db;
}

void     oyDB_release                ( oyDB_s           ** db )
{
  oyDB_s * s = *db;

  if(!s)
    return;

  kdbClose( s->h, s->error );
  s->h = NULL;
  keyDel(s->error);
  s->error = NULL;
  if(s->top_key_name)
    oyFree_m_( s->top_key_name );
  if(s->ks)
    ksDel( s->ks );
  s->ks = NULL;
  s->alloc = 0;
  oyDeAllocateFunc_( s );
  *db = NULL;
}

char *   oyDB_getString              ( oyDB_s            * db,
                                       const char        * key_name )
{
  int error = !db;
  const char * current_name;
  KeySet * my_key_set = NULL;
  Key * current = NULL;
  const char *name = NULL;
  char * value = NULL;

  DBG_PROG_START

  if(!error)
    name = key_name;

  if(!db->ks)
    oyDB_GetChildren( db );

  if(!error)
    my_key_set = db->ks;

  if(my_key_set)
  {
    FOR_EACH_IN_KDBKEYSET( current, my_key_set )
    {
      current_name = keyName(current);
      if(current_name &&
         oyStrstr_(current_name, name) )
      {
        const char * t = oyStrstr_(current_name, name);

        if(t && strcmp(t,name) == 0)
        {
          t = keyString( current );
          if(t)
            value = oyStringCopy( t, db->alloc );
          break;
        }
      }
    }
  }

  DBG_PROG_ENDE
  return value;
}


KeySet* oyReturnChildrenList_ (const char* keyParentName, oySCOPE_e scope, int* rc_ptr)
{
  int rc = 0;
  KeySet*list_user = 0;
  KeySet*list_sys = 0;
  KeySet*list = ksNew(0,NULL);
  char  *list_name_user = NULL;
  char  *list_name_sys = NULL;
#if KDB_VERSION_NUM >= 800
  Key * error_key = keyNew(KEY_END);
  KDB * oy_handle_ = kdbOpen(error_key); oyERRopen(error_key)
#endif

  DBG_PROG_START

  if( scope == oySCOPE_USER_SYS || scope == oySCOPE_USER ) {
    list_user = ksNew(0,NULL);
    oyStringAddPrintf( &list_name_user, AD, "%s%s", OY_USER, keyParentName );
    if(!oy_handle_)
    {
      *rc_ptr = 1;
      return 0;
    }
    rc =
      kdbGetChildKeys( oy_handle_, list_name_user, list_user, KDB_O_SORT); oyERR(error_key)

    if(rc > 0)
      DBG_NUM1_S("kdbGetChildKeys returned with %d", rc);
  }
  if( scope == oySCOPE_USER_SYS || scope == oySCOPE_SYSTEM ) {
    list_sys = ksNew(0,NULL);
    oyStringAddPrintf( &list_name_sys, AD, "%s%s", OY_SYS, keyParentName );
    if(!oy_handle_)
    {
      *rc_ptr = 1;
      return 0;
    }
    rc =
      kdbGetChildKeys( oy_handle_, list_name_sys, list_sys, KDB_O_SORT); oyERR(error_key)

    if(rc > 0)
      DBG_NUM1_S("kdbGetChildKeys returned with %d", rc);
  }

  if(list_user)
    ksAppendKeys(list, list_user);
  if(list_sys)
    ksAppendKeys(list, list_sys);

  DBG_PROG_V(( (intptr_t)keyParentName ))
  DBG_PROG_S(( keyParentName ))
  DBG_PROG_V(( (intptr_t)ksGetSize(list) ))

  oyFree_m_( list_name_user )
  oyFree_m_( list_name_sys )

#if KDB_VERSION_NUM >= 800
  kdbClose( oy_handle_,error_key ); oyERR(error_key)
  keyDel(error_key);
#endif
  DBG_PROG_ENDE
  return list;
}

char*    oyDBSearchEmptyKeyname_       ( const char      * key_parent_name,
                                         oySCOPE_e         scope )
{
  char * key_base_name = oyStringCopy( oyGetScopeString_(scope), oyAllocateFunc_ );
  char * new_key_name = NULL;
  int nth = -1, i = 0, rc=0;
  Key *key = 0;
  KeySet * ks = ksNew(0,NULL),
         * cut;
  size_t count;
#if KDB_VERSION_NUM >= 800
  Key * error_key = keyNew(KEY_END);
  KDB * oy_handle_ = kdbOpen(error_key); oyERRopen(error_key)
#endif

  DBG_PROG_START

  oyStringAddPrintf( &key_base_name, AD, "%s", key_parent_name );

  if(!oy_handle_)
    return 0;

  key = keyNew( key_base_name, KEY_END );
  rc = kdbGet( oy_handle_, ks, key ); oyERR(key)

  ksRewind( ks );

  keyDel( key );

  /* search for empty keyname in array */
  while (nth == -1)
  {
    if(new_key_name) oyFree_m_( new_key_name );
    oyStringAddPrintf( &new_key_name, AD, "%s/#%d", key_base_name, i );

    ksRewind( ks );
    key = keyNew( new_key_name, KEY_END );
    cut = ksCut( ks, key );
    count = ksGetSize( cut );
    if(!cut || !count)
      nth = i;
    keyDel( key );
    DBG_PROG3_S("search key = \"%s\" %s %d\n", new_key_name, cut?"found":"not found", (int)ksGetSize(cut) );

    i++;
  }


  ksDel( ks );
#if KDB_VERSION_NUM >= 800
  kdbClose( oy_handle_,error_key ); oyERR(error_key)
  keyDel(error_key);
#endif

  DBG_PROG_ENDE
  return new_key_name;
} 

/** @brief The function returns all keys found under the arguments one. */
char **  oyDB_getKeyNames            ( oyDB_s            * db,
                                       const char        * key_name,
                                       int               * n )
{
  int error = !db || !n;
  const char * current_name = NULL;
  KeySet * my_key_set = NULL;
  Key * current = NULL;
  const char *name = NULL;
  char ** texts = NULL;
  int name_len;

  DBG_PROG_START

  if(!error)
    name = key_name;
  if(n)
    *n = 0;

  if(!db->ks)
    oyDB_GetChildren( db );

  if(!error)
    my_key_set = db->ks;

  if(my_key_set)
  {
    name_len = strlen(name);

    FOR_EACH_IN_KDBKEYSET( current, my_key_set )
    {
      current_name = keyName(current);
      if(current_name &&
         oyStrstr_(current_name, name) )
      {
        const char * t = oyStrstr_(current_name, name);

        if(strlen(t) > name_len &&
           !oyStringListHas_( (const char **)texts, *n, t ) )
          oyStringListAddStaticString_( &texts, n, t,
                                        oyAllocateFunc_, oyDeAllocateFunc_);
      }
    }
  }

  DBG_PROG_ENDE
  return texts;
}

/** @brief The function returns keys found just one level under the arguments one. */
char **  oyDB_getKeyNamesOneLevel    ( oyDB_s            * db,
                                       const char        * key_name,
                                       int               * n )
{
  int error = !db || !n;
  const char * current_name = NULL;
  KeySet * my_key_set = NULL;
  Key * current = NULL;
  const char *name = NULL;
  char ** texts = NULL;
  int name_len;

  DBG_PROG_START

  if(!error)
    name = key_name;
  if(n)
    *n = 0;

  if(!db->ks)
    oyDB_GetChildren( db );

  if(!error)
    my_key_set = db->ks;

  if(my_key_set)
  {
    name_len = strlen(name);

    FOR_EACH_IN_KDBKEYSET( current, my_key_set )
    {
      current_name = keyName(current);
      if(current_name &&
         oyStrstr_(current_name, name) )
      {
        const char * t = oyStrstr_(current_name, name);
        char * txt = NULL, * tmp;
        /** Cut after one level behind key_parent_name. */
        if( oyStrrchr_( &t[oyStrlen_(name)+1], OY_SLASH_C ) )
        {
          txt = oyStringCopy_( t, oyAllocateFunc_ );
          tmp = &txt[oyStrlen_(name)+1];
          tmp = oyStrchr_(tmp, OY_SLASH_C);
          if(tmp)
            tmp[0] = '\000';
          t = txt;
        }

        if(strlen(t) > name_len &&
           !oyStringListHas_( (const char **)texts, *n, t ) )
          oyStringListAddStaticString_( &texts, n, t,
                                        oyAllocateFunc_, oyDeAllocateFunc_);

        if(txt) oyFree_m_(txt);
      }
    }
  }

  DBG_PROG_ENDE
  return texts;
}


int oyDBSetString_ (const char* key_name,
                                       oySCOPE_e           scope,
                        const char* value,
                        const char* comment)
{
#if KDB_VERSION_NUM >= 800
  Key * error_key = keyNew(KEY_END);
  KDB * oy_handle_ = kdbOpen(error_key); oyERRopen(error_key)
#endif
  int rc=0,
      max_len;
  Key *key;
  char *name = NULL;
  char *value_utf8 = NULL;
  char *comment_utf8 = NULL;

  DBG_PROG_START

  oyAllocHelper_m_(value_utf8, char, MAX_PATH, 0,; )
  oyAllocHelper_m_(comment_utf8, char, MAX_PATH, 0,; )

  oyStringAddPrintf( &name, AD, "%s%s", oyGetScopeString_(scope), key_name );
  if(value && oyStrlen_(value))
  {
    max_len = strlen(value) < MAX_PATH ? strlen(value) : MAX_PATH;
    oyIconv( value, max_len, max_len,
             value_utf8, 0, "UTF-8" );
  }
  if(comment && oyStrlen_(comment))
  {
    max_len = strlen(comment) < MAX_PATH ? strlen(comment) : MAX_PATH;
    oyIconv( comment, max_len, max_len,
             comment_utf8, 0, "UTF-8" );
  }

  if (key_name)
    DBG_PROG_S(( key_name ));
  if (value)
    DBG_PROG_S(( value ));
  if (comment)
    DBG_PROG_S(( comment ));
  if (!key_name || !strlen(key_name))
    WARNc_S( "no key_name given" );

  key = keyNew( KEY_END );
  DBG_EL1_S( "key = keyNew( KEY_END ) == %s", key ? "good":"NULL" );
  keySetName( key, name );
  DBG_EL1_S( "keySetName( key, \"%s\" )", name );

  if(!oy_handle_)
    goto clean;

  if(value)
  {
    rc=keySetString (key, value_utf8);
    DBG_EL2_S( "rc = keySetString( key, \"%s\" ) == %d", value_utf8, rc);
    if(rc <= 0)
      oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_"code:%d %s name:%s value:%s",
                       OY_DBG_ARGS_, rc, kdbStrError(rc), name, value);
  }
  if(comment)
  {
    rc=keySetComment (key, comment_utf8);
    DBG_EL2_S( "rc = keySetComment( key, \"%s\" ) == %d", comment_utf8, rc);
    if(rc <= 0)
      oyMessageFunc_p( oyMSG_WARN, 0, OY_DBG_FORMAT_"code:%d %s name:%s comment:%s",
                       OY_DBG_ARGS_, rc, kdbStrError(rc), name, comment);
  }

  rc=kdbSetKey( oy_handle_, key ); oyERR(key)
  DBG_EL1_S( "rc = kdbSetKey( oy_handle, key ) == %d", rc );
  if(rc < 0)
    oyMessageFunc_p( oyMSG_WARN, 0, OY_DBG_FORMAT_ "code:%d %s name:%s",
                     OY_DBG_ARGS_, rc, kdbStrError(rc), name);
  keyDel( key );
  DBG_EL_S( "keyDel( key )" );


  clean:
  oyFree_m_( name )
  oyFree_m_( value_utf8 )
  oyFree_m_( comment_utf8 )
#if KDB_VERSION_NUM >= 800
  kdbClose( oy_handle_,error_key ); oyERR(error_key)
  keyDel(error_key);
  DBG_PROG_ENDE
  return rc < 0 ? 1 : 0;
#endif

  DBG_PROG_ENDE
  return rc;
}

const char * oyGetScopeString_       ( oySCOPE_e           scope )
{
  /* enable system wide keys for privileged users */
  if(scope == oySCOPE_SYSTEM)
    return OY_SYS;
  else if(scope == oySCOPE_USER_SYS)
    return "";
  else
    return OY_USER;
}



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
    if ( type < 0 )
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
      if(name) {
        r = oySetPersistentString (config_name, scope, fileName, com);
        DBG_PROG3_S( "%s %s %s",config_name,fileName,com?com:"" )
      } else {
        KeySet* list;
        Key *current;
        char* value = (char*) calloc (sizeof(char), MAX_PATH);
        int rc = 0;

        DBG_PROG

        list = oyReturnChildrenList_(OY_STD, scope, &rc );
        if(!list)
        {
          FOR_EACH_IN_KDBKEYSET( current, list )
          {
            keyGetName(current, value, MAX_PATH);
            DBG_NUM_S( value )
            if(strstr(value, config_name) != 0 &&
               strlen(value) == strlen(config_name))
            {
              DBG_PROG_S(value)
#if KDB_VERSION_NUM >= 800
#else
              if(!oy_handle_)
                return 1;
#endif
              oyKdbRemove ( oy_handle_, value, scope );
              break;
            }
          }
        }

        DBG_NUM_S( value )

        oyFree_m_ (list) DBG_PROG
        oyFree_m_ (value) DBG_PROG
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


/**@brief read Key value
 *
 *  1. ask user
 *  2. if user has no setting ask system
 */
char *   oyDBGetStringFast_          ( KDB               * oy_handle_,
                                       const char        * key_name,
                                       oySCOPE_e           scope,
                                       oyAlloc_f           allocate_func )
{
  char* name = 0;
  char* full_key_name = NULL;
  int rc = 0;
  Key * key = 0;
  int success = 0;

  name = (char*) oyAllocateWrapFunc_( MAX_PATH, allocate_func );

  /** check if the key is a binary one */
  if(scope == oySCOPE_USER_SYS ||
     scope == oySCOPE_USER)
  {
    oyStringAddPrintf( &full_key_name, AD, "%s%s", OY_USER, key_name );

    name[0] = 0;

    key = keyNew( full_key_name, KEY_END );
    rc=kdbGetKey( oy_handle_, key ); oyERR(key)
    success = keyIsString(key);
  }

  if(success)
    keyGetString ( key, name, MAX_PATH );
  keyDel( key ); key = 0;

  /** Fallback to system key otherwise */
  if( (rc || !strlen( name )) &&
      (scope == oySCOPE_USER_SYS ||
       scope == oySCOPE_SYSTEM))
  {
    if(full_key_name)
      oyFree_m_( full_key_name );
    oyStringAddPrintf( &full_key_name, AD, "%s%s", OY_SYS, key_name );
    key = keyNew( full_key_name, KEY_END );
    rc=kdbGetKey( oy_handle_, key ); oyERR(key)
    success = keyIsString(key);
    if(success)
      keyGetString( key, name, MAX_PATH );
    keyDel( key ); key = 0;
  }


  oyFree_m_( full_key_name );

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  return name;
}


int      oyDB_getStrings             ( oyDB_s            * db,
                                       oyOptions_s      ** options,
                                       const char       ** key_names,
                                       int                 key_names_n )
{
  char* value = 0;
  const char * key;
  int i;
  int error = 0;

  for(i = 0; i < key_names_n; ++i)
  {
    key = key_names[i];
    value = oyDB_getString( db, key );
    if(value)
    {
      if(value[0])
      {
        oyOption_s * o = oyOption_FromRegistration( key, NULL );
        oyOption_SetFromText(o, value, 0);
        oyOption_SetFlags(o, oyOption_GetFlags(o) & (~oyOPTIONATTRIBUTE_EDIT));
        oyOption_SetSource( o, oyOPTIONSOURCE_DATA );
        if(!*options)
          *options = oyOptions_New(NULL);
        oyOptions_MoveIn( *options, &o, -1 );
      } else
        ++error;
      oyFree_m_(value);
    } else
      ++error;
  }

  DBG_PROG_ENDE

  return error;
}

int      oyDBEraseKey_               ( const char        * key_name,
                                       oySCOPE_e           scope )
{
  int error = !key_name,
      rc = 0;
  KeySet * ks = 0;
  char * name = NULL;

#if KDB_VERSION_NUM >= 800
  Key * error_key = keyNew(KEY_END);
  KDB * oy_handle_ = kdbOpen(error_key); oyERRopen(error_key)
  Key * top =  keyNew(KEY_END);
  KeySet * cut;

  oyStringAddPrintf( &name, AD, "%s%s", oyGetScopeString_(scope), key_name );

  keySetName( top, name );

  ks = ksNew(0,NULL);
  rc = kdbGet(oy_handle_, ks, top);
  keySetName( top, name );
  cut = ksCut(ks, top);
  rc = kdbSet(oy_handle_, ks, top); oyERR( top )

  ksDel(ks);
  ksDel(cut);
  keyDel(top);
  kdbClose(oy_handle_, error_key);
  keyDel(error_key);
  oyFree_m_( name );

#else /* KDB_VERSION_NUM >= 800 */
  int success = 0;
  Key * key = 0,
      * current = 0;
  char * value = NULL;

  

  if(!oy_handle_)
    return 1;

  oyStringAddPrintf( &name, AD, "%s%s", oyGetScopeString_(scope), key_name );

  if(!error)
  {
    key = keyNew( name, KEY_END );
    rc = kdbGetKey( oy_handle_, key ); oyERR(key)

    if(!keyIsDir( key ))
    {
      if( keyRemove( key ) == 1 )
      {
        rc = kdbSetKey( oy_handle_, key ); oyERR(key)
        if(rc == 0)
        {
          DBG_PROG1_S( "removed key %s", name );
          success = 1;
        }
      }
      keyDel( key );

      if(success)
        return error;

      rc = oyKdbRemove ( oy_handle_, name, scope ); oyERR(error_key)
      if(rc == 0)    
      {
        return error;
      }
    }

    rc = 0;
    ks = oyReturnChildrenList_( key_name, scope, &rc ); /* rc == 0 */
    if(ks)
    {
      oyAllocHelper_m_( value, char, MAX_PATH, 0, return 1 )

      FOR_EACH_IN_KDBKEYSET( current, ks )
      {
        keyGetName(current, value, MAX_PATH);

        if(strstr(value, key_name) != 0)
        {
          rc = oyKdbRemove ( oy_handle_, value, scope ); oyERR(error_key)
          if(rc == 0)
          {
            DBG_PROG1_S( "removed key %s", value );
          }
        }
      }

      oyFree_m_( value );
    }

    rc = oyKdbRemove ( oy_handle_, name, scope ); oyERR(error_key)
    if(rc == 0)
    {
      DBG_PROG1_S( "removed key %s", name );
    }
  }


  oyFree_m_( name );
#endif /* KDB_VERSION_NUM >= 800 */

  return error;
}

