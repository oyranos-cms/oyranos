#define OY_CREATE_NEW                  0x02        /** create */
#define OY_ADD_ALWAYS                  0x04        /** do not use for configurations */
/* decode */
#define oyToCreateNew_m(r)             (((r) >> 1)&1)
#define oyToAddAlways_m(r)             (((r) >> 2)&1)
