/*  @file openicc_config.c
 *
 *  libOpenICC - OpenICC Colour Management Configuration
 *
 *  @par Copyright:
 *            2011-2017 (C) Kai-Uwe Behrmann
 *
 *  @brief    OpenICC Colour Management configuration helpers
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2011/06/27
 */

#include "openicc_config_internal.h"
#include "oyjl_macros.h"

#if HAVE_POSIX
#include <unistd.h>  /* getpid() */
#endif
#include <string.h>  /* strdup() */
#include <stdarg.h>  /* vsnprintf() */
#include <stdio.h>   /* vsnprintf() */


/**
 *  @brief   load configurations from in memory JSON text
 *  @memberof openiccConfig_s
 *
 *  Create a config object from JSON text. Add a ID afterwards
 *  for better error messages with openiccConfig_SetInfo() = source_file_name.
 */
openiccConfig_s *  openiccConfig_FromMem( const char       * data )
{
  openiccConfig_s * config = NULL;
  if(data && data[0])
  {
    char * msg = NULL;
    oyjlAllocHelper_m(config, openiccConfig_s, 1, malloc, return config);
    oyjlAllocHelper_m(msg, char, 1024, malloc, free(config); return NULL);

    config->type = openiccOBJECT_CONFIG;
    config->json_text = strdup( (char*)data );
    config->info = oyjlStringCopy( "openiccConfig_FromMem()", malloc );
    if(!config->info)
    {
      ERRcc_S( config, "could not allocate%s", "" );
      free(config); config = NULL;
      return config;
    }
    config->oyjl = oyjlTreeParse( data, msg, 1024 );
    if(!config->oyjl)
    {
      WARNcc_S( config, "%s\n", msg?msg:"" );
      openiccConfig_Release( &config );
    }
    free(msg);
  }

  return config;
}

/**
 *  @brief   release the data base object
 *  @memberof openiccConfig_s
 */
void               openiccConfig_Release (
                                       openiccConfig_s  ** config )
{
  openiccConfig_s * c = 0;
  if(config)
  {
    c = *config;
    if(c)
    {
      if(c->json_text)
        free(c->json_text);
      else
        WARNcc_S( c, "expected openiccConfig_s::json_text", 0 );
      if(c->oyjl)
        oyjlTreeFree(c->oyjl);
      else
        WARNcc_S( c, "expected openiccConfig_s::oyjl",0 );
      if(c->info)
        free(c->info);
      else
        WARNcc_S( c, "expected openiccConfig_s::info",0 );
      free(c);
    }
    *config = NULL;
  }
}

/**
 *  @brief   add a string for debugging and error messages
 *  @memberof openiccConfig_s
 */
void               openiccConfig_SetInfo (
                                       openiccConfig_s   * config,
                                       const char        * debug_info )
{
  if(config && debug_info)
  {
    if(config->info)
      free(config->info);
    config->info = strdup( (char*)debug_info );
  }
}

static const char * dev_cl[] = {
                OPENICC_DEVICE_MONITOR,
                OPENICC_DEVICE_SCANNER,
                OPENICC_DEVICE_PRINTER ,
                OPENICC_DEVICE_CAMERA , NULL };
/**
 *  @brief   get default device class
 */
const char** openiccConfigGetDeviceClasses (
                                       const char       ** device_classes,
                                       int               * count )
{
  int device_classes_n = 0;

  if(device_classes)
    while(device_classes[device_classes_n++]) ;
  else
  {
    device_classes_n = 4;
    device_classes = dev_cl;
  }

  *count = device_classes_n;

  return device_classes;
}

/**
 *  @brief count devices in data base object
 *  @memberof openiccConfig_s
 *
 *  @param[in]     config              the data base object
 *  @param[in]     device_classes      the device class filter
 *  @return                            count of matching device configurations
 */
