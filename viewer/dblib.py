"""Library to format and compress mscan databases for the viewer."""

import os
import json

def reformat_tests(testcases, runid):
    """Convert testcases into format expected by viewer."""

    for testcase in testcases:
        # Split name
        suite, name = testcase.pop('name').split('-', 1)
        calls, pathid, testno = name.rsplit('_', 2)
        testno = int(testno)
        testcase['calls'] = calls
        testcase['pathid'] = pathid
        testcase['testno'] = testno

        # Remove redundant fields
        for shared in testcase['shared']:
            if any(f in shared for f in ['stack', 'stack1', 'stack2']):
                for pc in ['pc', 'pc1', 'pc2']:
                    if pc in shared:
                        del shared[pc]
            del shared['rawaddr']

        # Add identifying fields
        testcase['runid'] = runid

    return testcases

def dedup_stacks(testcases):
    stacks = {}
    rstacks = {}
    for testcase in testcases:
        for shared in testcase['shared']:
            # Deduplicate stacks
            for skey in ['stack', 'stack1', 'stack2']:
                if skey not in shared:
                    continue
                stack = shared[skey]
                rstackskey = tuple(stack)
                if rstackskey not in rstacks:
                    name = 's%d' % len(stacks)
                    stacks[name] = stack
                    rstacks[rstackskey] = name
                shared[skey] = rstacks[rstackskey]

    return stacks

def split_details(testcases):
    general, details = [], []
    for testcase in testcases:
        shared = testcase.pop('shared')
        details.append({'runid': testcase['runid'],
                        'shared': shared})
        testcase['nshared'] = len(shared)
        general.append(testcase)
    return general, details

def split_template(data, fields, outdir):
    template = []
    for rec in data:
        template.append({f: rec.pop(f) for f in fields})

    # Write template
    tid = abs(hash(str(template)))
    template_path = 'template-%x.json' % tid
    json.dump(tablify(template, fields),
              file(os.path.join(outdir, template_path), 'w'),
              separators=(',',':'))

    # Return template-compressed data
    return {'!template': template_path, '!data': data}

def tablify(dicts, fields):
    data = []
    prev = []
    for d in dicts:
        # Convert to list
        dlst = []
        for f in fields:
            dlst.append(d.pop(f))

        # Run compress
        if prev == dlst:
            if isinstance(data[-1], int):
                data[-1] += 1
            else:
                data.append(1)
            continue

        # Delta compress against prev
        deltamask = 0
        delta = []
        for i in range(max(len(prev), len(dlst))):
            if i >= len(prev) or i >= len(dlst) or prev[i] != dlst[i]:
                deltamask |= 1 << i
                delta.append(dlst[i])
        if d:
            delta.append(d)
        data.append([deltamask] + delta)
        prev = dlst
    return {'!fields': fields, '!data': data}
