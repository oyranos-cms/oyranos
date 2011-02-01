oyCMMptr_s * oyCMMptrLookUpFromText  ( const char        * text,
                                       const char        * data_type );
oyCMMptr_s * oyCMMptrLookUpFromObject( oyStruct_s        * data,
                                       const char        * cmm );
int          oyCMMptr_Set            ( oyCMMptr_s        * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       oyPointer_release_f ptrRelease );
const char * oyCMMptr_GetFuncName    ( oyCMMptr_s        * cmm_ptr );
const char * oyCMMptr_GetLibName     ( oyCMMptr_s        * cmm_ptr );
const char * oyCMMptr_GetResourceName( oyCMMptr_s        * cmm_ptr );
int          oyCMMptr_GetSize        ( oyCMMptr_s        * cmm_ptr );
int          oyCMMptr_SetSize        ( oyCMMptr_s        * cmm_ptr,
                                       int                 size );
oyPointer    oyCMMptr_GetPointer     ( oyCMMptr_s        * cmm_ptr );
