  /** a colon separated list of sub paths to expect the scripts in,
      e.g. "color/shiva:color/octl" */
  const char     * sub_paths;
  /** optional filename extensions, e.g. "shi:ctl" */
  const char     * ext;
  /** 0: libs - libraries, Oyranos searches in the XDG_LIBRARY_PATH and sub_paths, The library will be provided as file_name\n  1: scripts - platform independent filters, Oyranos will search in the XDG_DATA_* paths, Script are provided as i memory blobs */
  int32_t          data_type;          /**< */

  oyCMMFilterLoad_f                oyCMMFilterLoad; /**< */
  oyCMMFilterScan_f                oyCMMFilterScan; /**< */
