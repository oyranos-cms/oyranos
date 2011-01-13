  /** unique strings, nick and name will be used as the connector's type ID,
   *  e.g."Img", "Image", "Image Socket"*/
  oyName_s             name;           

  char               * connector_type; /**< a @ref registration string */
  /** Check if two oyCMMapi7_s filter connectors of type oyConnector_s can 
   *  match each other inside a given socket and a plug. */
  oyCMMFilterSocket_MatchPlug_f  filterSocket_MatchPlug;

  /** make requests and receive data, by part of oyFilterPlug_s */
  int                  is_plug;
