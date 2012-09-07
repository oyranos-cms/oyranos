OYAPI oyArray2d_s * OYEXPORT
                   oyArray2d_Create  ( oyPointer           data,
                                       int                 width,
                                       int                 height,
                                       oyDATATYPE_e        type,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
                 oyArray2d_DataSet   ( oyArray2d_s       * obj,
                                       oyPointer           data );
OYAPI int  OYEXPORT
               oyArray2d_ReleaseArray( oyArray2d_s       * obj );
OYAPI int  OYEXPORT
                 oyArray2d_SetRows   ( oyArray2d_s       * obj,
                                       oyPointer         * rows,
                                       int                 do_copy );
OYAPI int  OYEXPORT  oyArray2d_SetFocus (
                                       oyArray2d_s       * array,
                                       oyRectangle_s     * rectangle );
