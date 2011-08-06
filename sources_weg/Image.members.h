  oyRectangle_s      * viewport;       /**< intented viewing area, normalised to the pixel width == 1.0 */
  double               resolution_x;   /**< resolution in horizontal direction*/
  double               resolution_y;   /**< resolution in vertical direction */

  oyPixel_t          * layout_;        /**< @private samples mask;
                                            the oyPixel_t pixel_layout variable
                                            passed during oyImage_Create is
                                            stored in position 0 */
  oyCHANNELTYPE_e    * channel_layout; /**< non profile described channels */
  int                  width;          /*!< data width */
  int                  height;         /*!< data height */
  oyOptions_s        * tags;           /**< display_rectangle, display_name ... */
  oyProfile_s        * profile_;       /*!< @private image profile */

  oyStruct_s         * pixel_data;     /**< struct used by each subsequent call of g/set* pixel acessors */
  oyImage_GetPoint_f   getPoint;       /**< the point interface */
  oyImage_GetLine_f    getLine;        /**< the line interface */
  oyImage_GetTile_f    getTile;        /**< the tile interface */
  oyImage_SetPoint_f   setPoint;       /**< the point interface */
  oyImage_SetLine_f    setLine;        /**< the line interface */
  oyImage_SetTile_f    setTile;        /**< the tile interface */
  int                  tile_width;     /**< needed by the tile interface */
  int                  tile_height;    /**< needed by the tile interface */
  uint16_t             subsampling[2]; /**< 1, 2 or 4 */
  int                  sub_positioning;/**< 0 None, 1 Postscript, 2 CCIR 601-1*/
  oyStruct_s         * user_data;      /**< user provided pointer */
