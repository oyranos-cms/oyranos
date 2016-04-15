#define OY_STRING_LIST                 0x01 /**< create a oyVAL_STRING_LIST */
/* decode */
#define oyToStringList_m(r)            ((r)&1)

/** @enum    oyOPTIONSOURCE_e
 *  @brief   a option source type
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/13 (Oyranos: 0.1.9)
 *  @date    2008/11/27
 */
typedef enum {
  oyOPTIONSOURCE_NONE = 0,             /**< not clear */
  oyOPTIONSOURCE_FILTER = 2,           /**< internal defaults, e.g. module */
  oyOPTIONSOURCE_DATA = 4,             /**< external defaults, e.g. policy, DB*/
  oyOPTIONSOURCE_USER = 8              /**< user settings, e.g. GUI */
} oyOPTIONSOURCE_e;

/* Value Definitions { */
/** @enum    oyVALUETYPE_e
 *  @brief   a value type
 *
    @see     oyValue_u
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/16 (Oyranos: 0.1.x)
 *  @date    2008/02/16
 */
typedef enum {
  oyVAL_INT,       /**< integer number */
  oyVAL_INT_LIST,  /**< integer numbers */
  oyVAL_DOUBLE,    /**< IEEE double precission floating point number */
  oyVAL_DOUBLE_LIST,  /**< double numbers */
  oyVAL_STRING,    /**< char array */
  oyVAL_STRING_LIST,  /**< array of char arrays */
  oyVAL_STRUCT     /**< for pure data blobs use oyBlob_s herein */
} oyVALUETYPE_e;

/** @union   oyValue_u
 *  @brief   a value
 *  @ingroup objects_value
 *
 *  @see     oyVALUETYPE_e
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/16 (Oyranos: 0.1.x)
 *  @date    2008/02/16
 */
typedef union {
  int32_t          int32;
  int32_t        * int32_list;         /**< first is element number of int32 list */
  double           dbl;
  double         * dbl_list;           /**< first is element number of dbl list */
  char           * string;             /**< null terminated */
  char          ** string_list;        /**< null terminated */

  oyStruct_s     * oy_struct;          /**< a struct, e.g. a profile, or oyBlob_s for a data pointer */
} oyValue_u;

void           oyValueCopy           ( oyValue_u         * to,
                                       oyValue_u         * from,
                                       oyVALUETYPE_e       type,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc );
void           oyValueRelease        ( oyValue_u        ** value,
                                       oyVALUETYPE_e       type,
                                       oyDeAlloc_f         deallocateFunc );
void           oyValueClear          ( oyValue_u         * v,
                                       oyVALUETYPE_e       type,
                                       oyDeAlloc_f         deallocateFunc );
int            oyValueEqual          ( oyValue_u         * a,
                                       oyValue_u         * b,
                                       oyVALUETYPE_e       type,
                                       int                 pos );
/* } Value Definitions */
