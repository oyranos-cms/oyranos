/** @file oyranos_edid_parse.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2005-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    EDID data block parsing
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2005/01/31
 */

#include "oyranos_edid_parse.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

static int
decode_color_characteristics (const unsigned char *edid, double * c);

static
int          edidBigEndian()
{
  int big = 0;
  char testc[2] = {0,0};
  uint16_t *testu = (uint16_t*)testc;
  *testu = 1;
  big = testc[1];
  return big;
}

static
void         XEdidSetInt             ( XEdidKeyValue_s   * entry,
                                       const char        * key,
                                       int                 value )
{
  entry->key = key;
  entry->type = XEDID_VALUE_INT;
  entry->value.integer = value;
}
static
void         XEdidSetDouble          ( XEdidKeyValue_s   * entry,
                                       const char        * key,
                                       double              value )
{
  entry->key = key;
  entry->type = XEDID_VALUE_DOUBLE;
  entry->value.dbl = value;
}
static
void         XEdidSetText            ( XEdidKeyValue_s   * entry,
                                       const char        * key,
                                       char              * value )
{
  entry->key = key;
  entry->type = XEDID_VALUE_TEXT;
  entry->value.text = value;
}

#define SET_INT(key)  if(key) \
    XEdidSetInt( &(*list)[pos++], #key, key );
#define SET_DBL(key)  if(key) \
    XEdidSetDouble( &(*list)[pos++], #key, key );
#define SET_TXT(key)  if(key) \
    XEdidSetText( &(*list)[pos++], #key, key );

/* basic access functions */
XEDID_ERROR_e  XEdidParse            ( void              * edid,
                                       XEdidKeyValue_s  ** list,
                                       int               * count )
{
  XEDID_ERROR_e error = XEDID_OK;
  char * t = 0;
  int len, i;
  char mnf[4];
  char ser[4];
  uint16_t mnft_id = 0, model_id = 0, week = 0, year = 0;
  char * serial = 0, * manufacturer = 0, * model = 0, * vendor = 0,
       * mnft = 0;
  double c[9] = {0,0,0,0,0,0,0,0,0};
  int pos = 0;
  XEdid_s * edi = edid;


  *list = calloc( 24, sizeof(XEdidKeyValue_s) );

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
    return XEDID_WRONG_SIGNATURE;
  }

  sprintf( mnf, "%c%c%c",
          (char)((edi->mnft_id[0] & 124) >> 2) + 'A' - 1,
          (char)((edi->mnft_id[0] & 3) << 3) + ((edi->mnft_id[1] & 227) >> 5) + 'A' - 1,
          (char)(edi->mnft_id[1] & 31) + 'A' - 1 );

  /* MSB */
  if(!edidBigEndian())
  {
    ser[0] = edi->mnft_id[1];
    ser[1] = edi->mnft_id[0];
    mnft_id = (*((uint16_t*)ser));
  }
  else
    mnft_id = (*((uint16_t*)edi->mnft_id));
  /* LSB */
  if(edidBigEndian())
  {
    ser[0] = edi->model_id[1];
    ser[1] = edi->model_id[0];
    model_id = (*((uint16_t*)ser));
  }
  else
    model_id = (*((uint16_t*)edi->model_id));

  SET_INT( mnft_id )
  SET_INT( model_id )

  /*printf( "MNF_ID: %d %d SER_ID: %d %d D:%d/%d bxh:%dx%dcm %s\n",
           edi->MNF_ID[0], edi->MNF_ID[1], edi->SER_ID[0], edi->SER_ID[1],
           edi->WEEK, edi->YEAR +1990,
           edi->width, edi->height, mnf );*/

  week = edi->week;
  year = edi->year + 1990;
  SET_INT( week )
  SET_INT( year )

  for( i = 0; i < 4; ++i)
  {
    unsigned char *block = edi->text1 + i * 18;
    char **target = NULL,
         * tmp = 0;

    if(block[0] == 0 && block[1] == 0 && block[2] == 0)
    {
      if( block[3] == 255 ) { /* serial */
        target = &serial;
      } else if( block[3] == 254 ) { /* vendor */
        target = &vendor;
      } else if( block[3] == 253 ) { /* frequenz ranges */
      } else if( block[3] == 252 ) { /* model */
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
            if(block[3] == 254)
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

  decode_color_characteristics( edid, c );

  XEdidSetDouble( &(*list)[pos++], XEDID_KEY_REDx, c[0] );
  XEdidSetDouble( &(*list)[pos++], XEDID_KEY_REDy, c[1] );
  XEdidSetDouble( &(*list)[pos++], XEDID_KEY_GREENx, c[2] );
  XEdidSetDouble( &(*list)[pos++], XEDID_KEY_GREENy, c[3] );
  XEdidSetDouble( &(*list)[pos++], XEDID_KEY_BLUEx, c[4] );
  XEdidSetDouble( &(*list)[pos++], XEDID_KEY_BLUEy, c[5] );
  XEdidSetDouble( &(*list)[pos++], XEDID_KEY_WHITEy, c[6] );
  XEdidSetDouble( &(*list)[pos++], XEDID_KEY_WHITEx, c[7] );

  /* Gamma */
  if (edi->gamma_factor == 0xFF)
    c[8] = -1.0;
  else
    c[8] = (edi->gamma_factor + 100.0) / 100.0;

  XEdidSetDouble( &(*list)[pos++], XEDID_KEY_GAMMA, c[8] );

  {
    t = (char*)malloc( 24 );
    if(!strcmp(mnf,"APP"))
      sprintf(t, "Apple");
    else if(!strcmp(mnf,"PHL"))
      sprintf(t, "Philips");
    else if(!strcmp(mnf,"HWP"))
      sprintf(t, "HP");
    else if(!strcmp(mnf,"NEC"))
      sprintf(t, "NEC");
    else if(!strcmp(mnf,"EIZ"))
      sprintf(t, "EIZO");
    else if(!strcmp(mnf,"MEI"))
      sprintf(t, "Panasonic");
    else if(!strcmp(mnf,"MIR"))
      sprintf(t, "miro");
    else if(!strcmp(mnf,"SNI"))
      sprintf(t, "Siemens Nixdorf");
    else if(!strcmp(mnf,"SNY"))
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

XEDID_ERROR_e  XEdidFree             ( XEdidKeyValue_s  ** list )
{
  int pos = 0;
  XEdidKeyValue_s * l = 0;

  if(!list)
    return XEDID_OK;

  l = *list;

  while(l[pos].key)
  {
    if(l[pos].type == XEDID_VALUE_TEXT && l[pos].value.text)
      free( l[pos].value.text );
    ++pos;
  }

  free(*list);
  *list = 0;

  return XEDID_OK;
}
const char *   XEdidErrorToString    ( XEDID_ERROR_e       error )
{
  const char * text = 0;
  switch(error)
  {
  case XEDID_OK: text = ""; break;
  case XEDID_WRONG_SIGNATURE: text = "Could not verifiy EDID"; break;
  }
  return text;
}


/* convinience functions */
XEDID_ERROR_e  XEdidPrintString      ( void              * edid,
                                       char             ** text,
                                       void             *(*alloc)(size_t sz) )
{
  XEdidKeyValue_s * l = 0;
  int count = 0, i;
  XEDID_ERROR_e err = XEdidParse( edid, &l, &count );
  char * txt = alloc(1024);

  txt[0] = 0;

  for(i = 0; i < count; ++i)
  {
    sprintf( &txt[strlen(txt)], "%s: ", l[i].key );
    if(l[i].type == XEDID_VALUE_TEXT)
      sprintf( &txt[strlen(txt)], "\"%s\"\n", l[i].value.text);
    if(l[i].type == XEDID_VALUE_INT)
      sprintf( &txt[strlen(txt)], "%d\n", l[i].value.integer);
    if(l[i].type == XEDID_VALUE_DOUBLE)
      sprintf( &txt[strlen(txt)], "%g\n", l[i].value.dbl);
  }

  if(count)
    *text = txt;

  XEdidFree( &l );

  return err;
}

#define  XEDID_COLOUR_MATRIX_SINGLE_GAMMA "colour_matrix.edid.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma"
XEDID_ERROR_e  XEdidComposeString    ( void              * edid,
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
    c[1]/*red_y*/ = decode_fraction (edid[0x1c], get_bits (edid[0x19], 5, 4));
    c[2]/*green_x*/ = decode_fraction (edid[0x1d], get_bits (edid[0x19], 2, 3));
    c[3]/*green_y*/ = decode_fraction (edid[0x1e], get_bits (edid[0x19], 0, 1));
    c[4]/*blue_x*/ = decode_fraction (edid[0x1f], get_bits (edid[0x1a], 6, 7));
    c[5]/*blue_y*/ = decode_fraction (edid[0x20], get_bits (edid[0x1a], 4, 5));
    c[6]/*white_x*/ = decode_fraction (edid[0x21], get_bits (edid[0x1a], 2, 3));
    c[7]/*white_y*/ = decode_fraction (edid[0x22], get_bits (edid[0x1a], 0, 1));

    return TRUE;
}

/* END edid-parse.c_SECTION */


