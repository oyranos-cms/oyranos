
/** param[out]     is_allocated          are the points always newly allocated*/
typedef oyPointer (*oyImage_GetPoint_f)( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel,
                                         int             * is_allocated );
/** param[out]     is_allocated          are the lines always newly allocated */
typedef oyPointer (*oyImage_GetLine_f) ( oyImage_s       * image,
                                         int               line_y,
                                         int             * height,
                                         int               channel,
                                         int             * is_allocated );
/** param[out]     is_allocated          are the tiles always newly allocated */
typedef oyPointer*(*oyImage_GetTile_f) ( oyImage_s       * image,
                                         int               tile_x,
                                         int               tile_y,
                                         int               channel,
                                         int             * is_allocated );
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
