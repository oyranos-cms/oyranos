  oyFilterNode_s     * node;           /**< filter node for this connector */
  oyFilterSocket_s_  * remote_socket_; /**< @private the remote output */

  oyConnector_s      * pattern;        /**< a pattern the filter node can handle through this connector */
  char               * relatives_;     /**< @private hint about belonging to a filter */
