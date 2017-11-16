OYAPI oyOption_s  *    OYEXPORT
                oyOption_FromRegistration (
                                       const char        * registration,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
                 oyOption_GetId      ( oyOption_s        * option );
OYAPI const  char *    OYEXPORT
                oyOption_GetText     ( oyOption_s        * obj,
                                       oyNAME_e            type );
OYAPI int  OYEXPORT
                 oyOption_SetFromString( oyOption_s        * obj,
                                       const char        * text,
                                       uint32_t            flags );
OYAPI char  *  OYEXPORT
                oyOption_GetValueText( oyOption_s        * obj,
                                       oyAlloc_f           allocateFunc );
OYAPI const  char *  OYEXPORT
                oyOption_GetValueString (
                                       oyOption_s        * obj,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyOption_SetFromInt ( oyOption_s        * obj,
                                       int32_t             integer,
                                       int                 pos,
                                       uint32_t            flags );
OYAPI int32_t  OYEXPORT
                 oyOption_GetValueInt( oyOption_s        * obj,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyOption_SetFromDouble (
                                       oyOption_s        * obj,
                                       double              floating_point,
                                       int                 pos,
                                       uint32_t            flags );
OYAPI double  OYEXPORT
                 oyOption_GetValueDouble(oyOption_s        * obj,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyOption_Clear      ( oyOption_s        * s );
OYAPI int  OYEXPORT
                 oyOption_SetFromData( oyOption_s        * option,
                                       oyPointer           ptr,
                                       size_t              size );
OYAPI oyPointer  OYEXPORT
                 oyOption_GetData    ( oyOption_s        * option,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
OYAPI int  OYEXPORT
                 oyOption_SetRegistration (
                                       oyOption_s        * option,
                                       const char        * registration );
OYAPI const  char *  OYEXPORT
                oyOption_GetRegistration (
                                       oyOption_s        * option );
OYAPI int  OYEXPORT
                 oyOption_MoveInStruct(oyOption_s        * option,
                                       oyStruct_s       ** s );
OYAPI oyStruct_s *  OYEXPORT
                oyOption_GetStruct   ( oyOption_s        * option,
                                       oyOBJECT_e          type );
OYAPI int  OYEXPORT
                 oyOption_SetValueFromDB (
                                       oyOption_s        * option );
OYAPI void  OYEXPORT
                 oyOption_SetSource  ( oyOption_s        * option,
                                       oyOPTIONSOURCE_e    source );
OYAPI oyOPTIONSOURCE_e  OYEXPORT
                 oyOption_GetSource  ( oyOption_s        * option );
OYAPI int  OYEXPORT
                 oyOption_GetFlags   ( oyOption_s        * object );
OYAPI int  OYEXPORT
                 oyOption_SetFlags   ( oyOption_s        * object,
                                       uint32_t            flags );
OYAPI oyVALUETYPE_e OYEXPORT
                 oyOption_GetValueType(oyOption_s        * object );

const char *     oyValueTypeText     ( oyVALUETYPE_e       type );

