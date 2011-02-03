int            oyOptions_Init__Members(oyOptions_s_      * s );
int            oyOptions_Copy__Members(oyOptions_s_      * s );
int            oyOptions_Release__Members (
                                       oyOptions_s_      * s );

void           oyOptions_ParseXML_   ( oyOptions_s_      * s,
                                       char            *** texts,
                                       int               * texts_n,
                                       xmlDocPtr           doc,
                                       xmlNodePtr          cur );
