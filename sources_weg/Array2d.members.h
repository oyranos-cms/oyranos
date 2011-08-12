  oyDATATYPE_e         t;              /**< data type */
  int                  width;          /**< width of actual data view */
  int                  height;         /**< height of actual data view */
  oyRectangle_s        data_area;      /**< size of reserve pixels, x,y <= 0, width,height >= data view width,height */

  unsigned char     ** array2d;        /**< sorted data, pointer is always owned
                                            by the object */
  int                  own_lines;      /**< Are *array2d rows owned by object?
                                            - 0 not owned by the object
                                            - 1 one own monolithic memory block
                                                starting in array2d[0]
                                            - 2 several owned memory blocks */
  oyStructList_s     * refs_;          /**< references of other arrays to this*/
  oyArray2d_s        * refered_;       /**< array this one refers to */
