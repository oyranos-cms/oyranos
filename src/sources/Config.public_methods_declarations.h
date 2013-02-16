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
OYAPI int  OYEXPORT
               oyConfig_DomainRank   ( oyConfig_s        * config );
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
OYAPI oyOptions_s ** OYEXPORT
               oyConfig_GetOptions   ( oyConfig_s        * config,
                                       const char        * source );
OYAPI oyConfig_s * OYEXPORT
               oyConfig_FromRegistration
                                     ( const char        * registration,
                                       oyObject_s          object );
OYAPI const char *  OYEXPORT
               oyConfig_GetRegistration
                                     ( oyConfig_s        * config );
OYAPI int  OYEXPORT
               oyConfig_SetRankMap   ( oyConfig_s        * config,
                                       const oyRankMap   * rank_map );
OYAPI const oyRankMap *  OYEXPORT
               oyConfig_GetRankMap   ( oyConfig_s        * config );
OYAPI oyRankMap * OYEXPORT
                 oyRankMapCopy       ( const oyRankMap   * rank_map,
                                       oyAlloc_f           allocateFunc );
OYAPI void OYEXPORT
                 oyRankMapRelease    ( oyRankMap        ** rank_map,
                                       oyDeAlloc_f         deAllocateFunc );
OYAPI int  OYEXPORT oyRankMapFromJSON( const char        * json_text,
                                       oyOptions_s       * options,
                                       oyRankMap        ** rank_map,
                                       oyAlloc_f           allocateFunc );
OYAPI int OYEXPORT oyRankMapToJSON   ( const oyRankMap   * rank_map,
                                       oyOptions_s       * options,
                                       char             ** json_text,
                                       oyAlloc_f           allocateFunc );
