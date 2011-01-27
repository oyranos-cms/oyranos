  /** object type id,
   *  set to a object type known to Oyranos, or
   *  set as a unique four byte signature, like 'myID' just more unique
   *  to avoid collisions. The id shall match a the oyStruct_s::type_ member
   *  generated through oyCMMobjectLoadFromMem_f. */
  oyOBJECT_e       id;
  /** a colon separated list of sub paths to expect the data in,
      e.g. "color/icc" */
  const char     * paths;
  const char     * (*pathsGet)();      /**< e.g. non XDG colon separated paths*/
  const char     * exts;               /**< file extensions, e.g. "icc:icm" */
  const char     * element_name;       /**< XML element name, e.g. "profile" */
  oyCMMobjectGetText_f             oyCMMobjectGetText; /**< */
  oyCMMobjectLoadFromMem_f         oyCMMobjectLoadFromMem; /**< */
  oyCMMobjectScan_f                oyCMMobjectScan; /**< */
