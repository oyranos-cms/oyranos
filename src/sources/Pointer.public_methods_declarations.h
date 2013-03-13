OYAPI int  OYEXPORT
                 oyPointer_Set           ( oyPointer_s       * cmm_ptr,
                                           const char        * lib_name,
                                           const char        * resource,
                                           oyPointer           ptr,
                                           const char        * func_name,
                                           oyPointer_release_f ptrRelease );
OYAPI const char *  OYEXPORT
                oyPointer_GetFuncName    ( oyPointer_s       * cmm_ptr );
OYAPI const char *  OYEXPORT
                oyPointer_GetLibName     ( oyPointer_s       * cmm_ptr );
OYAPI const char *  OYEXPORT
                oyPointer_GetResourceName(oyPointer_s       * cmm_ptr );
OYAPI int  OYEXPORT
                 oyPointer_GetSize       ( oyPointer_s       * cmm_ptr );
OYAPI int  OYEXPORT
                 oyPointer_SetSize       ( oyPointer_s       * cmm_ptr,
                                           int                 size );
OYAPI oyPointer  OYEXPORT
                 oyPointer_GetPointer    ( oyPointer_s       * cmm_ptr );
OYAPI oyPointer_s *  OYEXPORT
                oyPointer_LookUpFromObject (
                                          oyStruct_s        * data,
                                          const char        * data_type );
OYAPI oyPointer_s *  OYEXPORT
                oyPointer_LookUpFromText( const char        * text,
                                          const char        * data_type );
OYAPI const char *  OYEXPORT
                oyPointer_GetId         ( oyPointer_s       * cmm_ptr );
OYAPI void OYEXPORT
                oyPointer_SetId         ( oyPointer_s       * cmm_ptr,
                                          const char        * id_string );
