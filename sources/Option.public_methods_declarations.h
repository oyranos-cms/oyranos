oyOption_s *   oyOption_New          ( const char        * registration,
                                       oyObject_s          object );
oyOption_s *   oyOption_FromDB       ( const char        * registration,
                                       oyObject_s          object );
oyOption_s *   oyOption_Copy         ( oyOption_s        * option,
                                       oyObject_s          object );
int            oyOption_Release      ( oyOption_s       ** option );
int            oyOption_GetId        ( oyOption_s        * option );
const char *   oyOption_GetText      ( oyOption_s        * obj,
                                       oyNAME_e            type );
int            oyOption_SetFromText  ( oyOption_s        * obj,
                                       const char        * text, 
                                       uint32_t            flags );
char *         oyOption_GetValueText ( oyOption_s        * obj,
                                       oyAlloc_f           allocateFunc );
int            oyOption_SetFromInt   ( oyOption_s        * obj,
                                       int32_t             integer,
                                       int                 pos,
                                       uint32_t            flags );
int32_t        oyOption_GetValueInt  ( oyOption_s        * obj,
                                       int                 pos );
int            oyOption_SetFromDouble( oyOption_s        * obj,
                                       double              floating_point,
                                       int                 pos,
                                       uint32_t            flags );
double         oyOption_GetValueDouble(oyOption_s        * obj,
                                       int                 pos );
int            oyOption_Clear        ( oyOption_s        * s );
int            oyOption_SetFromData  ( oyOption_s        * option,
                                       oyPointer           ptr,
                                       size_t              size );
oyPointer      oyOption_GetData      ( oyOption_s        * option,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
int            oyOption_SetRegistration (
                                       oyOption_s        * option,
                                       const char        * registration );
const char *   oyOption_GetRegistration (
                                       oyOption_s        * option );
int            oyOption_SetValueFromDB(oyOption_s        * option );
int            oyOption_StructMoveIn ( oyOption_s        * option,
                                       oyStruct_s       ** s );
oyStruct_s *   oyOption_StructGet    ( oyOption_s        * option,
                                       oyOBJECT_e          type );
