  /** mandatory for all filters; Special care has to taken for the
      oyPixelAccess_s argument to this function. */
  oyCMMFilterPlug_Run_f oyCMMFilterPlug_Run;
  /** e.g. oyCMMFilterPlug_Run specific context data "lcCC" */
  char             context_type[8];

  /** We have to tell about valid input and output connectors, by 
      passively providing enough informations. */
  oyConnector_s ** plugs;
  uint32_t         plugs_n;            /**< number of different plugs */ 
  /** additional allowed number for last input connector, e.g. typical 0 */
  uint32_t         plugs_last_add;
  oyConnector_s ** sockets;            /**< the socket description */
  uint32_t         sockets_n;          /**< number of sockets */
  /** additional allowed number for last output connector, e.g. typical 0 */
  uint32_t         sockets_last_add;
  /** A zero terminated list of strings. The semantics are plugin defined.
   *  e.g.: {"key1=value1","key2=value2",0} */
  char **          properties;
