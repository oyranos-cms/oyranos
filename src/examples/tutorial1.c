#include <oyranos.h>
#include <oyranos_devices.h>
#include <oyConversion_s.h>
#include <stdio.h>

int main( int argc OY_UNUSED, char ** argv OY_UNUSED ) {
  oyConfigs_s * devices = NULL;
  oyConfig_s * monitor = NULL;
  oyOptions_s * options = NULL;
  oyProfile_s * monitor_icc = NULL;
  const char * monitor_icc_dscr = NULL;
  // get all monitors
  oyDevicesGet( NULL, "monitor", NULL, &devices );
  // just pick the first monitor
  monitor = oyConfigs_Get( devices, 0 );
  /* get XCM_ICC_COLOR_SERVER_TARGET_PROFILE_IN_X_BASE */
  oyOptions_SetFromText( &options,
              "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target", "yes", OY_CREATE_NEW );
  oyDeviceGetProfile( monitor, options, &monitor_icc );
  // get the profiles internal name
  monitor_icc_dscr = oyProfile_GetText( monitor_icc, oyNAME_DESCRIPTION );
  printf( "first monitor has profile: %s\n", monitor_icc_dscr );

  // now convert some colors from sRGB to monitor space
  // find the appropriate profile flags
  uint32_t icc_profile_flags =oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                   "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyProfile_s * srgb = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, 0 );
  float rgb_in[9] = {0,0,0, 0.5,0.5,0.5, 1,1,1};
  float rgb_moni[9];
  // setup the color context
  oyConversion_s * cc = oyConversion_CreateBasicPixelsFromBuffers(
                         srgb, rgb_in, OY_TYPE_123_FLOAT,
                         monitor_icc, rgb_moni, OY_TYPE_123_FLOAT,
                         NULL, 3 );
  // convert by running the conversion graph
  oyConversion_RunPixels( cc, NULL );
  // show the source colors and the converted colors
  int i;
  for(i = 0; i < 3; ++i)
    printf("%.02f %.02f %.02f -> %.05f %.05f %.05f\n",
           rgb_in[3*i+0],rgb_in[3*i+1],rgb_in[3*i+2], rgb_moni[3*i+0],rgb_moni[3*i+1],rgb_moni[3*i+2]);

  return 0;
}
