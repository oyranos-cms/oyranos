OYAPI int  OYEXPORT
                 oyConversion_Correct (
                                       oyConversion_s    * conversion,
                                       const char        * registration,
                                       uint32_t            flags,
                                       oyOptions_s       * options );
OYAPI oyConversion_s *  OYEXPORT
                oyConversion_CreateBasicPixels (
                                       oyImage_s         * input,
                                       oyImage_s         * output,
                                       oyOptions_s       * options,
                                       oyObject_s          object );
OYAPI oyConversion_s *  OYEXPORT
                oyConversion_CreateBasicPixelsFromBuffers (
                                       oyProfile_s       * p_in,
                                       oyPointer           buf_in,
                                       oyPixel_t           buf_type_in,
                                       oyProfile_s       * p_out,
                                       oyPointer           buf_out,
                                       oyPixel_t           buf_type_out,
                                       oyOptions_s       * options,
                                       int                 count );
OYAPI oyConversion_s *  OYEXPORT
                oyConversion_CreateFromImage (
                                       oyImage_s         * image_in,
                                       const char        * module,
                                       oyOptions_s       * module_options,
                                       oyProfile_s       * output_profile,
                                       oyDATATYPE_e        buf_type_out,
                                       uint32_t            flags,
                                       oyObject_s          obj );
OYAPI oyFilterGraph_s *  OYEXPORT
                oyConversion_GetGraph (
                                       oyConversion_s    * conversion );
OYAPI oyImage_s *  OYEXPORT
                oyConversion_GetImage (
                                       oyConversion_s    * conversion,
                                       uint32_t            flags );
OYAPI oyFilterNode_s *  OYEXPORT
                oyConversion_GetNode (
                                       oyConversion_s    * conversion,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
                 oyConversion_GetOnePixel (
                                       oyConversion_s    * conversion,
                                       double              x,
                                       double              y,
                                       oyPixelAccess_s   * pixel_access );
OYAPI int  OYEXPORT
                 oyConversion_RunPixels (
                                       oyConversion_s    * conversion,
                                       oyPixelAccess_s   * pixel_access );
OYAPI int  OYEXPORT
                 oyConversion_Set  ( oyConversion_s    * conversion,
                                       oyFilterNode_s    * input,
                                       oyFilterNode_s    * output );
OYAPI char *  OYEXPORT
                oyConversion_ToText (
                                       oyConversion_s    * conversion,
                                       const char        * head_line,
                                       int                 reserved,
                                       oyAlloc_f           allocateFunc );
