  uint32_t             id;             /**< id to map to events and widgets */
  /** This property contains the identifier for communication with a Oyranos
   *  or a module through Oyranos. It defines the basic key path name to store
   *  configuration.\n
   *  e.g. "org/freedesktop/imaging/config.monitor.xorg" \n
   *  see as well @ref registration */
  char               * registration;
  int                  version[3];     /**< as for oyCMMapi4_s::version */

  /** data base (Elektra) properties,
  e.g. "org/freedesktop/imaging/config.monitor.xorg/1/manufacturer=EIZO"*/
  oyOptions_s        * db;
  /** These are the module core properties, the ones to identify the 
   *  device and store in DB. They must be filled by the module.
  e.g. "org/freedesktop/imaging/config.monitor.xorg/manufacturer=EIZO" */
  oyOptions_s        * backend_core;
  /** Additional informations from modules, with non identification purpose,
   *  can be stored herein,
  e.g. "org/freedesktop/imaging/config.monitor.xorg/edid=oyBlob_s*" */
  oyOptions_s        * data;

  oyRankPad          * rank_map;       /**< zero terminated list; key compare */
