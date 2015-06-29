/** @file oyranos_icc.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2007-2013 (C) Kai-Uwe Behrmann
 *
 *  @brief    ICC definitions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2007/07/03
 */


#ifndef OYRANOS_ICC_H
#define OYRANOS_ICC_H

#include <stdlib.h> /* for linux size_t */
#include "oyranos_definitions.h"
#include "oyranos_version.h"
#include "icc34.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

icUInt16Number          oyValueUInt16   (icUInt16Number val);
icUInt32Number          oyValueUInt32   (icUInt32Number val);
icS15Fixed16Number      oyValueInt32    (icS15Fixed16Number val);
unsigned long           oyValueUInt64   (icUInt64Number val);

icColorSpaceSignature   oyValueCSpaceSig(icColorSpaceSignature val);
icPlatformSignature     oyValuePlatSig  (icPlatformSignature val);
icProfileClassSignature oyValueProfCSig (icProfileClassSignature val);
icTagSignature          oyValueTagSig   (icTagSignature val);


int            oyICCColorSpaceGetChannelCount ( icColorSpaceSignature color );
icColorSpaceSignature * oyICCGetColorSpaceWithChannelCount ( int channels_n );
const char   * oyICCColorSpaceGetName ( icColorSpaceSignature sig );
const char   * oyICCColorSpaceGetChannelName ( icColorSpaceSignature sig,
                                       int                 pos,
                                       int                 type );
const char   * oyICCTagDescription   ( icTagSignature      sig );
const char   * oyICCTagName          ( icTagSignature      sig );
const char   * oyICCDeviceClassDescription ( icProfileClassSignature sig );
const char   * oyICCPlatformDescription ( icPlatformSignature platform );
const char   * oyICCCmmDescription   ( icSignature         sig );
const char   * oyICCTagTypeName      ( icTagTypeSignature  sig );
const char   * oyICCTechnologyDescription ( icTechnologySignature sig );
const char   * oyICCChromaticityColorantDescription ( icSignature sig );
const char   * oyICCIlluminantDescription ( icIlluminant sig );
const char   * oyICCStandardObserverDescription ( icStandardObserver sig );
const char   * oyICCMeasurementGeometryDescription ( icMeasurementGeometry sig );
const char   * oyICCMeasurementFlareDescription ( icMeasurementFlare sig );



/* additionals - not defined in icc34.h */
/* partitially taken from littleCMS and from elsewhere */

#ifndef icSigParametricCurveType
#define icSigParametricCurveType 0x70617261 /* para */
#endif

#ifndef icSigLutAtoBType
#define icSigLutAtoBType 0x6D414220 /* mAB */
#endif

#ifndef icSigLutBtoAType
#define icSigLutBtoAType 0x6D424120 /* mBA */
#endif

#ifndef icSigColorantOrderType
#define icSigColorantOrderType 0x636c726f /* clro */
#endif

#ifndef icSigColorantTableType
#define icSigColorantTableType 0x636c7274 /* clrt */
#endif

#ifndef icSigColorantTableOutType
#define icSigColorantTableOutType 0x636c6f74 /* clot */
#endif

#ifndef icSigDictType
#define icSigDictType 0x64696374 /* 'dict' */
#endif

#ifndef icSigMetaDataTag
#define icSigMetaDataTag 0x6d657461 /* 'meta' */
#endif

#ifndef icSigDescriptiveNameValueMuArrayType_
#define icSigDescriptiveNameValueMuArrayType_ 0x6e766d74 /* 'nvmt' */
#endif

#ifndef icSigProfileDetailDescriptionTag_
#define icSigProfileDetailDescriptionTag_ 0x70646474 /* 'pddt' */
#endif

#ifndef icSigUnix
#define icSigUnix 0x2A6E6978  /* '*nix' */
#endif

#ifndef icSigDeviceSettingsTag
#define icSigDeviceSettingsTag 0x44657653 /* DevS */
#endif

#ifndef icSigDeviceSettingsType
#define icSigDeviceSettingsType 0x44657653 /* DevS */
#endif

#ifndef icSigWCSProfileTag
#define icSigWCSProfileTag 0x4D533130 /* MS10 */
#endif

#ifndef icSigProfileSequenceIdentifierTag
#define icSigProfileSequenceIdentifierTag 0x70736964 /* psid */
#endif

#ifndef icSigMultiLocalizedUnicodeType
#define icSigMultiLocalizedUnicodeType 0x6D6C7563  /* 'mluc' */
#endif

