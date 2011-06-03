int              oyStructList_MoveIn ( oyStructList_s    * list,
                                       oyStruct_s       ** ptr,
                                       int                 pos,
                                       uint32_t            flags );
oyStruct_s *     oyStructList_GetRef ( oyStructList_s    * list,
                                       int                 pos );
oyStruct_s *     oyStructList_GetRefType( oyStructList_s * list,
                                          int              pos,
                                          oyOBJECT_e       type );
int              oyStructList_ReleaseAt( oyStructList_s  * list,
                                       int                 pos );
int              oyStructList_Count  ( oyStructList_s    * list );
const char *     oyStructList_GetText( oyStructList_s    * s,
                                       oyNAME_e            name_type,
                                       int                 intent_spaces,
                                       uint32_t            flags );
const char *     oyStructList_GetID  ( oyStructList_s    * s,
                                       int                 intent_spaces,
                                       uint32_t            flags );
int              oyStructList_Clear  ( oyStructList_s    * s );
int              oyStructList_CopyFrom(oyStructList_s    * list,
                                       oyStructList_s    * from,
                                       oyObject_s          object );
int              oyStructList_MoveTo ( oyStructList_s    * s,
                                       int                 pos,
                                       int                 new_pos );
int              oyStructList_Sort   ( oyStructList_s    * s,
                                       int32_t           * rank_map );
int              oyStructList_ObserverAdd (
                                       oyStructList_s    * list,
                                       oyStruct_s        * observer,
                                       oyStruct_s        * user_data,
                                       oyObserver_Signal_f signalFunc );
int              oyStructList_MoveInName (
                                       oyStructList_s    * texts,
                                       char             ** text,
                                       int                 pos );
int              oyStructList_AddName( oyStructList_s    * texts,
                                       const char        * text,
                                       int pos );
const char *     oyStructList_GetName( oyStructList_s    * texts,
                                       int pos );

oyStructList_s * oyStructList_Create ( oyOBJECT_e          parent_type,
                                       const char        * list_name,
                                       oyObject_s          object );
oyStruct_s *     oyStructList_GetType( oyStructList_s    * list,
                                       int                 pos,
                                       oyOBJECT_e          type );
oyOBJECT_e       oyStructList_GetParentObjType (
                                       oyStructList_s    * list );
