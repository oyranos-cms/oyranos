/** @file lcm2_profiler.h
 *
 *  @par Copyright:
 *            2009-2019 (C) Kai-Uwe Behrmann
 *
 *  @brief    littleCMS CMM profile generator for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/MIT>
 *  @since    2009/10/24
 */

#ifndef LCM2_PROFILER_H
#define LCM2_PROFILER_H

#include <lcms2.h>

/** \addtogroup profiler
 *  @{ */

#define LCM2PROFILER_MAJOR 1 /**< @brief major version */
#define LCM2PROFILER_MINOR 0 /**< @brief minor version */
#define LCM2PROFILER_MICRO 0 /**< @brief patch version */

/** @brief compile time API Version
 */
#define LCM2PROFILER_API (LCM2PROFILER_MAJOR*10000 + LCM2PROFILER_MINOR*100 + LCM2PROFILER_MICRO)

/** @brief ICC 2011 license template
 */
#define ICC_2011_LICENSE "This profile is made available by %s, with permission of %s, and may be copied, distributed, embedded, made, used, and sold without restriction. Altered versions of this profile shall have the original identification and copyright information removed and shall not be misrepresented as the original profile."


/** @brief Manipulation function type for LUT filling
 *
 *  A pure function based sampler in double precission. i and o can be
 *  in different color spaces.
 *
 *  Predefined samplers can be found in @ref samplers.
 *
 *  @param[in]     i                   input doubles from internal LUT
 *                                     generation code
 *  @param[out]    o                   output doubles in destination space
 *  @param[in,out] vars                user variables as passed in from
 *                                     lcm2CreateProfileLutByFuncAndCurves()
 *                                     and friends
 */
typedef void (*lcm2Sampler_f)        ( const double        i[],
                                             double        o[],
                                       void              * vars);
/** @} */ /* profiler */

int          lcm2CreateAbstractProfile (
                                       lcm2Sampler_f       samplerMySpace,
                                       void              * samplerArg,
                                       const char        * my_space_profile,
                                       int                 grid_size,
                                       double              icc_profile_version,
                                       const char        * my_abstract_description,
                                       const char       ** my_abstract_descriptions,
                                       const char        * my_abstract_file_name,
                                       const char        * provider,
                                       const char        * vendor,
                                       const char        * my_license,
                                       const char        * device_model,
                                       const char        * device_manufacturer,
                                       const char       ** my_meta_data,
                                       cmsHPROFILE       * h_profile
                                     );
/** @brief *xyz range alignment
 *
 *  This must be used for cmsToneCurve and matrix scaling.
 */
#define LCM2_ADAPT_TO_PCS_XYZ (1.0 + (32767.0/32768.0))
int          lcm2CreateAbstractProfileM (
                                       cmsToneCurve      * m_curve,
                                       const double      * matrix,
                                       cmsToneCurve      * b_curve,
                                       double              icc_profile_version,
                                       const char        * my_abstract_description,
                                       const char       ** my_abstract_descriptions,
                                       const char        * my_abstract_file_name,
                                       const char        * provider,
                                       const char        * vendor,
                                       const char        * my_license,
                                       const char        * device_model,
                                       const char        * device_manufacturer,
                                       const char       ** my_meta_data,
                                       cmsHPROFILE       * h_profile
                                     );
int          lcm2CreateAbstractTemperatureProfile (
                                       float               kelvin,
                                       cmsHPROFILE         source_white_profile,
                                       int                 grid_size,
                                       double              icc_profile_version,
                                       char             ** my_abstract_file_name,
                                       cmsHPROFILE       * h_profile
                                     );
int          lcm2CreateAbstractWhitePointProfileLab (
                                       double              cie_a,
                                       double              cie_b,
                                       int                 grid_size,
                                       double              icc_profile_version,
                                       char             ** my_abstract_file_name,
                                       cmsHPROFILE       * h_profile
                                     );
int          lcm2CreateAbstractWhitePointProfileBradford (
                                       double            * src_iccXYZ,
                                       const char        * src_name,
                                       double            * illu_iccXYZ,
                                       const char        * illu_name,
                                       double            * scale,
                                       double              icc_profile_version,
                                       int                 flags,
                                       char             ** my_abstract_file_name,
                                       cmsHPROFILE       * h_profile
                                     );