#ifndef icSigProfileSequenceIdentifierType
#define icSigProfileSequenceIdentifierType 0x70736964  /* 'psid' */
#endif

#ifndef icSigMakeAndModelType
#define icSigMakeAndModelType 0x6d6d6f64 /* 'mmod' */
#endif

#ifndef icSigNativeDisplayInfoType
#define icSigNativeDisplayInfoType 0x6e64696e /* 'ndin' */
#endif

#ifndef icSigVideoCardGammaTable
#define icSigVideoCardGammaTable 0x76636774 /* 'vcgt' */
#endif


/** @internal
 *  @brief deviceSettingsType tag for ICC profiles
 *  @ingroup icc_low
 *
 *  For details see: http://www.oyranos.org/wiki/index.php?title=Device_Settings_in_ICC_0.2
 *
 *  The tag size will be 84 + deviceSettingsType::data_size.
 *
 *  @see icDictTagType
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/04 (Oyranos: 0.1.10)
 *  @date    2009/01/04
 */
typedef struct {
  icTagTypeSignature type;             /**< "DevS" */
  icUInt32Number   dummy;              /**< keep zero */
  icUInt8Number    version;            /**< 1 - for version 0.2 */
  char             device_serial[12];  /**< 12 byte field, null terminated, unused bytes must be set to zero */
  char             driver_name[12];    /**< 12 byte field, null terminated, unused bytes must be set to zero */
  char             driver_version[12]; /**< 12 byte field, null terminated, unused bytes must be set to zero */
  char             driver_signature[12];/**< 12 byte field, null terminated, unused bytes must be set to zero */
  icUInt8Number    priority;           /**< priority of appliance (low - 0 ... high - 255) */
  icUInt8Number    reserved[22];       /**< keep to zero */
  icUInt32Number   data_size;          /**< as follows starting from byte 84 */
  /* with data_size being > zero the byte 84 is the starting one */
} deviceSettingsType;

typedef struct {
  icUInt32Number name_string_offset;
  icUInt32Number name_string_size;
  icUInt32Number value_string_offset;
  icUInt32Number value_string_size;
  icUInt32Number display_name_string_offset;
  icUInt32Number display_name_string_size;
  icUInt32Number display_value_string_offset;
  icUInt32Number display_value_string_size;
} icNameValueRecord;

typedef struct {
  icUInt32Number sig;
  icUInt32Number reserved;
  icUInt32Number number;
  icUInt32Number size;
  /* icNameValueRecord record[0]; */
} icDictTagType;



#ifndef icSigChromaticityType
#define icSigChromaticityType 0x6368726D /* chrm */
#endif

#ifndef icSigMCH5Data
#define icSigMCH5Data 1296255029 /* MCH5 */
#endif
#ifndef icSigMCH6Data
#define icSigMCH6Data 1296255030 /* MCH6 */
#endif
#ifndef icSigMCH7Data
#define icSigMCH7Data 1296255031 /* MCH7 */
#endif
#ifndef icSigMCH8Data
#define icSigMCH8Data 1296255032 /* MCH8 */
#endif
#ifndef icSigMCH9Data
#define icSigMCH9Data 1296255033 /* MCH9 */
#endif
#ifndef icSigMCHAData
#define icSigMCHAData 1296255041 /* MCHA */
#endif
#ifndef icSigMCHBData
#define icSigMCHBData 1296255042 /* MCHB */
#endif
#ifndef icSigMCHCData
#define icSigMCHCData 1296255043 /* MCHC */
#endif
#ifndef icSigMCHDData
#define icSigMCHDData 1296255044 /* MCHD */
#endif
#ifndef icSigMCHEData
#define icSigMCHEData 1296255045 /* MCHE */
#endif
#ifndef icSigMCHFData
#define icSigMCHFData 1296255046 /* MCHF */
#endif
#ifndef icSiglcms
#define icSiglcmsData 1818455411 /* lcms */
#endif
#ifndef icSigLuvKData
#define icSigLuvKData 1282766411 /* LuvK */
#endif
#ifndef icSigJab1Data
#define icSigJab1Data 1247896113 /* Jab1 */
#endif
#ifndef icSigJab2Data
#define icSigJab2Data 1247896114 /* Jab2 */
#endif
#ifndef icSigJcj2Data
#define icSigJcj2Data 1248029234 /* Jcj2 */
#endif
#ifndef icSigChromaticityTag
#define icSigChromaticityTag 1667789421 /* chrm */
#endif
#ifndef icSigHPGamutDescTag
#define icSigHPGamutDescTag 1735226433 /* gmtA */
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_ICC_H */
