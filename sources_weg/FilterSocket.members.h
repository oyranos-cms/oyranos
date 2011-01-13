  oyFilterNode_s     * node;           /**< filter node for this connector */
  oyFilterPlugs_s    * requesting_plugs_;/**< @private all remote inputs */
  oyStruct_s         * data;           /**< unprocessed data model */

  oyConnector_s      * pattern;        /**< a pattern the filter node can handle through this connector */
  char               * relatives_;     /**< @private hint about belonging to a filter */