int          lcm2CreateCalibrationProfileM (
                                       cmsToneCurve     ** m_curves,
                                       const char        * csp,
                                       double              icc_profile_version,
                                       const char        * my_calibration_description,
                                       const char       ** my_calibration_descriptions,
                                       const char        * my_calibration_file_name,
                                       const char        * provider,
                                       const char        * vendor,
                                       const char        * my_license,
                                       const char        * device_model,
                                       const char        * device_manufacturer,
                                       const char       ** my_meta_data,
                                       cmsHPROFILE       * h_profile
                                     );
cmsHPROFILE  lcm2OpenProfileFile     ( const char        * my_space_profile,
                                       const char        * my_space_profile_path );
char *       lcm2WriteProfileToFile  ( cmsHPROFILE         my_space_profile,
                                       const char        * my_space_profile_name,
                                       const char        * my_space_profile_version,
                                       const char        * vendor_four_bytes );
void *       lcm2WriteProfileToMem   ( cmsHPROFILE       * profile,
                                       size_t            * size,
                                       void *            (*allocateFunc)(size_t size) );
cmsHPROFILE  lcm2CreateICCMatrixProfile2 (
                                       float             gamma,
                                       float rx, float ry,
                                       float gx, float gy,
                                       float bx, float by,
                                       float wx, float wy );
cmsHPROFILE  lcm2CreateProfileFragment(
                                       const char        * in_space_profile,
                                       const char        * out_space_profile,
                                       double              icc_profile_version,
                                       const char        * my_abstract_description,
                                       const char        * provider,
                                       const char        * vendor,
                                       const char        * my_license,
                                       const char        * device_model,
                                       const char        * device_manufacturer,
                                       cmsHPROFILE         h_profile
                                     );
void         lcm2AddMluDescription   ( cmsHPROFILE         profile,
                                       const char        * texts[],
                                       cmsTagSignature     tag_sig );
void         lcm2AddMetaTexts        ( cmsHPROFILE         profile,
                                       const char        * prefixes,
                                       const char        * key_value[],
                                       cmsTagSignature     tag_sig );
int          lcm2CreateProfileLutByFunc (
                                       cmsHPROFILE         profile,
                                       lcm2Sampler_f       samplerMySpace,
                                       void              * samplerArg,
                                       const char        * in_space_profile,
                                       const char        * my_space_profile,
                                       const char        * out_space_profile,
                                       int                 grid_size,
                                       cmsTagSignature     tag_sig
                                     );
int          lcm2CreateProfileLutByFuncAndCurves (
                                       cmsHPROFILE         profile,
                                       lcm2Sampler_f       samplerMySpace,
                                       void              * samplerArg,
                                       cmsToneCurve      * in_curves[],
                                       cmsToneCurve      * out_curves[],
                                       const char        * in_space_profile,
                                       const char        * my_space_profile,
                                       const char        * out_space_profile,
                                       int                 grid_size,
                                       cmsTagSignature     tag_sig
                                     );
int          lcm2CreateProfileLutByMatrixAndCurves (
                                       cmsHPROFILE         profile,
                                       cmsToneCurve      * in_curves[],
                                       const double      * matrix,
                                       cmsToneCurve      * out_curves[],
                                       const char        * in_space_profile,
                                       const char        * out_space_profile,
                                       cmsTagSignature     tag_sig
                                     );


/* PCS color space conversions */
void         lcm2iccLab2CIEXYZ       ( const double      * icc_Lab,
                                       cmsCIEXYZ         * XYZ );
void         lcm2CIEXYZ2iccLab       ( const cmsCIEXYZ   * XYZ,
                                       double            * icc_Lab );
void         lcm2iccXYZ2iccLab       ( const double      * XYZ,
                                       double            * icc_Lab );
/* color space converters */
void         lcm2SamplerLab2LCh      ( const double        i[],
                                       double              o[],
                                       void              * none );
void         lcm2SamplerLCh2Lab      ( const double        i[],
                                       double              o[],
                                       void              * none );
