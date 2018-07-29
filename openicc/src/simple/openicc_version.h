#ifndef OPENICC_VERSION_H
#define OPENICC_VERSION_H

#define OPENICC_VERSION_A 0
#define OPENICC_VERSION_B 1
#define OPENICC_VERSION_C 1
#define OPENICC_VERSION_NAME "0.1.1"
#define OPENICC_VERSION (OPENICC_VERSION_A * 10000 + OPENICC_VERSION_B * 100 + OPENICC_VERSION_C)	/**< version variable */

#define OPENICC_PREFIX         "/home/kuwe/.local"	/**< install variable */
#define OPENICC_BINDIR         "/home/kuwe/.local/bin"	/**< install variable */
#define OPENICC_DATADIR        "/home/kuwe/.local/share"	/**< install variable */
#define OPENICC_LIBDIR         "/home/kuwe/.local/lib64"	/**< install variable */
#define OPENICC_DATADIR        "/home/kuwe/.local/share"	/**< install variable */
#define OPENICC_SYSCOLORDIR    OPENICC_DATADIR "/color"	/**< install variable */
#define OPENICC_LOCALEDIR      "/home/kuwe/.local/share/locale"
#define OPENICC_CMMSUBPATH     "color/modules"	/**< install variable */
#define OPENICC_METASUBPATH    "openicc"	/**< install variable */
#define OPENICC_CMMDIR         OPENICC_LIBDIR "/" OPENICC_CMMSUBPATH	/**< install variable */

#endif /* OPENICC_VERSION_H */
