  oyFilterPlug_s    ** plugs;          /**< possible input connectors */
  int                  plugs_n_;       /**< readonly number of inputs */
  oyFilterSocket_s  ** sockets;        /**< possible output connectors */
  int                  sockets_n_;     /**< readonly number of outputs */

  oyFilterCore_s     * core;           /**< the filter core */
  char               * relatives_;     /**< @private hint about belonging to a filter */
  oyOptions_s        * tags;           /**< infos, e.g. group markers */

  /** the filters private data, requested over 
   *  oyCMMapi4_s::oyCMMFilterNode_ContextToMem() and converted to
   *  oyCMMapi4_s::context_type */
  oyPointer_s        * backend_data;
  /** the processing function and node connector descriptions */
  oyCMMapi7_s        * api7_;
