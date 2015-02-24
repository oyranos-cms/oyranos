OYAPI int  OYEXPORT
               oyConfigs_FromDomain  ( const char        * registration_domain,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** configs,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
               oyConfigs_FromDeviceClass (
                                       const char        * device_type,
                                       const char        * device_class,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** configs,
                                       oyObject_s          object );
OYAPI int OYEXPORT oyConfigs_FromDB  ( const char        * registration,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** configs,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
                 oyConfigs_Modify    ( oyConfigs_s       * configs,
                                       oyOptions_s       * options );
OYAPI int  OYEXPORT
                 oyConfigs_SelectSimilars (
                                       oyConfigs_s       * list,
                                       const char        * pattern[][2],
                                       oyConfigs_s      ** filtered );
OYAPI int  OYEXPORT
                 oyConfigDomainList  ( const char        * registration_pattern,
                                       char            *** list,
                                       uint32_t          * count,
                                       uint32_t         ** rank_list,
                                       oyAlloc_f           allocateFunc );
OYAPI int  OYEXPORT
               oyConfig_GetBestMatchFromTaxiDB(
                                       oyConfig_s        * device,
                                       int32_t           * rank_value );