int                openiccConfig_DevicesCount (
                                       openiccConfig_s   * config,
                                       const char       ** device_classes )
{
  int n = 0;

  if(config)
  {
    const char * base_path[] = {"org","freedesktop","openicc","device",0};
    oyjl_val base = oyjlTreeGet( config->oyjl, base_path, oyjl_t_object );
    if(base)
    {
      oyjl_val dev_class;
      {
        int i = 0, device_classes_n = 0;

        device_classes = openiccConfigGetDeviceClasses( device_classes,
                                       &device_classes_n );

        for(i = 0; i < device_classes_n; ++i)
        {
          const char * obj_key[] = { device_classes[i], 0 };
          dev_class = oyjlTreeGet( base, obj_key, oyjl_t_array );
          if(dev_class)
            n += dev_class->u.array.len;
        }
      }
    } else
      WARNcc_S( config, "could not find " OPENICC_DEVICE_PATH " %s",
                config->info ? config->info : "" );
  }

  return n;
}

/**
 *  @brief   get keys and their values
 *  @memberof openiccConfig_s
 *
 *  @param[in]     config              the data base object
 *  @param[in]     device_classes      the device class filter
 *  @param[in]     pos                 the device position
 *  @param[out]    keys                a zero terminated list of device keys
 *  @param[out]    values              a zero terminated list of device values
 *  @param[in]     alloc               user allocation function
 */
const char *       openiccConfig_DeviceGet (
                                       openiccConfig_s   * config,
                                       const char       ** device_classes,
                                       int                 pos,
                                       char            *** keys,
                                       char            *** values,
                                       openiccAlloc_f      alloc,
                                       openiccDeAlloc_f    dealloc )
{
  int n = 0, i, count = 0;
  const char * actual_device_class = 0;

  if(config)
  {
    const char * base_path[] = {"org","freedesktop","openicc","device",0};
    oyjl_val base = oyjlTreeGet( config->oyjl, (const char**)base_path,
                                   oyjl_t_object );
    if(base)
    {
      oyjl_val dev_class;
      {
        int device_classes_n = 0;

        device_classes = openiccConfigGetDeviceClasses( device_classes,
                                       &device_classes_n );

        for(i = 0; i < device_classes_n; ++i)
        {
          const char * obj_key[] = { device_classes[i], 0 };
          int j = 1;
          oyjl_val device = 0;
          dev_class = oyjlTreeGet( base, obj_key, oyjl_t_array );
          if(dev_class)
          {
            int elements = dev_class->u.array.len;
            for(j = 0; j < elements; ++j)
            {
              device = dev_class->u.array.values[j];
              if(n == pos)
              {
                actual_device_class = device_classes[i];
                if(OYJL_IS_OBJECT( device ))
                {
                  count = device->u.object.len;
                  oyjlAllocHelper_m(*keys, char*, count + 1, alloc, return NULL);
                  oyjlAllocHelper_m(*values, char*, count + 1, alloc, goto clean_openiccConfig_DeviceGet);
                  for(i = 0; i < count; ++i)
                  {
                    if(device->u.object.keys[i] && device->u.object.keys[i][0])
                    {
                      oyjlAllocHelper_m((*keys)[i], char, strlen(device->u.object.keys[i]) + 1, alloc, goto clean_openiccConfig_DeviceGet);
                      strcpy( (*keys)[i], device->u.object.keys[i] );
                    }
                    if(device->u.object.values[i])
                    {
                      char * t = 0;
                      const char * tmp = NULL, * tmp2 = NULL;
                      switch(device->u.object.values[i]->type)
                      {
                        case oyjl_t_string:
                             tmp = device->u.object.values[i]->u.string; break;
                        case oyjl_t_number:
                             tmp = device->u.object.values[i]->u.number.r;break;
                        case oyjl_t_array:
                             {
                               int k = 0,
                                   n = device->u.object.values[i]->u.array.len;
                               STRING_ADD( t, "[" );
                               for(k = 0; k < n; ++k)
                               {
                                 if(device->u.object.values[i]->
                                    u.array.values[k]->type == oyjl_t_string)
                                   tmp2 = device->u.object.values[i]->
                                         u.array.values[k]->u.string;
                                 else
                                 if(device->u.object.values[i]->
                                    u.array.values[k]->type == oyjl_t_number)
                                   tmp2 = device->u.object.values[i]->
                                         u.array.values[k]->u.number.r;

                                 if(tmp2)
                                 {
                                   if(k != 0)
                                   STRING_ADD( t, "," );
                                   STRING_ADD( t, "\"" );
                                   STRING_ADD( t, tmp2 );
                                   STRING_ADD( t, "\"" );
                                   tmp = t;
                                 }
                               }
                               STRING_ADD( t, "]" );
                               tmp = t;
                             }
                             break;
                        default:
                             tmp = "no string or number"; break;
                      }
                      if(!tmp)
                        tmp = "no value found";
                      oyjlAllocHelper_m((*values)[i], char, strlen(tmp) + 1, alloc, goto clean_openiccConfig_DeviceGet);
                      strcpy( (*values)[i], tmp );
                    }
                  }
                }
                break;
              }
              if(device)
                ++n;
            }
          }
        }
      }
    } else
      WARNcc_S( config, "could not find " OPENICC_DEVICE_PATH " %s",
                config->info ? config->info : "" );
  }

  return actual_device_class;

clean_openiccConfig_DeviceGet:
  if(*keys)
  {
    for(i = 0; i < count; ++i)
      if((*keys)[i]) dealloc((*keys)[i]);
    dealloc(*keys);
  }
  if(*values)
  {
    for(i = 0; i < count; ++i)
      if((*values)[i]) dealloc((*values)[i]);
    dealloc(*values);
  }
  return NULL;
}

