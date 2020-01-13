#include "oyHash_s.h"
OYAPI int  OYEXPORT
                 oyStructList_MoveIn ( oyStructList_s    * list,
                                       oyStruct_s       ** ptr,
                                       int                 pos,
                                       uint32_t            flags );
OYAPI oyStruct_s *  OYEXPORT
                oyStructList_GetRef  ( oyStructList_s    * list,
                                       int                 pos );
OYAPI oyStruct_s *  OYEXPORT
               oyStructList_GetRefType(oyStructList_s    * list,
                                       int                 pos,
                                       oyOBJECT_e          type );
OYAPI int  OYEXPORT
               oyStructList_ReleaseAt( oyStructList_s    * list,
                                       int                 pos );
OYAPI int  OYEXPORT
               oyStructList_Count    ( oyStructList_s    * list );
OYAPI const char *  OYEXPORT
                oyStructList_GetText( oyStructList_s    * list,
                                       oyNAME_e            name_type,
                                       int                 intent_spaces,
                                       uint32_t            flags );
OYAPI const char *  OYEXPORT
                oyStructList_GetID   ( oyStructList_s    * list,
                                       int                 intent_spaces,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
                 oyStructList_Clear  ( oyStructList_s    * list );
OYAPI int  OYEXPORT
                 oyStructList_CopyFrom(oyStructList_s    * list,
                                       oyStructList_s    * from,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
                 oyStructList_MoveTo ( oyStructList_s    * s,
                                       int                 pos,
                                       int                 new_pos );
OYAPI int  OYEXPORT
                 oyStructList_Sort   ( oyStructList_s    * list,
                                       int32_t           * rank_map );
OYAPI int  OYEXPORT
                 oyStructList_ObserverAdd (
                                       oyStructList_s    * list,
                                       oyStruct_s        * observer,
                                       oyStruct_s        * user_data,
                                       oyObserver_Signal_f signalFunc );
OYAPI int  OYEXPORT
                 oyStructList_MoveInName (
                                       oyStructList_s    * texts,
                                       char             ** text,
                                       int                 pos,
                                       oyNAME_e            type );
/** @brief a short identifier for Language and Country, e.g. "de_DE" */
#define oyNAME_LC (oyNAME_DESCRIPTION + 6)
OYAPI int OYEXPORT oyStructList_AddName (
                                       oyStructList_s    * list,
                                       const char        * text,
                                       int                 pos,
                                       oyNAME_e            type );
OYAPI const char *  OYEXPORT oyStructList_GetName (
                                       oyStructList_s    * texts,
                                       int                 pos,
                                       oyNAME_e            type );

OYAPI oyStructList_s *  OYEXPORT
                 oyStructList_Create ( oyOBJECT_e          parent_type,
                                       const char        * list_name,
                                       oyObject_s          object );
OYAPI oyStruct_s *  OYEXPORT
                 oyStructList_GetType( oyStructList_s    * list,
                                       int                 pos,
                                       oyOBJECT_e          type );
OYAPI oyOBJECT_e  OYEXPORT
                 oyStructList_GetParentObjType (
                                       oyStructList_s    * list );

oyHash_s *       oyStructList_GetHash( oyStructList_s    * cache_list,
                                       uint32_t            flags,
                                       const char        * hash_text );
oyStruct_s *     oyStructList_GetHashStruct (
                                       oyStructList_s    * cache_list,
                                       uint32_t            flags,
                                       const char        * hash_text,
                                       oyOBJECT_e          type );
