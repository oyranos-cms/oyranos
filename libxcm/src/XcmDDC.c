/*  @file XcmDDC.c
 *
 *  libXcm  Xorg Colour Management
 *
 *  @par Copyright:
 *            2010 (C) Kai-Uwe Behrmann
 *
 *  @brief    DDC communication with monitor devices 
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2010/12/28
 */

#include "XcmVersion.h"

#if XCM_HAVE_LINUX

#include "XcmDDC.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>       /* open(), write() */
#include <dirent.h>       /* opendir() */
#include <fcntl.h>        /* O_RDWR */
/*#include <sys/stat.h>*/
#include <unistd.h>       /* usleep() */

#include <sys/ioctl.h>
#include <linux/types.h>

#ifndef USE_GETTEXT
#define _(text) text
#endif

#ifndef I2C_SLAVE
#define I2C_SLAVE  0x0703
#endif

#ifndef I2C_SLAVE_FORCE
#define I2C_SLAVE_FORCE 0x0706
#endif

#define TIMEOUT 50000

#define I2C_DIR "/dev/"

/** \addtogroup XcmDDC X Color Management DDC/CI communication API's

 *  @{
 *
 *  The purpose of the API is to establish client side communication with a
 *  monitor device. This can be used to update EEDID data and get MCCS 
 *  properties. The former is supported by this library.
 *
 *  XcmDDClist() returns a list of monitor i2c device names.
 *  XcmDDCgetEDID() can be used to fetch EEDID from a device name.
 *
 */

/* basic access functions */

/** Function XcmDDClist
 *  @brief   obtain all monitors i2c device names
 *
 *  @param[out]    list                empty pointer to a list of device names
 *  @param[out]    count               pass in a pointer to a int. gives the 
 *                                     number of elements in list
 *  @return                            error code
 *
 *  @version libXcm: 0.4.0
 *  @since   2010/12/28 (libXcm: 0.4.0)
 *  @date    2010/12/28
 */
XCM_DDC_ERROR_e   XcmDDClist         ( char            *** list,
                                       int               * count )
{
  XCM_DDC_ERROR_e error = XCM_DDC_OK;

  DIR * dir = opendir(I2C_DIR);
  struct dirent * entry;
  char * data = 0;
  size_t size;
  char * fn = NULL;
  int n = 0;
  char ** devices = NULL;

  if(!dir)
    return XCM_DDC_NO_FILE;

  fn = calloc(sizeof(char),1024);
  if(!fn)
  {
    error = XCM_DDC_NO_FILE;
    goto clean_list;
  }

  devices = calloc(sizeof(char*), 128);
  if(!devices)
  {
    error = XCM_DDC_NO_FILE;
    goto clean_list;
  }

  do
  {
    entry = readdir( dir );
    if(entry && strstr( entry->d_name, "i2c-" ))
    {
      sprintf( fn, I2C_DIR "%s", entry->d_name );
      size = 0;
      error = XcmDDCgetEDID( fn, &data, &size );

      if(!error && (size % 128) == 0)
      {
        devices[n] = strdup(fn);
        ++n;
      }

      if(data)
        free(data);
      data = 0;
    }
  } while(entry);
 

  *count = n;

  if(n > 0)
  {
    *list = devices;
    devices = NULL;
  } else if(devices)
  {
    free(devices);
    devices = NULL;
  }

  clean_list:
  if(devices)
    free(devices);
  if(dir)
    closedir(dir);
  if(fn)
    free(fn);

  return error;
}


int fds[128];
int fd_n = -1;

/** Function XcmDDCgetEDID
 *  @brief   obtain all monitors i2c device names
 *
 *  @param[in]     device_name         system specific device name
 *  @param[out]    data                empty pointer to EDID data, will be 
 *                                     mallocd for the user
 *  @param[out]    size                size of memory in data
 *  @return                            error code
 *
 *  @version libXcm: 0.4.0
 *  @since   2010/12/28 (libXcm: 0.4,0)
 *  @date    2010/12/28
 */
XCM_DDC_ERROR_e   XcmDDCgetEDID      ( const char        * device_name,
                                       char             ** data,
                                       size_t            * size )
{
  int fd = 0,
      ret = 0;
  XCM_DDC_ERROR_e error = XCM_DDC_OK;
  char command[128] = {0};


  ++fd_n;

  fds[fd_n] = open( device_name, O_RDWR );
  fd = fds[fd_n];

  if(fd != -1)
  {
    /* request the first 128 byte EDID block */
    error = ioctl(fd, I2C_SLAVE, 0x50);
    if(error == XCM_DDC_OK)
    {
      if(TIMEOUT)
        usleep(TIMEOUT);
      ret = write( fd, &command, 1 );
    } else
      error = XCM_DDC_PREPARE_FAIL;

    if(ret == 1)
    {
      *data = (char*)calloc(sizeof(char),256);
      if(TIMEOUT)
        usleep(TIMEOUT);
      ret = read( fd, *data, 128 );
    } else
      error = XCM_DDC_PREPARE_FAIL;

    if((ret % 128) == 0)
    {
      unsigned char * edid = (unsigned char*) *data;

      if(
         edid[0] == 0 &&
         edid[1] == 255 &&
         edid[2] == 255 &&
         edid[3] == 255 &&
         edid[4] == 255 &&
         edid[5] == 255 &&
         edid[6] == 255 &&
         edid[7] == 0)
      {
        *size = 128;
      } else
        error = XCM_DDC_WRONG_EDID;
    }

    if(error == XCM_DDC_OK)
    {
      if(TIMEOUT)
        usleep(TIMEOUT);
      error = ioctl(fd, I2C_SLAVE_FORCE, 0x37);
      if(error != XCM_DDC_OK)
        error = XCM_DDC_PREPARE_FAIL;
    }
  } else
    error = XCM_DDC_NO_FILE;

  return error;
}


/** Function XcmDDCErrorToString
 *  @brief   convert enum into a meaningful text string
 *
 *  @param[in]     error               the error
 *  @return                            library owned error text string
 *
 *  @version libXcm: 0.4.0
 *  @since   2010/12/28 (libXcm: 0.4.0)
 *  @date    2010/12/28
 */
const char *   XcmDDCErrorToString   ( XCM_DDC_ERROR_e     error )
{
  const char * text = 0;
  switch(error)
  {
  case XCM_DDC_OK: text = ""; break;
  case XCM_DDC_NO_FILE: text = "Could not open file. Did you check permissions?."; break;
  case XCM_DDC_PREPARE_FAIL: text = "Setting EEDID adress failed."; break;
  case XCM_DDC_EDID_READ_FAIL: text = "Could not obtain EEDID."; break;
  case XCM_DDC_WRONG_EDID: text = "Wrong EDID header."; break;
  }
  return text;
}


/** @} XcmDDC */

#endif /* Linux */
