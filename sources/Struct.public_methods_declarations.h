const char * oyStruct_GetText        ( oyStruct_s        * obj,
                                       oyNAME_e            name_type,
                                       uint32_t            flags );

oyAlloc_f    oyStruct_GetAllocator   ( oyStruct_s        * obj );
oyDeAlloc_f  oyStruct_GetDeAllocator ( oyStruct_s        * obj );
int          oyStruct_CheckType      ( oyStruct_s        * obj,
                                       oyOBJECT_e          type );
