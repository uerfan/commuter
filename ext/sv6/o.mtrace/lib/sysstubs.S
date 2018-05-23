#include "traps.h"

.globl SYS_disktest
SYS_disktest = 1

.globl disktest
disktest:
  movq $1, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_time_nsec
SYS_time_nsec = 2

.globl time_nsec
time_nsec:
  movq $2, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_dup
SYS_dup = 3

.globl dup
dup:
  movq $3, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_dup2
SYS_dup2 = 4

.globl dup2
dup2:
  movq $4, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_lseek
SYS_lseek = 5

.globl lseek
lseek:
  movq $5, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_close
SYS_close = 6

.globl close
close:
  movq $6, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_sync
SYS_sync = 7

.globl sync
sync:
  movq $7, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_fsync
SYS_fsync = 8

.globl fsync
fsync:
  movq $8, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_read
SYS_read = 9

.globl read
read:
  movq $9, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_pread
SYS_pread = 10

.globl pread
pread:
  movq $10, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_write
SYS_write = 11

.globl write
write:
  movq $11, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_pwrite
SYS_pwrite = 12

.globl pwrite
pwrite:
  movq $12, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_fstatx
SYS_fstatx = 13

.globl fstatx
fstatx:
  movq $13, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_link
SYS_link = 14

.globl link
link:
  movq $14, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_rename
SYS_rename = 15

.globl rename
rename:
  movq $15, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_unlink
SYS_unlink = 16

.globl unlink
unlink:
  movq $16, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_openat
SYS_openat = 17

.globl openat
openat:
  movq $17, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_mkdirat
SYS_mkdirat = 18

.globl mkdirat
mkdirat:
  movq $18, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_mknod
SYS_mknod = 19

.globl mknod
mknod:
  movq $19, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_chdir
SYS_chdir = 20

.globl chdir
chdir:
  movq $20, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_execv
SYS_execv = 21

.globl execv
execv:
  movq $21, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_pipe2
SYS_pipe2 = 22

.globl pipe2
pipe2:
  movq $22, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_pipe
SYS_pipe = 23

.globl pipe
pipe:
  movq $23, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_readdir
SYS_readdir = 24

.globl readdir
readdir:
  movq $24, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_sys_spawn
SYS_sys_spawn = 25

.globl sys_spawn
sys_spawn:
  movq $25, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_fork_flags
SYS_fork_flags = 26

.globl fork_flags
fork_flags:
  movq $26, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_exit
SYS_exit = 27

.globl exit
exit:
  movq $27, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_waitpid
SYS_waitpid = 28

.globl waitpid
waitpid:
  movq $28, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_wait
SYS_wait = 29

.globl wait
wait:
  movq $29, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_kill
SYS_kill = 30

.globl kill
kill:
  movq $30, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_getpid
SYS_getpid = 31

.globl getpid
getpid:
  movq $31, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_sbrk
SYS_sbrk = 32

.globl sbrk
sbrk:
  movq $32, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_nsleep
SYS_nsleep = 33

.globl nsleep
nsleep:
  movq $33, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_uptime
SYS_uptime = 34

.globl uptime
uptime:
  movq $34, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_mmap
SYS_mmap = 35

.globl mmap
mmap:
  movq $35, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_munmap
SYS_munmap = 36

.globl munmap
munmap:
  movq $36, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_madvise
SYS_madvise = 37

.globl madvise
madvise:
  movq $37, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_mprotect
SYS_mprotect = 38

.globl mprotect
mprotect:
  movq $38, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_pt_pages
SYS_pt_pages = 39

.globl pt_pages
pt_pages:
  movq $39, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_halt
SYS_halt = 40

.globl halt
halt:
  movq $40, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_cpuhz
SYS_cpuhz = 41

.globl cpuhz
cpuhz:
  movq $41, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_setfs
SYS_setfs = 42

.globl setfs
setfs:
  movq $42, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_setaffinity
SYS_setaffinity = 43

.globl setaffinity
setaffinity:
  movq $43, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_futex
SYS_futex = 44

.globl futex
futex:
  movq $44, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_yield
SYS_yield = 45

.globl yield
yield:
  movq $45, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_uname
SYS_uname = 46

.globl uname
uname:
  movq $46, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_dup_page
SYS_dup_page = 47

.globl dup_page
dup_page:
  movq $47, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_sigaction
SYS_sigaction = 48

.globl sigaction
sigaction:
  movq $48, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_socket
SYS_socket = 49

.globl socket
socket:
  movq $49, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_bind
SYS_bind = 50

.globl bind
bind:
  movq $50, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_listen
SYS_listen = 51

.globl listen
listen:
  movq $51, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_accept
SYS_accept = 52

.globl accept
accept:
  movq $52, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_recvfrom
SYS_recvfrom = 53

.globl recvfrom
recvfrom:
  movq $53, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_sendto
SYS_sendto = 54

.globl sendto
sendto:
  movq $54, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_connect
SYS_connect = 55

.globl connect
connect:
  movq $55, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_send
SYS_send = 56

.globl send
send:
  movq $56, %rax
  movq %rcx, %r10
  syscall
  ret

.globl SYS_recv
SYS_recv = 57

.globl recv
recv:
  movq $57, %rax
  movq %rcx, %r10
  syscall
  ret

