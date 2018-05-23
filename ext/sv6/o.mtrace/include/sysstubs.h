#include "types.h"
#include <uk/unistd.h>

BEGIN_DECLS

struct stat;
struct sigaction;
struct sockaddr;
struct utsname;
void disktest(void);
uint64_t time_nsec(void);
int dup(int);
int dup2(int, int);
off_t lseek(int, off_t, int);
int close(int);
void sync(void);
int fsync(int);
ssize_t read(int, void*, size_t);
ssize_t pread(int, void *, size_t, off_t);
ssize_t write(int, const void*, size_t);
ssize_t pwrite(int, const void *, size_t, off_t);
int fstatx(int, struct stat*, enum stat_flags);
int link(const char *, const char *);
int rename(const char *, const char *);
int unlink(const char *);
int openat(int, const char *, int, ...);
int mkdirat(int, const char *, mode_t);
int mknod(const char *, int, int);
int chdir(const char *);
int execv(const char *upath, char * const uargv[]);
int pipe2(int*, int);
int pipe(int*);
int readdir(int, const char*, char*);
int sys_spawn(const char *upath, char * const uargv[], const void *actions, size_t actions_len);
int fork_flags(int);
void exit(int) __attribute__((noreturn));
int waitpid(int, int*, int);
int wait(int*);
int kill(int);
int getpid(void);
char* sbrk(int);
int nsleep(u64);
u64 uptime(void);
void * mmap(void*, size_t, int, int, int, off_t);
int munmap(void*, size_t);
int madvise(void*, size_t, int);
int mprotect(void*, size_t, int);
long pt_pages(void);
void halt(void) __attribute__((noreturn));
long cpuhz(void);
int setfs(u64);
int setaffinity(int);
long futex(const u64*, int, u64, u64);
long yield(void);
int uname(struct utsname*);
int dup_page(void*, void*);
int sigaction(int, struct sigaction*, struct sigaction*);
int socket(int, int, int);
int bind(int, const struct sockaddr*, uint32_t);
int listen(int, int);
int accept(int, struct sockaddr*, uint32_t*);
ssize_t recvfrom(int, void*, size_t, int, struct sockaddr*, uint32_t*);
ssize_t sendto(int, const void*, size_t, int, const struct sockaddr*, uint32_t);
int connect(int, const struct sockaddr*, u32);
ssize_t send(int, const void*, size_t, int);
ssize_t recv(int, void*, size_t, int);

END_DECLS
