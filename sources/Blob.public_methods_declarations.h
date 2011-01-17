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
oyPointer          oyBlob_GetPointer ( oyBlob_s          * blob );
size_t             oyBlob_GetSize    ( oyBlob_s          * blob );
const char *       oyBlob_GetType    ( oyBlob_s          * blob );
