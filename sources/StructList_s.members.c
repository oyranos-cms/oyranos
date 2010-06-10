/** @internal
 *  @brief a pointer list
 *  @ingroup objects_generic
 *  @extends oyStruct_s
 *
 *  Memory management is done by Oyranos' oyAllocateFunc_ and oyDeallocateFunc_.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */

oyStruct_s        ** ptr_;           /**< @private the list data */
int                  n_;             /**< @private the number of visible pointers */
int                  n_reserved_;    /**< @private the number of allocated pointers */
char               * list_name;      /**< name of list */
oyOBJECT_e           parent_type_;   /**< @private parents struct type */
