/** @file test.c
 *
 *  libOpenICC - OpenICC Colour Management Configuration
 *
 *  Copyright (C) 2011-2018  Kai-Uwe Behrmann
 *
 *  @brief    OpenICC test suite
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2011/06/27
 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openicc_config.h"
#include "openicc_version.h"
#include "openicc_config_internal.h"

#define TESTS_RUN \
  TEST_RUN( testVersion, "Version matching", 1 ); \
  TEST_RUN( testI18N, "i18n", 1 ); \
  TEST_RUN( testIO, "file i/o", 1 ); \
  TEST_RUN( testStringRun, "String handling", 1 ); \
  TEST_RUN( testConfig, "JSON handling", 1 ); \
  TEST_RUN( testDeviceJSON, "Device JSON handling", 1 ); \
  TEST_RUN( testPaths, "Paths", 1 ); \
  TEST_RUN( testXDG, "XDG", 1 ); \
  TEST_RUN( testODB, "ODB", 1 ); \
  //TEST_RUN( testDB, "DB" );

void oyjlLibRelease();
#define OYJL_TEST_MAIN_SETUP  printf("\n    OpenICC Test Program\n");
#define OYJL_TEST_MAIN_FINISH printf("\n    OpenICC Test Program finished\n\n"); oyjlLibRelease(); if(oyjl_print_sub) free(oyjl_print_sub);
#define OYJL_TEST_NAME "test-core"
#include "oyjl_test.h"
#include "oyjl_test_main.h"

#define free_m_( ptr_ ) { if(ptr_) {free(ptr_); ptr_ = NULL;} }

/* --- actual tests --- */

oyjlTESTRESULT_e testVersion()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );
  fprintf(zout, "compiled version:     %d\n", OPENICC_VERSION );
  fprintf(zout, " runtime version:     %d\n", openiccVersion() );

  if(OPENICC_VERSION == openiccVersion())
    result = oyjlTESTRESULT_SUCCESS;
  else
    result = oyjlTESTRESULT_FAIL;

  return result;
}

#include <locale.h>

oyjlTESTRESULT_e testI18N()
{
  const char * lang = 0, * text, * loc;
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  loc = setlocale(LC_ALL,"de_DE.UTF8");
  openiccInit( loc );

  lang = setlocale(LC_ALL, NULL);
  if(lang && (strcmp(lang, "C") != 0))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "setlocale() initialised good %s            ", lang );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "setlocale() initialised failed %s          ", lang );
  }

  text = _("Camera");
  if(strcmp(text,"Kamera") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "dgettext() good \"%s\"                      ", text );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL, 
    "dgettext() failed \"%s\"                    ", text );
  }

  setlocale(LC_ALL,"");

  return result;
}


oyjlTESTRESULT_e testPaths()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  const char * type_names[] = {
    "openiccPATH_NONE", "openiccPATH_ICC", "openiccPATH_POLICY", "openiccPATH_MODULE", "openiccPATH_SCRIPT", "openiccPATH_CACHE", "openiccPATH_LOGO"
  };
  openiccPATH_TYPE_e types[] = {
    openiccPATH_NONE, openiccPATH_ICC, openiccPATH_POLICY, openiccPATH_MODULE, openiccPATH_SCRIPT, openiccPATH_CACHE, openiccPATH_LOGO
  };
  const char * scope_names[] = {
    "openiccSCOPE_USER_SYS", "openiccSCOPE_USER", "openiccSCOPE_SYSTEM", "openiccSCOPE_OPENICC", "openiccSCOPE_MACHINE"
  };
  openiccSCOPE_e scopes[] = {
    openiccSCOPE_USER_SYS, openiccSCOPE_USER, openiccSCOPE_SYSTEM, (openiccSCOPE_e)openiccSCOPE_OPENICC, (openiccSCOPE_e)openiccSCOPE_MACHINE
  };
  int i,j;

  for(i = 1; i <= 6; ++i)
  for(j = 1; j <= 4; ++j)
  {
  char * text = openiccGetInstallPath( types[i], scopes[j], malloc );
  if(text)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccGetInstallPath( %s, %s ): %s", type_names[i],scope_names[j],
                                                openiccNoEmptyString_m_(text) );
    free_m_(text);
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "openiccGetInstallPath( %s, %s ): %s", type_names[i],scope_names[j],
                                                openiccNoEmptyString_m_(text) );
  }
  }


  return result;
}

