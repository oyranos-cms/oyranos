typedef struct oyName_s* oyName;

//Public function declarations:
int          oyName_releaseMembers   ( oyName_s          * obj,
                                           oyDeAlloc_f         deallocateFunc );
oyName_s *   oyName_new              ( oyObject_s          object );

oyName_s *   oyName_copy             ( oyName_s          * obj,
                                       oyObject_s          object );
int          oyName_release          ( oyName_s         ** obj );
