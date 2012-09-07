/** Function  oyConfig_Set
 *  @memberof oyConfig_s
 *  @brief    Unimplemented!
 */
OYAPI int  OYEXPORT
               oyConfig_Set          ( oyConfig_s        * config,
                                       const char        * registration_domain,
                                       oyOptions_s       * options,
                                       oyBlob_s          * data );
OYAPI int  OYEXPORT
               oyConfig_AddDBData    ( oyConfig_s        * config,
                                       const char        * key,
                                       const char        * value,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
               oyConfig_ClearDBData  ( oyConfig_s        * config );
OYAPI int  OYEXPORT
               oyConfig_GetDB        ( oyConfig_s        * device,
                                       int32_t           * rank_value );
OYAPI int  OYEXPORT
               oyConfig_SaveToDB     ( oyConfig_s        * config );
OYAPI int  OYEXPORT
               oyConfig_GetFromDB    ( oyConfig_s        * device,
                                       oyConfigs_s       * configs,
                                       int32_t           * rank_value );
OYAPI int  OYEXPORT
               oyConfig_EraseFromDB  ( oyConfig_s        * config );
OYAPI int  OYEXPORT
               oyConfig_Compare      ( oyConfig_s        * device,
                                       oyConfig_s        * pattern,
                                       int32_t           * rank_value );
#ifdef UNHIDE_CMM
OYAPI int  OYEXPORT
               oyConfig_DomainRank   ( oyConfig_s        * config );
#endif
OYAPI const char * OYEXPORT
               oyConfig_FindString   ( oyConfig_s        * config,
                                       const char        * key,
                                       const char        * value );
OYAPI int  OYEXPORT
               oyConfig_Has          ( oyConfig_s        * config,
                                       const char        * key );
OYAPI oyOption_s * OYEXPORT
               oyConfig_Find         ( oyConfig_s        * config,
                                       const char        * key );
OYAPI int  OYEXPORT
               oyConfig_Count        ( oyConfig_s        * config );
OYAPI oyOption_s * OYEXPORT
               oyConfig_Get          ( oyConfig_s        * config,
                                       int                 pos );
OYAPI oyConfig_s * OYEXPORT
               oyConfig_FromRegistration( const char        * registration,
                                          oyObject_s          object );
OYAPI oyRankPad * OYEXPORT
                 oyRankMapCopy     ( const oyRankPad   * rank_map,
                                       oyAlloc_f           allocateFunc );
