oyPointer_s * oyPointerLookUpFromText( const char        * text,
                                       const char        * data_type );
oyPointer_s * oyPointerLookUpFromObject(oyStruct_s       * data,
                                       const char        * cmm );
int          oyPointer_Set           ( oyPointer_s       * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       oyPointer_release_f ptrRelease );
const char * oyPointer_GetFuncName   ( oyPointer_s       * cmm_ptr );
const char * oyPointer_GetLibName    ( oyPointer_s       * cmm_ptr );
const char * oyPointer_GetResourceName(oyPointer_s       * cmm_ptr );
int          oyPointer_GetSize       ( oyPointer_s       * cmm_ptr );
int          oyPointer_SetSize       ( oyPointer_s       * cmm_ptr,
                                       int                 size );
oyPointer    oyPointer_GetPointer    ( oyPointer_s       * cmm_ptr );
oyPointer_s  * oyPointerLookUpFromObject (
                                       oyStruct_s        * data,
                                       const char        * data_type );
oyPointer_s * oyPointerLookUpFromText( const char        * text,
                                       const char        * data_type );
