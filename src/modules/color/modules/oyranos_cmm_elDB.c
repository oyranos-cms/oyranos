/** @file oyranos_cmm_elDB.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2016-2018 (C) Kai-Uwe Behrmann
 *
 *  @brief    Elektra DB module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2016/10/23
 */

/**
 *  The module provides a drop in replacement for the configuration DB
 *  access functions using Elektra.
 */

#include "oyCMM_s.h"
#include "oyCMMapi10_s_.h"

#include "oyranos_cmm.h"         /* the API's this CMM implements */
#include "oyranos_config.h"            /* oyDB_s stuff */
#include "oyranos_i18n.h"
#include "oyranos_string.h"

/*
oyCMM_s         elDB_cmm_module;
oyCMMapi10_s    elDB_api10_cmm;
*/


/* --- internal definitions --- */

#define CMM_NICK "elDB"

#define CMM_VERSION {0,6,0}


/* declarations */
oyDB_s * elDB_newFrom                ( const char        * top_key_name,
                                       oySCOPE_e           scope,
                                       oyAlloc_f           allocFunc,
                                       oyDeAlloc_f         deAllocFunc );
void     elDB_release                ( oyDB_s           ** db );
char *   elDB_getString              ( oyDB_s            * db,
                                       const char        * key_name );
int      elDB_getStrings             ( oyDB_s            * db,
                                       oyOptions_s      ** options,
                                       const char       ** key_names,
                                       int                 key_names_n );
char **  elDB_getKeyNames            ( oyDB_s            * db,
                                       const char        * key_name,
                                       int               * n );
char **  elDB_getKeyNamesOneLevel    ( oyDB_s            * db,
                                       const char        * key_name,
                                       int               * n );

/* DB write wrappers */
int      elDBSetString               ( const char        * keyName,
                                       oySCOPE_e           scope,
                                       const char        * value,
                                       const char        * comment );
char*    elDBSearchEmptyKeyname      ( const char        * keyParentName,
                                       oySCOPE_e           scope );
int      elDBEraseKey                ( const char        * key_name,
                                       oySCOPE_e           scope );


oyMessage_f elDB_msg = oyMessageFunc;

int            elDBMessageFuncSet ( oyMessage_f         elDB_msg_func );
int                elDBInit       ( );
int                elDBReset      ( oyStruct_s        * filter OY_UNUSED );


/* implementation */
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <kdb.h>

char oy_elektra_error_text[24] = {0};

char * oy__kdbStrError(int rc) { sprintf(oy_elektra_error_text, "elektra: %d", rc);
                                 return oy_elektra_error_text; }
#define kdbStrError(t) oy__kdbStrError(t)

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

#define DBG_EL1_S DBG_MEM1_S
#define DBG_EL2_S DBG_MEM2_S
#define DBG_EL_S DBG_MEM_S

/* --- static variables   --- */

int oyranos_init = 0;

/* --- internal API definition --- */

const char * oyGetScopeString_       ( oySCOPE_e           scope );
char *       oyGetScopeString        ( oySCOPE_e           scope,
                                       oySCOPE_e           scope_prefered,
                                       const char        * reg );

  /* ksNext uses the same entry twice in a 1 component KeySet, we avoid this */
#define FOR_EACH_IN_KDBKEYSET( current_, list ) \
   ksRewind( list );  \
   for( current_ = ksNext( list ); current_; current_ = ksNext( list )  )