void         lcm2SamplerLab2JCh      ( const double        i[],
                                       double              o[],
                                       void              * viewing_condition );
void         lcm2SamplerLCh2Lab      ( const double        i[],
                                       double              o[],
                                       void              * viewing_condition );
void         lcm2SamplerRGB2JpegYCbCr( const double        i[],
                                       double              o[],
                                       void              * none );
void         lcm2SamplerJpegYCbCr2RGB( const double        i[],
                                       double              o[],
                                       void              * none );
/* effects */
void         lcm2SamplerIdendity     ( const double        i[],
                                       double              o[],
                                       void              * none );
void         lcm2SamplerGrayer       ( const double        i[],
                                       double              o[],
                                       void              * none );
void         lcm2SamplerBlacknWhite  ( const double        i[],
                                       double              o[],
                                       void              * none );
void         lcm2SamplerSepia        ( const double        i[],
                                       double              o[],
                                       void              * none );
void         lcm2SamplerReddish      ( const double        i[],
                                       double              o[],
                                       void              * none );
void         lcm2SamplerWhitePointLab( const double        i[],
                                       double              o[],
                                       void              * data );
void lcm2SamplerWhitePointBradford   ( const double        i[],
                                       double              o[],
                                       void              * data );
void         lcm2SamplerProof        ( const double        i[],
                                             double        o[],
                                       void              * data );
void         lcm2SamplerProofD       ( const double        i[],
                                             double        o[],
                                       void              * data );



typedef int  (*lcm2Message_f)        ( int/*oyMSG_e*/      code, 
                                       const void        * context,
                                       const char        * format,
                                       ... );
int            lcm2MessageFuncSet    ( lcm2Message_f       message_func );

int            lcm2Version           ( );

/** \addtogroup profiler
 *  @{ */

/* The oyVEC3, oyMAT3, oyMAT3inverse, oyVEC3init and oyMAT3per definitions 
 * origin from lcms2' cmsmtrx.c written by Marti Maria www.littlecms.com 
 * and is MIT licensed there
 */
/** @brief Vector of 3 doubles
 */
typedef struct {
  double n[3];                         /**< triple of doubles */ 
} lcm2VEC3;
 
/** @brief 3x3 Matrix
 */
typedef struct {
  lcm2VEC3 v[3];                         /**< 3x3 matrix */
} lcm2MAT3;

/** @} */ /* profiler */

int          lcm2MAT3inverse         ( const lcm2MAT3    * a,
                                       lcm2MAT3          * b);
void         lcm2VEC3init            ( lcm2VEC3          * r,
                                       double              x,
                                       double              y,
                                       double              z );
void         lcm2MAT3per             ( lcm2MAT3          * r,
                                       const lcm2MAT3    * a,
                                       const lcm2MAT3    * b );
void         lcm2MAT3eval            ( lcm2VEC3          * r,
                                       const lcm2MAT3    * a,
                                       const lcm2VEC3    * v );
int          lcm2AdaptationMatrix    ( lcm2MAT3          * r,
                                       const lcm2MAT3    * ConeMatrix,
                                       const cmsCIEXYZ   * FromIll,
                                       const cmsCIEXYZ   * ToIll );

/* End of lcms code */

/** \addtogroup profiler
 *  @{ */

/** @brief CIE*xy
 */
typedef struct {
  double xy[2];                        /**< CIE*xy */
} lcm2CIExyY;
/** @brief CIE*xy triple
 */
typedef struct {
  lcm2CIExyY v[3];                     /**< CIE*xy triple */
} lcm2CIExyYTriple;

/** @} */ /* profiler */

int          lcm2MAT3toCIExyYTriple  ( const lcm2MAT3    * a,
                                       lcm2CIExyYTriple  * triple );
const char * lcm2MAT3show ( const lcm2MAT3* a );
const char * lcm2Mat34show ( const float a[3][4] );
const char * lcm2Mat4show ( const float a[4] );
const char * lcm2Mat43show ( const float a[4][3] );
const char * lcm2CIExyYTriple_Show( lcm2CIExyYTriple * triple );

#endif /* LCM2_PROFILER_H */
