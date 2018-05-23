import os
import simsym
import symtypes
import errno
import model
import signal
import fs_testgen

SFn = simsym.tuninterpreted("SFn")
SInum = simsym.tuninterpreted("SInum")
SDataVal = simsym.tuninterpreted("SDataVal")
SVa = simsym.tuninterpreted("SVa")
SPipeId = simsym.tuninterpreted("SPipeId")

DATAVAL_BYTES = 4096
PAGE_BYTES = 4096
PAGE_DATAVALS = PAGE_BYTES / DATAVAL_BYTES
assert PAGE_BYTES % DATAVAL_BYTES == 0
DATA_MAX_LEN = 8

SPid = simsym.SBool
SOffset = simsym.tsynonym("SOffset", simsym.SInt)
SData = symtypes.tsmalllist(DATA_MAX_LEN, SDataVal, lenType=SOffset)
SPipe = simsym.tstruct(data = SData)
SPipeMap = symtypes.tmap(SPipeId, SPipe)
class SFd(simsym.tstruct(ispipe = simsym.SBool,
                         pipeid = SPipeId,
                         pipewriter = simsym.SBool,
                         inum = SInum,
                         # This offset is in datavals
                         off = SOffset)):
    def _declare_assumptions(self, assume):
        super(SFd, self)._declare_assumptions(assume)
        assume(self.off >= 0)

    def _eq_internal(self, o):
        if type(self) != type(o):
            return NotImplemented
        return simsym.symand(
            [self.ispipe == o.ispipe,
             simsym.symif(self.ispipe,
                          simsym.symand([self.pipeid == o.pipeid,
                                         self.pipewriter == o.pipewriter]),
                          simsym.symand([self.inum == o.inum,
                                         self.off == o.off]))])
SFdNum = simsym.tsynonym("SFdNum", simsym.SInt)
SFdMap = symtypes.tdict(SFdNum, SFd)
class SVMA(simsym.tstruct(anon = simsym.SBool,
                          writable = simsym.SBool,
                          inum = SInum,
                          # This offset is in datavals
                          off = SOffset,
                          anondata = SDataVal)):
    def _declare_assumptions(self, assume):
        super(SVMA, self)._declare_assumptions(assume)
        assume(self.off >= 0)
        assume(self.off % PAGE_DATAVALS == 0)

    def _eq_internal(self, o):
        if type(self) != type(o):
            return NotImplemented
        return simsym.symand(
            [self.anon == o.anon, self.writable == o.writable,
             simsym.symif(self.anon, self.anondata == o.anondata,
                          simsym.symand([self.off == o.off,
                                         self.inum == o.inum]))])
SVaMap = symtypes.tdict(SVa, SVMA)
SProc = symtypes.tstruct(fd_map = SFdMap,
                         va_map = SVaMap)
SDirMap = symtypes.tdict(SFn, SInum)
SNLink = simsym.tsynonym("SNLink", simsym.SInt)
STime = simsym.tsynonym("STime", simsym.SInt)
SInode = simsym.tstruct(data = SData,
                        nlink = SNLink,
                        atime = STime,
                        mtime = STime,
                        ctime = STime,
                        ## XXX Directories impl:
                        # isdir = simsym.SBool,
                        # dirmap = SDirMap,
                       )
SIMap = symtypes.tmap(SInum, SInode)
## XXX Directories impl:
# SPathname = simsym.tstruct(last = SFn)
## XXX Non-directories impl:
SPathname = SFn