/**
 *  @brief   obtain a JSON string
 *  @memberof openiccConfig_s
 * 
 *  @param[in]     config              a data base object
 *  @param[in]     device_classes      a zero terminated list of device class
 *                                     strings
 *  @param[in]     pos                 device position in list
 *  @param[in]     flags               - OPENICC_CONFIGS_SKIP_HEADER
 *                                     - OPENICC_CONFIGS_SKIP_FOOTER
 *  @param[in]     device_class        the last written device class
 *  @param[out]    json                the resulting JSON string allocated by
 *                                     alloc
 *  @param[in]     alloc               user allocation function
 *  @return                            device class
 */
const char *       openiccConfig_DeviceGetJSON (
                                       openiccConfig_s   * config,
                                       const char       ** device_classes,
                                       int                 pos,
                                       int                 flags,
                                       const char        * device_class,
                                       char             ** json,
                                       openiccAlloc_f      alloc,
                                       openiccDeAlloc_f    dealloc )
{
  char            ** keys = 0;
  char            ** values = 0;
  int j, n = 0;
  char * txt = 0;

  const char * d = openiccConfig_DeviceGet( config, device_classes, pos,
                                            &keys, &values, malloc, dealloc );

  if(alloc)
    txt = alloc(4096);
  else
    txt = calloc( sizeof(char), 4096 );

  if(txt)
    txt[0] = '\000';
  else
  {
    ERRcc_S( config, "could not allocate 4096 bytes",0 );
    return txt;
  }

  if(!(flags & OPENICC_CONFIGS_SKIP_HEADER))
    sprintf( txt, OPENICC_DEVICE_JSON_HEADER, d );
  else if(device_class)
  {
    if(d != device_class)
      sprintf( &txt[strlen(txt)],
  "\n          ],\n          \"%s\": [{\n", d);
    else
        /* end the previous JSON array field and open the next one */
      sprintf( &txt[strlen(txt)], ",\n            {\n");
  }

    n = 0; if(keys) while(keys[n]) ++n;
    for( j = 0; j < n; ++j )
    {
      char * val = values[j];
      if(val[0] != '[')
        sprintf( &txt[strlen(txt)],
  "              \"%s\": \"%s\"", keys[j], val);
      else
        sprintf( &txt[strlen(txt)],
  "              \"%s\": %s", keys[j], val);
      if(j < n-1)
        sprintf( &txt[strlen(txt)], ",");
      sprintf( &txt[strlen(txt)], "\n");
      free(keys[j]);
      free(val);
    }
    if(keys) {free(keys);} if(values) {free(values);}

    /* close the object */
  if(!(flags & OPENICC_CONFIGS_SKIP_FOOTER))
    sprintf( &txt[strlen(txt)], "            }\n" OPENICC_DEVICE_JSON_FOOTER);
  else
    sprintf( &txt[strlen(txt)], "            }" );

  *json = txt;

  return d;
}

