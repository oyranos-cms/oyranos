int            oyOption_Match_       ( oyOption_s_       * option_a,
                                       oyOption_s_       * option_b );
void         oyOption_UpdateFlags_   ( oyOption_s_       * o );
int            oyOption_GetId_       ( oyOption_s_       * obj );
int            oyOption_SetFromString_(oyOption_s_       * obj,
                                       const char        * text,
                                       uint32_t            flags );
char *         oyOption_GetValueText_( oyOption_s_       * obj,
                                       oyAlloc_f           allocateFunc );
int            oyOption_SetFromInt_  ( oyOption_s_       * obj,
                                       int32_t             integer,
                                       int                 pos,
                                       uint32_t            flags );
int            oyOption_MoveInStruct_( oyOption_s_       * option,
                                       oyStruct_s       ** s );
