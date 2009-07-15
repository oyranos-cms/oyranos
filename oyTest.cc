#include <stdio.h>
#include <oyranos_alpha.h>

using namespace oyranos;

int test_backend( const char * name );

int main()
{
	test_backend( "monitor" );
	test_backend( "scanner" );

	return 0;
}
#if 0
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
#endif
#if 0
    oyConfig_s * device = 0; /* initialise with zero to avoid errors */
    oyProfile_s * profile = 0;
    oyOptions_s * options = 0;  /* not used in this example */
    char * text = 0;
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

//All backends should understand the following calls
int test_backend( const char * name )
{
	 int error;

	 /******** Test Device Backend ********/

	 /**1. Query Device backend for all avaliable devices**/
	 /* calls Configs_FromPattern() with  "command" -> "list" option. */
	 oyConfigs_s *sane_devices = NULL;
	 /* OY_TYPE_STD: defaults to "imaging"
	  * "name": is defined in oyranos_cmm_xxxx.c
	  * options: is NULL, not used here (TODO what is it used for?)
	  * sane_devices: List of configuration objects, one for each device*/
	 error = oyDevicesGet(OY_TYPE_STD, name, NULL, &sane_devices);
	 int num_scanners = oyConfigs_Count( sane_devices );
	 printf("Found %d %s devices\n", num_scanners, name, num_scanners>1?"s":"" );

	 for (int i=0; i<num_scanners; i++) {
		 oyConfig_s * sane_device = oyConfigs_Get( sane_devices, i );
		 int num_options = oyConfig_Count( sane_device );
		 printf("\tFound %d option%s for device %d\n", num_options, num_options>1?"s":"", i );
		 for (int j=0; j<num_options; j++) {
			 oyOption_s * opt = oyConfig_Get( sane_device, j );
			 /*Problems with manipulating an option struct. FIXME*/
			 int id = oyOption_GetId( opt );
			 char * text = oyOption_GetValueText( opt, malloc );
			 printf("\t\tOption[%d] ID=%d\n\t\t[%s]: \"%s\"\n", j, id, opt->registration, text );
			 free(text);
			 oyOption_Release( &opt );
		 }
		 oyConfig_Release( &sane_device );
	 }
	 printf("\n\n");

	 /**2. Query Device backend for each found device**/
	 /* calls Configs_FromPattern() with  "command" -> "properties" option. */
	 for (int i=0; i<num_scanners; i++) {
		 oyConfig_s * sane_device = oyConfigs_Get( sane_devices, i );
		 oyOption_s * device_name_opt = oyConfig_Find( sane_device, "device_name" );
		 const char * device_name = oyOption_GetValueText( device_name_opt, malloc );
		 const char * dev_reg_app_field = oyFilterRegistrationToText(
				 device_name_opt->registration,
				 oyFILTER_REG_APPLICATION,
				 malloc );
		 oyOption_Release( &device_name_opt );
		 oyConfig_Release( &sane_device );

		 oyConfig_s * device_properties = NULL;
#if 0
		 /* When the 'oyOptions_s* options' struct is NULL, it is like sending a "properties" option */
		 //Seems to not work! /* FIXME */
		 error = oyDeviceGet( OY_TYPE_STD, name, device_name, NULL, &device_properties );
#endif
		 oyOptions_s * options = oyOptions_New( 0 );
		 char device_registration[256];
		 snprintf(device_registration, 256, "//%s/%s/command", OY_TYPE_STD, dev_reg_app_field);
		 oyOptions_SetFromText(
			 &options,
			 device_registration,
			 "properties",
			 OY_CREATE_NEW );
		 error = oyDeviceGet( OY_TYPE_STD, name, device_name, options, &device_properties );
		 int num_properties = oyConfig_Count( device_properties );
		 printf("\tFound %d propert%s for device \"%s\"\n", num_properties, num_properties>1?"ies":"y", device_name );
		 fflush(NULL);

		 for (int j=0; j<num_properties; j++) {
			 oyOption_s * opt = oyConfig_Get( device_properties, j );
			 /*Problems with manipulating an option struct. FIXME*/
			 int id = oyOption_GetId( opt );
			 char * text = oyOption_GetValueText( opt, malloc );
			 printf("\t\tProperty[%d] ID=%d\n\t\t[%s]: \"%s\"\n", j, id, opt->registration, text );
			 free(text);
			 oyOption_Release( &opt );
		 }
	 }

	 oyConfigs_Release( &sane_devices );
}

