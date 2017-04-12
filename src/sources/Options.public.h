/** \addtogroup objects_value
 *  @{ *//* objects_value */
#define OY_CREATE_NEW                  0x02        /**< create */
#define OY_ADD_ALWAYS                  0x04        /**< no check for double occurencies; do not use for configurations */
#define OY_MATCH_KEY                   0x08        /**< check for double occurencies of key name, ignoring the path */
/* decode */
#define oyToCreateNew_m(r)             (((r) >> 1)&1)
#define oyToAddAlways_m(r)             (((r) >> 2)&1)
/** @} *//* objects_value */
