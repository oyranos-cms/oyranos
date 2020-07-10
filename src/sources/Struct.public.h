#include <oyranos_core.h>

const char * oyStruct_TypeToText     ( const oyStruct_s  * st );

/** @brief reference a object or with second argument present do a copy */
typedef oyStruct_s * (*oyStruct_Copy_f ) ( oyStruct_s *, oyPointer );
/** @brief release object */
typedef int       (*oyStruct_Release_f ) ( oyStruct_s ** );
/** @brief create a lock object */
typedef oyPointer (*oyStruct_LockCreate_f)(oyStruct_s * obj );

/* Locking function declarations { */
extern oyStruct_LockCreate_f   oyStruct_LockCreateFunc_;
extern oyLockRelease_f         oyLockReleaseFunc_;
extern oyLock_f                oyLockFunc_;
extern oyUnLock_f              oyUnLockFunc_;
/* } Locking function declarations */

void         oyThreadLockingSet      ( oyStruct_LockCreate_f  createLockFunc,
                                       oyLockRelease_f     releaseLockFunc,
                                       oyLock_f            lockFunc,
                                       oyUnLock_f          unlockFunc );
int          oyThreadLockingReady    ( void );
void         oyThreadLockingReset    ( );

