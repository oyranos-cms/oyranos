/**
 *  typedef oyCMMProfileTag_GetValues_f
 *  @memberof oyCMMapi3_s
 */
typedef oyStructList_s *    (*oyCMMProfileTag_GetValues_f) (
                                       oyProfileTag_s    * tag );
/**
 *  typedef oyCMMProfileTag_Create_f
 *  @memberof oyCMMapi3_s
 */
typedef int                 (*oyCMMProfileTag_Create_f) (
                                       oyProfileTag_s    * tag,
                                       oyStructList_s    * list,
                                       icTagTypeSignature  tag_type,
                                       uint32_t            version );
