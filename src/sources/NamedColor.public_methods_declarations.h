oyNamedColor_s *   oyNamedColor_Create(const double      * chan,
                                       const char        * blob,
                                       int                 blob_len,
                                       oyProfile_s       * profile_ref,
                                       oyObject_s          object );
oyNamedColor_s *   oyNamedColor_CreateWithName (
                                       const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const double      * chan,
                                       const double      * XYZ,
                                       const char        * blob,
                                       int                 blob_len,
                                       oyProfile_s       * profile_ref,
                                       oyObject_s          object );

oyProfile_s *      oyNamedColor_GetSpaceRef ( oyNamedColor_s  * color );
void               oyNamedColor_SetChannels (
                                       oyNamedColor_s    * color,
                                       const double      * channels,
                                       uint32_t            flags );
int                oyNamedColor_SetColorStd (
                                       oyNamedColor_s    * color,
                                       oyPROFILE_e         color_space,
                                       oyPointer           channels,
                                       oyDATATYPE_e        channels_type,
                                       uint32_t            flags,
                                       oyOptions_s       * options );
const double *    oyNamedColor_GetChannelsConst (
                                       oyNamedColor_s    * color,
                                       uint32_t          * flags );
const double *     oyNamedColor_GetXYZConst ( oyNamedColor_s * color);
int                oyNamedColor_GetColorStd (
                                       oyNamedColor_s    * color,
                                       oyPROFILE_e         color_space,
                                       oyPointer           buf,
                                       oyDATATYPE_e        buf_type,
                                       uint32_t          * flags,
                                       oyOptions_s       * options );
int                oyNamedColor_GetColor (
                                       oyNamedColor_s    * color,
                                       oyProfile_s       * profile,
                                       oyPointer           buf,
                                       oyDATATYPE_e        buf_type,
                                       uint32_t            flags,
                                       oyOptions_s       * options );
const char   *     oyNamedColor_GetName (
                                       oyNamedColor_s    * s,
                                       oyNAME_e            type,
                                       uint32_t            flags );

