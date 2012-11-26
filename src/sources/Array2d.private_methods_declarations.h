int                oyArray2d_Init_   ( oyArray2d_s_      * s,
                                       int                 width,
                                       int                 height,
                                       oyDATATYPE_e        data_type );
oyArray2d_s_ *
                   oyArray2d_Create_ ( int                 width,
                                       int                 height,
                                       oyDATATYPE_e        data_type,
                                       oyObject_s          object );
int
             oyArray2d_ReleaseArray_ ( oyArray2d_s       * obj );
int              oyArray2d_ToPPM_    ( oyArray2d_s_      * array,
                                       const char        * file_name );
