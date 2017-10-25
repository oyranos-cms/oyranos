OYAPI const char *  OYEXPORT
                oyStruct_GetText        ( oyStruct_s        * obj,
                                          oyNAME_e            name_type,
                                          uint32_t            flags );

OYAPI oyAlloc_f  OYEXPORT
                 oyStruct_GetAllocator   ( oyStruct_s        * obj );
OYAPI oyDeAlloc_f  OYEXPORT
                 oyStruct_GetDeAllocator ( oyStruct_s        * obj );
OYAPI int  OYEXPORT
                 oyStruct_CheckType      ( oyStruct_s        * obj,
                                           oyOBJECT_e          type );
OYAPI int  OYEXPORT
                 oyStruct_GetId          ( oyStruct_s        * obj );
#define oySTRUCT_ALLOC_m(s,len) oyStruct_Allocate((oyStruct_s*)s, len)
OYAPI oyPointer  OYEXPORT
                 oyStruct_Allocate       ( oyStruct_s        * st,
                                           size_t              size );
#define oySTRUCT_FREE_m(s,ptr) oyStruct_DeAllocate((oyStruct_s*)s, ptr)
OYAPI void OYEXPORT  oyStruct_DeAllocate ( oyStruct_s        * st,
                                           oyPointer           ptr );
