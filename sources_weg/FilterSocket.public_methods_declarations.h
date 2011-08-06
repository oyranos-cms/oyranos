OYAPI int  OYEXPORT
                 oyFilterSocket_Callback (
                                       oyFilterPlug_s    * c,
                                       oyCONNECTOR_EVENT_e e );
#ifdef UNHIDE_FILTERSOCKET
OYAPI int  OYEXPORT
                 oyFilterSocket_SignalToGraph (
                                       oyFilterSocket_s  * c,
                                       oyCONNECTOR_EVENT_e e );
#endif /*UNHIDE_FILTERSOCKET*/
