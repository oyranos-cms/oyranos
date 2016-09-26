int            oyImage_GetWidth      ( oyImage_s         * image );
int            oyImage_GetHeight     ( oyImage_s         * image );
int            oyImage_GetPixelLayout( oyImage_s         * image,
                                       oyLAYOUT_e          type );
oyCHANNELTYPE_e  oyImage_GetChannelType (
                                       oyImage_s         * image,
                                       int                 pos );
int            oyImage_GetSubPositioning (
                                       oyImage_s         * image );
oyOptions_s *  oyImage_GetTags       ( oyImage_s         * image );
oyProfile_s *  oyImage_GetProfile    ( oyImage_s         * image );
oyStruct_s *   oyImage_GetPixelData  ( oyImage_s         * image );
oyImage_GetPoint_f oyImage_GetPointF ( oyImage_s         * image );
oyImage_GetLine_f oyImage_GetLineF   ( oyImage_s         * image );
oyImage_SetPoint_f oyImage_GetSetPointF (
                                       oyImage_s         * image );
oyImage_SetLine_f oyImage_GetSetLineF( oyImage_s         * image );
oyStruct_s *   oyImage_GetUserData   ( oyImage_s         * image );
oyImage_s *    oyImage_Create        ( int                 width,
                                       int                 height,
                                       oyPointer           channels,
                                       oyPixel_t           pixel_layout,
                                       oyProfile_s       * profile,
                                       oyObject_s          object);
oyImage_s *    oyImage_CreateForDisplay ( int              width,
                                       int                 height, 
                                       oyPointer           channels,
                                       oyPixel_t           pixel_layout,
                                       const char        * display_name,
                                       int                 window_pos_x,
                                       int                 window_pos_y,
                                       int                 window_width,
                                       int                 window_height,
                                       int                 icc_profile_flags,
                                       oyObject_s          object);
int            oyImage_FromFile      ( const char        * file_name,
                                       int                 icc_profile_flags,
                                       oyImage_s        ** image,
                                       oyObject_s          object );
int            oyImage_ToFile        ( oyImage_s         * image,
                                       const char        * file_name,
                                       oyOptions_s       * opts );
int            oyImage_SetCritical   ( oyImage_s         * image,
                                       oyPixel_t           pixel_layout,
                                       oyProfile_s       * profile,
                                       oyOptions_s       * tags,
                                       int                 width,
                                       int                 height );
int            oyImage_SetData       ( oyImage_s         * image,
                                       oyStruct_s       ** pixel_data,
                                       oyImage_GetPoint_f  getPoint,
                                       oyImage_GetLine_f   getLine,
                                       oyImage_GetTile_f   getTile,
                                       oyImage_SetPoint_f  setPoint,
                                       oyImage_SetLine_f   setLine,
                                       oyImage_SetTile_f   setTile );
int            oyImage_FillArray     ( oyImage_s         * image,
                                       oyRectangle_s     * rectangle,
                                       int                 do_copy,
                                       oyArray2d_s      ** array,
                                       oyRectangle_s     * array_rectangle,
                                       oyObject_s          obj );
int            oyImage_ReadArray     ( oyImage_s         * image,
                                       oyRectangle_s     * rectangle,
                                       oyArray2d_s       * array,
                                       oyRectangle_s     * array_rectangle );
int            oyImage_RoiToSamples  ( oyImage_s         * image,
                                       oyRectangle_s     * roi,
                                       oyRectangle_s    ** sample_rectangle );
int            oyImage_SamplesToRoi  ( oyImage_s         * image,
                                       oyRectangle_s     * sample_rectangle,
                                       oyRectangle_s    ** roi );
int            oyImage_SamplesToPixels(oyImage_s         * image,
                                       oyRectangle_s     * sample_rectangle,
                                       oyRectangle_s     * pixel_rectangle );
int            oyImage_PixelsToSamples(oyImage_s         * image,
                                       oyRectangle_s     * pixel_rectangle,
                                       oyRectangle_s     * sample_rectangle );
int            oyImage_WritePPM      ( oyImage_s         * image,
                                       const char        * file_name,
                                       const char        * comment );
