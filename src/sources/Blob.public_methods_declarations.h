OYAPI int  OYEXPORT
                 oyBlob_SetFromData  ( oyBlob_s          * obj,
                                       oyPointer           ptr,
                                       size_t              size,
                                       const char        * type );
OYAPI int  OYEXPORT
                 oyBlob_SetFromStatic( oyBlob_s          * blob,
                                       const oyPointer     ptr,
                                       size_t              size,
                                       const char        * type );
OYAPI oyPointer  OYEXPORT
                 oyBlob_GetPointer ( oyBlob_s          * blob );
OYAPI size_t  OYEXPORT
                 oyBlob_GetSize    ( oyBlob_s          * blob );
OYAPI const char *  OYEXPORT
                 oyBlob_GetType    ( oyBlob_s          * blob );
