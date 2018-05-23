#define STR_1(x...) #x
#define STR(x...)   STR_1(x)
const char *kconfig =
"NPROC=" STR(NPROC) "\n"
"KSTACKSIZE=" STR(KSTACKSIZE) "\n"
"NOFILE=" STR(NOFILE) "\n"
"NFILE=" STR(NFILE) "\n"
"NBUF=" STR(NBUF) "\n"
"NINODE=" STR(NINODE) "\n"
"NDEV=" STR(NDEV) "\n"
"ROOTDEV=" STR(ROOTDEV) "\n"
"MAXARG=" STR(MAXARG) "\n"
"MAXARGLEN=" STR(MAXARGLEN) "\n"
"MAXNAME=" STR(MAXNAME) "\n"
"UNIX_PATH_MAX=" STR(UNIX_PATH_MAX) "\n"
"NEPOCH=" STR(NEPOCH) "\n"
"CACHELINE=" STR(CACHELINE) "\n"
"CPUKSTACKS=" STR(CPUKSTACKS) "\n"
"VICTIMAGE=" STR(VICTIMAGE) "\n"
"VERBOSE=" STR(VERBOSE) "\n"
"SPINLOCK_DEBUG=" STR(SPINLOCK_DEBUG) "\n"
"RCU_TYPE_DEBUG=" STR(RCU_TYPE_DEBUG) "\n"
"LOCKSTAT=" STR(LOCKSTAT) "\n"
"ALLOC_MEMSET=" STR(ALLOC_MEMSET) "\n"
"BUDDY_DEBUG=" STR(BUDDY_DEBUG) "\n"
"REFCACHE_DEBUG=" STR(REFCACHE_DEBUG) "\n"
"RADIX_DEBUG=" STR(RADIX_DEBUG) "\n"
"SEQLOCK_DEBUG=" STR(SEQLOCK_DEBUG) "\n"
"KSTACK_DEBUG=" STR(KSTACK_DEBUG) "\n"
"USTACKPAGES=" STR(USTACKPAGES) "\n"
"GCINTERVAL=" STR(GCINTERVAL) "\n"
"GC_GLOBAL=" STR(GC_GLOBAL) "\n"
"MMU_SCHEME=" STR(MMU_SCHEME) "\n"
"TLB_SCHEME=" STR(TLB_SCHEME) "\n"
"PAGE_REFCOUNT=" STR(PAGE_REFCOUNT) "\n"
"KALLOC_HOT_PAGES=" STR(KALLOC_HOT_PAGES) "\n"
"KALLOC_LOAD_BALANCE=" STR(KALLOC_LOAD_BALANCE) "\n"
"KALLOC_BUDDY_PER_CPU=" STR(KALLOC_BUDDY_PER_CPU) "\n"
"SCHED_LOAD_BALANCE=" STR(SCHED_LOAD_BALANCE) "\n"
"FS_NLINK_REFCOUNT=" STR(FS_NLINK_REFCOUNT) "\n"
"RANDOMIZE_KMALLOC=" STR(RANDOMIZE_KMALLOC) "\n"
"KERNEL_HEAP_PROFILE=" STR(KERNEL_HEAP_PROFILE) "\n"
"CODEX=" STR(CODEX) "\n"
"E1000_PORT=" STR(E1000_PORT) "\n"
"TZ_SECS=" STR(TZ_SECS) "\n"
"RTC_TZ_SECS=" STR(RTC_TZ_SECS) "\n"
"MEMIDE=" STR(MEMIDE) "\n"
"AHCIIDE=" STR(AHCIIDE) "\n"
;