const char * oiGetConfigFileName()
{
  const char * file_name = "../../../OpenICC_device_config_DB.json";
  FILE * fp = fopen( file_name, "r" );

  if(!fp)
  {
    file_name = "../OpenICC_device_config_DB.json";
    fp = fopen( file_name, "r" );
    if(!fp)
    {
      file_name = "OpenICC_device_config_DB.json";
      fp = fopen( file_name, "r" );
      if(!fp)
      {
        file_name = "../openicc/OpenICC_device_config_DB.json";
        fp = fopen( file_name, "r" );
        if(!fp)
        {
          file_name = "../config/OpenICC_device_config_DB.json";
          fp = fopen( file_name, "r" );
          if(!fp)
          {
            file_name = "../../../config/OpenICC_device_config_DB.json";
            fp = fopen( file_name, "r" );
            if(!fp)
            {
              file_name = "../../../openicc/OpenICC_device_config_DB.json";
              fp = fopen( file_name, "r" );
              if(!fp)
                file_name = NULL;
            }
          }
        }
      }
    }
  }

  if(fp)
  {
    fclose(fp);
    fprintf(zout, "\tfile_name: %s\n", file_name );
  } else
  {
    fprintf(zout, "\tfile_name: \"\"\npwd: " );
    system("pwd");
  }

  return file_name;
}


