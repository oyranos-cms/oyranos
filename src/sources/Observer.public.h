#define OY_SIGNAL_BLOCK                0x01 /**< do not send new signals */
#define oyToSignalBlock_m(r)           ((r)&1)
#define OY_SIGNAL_OBSERVERS OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "oyStructList_s/observers"
/*  The models list of a observing object is just a clone of the oyObserver_s
 *  object added to the model. */
#define OY_SIGNAL_MODELS OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "oyStructList_s/models"
 
/** @enum    oySIGNAL_e
 *  @brief   observer signals
 *  @ingroup objects_generic
 *
 *  The signal types are similiar to the graph event enum oyCONNECTOR_EVENT_e.
 *
 *  @see oySignalToString()
 *
 *  @version Oyranos: 0.9.7
 *  @date    2020/02/26
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 */
typedef enum {
  oySIGNAL_OK,
  oySIGNAL_CONNECTED,                  /**< connection established */
  oySIGNAL_RELEASED,                   /**< released the connection */
  oySIGNAL_DATA_CHANGED,               /**< call to update image views */
  oySIGNAL_STORAGE_CHANGED,            /**< new data accessors */
  oySIGNAL_INCOMPATIBLE_DATA,          /**< can not process image */
  oySIGNAL_INCOMPATIBLE_OPTION,        /**< can not handle option */
  oySIGNAL_INCOMPATIBLE_CONTEXT,       /**< can not handle profile */
  oySIGNAL_INCOMPLETE_GRAPH,           /**< can not completely process */
  oySIGNAL_VISITED,                    /**< just accessed */
  oySIGNAL_USER1 = 20,                 /**< user signal type */
  oySIGNAL_USER2,                      /**< user signal type */
  oySIGNAL_USER3                       /**< more signal types are possible */
} oySIGNAL_e;

const char *       oySignalToString  ( oySIGNAL_e          signal_type );


/** @brief   signal handler type
 *
 *  @param[in]     observer            the actual observer
 *  @param[in]     signal_type         the sent signal type
 *  @param[in,out] signal_data         the sent signal data to analyse or manipulate
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/10/26
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 */
typedef  int      (*oyObserver_Signal_f) (
                                       oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data );

