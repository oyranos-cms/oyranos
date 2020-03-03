oyObject_s   oyObject_SetAllocators_  ( oyObject_s        object,
                                        oyAlloc_f         allocateFunc,
                                        oyDeAlloc_f       deallocateFunc );
int          oyObject_Ref            ( oyObject_s          obj );
int32_t      oyObject_Hashed_        ( oyObject_s          s );
int          oyObject_HashSet          ( oyObject_s        s,
                                         const unsigned char * hash );
int          oyObject_HashEqual        ( oyObject_s        s1,
                                         oyObject_s        s2 );
int          oyGetNewObjectID          ( );
void         oyObjectIdRelease         ( );
/* object tracking */
int *              oyObjectGetCurrentObjectIdList( void );
int                oyObjectCountCurrentObjectIdList( void );
int *              oyObjectFindNewIds( int               * old_ids,
                                       int               * new_ids );
void               oyObjectReleaseCurrentObjectIdList(
                                       int              ** id_list );
const oyObject_s * oyObjectGetList   ( int               * max_count );
int                oyObjectIdListShowDiffAndRelease (
                                       int              ** ids_old,
                                       const char        * location );

