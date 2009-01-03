/** @file oyranos_icc.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2007-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    ICC definitions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2007/07/03
 */


#ifndef OYRANOS_ICC_H
#define OYRANOS_ICC_H

#include <stdlib.h> /* for linux size_t */
#include "oyranos_definitions.h"
#include "oyranos_version.h"
#include "icc34.h"

/** \namespace oyranos
    @brief The Oyranos namespace.
 */
#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */

icUInt16Number          oyValueUInt16   (icUInt16Number val);
icUInt32Number          oyValueUInt32   (icUInt32Number val);
unsigned long           oyValueUInt64   (icUInt64Number val);
icColorSpaceSignature   oyValueCSpaceSig(icColorSpaceSignature val);
icPlatformSignature     oyValuePlatSig  (icPlatformSignature val);
icProfileClassSignature oyValueProfCSig (icProfileClassSignature val);
icTagSignature          oyValueTagSig   (icTagSignature val);


int            oyICCColourSpaceGetChannelCount ( icColorSpaceSignature color );
const char   * oyICCColourSpaceGetName ( icColorSpaceSignature sig );
const char   * oyICCColourSpaceGetChannelName ( icColorSpaceSignature sig,
                                       int                 pos,
                                       int                 type );
const char   * oyICCTagDescription   ( icTagSignature      sig );
const char   * oyICCTagName          ( icTagSignature      sig );
const char   * oyICCDeviceClassDescription ( icProfileClassSignature sig );
const char   * oyICCPlatformDescription ( icPlatformSignature platform );
const char   * oyICCTagTypeName      ( icTagTypeSignature  sig );
const char   * oyICCTechnologyDescription ( icTechnologySignature sig );
const char   * oyICCChromaticityColorantDescription ( icSignature sig );
const char   * oyICCIlluminantDescription ( icIlluminant sig );
const char   * oyICCStandardObserverDescription ( icStandardObserver sig );
const char   * oyICCMeasurementGeometryDescription ( icMeasurementGeometry sig );
const char   * oyICCMeasurementFlareDescription ( icMeasurementFlare sig );



/* additionals - not defined in icc34.h */
/* partitially taken from littleCMS */

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

#ifndef HAVE_LCMS

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

#endif /* HAVE_LCMS */

#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_H */