/**
 *  @brief   find out the device class of a given data base entry
 *  @memberof openiccConfig_s
 *
 *  @param[in]     config              a data base entry object
 *  @param[in]     alloc               user allocation function
 */
char *             openiccConfig_DeviceClassGet (
                                       openiccConfig_s   * config,
                                       openiccAlloc_f      alloc )
{
  char * device_class = 0;

  if(config)
  {
    const char * base_path[] = {"org","freedesktop","openicc","device",0};
    oyjl_val base = oyjlTreeGet( config->oyjl, (const char**)base_path,
                                   oyjl_t_object );
    if(base && OYJL_IS_OBJECT( base ))
    {
      oyjl_val v = base;

      if(v->u.object.keys[0] && v->u.object.keys[0][0])
      {
        device_class = oyjlStringCopy((v->u.object.keys[0]), malloc);
        if(!device_class)
          ERRcc_S( config, "could not allocate string%s", "" );
      }
    } else
      WARNcc_S( config, "could not find " OPENICC_DEVICE_PATH " %s",
                config->info ? config->info : "" );
  }

  if(alloc != malloc && device_class)
  {
    char * custom = oyjlStringCopy( device_class, alloc );
    free(device_class);
    device_class = custom; custom = NULL;
  }

  return device_class;
}

/**
 *  @brief    get a filtered list of key names
 *  @memberof openiccConfig_s
 *
 *  @param[in]     config              a data base entry object
 *  @param[in]     xpath               top key name to filter for
 *  @param[in]     child_levels        how deeply nested child levels are
 *                                     desired; 0 - means all levels
 *  @param[in]     alloc               user allocation function; optional -
 *                                     default: malloc
 *  @param[out]    key_names           found full keys with path part; optional
 *  @param[out]    n                   number of found keys; optional
 *  @return                            0 - success, >=1 - error, <0 - issue
 */
int                openiccConfig_GetKeyNames (
                                       openiccConfig_s   * config,
                                       const char        * xpath,
                                       int                 child_levels,
                                       openiccAlloc_f      alloc,
                                       char            *** key_names,
                                       int               * n )
{
  int error = !config || !xpath;
  oyjl_val list = NULL;
  int count = 0, i;
  char ** keys = (char**)  calloc(sizeof(char*),2);

  if(!keys) return 1;

  if(!error)
    list = oyjlTreeGetValue( config->oyjl, 0, xpath );

  if(!error)
    error = !list ? -1:0;

  if(!error)
  {
    keys[0] = oyjlStringCopy( xpath, malloc );
    error = !keys[0];
  }

  if(!error)
    keys = oyjlTreeToPaths( list, child_levels, NULL, 0, &count );

  if(!error && n)
    *n = count?count-1:0;

  if(!error && key_names && keys)
  {
    /* the first key comes from this function is is artifical: remove it */
    free(keys[0]);
    for(i=0; i < count-1; ++i)
      keys[i] = keys[i+1];
    keys[--count] = NULL;
    if(alloc && alloc != malloc)
    {
      char ** l = (char**) alloc(sizeof(char*) * (count+1));
      for(i=0; i < count; ++i)
        l[i] = oyjlStringCopy( keys[i], alloc );
      oyjlStringListRelease( &keys, count, free );
      keys = l;
      l = NULL;
    }
    *key_names = keys;
  } else
    oyjlStringListRelease( &keys, count, free );

  return error;
}


