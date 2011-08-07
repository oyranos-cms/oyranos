double         oyRectangle_CountPoints(oyRectangle_s     * rectangle );
int            oyRectangle_Index     ( oyRectangle_s     * rectangle,
                                       double              x,
                                       double              y );
int            oyRectangle_IsEqual   ( oyRectangle_s     * rectangle1,
                                       oyRectangle_s     * rectangle2 );
int            oyRectangle_IsInside  ( oyRectangle_s     * test,
                                       oyRectangle_s     * ref );
void           oyRectangle_MoveInside( oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref );
oyRectangle_s* oyRectangle_NewFrom   ( oyRectangle_s     * ref,
                                       oyObject_s          object );
oyRectangle_s* oyRectangle_NewWith   ( double              x,
                                       double              y,
                                       double              width,
                                       double              height,
                                       oyObject_s          object );
void           oyRectangle_Normalise ( oyRectangle_s     * edit_rectangle );
int            oyRectangle_PointIsInside( oyRectangle_s  * rectangle,
                                       double              x,
                                       double              y );
void           oyRectangle_Round     ( oyRectangle_s     * edit_rectangle );
int            oyRectangle_SamplesFromImage (
                                       oyImage_s         * image,
                                       oyRectangle_s     * image_rectangle,
                                       oyRectangle_s     * pixel_rectangle );
void           oyRectangle_Scale     ( oyRectangle_s     * edit_rectangle,
                                       double              factor );
void           oyRectangle_SetByRectangle (
                                       oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref );
void           oyRectangle_SetGeo    ( oyRectangle_s     * edit_rectangle,
                                       double              x,
                                       double              y,
                                       double              width,
                                       double              height );
const char *   oyRectangle_Show      ( oyRectangle_s     * rectangle );
void           oyRectangle_Trim      ( oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref );
