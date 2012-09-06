OYAPI double  OYEXPORT
                 oyRectangle_CountPoints(oyRectangle_s     * rectangle );
OYAPI int  OYEXPORT
                 oyRectangle_Index   ( oyRectangle_s     * rectangle,
                                       double              x,
                                       double              y );
OYAPI int  OYEXPORT
                 oyRectangle_IsEqual ( oyRectangle_s     * rectangle1,
                                       oyRectangle_s     * rectangle2 );
OYAPI int  OYEXPORT
                 oyRectangle_IsInside( oyRectangle_s     * test,
                                       oyRectangle_s     * ref );
OYAPI void  OYEXPORT
                 oyRectangle_MoveInside (
                                       oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref );
OYAPI oyRectangle_s *  OYEXPORT
                oyRectangle_NewFrom  ( oyRectangle_s     * ref,
                                       oyObject_s          object );
OYAPI oyRectangle_s *  OYEXPORT
                oyRectangle_NewWith  ( double              x,
                                       double              y,
                                       double              width,
                                       double              height,
                                       oyObject_s          object );
OYAPI void  OYEXPORT
                 oyRectangle_Normalise(oyRectangle_s     * edit_rectangle );
OYAPI int  OYEXPORT
                 oyRectangle_PointIsInside( oyRectangle_s  * rectangle,
                                       double              x,
                                       double              y );
OYAPI void  OYEXPORT
                 oyRectangle_Round   ( oyRectangle_s     * edit_rectangle );
OYAPI int  OYEXPORT
                 oyRectangle_SamplesFromImage (
                                       oyImage_s         * image,
                                       oyRectangle_s     * image_rectangle,
                                       oyRectangle_s     * pixel_rectangle );
OYAPI void  OYEXPORT
                 oyRectangle_Scale   ( oyRectangle_s     * edit_rectangle,
                                       double              factor );
OYAPI void  OYEXPORT
                 oyRectangle_SetByRectangle (
                                       oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref );
OYAPI void  OYEXPORT
                 oyRectangle_SetGeo  ( oyRectangle_s     * edit_rectangle,
                                       double              x,
                                       double              y,
                                       double              width,
                                       double              height );
OYAPI void  OYEXPORT
                 oyRectangle_GetGeo  ( oyRectangle_s     * rectangle,
                                       double            * x,
                                       double            * y,
                                       double            * width,
                                       double            * height );
OYAPI double  OYEXPORT
                 oyRectangle_GetGeo1 ( oyRectangle_s     * rectangle,
                                       int                 x_y_w_h );
OYAPI const char *  OYEXPORT
                oyRectangle_Show     ( oyRectangle_s     * rectangle );
OYAPI void  OYEXPORT
                 oyRectangle_Trim    ( oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref );
