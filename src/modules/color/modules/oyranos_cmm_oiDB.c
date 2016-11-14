/** @file oyranos_cmm_oiDB.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2016 (C) Kai-Uwe Behrmann
 *
 *  @brief    OpenICC DB module for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2016/10/23
 */

/**
 *  The module provides a drop in replacement for the configuration DB
 *  access functions using libOpenICC.
 */


#include "oyCMM_s.h"
#include "oyCMMapi10_s_.h"

#include "oyranos_cmm.h"         /* the API's this CMM implements */
#include "oyranos_config.h"            /* oyDB_s stuff */
#include "oyranos_i18n.h"
#include "oyranos_string.h"

#include "oyjl/oyjl_tree.h"

/*
oyCMM_s         oiDB_cmm_module;
oyCMMapi10_s    oiDB_api10_cmm;
*/


/* --- internal definitions --- */

#define CMM_NICK "oiDB"

#define CMM_VERSION {0,1,0}


/* declarations */
oyDB_s * oiDB_newFrom                ( const char        * top_key_name,
                                       oySCOPE_e           scope,
                                       oyAlloc_f           allocFunc,
                                       oyDeAlloc_f         deAllocFunc );
void     oiDB_release                ( oyDB_s           ** db );
char *   oiDB_getString              ( oyDB_s            * db,
                                       const char        * key_name );
int      oiDB_getStrings             ( oyDB_s            * db,
                                       oyOptions_s      ** options,
                                       const char       ** key_names,
                                       int                 key_names_n );
char **  oiDB_getKeyNames            ( oyDB_s            * db,
                                       const char        * key_name,
                                       int               * n );
char **  oiDB_getKeyNamesOneLevel    ( oyDB_s            * db,
                                       const char        * key_name,
                                       int               * n );

/* DB write wrappers */
int      oiDBSetString               ( const char        * keyName,
                                       oySCOPE_e           scope,
                                       const char        * value,
                                       const char        * comment );
char*    oiDBSearchEmptyKeyname      ( const char        * keyParentName,
                                       oySCOPE_e           scope );
int      oiDBEraseKey                ( const char        * key_name,
                                       oySCOPE_e           scope );


oyMessage_f oiDB_msg = oyMessageFunc;

int            oiDBMessageFuncSet ( oyMessage_f         oiDB_msg_func );
int                oiDBInit       ( );


/* implementation */
#include <openicc_config.h>
#include <openicc_db.h>

/* our object type */
struct oyDB_s {
  char       type[8];                  /* 4 chars long ID string + zero terminated */
  oySCOPE_e  scope;
  char     * top_key_name;
  oyAlloc_f  alloc;
  oyDeAlloc_f deAlloc;

  /* private members */
  openiccDB_s * db;
};

oyDB_s * oiDB_newFrom                ( const char        * top_key_name,
                                       oySCOPE_e           scope,
                                       oyAlloc_f           allocFunc,
                                       oyDeAlloc_f         deAllocFunc )
{
  oyDB_s * db = allocFunc(sizeof(oyDB_s));
  if(db)
  {
    memset( db,0, sizeof(oyDB_s) );

    sprintf( db->type, CMM_NICK );
    db->db = openiccDB_NewFrom( top_key_name, (openiccSCOPE_e) scope );
    if(!db->db)
    {
      oiDB_msg( oyMSG_ERROR, 0, OY_DBG_FORMAT_ "creation of DB object failed", OY_DBG_ARGS_ );
      deAllocFunc( db );
      return NULL;
    }
    db->top_key_name = oyStringCopy( top_key_name, oyAllocateFunc_ );
    db->alloc = allocFunc;
    db->deAlloc = deAllocFunc;
    db->scope = scope;
  }

  return db;
}

