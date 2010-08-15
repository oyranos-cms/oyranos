oyProfile_s_* oyProfile_FromMemMove_ ( size_t              size,
                                       oyPointer         * block,
                                       int                 flags,
                                       oyObject_s          object);
oyProfile_s_ *  oyProfile_FromFile_  ( const char        * name,
                                       uint32_t            flags,
                                       oyObject_s          object );
oyPointer    oyProfile_TagsToMem_    ( oyProfile_s_      * profile,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
int          oyProfile_ToFile_       ( oyProfile_s_      * profile,
                                       const char        * file_name );
int32_t      oyProfile_Match_        ( oyProfile_s_      * pattern,
                                       oyProfile_s_      * profile );
int32_t      oyProfile_Hashed_       ( oyProfile_s_      * s );
char *       oyProfile_GetFileName_r ( oyProfile_s_      * profile,
                                       oyAlloc_f           allocateFunc );
oyProfileTag_s * oyProfile_GetTagByPos_( oyProfile_s_    * profile,
                                       int                 pos );
int                oyProfile_GetTagCount_ (
                                       oyProfile_s       * profile );
int          oyProfile_TagMoveIn_    ( oyProfile_s       * profile,
                                       oyProfileTag_s   ** obj,
                                       int                 pos );
int          oyProfile_TagReleaseAt_ ( oyProfile_s       * profile,
                                       int                 pos );
int                oyProfile_DeviceAdd(oyProfile_s       * profile,
                                       oyConfig_s        * config );
oyPointer    oyProfile_WriteHeader_  ( oyProfile_s       * profile,
                                       size_t            * size );
oyPointer    oyProfile_WriteTags_    ( oyProfile_s       * profile,
                                       size_t            * size,
                                       oyPointer           icc_header,
                                       oyPointer           icc_list,
                                       oyAlloc_f           allocateFunc );
oyPointer    oyProfile_WriteTagTable_( oyProfile_s       * profile,
                                       size_t            * size );