int openiccMakeDir_ (const char *);
oyjlTESTRESULT_e testIO ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int error = 0;
  int odo = 0;

  char * t1 = NULL, *t2 = NULL, *t3 = NULL;
  const char * file_name = "/usr/share/color/icc/OpenICC/sRGB.icc";
  int size = 0;
  FILE * fp = NULL;

  t1 = openiccExtractPathFromFileName_( file_name );
  fprintf(zout, "file_name: %s\n", file_name );
  error = !t1 || (strlen(t1) >= strlen(file_name));
  if(!error)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccExtractPathFromFileName_() %s", t1 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccExtractPathFromFileName_()                  " );
  }

  file_name = "../icc/OpenICC/sRGB.icc";
  t2 = openiccExtractPathFromFileName_( file_name );
  fprintf(zout, "file_name: %s\n", file_name );
  error = !t2 || (strlen(t2) >= strlen(file_name));
  if(!error)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccExtractPathFromFileName_() %s", t2 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccExtractPathFromFileName_() %s               ", t2?t2:"" );
  }

  file_name = "~/.local/share/color/icc/OpenICC/sRGB.icc";
  t3 = openiccExtractPathFromFileName_( file_name );
  fprintf(zout, "file_name: %s\n", file_name );
  error = !t3 || (strlen(t3) >= strlen(file_name));
  if(!error)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccExtractPathFromFileName_() %s", t3 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccExtractPathFromFileName_() %s               ", t3?t3:"" );
  }

  if(openiccIsDirFull_("/usr/share/color/icc"))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccIsDirFull_() %s        ", "/usr/share/color/icc" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccIsDirFull_() %s               ", "/usr/share/color/icc" );
  }

  if(openiccIsDirFull_(t2))
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccIsDirFull_() ! %s", t2 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccIsDirFull_() ! %s               ", t2?t2:"" );
  }

  if(openiccIsDirFull_("/not/existing"))
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccIsDirFull_(/not/existing) !               " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccIsDirFull_(/not/existing) !               " );
  }

  free_m_(t3);

  t3 = openiccPathGetParent_(OPENICC_DEVICE_PATH);
  fprintf(zout, "name: %s\n", OPENICC_DEVICE_PATH );
  error = !t3 || (strlen(t3) >= strlen(OPENICC_DEVICE_PATH));
  if(!error)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccPathGetParent_() %s  ", t3 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccPathGetParent_() %s               ", t3?t3:"" );
  }
  free_m_(t3);

  t3 = openiccPathGetParent_(OPENICC_DEVICE_PATH "/");
  fprintf(zout, "name: %s\n", OPENICC_DEVICE_PATH "/" );
  error = !t3 || (strlen(t3) >= strlen(OPENICC_DEVICE_PATH "/"));
  if(!error)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccPathGetParent_() %s  ", t3 );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccPathGetParent_() %s               ", t3?t3:"" );
  }

  free_m_(t1);
  free_m_(t2);
  free_m_(t3);

  odo = *openicc_debug;
  *openicc_debug = 2;

  if(openiccIsFileFull_("/not/existing", "r"))
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccIsFileFull_(\"/not/existing\", \"r\") !   " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccIsFileFull_(\"/not/existing\", \"r\") !   " );
  }

  if(openiccIsFileFull_("/etc/shadow", "rw"))
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccIsFileFull_(/etc/shadow) !             " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccIsFileFull_(/etc/shadow) !             " );
  }

  file_name = oiGetConfigFileName();

  if(openiccIsFileFull_("................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................................", "rw"))
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccIsFileFull_(longlong) !                " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccIsFileFull_(longlong) !                " );
  }

  if(openiccIsFileFull_(file_name, "r"))
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccIsFileFull_()                          " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccIsFileFull_()                          " );
  }

  t1 = openiccReadFile( file_name, &size );
  if(t1 && size == 2394)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccReadFile() &size %d                    ", size );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccReadFile() %s    ", file_name );
  }
  free_m_(t1);

  t1 = openiccReadFile( "not_existing.file", &size );
  if(t1 == NULL)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccReadFile(not existing) &size %d           ", size );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccReadFile(not existing) %s    ", file_name );
  }
  free_m_(t1);

  error = openiccMakeDir_ ("");
  if(!error)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccMakeDir_(\"\")  %d                       ", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccMakeDir_(\"\")  %d                       ", error );
  }

  error = openiccMakeDir_ (NULL);
  if(error)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccMakeDir_(NULL)  %d                       ", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccMakeDir_(NULL)  %d                       ", error );
  }

  error = openiccMakeDir_ ("/never");
  if(error)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccMakeDir_(/never)  %d                       ", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccMakeDir_(/never)  %d                       ", error );
  }

  error = openiccMakeDir_ ("/etc/never/ever");
  if(error)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccMakeDir_(/etc/never/ever)  %d              ", error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccMakeDir_(/etc/never/ever)  %d              ", error );
  }
  *openicc_debug = odo;

  t1 = openiccReadFile( file_name, &size );
  file_name = "test.txt";
  if(t1)
    size = strlen(t1);
  size = openiccWriteFile( file_name,
                           t1,
                           size );
  if(size)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccWriteFile() size %d                    ", size );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccWriteFile() %s                ", file_name );
  }
  free_m_(t1);

  fp = fopen( file_name, "r" );
  size = 0;
  error = openiccReadFileSToMem( fp, &t1, &size );
  if(t1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccReadFileSToMem() &size %u                 ", (unsigned)size );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccReadFileSToMem() %s    ", file_name );
  }
  free_m_(t1);
  if(fp) fclose(fp);

  return result;
}

oyjlTESTRESULT_e testStringRun ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  fprintf(stdout, "\n" );

  int error = 0;

  char * t = NULL;
  openiccConfig_s * config = NULL;

  openiccMessageFuncSet( openiccMessageFunc );
  error = openiccMessage_p( openiccMSG_WARN, config, "test message %s", OPENICC_VERSION_NAME );

  if( !error )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccMessage_p()...                              " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccMessage_p()...                              " );
  }


  oyjlStringAdd( &t, 0,0, OPENICC_BASE_PATH "%s", "/behaviour");
  if( t && strlen(t) > strlen(OPENICC_BASE_PATH) )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "oyjlStringAdd() %s", t );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "oyjlStringAdd() ...                      " );
  }

  STRING_ADD( t, "/rendering_intent" );
  if( t && strlen(t) > 40 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccStringAdd_() %s", t );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccStringAdd_() ...                            " );
  }
  free_m_(t);

  return result;
}

