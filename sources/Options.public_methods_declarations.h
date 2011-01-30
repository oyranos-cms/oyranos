oyOptions_s *  oyOptions_FromBoolean ( oyOptions_s       * pattern,
                                       oyOptions_s       * options,
                                       oyBOOLEAN_e         type,
                                       oyObject_s          object );
#ifdef OY_FILTER_CORE_S_H
oyOptions_s *  oyOptions_ForFilter   ( const char        * registration,
                                       const char        * cmm,
                                       uint32_t            flags,
                                       oyObject_s          object );
#endif /* OY_FILTER_CORE_S_H */
oyOptions_s *  oyOptions_FromText    ( const char        * text,
                                       uint32_t            flags,
                                       oyObject_s          object );

int            oyOptions_Add         ( oyOptions_s       * options,
                                       oyOption_s        * option,
                                       int                 pos,
                                       oyObject_s          object );
int            oyOptions_Set         ( oyOptions_s       * options,
                                       oyOption_s        * option,
                                       int                 pos,
                                       oyObject_s          object );
int            oyOptions_SetOpts     ( oyOptions_s       * list,
                                       oyOptions_s       * add );
int            oyOptions_AppendOpts  ( oyOptions_s       * list,
                                       oyOptions_s       * append );
int            oyOptions_CopyFrom    ( oyOptions_s      ** list,
                                       oyOptions_s       * from,
                                       oyBOOLEAN_e         type,
                                       oyFILTER_REG_e      fields,
                                       oyObject_s          object );
int            oyOptions_DoFilter    ( oyOptions_s       * s,
                                       uint32_t            flags,
                                       const char        * filter_type );
int            oyOptions_Filter      ( oyOptions_s      ** add_list,
                                       int32_t           * count,
                                       uint32_t            flags,
                                       oyBOOLEAN_e         type,
                                       const char        * registration,
                                       oyOptions_s       * src_list );
const char *   oyOptions_GetText     ( oyOptions_s       * options,
                                       oyNAME_e            type );
int            oyOptions_CountType   ( oyOptions_s       * options,
                                       const char        * registration,
                                       oyOBJECT_e          type );
oyOption_s *   oyOptions_Find        ( oyOptions_s       * options,
                                       const char        * key );
const char *   oyOptions_FindString  ( oyOptions_s       * options,
                                       const char        * key,
                                       const char        * value );
int            oyOptions_SetFromText ( oyOptions_s      ** obj,
                                       const char        * key,
                                       const char        * value,
                                       uint32_t            flags );
int            oyOptions_FindInt     ( oyOptions_s       * options,
                                       const char        * key,
                                       int                 pos,
                                       int32_t           * result );
int            oyOptions_SetFromInt  ( oyOptions_s      ** obj,
                                       const char        * key,
                                       int32_t             integer,
                                       int                 pos,
                                       uint32_t            flags );
int            oyOptions_FindDouble  ( oyOptions_s       * options,
                                       const char        * key,
                                       int                 pos,
                                       double            * result );
int            oyOptions_SetFromDouble(oyOptions_s      ** obj,
                                       const char        * key,
                                       double              value,
                                       int                 pos,
                                       uint32_t            flags );
oyStruct_s *   oyOptions_GetType     ( oyOptions_s       * options,
                                       int                 pos,
                                       const char        * registration,
                                       oyOBJECT_e          type );
int            oyOptions_MoveInStruct( oyOptions_s      ** obj,
                                       const char        * registration,
                                       oyStruct_s       ** oy_struct,
                                       uint32_t            flags );
int            oyOptions_SetFromData ( oyOptions_s      ** options,
                                       const char        * registration,
                                       oyPointer           ptr,
                                       size_t              size,
                                       uint32_t            flags );
int            oyOptions_FindData    ( oyOptions_s       * options,
                                       const char        * registration,
                                       oyPointer         * result,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
int            oyOptions_SetSource   ( oyOptions_s       * options,
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
int             oyOptions_Handle     ( const char        * registration,
                                       oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result );
int            oyOption_GetFlags     ( oyOptions_s       * object );
int            oyOption_SetFlags     ( oyOptions_s       * object,
                                       uint32_t            flags );
