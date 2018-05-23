#include <stdint.h>
#include <string.h>

#include <fcntl.h>
#include <getopt.h>

#include <map>
#include <list>
#include <iostream>
#include <fstream>

extern "C" {
#include <mtrace-magic.h>
#include "util.h"
}

#include "addr2line.hh"
#include "demangle.hh"
#include "mscan.hh"
#include "calltrace.hh"
#include "dissys.hh"
#include "sysaccess.hh"
#include "false.hh"
#include "asharing.hh"
#include "argparse.hh"
#include "addrs.hh"
#include "checktest.hh"
#include "allsharing.hh"
#include "cacheassoc.hh"
#include "sbw0.hh"
#include "checkgc.hh"
#include "serlen.hh"

#include "bininfo.hh"
#include <elf++.hh>
#include <dwarf++.hh>

using namespace::std;

typedef map<uint64_t, struct mtrace_label_entry> LabelMap;

// A bunch of global state the default handlers update
struct mtrace_host_entry mtrace_first;
struct mtrace_host_entry mtrace_enable;
Addr2line* addr2line;
MtraceSummary mtrace_summary;
pc_t mtrace_call_pc[MAX_CPUS];
tid_t mtrace_tid[MAX_CPUS];
MtraceAddr2label mtrace_label_map;
uint64_t mtrace_object_count;
CallTrace* mtrace_call_trace;
dwarf::dwarf mtrace_dwarf;
elf::elf mtrace_elf;

static LabelMap labels;
static list<struct mtrace_label_entry> percpu_labels;

static struct MtraceOptions {
    string      elf_file;
    string      log_file;
    set<pc_t>   stack_trace_pc;
    bool        syscall_accesses;
    bool        syscall_accesses_pc;
    bool        false_sharing;
    bool        distinct_ops;
    bool        distinct_sys;
    bool        abstract_scopes;
    bool        unexpected_sharing;
    bool        summary;
    bool        shared_addresses;
    bool        check_testcases;
    bool        all_sharing;
    bool        cache_assoc;
    bool        sbw0;
    bool        ser_len;
    bool        check_gc;

    MtraceOptions() : elf_file("mscan.kern"), log_file("mtrace.out") {}
} mtrace_options;

class DefaultHostHandler : public EntryHandler {
public:
    virtual void handle(const union mtrace_entry* entry) {
        const struct mtrace_host_entry* e = &entry->host;
        if (e->host_type == mtrace_call_clear_cpu ||
            e->host_type == mtrace_call_set_cpu ||
            e->host_type == mtrace_disable_count_cpu ||
            e->host_type == mtrace_enable_count_cpu) {
            return;
        } else if (e->host_type != mtrace_access_all_cpu)
            die("handle_host: unhandled type %u", e->host_type);

        if (!mtrace_summary.app_name[0])
            strncpy(mtrace_summary.app_name, e->access.str,
                    sizeof(mtrace_summary.app_name));

        if (mtrace_first.h.ts == 0)
            mtrace_first = *e;
        mtrace_enable = *e;
    }
};

class DefaultAppDataHandler : public EntryHandler {
public:
    virtual void handle(const union mtrace_entry* entry) {
        const struct mtrace_appdata_entry* a = &entry->appdata;
        mtrace_summary.app_ops = a->u64;
    }
};

class DefaultMachineHandler : public EntryHandler {
    virtual void handle(const union mtrace_entry* entry) {
        const struct mtrace_machine_entry* m = &entry->machine;
        mtrace_summary.num_cpus = m->num_cpus;
        mtrace_summary.num_ram = m->num_ram;
    }
};

class DefaultFcallHandler : public EntryHandler {
public:
    virtual void handle(const union mtrace_entry* entry) {
        const struct mtrace_fcall_entry* f = &entry->fcall;
        int cpu = f->h.cpu;

        switch (f->state) {
        case mtrace_resume:
            mtrace_call_pc[cpu] = f->pc;
            mtrace_tid[cpu] = f->tid;
            break;
        case mtrace_start:
            mtrace_call_pc[cpu] = f->pc;
            mtrace_tid[cpu] = f->tid;
            break;
        case mtrace_pause:
            mtrace_call_pc[cpu] = 0;
            mtrace_tid[cpu] = 0;
            break;
        case mtrace_done_value:
        case mtrace_done:
            mtrace_call_pc[cpu] = 0;
            mtrace_tid[cpu] = 0;
            break;
        default:
            die("DefaultFcallHandler::handle: default error");
        }
    }
};

