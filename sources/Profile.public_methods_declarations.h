OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromStd ( oyPROFILE_e         colour_space,
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
                                       oyObject_s          object );
OYAPI int OYEXPORT oyProfile_GetChannelsCount ( oyProfile_s * colour );
OYAPI icSignature OYEXPORT
             oyProfile_GetSignature (  oyProfile_s       * profile,
                                       oySIGNATURE_TYPE_e  type );
OYAPI int OYEXPORT
             oyProfile_SetSignature (  oyProfile_s       * profile,
                                       icSignature         sig,
                                       oySIGNATURE_TYPE_e  type );
OYAPI void OYEXPORT oyProfile_SetChannelNames( oyProfile_s * colour,
                                       oyObject_s        * names_chan );
OYAPI const oyObject_s * OYEXPORT
                   oyProfile_GetChannelNames( oyProfile_s * colour);
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
oyProfileTag_s *   oyProfile_GetTagByPos ( oyProfile_s   * profile,
                                       int                 pos );
oyProfileTag_s *   oyProfile_GetTagById ( oyProfile_s    * profile,
                                       icTagSignature      id );
int                oyProfile_GetTagCount( oyProfile_s    * profile );
int                oyProfile_TagMoveIn(oyProfile_s       * profile,
                                       oyProfileTag_s   ** tag,
                                       int                 pos );
int                oyProfile_TagReleaseAt ( oyProfile_s  * profile,
                                       int                 pos );
int                oyProfile_AddTagText ( oyProfile_s       * profile,
                                          icSignature         signature,
                                          const char        * text );
const char   *     oyProfile_GetFileName ( oyProfile_s   * profile,
                                       int                 dl_pos );
int                oyProfile_DeviceGet ( oyProfile_s     * profile,
                                       oyConfig_s        * device );


