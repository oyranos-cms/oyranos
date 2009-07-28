#include <stdio.h>
#include <oyranos_alpha.h>

#define DBG printf("%s: %d\n", __FILE__, __LINE__ ); fflush(NULL);

using namespace oyranos;

int test_backend( const char * name );
void help(char* progname);

int main(int argc, char** argv)
{
	if (argc < 2) {
		help(argv[0]);
		return 0;
	}

	for (int c=1; c<argc; ++c) {
		int num = atoi(argv[c])-1;
		if (num>3)
			help(argv[0]);
		switch (num) {
			case 0:
				test_backend( "monitor" );
				break;
			case 1:
				test_backend( "scanner" );
				break;
			case 2:
				test_backend( "raw-image" );
				break;
			case 3:
				test_backend( "printer" );
				break;
		}
	}

	return 0;
}


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
	 if (oyDevicesGet(OY_TYPE_STD, name, NULL, &sane_devices) !=0 )
		 return 1;
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
		 //Some clarification is needed on how to create these strings TODO
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
 
	 /**3. Display a help message**/
#if 0
	 // pass empty options to the backend to get a usage message
	 // Does not seem to work now FIXME
	 // Also, scanning all devices to get one device_name to put into oyDeviceGet()
	 // is (at least for sane) really expensive.
		 oyConfig_s * sane_device = oyConfigs_Get( sane_devices, 0 );
		 oyOption_s * device_name_opt = oyConfig_Find( sane_device, "device_name" );
		 const char * device_name = oyOption_GetValueText( device_name_opt, malloc );
		 oyOptions_s * options = oyOptions_New( 0 );
		error = oyDeviceGet( OY_TYPE_STD, name, device_name, options, 0 );
#endif
	 oyConfig_s * device = oyConfigs_Get( sane_devices, 0 );
	 char device_registration[256];
	 snprintf(device_registration, 256, "%s/command", device->registration );
	 oyOptions_s * options = oyOptions_New( 0 );
	 oyOptions_SetFromText(
		 &options,
		 device_registration,
		 "help",
		 OY_CREATE_NEW );
	 error = oyDevicesGet(OY_TYPE_STD, name, options, NULL);


	 oyConfigs_Release( &sane_devices );
}

void help(char* progname)
{
	const char* usage =
		"Usage: %s [[int] [int]...]\n"
		"1:\tmonitor\n"
		"2:\tscanner\n"
		"3:\traw-image\n"
		"4:\tprinter\n";
	printf(usage,progname);
}
