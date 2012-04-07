// !cc -Wall -g test_device.c -o test_device2 -L. `oyranos-config --cflags --ldstaticflags` -lm -lltdl
#include <stdio.h>


#include "oyranos.h"
#include "oyranos_helper.h"
#include "oyranos_elektra.h"
#include "oyranos_string.h"
#include "config.h"
#include <locale.h>

void myDeAllocFunc(void *block)
{
  if(block) free(block);
}


int main(int argc, char *argv[])
{
  int error = 0;
  oyProfile_s * temp_prof = 0;
  char * device_class = 0,
       * device_name = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif

  oyProfile_s * p = 0;//oyProfile_FromStd( oyASSUMED_RGB, 0 );

  if(argc == 3 && strcmp(argv[1], "-v" ) == 0)
  {
    uint32_t count = 0;
    char  * temp = 0,
          * device_class = argv[2];
    int i;
    oyConfig_s * device = 0;
    oyConfigs_s * devices = 0;
    oyOptions_s * options = 0;

    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                   "list", OY_CREATE_NEW );  
    error = oyConfigs_FromDeviceClass( 0, device_class, options, &devices, 0 );

    count = oyConfigs_Count( devices );
    for( i = 0; i < count; ++i )
    {
      device = oyConfigs_Get( devices, i );
      temp = (char*)oyConfig_FindString( device, "device_name", 0 );

      printf("--------------------------------------------------------------------------------\n%d: %s\n", i, temp);

          // print all properties
      char * text = 0;
      char * list, * tmp = 0, * line = malloc(128);
      int even = 1;

      error = oyDeviceGetInfo( device, oyNAME_DESCRIPTION, 0, &text, malloc );
      list = text;
        tmp = list;
        while(list && list[0])
        {
          snprintf( line, 128, "%s", list );
          if(strchr( line, '\n' ))
          {
            tmp = strchr( line, '\n' );
            tmp[0] = 0;
          }
          if(even)
            printf( "%s\n", line );
          else
            printf( "  %s\n", line );
          list = strchr( list, '\n' );
          if(list) ++list;
          even = !even;
        }
  
      if(line) free(line);
      if(text) free(text);
    }

    exit(0);

  } else
  if(argc == 3 && strcmp(argv[1], "-l" ) == 0)
  {
    uint32_t count = 0;
    char  * temp = 0,
          * device_class = argv[2];
    int i;
    oyConfig_s * device = 0;
    oyConfigs_s * devices = 0;
    oyOptions_s * options = 0;

    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                   "list", OY_CREATE_NEW );  
    error = oyConfigs_FromDeviceClass( 0, device_class, options, &devices, 0 );

    count = oyConfigs_Count( devices );
    for( i = 0; i < count; ++i )
    {
      device = oyConfigs_Get( devices, i );
      temp = (char*)oyConfig_FindString( device, "device_name", 0 );

      printf("%d: %s\n", i, temp);
    }

    exit(0);

  } else
  if(argc == 3)
  {
    device_class = argv[1];
    device_name = argv[2];

  } else
  if(argc == 2 && strcmp(argv[1], "-l" ) == 0)
  {
    uint32_t count = 0,
           * rank_list = 0;
    char ** texts = 0,
          * temp = 0,
         ** attributes = 0,
          * device_class = 0;
    int i,j, attributes_n;

    // get all configuration filters
    oyConfigDomainList("//"OY_TYPE_STD"/config", &texts, &count,&rank_list ,0 );
    for( i = 0; i < count; ++i )
    {
      attributes_n = 0;
 
      // pick the filters name and remove the common config part
      temp = oyFilterRegistrationToText( texts[i], oyFILTER_REG_APPLICATION,
                                         malloc );
      attributes = oyStringSplit_( temp, '.', &attributes_n, malloc );
      free(temp);
      temp = malloc(1024); temp[0] = 0;
      for(j = 0; j < attributes_n; ++j)
      {
        if(strcmp(attributes[j], "config") == 0)
          continue;

        if(j && temp[0])
          sprintf( &temp[strlen(temp)], "." );
        sprintf( &temp[strlen(temp)], "%s", attributes[j]);
      }

      // The string in temp can be passed as the device_class argument to
      // oyDevicesGet().
      printf("%d: %s \"%s\"\n", i, texts[i], temp);

      oyStringListRelease_( &attributes, attributes_n, free );
      free (device_class);
      free(temp);
    }

    exit(0);

  } else
  if(argc >= 4 && strcmp(argv[1], "-s" ) == 0)
  {
    char * profile_name = argv[4];
    oyConfig_s * oy_device = 0;
    oyProfile_s * profile = 0;
    const char * tmp = 0;

    device_class = argv[2];
    device_name = argv[3];

    error = oyDeviceGet( 0, device_class, device_name, 0, &oy_device );
    if(oy_device)
      error = oyDeviceGetProfile( oy_device, 0, &profile );

    if(profile)
      tmp = oyProfile_GetFileName( profile, -1 );

    printf( "%s %s %s %s%s%s\n",
            device_class, device_name, profile_name, error?"wrong":"good",
            tmp?"\n has already a profile: ":"", tmp?tmp:"" );


    if(!oy_device)
      exit(1);


    if(profile_name)
      error = oyDeviceSetProfile( oy_device, profile_name );
    else
      error = oyDeviceUnset( oy_device );


    oyConfig_Release( &oy_device );
    exit(0);

  } else
  {
    printf( "Usage - get all profiles for a device:\n  %s [device_class \"monitor\"] [device_name \":0.0\"]\n",
            strrchr(argv[0],'/') ? strrchr(argv[0],'/')+1 : argv[0] );
    printf( "Usage - list all classes:\n  %s -l\n",
            strrchr(argv[0],'/') ? strrchr(argv[0],'/')+1 : argv[0] );
    printf( "Usage - list all devices of a class:\n  %s -l [device_class \"monitor\"]\n",
            strrchr(argv[0],'/') ? strrchr(argv[0],'/')+1 : argv[0] );
    printf( "Usage - set a device profile:\n  %s -s [device_class \"monitor\"] [device_name \":0.0\"] profilename.icc\n",
            strrchr(argv[0],'/') ? strrchr(argv[0],'/')+1 : argv[0] );
    printf( "Usage - tell verbosely about devices:\n  %s -v [device_class \"monitor\"]\n",
            strrchr(argv[0],'/') ? strrchr(argv[0],'/')+1 : argv[0] );
    exit(1);
  }

  /* device profile */
  {
    int i,n, pos = 0;
    oyProfileTag_s * tag_ = 0;
    oyConfig_s * oy_device = 0;
    oyOption_s * o = 0;
    char * text = 0, * name = 0;
    icSignature vs;
    char * v = 0;
    icTagTypeSignature texttype;

    error = oyDeviceGet( 0, device_class, device_name, 0, &oy_device );
    /* pick expensive informations */
    oyDeviceGetInfo( oy_device, oyNAME_DESCRIPTION, 0, &text, oyAllocateFunc_);
    oyDeAllocateFunc_( text );
    error = oyDeviceGetProfile( oy_device, 0, &p );

    vs = oyValueUInt32( oyProfile_GetSignature(p,oySIGNATURE_VERSION) );
    v = (char*)&vs;
    if(v[0] <= 2)
      texttype = icSigTextDescriptionType;
    else
      texttype = (icTagTypeSignature) icSigMultiLocalizedUnicodeType;

    n = oyOptions_Count( oy_device->backend_core );

    for(i = 0; i < n; ++i)
    {
      o = oyOptions_Get( oy_device->backend_core, i );

      text = oyOption_GetValueText( o, oyAllocateFunc_ );
      if(!text) continue;

      name = oyFilterRegistrationToText( oyOption_GetRegistration(o),
                                         oyFILTER_REG_MAX, oyAllocateFunc_ );
      if(strstr(name, "manufacturer"))
      {
        /* add a Manufacturer desc tag */
        tag_ = oyProfileTag_CreateFromText( text, texttype,
                                            icSigDeviceMfgDescTag, 0 );
        error = !tag_;
        if(tag_)
          error = oyProfile_TagMoveIn ( p, &tag_, -1 );

        oyDeAllocateFunc_( name );
        oyDeAllocateFunc_( text );
        continue;

      } else if(strstr(name, "model"))
      {

        /* add a Device Model desc tag */
        tag_ = oyProfileTag_CreateFromText( text, texttype,
                                            icSigDeviceModelDescTag, 0 );
        error = !tag_;
        if(tag_)
          error = oyProfile_TagMoveIn ( p, &tag_, -1 );

        oyDeAllocateFunc_( name );
        oyDeAllocateFunc_( text );
        continue;

      }

      oyDeAllocateFunc_( text );

      text = oyFilterRegistrationToText( oyOption_GetRegistration(o),
                                         oyFILTER_REG_MAX, oyAllocateFunc_ );
      oyDeAllocateFunc_( text );
      ++pos;
    }

    oyProfile_Release( &p );

    oyConfig_s * device = oyConfig_New( "//" OY_TYPE_STD "/config",
                                        0 );
    oyProfile_DeviceGet( p, device );

    printf("following key/values are in the devices backend_core set:\n");
    n = oyOptions_Count( device->backend_core );
    for(i = 0; i < n; ++i)
    {
      o = oyOptions_Get( device->backend_core, i );

      text = oyOption_GetValueText( o, oyAllocateFunc_ );
      if(!text) continue;

      printf("%s: %s\n", oyOption_GetRegistration(o), text);

      oyOption_Release( &o );
    }

    printf("\ngoing to rank installed profiles according to the device[\"%s\",\"%s\"]:\n", device_class, device_name );
    oyProfiles_s * p_list = oyProfiles_ForStd( oyASSUMED_RGB, 0,0 );
    int32_t * rank_list = (int32_t*) oyAllocateFunc_( oyProfiles_Count(p_list)
                                                        * sizeof(int32_t) );
    oyProfiles_DeviceRank( p_list, oy_device, rank_list );
    n = oyProfiles_Count( p_list );
    for(i = 0; i < n; ++i)
    {
      temp_prof = oyProfiles_Get( p_list, i );
      printf("%d %d: \"%s\" %s\n", rank_list[i], i,
             oyProfile_GetText( temp_prof, oyNAME_DESCRIPTION ),
             oyProfile_GetFileName(temp_prof, 0));
      oyProfile_Release( &temp_prof );
    }
  }

  return 0;
}

