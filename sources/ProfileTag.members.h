  icTagSignature       use;            /**< tag functionality inside profile */
  icTagTypeSignature   tag_type_;      /**< @private tag type to decode the data block */

  oySTATUS_e           status_;        /**< @private status at load time */

  size_t               offset_orig;    /**< information from profile read */
  size_t               size_;          /**< @private data block size */
  oyPointer            block_;         /**< @private the data to interprete */

  char                 profile_cmm_[5];/**< @private the profile prefered CMM */
  char                 required_cmm[5];/**< selected a certain CMM */
  char                 last_cmm_[5];   /**< info: last processing CMM */
