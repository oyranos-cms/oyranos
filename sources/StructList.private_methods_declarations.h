/*oyStruct_s **    oyStructList_GetRaw_( oyStructList_s    * list );*/
oyStruct_s *     oyStructList_Get_   ( oyStructList_s    * list,
                                       int                 pos );
oyStruct_s *     oyStructList_GetType_(oyStructList_s    * list,
                                       int                 pos,
                                       oyOBJECT_e          type );
int              oyStructList_ReferenceAt_(oyStructList_s* list,
                                       int                 pos );