class DefaultLabelHandler : public EntryHandler {
public:
    virtual void handle(const union mtrace_entry* entry) {
        const struct mtrace_label_entry* l = &entry->label;

        if (l->label_type == 0 || l->label_type >= mtrace_label_end)
            die("DefaultLabelHandler::handle: bad label type: %u", l->label_type);

        if (l->bytes)
            mtrace_label_map.add_label(l);
        else
            mtrace_label_map.rem_label(l);
    }
};

class DefaultSegmentHandler : public EntryHandler {
public:
    virtual void handle(const union mtrace_entry* entry) {
        const struct mtrace_segment_entry* s = &entry->seg;

        if (s->object_type != mtrace_label_percpu)
            die("DefaultSegmentHandler::handle: bad type %u", s->object_type);

        auto it = percpu_labels.begin();
        for (; it != percpu_labels.end(); ++it) {
            struct mtrace_label_entry offset = *it;
            struct mtrace_label_entry l;

            memcpy(&l, &offset, sizeof(l));
            l.guest_addr = s->baseaddr + offset.guest_addr;
            l.label_type = mtrace_label_percpu;

            if (l.guest_addr + l.bytes > s->endaddr)
                die("DefaultSegmentHandler::handle: bad label %s", l.str);

            mtrace_label_map.add_label(&l);
        }

        // XXX Oops, we leak the mtrace_label_entry in percpu_labels after
        // we handle the final segment.
    }
};

class DefaultSummary : public EntryHandler {
public:
    virtual void exit(JsonDict* json_file) {
        JsonDict* dict = JsonDict::create();
        
        dict->put("total-instructions", total_instructions());
        json_file->put("summary", dict);
    }
};

static list<EntryHandler*> entry_handler[mtrace_entry_num];
static list<EntryHandler*> exit_handler;

static inline union mtrace_entry* alloc_entry(void) {
    return (union mtrace_entry*)malloc(sizeof(union mtrace_entry));
}

static inline void free_entry(void* entry)
{
    free(entry);
}

static inline void init_entry_alloc(void)
{
    // nothing
}

static void process_log(gzFile log)
{
    union mtrace_entry entry;
    int r;

    fflush(0);
    while ((r = read_entry(log, &entry)) > 0) {
        list<EntryHandler*> *l = &entry_handler[entry.h.type];
        list<EntryHandler*>::iterator it = l->begin();
        for (; it != l->end(); ++it)
            (*it)->handle(&entry);
    }

    JsonDict* json_dict = JsonDict::create();
    json_dict->write_to(&cout, 0, nullptr);

    list<EntryHandler*>::iterator it = exit_handler.begin();
    for (; it != exit_handler.end(); ++it)
        (*it)->exit(json_dict);

    json_dict->done();
    cout << "\n";
}

