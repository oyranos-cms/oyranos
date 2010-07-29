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
int          oyCMMptr_ConvertData    ( oyCMMptr_s        * cmm_ptr,
                                       oyCMMptr_s        * cmm_ptr_out,
                                       oyFilterNode_s    * node );
