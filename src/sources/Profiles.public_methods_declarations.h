OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_Create   ( oyProfiles_s      * patterns,
                                       uint32_t            flags,
                                       oyObject_s          object);
OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_ForStd   ( oyPROFILE_e         std_profile_class,
                                       uint32_t            flags,
                                       int               * current,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyProfiles_DeviceRank(oyProfiles_s      * list,
                                       oyConfig_s        * device,
                                       int32_t           * rank_list );
OYAPI int OYEXPORT oyProfiles_Rank   ( oyProfiles_s      * list,
                                       oyConfig_s        * device,
                                       char                path_separator,
                                       char                key_separator,
                                       int                 flags,
                                       int32_t           * rank_list );