static void init_handlers(void)
{
    //
    // The default handler come first
    //
    entry_handler[mtrace_entry_host].push_front(new DefaultHostHandler());
    entry_handler[mtrace_entry_appdata].push_front(new DefaultAppDataHandler());
    entry_handler[mtrace_entry_fcall].push_front(new DefaultFcallHandler());
    entry_handler[mtrace_entry_label].push_front(new DefaultLabelHandler());
    entry_handler[mtrace_entry_segment].push_front(new DefaultSegmentHandler());
    entry_handler[mtrace_entry_machine].push_front(new DefaultMachineHandler());
    CallTrace* call_trace = new CallTrace();
    mtrace_call_trace = call_trace;
    entry_handler[mtrace_entry_call].push_back(call_trace);
    entry_handler[mtrace_entry_fcall].push_back(call_trace);

    //
    // Extra handlers come next
    //
    if (mtrace_options.distinct_sys) {
        DistinctSyscalls* dissys = new DistinctSyscalls();
        entry_handler[mtrace_entry_access].push_back(dissys);
        entry_handler[mtrace_entry_fcall].push_back(dissys);
        exit_handler.push_back(dissys);

        if (mtrace_options.distinct_ops) {
            DistinctOps* disops = new DistinctOps(dissys);
            exit_handler.push_back(disops);
        }
    }

    if (mtrace_options.false_sharing) {
        FalseSharing* false_sharing = new FalseSharing();
        entry_handler[mtrace_entry_access].push_back(false_sharing);
        exit_handler.push_back(false_sharing);
    }

    if (!mtrace_options.stack_trace_pc.empty()) {
        CallTraceFilter* call_trace_filter = new CallTraceFilter(mtrace_options.stack_trace_pc);
        entry_handler[mtrace_entry_access].push_back(call_trace_filter);
        exit_handler.push_back(call_trace_filter);
    }

    if (mtrace_options.syscall_accesses) {
        SyscallAccesses* sysaccesses = new SyscallAccesses();
        entry_handler[mtrace_entry_access].push_back(sysaccesses);
        exit_handler.push_back(sysaccesses);
        
        if (mtrace_options.syscall_accesses_pc) {
            SyscallAccessesPC* sys_accesses_pc = new SyscallAccessesPC(sysaccesses);
            exit_handler.push_back(sys_accesses_pc);
        }
    }

    if (mtrace_options.abstract_scopes ||
        mtrace_options.unexpected_sharing) {
        AbstractSharing *ashare = new AbstractSharing(mtrace_options.abstract_scopes,
                                                      mtrace_options.unexpected_sharing);
        entry_handler[mtrace_entry_ascope].push_back(ashare);
        entry_handler[mtrace_entry_avar].push_back(ashare);
        entry_handler[mtrace_entry_access].push_back(ashare);
        entry_handler[mtrace_entry_fcall].push_back(ashare);
        exit_handler.push_back(ashare);
    }

    if (mtrace_options.summary)
        exit_handler.push_back(new DefaultSummary());

    if (mtrace_options.shared_addresses) {
        SharedAddresses* addrs = new SharedAddresses();
        entry_handler[mtrace_entry_access].push_back(addrs);
        exit_handler.push_back(addrs);
    }

    if (mtrace_options.check_testcases) {
        CheckTestcases* ct = new CheckTestcases();
        entry_handler[mtrace_entry_host].push_back(ct);
        entry_handler[mtrace_entry_ascope].push_back(ct);
        entry_handler[mtrace_entry_access].push_back(ct);
        exit_handler.push_back(ct);
    }

    if (mtrace_options.all_sharing) {
        AllSharing* as = new AllSharing();
        entry_handler[mtrace_entry_host].push_back(as);
        entry_handler[mtrace_entry_access].push_back(as);
        exit_handler.push_back(as);
    }

    if (mtrace_options.cache_assoc) {
        CacheAssoc* ca = new CacheAssoc();
        entry_handler[mtrace_entry_machine].push_back(ca);
        entry_handler[mtrace_entry_host].push_back(ca);
        entry_handler[mtrace_entry_access].push_back(ca);
        exit_handler.push_back(ca);
    }

    if (mtrace_options.sbw0) {
        SBW0* sbw0 = new SBW0();
        entry_handler[mtrace_entry_access].push_back(sbw0);
        entry_handler[mtrace_entry_fcall].push_back(sbw0);
        exit_handler.push_back(sbw0);
    }

    if (mtrace_options.check_gc) {
        CheckGC* cg = new CheckGC();
        entry_handler[mtrace_entry_host].push_back(cg);
        entry_handler[mtrace_entry_access].push_back(cg);
        entry_handler[mtrace_entry_gc].push_back(cg);
        entry_handler[mtrace_entry_gcepoch].push_back(cg);
        exit_handler.push_back(cg);
    }

    if (mtrace_options.ser_len) {
        SerLen* serlen = new SerLen();
        entry_handler[mtrace_entry_lock].push_back(serlen);
        exit_handler.push_back(serlen);
    }
}

static void init_static_syms(const elf::elf &elf)
{
    list<struct mtrace_label_entry> tmp;
    uint64_t percpu_start = 0;
    uint64_t percpu_end = 0;

    for (auto &sec : elf.sections()) {
        if (sec.get_hdr().type != elf::sht::symtab)
            continue;

        elf::symtab syms = sec.as_symtab();

        for (auto sym : syms) {
            auto &data = sym.get_data();

            if ((data.binding() != elf::stb::global &&
                 data.binding() != elf::stb::local) ||
                data.type() != elf::stt::object)
                continue;

            const char *name = sym.get_name(nullptr);
            if (strcmp("__per_cpu_end", name) == 0) {
                percpu_end = data.value;
                continue;
            } else if (strcmp("__per_cpu_start", name) == 0){
                percpu_start = data.value;
                continue;
            } else if (data.size == 0) {
                continue;
            }

            // XXX Get symbol type information from DWARF

            struct mtrace_label_entry l;
            l.h.type = mtrace_entry_label;
            l.h.access_count = 0;
            l.label_type = mtrace_label_static;
            strncpy(l.str, demangle(name).c_str(), sizeof(l.str) - 1);
            l.str[sizeof(l.str) - 1] = 0;
            l.host_addr = 0;
            l.guest_addr = data.value;
            l.bytes = data.size;
            l.pc = 0;

            tmp.push_back(l);
        }
    }

    // Move all the labels for percpu variables from the mtrace_label_static
    // list to the temporary list.  Once we know each CPUs percpu base
    // address we add percpu objects onto the mtrace_label_percpu list.
    while (tmp.size()) {
        auto it = tmp.begin();
        struct mtrace_label_entry l = *it;

        if (percpu_start <= l.guest_addr && l.guest_addr < percpu_end)
            percpu_labels.push_back(l);
        else
            mtrace_label_map.add_label(&l);

        tmp.erase(it);
    }
}

