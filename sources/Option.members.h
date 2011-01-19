  uint32_t             id;             /**< id to map to events and widgets */
  char               * registration;   /**< full key path name to store configuration, e.g. "shared/oyranos.org/imaging/scale/x", see as well @ref registration @see oyOPTIONATTRIBUTE_e */
  int                  version[3];     /**< as for oyCMMapi4_s::version */
  oyVALUETYPE_e        value_type;     /**< the type in value */
  oyValue_u          * value;          /**< the actual value */
  oyOPTIONSOURCE_e     source;         /**< the source of this value */
  uint32_t             flags;          /**< | oyOPTIONATTRIBUTE_e */
