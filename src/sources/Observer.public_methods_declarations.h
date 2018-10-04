OYAPI int  OYEXPORT
           oyObserver_SignalSend     ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * user_data );
OYAPI int  OYEXPORT
           oyStruct_ObserverAdd      ( oyStruct_s        * model,
                                       oyStruct_s        * observer,
                                       oyStruct_s        * user_data,
                                       oyObserver_Signal_f signalFunc );
OYAPI int  OYEXPORT
           oyStruct_ObserverRemove   ( oyStruct_s        * model,
                                       oyStruct_s        * observer,
                                       oyObserver_Signal_f signalFunc );
OYAPI int  OYEXPORT
           oyStruct_ObserverSignal   ( oyStruct_s        * model,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data );
OYAPI int  OYEXPORT
           oyStruct_DisableSignalSend( oyStruct_s        * model );
OYAPI int  OYEXPORT
           oyStruct_EnableSignalSend ( oyStruct_s        * model );
OYAPI int  OYEXPORT
           oyStruct_ObserversCopy    ( oyStruct_s        * object,
                                       oyStruct_s        * pattern,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
           oyStruct_ObserverCopyModel( oyStruct_s        * model,
                                       oyStruct_s        * pattern,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
           oyStruct_ObserverCopyObserver (
                                       oyStruct_s        * observer,
                                       oyStruct_s        * pattern,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
           oyStruct_IsObserved       ( oyStruct_s        * model,
                                       oyStruct_s        * observer );
OYAPI int  OYEXPORT
           oyStruct_ObservationCount ( oyStruct_s        * observer,
                                       uint32_t            flags );
OYAPI uint32_t OYEXPORT
           oyObserverGetFlags        ( void );
OYAPI int  OYEXPORT
           oyObserverSetFlags        ( uint32_t            flags );
