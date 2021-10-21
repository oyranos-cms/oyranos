/** @file oyranos_json.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2016-2019 (C) Kai-Uwe Behrmann
 *
 *  @brief    oyjl dependent functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2016/11/17
 */

#include "oyranos.h"
#include "oyranos_core.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_json.h"
#include "oyranos_config_internal.h"

oyjl_val     oyJsonParse             ( const char        * json )
{
  int error = !json;
  oyjl_val root;
  char * error_buffer;
 
  if(error) return NULL;

  error_buffer = (char*)oyAllocateFunc_( 256 );
  if(!error_buffer) return NULL;

  error_buffer[0] = '\000';

  root = oyjlTreeParse( json, error_buffer, 256 );
  if(error_buffer[0] != '\000')
    oyMessageFunc_p( oyMSG_WARN, NULL, OY_DBG_FORMAT_ "ERROR:\t\"%s\"\n%s", OY_DBG_ARGS_, error_buffer, json );

  oyDeAllocateFunc_( error_buffer );

  return root;
}

char **      oyJsonPathsFromPattern  ( oyjl_val            root,
                                       const char        * key )
{
  char ** list = NULL;
  int list_n = 0;

  int paths_n = 0, i;
  char ** paths = oyjlTreeToPaths( root, 1000000, NULL, 0, &paths_n );

  if(paths)
  {
    for(i = 0; i < paths_n; ++i)
    {
      const char * path = paths[i];
      if(oyjlPathMatch(path, key, OYJL_PATH_MATCH_LAST_ITEMS))
        oyStringListAddStaticString( &list, &list_n, path, oyAllocateFunc_, oyDeAllocateFunc_ );
    }
    oyjlStringListRelease( &paths, paths_n, free );

  }

  return list;
}

char *       oyJsonFromModelAndUi    ( const char        * data,
                                       const char        * ui_text,
                                       oyAlloc_f           allocate_func )
{
  char * text = NULL;
  oyjl_val droot = oyJsonParse( data ),
           uiroot = oyJsonParse( ui_text );

  int paths_n = 0, i;
  char ** paths = oyJsonPathsFromPattern( uiroot, "groups//options//key");
  while(paths && paths[paths_n]) ++paths_n;

  if(!droot)
    oyMessageFunc_p( oyMSG_WARN, NULL, OY_DBG_FORMAT_ "ERROR: data\n%s\n", OY_DBG_ARGS_, data );
  if(!uiroot)
    oyMessageFunc_p( oyMSG_WARN, NULL, OY_DBG_FORMAT_ "ERROR: ui_text\n%s\n", OY_DBG_ARGS_, ui_text );


  for(i = 0; i < paths_n; ++i)
  {
    char * path = paths[i];
    /* get the key node */
    oyjl_val uiv = oyjlTreeGetValue( uiroot, 0, path ),
             dv = NULL, v = NULL;
    /* get the key name */
    char * key = oyjlValueText( uiv, oyAllocateFunc_ ),
         * value = NULL;
    /* get the value node by the key */
    if(key)
      dv = oyjlTreeGetValue( droot, 0, key );
    /* get the value */
    if(dv)
      value = oyjlValueText( dv, oyAllocateFunc_ );
    /* write the value into the options "default" key */
    if(value && key && strchr(key,'/'))
    {
      char * t = strrchr(path,'/');
      t[0] = 0;
      v = oyjlTreeGetValueF( uiroot, OYJL_CREATE_NEW, "%s/default", path );
      oyjlValueSetString( v, value );
    }
    if(key) oyFree_m_( key );
    if(value) oyFree_m_( value );
  }

  i = 0;
  oyjlTreeToJson( uiroot, &i, &text );

  oyjlTreeFree( uiroot );
  oyjlTreeFree( droot );
  oyjlStringListRelease( &paths, paths_n, free );

  if(allocate_func && allocate_func != oyAllocateFunc_)
  {
    char * t = oyStringCopy( text, allocate_func );
    oyDeAllocateFunc_(text);
    text = t; t = 0;
  }

  return text;
}

/* just for debuging */
char *       oyJsonPrint             ( oyjl_val            root )
{
  char * json = NULL;
  int level = 0;
  oyjlTreeToJson( root, &level, &json );
  return json;
}
