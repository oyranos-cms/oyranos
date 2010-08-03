  /** translated, e.g. "help": "createICC" "create a ICC profile" "With ..."
   */
  oyCMMGetText_f   getText;
  /** zero terminated list for ::getText()
   *  include "help", "can_handle" and the supported commands
   */
  char          ** texts;              /**< zero terminated list for getText */

  oyMOptions_Handle_f oyMOptions_Handle; /**< @memberof oyCMMapi10_s */
