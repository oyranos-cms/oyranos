#include <string.h>
#include <stdio.h>                /* sprintf() */
#include <oyConfig_s.h>
#include <oyOptions_s.h>
#include <oyranos_conversion.h>
#include <oyranos_devices.h>
int main ( int argc, char ** argv )
{
  // get a device
  oyConfig_s * device = 0;
  oyConfigs_s * ds = 0;
  oyDevicesGet( 0, "monitor", 0, &ds );
  device = oyConfigs_Get( ds, 0 ); oyConfigs_Release( &ds );
  // get all Taxi DB entries for a device
  oyConfigs_s * taxi_devices = 0;
  int error;
  oySCOPE_e scope = oySCOPE_USER;
  error = oyDevicesFromTaxiDB( device, 0, &taxi_devices, 0 );
  // see how many are included
  int n = oyConfigs_Count( taxi_devices ),
      i;
  char * id = calloc( sizeof(char), 1024 );
  for(i = 0; i < n; ++i)
  {
    int32_t rank = 0;
    oyOptions_s * options = NULL;
    oyProfile_s * ip;
    oyConfig_s * taxi_device = oyConfigs_Get( taxi_devices, i );
    error = oyConfig_Compare( device, taxi_device, &rank );
    // get first profile from Taxi DB
    if(i == 0)
    {
      // select the first assigned profile in position zero
      snprintf( id, 1024, "%s/0", oyOptions_FindString(
                                   *oyConfig_GetOptions(taxi_device,"db"),
                                   "TAXI_id", 0 ));
      error = oyOptions_SetFromText( &options,
                                   "//" OY_TYPE_STD "/db/TAXI_id",
                                   id,
                                   OY_CREATE_NEW );
      ip = oyProfile_FromTaxiDB( options, NULL );
      oyOptions_Release( &options );
      if(rank > 0)
      {
        error = oyOptions_SetFromText( &options,
                                       "////device", "1",
                                       OY_CREATE_NEW );
        error = oyProfile_Install( ip, scope, options );
        oyOptions_Release( &options );
        if(!ip)
          printf( "No valid Profile obtained: %s\n", id );
        if(error == oyERROR_DATA_AMBIGUITY)
          printf( "Profile already installed: %s\n", oyProfile_GetText( ip, oyNAME_DESCRIPTION ));
        else if(error == oyERROR_DATA_WRITE)
          printf( "User Path can not be written\n" );
        else if(error == oyCORRUPTED)
          printf( "Profile not useable: %s\n", oyProfile_GetText( ip, oyNAME_DESCRIPTION ) );
        else if(error > 0)
          printf( "%s - %d","Internal Error", error );
        else
        {
          uint32_t icc_profile_flags = 0;
          const char * filename = oyProfile_GetFileName( ip, -1 );
          /* select profiles matching actual capabilities */
          icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                       "//" OY_TYPE_STD "/icc_color", NULL, 0 );
          printf( "installed -> %s\n", filename );
          // store new settings in the Oyranos data base
          oyDeviceSetProfile( device, oySCOPE_USER, strrchr( filename, OY_SLASH_C ) + 1 );
          // remove any device entries
          oyDeviceUnset( device );
          // update the device from the newly added Oyranos data base settings
          error = oyOptions_SetFromInt( &options,
                                "//" OY_TYPE_STD "/icc_profile_flags",
                                icc_profile_flags, 0, OY_CREATE_NEW );
          error = oyOptions_SetFromText( &options,
                                  "//"OY_TYPE_STD"/config/skip_ask_for_profile", "yes", OY_CREATE_NEW );
          oyDeviceSetup( device, options );
          printf( "assigned -> %s\n", strrchr( filename, OY_SLASH_C ) + 1 );
        }
      }
    }
    if(rank > 0)
    {
      const char * t = oyConfig_FindString(taxi_device, "TAXI_profile_description", 0);
      printf( "rank[%d] %s\n", rank, t?t:"" );
    }
  }
  // release data
  oyConfigs_Release( &taxi_devices );

  return 0;
}
