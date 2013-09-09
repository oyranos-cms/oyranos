  oyDATATYPE_e       * data_types;     /**< supported float's and int's */
  int                  data_types_n;   /**< # elements in data_types array */
  int                  max_color_offset;
  int                  min_channels_count;
  int                  max_channels_count;
  int                  min_color_count;
  int                  max_color_count;
  int                  can_planar;     /**< can read separated channels */
  int                  can_interwoven; /**< can read continuous channels */
  int                  can_swap;       /**< can swap color channels (BGR)*/
  int                  can_swap_bytes; /**< non host byte order */
  int                  can_revert;     /**< revert 1 -> 0 and 0 -> 1 */
  int                  can_premultiplied_alpha;
  int                  can_nonpremultiplied_alpha;
  int                  can_subpixel;   /**< understand subpixel order */
  /** describe which channel types the connector requires */
  oyCHANNELTYPE_e    * channel_types;
  int                  channel_types_n;/**< count in channel_types */
  int                  id;             /**< relative to oyFilterCore_s, e.g. 1*/
  /**< connector is mandatory or optional, important for modules */
  int                  is_mandatory;
