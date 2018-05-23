#include "types.h"
#include "kernel.hh"
#include <uk/unistd.h>
#include <uk/signal.h>

extern void sys_disktest(void);
extern uint64_t sys_time_nsec(void);
extern int sys_dup(int ofd);
extern int sys_dup2(int ofd, int nfd);
extern off_t sys_lseek(int fd, off_t offset, int whence);
extern int sys_close(int fd);
extern void sys_sync(void);
extern int sys_fsync(int fd);
extern ssize_t sys_read(int fd, userptr<void> p, size_t n);
extern ssize_t sys_pread(int fd, void *ubuf, size_t count, off_t offset);
extern ssize_t sys_write(int fd, const userptr<void> p, size_t n);
extern ssize_t sys_pwrite(int fd, const void *ubuf, size_t count, off_t offset);
extern int sys_fstatx(int fd, userptr<struct stat> st, enum stat_flags flags);
extern int sys_link(userptr_str old_path, userptr_str new_path);
extern int sys_rename(userptr_str old_path, userptr_str new_path);
extern int sys_unlink(userptr_str path);
extern int sys_openat(int dirfd, userptr_str path, int omode, ...);
extern int sys_mkdirat(int dirfd, userptr_str path, mode_t mode);
extern int sys_mknod(userptr_str path, int major, int minor);
extern int sys_chdir(userptr_str path);
extern int sys_execv(userptr_str upath, userptr<userptr_str> uargv);
extern int sys_pipe2(userptr<int> fd, int flags);
extern int sys_pipe(userptr<int> fd);
extern int sys_readdir(int dirfd, const userptr<char> prevptr, userptr<char> nameptr);
extern int sys_sys_spawn(userptr_str upath, userptr<userptr_str> uargv, const userptr<void> uactions, size_t actions_len);
extern int sys_fork_flags(int flags);
extern void sys_exit(int status);
extern int sys_waitpid(int pid, userptr<int> status, int options);
extern int sys_wait(userptr<int> status);
extern int sys_kill(int pid);
extern int sys_getpid(void);
extern char* sys_sbrk(int n);
extern int sys_nsleep(u64 nsec);
extern u64 sys_uptime(void);
extern void * sys_mmap(userptr<void> addr, size_t len, int prot, int flags, int fd, off_t offset);
extern int sys_munmap(userptr<void> addr, size_t len);
extern int sys_madvise(userptr<void> addr, size_t len, int advice);
extern int sys_mprotect(userptr<void> addr, size_t len, int prot);
extern long sys_pt_pages(void);
extern void sys_halt(void);
extern long sys_cpuhz(void);
extern int sys_setfs(u64 base);
extern int sys_setaffinity(int cpu);
extern long sys_futex(const u64* addr, int op, u64 val, u64 timer);
extern long sys_yield(void);
extern int sys_uname(userptr<struct utsname> buf);
extern int sys_dup_page(userptr<void> dest, userptr<void> src);
extern int sys_sigaction(int signo, userptr<struct sigaction> act, userptr<struct sigaction> oact);
extern int sys_socket(int domain, int type, int protocol);
extern int sys_bind(int xsock, const userptr<struct sockaddr> xaddr, uint32_t xaddrlen);
extern int sys_listen(int xsock, int backlog);
extern int sys_accept(int xsock, userptr<struct sockaddr> xaddr, userptr<uint32_t> xaddrlen);
extern ssize_t sys_recvfrom(int sockfd, userptr<void> buf, size_t len, int flags, userptr<struct sockaddr> src_addr, userptr<uint32_t> addrlen);
extern ssize_t sys_sendto(int sockfd, const userptr<void> buf, size_t len, int flags, const userptr<struct sockaddr> dest_addr, uint32_t addrlen);
extern int sys_connect(int sockfd, const userptr<struct sockaddr> addr, u32 addrlen);
extern ssize_t sys_send(int sockfd, const userptr<void> buf, size_t len, int flags);
extern ssize_t sys_recv(int sockfd, userptr<void> buf, size_t len, int flags);

u64 (*syscalls[])(u64, u64, u64, u64, u64, u64) = {
  nullptr,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_disktest,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_time_nsec,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_dup,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_dup2,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_lseek,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_close,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_sync,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_fsync,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_read,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_pread,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_write,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_pwrite,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_fstatx,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_link,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_rename,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_unlink,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_openat,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_mkdirat,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_mknod,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_chdir,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_execv,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_pipe2,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_pipe,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_readdir,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_sys_spawn,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_fork_flags,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_exit,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_waitpid,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_wait,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_kill,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_getpid,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_sbrk,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_nsleep,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_uptime,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_mmap,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_munmap,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_madvise,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_mprotect,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_pt_pages,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_halt,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_cpuhz,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_setfs,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_setaffinity,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_futex,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_yield,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_uname,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_dup_page,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_sigaction,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_socket,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_bind,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_listen,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_accept,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_recvfrom,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_sendto,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_connect,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_send,
  (u64(*)(u64,u64,u64,u64,u64,u64))sys_recv,
};

const char* syscall_names[] = {
  nullptr,
  "sys_disktest",
  "sys_time_nsec",
  "sys_dup",
  "sys_dup2",
  "sys_lseek",
  "sys_close",
  "sys_sync",
  "sys_fsync",
  "sys_read",
  "sys_pread",
  "sys_write",
  "sys_pwrite",
  "sys_fstatx",
  "sys_link",
  "sys_rename",
  "sys_unlink",
  "sys_openat",
  "sys_mkdirat",
  "sys_mknod",
  "sys_chdir",
  "sys_execv",
  "sys_pipe2",
  "sys_pipe",
  "sys_readdir",
  "sys_sys_spawn",
  "sys_fork_flags",
  "sys_exit",
  "sys_waitpid",
  "sys_wait",
  "sys_kill",
  "sys_getpid",
  "sys_sbrk",
  "sys_nsleep",
  "sys_uptime",
  "sys_mmap",
  "sys_munmap",
  "sys_madvise",
  "sys_mprotect",
  "sys_pt_pages",
  "sys_halt",
  "sys_cpuhz",
  "sys_setfs",
  "sys_setaffinity",
  "sys_futex",
  "sys_yield",
  "sys_uname",
  "sys_dup_page",
  "sys_sigaction",
  "sys_socket",
  "sys_bind",
  "sys_listen",
  "sys_accept",
  "sys_recvfrom",
  "sys_sendto",
  "sys_connect",
  "sys_send",
  "sys_recv",
};

extern const int nsyscalls = 58;
