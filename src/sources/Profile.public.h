/** \addtogroup objects_profile ICC Profile APIs
 *
 *  @{ */
/** @typedef oyIO_t
    parametric type as shorthand for IO flags \n

    should fit into a 32bit type, usual unsigned int or uint32_t \n

    ------ W R \n

    R  no cache read \n
    W  no cache write \n

 */
#define OY_NO_CACHE_READ            0x01        /**< read not from cache */
#define OY_NO_CACHE_WRITE           0x02        /**< write not from cache */
/* decode */
#define oyToNoCacheRead_m(r)        ((r)&1)
#define oyToNoCacheWrite_m(w)       (((w) >> 1)&1)

#define OY_FROM_PROFILE 0x04                    /**< use from profile */
#define OY_COMPUTE      0x08                    /**< compute newly */
#define OY_ICC_VERSION_2 0x10                   /**< filter for version 2 profiles */
#define OY_ICC_VERSION_4 0x20                   /**< filter for version 4 profiles */
#define OY_SKIP_NON_DEFAULT_PATH 0x40           /**< ignore profiles outside of default paths */
#define OY_NO_REPAIR     0x80                   /**< do not try to repair or fix profiles */
/** @} *//* objects_profile */
