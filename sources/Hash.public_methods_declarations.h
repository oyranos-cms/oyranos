oyHash_s *         oyHash_Get        ( const char        * hash_text,
                                       oyObject_s          object );
oyOBJECT_e         oyHash_GetType    ( oyHash_s          * hash );
int                oyHash_IsOf       ( oyHash_s          * hash,
                                       oyOBJECT_e          type );
int                oyHash_SetPointer ( oyHash_s          * hash,
                                       oyStruct_s        * obj );
oyStruct_s *       oyHash_GetPointer ( oyHash_s          * hash,
                                       oyOBJECT_e          type );
