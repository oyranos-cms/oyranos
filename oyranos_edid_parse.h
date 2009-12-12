/** @file oyranos_edid_parse.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2005-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    monitor device detection
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2005/01/31
 */

#ifndef OYRANOS_EDID_PARSE_H
#define OYRANOS_EDID_PARSE_H
#include <stddef.h> /* size_t */

/** @brief \internal DDC struct */
typedef struct {
  unsigned char sig[8];
  unsigned char mnft_id[2];            /* [8] manufaturer ID */
  unsigned char model_id[2];           /* [10] model ID */
  unsigned char ser_id[2];             /* [12] serial ID */
  unsigned char dummy_li[2];
  unsigned char week;                  /* [16] Week */
  unsigned char year;                  /* [17] + 1990 => Year */
  unsigned char major_version;         /* [18] */
  unsigned char minor_version;         /* [19] */
  unsigned char video_input_type;      /* [20] */
  unsigned char width;                 /* [21] */
  unsigned char height;                /* [22] */
  unsigned char gamma_factor;          /* [23] */
  unsigned char dpms;                  /* [24] */
  unsigned char rg;                    /* [25] colour information */
  unsigned char wb;                    /* [26] */
  unsigned char rY;                    /* [27] */
  unsigned char rX;                    /* [28] */
  unsigned char gY;                    /* [29] */
  unsigned char gX;                    /* [30] */
  unsigned char bY;                    /* [31] */
  unsigned char bX;                    /* [32] */
  unsigned char wY;                    /* [33] */
  unsigned char wX;                    /* [34] */
  unsigned char etiming1;              /* [35] */
  unsigned char etiming2;              /* [36] */
  unsigned char mtiming;               /* [37] */
  unsigned char stdtiming[16];         /* [38] */
  unsigned char text1[18];             /* [54] Product string */
  unsigned char text2[18];             /* [72] text 2 */
  unsigned char text3[18];             /* [90] text 3 */
  unsigned char text4[18];             /* [108] text 4 */
  unsigned char extension_blocks;      /* [126] number of following extensions*/
  unsigned char checksum;              /* [127] */
} XEdid_s;

typedef enum {
  XEDID_OK,
  XEDID_WRONG_SIGNATURE
} XEDID_ERROR_e;

typedef enum {
  XEDID_VALUE_TEXT,
  XEDID_VALUE_INT,
  XEDID_VALUE_DOUBLE
} XEDID_VALUE_e;

union XEdidValue_u {
  char * text;
  double dbl;
  int    integer;
};

typedef struct {
  const char         * key;
  XEDID_VALUE_e        type;
  union XEdidValue_u   value;
} XEdidKeyValue_s;

/* basic access functions */
XEDID_ERROR_e  XEdidParse            ( void              * edid,
                                       XEdidKeyValue_s  ** list,
                                       int               * count );
XEDID_ERROR_e  XEdidFree             ( XEdidKeyValue_s  ** list );
const char *   XEdidErrorToString    ( XEDID_ERROR_e       error );

/* convinience functions */
XEDID_ERROR_e  XEdidPrintString      ( void              * edid,
                                       char             ** text,
                                       void              (*alloc)(size_t sz) );

#endif /* OYRANOS_EDID_PARSE_H */
