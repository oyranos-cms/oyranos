/**
 *  Typedef   oyImage_GetPoint_f
 *  @memberof oyImage_s
 *  @brief    pixel accessor
 *
 *  @param[in,out] image                 the image object
 *  @param[in]     point_x               the data location in x direction
 *  @param[in]     point_y               the data location in y direction
 *  @param[in]     channel               the nth channel will is requested
 *                                       -1 means to request all channels, note
 *                                       that can be an expensive call for non 
 *                                       continous pixel layouts
 *  @param[out]    is_allocated          are the points always newly allocated?
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
typedef oyPointer (*oyImage_GetPoint_f)( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel,
                                         int             * is_allocated );
/** param[out]     is_allocated          are the lines always newly allocated? */
typedef oyPointer (*oyImage_GetLine_f) ( oyImage_s       * image,
                                         int               line_y,
                                         int             * height,
                                         int               channel,
                                         int             * is_allocated );
/** param[out]     is_allocated          are the tiles always newly allocated? */
typedef oyPointer*(*oyImage_GetTile_f) ( oyImage_s       * image,
                                         int               tile_x,
                                         int               tile_y,
                                         int               channel,
                                         int             * is_allocated );
/**
 *  Typedef   oyImage_SetPoint_f
 *  @memberof oyImage_s
 *  @brief    pixel setter
 *
 *  @param[in,out] image                 the image object
 *  @param[in]     point_x               the data location in x direction
 *  @param[in]     point_y               the data location in y direction
 *  @param[in]     channel               the nth channel will is requested
 *                                       -1 means to request all channels, note
 *                                       that can be an expensive call for non 
 *                                       continous pixel layouts
 *  @param[out]    data                  the data to be copied into the image
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
typedef int       (*oyImage_SetPoint_f)( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel,
                                         oyPointer         data );
typedef int       (*oyImage_SetLine_f) ( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               pixel_n,
                                         int               channel,
                                         oyPointer         data );
typedef int       (*oyImage_SetTile_f) ( oyImage_s       * image,
                                         int               tile_x,
                                         int               tile_y,
                                         int               channel,
                                         oyPointer         data );