static char *   elToJson             ( const char        * key_name,
                                       oyAlloc_f           alloc,
                                       int                 strict )
{
  int count = 0, i;
  char** list;
  char * key = NULL;

  if(!key_name || !*key_name) return NULL;

  if(strchr(key_name,'[') && strict)
    elDB_msg( oyMSG_DBG, 0, OY_DBG_FORMAT_ "expected Elektra style array index but obtained: %s %d", OY_DBG_ARGS_, key_name, oy_debug );

  list = oyStringSplit( key_name, '/', &count, 0 );

  for(i = 0; i < count; ++i)
  {
    char * k = list[i];
    if(k[0] == '#')
      oyStringAddPrintf( &key, 0,0, "%s[%s]", i && i < count ? "/":"", k+1 );
    else
      oyStringAddPrintf( &key, 0,0, "%s%s", i && i < count ? "/":"", k );
  }

  if(alloc && alloc != oyAllocateFunc_)
  {
    char * r = oyStringCopy( key, alloc );
    oyFree_m_( key );
    key = r;
  }

  oyStringListRelease( &list, count, 0 );

  return key;
}
static char *   elToJson2            ( char              * key_name,
                                       oyAlloc_f           alloc,
                                       oyDeAlloc_f         dealloc, 
                                       int                 strict )
{
  char * t = NULL;
  oyDeAlloc_f d = dealloc?dealloc : oyDeAllocateFunc_;

  if(key_name)
  {
    t = elToJson(key_name, alloc?alloc:oyAllocateFunc_, strict);
    d(key_name);
  }

  return t;
}

static char *   jsonToEl             ( const char        * key_name,
                                       oyAlloc_f           alloc )
{
  int count = 0, i;
  char** list;
  char * key = NULL, *r;

  if(!key_name || !*key_name) return NULL;

  if(strchr(key_name,'#'))
    elDB_msg( oyMSG_DBG, 0, OY_DBG_FORMAT_ "expected JSON style array index but obtained: %s", OY_DBG_ARGS_, key_name );

  list = oyStringSplit( key_name, '/', &count, 0 );

  for(i = 0; i < count; ++i)
  {
    char * k = list[i];
    if(k[0] == '[')
    {
      char * t = oyStringCopy( k, 0 ), * t2 = strrchr( t, ']' );

      if(t2)
        t2[0] = '\000';
      oyStringAddPrintf( &key, 0,0, "%s#%s", i && i < count ? "/":"", t+1 );
      oyFree_m_(t);
    }
    else
      oyStringAddPrintf( &key, 0,0, "%s%s", i && i < count ? "/":"", k );
  }

  if(alloc && alloc != oyAllocateFunc_)
  {
    r = oyStringCopy( key, alloc );
    oyFree_m_( key );
    key = r;
  }

  oyStringListRelease( &list, count, 0 );

  return key;
}

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


/* --- function definitions --- */

/* our object type */
struct oyDB_s {
  char       type[8];                  /* 4 chars long ID string + zero terminated */
  oySCOPE_e  scope;
  char     * top_key_name;
  oyAlloc_f  alloc;
  oyDeAlloc_f deAlloc;

  /* private members */
  KDB      * h;
  Key      * error;
  int        err;
  KeySet   * ks;
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
    list_name_user = oyGetScopeString( db->scope, oySCOPE_USER, db->top_key_name );
    if(!db->h)
      return 0;

    rc =
      oyGetByName( list_user, list_name_user );
    if(rc < 0)
      oyDB_printWarn(db);

    if(rc > 0)
      DBG_NUM1_S("oyGetByName returned with %d", rc);
  }

  if( db->scope == oySCOPE_USER_SYS || db->scope == oySCOPE_SYSTEM )
  {
    list_sys = ksNew(0,NULL);
    list_name_sys = oyGetScopeString( db->scope, oySCOPE_SYSTEM, db->top_key_name );
    if(!db->h)
      return 0;

    rc =
      oyGetByName( list_sys, list_name_sys );
    if(rc < 0)
      oyDB_printWarn(db);

    if(rc > 0)
      DBG_NUM1_S("kdbGetChildKeys returned with %d", rc);
  }

  if(list_user)
  {
    ksAppend( db->ks, list_user );
    ksDel( list_user );
  }
  if(list_sys)
  {
    ksAppend( db->ks, list_sys );
    ksDel( list_sys );
  }

  if(list_name_user) oyFree_m_( list_name_user )
  if(list_name_sys) oyFree_m_( list_name_sys )

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

