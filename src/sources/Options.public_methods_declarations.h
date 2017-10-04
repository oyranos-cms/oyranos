OYAPI oyOptions_s *  OYEXPORT
                oyOptions_FromBoolean( oyOptions_s       * pattern,
                                       oyOptions_s       * options,
                                       oyBOOLEAN_e         type,
                                       oyObject_s          object );
#ifdef OY_FILTER_CORE_S_H
#endif /* OY_FILTER_CORE_S_H */
OYAPI oyOptions_s *  OYEXPORT
                oyOptions_FromText   ( const char        * text,
                                       uint32_t            flags,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
                oyOptions_FromJSON   ( const char        * json_text,
                                       oyOptions_s       * options,
                                       oyOptions_s      ** result,
                                       const char        * xformat,
                                       ... );

OYAPI int  OYEXPORT
                 oyOptions_Add       ( oyOptions_s       * options,
                                       oyOption_s        * option,
                                       int                 pos,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
                 oyOptions_Set       ( oyOptions_s       * options,
                                       oyOption_s        * option,
                                       int                 pos,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
                 oyOptions_SetOpts   ( oyOptions_s       * list,
                                       oyOptions_s       * add );
OYAPI int  OYEXPORT
                 oyOptions_AppendOpts( oyOptions_s       * list,
                                       oyOptions_s       * append );
OYAPI int  OYEXPORT
                 oyOptions_CopyFrom  ( oyOptions_s      ** list,
                                       oyOptions_s       * from,
                                       oyBOOLEAN_e         type,
                                       oyFILTER_REG_e      fields,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
                 oyOptions_DoFilter  ( oyOptions_s       * s,
                                       uint32_t            flags,
                                       const char        * filter_type );
OYAPI int  OYEXPORT
                 oyOptions_Filter    ( oyOptions_s      ** add_list,
                                       int32_t           * count,
                                       uint32_t            flags,
                                       oyBOOLEAN_e         type,
                                       const char        * registration,
                                       oyOptions_s       * src_list );
OYAPI const char *  OYEXPORT
                oyOptions_GetText    ( oyOptions_s       * options,
                                       oyNAME_e            type );
OYAPI int  OYEXPORT
                 oyOptions_CountType ( oyOptions_s       * options,
                                       const char        * registration,
                                       oyOBJECT_e          type );
OYAPI oyOption_s *  OYEXPORT
                oyOptions_Find       ( oyOptions_s       * options,
                                       const char        * key,
                                       uint32_t            type );
OYAPI const char *  OYEXPORT
                oyOptions_FindString ( oyOptions_s       * options,
                                       const char        * key,
                                       const char        * value );
OYAPI int  OYEXPORT
                 oyOptions_SetFromText(oyOptions_s      ** obj,
                                       const char        * key,
                                       const char        * value,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
                 oyOptions_FindInt   ( oyOptions_s       * options,
                                       const char        * key,
                                       int                 pos,
                                       int32_t           * result );
OYAPI int  OYEXPORT
                 oyOptions_SetFromInt( oyOptions_s      ** obj,
                                       const char        * key,
                                       int32_t             integer,
                                       int                 pos,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
                 oyOptions_FindDouble( oyOptions_s       * options,
                                       const char        * key,
                                       int                 pos,
                                       double            * result );
OYAPI int  OYEXPORT
                 oyOptions_SetFromDouble (
                                       oyOptions_s      ** obj,
                                       const char        * key,
                                       double              value,
                                       int                 pos,
                                       uint32_t            flags );
OYAPI oyStruct_s *  OYEXPORT
                 oyOptions_GetType   ( oyOptions_s       * options,
                                       int                 pos,
                                       const char        * registration,
                                       oyOBJECT_e          type );
OYAPI int  OYEXPORT
                 oyOptions_GetType2  ( oyOptions_s       * options,
                                       int                 pos,
                                       const char        * pattern,
                                       uint32_t            pattern_type,
                                       oyOBJECT_e          object_type,
                                       oyStruct_s       ** result,
                                       oyOption_s       ** option );
OYAPI int  OYEXPORT
                 oyOptions_MoveInStruct(
                                       oyOptions_s      ** obj,
                                       const char        * registration,
                                       oyStruct_s       ** oy_struct,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
                 oyOptions_SetFromData(oyOptions_s      ** options,
                                       const char        * registration,
                                       oyPointer           ptr,
                                       size_t              size,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
                 oyOptions_FindData  ( oyOptions_s       * options,
                                       const char        * registration,
                                       oyPointer         * result,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
OYAPI int  OYEXPORT
                 oyOptions_SetSource ( oyOptions_s       * options,
                                       oyOPTIONSOURCE_e    source );
OYAPI int  OYEXPORT
               oyOptions_ObserverAdd ( oyOptions_s       * object,
                                       oyStruct_s        * observer,
                                       oyStruct_s        * user_data,
                                       oyObserver_Signal_f signalFunc );
OYAPI int  OYEXPORT
               oyOptions_ObserversDisable (
                                       oyOptions_s       * object );
OYAPI int  OYEXPORT
               oyOptions_ObserversEnable (
                                       oyOptions_s       * object );
/* needs modules from oyranos_modules */
OYAPI int  OYEXPORT
                 oyOptions_Handle    ( const char        * registration,
                                       oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result );
/* needs modules from oyranos_modules */
OYAPI oyOptions_s *  OYEXPORT
                oyOptions_ForFilter  ( const char        * registration,
                                       uint32_t            flags,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
           oyOptions_SetDriverContext( oyOptions_s      ** options,
                                       oyPointer           driver_context,
                                       const char        * driver_context_type,
                                       size_t              driver_context_size,
                                       oyObject_s          object );
/* needs DB from oyranos_config */
OYAPI int  OYEXPORT oyOptions_SaveToDB(oyOptions_s       * options,
                                       oySCOPE_e           scope,
                                       const char        * registration,
                                       char             ** new_reg,
                                       oyAlloc_f           allocateFunc );
