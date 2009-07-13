#include <stdio.h>
#include <oyranos_alpha.h>

/* cc `oyranos-config --ldflags --cflags` -g -o oyTest oyTest.c */
int main()
{
    oyConfig_s * device = 0; /* initialise with zero to avoid errors */
    oyProfile_s * profile = 0;
    oyOptions_s * options = 0;  /* not used in this example */
    char * text = 0;
	 int error;

	 /******** Test SANE Backend ********/

	 /**Query SANE backend for all avaliable devices**/
	 oyConfigs_s *sane_devices = NULL;
	 /* OY_TYPE_STD: defaults to "imaging"
	  * "scanner": is defined in oyranos_cmm_SANE.c
	  * options: is NULL, not used here (TODO what is it used for?)
	  * sane_devices: List of configuration objects, one for each device*/
	 error = oyDevicesGet (OY_TYPE_STD, "scanner", NULL, &sane_devices);
	 int num_scanners = oyConfigs_Count( sane_devices );

	 for (int i=0; i<num_scanners; i++) {
		 oyConfig_s * sane_device = oyConfigs_Get( sane_devices, i );
		 int num_options = oyConfig_Count( sane_device );
		 for (int j=0; j<num_options; j++) {
			 oyOption_s opt = oyConfig_Get( sane_device, j );
			 /*Problems with manipulating an option struct. FIXME*/
			 oyOption_Release( &opt );
		 }
		 oyConfig_Release( &sane_device );
	 }
	 /**Select SANE**/
	 options = oyOptions_New( 0 );
    error = oyDeviceGet( OY_TYPE_STD, "scanner", "pnm:0", options, 0 );
	 /*Select SANE device TODO*/
	 /*Command SANE backend to say hi*/
	 oyOptions_SetFromText(
			 &options,
			 "//" OY_TYPE_STD "/config.scanner.SANE/command",
			 "say_hello",
			 OY_CREATE_NEW );
    error = oyDeviceGet( OY_TYPE_STD, "scanner", "dev0", options, 0 );
/*	 oyOptions_SetFromText(
			 &options,
			 "//" OY_TYPE_STD "/config.scanner.SANE/command",
			 "list",
			 OY_CREATE_NEW );
    error = oyDeviceGet( OY_TYPE_STD, "scanner", "v4l:/dev/video0", options, 0 );
*/
#if 0
    /* Call into the device backends oyCMMapi8_s::oyConfigs_FromPattern
     * function with the "list" command.
     * "monitor" is to be exchanged by the actual device class and
     * ":0.0" by the actual device nick name.
     */

    int error = oyDeviceGet( 0, "monitor", ":0.0", options, &device );
    /* obtain a expensive list of key/value pairs from the device backend */
    error = oyDeviceGetInfo( device, oyNAME_DESCRIPTION, 0, &text,
                                     malloc );

	 printf("%s\n", text);
    /* assign a profile to the device/driver */
    error = oyDeviceSetProfile( device, "some_real_profile_in_your_path.icc" );

    /* Ask Oyranos to lookup its DB for a matching profile. It is only useful
     * if a profile is already set for the devices keys. The set devices can be
     * viewed at the command line with:
     * ls -d ~/.kdb/user/shared/freedesktop.org/imaging/config.*
     */
    error = oyDeviceAskProfile( device, &profile );
#endif
}