char * oyGetScopeString              ( oySCOPE_e           scope,
                                       oySCOPE_e           scope_prefered,
                                       const char        * key_name )
{
  int has_scope = 0;
  char * full_elektra_key_reg = NULL;
  static int user_len = 0;
  static int sys_len = 0;
  char * name = NULL;

  if(user_len == 0)
  {
    sys_len = strlen(OY_SYS);
    user_len = strlen(OY_USER);
  }

  if(key_name)
    if((strlen(key_name) > (size_t)user_len && memcmp(key_name,OY_USER,user_len) == 0) ||
       (strlen(key_name) > (size_t)sys_len && memcmp(key_name,OY_SYS,sys_len) == 0))
      has_scope = 1;

  if(scope == oySCOPE_USER_SYS)
    scope = scope_prefered;

  name = jsonToEl( key_name, oyAllocateFunc_ );

  oyStringAddPrintf( &full_elektra_key_reg, 0,0, "%s%s", has_scope?"":oyGetScopeString_(scope), name );

  return full_elektra_key_reg;
}



/* oyranos hook part */


oyDB_s * elDB_newFrom                ( const char        * top_key_name,
                                       oySCOPE_e           scope,
                                       oyAlloc_f           allocFunc,
                                       oyDeAlloc_f         deAllocFunc )
{
  oyDB_s * db = allocFunc(sizeof(oyDB_s));

  if(db)
  {
    memset( db,0, sizeof(oyDB_s) );

    sprintf( db->type, CMM_NICK );
    db->error = keyNew(KEY_END);
    db->h = kdbOpen(db->error);
    if(!db->h)
    {
      db->err = -1;
      oyDB_printWarn( db );
    }
    db->top_key_name = oyStringCopy( top_key_name, oyAllocateFunc_ );
    db->ks = NULL;
    db->alloc = allocFunc;
    db->deAlloc = deAllocFunc;
    db->scope = scope;
  }

  return db;
}

