/** @file lcm2_profiler.h
 *
 *  @par Copyright:
 *            2009-2017 (C) Kai-Uwe Behrmann
 *
 *  @brief    littleCMS CMM profile generator for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/MIT>
 *  @since    2009/10/24
 */

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


/** @brief Manipulation function type
 *
 *  A pure function based sampler in double precission. i and o can be
 *  in different color spaces.
 */
typedef void (*lcm2Sampler_f)        ( const double        i[],
                                             double        o[],
                                       void              * vars);
/** @} */ /* profiler */

int          lcm2CreateAbstractProfile(lcm2Sampler_f       samplerMySpace,
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
int          lcm2CreateAbstractTemperatureProfile (
                                       float               kelvin,
                                       cmsHPROFILE         source_white_profile,
                                       int                 grid_size,
                                       double              icc_profile_version,
                                       char             ** my_abstract_file_name,
                                       cmsHPROFILE       * h_profile
                                     );
int          lcm2CreateAbstractWhitePointProfile (
                                       double              cie_a,
                                       double              cie_b,
                                       int                 grid_size,
                                       double              icc_profile_version,
                                       char             ** my_abstract_file_name,
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


void         lcm2SamplerLab2LCh      ( const double        i[],
                                       double              o[],
                                       void              * none );
void         lcm2SamplerLCh2Lab      ( const double        i[],
                                       double              o[],
                                       void              * none );
void         lcm2SamplerRGB2JpegYCbCr( const double        i[],
                                       double              o[],
                                       void              * none );
void         lcm2SamplerJpegYCbCr2RGB( const double        i[],
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
void         lcm2SamplerWhitePoint   ( const double        i[],
                                       double              o[],
                                       void              * data );
void         lcm2SamplerProof        ( const double        i[],
                                             double        o[],
                                       void              * data );



typedef int  (*lcm2Message_f)        ( int/*oyMSG_e*/      code, 
                                       const void        * context,
                                       const char        * format,
                                       ... );
int            lcm2MessageFuncSet    ( lcm2Message_f       message_func );

int            lcm2Version           ( );
