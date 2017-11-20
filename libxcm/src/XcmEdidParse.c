/*  @file XcmEdidParse.c
 *
 *  libXcm  Xorg Colour Management
 *
 *  @par Copyright:
 *            2005-2015 (C) Kai-Uwe Behrmann
 *
 *  @brief    EDID data block parsing
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *            Soren Sandmann <sandmann@redhat.com>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2005/01/31
 */

#include "XcmEdidParse.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/** \addtogroup XcmEdidParse X Color Management EDID data block parsing API's

 *  @{
 *
 *  The purpouse of this API is to obtain basic and displayable device 
 *  identification strings and colorimetric informations contained in the EDID
 *  data block sent by monitors.
 *
 *  The user has to pass in a valid EDID block. This can be obtained
 *  per a root window X atom or a XRandR output property and is not part of
 *  this API. The data block is passed to XcmEdidParse(). This function 
 *  generates a list of key value pairs,
 *  with some first rough interpretation. The key/values are useable for 
 *  data bases, ICC profile generation or device identification.
 *  The returned XcmEdidKeyValue_s list elements can be individually read and
 *  worked with. Please look as well on the XcmEdidPrintString() function
 *  and examples/edid-parse/ as a small example on how to use the API.
 *  The XcmEdidPrintOpenIccJSON() will pull out a JSON data structure.
 *  XcmEdidFree() releases allocated list memory.
 *
 */

static int
decode_color_characteristics (const unsigned char *edid, double * c);

static int          XcmBigEndian()
{
  int big = 0;
  char testc[2] = {0,0};
  uint16_t *testu = (uint16_t*)testc;
  *testu = 1;
  big = testc[1];
  return big;
}

static uint16_t     XcmValueUInt16Swap ( uint16_t          val )
{
  uint8_t * bytes = (uint8_t*)&val, 
            new_val[2];
  {
    new_val[0] = bytes[1];
    new_val[1] = bytes[0];
    memcpy(&val, new_val, sizeof(val));
  }
  return val;
}

static uint32_t     XcmValueUInt32Swap ( uint32_t          val )
{
  uint8_t * bytes = (uint8_t*)&val, 
            new_val[4];
  {
    new_val[0] = bytes[3];
    new_val[1] = bytes[2];
    new_val[2] = bytes[1];
    new_val[3] = bytes[0];
    memcpy(&val, new_val, sizeof(uint32_t));
  }
  return val;
}

static int16_t      XcmValueInt16Swap  ( int16_t           val )
{
  uint8_t * bytes = (uint8_t*)&val, 
            new_val[2];
  {
    new_val[0] = bytes[1];
    new_val[1] = bytes[0];
    memcpy(&val, new_val, sizeof(val));
  }
  return val;
}

static uint16_t     XcmValueUInt16    ( uint16_t            val )
{
  if(!XcmBigEndian())
    val = XcmValueUInt16Swap(val);
  return val;
}

static uint32_t     XcmValueUInt32    ( uint32_t            val )
{
  if(!XcmBigEndian())
    val = XcmValueUInt32Swap(val);
  return val;
}


static void         XcmEdidSetInt    ( XcmEdidKeyValue_s * entry,
                                       const char        * key,
                                       int                 value )
{
  entry->key = key;
  entry->type = XCM_EDID_VALUE_INT;
  entry->value.integer = value;
}
static void         XcmEdidSetDouble ( XcmEdidKeyValue_s * entry,
                                       const char        * key,
                                       double              value )
{
  entry->key = key;
  entry->type = XCM_EDID_VALUE_DOUBLE;
  entry->value.dbl = value;
}
static char *       XcmCheckText     ( char              * value )
{
  int len = strlen( value ),
      i;
  for( i = 0; i < len; ++i )
    if( value[i] < 32 || 126 < value[i] )
    {
      value[i] = '\000';
      break;
    }
  return value;
}
static void         XcmEdidSetText   ( XcmEdidKeyValue_s * entry,
                                       const char        * key,
                                       char              * value )
{
  entry->key = key;
  entry->type = XCM_EDID_VALUE_TEXT;
  entry->value.text = XcmCheckText( value );
}

#define SET_INT(key)  if(key) \
    XcmEdidSetInt( &(*list)[pos++], #key, key );
#define SET_DBL(key)  if(key) \
    XcmEdidSetDouble( &(*list)[pos++], #key, key );
#define SET_TXT(key)  if(key) \
    XcmEdidSetText( &(*list)[pos++], #key, key );

/* basic access functions */

/** Function XcmEdidParse
 *  @brief   EDID to key/value pair transformation
 *
 *  The function performs no verification of the data block other than the
 *  first eight byte block signature.
 *
 *  @param[in]     edid                EDID data block 128 or 256 bytes long
 *  @param[out]    list                the key/value data structures
 *  @param[in,out] count               pass in a pointer to a int. gives the 
 *                                     number of elements in list
 *  @return                            error code
 *
 *  @version libXcm: 0.3.0
 *  @since   2009/12/12 (libXcm: 0.3.0)
 *  @date    2010/10/01
 */
XCM_EDID_ERROR_e  XcmEdidParse       ( void              * edid,
                                       XcmEdidKeyValue_s** list,
                                       int               * count )
{
  XCM_EDID_ERROR_e error = XCM_EDID_OK;
  char * t = 0;
  int len, i, j;
  char mnf[4], * num;
  uint16_t mnft_id = 0, model_id = 0, week = 0, year = 0;
  uint32_t serial_id = 0;
  char * serial = 0, * manufacturer = 0, * model = 0, * vendor = 0,
       * mnft = 0;
  double c[9] = {0,0,0,0,0,0,0,0,0};
  double a[6] = {0,0,0,0,0,0};
  int pos = 0;
  int has_cmd = 0;
  XcmEdid_s * edi = edid;


  *list = calloc( 24, sizeof(XcmEdidKeyValue_s) );

  /* check */
  if(edi &&
     edi->sig[0] == 0 &&
     edi->sig[1] == 255 &&
     edi->sig[2] == 255 &&
     edi->sig[3] == 255 &&
     edi->sig[4] == 255 &&
     edi->sig[5] == 255 &&
     edi->sig[6] == 255 &&
     edi->sig[7] == 0 
    )
  {
    /* verified */
  } else {
    return XCM_EDID_WRONG_SIGNATURE;
  }

  sprintf( mnf, "%c%c%c",
          (char)((edi->mnft_id[0] & 124) >> 2) + 'A' - 1,
          (char)((edi->mnft_id[0] & 3) << 3) + ((edi->mnft_id[1] & 227) >> 5) + 'A' - 1,
          (char)(edi->mnft_id[1] & 31) + 'A' - 1 );

  /* MSB */
  memcpy(&mnft_id, &edi->mnft_id[0], sizeof(mnft_id));
  mnft_id = XcmValueUInt16(mnft_id);
  /* LSB */
  memcpy(&model_id, &edi->model_id, sizeof(model_id));
  if(XcmBigEndian())
    model_id = XcmValueUInt16Swap(model_id);

  SET_INT( mnft_id )
  SET_INT( model_id )

  memcpy(&serial_id, &edi->ser_id, sizeof(uint32_t));
  serial_id = XcmValueUInt32(serial_id);

  if(0)
  fprintf( stderr, "MNF_ID: %d %d SER_ID: %d D:%d/%d bxh:%dx%dcm %s\n",
           edi->mnft_id[0], edi->mnft_id[1], serial_id,
           edi->week, edi->year +1990,
           edi->width, edi->height, mnf );

  week = edi->week;
  year = edi->year + 1990;
  SET_INT( week )
  SET_INT( year )

  for( i = 0; i < 4; ++i)
  {
    /* we get i==0 -> text1, i==1 -> text2, ... i==3 -> text3 */
    unsigned char * block;
    char **target = NULL,
         * tmp = 0;

    switch(i)
    {
    case 0: block = edi->text1; break;
    case 1: block = edi->text2; break;
    case 2: block = edi->text3; break;
    case 3: block = edi->text4; break;
    }

    if(block[0] == 0 && block[1] == 0 && block[2] == 0)
    {
      uint8_t type = block[3];

      if(        type == 249 ) { /* CMD */
        if(block[5] == 3)
        {
          uint16_t v;
          has_cmd = 1;
          for(j = 0; j < 6; ++j)
          {
            v = XcmValueInt16Swap(*((int16_t*)&block[6+2*j]));
            a[j] = v / 100.0;
          }
        }
      } else if( type == 255 && !(serial && serial[0]) ) { /* serial */
        target = &serial;
      } else if( type == 254 && !(vendor && vendor[0]) ) { /* vendor */
        target = &vendor;
      } else if( type == 253 ) { /* frequenz ranges */
      } else if( type == 252 && !(model && model[0]) ) { /* model */
        target = &model;
      }
      if(target)
      {
        len = strlen((char*)&block[5]);
        if(len) {
          ++len;
          t = (char*)malloc( 16 );
          snprintf(t, 15, "%s", (char*)&block[5]);
          t[15] = '\000';
          if(strrchr(t, '\n'))
          {
            tmp = strrchr(t, '\n');
            tmp[0] = 0;
          }

          /* workaround for APP */
          if(strcmp(mnf,"APP") == 0)
          {
            if(type == 254)
            {
              if(!serial)
                target = &serial;
              else
              if(serial && !model)
                target = &model;
            }
          }
        
          *target = t;
        }
      }
    }
  }

  SET_TXT( vendor )
  SET_TXT( model )
  SET_TXT( serial )

  if(!model)
  {
    num = malloc(128);
    if(num)
    {
      sprintf(num, "%d", model_id);
      model = num;
      SET_TXT( model )
      num = model = 0;
    }
  }

  decode_color_characteristics( edid, c );

  XcmEdidSetDouble( &(*list)[pos++], XCM_EDID_KEY_REDx, c[0] );
  XcmEdidSetDouble( &(*list)[pos++], XCM_EDID_KEY_REDy, c[1] );
  XcmEdidSetDouble( &(*list)[pos++], XCM_EDID_KEY_GREENx, c[2] );
  XcmEdidSetDouble( &(*list)[pos++], XCM_EDID_KEY_GREENy, c[3] );
  XcmEdidSetDouble( &(*list)[pos++], XCM_EDID_KEY_BLUEx, c[4] );
  XcmEdidSetDouble( &(*list)[pos++], XCM_EDID_KEY_BLUEy, c[5] );
  XcmEdidSetDouble( &(*list)[pos++], XCM_EDID_KEY_WHITEx, c[6] );
  XcmEdidSetDouble( &(*list)[pos++], XCM_EDID_KEY_WHITEy, c[7] );

  if( has_cmd == 1 )
  {
    XcmEdidSetDouble( &(*list)[pos++], XCM_EDID_KEY_A3RED, a[0] );
    XcmEdidSetDouble( &(*list)[pos++], XCM_EDID_KEY_A2RED, a[1] );
    XcmEdidSetDouble( &(*list)[pos++], XCM_EDID_KEY_A3GREEN, a[2] );
    XcmEdidSetDouble( &(*list)[pos++], XCM_EDID_KEY_A2GREEN, a[3] );
    XcmEdidSetDouble( &(*list)[pos++], XCM_EDID_KEY_A3BLUE, a[4] );
    XcmEdidSetDouble( &(*list)[pos++], XCM_EDID_KEY_A2BLUE, a[5] );
  }

  /* Gamma */
  if (edi->gamma_factor == 0xFF)
    c[8] = -1.0;
  else
    c[8] = (edi->gamma_factor + 100.0) / 100.0;

  XcmEdidSetDouble( &(*list)[pos++], XCM_EDID_KEY_GAMMA, c[8] );

  {
    t = (char*)malloc( 80 );
    if(!memcmp(mnf,"ACR",3))
      sprintf(t, "Acer");
    else if(!memcmp(mnf,"AUO",3))
      sprintf(t, "AU Optronics");
    else if(!memcmp(mnf,"APP",3))
      sprintf(t, "Apple");
    else if(!memcmp(mnf,"BDS",3))
      sprintf(t, "Barco");
    else if(!memcmp(mnf,"CPQ",3))
      sprintf(t, "COMPAQ");
    else if(!memcmp(mnf,"DEC",3))
      sprintf(t, "Digital Equipment Corporation");
    else if(!memcmp(mnf,"DEL",3))
      sprintf(t, "Dell Computer");
    else if(!memcmp(mnf,"DWE",3))
      sprintf(t, "Daewoo");
    else if(!memcmp(mnf,"EIZ",3))
      sprintf(t, "EIZO");
    else if(!memcmp(mnf,"GSM",3))
      sprintf(t, "Goldstar Company Ltd"); /* as per gnome-desktop */
    else if(!memcmp(mnf,"HSD",3))
      sprintf(t, "HannStar");
    else if(!memcmp(mnf,"HWP",3))
      sprintf(t, "Hewlett Packard");
    else if(!memcmp(mnf,"NEC",3))
      sprintf(t, "NEC");
    else if(!memcmp(mnf,"LGD",3))
      sprintf(t, "LG Display");
    else if(!memcmp(mnf,"LGE",3))
      sprintf(t, "LG Electronics");
    else if(!memcmp(mnf,"LEN",3))
      sprintf(t, "Lenovo");
    else if(!memcmp(mnf,"LPL",3))
      sprintf(t, "LG Philips");
    else if(!memcmp(mnf,"MEI",3))
      sprintf(t, "Panasonic");
    else if(!memcmp(mnf,"MEL",3))
      sprintf(t, "Mitsubishi");
    else if(!memcmp(mnf,"MID",3))
      sprintf(t, "miro");
    else if(!memcmp(mnf,"NOK",3))
      sprintf(t, "Nokia");
    else if(!memcmp(mnf,"NVD",3))
      sprintf(t, "Nvidia");
    else if(!memcmp(mnf,"PHL",3))
      sprintf(t, "Philips");
    else if(!memcmp(mnf,"QUA",3))
      sprintf(t, "Quatographic");
    else if(!memcmp(mnf,"SAM",3))
      sprintf(t, "Samsung");
    else if(!memcmp(mnf,"SEC",3))
      sprintf(t, "Seiko Epson Corporation");
    else if(!memcmp(mnf,"SNI",3))
      sprintf(t, "Siemens Nixdorf");
    else if(!memcmp(mnf,"SNY",3))
      sprintf(t, "Sony");
    else
      sprintf(t, "%s", mnf);

    manufacturer = t;
  }
  mnft = (char*)malloc( 24 );
  sprintf(mnft, "%s", mnf);

  SET_TXT( mnft )
  SET_TXT( manufacturer )

  *count = pos;

  return error;
}

/** Function XcmEdidFree
 *  @brief   free library allocated list
 *
 *  @param[in,out] list                the key/value data structures
 *  @return                            error code
 *
 *  @version libXcm: 0.3.0
 *  @since   2009/12/12 (libXcm: 0.3.0)
 *  @date    2010/10/01
 */
XCM_EDID_ERROR_e  XcmEdidFree        ( XcmEdidKeyValue_s** list )
{
  int pos = 0;
  XcmEdidKeyValue_s * l = 0;

  if(!list)
    return XCM_EDID_OK;

  l = *list;

  while(l[pos].key)
  {
    if(l[pos].type == XCM_EDID_VALUE_TEXT && l[pos].value.text)
      free( l[pos].value.text );
    ++pos;
  }

  free(*list);
  *list = 0;

  return XCM_EDID_OK;
}

/** Function XcmEdidErrorToString
 *  @brief   convert enum into a meaningful text string
 *
 *  @param[in]     error               the error
 *  @return                            library owned error text string
 *
 *  @version libXcm: 0.3.0
 *  @since   2009/12/12 (libXcm: 0.3.0)
 *  @date    2010/10/01
 */
const char *   XcmEdidErrorToString    ( XCM_EDID_ERROR_e       error )
{
  const char * text = 0;
  switch(error)
  {
  case XCM_EDID_OK: text = ""; break;
  case XCM_EDID_WRONG_SIGNATURE: text = "Could not verifiy EDID"; break;
  }
  return text;
}


/* convinience functions */
/** Function XcmEdidPrintString
 *  @brief   convert a EDID block into text
 *
 *  @param[in]     edid                the EDID data block
 *  @param[out]    text                the resulting text string
 *  @param[in]     alloc               a user provided function to allocate text
 *  @return                            error code
 *
 *  @version libXcm: 0.3.0
 *  @since   2009/12/12 (libXcm: 0.3.0)
 *  @date    2010/10/01
 */
XCM_EDID_ERROR_e  XcmEdidPrintString ( void              * edid,
                                       char             ** text,
                                       void             *(*alloc)(size_t sz) )
{
  XcmEdidKeyValue_s * l = 0;
  int count = 0, i;
  XCM_EDID_ERROR_e err = XcmEdidParse( edid, &l, &count );
  char * txt = alloc(1024);

  txt[0] = 0;

  for(i = 0; i < count; ++i)
  {
    sprintf( &txt[strlen(txt)], "%s: ", l[i].key );
    if(l[i].type == XCM_EDID_VALUE_TEXT)
      sprintf( &txt[strlen(txt)], "\"%s\"\n", l[i].value.text);
    if(l[i].type == XCM_EDID_VALUE_INT)
      sprintf( &txt[strlen(txt)], "%d\n", l[i].value.integer);
    if(l[i].type == XCM_EDID_VALUE_DOUBLE)
      sprintf( &txt[strlen(txt)], "%g\n", l[i].value.dbl);
  }

  if(count)
    *text = txt;

  XcmEdidFree( &l );

  return err;
}

/** Function XcmEdidPrintOpenIccJSON
 *  @brief   convert a EDID block into a device configuration
 *
 *  @param[in]     edid                the EDID data block
 *  @param[out]    text                the resulting text string
 *  @param[in]     alloc               a user provided function to allocate text
 *  @return                            error code
 *
 *  @version libXcm: 0.4.2
 *  @since   2011/06/19 (libXcm: 0.4.2)
 *  @date    2011/06/19
 */
XCM_EDID_ERROR_e  XcmEdidPrintOpenIccJSON (
                                       void              * edid,
                                       char             ** text,
                                       void             *(*alloc)(size_t sz) )
{
  XcmEdidKeyValue_s * l = 0;
  int count = 0, i;
  XCM_EDID_ERROR_e err = XcmEdidParse( edid, &l, &count );
  char * txt;
  int year = 0, week = 0;

  if(!count)
  {
    XcmEdidFree( &l );
    return err;
  }

  txt = alloc( sizeof(char) * 4096 );

  sprintf(txt,
  "{\n"
  "  \"org\": {\n"
  "    \"freedesktop\": {\n"
  "      \"openicc\": {\n"
  "        \"device\": {\n"
  "          \"monitor\": [{\n"
  "            \"prefix\": \"EDID_\",\n"
  );

  for(i = 0; i < count; ++i)
  {
    if(strcmp(l[i].key,"week") == 0)
    {
      week = l[i].value.integer;
      continue;
    } else
    if(strcmp(l[i].key,"year") == 0)
    {
      year = l[i].value.integer;
      continue;
    }
    sprintf( &txt[strlen(txt)], "              \"EDID_%s\": ", l[i].key );
    if(l[i].type == XCM_EDID_VALUE_TEXT)
      sprintf( &txt[strlen(txt)], "\"%s\"", l[i].value.text);
    if(l[i].type == XCM_EDID_VALUE_INT)
      sprintf( &txt[strlen(txt)], "\"%d\"", l[i].value.integer);
    if(l[i].type == XCM_EDID_VALUE_DOUBLE)
      sprintf( &txt[strlen(txt)], "\"%g\"", l[i].value.dbl);
    sprintf( &txt[strlen(txt)], ",");
    sprintf( &txt[strlen(txt)], "\n");
  }

  sprintf( &txt[strlen(txt)], "              \"EDID_date\": \"%d-T%d\"\n",
           year, week );

  sprintf( &txt[strlen(txt)], 
  "            }\n"
  "          ]\n"
  "        }\n"
  "      }\n"
  "    }\n"
  "  }\n"
  "}\n"
  );

  *text = txt;

  XcmEdidFree( &l );

  return err;
}

#define  XCM_EDID_COLOUR_MATRIX_SINGLE_GAMMA "colour_matrix.edid.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma"
XCM_EDID_ERROR_e  XcmEdidComposeString(void              * edid,
                                       const char        * key,
                                       char             ** text,
                                       void              (*alloc)(size_t sz) );

#ifndef TRUE
#define TRUE 1
#endif

/* BEGINN edid-parse.c_SECTION

 Author: Soren Sandmann <sandmann@redhat.com>

   Should be MIT licensed.
 */

static int
get_bit (int in, int bit)
{
    return (in & (1 << bit)) >> bit;
}

static int
get_bits (int in, int begin, int end)
{
    int mask = (1 << (end - begin + 1)) - 1;

    return (in >> begin) & mask;
}

static double
decode_fraction (int high, int low)
{
    double result = 0.0;
    int i;

    high = (high << 2) | low;

    for (i = 0; i < 10; ++i) 
        result += get_bit (high, i) * pow (2, i - 10);

    return result;
}

static int
decode_color_characteristics (const unsigned char *edid, double * c)
{   
    c[0]/*red_x*/ = decode_fraction (edid[0x1b], get_bits (edid[0x19], 6, 7));
    c[1]/*red_y*/ = decode_fraction (edid[0x1c], get_bits (edid[0x19], 4, 5));
    c[2]/*green_x*/ = decode_fraction (edid[0x1d], get_bits (edid[0x19], 2, 3));
    c[3]/*green_y*/ = decode_fraction (edid[0x1e], get_bits (edid[0x19], 0, 1));
    c[4]/*blue_x*/ = decode_fraction (edid[0x1f], get_bits (edid[0x1a], 6, 7));
    c[5]/*blue_y*/ = decode_fraction (edid[0x20], get_bits (edid[0x1a], 4, 5));
    c[6]/*white_x*/ = decode_fraction (edid[0x21], get_bits (edid[0x1a], 2, 3));
    c[7]/*white_y*/ = decode_fraction (edid[0x22], get_bits (edid[0x1a], 0, 1));

    return TRUE;
}

/* END edid-parse.c_SECTION */

/** @} XcmEdidParse */