oyjlTESTRESULT_e testConfig()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  openiccConfig_s * config;
  const char * file_name;
  char * text;
  int size = 0;
  int key_names_n = 0, values_n = 0,i,
      error OI_UNUSED = 0;
  char ** key_names = NULL, ** values = NULL;
  const char * base_key;
  oyjl_val root, array, v;
  char * json = NULL;
  int level = 0;

  fprintf(stdout, "\n" );

  /* test JSON generation */
  root = (oyjl_val) calloc( sizeof(struct oyjl_val_s), 1 );
  base_key = "org/freedesktop/openicc/[]";
  array = oyjlTreeGetValue( root, OYJL_CREATE_NEW, base_key );
  v = oyjlTreeGetValue( array, OYJL_CREATE_NEW, "my_key_A" );
  oyjlValueSetString( v, "my_value_A" );
  v = oyjlTreeGetValue( array, OYJL_CREATE_NEW, "my_key_B" );
  oyjlValueSetString( v, "my_value_B" );
  oyjlTreeToJson( root, &level, &json ); level = 0;
  if(root && json)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "oyjlTreeGetValue(root,OYJL_CREATE_NEW,\"%s\")", base_key );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "oyjlTreeGetValue(root,OYJL_CREATE_NEW,\"%s\")", base_key );
  }
  oyjlTreeFree( root ); root = NULL;
  if(verbose)
    fprintf(zout, "%s\n", json );
  free_m_(json);


  base_key = "org/freedesktop/openicc/device/camera/[1]";
  file_name = oiGetConfigFileName();

  /* read JSON input file */
  text = openiccReadFile( file_name, &size );

  /* parse json ... */
  root = oyjlTreeParse( text, NULL, 0 );
  
  /* and write back */
  oyjlTreeToJson( root, &level, &json );
  oyjlTreeFree( root ); root = NULL;
  free_m_(text);


  /* parse JSON */
  config = openiccConfig_FromMem( json );
  free_m_(json);
  openiccConfig_SetInfo ( config, file_name );

  if(config)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "config created                                 " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "config created                                 " );
  }

  /* Get key names below some point in the JSON tree. */
  error = openiccConfig_GetKeyNames( config, base_key, 0,
                                     myAllocFunc, &key_names, &key_names_n );
  if(key_names_n)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccConfig_GetKeyNames()                  %d", key_names_n );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccConfig_GetKeyNames()                    " );
  }

  /* Get values for the above key names. */
  error = openiccConfig_GetStrings( config, (const char **)key_names,
                                    myAllocFunc, &values, &values_n );
  i = 0;
  while(values && values[i]) ++i;
  if(key_names_n == values_n && values_n == i)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccConfig_GetStrings()             %d==%d==%d", key_names_n,values_n,i );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccConfig_GetStrings()             %d==%d==%d", key_names_n,values_n,i );
  }

  if(key_names)
  for(i = 0; i < key_names_n; ++i)
  {
    /* Get a single value from the config object by conviniently contructing
     * the key name.
     */
    const char * t = NULL;
    openiccConfig_GetStringf( config, &t, "%s/[%d]", base_key, i );
    if(!t)
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccConfig_GetString()                      " );
    }
    if(verbose)
      fprintf(zout, "\t%s:\t\"%s\"\n", (key_names && key_names[i])?key_names[i]:"????", t?t:"????" );
    myDeAllocFunc( key_names[i] );
    if(values && values[i]) myDeAllocFunc(values[i]);
  }

  if( key_names ) { myDeAllocFunc(key_names); key_names = NULL; }
  if( values ) { myDeAllocFunc(values); values = NULL; }


  /* get all key names */
  error = openiccConfig_GetKeyNames( config, "org", 0,
                                     myAllocFunc, &key_names, &i );
  if(key_names_n < i )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccConfig_GetKeyNames(many levels)      %d<%d", key_names_n,i );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccConfig_GetKeyNames(many levels)      %d<%d", key_names_n,i );
  }
  key_names_n = i;
  if(verbose)
  {
    for(i = 0; i < key_names_n; ++i)
    {
      const char * key = key_names[i];
      const char * t = NULL;
      openiccConfig_GetStringf( config, &t, "%s", key );

      fprintf(zout, "\t[%d]%s: %s%s%s\n", i, key, t?"\"":"", t?t:"XPATH", t?"\"":"");
    }
  }
  oyjlStringListRelease( &key_names, i, myDeAllocFunc );


  /* get one key name */
  error = openiccConfig_GetKeyNames( config, "org", 1,
                                     myAllocFunc, &key_names, &i );
  if(i == 1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccConfig_GetKeyNames(\"org\",one level) 1 == %d", i );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "openiccConfig_GetKeyNames(\"org\",one level) 1 == %d", i );
  }
  i = 0;
  if(verbose)
    fprintf(zout, "\t%s\n", (key_names && key_names[i])?key_names[i]:"????" );
  while(key_names && key_names[i]) myDeAllocFunc( key_names[i++] );
  if( key_names ) { myDeAllocFunc(key_names); key_names = NULL; }



  openiccConfig_Release( &config );

  return result;
}

