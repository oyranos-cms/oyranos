OYAPI int  OYEXPORT
                 oyConnectorImaging_SetDataTypes (
                                       oyConnectorImaging_s  * c,
                                       oyDATATYPE_e          * data_types,
                                       int                     data_types_n );
OYAPI int  OYEXPORT
                 oyConnectorImaging_GetDataTypes (
                                       oyConnectorImaging_s  * c,
                                       const oyDATATYPE_e   ** data_types,
                                       int                   * data_types_n );
OYAPI int  OYEXPORT
                 oyConnectorImaging_GetChannelTypes (
                                       oyConnectorImaging_s  * c,
                                       const oyCHANNELTYPE_e** channel_types,
                                       int                   * channel_types_n );

/** @enum    oyCONNECTOR_IMAGING_CAP_e
 *  @brief   Capabilities of oyConnectorImaging_s
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/06 (Oyranos: 0.5.0)
 *  @date    2012/09/06
 */
typedef enum {
  oyCONNECTOR_IMAGING_CAP_IS_PLUG,
  oyCONNECTOR_IMAGING_CAP_MAX_COLOR_OFFSET,
  oyCONNECTOR_IMAGING_CAP_MIN_CHANNELS_COUNT,
  oyCONNECTOR_IMAGING_CAP_MAX_CHANNELS_COUNT,
  oyCONNECTOR_IMAGING_CAP_MIN_COLOR_COUNT,
  oyCONNECTOR_IMAGING_CAP_MAX_COLOR_COUNT,
  oyCONNECTOR_IMAGING_CAP_CAN_PLANAR,        /**< can read separated channels */
  oyCONNECTOR_IMAGING_CAP_CAN_INTERWOVEN,    /**< can read continuous channels */
  oyCONNECTOR_IMAGING_CAP_CAN_SWAP,          /**< can swap color channels (BGR)*/
  oyCONNECTOR_IMAGING_CAP_CAN_SWAP_BYTES,    /**< non host byte order */
  oyCONNECTOR_IMAGING_CAP_CAN_REVERT,        /**< revert 1 -> 0 and 0 -> 1 */
  oyCONNECTOR_IMAGING_CAP_CAN_PREMULTIPLIED_ALPHA,
  oyCONNECTOR_IMAGING_CAP_CAN_NONPREMULTIPLIED_ALPHA,
  oyCONNECTOR_IMAGING_CAP_CAN_SUBPIXEL,      /**< understand subpixel order */
  oyCONNECTOR_IMAGING_CAP_ID,                /**< relative to oyFilterCore_s, e.g. 1*/
  /**< connector is mandatory or optional, important for modules */
  oyCONNECTOR_IMAGING_CAP_IS_MANDATORY
} oyCONNECTOR_IMAGING_CAP_e;
OYAPI int  OYEXPORT
                 oyConnectorImaging_GetCapability (
                                       oyConnectorImaging_s    * c,
                                       oyCONNECTOR_IMAGING_CAP_e type );
OYAPI int  OYEXPORT
                 oyConnectorImaging_SetCapability (
                                       oyConnectorImaging_s    * c,
                                       oyCONNECTOR_IMAGING_CAP_e type,
                                       int                       value );