static void handle_arg(const ArgParse* parser, string option, string val)
{
    if (option == "kernel") {
        mtrace_options.elf_file = val;
    } else if (option == "mtrace-log-file") {
        mtrace_options.log_file = val;
    } else if (option == "stack-trace-pc") {
        uint64_t x;
        stringstream ss;

        ss << hex << val;
        ss >> x;
        mtrace_options.stack_trace_pc.insert(x);
    } else if (option == "syscall-accesses") {
        mtrace_options.syscall_accesses = true;
    } else if (option == "syscall-accesses-pc") {
        mtrace_options.syscall_accesses = true;
        mtrace_options.syscall_accesses_pc = true;        
    } else if (option == "false-sharing")  {
        mtrace_options.false_sharing = true;
    } else if (option == "distinct-ops") {
        mtrace_options.distinct_ops = true;
        mtrace_options.distinct_sys = true;
    } else if (option == "distinct-sys") {
        mtrace_options.distinct_sys = true;
    } else if (option == "summary") {
        mtrace_options.summary = true;
    } else if (option == "abstract-scopes") {
        mtrace_options.abstract_scopes = true;
    } else if (option == "unexpected-sharing") {
        mtrace_options.unexpected_sharing = true;
    } else if (option == "shared-addresses") {
        mtrace_options.shared_addresses = true;
    } else if (option == "check-testcases") {
        mtrace_options.check_testcases = true;
    } else if (option == "all-sharing") {
        mtrace_options.all_sharing = true;
    } else if (option == "cache-assoc") {
        mtrace_options.cache_assoc = true;
    } else if (option == "sbw0") {
        mtrace_options.sbw0 = true;
    } else if (option == "check-gc") {
        mtrace_options.check_gc = true;
    } else if (option == "serial-length") {
        mtrace_options.ser_len = true;
    } else {
        die("handle_arg: unexpected");
    }
}

int main(int ac, char** av)
{
    gzFile log;

    ArgParse parse(ac, av);
    parse.add_option("kernel", "FILE",
                     "ELF file of kernel (default mscan.kern)");
    parse.add_option("mtrace-log-file", "FILE",
                     "mtrace.out file name");
    parse.add_option("stack-trace-pc", "PC",
                     "Stack traces for access at PC");
    parse.add_option("syscall-accesses",
                     "Every access, organized by syscall");
    parse.add_option("syscall-accesses-pc",
                     "The PC of every access, organized by syscall");
    parse.add_option("false-sharing",
                     "False sharing");
    parse.add_option("distinct-ops",
                     "Average distinct cache lines per operation");
    parse.add_option("distinct-sys",
                     "Average distinct cache lines per syscall");
    parse.add_option("abstract-scopes",
                     "Abstract sharing scopes");
    parse.add_option("unexpected-sharing",
                     "Unexpected abstract sharing");
    parse.add_option("summary",
                     "Workload summary");
    parse.add_option("shared-addresses",
                     "Shared (object, address) pairs");
    parse.add_option("check-testcases",
                     "Check for cache line sharing in commutative testcases");
    parse.add_option("all-sharing",
                     "Report all sharing between CPUs");
    parse.add_option("cache-assoc",
                     "Report cache associativity set contention");
    parse.add_option("sbw0", "");
    parse.add_option("check-gc",
                     "Check for RCU memory accessed without gc_epoch");
    parse.add_option("serial-length",
                     "The number of instructions executed in each "
                     "serial section");
    parse.parse(handle_arg);

    // The default if no arguments
    if (ac == 1) {
        mtrace_options.distinct_sys = true;
        mtrace_options.false_sharing = true;
        mtrace_options.summary = true;
    }

    log = gzopen(mtrace_options.log_file.c_str(), "rb");
    if (!log)
        edie("gzopen %s", mtrace_options.log_file.c_str());

    addr2line = new Addr2line(mtrace_options.elf_file);

    int fd = open(mtrace_options.elf_file.c_str(), O_RDONLY);
    if (fd < 0)
        die("failed to open %s", mtrace_options.elf_file.c_str());
    mtrace_elf = elf::elf(elf::create_mmap_loader(fd));
    try {
        mtrace_dwarf = dwarf::dwarf(dwarf::elf::create_loader(mtrace_elf));
    } catch (std::exception& e) {
        cerr << "Cannot init dwarf: " << e.what() << "\n";
    }

    init_static_syms(mtrace_elf);
    init_entry_alloc();
    init_handlers();

    process_log(log);

    gzclose(log);
    return 0;
}