oyjlTESTRESULT_e testDeviceJSON ()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;

  openiccConfig_s * config, * config2;
  const char * file_name;
  char * text = 0;
  char            ** keys = 0;
  char            ** values = 0;
  int i,j, n = 0, devices_n, flags;
  char * json, * full_json = NULL, * device_class;
  const char * devices_filter[] = {OPENICC_DEVICE_CAMERA,NULL},
             * old_device_class = NULL,
             * d = NULL;
  int size = 0;

  const char * non_json = "{\"org\":{\"free{\"openicc\")))";

  fprintf(stdout, "\n" );

  config = openiccConfig_FromMem( non_json );
  if( !config )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccConfig_FromMem(\"%s\") ", non_json );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "openiccConfig_FromMem(\"%s\") ", non_json );
  }
  openiccConfig_Release( &config );

  file_name = oiGetConfigFileName();
  /* read JSON input file */
  text = openiccReadFile( file_name, &size );

  /* parse JSON */
  config = openiccConfig_FromMem( text );
  free_m_(text);
  openiccConfig_SetInfo ( config, file_name );
  devices_n = openiccConfig_DevicesCount(config, NULL);
  if(verbose)
    fprintf( zout, "Found %d devices.\n", devices_n );
  if( devices_n )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccConfig_FromMem(\"%s\") %d ", file_name, devices_n );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "openiccConfig_FromMem()...                        " );
  }


  
  /* print all found key/value pairs */
  for(i = 0; i < devices_n; ++i)
  {
    const char * d = openiccConfig_DeviceGet( config, NULL, i,
                                               &keys, &values, malloc,free );

    if(i && verbose)
      fprintf( zout,"\n");

    n = 0; if(keys) while(keys[n]) ++n;
    if(verbose)
      fprintf( zout, "[%d] device class:\"%s\" with %d key/value pairs\n", i, d, n);
    for( j = 0; j < n; ++j )
    {
      if(verbose)
        fprintf(zout, "%s:\"%s\"\n", keys[j], values[j]);
      free_m_(keys[j]);
      free_m_(values[j]);
    }
    free_m_(keys); free_m_(values);
  }
  if(verbose)
    fprintf(zout, "\n" );

  /* get a single JSON device */
  i = 2; /* select the second one, we start counting from zero */
  d = openiccConfig_DeviceGetJSON ( config, NULL, i, 0,
                                     old_device_class, &json, malloc,free );
  config2 = openiccConfig_FromMem( json );
  openiccConfig_SetInfo ( config2, file_name );
  device_class = openiccConfig_DeviceClassGet( config2, malloc );
  if( device_class && strcmp(device_class,"camera") == 0 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccConfig_DeviceClassGet([%d]) %s      ", i, device_class );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "openiccConfig_DeviceClassGet()...                 " );
  }
  free_m_(json);
  free_m_(device_class);
  openiccConfig_Release( &config2 );


  /* we want a single device class DB for lets say cameras */
  devices_n = openiccConfig_DevicesCount(config, devices_filter);
  if( devices_n == 2 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccConfig_DevicesCount(%s) %d          ", OPENICC_DEVICE_CAMERA, devices_n );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "openiccConfig_DevicesCount()...                 " );
  }
  old_device_class = NULL;
  for(i = 0; i < devices_n; ++i)
  {
    flags = 0;
    if(i != 0) /* not the first */
      flags |= OPENICC_CONFIGS_SKIP_HEADER;
    if(i != devices_n - 1) /* not the last */
      flags |= OPENICC_CONFIGS_SKIP_FOOTER;

    d = openiccConfig_DeviceGetJSON( config, devices_filter, i, flags,
                                     old_device_class, &json, malloc,free );
    old_device_class = d;
    STRING_ADD( full_json, json );
    free_m_(json);
  }
  openiccConfig_Release( &config );


  config = openiccConfig_FromMem( full_json );
  openiccConfig_SetInfo ( config, "full_json" );
  free_m_(full_json);
  devices_n = openiccConfig_DevicesCount(config, NULL);
  if( devices_n == 2 )
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccConfig_DeviceGetJSON()                     " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "openiccConfig_DeviceGetJSON()                     " );
  }
  openiccConfig_Release( &config );


  return result;
}


