oyObject_s*   oyObject_SetParent       ( oyObject_s*        object,
                                        oyOBJECT_e        type,
                                        oyPointer         ptr );
/*oyPointer    oyObjectAlign            ( oyObject_s*        oy,
                                        size_t          * size,
                                        oyAlloc_f         allocateFunc );*/

int          oyObject_SetNames        ( oyObject_s*        object,
                                        const char      * nick,
                                        const char      * name,
                                        const char      * description );
int          oyObject_SetName         ( oyObject_s*        object,
                                        const char      * name,
                                        oyNAME_e          type );
const char  *oyObject_GetName         ( const oyObject_s*  object,
                                        oyNAME_e          type );
/*oyCMMptr_s * oyObject_GetCMMPtr       ( oyObject_s*        object,
                                        const char      * cmm );
oyObject_s*   oyObject_SetCMMPtr       ( oyObject_s*        object,
                                        oyCMMptr_s      * cmm_ptr );*/
int          oyObject_Lock             ( oyObject_s*        object,
                                         const char      * marker,
                                         int               line );
int          oyObject_UnLock           ( oyObject_s*        object,
                                         const char      * marker,
                                         int               line );
int          oyObject_UnSetLocking     ( oyObject_s*        object,
                                         const char      * marker,
                                         int               line );
int          oyObject_GetId            ( oyObject_s*        object );
int          oyObject_GetRefCount      ( oyObject_s*        object );
