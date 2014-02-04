oyObject_s   oyObject_SetAllocators_  ( oyObject_s        object,
                                        oyAlloc_f         allocateFunc,
                                        oyDeAlloc_f       deallocateFunc );
int          oyObject_Ref            ( oyObject_s          obj );
int32_t      oyObject_Hashed_        ( oyObject_s          s );
int          oyObject_HashSet          ( oyObject_s        s,
                                         const unsigned char * hash );
int          oyObject_HashEqual        ( oyObject_s        s1,
                                         oyObject_s        s2 );
int          oyGetNewObjectID        ( );
