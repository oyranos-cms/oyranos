OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromStd ( oyPROFILE_e         type,
                                       uint32_t            flags,
                                       oyObject_s          object);
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromName( const char*         name,
                                       uint32_t            flags,
                                       oyObject_s          object);
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromFile( const char*         file_name,
                                       uint32_t            flags,
                                       oyObject_s          object);
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromMem ( size_t              buf_size,
                                       const oyPointer     buf,
                                       uint32_t            flags,
                                       oyObject_s          object);
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromSignature(
                                       icSignature         sig,
                                       oySIGNATURE_TYPE_e  type,
                                       oyObject_s          object );
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromMD5(  uint32_t          * md5,
                                       uint32_t            flags,
                                       oyObject_s          object );
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromTaxiDB (
                                       oyOptions_s       * options,
                                       oyObject_s          object );
OYAPI int OYEXPORT oyProfile_Install ( oyProfile_s       * profile,
                                       oyOptions_s       * options );
OYAPI int OYEXPORT
         oyProfile_GetChannelsCount ( oyProfile_s * color );
OYAPI icSignature OYEXPORT
             oyProfile_GetSignature (  oyProfile_s       * profile,
                                       oySIGNATURE_TYPE_e  type );
OYAPI int OYEXPORT
             oyProfile_SetSignature (  oyProfile_s       * profile,
                                       icSignature         sig,
                                       oySIGNATURE_TYPE_e  type );
OYAPI void OYEXPORT
            oyProfile_SetChannelNames( oyProfile_s * color,
                                       oyObject_s        * names_chan );
OYAPI const oyObject_s * OYEXPORT
                   oyProfile_GetChannelNames( oyProfile_s * color);
OYAPI const char   * OYEXPORT
                   oyProfile_GetChannelName ( oyProfile_s * profile,
                                         int               channel_pos,
                                         oyNAME_e          type );
OYAPI const char  * OYEXPORT
                   oyProfile_GetID   ( oyProfile_s       * profile );
OYAPI int OYEXPORT
                   oyProfile_Equal   ( oyProfile_s       * profileA,
                                       oyProfile_s       * profileB );
OYAPI const char  * OYEXPORT
                   oyProfile_GetText ( oyProfile_s       * profile,
                                       oyNAME_e            type );
OYAPI oyPointer OYEXPORT
                   oyProfile_GetMem  ( oyProfile_s       * profile,
                                       size_t            * size,
                                       uint32_t            flag,
                                       oyAlloc_f           allocateFunc );
OYAPI size_t OYEXPORT
                   oyProfile_GetSize ( oyProfile_s       * profile,
                                       uint32_t            flag );
OYAPI oyProfileTag_s * OYEXPORT
               oyProfile_GetTagByPos ( oyProfile_s   * profile,
                                       int                 pos );
OYAPI oyProfileTag_s * OYEXPORT
                oyProfile_GetTagById ( oyProfile_s    * profile,
                                       icTagSignature      id );
OYAPI int OYEXPORT
                   oyProfile_GetTagCount( oyProfile_s    * profile );
OYAPI int OYEXPORT
                   oyProfile_TagMoveIn(oyProfile_s       * profile,
                                       oyProfileTag_s   ** tag,
                                       int                 pos );
OYAPI int OYEXPORT
                   oyProfile_TagReleaseAt ( oyProfile_s  * profile,
                                       int                 pos );
OYAPI int OYEXPORT
                   oyProfile_AddTagText ( oyProfile_s       * profile,
                                          icSignature         signature,
                                          const char        * text );
OYAPI const char * OYEXPORT
                   oyProfile_GetFileName ( oyProfile_s   * profile,
                                           int             dl_pos );
OYAPI int OYEXPORT
                   oyProfile_GetDevice ( oyProfile_s     * profile,
                                         oyConfig_s      * device );
OYAPI int  OYEXPORT
                 oyProfile_GetMD5  ( oyProfile_s       * profile,
                                       int                 flags,
                                       uint32_t          * md5 );
OYAPI int  OYEXPORT
                 oyProfile_AddDevice(oyProfile_s       * profile,
                                       oyConfig_s        * device,
                                       oyOptions_s       * options );
