OYAPI oyHash_s *  OYEXPORT
                 oyHash_Create     ( const char        * hash_text,
                                     oyObject_s          object );
OYAPI oyOBJECT_e  OYEXPORT
                 oyHash_GetType    ( oyHash_s          * hash );
OYAPI int  OYEXPORT
                 oyHash_IsOf       ( oyHash_s          * hash,
                                     oyOBJECT_e          type );
OYAPI int  OYEXPORT
                 oyHash_SetPointer ( oyHash_s          * hash,
                                     oyStruct_s        * obj );
OYAPI oyStruct_s * OYEXPORT
                 oyHash_GetPointer ( oyHash_s          * hash,
                                     oyOBJECT_e          type );
