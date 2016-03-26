  double           start_xy[2];        /**< the start point of output_image */
  double           start_xy_old[2];    /**< @deprecated the previous start point */
  int32_t        * array_xy;           /**< @deprecated array of shifts, e.g. 1,0,2,0,1,0 */
  int              array_n;            /**< @deprecated the number of points in array_xy */

  int              index;              /**< @deprecated to be advanced by the last caller */
  size_t           pixels_n;           /**< @deprecated pixels to process/cache at once; should be set to 0 or 1 */

  int32_t          workspace_id;       /**< a ID to assign distinct resources to */
  oyStruct_s     * user_data;          /**< user data, e.g. for error messages*/
  oyArray2d_s    * array;              /**< processing data. The position is in
                                            start_xy relative to the previous
                                            mediator in the graph. */
  oyRectangle_s_ * output_array_roi;   /**< rectangle of interesst; The
                                            rectangle is to be seen in relation
                                            to the tickets array::width/channels
                                            .*/
  oyImage_s      * output_image;       /**< the image which issued the request*/
  oyFilterGraph_s_ * graph;            /**< the graph to process */
  oyOptions_s    * request_queue;      /**< messaging; requests to resolve */