class Fs(simsym.tstruct(
        # Process state
        proc0=SProc, proc1=SProc, pipes=SPipeMap,
        # In-memory file system state
        i_map=SIMap,
        root_dir=SDirMap, ## XXX Non-directories impl
        # "On-disk" file system state
        durable_root_dir=SDirMap, durable_i_map=SIMap)):

    def getproc(self, pid):
        if pid == False:
            return self.proc0
        return self.proc1

    def iused(self, inum):
        dir = SInum.var('dir')
        fn = SFn.var('fn')
        fd = SFdNum.var('fd')
        pid = SPid.var('pid')

        # If we try to simply index into dirmap, its __getitem__
        # won't have access to the supposition that it contains the right
        # key, and throw an exception.  Thus, we use _map directly.
        return simsym.symor([
            ## XXX Directories impl:
            # simsym.exists(dir,
            #     simsym.symand([
            #         self.i_map[dir].isdir,
            #         simsym.exists(fn,
            #             simsym.symand([self.i_map[dir].dirmap.contains(fn),
            #                            self.i_map[dir].dirmap._map[fn] == inum]))])),

            ## XXX Non-directories impl:
            simsym.exists(fn,
                simsym.symand([self.root_dir.contains(fn),
                               self.root_dir._map[fn] == inum])),

            simsym.exists(fd,
                simsym.symand([self.proc0.fd_map.contains(fd),
                               simsym.symnot(self.proc0.fd_map._map[fd].ispipe),
                               self.proc0.fd_map._map[fd].inum == inum])),

            simsym.exists(fd,
                simsym.symand([self.proc1.fd_map.contains(fd),
                               simsym.symnot(self.proc1.fd_map._map[fd].ispipe),
                               self.proc1.fd_map._map[fd].inum == inum])),
            ])

    def add_selfpid(self, pid):
        ## XXX hack due to our simplified PID model
        ## without loss of generality, assume syscall "a" happens in proc0
        if str(pid).startswith('a.'):
            simsym.assume(pid == False)

    def nameiparent(self, pn):
        ## XXX Non-directories impl:
        return 0, self.root_dir, pn

        ## XXX Directories impl:
        # simsym.assume(self.i_map[self.root_inum].isdir)
        # return self.root_inum, self.i_map[self.root_inum].dirmap, pn.last

    # Each model method must return a dictionary representing its the
    # return value of the implemented call.  The keys in these
    # dictionaries correspond to variables declared in the code
    # generated by fs_testgen, with one exception: in order for
    # spec.test to be able to correctly compare these dictionaries,
    # they must be well-typed.  So if, for example, a return value can
    # either be a literal -1 or some symbolic value, we name the
    # latter something like 'r:data' and handle it specially in
    # fs_testgen.

    @model.methodwrap(pn=SPathname,
                      creat=simsym.SBool,
                      excl=simsym.SBool,
                      trunc=simsym.SBool,
                      anyfd=simsym.SBool,
                      pid=SPid,
                     )
    def open(self, pn, creat, excl, trunc, anyfd, pid):
        # XXX O_RDONLY, O_WRONLY, O_RDWR
        self.add_selfpid(pid)
        internal_time = STime.var('internal_time*')
        created = False
        anyfd = False
        _, pndirmap, pnlast = self.nameiparent(pn)
        if creat:
            if not pndirmap.contains(pnlast):
                internal_alloc_inum = SInum.var('internal_alloc_inum*')
                simsym.assume(simsym.symnot(self.iused(internal_alloc_inum)))

                simsym.assume(internal_time >= self.i_map[internal_alloc_inum].atime)
                simsym.assume(internal_time >= self.i_map[internal_alloc_inum].mtime)
                simsym.assume(internal_time >= self.i_map[internal_alloc_inum].ctime)

                inode = self.i_map[internal_alloc_inum]
                inode.data._len = 0
                inode.nlink = 1
                inode.atime = inode.mtime = inode.ctime = internal_time
                pndirmap[pnlast] = internal_alloc_inum

                created = True
            else:
                if excl: return {'r': -1, 'errno': errno.EEXIST}
        if not pndirmap.contains(pnlast):
            return {'r': -1, 'errno': errno.ENOENT}

        inum = pndirmap[pnlast]
        if trunc:
            if not created:
                simsym.assume(internal_time >= self.i_map[inum].mtime)
                simsym.assume(internal_time >= self.i_map[inum].ctime)
                self.i_map[inum].mtime = internal_time
                self.i_map[inum].ctime = internal_time
            self.i_map[inum].data._len = 0

        internal_ret_fd = SFdNum.var('internal_ret_fd*')
        simsym.assume(internal_ret_fd >= 0)
        simsym.assume(simsym.symnot(self.getproc(pid).fd_map.contains(internal_ret_fd)))

        ## Lowest FD
        otherfd = SFdNum.var('fd')
        simsym.assume(simsym.symor([anyfd,
            simsym.symnot(simsym.exists(otherfd,
                simsym.symand([otherfd >= 0,
                               otherfd < internal_ret_fd,
                               self.getproc(pid).fd_map.contains(otherfd)])))]))

        fd_data = self.getproc(pid).fd_map.create(internal_ret_fd)
        fd_data.inum = inum
        fd_data.off = 0
        fd_data.ispipe = False

        return {'r': internal_ret_fd}

    @model.methodwrap(pid=SPid)
    def pipe(self, pid):
        self.add_selfpid(pid)
        internal_pipeid = SPipeId.var('internal_pipeid*')

        xfd = SFdNum.var('xfd')
        simsym.assume(simsym.symnot(simsym.symor([
            simsym.exists(xfd,
                simsym.symand([self.proc0.fd_map.contains(xfd),
                               self.proc0.fd_map._map[xfd].ispipe,
                               self.proc0.fd_map._map[xfd].pipeid == internal_pipeid])),
            simsym.exists(xfd,
                simsym.symand([self.proc1.fd_map.contains(xfd),
                               self.proc1.fd_map._map[xfd].ispipe,
                               self.proc1.fd_map._map[xfd].pipeid == internal_pipeid]))])))

        empty_pipe = self.pipes[internal_pipeid]
        empty_pipe.data._len = 0

        ## lowest FD for read end
        internal_fd_r = SFdNum.var('internal_fd_r*')
        simsym.assume(internal_fd_r >= 0)
        simsym.assume(simsym.symnot(self.getproc(pid).fd_map.contains(internal_fd_r)))
        simsym.assume(simsym.symnot(simsym.exists(xfd,
                simsym.symand([xfd >= 0,
                               xfd < internal_fd_r,
                               self.getproc(pid).fd_map.contains(xfd)]))))
        fd_r_data = self.getproc(pid).fd_map.create(internal_fd_r)
        fd_r_data.ispipe = True
        fd_r_data.pipeid = internal_pipeid
        fd_r_data.pipewriter = False

        ## lowest FD for write end
        internal_fd_w = SFdNum.var('internal_fd_w*')
        simsym.assume(internal_fd_w >= 0)
        simsym.assume(simsym.symnot(self.getproc(pid).fd_map.contains(internal_fd_w)))
        simsym.assume(simsym.symnot(simsym.exists(xfd,
                simsym.symand([xfd >= 0,
                               xfd < internal_fd_w,
                               self.getproc(pid).fd_map.contains(xfd)]))))
        fd_w_data = self.getproc(pid).fd_map.create(internal_fd_w)
        fd_w_data.ispipe = True
        fd_w_data.pipeid = internal_pipeid
        fd_w_data.pipewriter = True

        return {'r': 0, 'fds[0]': internal_fd_r, 'fds[1]': internal_fd_w}

    @model.methodwrap(src=SPathname, dst=SPathname)
    def rename(self, src, dst):
        internal_time = STime.var('internal_time*')
        srcdiri, srcdirmap, srclast = self.nameiparent(src)
        dstdiri, dstdirmap, dstlast = self.nameiparent(dst)
        if not srcdirmap.contains(srclast):
            return {'r': -1, 'errno': errno.ENOENT}
        if srcdiri == dstdiri and srclast == dstlast:
            return {'r': 0}
        if dstdirmap.contains(dstlast):
            dstinum = dstdirmap[dstlast]
        else:
            dstinum = None
        dstdirmap[dstlast] = srcdirmap[srclast]
        del srcdirmap[srclast]
        if dstinum is not None:
            self.i_map[dstinum].nlink = self.i_map[dstinum].nlink - 1
            simsym.assume(internal_time >= self.i_map[dstinum].ctime)
            self.i_map[dstinum].ctime = internal_time
        return {'r': 0}

    @model.methodwrap(pn=SPathname)
    def unlink(self, pn):
        internal_time = STime.var('internal_time*')
        _, dirmap, pnlast = self.nameiparent(pn)
        if not dirmap.contains(pnlast):
            return {'r': -1, 'errno': errno.ENOENT}
        inum = dirmap[pnlast]
        del dirmap[pnlast]
        self.i_map[inum].nlink = self.i_map[inum].nlink - 1
        simsym.assume(internal_time >= self.i_map[inum].ctime)
        self.i_map[inum].ctime = internal_time
        return {'r': 0}

    @model.methodwrap(oldpn=SPathname, newpn=SPathname)
    def link(self, oldpn, newpn):
        internal_time = STime.var('internal_time*')
        olddiri, olddirmap, oldlast = self.nameiparent(oldpn)
        newdiri, newdirmap, newlast = self.nameiparent(newpn)
        if not olddirmap.contains(oldlast):
            return {'r': -1, 'errno': errno.ENOENT}
        if newdirmap.contains(newlast):
            return {'r': -1, 'errno': errno.EEXIST}
        inum = olddirmap[oldlast]
        newdirmap[newlast] = inum
        self.i_map[inum].nlink = self.i_map[inum].nlink + 1
        simsym.assume(internal_time >= self.i_map[inum].ctime)
        self.i_map[inum].ctime = internal_time
        return {'r': 0}

    def iread(self, inum, off, time=None):
        simsym.assume(off >= 0)
        if off >= self.i_map[inum].data._len:
            return {'r': 0}
        if time is None:
            time = STime.var('internal_time*')
        if time is not False:
            simsym.assume(time >= self.i_map[inum].atime)
            self.i_map[inum].atime = time
        return {'r': DATAVAL_BYTES, 'data': self.i_map[inum].data[off]}

    @model.methodwrap(fd=SFdNum, pid=SPid)
    def read(self, fd, pid):
        self.add_selfpid(pid)
        if not self.getproc(pid).fd_map.contains(fd):
            return {'r': -1, 'errno': errno.EBADF}
        if self.getproc(pid).fd_map[fd].ispipe:
            if self.getproc(pid).fd_map[fd].pipewriter:
                return {'r': -1, 'errno': errno.EBADF}
            pipeid = self.getproc(pid).fd_map[fd].pipeid
            pipe = self.pipes[pipeid]
            if pipe.data.len() == 0:
                otherfd = SFdNum.var('otherfd')
                if simsym.symor([
                    simsym.exists(otherfd,
                        simsym.symand([self.proc0.fd_map.contains(otherfd),
                                       self.proc0.fd_map._map[otherfd].ispipe,
                                       self.proc0.fd_map._map[otherfd].pipewriter,
                                       self.proc0.fd_map._map[otherfd].pipeid == pipeid])),
                    simsym.exists(otherfd,
                        simsym.symand([self.proc1.fd_map.contains(otherfd),
                                       self.proc1.fd_map._map[otherfd].ispipe,
                                       self.proc1.fd_map._map[otherfd].pipewriter,
                                       self.proc1.fd_map._map[otherfd].pipeid == pipeid]))]):
                    return {'r': -1, 'errno': errno.EAGAIN}
                else:
                    # XXX The above condition can always be satisfied
                    # by making up some other FD, but testgen may
                    # never see that FD, so the real test may not
                    # reflect its presence.
                    return {'r': 0}
            d = pipe.data[0]
            pipe.data.shift()
            return {'r': DATAVAL_BYTES, 'data': d}
        off = self.getproc(pid).fd_map[fd].off
        r = self.iread(self.getproc(pid).fd_map[fd].inum, off)
        if 'data' in r:
            self.getproc(pid).fd_map[fd].off = off + 1
        return r

    @model.methodwrap(fd=SFdNum, off=SOffset, pid=SPid)
    def pread(self, fd, off, pid):
        self.add_selfpid(pid)
        if not self.getproc(pid).fd_map.contains(fd):
            return {'r': -1, 'errno': errno.EBADF}
        if self.getproc(pid).fd_map[fd].ispipe:
            return {'r': -1, 'errno': errno.ESPIPE}
        return self.iread(self.getproc(pid).fd_map[fd].inum, off)

    def iwrite(self, inum, off, databyte, time=None):
        simsym.assume(off >= 0)
        ## Avoid overly-long files.  fs-test.py caps file size at 16 units.
        simsym.assume(off < DATA_MAX_LEN)
        ## XXX Handle sparse files?
        simsym.assume(off <= self.i_map[inum].data._len)

        if off == self.i_map[inum].data._len:
            self.i_map[inum].data.append(databyte)
        else:
            self.i_map[inum].data[off] = databyte
        if time is None:
            time = STime.var('internal_time*')
        if time is not False:
            simsym.assume(time >= self.i_map[inum].mtime)
            simsym.assume(time >= self.i_map[inum].ctime)
            self.i_map[inum].mtime = time
            self.i_map[inum].ctime = time
        return {'r': DATAVAL_BYTES}

    @model.methodwrap(fd=SFdNum, databyte=SDataVal, pid=SPid)
    def write(self, fd, databyte, pid):
        self.add_selfpid(pid)
        if not self.getproc(pid).fd_map.contains(fd):
            return {'r': -1, 'errno': errno.EBADF}
        if self.getproc(pid).fd_map[fd].ispipe:
            if not self.getproc(pid).fd_map[fd].pipewriter:
                return {'r': -1, 'errno': errno.EBADF}
            pipeid = self.getproc(pid).fd_map[fd].pipeid
            pipe = self.pipes[pipeid]

            otherfd = SFdNum.var('otherfd')
            if simsym.symnot(simsym.symor([
                simsym.exists(otherfd,
                    simsym.symand([self.proc0.fd_map.contains(otherfd),
                                   self.proc0.fd_map._map[otherfd].ispipe,
                                   simsym.symnot(self.proc0.fd_map._map[otherfd].pipewriter),
                                   self.proc0.fd_map._map[otherfd].pipeid == pipeid])),
                simsym.exists(otherfd,
                    simsym.symand([self.proc1.fd_map.contains(otherfd),
                                   self.proc1.fd_map._map[otherfd].ispipe,
                                   simsym.symnot(self.proc1.fd_map._map[otherfd].pipewriter),
                                   self.proc1.fd_map._map[otherfd].pipeid == pipeid]))])):
                # XXX This condition has the same problem as the one
                # in read.
                return {'r': -1, 'errno': errno.EPIPE}

            simsym.assume(pipe.data.len() < DATA_MAX_LEN)
            pipe.data.append(databyte)
            return {'r': DATAVAL_BYTES}
        off = self.getproc(pid).fd_map[fd].off
        self.getproc(pid).fd_map[fd].off = off + 1
        return self.iwrite(self.getproc(pid).fd_map[fd].inum, off, databyte)

    @model.methodwrap(fd=SFdNum, off=SOffset, databyte=SDataVal, pid=SPid)
    def pwrite(self, fd, off, databyte, pid):
        self.add_selfpid(pid)
        if not self.getproc(pid).fd_map.contains(fd):
            return {'r': -1, 'errno': errno.EBADF}
        if self.getproc(pid).fd_map[fd].ispipe:
            return {'r': -1, 'errno': errno.ESPIPE}
        return self.iwrite(self.getproc(pid).fd_map[fd].inum, off, databyte)

    def istat(self, inum):
        len = self.i_map[inum].data._len
        nlink = self.i_map[inum].nlink
        atime = self.i_map[inum].atime
        mtime = self.i_map[inum].mtime
        ctime = self.i_map[inum].ctime
        return {'r': 0, 'st.st_ino': inum, 'st.st_size': len,
                'st.st_nlink': nlink, 'st.st_atime': atime,
                'st.st_mtime': mtime, 'st.st_ctime': ctime}

    @model.methodwrap(pn=SPathname)
    def stat(self, pn):
        _, dirmap, pnlast = self.nameiparent(pn)
        if not dirmap.contains(pnlast):
            return {'r': -1, 'errno': errno.ENOENT}
        return self.istat(dirmap[pnlast])

    @model.methodwrap(fd=SFdNum, pid=SPid)
    def fstat(self, fd, pid):
        self.add_selfpid(pid)
        if not self.getproc(pid).fd_map.contains(fd):
            return {'r': -1, 'errno': errno.EBADF}
        if self.getproc(pid).fd_map[fd].ispipe:
            return {'r': 0, '!!S_ISFIFO(st.st_mode)': 1}
        return self.istat(self.getproc(pid).fd_map[fd].inum)

    @model.methodwrap(fd=SFdNum, pid=SPid)
    def close(self, fd, pid):
        self.add_selfpid(pid)
        if not self.getproc(pid).fd_map.contains(fd):
            return {'r': -1, 'errno': errno.EBADF}
        del self.getproc(pid).fd_map[fd]
        return {'r': 0}

    @model.methodwrap(fd=SFdNum, off=SOffset,
                      whence_set=simsym.SBool,
                      whence_cur=simsym.SBool,
                      whence_end=simsym.SBool,
                      pid=SPid)
    def lseek(self, fd, off, whence_set, whence_cur, whence_end, pid):
        self.add_selfpid(pid)
        if not self.getproc(pid).fd_map.contains(fd):
            return {'r': -1, 'errno': errno.EBADF}
        fdm = self.getproc(pid).fd_map[fd]
        if fdm.ispipe:
            return {'r': -1, 'errno': errno.EINVAL}
            ## POSIX says ESPIPE, but Linux returns EINVAL
        if whence_set:
            new_off = off
        elif whence_cur:
            new_off = fdm.off + off
        elif whence_end:
            new_off = self.i_map[fdm.inum].data._len + off
        else:
            return {'r': -1, 'errno': errno.EINVAL}
        if new_off < 0:
            return {'r': -1, 'errno': errno.EINVAL}
        fdm.off = new_off
        return {'r': fdm.off}

    @model.methodwrap(anon=simsym.SBool,
                      writable=simsym.SBool,
                      fixed=simsym.SBool,
                      va=SVa,
                      fd=SFdNum,
                      off=SOffset,
                      pid=SPid)
    def mmap(self, anon, writable, fixed, va, fd, off, pid):
        ## TODO: MAP_SHARED/MAP_PRIVATE for files
        ##       -> how to model delayed file read?
        ## TODO: MAP_SHARED/MAP_PRIVATE for anon (with fork)
        ## TODO: zeroing anon memory
        self.add_selfpid(pid)
        myproc = self.getproc(pid)
        if not fixed:
            va = SVa.var('internal_freeva*')
            simsym.assume(simsym.symnot(myproc.va_map.contains(va)))
        if not anon:
            if not myproc.fd_map.contains(fd):
                return {'r': -1, 'errno': errno.EBADF}
            if myproc.fd_map[fd].ispipe:
                # The Linux manpage is misleading, but POSIX is clear
                # about ENODEV and this is what Linux does.
                return {'r': -1, 'errno': errno.ENODEV}
        vma = myproc.va_map.create(va)
        vma.anon = anon
        vma.writable = writable
        if anon:
            vma.anondata = SDataVal.var()
        else:
            simsym.assume(off >= 0)
            simsym.assume(off % PAGE_DATAVALS == 0)
            vma.off = off
            vma.inum = myproc.fd_map[fd].inum
        # This has to be well-typed, so we use a different variable to
        # represent VAs.
        return {'r:va': va}

    @model.methodwrap(va=SVa, pid=SPid)
    def munmap(self, va, pid):
        self.add_selfpid(pid)
        del self.getproc(pid).va_map[va]
        return {'r': 0}

    @model.methodwrap(va=SVa, writable=simsym.SBool, pid=SPid)
    def mprotect(self, va, writable, pid):
        self.add_selfpid(pid)
        myproc = self.getproc(pid)
        if not myproc.va_map.contains(va):
            return {'r': -1, 'errno': errno.ENOMEM}
        myproc.va_map[va].writable = writable
        return {'r': 0}

    @model.methodwrap(va=SVa, pid=SPid)
    def memread(self, va, pid):
        self.add_selfpid(pid)
        myproc = self.getproc(pid)
        if not myproc.va_map.contains(va):
            return {'r': -1, 'signal': signal.SIGSEGV}
        if myproc.va_map[va].anon:
            return {'r:data': myproc.va_map[va].anondata, 'signal': 0}
        ## TODO: memory-mapped reads don't bump atime?
        res = self.iread(myproc.va_map[va].inum,
                         myproc.va_map[va].off,
                         False)
        if res['r'] == 0:
            # This means there was no page here
            return {'r': -1, 'signal': signal.SIGBUS}
        elif res['r'] == DATAVAL_BYTES:
            return {'r:data': res['data'], 'signal': 0}
        else:
            raise RuntimeError('Unexpected result from iread: %r' % res)

    @model.methodwrap(va=SVa, databyte=SDataVal, pid=SPid)
    def memwrite(self, va, databyte, pid):
        self.add_selfpid(pid)
        myproc = self.getproc(pid)
        if not myproc.va_map.contains(va):
            return {'r': -1, 'signal': signal.SIGSEGV}
        if not myproc.va_map[va].writable:
            return {'r': -1, 'signal': signal.SIGSEGV}
        if myproc.va_map[va].anon:
            myproc.va_map[va].anondata = databyte
            return {'r': 0, 'signal': 0}
        vma = myproc.va_map[va]
        if vma.off >= self.i_map[vma.inum].data._len:
            return {'r': -1, 'signal': signal.SIGBUS}
        ## TODO: memory-mapped writes don't bump mtime/ctime?
        res = self.iwrite(myproc.va_map[va].inum,
                          myproc.va_map[va].off,
                          databyte, False)
        if res['r'] == DATAVAL_BYTES:
            return {'r': 0, 'signal': 0}
        else:
            raise RuntimeError('Unexpected result from iwrite: %r' % res)

    @model.methodwrap()
    def sync(self):
        # Update the "on-disk" state to match the "in-memory" file
        # system state.  If only the real sync were this easy!
        self.durable_i_map = self.i_map
        self.durable_root_dir = self.root_dir
        return {'r': 0}

    @model.methodwrap(fd=SFdNum, pid=SPid)
    def fsync(self, fd, pid):
        self.add_selfpid(pid)
        if not self.getproc(pid).fd_map.contains(fd):
            return {'r': -1, 'errno': errno.EBADF}
        inum = self.getproc(pid).fd_map[fd].inum
        self.durable_i_map[inum] = self.i_map[inum]
        return {'r': 0}

    @model.methodwrap()
    def reboot(self):
        # Undo "in-memory" file system state back to "durable" state
        self.i_map = self.durable_i_map
        self.root_dir = self.durable_root_dir
        # XXX We should probably do something with the process state.
        # We can't reset it to "nothing" because we don't have a way
        # to represent nothing.  We could de-constrain it as below,
        # but I'm not sure that's right either (and it sends Z3 into
        # an infinite loop).
        # self.proc0 = SProc.var()
        # self.proc1 = SProc.var()
        # self.pipes = SPipeMap.var()
        return {'r': 0}

model_class = Fs
model_testgen = fs_testgen.FsTestGenerator