/**
 *  @brief    get a value
 *  @memberof openiccConfig_s
 *
 *  @param[in]     config              a data base entry object
 *  @param[in]     xpath               key name to ask for
 *  @param[out]    value               found value; optional
 *  @return                            0 - success, >=1 - error, <0 - issue
 */
int                openiccConfig_GetString (
                                       openiccConfig_s   * config,
                                       const char        * xpath,
                                       const char       ** value )
{
  int error = !config || !xpath;
  oyjl_val o = NULL;
  const char * string = NULL;

  if(error == 0)
  {
    o = oyjlTreeGetValue( config->oyjl, 0, xpath );
    error = !o ? -1:0;
  }

  if(error == 0)
  {
    switch(o->type)
    {
      case oyjl_t_string:
        string = o->u.string; break;
      case oyjl_t_number:
        string = o->u.number.r; break;
      default:
        string = NULL; break;
    }
  }

  if(value)
    *value = string;

  return error;
}

/**
 *  @brief    get a value
 *  @memberof openiccConfig_s
 *
 *  @param[in]     config              a data base entry object
 *  @param[out]    value               found value
 *  @param[in]     format              full key name to ask for
 *
 *  @return                            0 - success, >=1 - error, <0 - issue
 */
int                openiccConfig_GetStringf (
                                       openiccConfig_s   * config,
                                       const char       ** value,
                                       const char        * format,
                                       ... )
{
  int error;

  char * text = 0;
  va_list list;
  int len;
  size_t sz = strlen(format) * 2;

  text = malloc( sz );
  if(!text)
  {
    fprintf( stderr, "!!! ERROR: could not allocate memory\n" );
    return 0;
  }

  text[0] = 0;

  va_start( list, format);
  len = vsnprintf( text, sz, format, list );
  va_end  ( list );

  if (len >= (int)sz)
  {
    text = realloc( text, (len+1)*sizeof(char) );
    if(!text) return 0;
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list );
    va_end  ( list );
  }

  error = openiccConfig_GetString( config, text, value );

  if(text) free(text);

  return error;
}

/**
 *  @brief    get a set of values
 *  @memberof openiccConfig_s
 *
 *  @param[in]     config              a data base entry object
 *  @param[in]     xpaths              key names to use
 *  @param[in]     alloc               user allocation function; optional -
 *                                     default: malloc
 *  @param[out]    values              found values; optional
 *  @param[out]    n                   number of found values; optional
 *
 *  @return                            0 - success, >=1 - error, <0 - issue
 */
int                openiccConfig_GetStrings (
                                       openiccConfig_s   * config,
                                       const char       ** xpaths,
                                       openiccAlloc_f      alloc,
                                       char            *** values,
                                       int               * n )
{
  int error = !config || !xpaths;
  int count = 0, i, pos = 0;
  size_t size;
  char ** vals = NULL;

  if(!error)
  {
    count = 0;
    while(xpaths[count]) ++count;
    size = sizeof(char*) * (count + 1);
  }

  if(!alloc)
    alloc = malloc;

  if(count)
  {
    vals = alloc(size);
    error = !vals;
    if(!error)
      memset( vals, 0, size );
    else
      ERRcc_S( config, "could not allocate : %lu", (unsigned long)size );
  }

  if(error <= 0)
  for( i = 0; i < count; ++i )
  {
    const char * t = NULL;
    error = openiccConfig_GetString( config, xpaths[i], &t );
    if(t)
    {
      if(values)
      {
        vals[pos] = oyjlStringCopy( t, alloc );
        if(!vals[pos])
        {
          ERRcc_S( config, "could not allocate string : %s", t );
          break;
        }
      }
      pos++;
    }
  }

  if(values)
    *values = vals;

  if(n)
    *n = pos;

  return error;
}


