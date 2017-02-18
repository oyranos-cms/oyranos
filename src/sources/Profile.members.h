  char               * file_name_;     /*!< @private file name for loading on request */
  size_t               size_;          /*!< @private ICC profile size */
  void               * block_;         /*!< @private ICC profile data */
  icColorSpaceSignature sig_;          /*!< @private ICC profile signature */
  oyPROFILE_e          use_default_;   /*!< @private if > 0 : take from settings */
  oyObject_s         * names_chan_;    /*!< @private user visible channel description */
  int                  channels_n_;    /*!< @private number of channels */
  oyStructList_s     * tags_;          /**< @private list of header + tags */
  int                  tags_modified_; /**< @private note manipulation */
  oyConfig_s         * meta_;          /**< @private parsed meta tag */
