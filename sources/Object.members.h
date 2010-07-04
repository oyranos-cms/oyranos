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
