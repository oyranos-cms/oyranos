OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_Create   ( oyProfiles_s      * patterns,
                                       oyObject_s          object);
OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_ForStd   ( oyPROFILE_e         colour_space,
                                       int               * current,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyProfiles_Release  ( oyProfiles_s     ** profile_list );


oyProfiles_s   * oyProfiles_MoveIn   ( oyProfiles_s      * list,
                                       oyProfile_s      ** ptr,
                                       int                 pos );
int              oyProfiles_ReleaseAt( oyProfiles_s      * list,
                                       int                 pos );
oyProfile_s *    oyProfiles_Get      ( oyProfiles_s      * list,
                                       int                 pos );
int              oyProfiles_Count    ( oyProfiles_s      * list );
int              oyProfiles_DeviceRank ( oyProfiles_s    * list,
                                       oyConfig_s        * device,
                                       int32_t           * rank_list );