void     elDB_release                ( oyDB_s           ** db )
{
  if(db && *db)
  {
    oyDB_s * s = *db;
    oyDeAlloc_f deAlloc = s->deAlloc;

    if(strcmp( s->type, CMM_NICK ) != 0)
      elDB_msg( oyMSG_ERROR, 0, OY_DBG_FORMAT_ "wrong object type: %s - expected %s", OY_DBG_ARGS_, s->type, CMM_NICK );
    if(s->top_key_name) { deAlloc(s->top_key_name); s->top_key_name = NULL; }
    memset( s->type, 0, 8 );

    kdbClose( s->h, s->error );
    s->h = NULL;
    keyDel(s->error);
    s->error = NULL;
    if(s->ks)
      ksDel( s->ks );
    s->ks = NULL;
    s->alloc = 0;

    deAlloc( s );
    *db = NULL;
  }
}
char *   elDB_getString              ( oyDB_s            * db,
                                       const char        * key_name )
{
  int error = !db;
  const char * current_name;
  KeySet * my_key_set = NULL;
  Key * current = NULL;
  char * name = NULL;
  char * value = NULL;

  DBG_PROG_START

  if(!error)
    name = jsonToEl( key_name, oyAllocateFunc_ );

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

  oyFree_m_(name);

  DBG_PROG_ENDE
  return value;
}
int      elDB_getStrings             ( oyDB_s            * db,
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
    value = elDB_getString( db, key );
    if(value)
    {
      if(value[0])
      {
        oyOption_s * o = oyOption_FromRegistration( key, NULL );
        oyOption_SetFromString(o, value, 0);
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
char **  elDB_getKeyNames            ( oyDB_s            * db,
                                       const char        * key_name,
                                       int               * n )
{
  int error = !db || !n;
  const char * current_name = NULL;
  KeySet * my_key_set = NULL;
  Key * current = NULL;
  char * name = NULL,
       * current_name_json = NULL;
  char ** texts = NULL;
  int name_len;

  DBG_PROG_START

  if(!error)
    name = jsonToEl( key_name, oyAllocateFunc_ );
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
      current_name_json = elToJson(current_name, oyAllocateFunc_, 1);
      if(current_name &&
         oyStrstr_(current_name_json, name) )
      {
        const char * t = oyStrstr_(current_name_json, name);

        if(strlen(t) > (size_t)name_len &&
           !oyStringListHas_( (const char **)texts, *n, t ) )
          oyStringListAddStaticString( &texts, n, t,
                                        oyAllocateFunc_, oyDeAllocateFunc_);
      }
      oyFree_m_(current_name_json);
    }
  }

  oyFree_m_(name);

  DBG_PROG_ENDE
  return texts;
}
/** @brief The function returns keys found just one level under the arguments one. */
char **  elDB_getKeyNamesOneLevel    ( oyDB_s            * db,
                                       const char        * key_name,
                                       int               * n )
{
  int error = !db || !n;
  const char * current_name = NULL;
  KeySet * my_key_set = NULL;
  Key * current = NULL;
  char * name = NULL,
       * current_name_json = NULL;
  char ** texts = NULL;
  int name_len;

  DBG_PROG_START

  if(!error)
    name = elToJson( key_name, oyAllocateFunc_, 0 );
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
      current_name_json = elToJson(current_name, oyAllocateFunc_, 0);
      if(current_name &&
         oyStrstr_(current_name_json, name) )
      {
        const char * t = oyStrstr_(current_name_json, name);
        char * txt = NULL, * tmp;
        /** Cut after one level behind key_parent_name. */
        if( oyStrlen_(name) < oyStrlen_(t) &&
            oyStrrchr_( &t[oyStrlen_(name)+1], OY_SLASH_C ) )
        {
          txt = oyStringCopy_( t, oyAllocateFunc_ );
          tmp = &txt[oyStrlen_(name)+1];
          tmp = oyStrchr_(tmp, OY_SLASH_C);
          if(tmp)
            tmp[0] = '\000';
          t = txt;
        }

        if(strlen(t) > (size_t)name_len &&
           !oyStringListHas_( (const char **)texts, *n, t ) )
          oyStringListAddStaticString( &texts, n, t,
                                        oyAllocateFunc_, oyDeAllocateFunc_);

        if(txt) oyFree_m_(txt);
      }
      oyFree_m_(current_name_json);
    }
  }

  oyFree_m_(name);

  DBG_PROG_ENDE
  return texts;
}

int elDBSetString                    ( const char        * key_name,
                                       oySCOPE_e           scope,
                                       const char        * value,
                                       const char        * comment)
{
  Key * error_key = keyNew(KEY_END);
  KDB * oy_handle_ = kdbOpen(error_key); oyERRopen(error_key)
  int rc=0,
      max_len;
  Key *key;
  char *name = NULL;
  char *value_utf8 = NULL;
  char *comment_utf8 = NULL;

  DBG_PROG_START

  oyAllocHelper_m_(value_utf8, char, MAX_PATH, 0,; )
  oyAllocHelper_m_(comment_utf8, char, MAX_PATH, 0,; )

  name = oyGetScopeString( scope, oySCOPE_USER, key_name );
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
  if (name)
    DBG_PROG_S(( name ));
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
    rc=keySetMeta(key,"comment",comment_utf8);
    DBG_EL2_S( "rc = keySetMeta( key, \"comment\", \"%s\" ) == %d", comment_utf8, rc);
    if(rc <= 0)
      oyMessageFunc_p( oyMSG_WARN, 0, OY_DBG_FORMAT_"code:%d %s name:%s comment:%s",
                       OY_DBG_ARGS_, rc, kdbStrError(rc), name, comment);
  }

  rc=oySetKey( key ); oyERR(key)
  DBG_EL1_S( "rc = oySetKey( oy_handle, key ) == %d", rc );
  if(rc < 0)
    oyMessageFunc_p( oyMSG_WARN, 0, OY_DBG_FORMAT_ "code:%d %s name:%s",
                     OY_DBG_ARGS_, rc, kdbStrError(rc), name);
  keyDel( key );
  DBG_EL_S( "keyDel( key )" );


  clean:
  oyFree_m_( name )
  oyFree_m_( value_utf8 )
  oyFree_m_( comment_utf8 )
  kdbClose( oy_handle_,error_key ); oyERR(error_key)
  keyDel(error_key);
  DBG_PROG_ENDE
  return rc < 0 ? 1 : 0;
}

