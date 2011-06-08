OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_Create   ( oyProfiles_s      * patterns,
                                       oyObject_s          object);
OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_ForStd   ( oyPROFILE_e         std_profile_class,
                                       int               * current,
                                       oyObject_s          object);
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