#include "xdg_bds.h"
oyjlTESTRESULT_e testXDG()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  const char * config_file = OPENICC_DB_PREFIX OPENICC_SLASH OPENICC_DB;
  int i;
  /* Locate the directories where the config file is, */
  /* and where we should copy the profile to. */
  int npaths;
  xdg_error er;
  char **paths;
  openiccSCOPE_e scope = openiccSCOPE_USER;

  fprintf(stdout, "\n" );

  if ((npaths = xdg_bds(&er, &paths, xdg_conf, xdg_write, 
                        (scope == openiccSCOPE_SYSTEM) ? xdg_local : xdg_user,
                        config_file)) == 0)
  {
    ERRc_S("%s %d", _("Could not find config"), scope );
    return 1;
  }

  if(verbose)
  {
    fprintf( zout, "%s\n", _("Paths:") );
    for(i=0; i < npaths; ++i)
      fprintf( zout, "%s\n", paths[i]);
  }

  xdg_free(paths, npaths);

  if(npaths == 1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "xdg_bds openiccSCOPE_USER found %d             ", npaths );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "xdg_bds openiccSCOPE_USER found %d             ", npaths );
  }

  scope = openiccSCOPE_SYSTEM;
  if ((npaths = xdg_bds(&er, &paths, xdg_conf, xdg_write, 
                        (scope == openiccSCOPE_SYSTEM) ? xdg_local : xdg_user,
                        config_file)) == 0)
  {
    ERRc_S("%s %d", _("Could not find config"), scope );
  }

  if(verbose)
  {
    fprintf( zout, "%s\n", _("Paths:") );
    for(i=0; i < npaths; ++i)
      fprintf( zout, "%s\n", paths[i]);
  }

  xdg_free(paths, npaths);

  if(npaths == 1)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "xdg_bds openiccSCOPE_SYSTEM found %d           ", npaths );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "xdg_bds openiccSCOPE_SYSTEM found %d           ", npaths );
  }

  return result;
}