char*    elDBSearchEmptyKeyname        ( const char      * key_parent_name,
                                         oySCOPE_e         scope )
{
  char * key_base_name = NULL;
  char * new_key_name = NULL;
  int nth = -1, i = 0, rc=0;
  Key *key = 0;
  KeySet * ks = ksNew(0,NULL);
  KeySet * cut;
  size_t count;
  Key * error_key = keyNew(KEY_END);
  KDB * oy_handle_ = kdbOpen(error_key); oyERRopen(error_key)

  DBG_PROG_START

  if(!oy_handle_)
    return 0;

  key_base_name = oyGetScopeString( scope, oySCOPE_USER, key_parent_name );

  key = keyNew( key_base_name, KEY_END );
  rc = kdbGet( oy_handle_, ks, key ); oyERR(key)

  ksRewind( ks );

  keyDel( key );

  /* search for empty keyname in array */
  while (nth == -1)
  {
    if(new_key_name) oyFree_m_( new_key_name );
    oyStringAddPrintf( &new_key_name, 0,0, "%s/#%d", key_base_name, i );

    ksRewind( ks );
    key = keyNew( new_key_name, KEY_END );
    cut = ksCut( ks, key );
    count = ksGetSize( cut );
    if(!cut || !count)
      nth = i;
    keyDel( key );
    DBG_PROG3_S("search key = \"%s\" %s %d\n", new_key_name, cut?"found":"not found", (int)ksGetSize(cut) );
    ksDel( cut );

    i++;
  }

  oyFree_m_( key_base_name );
  ksDel( ks );
  kdbClose( oy_handle_,error_key ); oyERR(error_key)
  keyDel(error_key);

  DBG_PROG_ENDE
  return elToJson2(new_key_name, 0,0, 1);
}

int      elDBEraseKey                ( const char        * key_name,
                                       oySCOPE_e           scope )
{
  int error = !key_name,
      rc = 0;
  KeySet * ks = 0;
  char * name = NULL;

  Key * error_key = keyNew(KEY_END);
  KDB * oy_handle_ = kdbOpen(error_key); oyERRopen(error_key)
  Key * top =  keyNew(KEY_END);
  KeySet * cut;

  name = oyGetScopeString( scope, oySCOPE_USER, key_name );

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

  return error;
}

/** Function elDBInit
 *  @brief   API requirement
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/04/29
 *  @since   2016/04/29 (Oyranos: 0.9.6)
 */
int                elDBInit       ( oyStruct_s        * filter OY_UNUSED )
{
  int error = 0;
  return error;
}

/** Function elDBReset
 *  @brief   API requirement
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/09/03
 *  @since   2019/09/03 (Oyranos: 0.9.7)
 */
int                elDBReset      ( oyStruct_s        * filter OY_UNUSED )
{
  int error = 0;
  return error;
}



/** Function elDBMessageFuncSet
 *  @brief
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
int            elDBMessageFuncSet ( oyMessage_f         message_func )
{
  elDB_msg = message_func;
  return 0;
}

oyDbAPI_s elDBopeniccDbAPI = {
  oyOBJECT_DB_API_S,               /**< type set to oyOBJECT_DB_API_S for ABI compatibility with the actual used header version */
  CMM_NICK,                        /**< four byte nick name of module + terminating zero */
  CMM_VERSION,                     /**< set to module version; Major, Minor, Micro */
  {KDB_VERSION_MAJOR, KDB_VERSION_MINOR, KDB_VERSION_MICRO}, /**< lib_version */

  /* newFrom */ elDB_newFrom,
  /* release */ elDB_release,
  /* getString */ elDB_getString,
  /* getStrings */ elDB_getStrings,
  /* getKeyNames */ elDB_getKeyNames,
  /* getKeyNamesOneLevel */ elDB_getKeyNamesOneLevel,

  /* setString */ elDBSetString,
  /* searchEmptyKeyname */ elDBSearchEmptyKeyname,
  /* eraseKey */ elDBEraseKey
};

