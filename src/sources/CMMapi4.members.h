  /** mandatory for "//color/icc" context producing filters */
  oyCMMFilterNode_ContextToMem_f   oyCMMFilterNode_ContextToMem;
  /** optional for a set oyCMMFilterNode_ContextToMem,
   *  used to override a Oyranos side hash creation */
  oyCMMFilterNode_GetText_f        oyCMMFilterNode_GetText;
  /** the data type of the context returned by oyCMMapi4_s::oyCMMFilterNode_ContextToMem_f, mandatory for a set oyCMMFilterNode_ContextToMem
   *  e.g. oyCOLOR_ICC_DEVICE_LINK / "oyDL" */
  char             context_type[8];

  /** a UI description
   *  Obligatory is a implemented oyCMMapi4_s::ui->getText( x, y ) call. The x
   *  argument shall cover "name" and "help" */
  oyCMMui_s_     * ui;                 /**< a UI description */
