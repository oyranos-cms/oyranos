//Private class definition:
struct oyObject_s_ {
  oyOBJECT_e           type_;          /*!< @private struct type oyOBJECT_OBJECT_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  int                  id_;            /**< @private identification for Oyranos */
  oyAlloc_f            allocateFunc_;  /**< @private data  allocator */
  oyDeAlloc_f          deallocateFunc_;/**< @private data release function */
  oyPointer            parent_;        /*!< @private parent struct of parent_type */
  oyOBJECT_e           parent_type_;   /*!< @private parents struct type */
  oyPointer            backdoor_;      /*!< @private allow non breaking extensions */
  oyOptions_s        * handles_;       /**< @private addational data and infos*/
  oyName_s           * name_;          /*!< @private naming feature */
  int                  ref_;           /*!< @private reference counter */
  int                  version_;       /*!< @private OYRANOS_VERSION */
  unsigned char      * hash_ptr_;      /**< @private 2*OY_HASH_SIZE */
  oyPointer            lock_;          /**< @private the user provided lock */
};

//Private function declarations:

oyObject_s   oyObject_SetAllocators_  ( oyObject_s        object,
                                        oyAlloc_f         allocateFunc,
                                        oyDeAlloc_f       deallocateFunc )
int          oyObject_Ref            ( oyObject_s          obj )
int          oyObject_UnRef          ( oyObject_s          obj )
int32_t      oyObject_Hashed_        ( oyObject_s          s )
int          oyObject_HashSet          ( oyObject_s        s,
                                         const unsigned char * hash )
int          oyObject_HashEqual        ( oyObject_s        s1,
                                         oyObject_s        s2 )
