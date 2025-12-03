#ifdef LAB_FS
#define NPROC        10  // maximum number of processes
#else
#define NPROC        64  // maximum number of processes (speedsup bigfile)
#endif
#define NCPU          8  // maximum number of CPUs
#define NOFILE       16  // open files per process
#define NFILE       100  // open files per system
#define NINODE       50  // maximum number of active i-nodes
#define NDEV         10  // maximum major device number
#define ROOTDEV       1  // device number of file system root disk
#define MAXARG       32  // max exec arguments
#define MAXOPBLOCKS  10  // max # of blocks any FS op writes
#define LOGBLOCKS    (MAXOPBLOCKS*3)  // max data blocks in on-disk log
#define NBUF         (MAXOPBLOCKS*3)  // size of disk block cache

// MLFQ Scheduler constants
#define MLFQ_LEVELS  4        // Number of priority levels
#define L0     4        // Time quantum level 0
#define L1     8        // Time quantum level 1
#define L2     16       // Time quantum level 2
#define L3     32       // Time quantum level 3
#define BOOST_INTERVAL  200   // Ticks between priority boosts
#ifdef LAB_FS
#define FSSIZE       200000  // size of file system in blocks
#else
#ifdef LAB_LOCK
#define FSSIZE       10000  // size of file system in blocks
#else
#define FSSIZE       2000   // size of file system in blocks
#endif
#endif
#define MAXPATH      128   // maximum file path name

#ifdef LAB_UTIL
#define USERSTACK    2     // user stack pages
#else
#define USERSTACK    1     // user stack pages
#endif