/** \addtogroup path_names
 *  @{
 */

/**
 *  @brief get Path Name for Installation 
 *
 *  Note: Not all combinations return a path name. Some make no sense.
 *  So be careful and test the result.
 *
 *  ::openiccPATH_MODULE + ::openiccSCOPE_USER and ::openiccPATH_MODULE + ::openiccSCOPE_OPENICC are
 *  supported.
 *
 *  @version OpenICC: 0.1.1
 *  @date    2018/06/09
 *  @since   2015/02/08 (OpenICC: 0.1.0)
 */
char *       openiccGetInstallPath   ( openiccPATH_TYPE_e  type,
                                       openiccSCOPE_e      scope,
                                       openiccAlloc_f      allocFunc )
{
  char * path = NULL;
#define C(p) oyjlStringCopy(p,allocFunc);
  switch (type)
  {
    case openiccPATH_ICC:
      switch((int)scope)
      {
        case openiccSCOPE_USER:
          path = C( OS_ICC_USER_DIR );
          break;
        case openiccSCOPE_SYSTEM:
          path = C( OS_ICC_SYSTEM_DIR ) ;
          break;
        case openiccSCOPE_OPENICC:
          path = C( OPENICC_SYSCOLORDIR OPENICC_SLASH OPENICC_ICCDIRNAME );
          break;
        case openiccSCOPE_MACHINE:
          path = C( OS_ICC_MACHINE_DIR );
        break;
        default:
          path = NULL;
      }
      break;
    case openiccPATH_POLICY:
    {
      switch((int)scope)
      {
        case openiccSCOPE_USER:
          path = C( OS_SETTINGS_USER_DIR );
          break;
        case openiccSCOPE_SYSTEM:
          path = C( OS_SETTINGS_SYSTEM_DIR );
          break;
        case openiccSCOPE_OPENICC:
          path = C( OPENICC_SYSCOLORDIR OPENICC_SLASH OPENICC_SETTINGSDIRNAME);
          break;
        case openiccSCOPE_MACHINE:
          path = C( OS_SETTINGS_MACHINE_DIR );
        break;
      }
      break;
    }
    case openiccPATH_MODULE:
    {
      switch((int)scope)
      {
        case openiccSCOPE_USER:
        {
          char * t = NULL;
          oyjlStringAdd( &t, 0,0,
                             "~/.local/lib%s/" OPENICC_CMMSUBPATH, strstr(OPENICC_LIBDIR, "lib64") ? "64":"");
          path = C( t );
          if(t) {free(t); t = NULL;}
          break;
        }
        case openiccSCOPE_OPENICC:
          path = C( OPENICC_CMMDIR );
          break;
        default:
          path = NULL;
      }
      break;
    }
    case openiccPATH_CACHE:
    {
      switch((int)scope)
      {
        case openiccSCOPE_USER:
          path = C( OS_DL_CACHE_USER_DIR );
          break;
        case openiccSCOPE_SYSTEM:
          path = C( OS_DL_CACHE_SYSTEM_DIR );
          break;
        default:
          path = NULL;
      }
      break;
    }
    case openiccPATH_LOGO:
    {
      switch((int)scope)
      {
        case openiccSCOPE_USER:
          path = C( OS_LOGO_USER_DIR );
          break;
        case openiccSCOPE_SYSTEM:
          path = C( OS_LOGO_SYSTEM_DIR );
          break;
        case openiccSCOPE_OPENICC:
          path = C( OPENICC_DATADIR OPENICC_SLASH OPENICC_PIXMAPSDIRNAME );
          break;
        case openiccSCOPE_MACHINE:
          path = NULL;
        break;
      }
      break;
    }
    default:
      path = NULL;
  }

  return path;
}

/*  @} *//* path_names */

