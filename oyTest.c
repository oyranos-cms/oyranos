#include <stdio.h>
#include <oyranos_alpha.h>

/* cc `oyranos-config --ldflags --cflags` -g -o oyTest oyTest.c */
int main()
{
    oyConfig_s * device = 0; /* initialise with zero to avoid errors */
    oyProfile_s * profile = 0;
    oyOptions_s * options = 0;  /* not used in this example */
    char * text = 0;

    /* Call into the device backends oyCMMapi8_s::oyConfigs_FromPattern
     * function with the "list" command.
     * "monitor" is to be exchanged by the actual device class and
     * ":0.0" by the actual device nick name.
     */
	 options = oyOptions_New( 0 );
    int error = oyDeviceGet( OY_TYPE_STD, "monitor", ":0.0", options, 0 );
#if 0
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
