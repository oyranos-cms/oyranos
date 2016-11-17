/** XcmEdidParse.h
 *
 *  Xcm Xorg Colour Management
 *
 *  @par Copyright:
 *            2005-2011 (C) Kai-Uwe Behrmann
 *
 *  @brief    EDID data block parsing
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2005/01/31
 */

#ifndef XCM_EDID_PARSE_H
#define XCM_EDID_PARSE_H
#include <stddef.h> /* size_t */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** \addtogroup XcmEdidParse
 *
 *  @{
 */

/* @internal
 * DDC struct */
typedef struct {
  unsigned char sig[8];
  unsigned char mnft_id[2];            /* [8] manufaturer ID */
  unsigned char model_id[2];           /* [10] model ID */
  unsigned char ser_id[4];             /* [12] serial ID */
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
} XcmEdid_s;

typedef enum {
  XCM_EDID_OK,
  XCM_EDID_WRONG_SIGNATURE
} XCM_EDID_ERROR_e;

typedef enum {
  XCM_EDID_VALUE_TEXT,
  XCM_EDID_VALUE_INT,
  XCM_EDID_VALUE_DOUBLE
} XCM_EDID_VALUE_e;

union XcmEdidValue_u {
  char * text;
  double dbl;
  int    integer;
};

typedef struct {
  const char         * key;
  XCM_EDID_VALUE_e        type;
  union XcmEdidValue_u   value;
} XcmEdidKeyValue_s;

/* basic access functions */
XCM_EDID_ERROR_e  XcmEdidParse        ( void              * edid,
                                       XcmEdidKeyValue_s** list,
                                       int               * count );
XCM_EDID_ERROR_e  XcmEdidFree        ( XcmEdidKeyValue_s** list );
const char *      XcmEdidErrorToString(XCM_EDID_ERROR_e    error );

/* convinience functions */
XCM_EDID_ERROR_e  XcmEdidPrintString ( void              * edid,
                                       char             ** text,
                                       void             *(*alloc)(size_t sz) );
XCM_EDID_ERROR_e  XcmEdidPrintOpenIccJSON ( void              * edid,
                                       char             ** text,
                                       void             *(*alloc)(size_t sz) );
/** full length vendor, string */
#define XCM_EDID_KEY_VENDOR               "vendor"
/** full length device model, string */
#define XCM_EDID_KEY_MODEL                "model"
/** full length device serial number, string */
#define XCM_EDID_KEY_SERIAL               "serial"
/** colorimetric primary red CIE*xyY x component, double */
#define XCM_EDID_KEY_REDx                 "red_x"
/** colorimetric primary red CIE*xyY y component, double */
#define XCM_EDID_KEY_REDy                 "red_y"
/** colorimetric primary green CIE*xyY x component, double */
#define XCM_EDID_KEY_GREENx               "green_x"
/** colorimetric primary green CIE*xyY y component, double */
#define XCM_EDID_KEY_GREENy               "green_y"
/** colorimetric primary blue CIE*xyY x component, double */
#define XCM_EDID_KEY_BLUEx                "blue_x"
/** colorimetric primary blue CIE*xyY y component, double */
#define XCM_EDID_KEY_BLUEy                "blue_y"
/** colorimetric primary white CIE*xyY x component, double */
#define XCM_EDID_KEY_WHITEx               "white_x"
/** colorimetric primary white CIE*xyY y component, double */
#define XCM_EDID_KEY_WHITEy               "white_y"
/** colorimetric single gamma for all channels, double */
#define XCM_EDID_KEY_GAMMA                "gamma"
/** manufactur week, integer */
#define XCM_EDID_KEY_WEEK                 "week"
/** manufactur year, integer */
#define XCM_EDID_KEY_YEAR                 "year"
/** manufacturer ID, integer */
#define XCM_EDID_KEY_MNFT_ID              "mnft_id"
/** model ID, integer */
#define XCM_EDID_KEY_MODEL_ID             "model_id"
/** manufacturer ID, string */
#define XCM_EDID_KEY_MNFT                 "mnft"
/** full length manufacturer, string */
#define XCM_EDID_KEY_MANUFACTURER         "manufacturer"
/** transfer value for red component, double */
#define XCM_EDID_KEY_A3RED                "a3red"
/** transfer value for red component, double */
#define XCM_EDID_KEY_A2RED                "a2red"
/** transfer value for green component, double */
#define XCM_EDID_KEY_A3GREEN              "a3green"
/** transfer value for green component, double */
#define XCM_EDID_KEY_A2GREEN              "a2green"
/** transfer value for blue component, double */
#define XCM_EDID_KEY_A3BLUE               "a3blue"
/** transfer value for blue component, double */
#define XCM_EDID_KEY_A2BLUE               "a2blue"

/* @} XcmEdidParse */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* XCM_EDID_PARSE_H */
