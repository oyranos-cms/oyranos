OYAPI oyProfileTag_s * OYEXPORT
               oyProfileTag_Create   ( oyStructList_s    * list,
                                       icTagSignature      tag_use,
                                       icTagTypeSignature  tag_type,
                                       uint32_t            version,
                                       oyObject_s          object );
OYAPI oyProfileTag_s * OYEXPORT
               oyProfileTag_CreateFromText (
                                       const char        * text,
                                       icTagTypeSignature  tag_type,
                                       icTagSignature      tag_usage,
                                       oyObject_s          object );
OYAPI oyProfileTag_s * OYEXPORT
               oyProfileTag_CreateFromData ( 
                                       icTagSignature      sig,
                                       icTagTypeSignature  type,
                                       oySTATUS_e          status,
                                       size_t              tag_size,
                                       oyPointer           tag_block,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
               oyProfileTag_Set      ( oyProfileTag_s    * tag,
                                       icTagSignature      sig,
                                       icTagTypeSignature  type,
                                       oySTATUS_e          status,
                                       size_t              tag_size,
                                       oyPointer         * tag_block );
OYAPI oyStructList_s  * OYEXPORT
                oyProfileTag_Get     ( oyProfileTag_s    * tag );
OYAPI char **  OYEXPORT
                oyProfileTag_GetText ( oyProfileTag_s    * tag,
                                       int32_t           * n,
                                       const char        * language,
                                       const char        * country,
                                       int32_t           * tag_size,
                                       oyAlloc_f           allocateFunc );
OYAPI int  OYEXPORT
                 oyProfileTag_GetBlock(oyProfileTag_s    * tag,
                                       oyPointer         * tag_block,
                                       size_t            * tag_size,
                                       oyAlloc_f           allocateFunc );
OYAPI size_t  OYEXPORT
               oyProfileTag_GetOffset( oyProfileTag_s    * tag );
OYAPI int  OYEXPORT
               oyProfileTag_SetOffset( oyProfileTag_s    * tag,
                                       size_t              offset );
OYAPI oySTATUS_e  OYEXPORT
               oyProfileTag_GetStatus( oyProfileTag_s    * tag );
OYAPI int  OYEXPORT
               oyProfileTag_SetStatus( oyProfileTag_s    * tag,
                                       oySTATUS_e          status );
OYAPI const char *  OYEXPORT
               oyProfileTag_GetCMM   ( oyProfileTag_s    * tag );
OYAPI int  OYEXPORT
               oyProfileTag_SetCMM   ( oyProfileTag_s    * tag,
                                       const char        * cmm );
OYAPI const char *  OYEXPORT
               oyProfileTag_GetLastCMM(oyProfileTag_s    * tag );
OYAPI int  OYEXPORT
               oyProfileTag_SetLastCMM(oyProfileTag_s    * tag,
                                       const char        * cmm );
OYAPI const char *  OYEXPORT
               oyProfileTag_GetRequiredCMM (
                                       oyProfileTag_s    * tag );
OYAPI int  OYEXPORT
               oyProfileTag_SetRequiredCMM (
                                       oyProfileTag_s    * tag,
                                       const char        * cmm );
OYAPI icTagSignature  OYEXPORT
               oyProfileTag_GetUse   ( oyProfileTag_s    * tag );
OYAPI icTagTypeSignature  OYEXPORT
               oyProfileTag_GetType  ( oyProfileTag_s    * tag );
OYAPI size_t  OYEXPORT
               oyProfileTag_GetSize  ( oyProfileTag_s    * tag );
OYAPI size_t  OYEXPORT
               oyProfileTag_GetSizeCheck (
                                       oyProfileTag_s    * tag );
OYAPI void  OYEXPORT
               oyProfileTag_SetSizeCheck (
                                       oyProfileTag_s    * tag,
                                       size_t              size_check );
