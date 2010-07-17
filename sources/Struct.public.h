typedef oyStruct_s * (*oyStruct_Copy_f ) ( oyStruct_s *, oyPointer );
typedef int       (*oyStruct_Release_f ) ( oyStruct_s ** );
typedef oyPointer (*oyStruct_LockCreate_f)(oyStruct_s * obj );

/* Locking function declarations { */
extern oyStruct_LockCreate_f   oyStruct_LockCreateFunc_;
extern oyLockRelease_f         oyLockReleaseFunc_;
extern oyLock_f                oyLockFunc_;
extern oyUnLock_f              oyUnLockFunc_;
/* } Locking function declarations */

/* Message function declarations { */
typedef enum {
  oyMSG_ERROR = 300,
  oyMSG_WARN,
  oyMSG_DBG
} oyMSG_e;

typedef int  (*oyMessage_f)          ( int/*oyMSG_e*/      code,
                                       const oyStruct_s  * context,
                                       const char        * format,
                                       ... );
int            oyMessageFuncSet      ( oyMessage_f         message_func );
extern         oyMessage_f             oyMessageFunc_p;
/* } Message function declarations */
