OYAPI oyArray2d_s * OYEXPORT
                   oyArray2d_Create  ( oyPointer           data,
                                       int                 width,
                                       int                 height,
                                       oyDATATYPE_e        type,
                                       oyObject_s          object );
OYAPI int OYEXPORT
                 oyArray2d_Reset     ( oyArray2d_s       * array,
                                       int                 width,
                                       int                 height,
                                       oyDATATYPE_e        data_type );
OYAPI oyPointer  OYEXPORT
                 oyArray2d_GetData   ( oyArray2d_s       * obj );
OYAPI int  OYEXPORT
                 oyArray2d_SetData   ( oyArray2d_s       * obj,
                                       oyPointer           data );
OYAPI int  OYEXPORT
                 oyArray2d_SetRows   ( oyArray2d_s       * obj,
                                       oyPointer         * rows,
                                       int                 do_copy );
OYAPI int  OYEXPORT  oyArray2d_SetFocus (
                                       oyArray2d_s       * array,
                                       oyRectangle_s     * rectangle );
OYAPI double  OYEXPORT
                 oyArray2d_GetDataGeo1(oyArray2d_s       * obj,
                                       int                 x_y_w_h );
OYAPI int  OYEXPORT
                 oyArray2d_GetWidth  ( oyArray2d_s       * obj );
OYAPI int  OYEXPORT
                 oyArray2d_GetHeight ( oyArray2d_s       * obj );
OYAPI oyDATATYPE_e  OYEXPORT
                 oyArray2d_GetType   ( oyArray2d_s       * array );
OYAPI const char *  OYEXPORT
                 oyArray2d_Show      ( oyArray2d_s       * array );