/**
 *  This function implements oyMOptions_Handle_f.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/10/23
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 */
int          elDBMOptions_Handle     ( oyOptions_s       * options OY_UNUSED,
                                       const char        * command,
                                       oyOptions_s      ** result OY_UNUSED )
{
  int error = 0;

  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    elDB_msg( oyMSG_DBG, 0, "called %s()::can_handle", __func__ );
    return error;
  }
  else if(oyFilterRegistrationMatch(command,"db_handler", 0))
  {
    error = oyDbHandlingSet( &elDBopeniccDbAPI );
    elDB_msg( error?oyMSG_WARN:oyMSG_DBG, 0, "called %s()::db_handler e:%d", __func__, error );
  }

  return 0;
}

const char *elDB_texts_profile_create[4] = {"can_handle","db_handler","help",0};

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/10/23
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 */
const char * elDBInfoGetTextDBC      ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, "db_handler")==0)
  {
         if(type == oyNAME_NICK)
      return "create_profile";
    else if(type == oyNAME_NAME)
      return _("Initialise the oyDB_s APIs.");
    else
      return _("The Oyranos \"db_handler\" command will initialise the oyDB hooks with the Elektra Configuration API.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Initialise the oyDB_s APIs.");
    else
      return _("The Oyranos \"db_handler\" command will initialise the oyDB hooks with the Elektra Configuration API.");
  }
  return 0;
}

/** @brief    elDB oyCMMapi10_s implementation
 *
 *  handlers for OpenICC DB
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/10/23
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 */
oyCMMapi10_s_    elDB_api10_cmm = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  (oyCMMapi_s*) NULL,

  elDBInit,
  elDBReset,
  elDBMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "db_handler._" CMM_NICK,

  CMM_VERSION,
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */
 
  elDBInfoGetTextDBC,             /**< getText */
  (char**)elDB_texts_profile_create,   /**<texts; list of arguments to getText*/
 
  elDBMOptions_Handle                  /**< oyMOptions_Handle_f oyMOptions_Handle */
};




/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/10/23
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 */
const char * elDBInfoGetText         ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return CMM_NICK;
    else if(type == oyNAME_NAME)
      return _("Elektra Configuration API");
    else
      return _("Elektra Configuration API");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return "Kai-Uwe";
    else if(type == oyNAME_NAME)
      return "Kai-Uwe Behrmann";
    else
      return _("Elektra project; www/support: http://github.com/ElektraInitiative/libelektra");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return "BSD-3-Clause";
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 2016 Kai-Uwe Behrmann; new BSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/BSD-3-Clause");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("The filter provides the Elektra Configuration API to Oyranos DB hooks." );
    else
      return _("The filter provides the Elektra Configuration API to Oyranos DB hooks." );
  }
  return 0;
}
const char *elDB_texts[5] = {"name","copyright","manufacturer","help",0};
oyIcon_s elDB_icon = {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"};

/** @brief    elDB module infos
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/10/23
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 */
oyCMM_s elDB_cmm_module = {

  oyOBJECT_CMM_INFO_S,                 /**< type, struct type */
  0,0,0,                               /**< ,dynamic object functions */
  CMM_NICK,                            /**< cmm, ICC signature */
  "0.6",                               /**< backend_version */
  elDBInfoGetText,                     /**< getText */
  (char**)elDB_texts,                  /**<texts; list of arguments to getText*/
  OYRANOS_VERSION,                     /**< oy_compatibility */

  (oyCMMapi_s*) & elDB_api10_cmm,      /**< api */

  &elDB_icon, /**< icon */
  elDBInit,                            /**< oyCMMinfoInit_f */
  elDBReset                            /**< oyCMMinfoReset_f */
};

