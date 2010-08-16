oyHash_s_ *   oyHash_CopyRef_         ( oyHash_s_         * entry,
                                        oyObject_s          object );
oyHash_s *         oyHash_Get_       ( const char        * hash_text,
                                       oyObject_s          object );
int                oyHash_IsOf_      ( oyHash_s          * hash,
                                       oyOBJECT_e          type );
int                oyHash_SetPointer_( oyHash_s          * hash,
                                       oyStruct_s        * obj );
oyStruct_s *       oyHash_GetPointer_( oyHash_s          * hash,
                                       oyOBJECT_e          type );