#include "openicc_db.h"
const char * openiccGetShortKeyFromFullKeyPath( const char * key, char ** temp );
oyjlTESTRESULT_e testODB()
{
  oyjlTESTRESULT_e result = oyjlTESTRESULT_UNKNOWN;
  openiccSCOPE_e scope = openiccSCOPE_USER_SYS;
  const char * top_key_name = OPENICC_DEVICE_PATH;
  openiccDB_s * db;
  const char * key = "org/freedesktop/openicc/device/camera/[0]/key";
  char * gkey, * temp, * temp2;
  char ** key_names = NULL;
  int key_names_n = 0, i,error = 0;

  fprintf(stdout, "\n" );

  db = openiccDB_NewFrom( top_key_name, scope );
  if(db)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "db created                                     " );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "db created                                     " );
  }

  error = openiccDB_GetKeyNames( db, "org", 0,
                                 myAllocFunc, myDeAllocFunc,
                                 NULL, &key_names_n );
  if(!key_names_n)
  {
    const char * db_file = oiGetConfigFileName();

    /* read JSON input file */
    int size = 0;
    char * text = openiccReadFile( db_file, &size );

    /* parse JSON */
    if(text)
    {
      int count = openiccArray_Count( (openiccArray_s*)&db->ks );
      openiccConfig_s * config = openiccConfig_FromMem( text );
      free_m_(text);
      openiccConfig_SetInfo ( config, db_file );

      /* reserve enough memory in list array */
      if( openiccArray_Push( (openiccArray_s*)&db->ks ))
      {
        ERRc_S("%s", _("Could not alloc memory") );
        return 1;
      }

      /* add new config to db */
      db->ks[count] = config;
    }
  }


  key = openiccGetShortKeyFromFullKeyPath( key, &temp );
  if(strcmp(key,"key") == 0 && !temp)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "openiccGetShortKeyFromFullKeyPath(xxx/key)    \"%s\"", key );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "openiccGetShortKeyFromFullKeyPath(xxx/key)    \"%s\"", key );
  }

  key = openiccGetShortKeyFromFullKeyPath( "key", &temp );
  if(strcmp(key,"key") == 0 && !temp)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "openiccGetShortKeyFromFullKeyPath(key)        \"%s\"", key );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "openiccGetShortKeyFromFullKeyPath(key)        \"%s\"", key );
  }

  key = openiccGetShortKeyFromFullKeyPath( "key.attribute", &temp );
  if(strcmp(key,"key") == 0 && temp)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "openiccGetShortKeyFromFullKeyPath(key.ignore) \"%s\"", key );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "openiccGetShortKeyFromFullKeyPath(key.ignore) \"%s\"", key );
  }
  free_m_(temp);

  key = openiccGetShortKeyFromFullKeyPath( "path/key.attribute", &temp );
  if(strcmp(key,"key") == 0 && temp)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS, 
    "openiccGetShortKeyFromFullKeyPath(path/key.ignore)" );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL, 
    "openiccGetShortKeyFromFullKeyPath(path/key.ignore)\"%s\"", key );
  }
  free_m_(temp);

  /* Get key names below some point in the JSON trees. */
  error = openiccDB_GetKeyNames( db, "org", 0,
                                 myAllocFunc, myDeAllocFunc,
                                 &key_names, &key_names_n );
  if(key_names_n)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccDB_GetKeyNames()                      %d", key_names_n );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccDB_GetKeyNames()                        " );
  }

  for(i = 0; i < key_names_n; ++i)
  {
    /* Get a single value from the config object by conviniently contructing
     * the key name.
     */
    const char * t = "";
    error = openiccDB_GetString( db, key_names[i], &t );
    if(error)
    { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccDB_GetString()                      " );
    }
    if(verbose)
      fprintf(zout, "\t%s:\t\"%s\"\n", key_names[i]?key_names[i]:"????", t?t:"" );
  }
  oyjlStringListRelease( &key_names, key_names_n, myDeAllocFunc );

  openiccDB_Release( &db );

  key = "org/freedesktop/openicc/device/camera";
  temp = openiccDBSearchEmptyKeyname( key, openiccSCOPE_USER );
  if(temp)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccDBSearchEmptyKeyname() %s", temp );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccDBSearchEmptyKeyname() %s", temp );
  }

  gkey = NULL;
  oyjlStringAdd( &gkey, 0,0, "%s%s", temp, "/my_test_key");
  error = openiccDBSetString( gkey, openiccSCOPE_SYSTEM, "my_test_value", "my_test_comment" );
  if(!error)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccDBSetString(%s)        %d", gkey, error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "openiccDBSetString(%s)        %d", gkey, error );
  }

  error = openiccDBSetString( gkey, openiccSCOPE_USER, "my_test_value", "my_test_comment" );
  if(!error)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccDBSetString(%s)        %d", gkey, error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "openiccDBSetString(%s)        %d", gkey, error );
  }

  const char * value = NULL;
  db = openiccDB_NewFrom( key, openiccSCOPE_USER_SYS );
  error = openiccDB_GetString( db, gkey, &value );
  if(error == 0 && value && strcmp(value, "my_test_value") == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccDB_GetString(%s)        %d", gkey, error, value );
  } else
  { PRINT_SUB( oyjlTESTRESULT_FAIL,
    "openiccDB_GetString(%s)        %d", gkey, error );
  }
  openiccDB_Release( &db );

  error = openiccDBSetString( gkey, openiccSCOPE_USER, NULL, "delete" );
  temp2 = openiccDBSearchEmptyKeyname( key, openiccSCOPE_USER );
  if(!error && temp && temp2 && strcmp(temp, temp2) == 0)
  { PRINT_SUB( oyjlTESTRESULT_SUCCESS,
    "openiccDBSetString(%s,\"delete\") %d", gkey, error );
  } else
  { PRINT_SUB( oyjlTESTRESULT_XFAIL,
    "openiccDBSetString(%s,\"delete\") %d", gkey, error );
  }


  free_m_(gkey);
  free_m_(temp);
  free_m_(temp2);

  return result;
}


/* --- end actual tests --- */