void     oiDB_release                ( oyDB_s           ** db )
{
  if(db && *db)
  {
    oyDB_s * s = *db;
    oyDeAlloc_f deAlloc = s->deAlloc;

    if(!db || strcmp( s->type, CMM_NICK ) != 0)
      oiDB_msg( oyMSG_ERROR, 0, OY_DBG_FORMAT_ "wrong object type: %s - expected %s", OY_DBG_ARGS_, s->type, CMM_NICK );
    if(s->top_key_name) deAlloc(s->top_key_name); s->top_key_name = NULL;
    memset( s->type, 0, 8 );

    openiccDB_Release( &s->db );

    deAlloc( s );
    *db = NULL;
  }
}
char *   oiDB_getString              ( oyDB_s            * db,
                                       const char        * key_name )
{
  const char * value = NULL;
  int error;

  if(!db || strcmp( db->type, CMM_NICK ) != 0)
    oiDB_msg( oyMSG_ERROR, 0, OY_DBG_FORMAT_ "wrong object type: %s - expected %s", OY_DBG_ARGS_, db->type, CMM_NICK );
  error = openiccDB_GetString( db->db, key_name, &value);
  if( error > 0 )
    oiDB_msg( oyMSG_ERROR, 0, OY_DBG_FORMAT_ "%s", OY_DBG_ARGS_, db->type );

  return oyStringCopy( value, db->alloc );
}
int      oiDB_getStrings             ( oyDB_s            * db,
                                       oyOptions_s      ** options,
                                       const char       ** key_names,
                                       int                 key_names_n )
{
  const char * value;
  const char * key;
  int i;
  int error = 0;

  if(!db || strcmp( db->type, CMM_NICK ) != 0)
    oiDB_msg( oyMSG_ERROR, 0, OY_DBG_FORMAT_ "wrong object type: %s - expected %s", OY_DBG_ARGS_, db->type, CMM_NICK );
  for(i = 0; i < key_names_n; ++i)
  {
    key = key_names[i];
    value = NULL;
    error = openiccDB_GetString( db->db, key, &value);
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
    } else
      ++error;
  }

  return error;
}
char **  oiDB_getKeyNames            ( oyDB_s            * db,
                                       const char        * key_name,
                                       int               * n )
{
  char ** keys = NULL;
  int error;

  if(!db || strcmp( db->type, CMM_NICK ) != 0)
    oiDB_msg( oyMSG_ERROR, 0, OY_DBG_FORMAT_ "wrong object type: %s - expected %s", OY_DBG_ARGS_, db->type, CMM_NICK );
  error = openiccDB_GetKeyNames( db->db, key_name, 0, oyAllocateFunc_, oyDeAllocateFunc_, &keys, n );
  if( error > 0 )
    oiDB_msg( oyMSG_ERROR, 0, OY_DBG_FORMAT_ "%s", OY_DBG_ARGS_, db->type );
  return keys; 
}
char **  oiDB_getKeyNamesOneLevel    ( oyDB_s            * db,
                                       const char        * key_name,
                                       int               * n )
{
  char ** keys = NULL;
  int error;

  if(!db || strcmp( db->type, CMM_NICK ) != 0)
    oiDB_msg( oyMSG_ERROR, 0, OY_DBG_FORMAT_ "wrong object type: %s - expected %s", OY_DBG_ARGS_, db->type, CMM_NICK );

  error = openiccDB_GetKeyNames( db->db, key_name, 1, oyAllocateFunc_, oyDeAllocateFunc_, &keys, n );
  if( error > 0 )
    oiDB_msg( oyMSG_ERROR, 0, OY_DBG_FORMAT_ "%s", OY_DBG_ARGS_, db->type );
  return keys; 
}

int oiDBSetString                    ( const char        * key_name,
                                       oySCOPE_e           scope,
                                       const char        * value,
                                       const char        * comment)
{
  return openiccDBSetString( key_name, scope, value, comment );
}
char*    oiDBSearchEmptyKeyname        ( const char      * key_parent_name,
                                         oySCOPE_e         scope )
{
  return openiccDBSearchEmptyKeyname( key_parent_name, (openiccSCOPE_e) scope );
}
int      oiDBEraseKey                ( const char        * key_name,
                                       oySCOPE_e           scope )
{
  return openiccDBSetString( key_name, scope, NULL, "delete" );
}

/** Function oiDBInit
 *  @brief   API requirement
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/04/29
 *  @since   2016/04/29 (Oyranos: 0.9.6)
 */
int                oiDBInit       ( oyStruct_s        * filter )
{
  int error = 0;
  oiDB_msg( oyMSG_WARN, 0, OY_DBG_FORMAT_ "", OY_DBG_ARGS_ );
  return error;
}

const char*oiDBopeniccStaticMessageFunc (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  typedef struct {
    openiccOBJECT_e type;
  } openicc_struct;
  openicc_struct * o = (openicc_struct *) obj;

  if(obj)
  {
    if(o->type == openiccOBJECT_CONFIG)
      return "openiccConfig_s";
    if(o->type == openiccOBJECT_DB)
      return "openiccDB_s";
  }
  return "unknown";
}


/** Function oiDBMessageFuncSet
 *  @brief
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 *  @date    2016/05/01
 */
int            oiDBMessageFuncSet ( oyMessage_f         message_func )
{
  oiDB_msg = message_func;
  openiccMessageFuncSet( message_func );
  oyStruct_RegisterStaticMessageFunc( openiccOBJECT_CONFIG,
                                      oiDBopeniccStaticMessageFunc );
  oyStruct_RegisterStaticMessageFunc( openiccOBJECT_DB,
                                      oiDBopeniccStaticMessageFunc );
  oiDB_msg( oyMSG_WARN, 0, OY_DBG_FORMAT_ "", OY_DBG_ARGS_ );
  return 0;
}

oyDbAPI_s oiDBopeniccDbAPI = {
  /* newFrom */ oiDB_newFrom,
  /* release */ oiDB_release,
  /* getString */ oiDB_getString,
  /* getStrings */ oiDB_getStrings,
  /* getKeyNames */ oiDB_getKeyNames,
  /* getKeyNamesOneLevel */ oiDB_getKeyNamesOneLevel,

  /* setString */ oiDBSetString,
  /* searchEmptyKeyname */ oiDBSearchEmptyKeyname,
  /* eraseKey */ oiDBEraseKey
};

/**
 *  This function implements oyMOptions_Handle_f.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/10/23
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 */
int          oiDBMOptions_Handle     ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  int error = 0;

  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    oiDB_msg( oyMSG_DBG, 0, "called %s()::can_handle", __func__ );
    return error;
  }
  else if(oyFilterRegistrationMatch(command,"db_handler", 0))
  {
    error = oyDbHandlingSet( &oiDBopeniccDbAPI );
    oiDB_msg( error?oyMSG_WARN:oyMSG_DBG, 0, "called %s()::db_handler", __func__ );
  }

  return 0;
}

const char *oiDB_texts_profile_create[4] = {"can_handle","db_handler","help",0};

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/10/23
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 */
const char * oiDBInfoGetTextDBC      ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
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
      return _("The Oyranos \"db_handler\" command will initialise the oyDB hooks with the OpenICC Configuration API.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Initialise the oyDB_s APIs.");
    else
      return _("The Oyranos \"db_handler\" command will initialise the oyDB hooks with the OpenICC Configuration API.");
  }
  return 0;
}

/** @instance oiDB_api10_cmm
 *  @brief    oiDB oyCMMapi10_s implementation
 *
 *  handlers for OpenICC DB
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/10/23
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 */
oyCMMapi10_s_    oiDB_api10_cmm = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  (oyCMMapi_s*) NULL,

  oiDBInit,
  oiDBMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "db_handler._" CMM_NICK,

  CMM_VERSION,
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */
 
  oiDBInfoGetTextDBC,             /**< getText */
  (char**)oiDB_texts_profile_create,   /**<texts; list of arguments to getText*/
 
  oiDBMOptions_Handle                  /**< oyMOptions_Handle_f oyMOptions_Handle */
};




/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/10/23
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 */
const char * oiDBInfoGetText         ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return CMM_NICK;
    else if(type == oyNAME_NAME)
      return _("OpenICC Configuration API");
    else
      return _("OpenICC Configuration API");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return "Kai-Uwe";
    else if(type == oyNAME_NAME)
      return "Kai-Uwe Behrmann";
    else
      return _("OpenICC project; www/support: http://github.com/OpenICC/config");
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
      return _("The filter provides the libOpenICC Configuration API to Oyranos DB hooks." );
    else
      return _("The filter provides the libOpenICC Configuration API to Oyranos DB hooks." );
  }
  return 0;
}
const char *oiDB_texts[5] = {"name","copyright","manufacturer","help",0};
oyIcon_s oiDB_icon = {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"};

/** @instance oiDB_cmm_module
 *  @brief    oiDB module infos
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/10/23
 *  @since   2016/05/01 (Oyranos: 0.9.6)
 */
oyCMM_s oiDB_cmm_module = {

  oyOBJECT_CMM_INFO_S,                 /**< type, struct type */
  0,0,0,                               /**< ,dynamic object functions */
  CMM_NICK,                            /**< cmm, ICC signature */
  "0.6",                               /**< backend_version */
  oiDBInfoGetText,                     /**< getText */
  (char**)oiDB_texts,                  /**<texts; list of arguments to getText*/
  OYRANOS_VERSION,                     /**< oy_compatibility */

  (oyCMMapi_s*) & oiDB_api10_cmm,       /**< api */

  &oiDB_icon, /**< icon */
  oiDBInit                          /**< oyCMMinfoInit_f */
};

