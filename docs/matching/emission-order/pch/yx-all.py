import re, sys, json, os, concurrent.futures, shlex
from pathlib import Path
sys.path.insert(0, 'scripts')
from homm2.tool import wine
from homm2.permute.tu_state_noise import compile_object
from homm2.permute.emission_order import object_functions
extra = sys.argv[1:]
text = Path('build.ninja').read_text()
edges = re.findall(r'build build/objdiff/base/(\S+)\.obj: cl (\S+) \|[^\n]*\n(?:    [^\n]*\n)*?  flags = ([^\n]*)\n', text)
out = Path('build/probe/yx-all/' + ('_'.join(f.strip('/').replace('/','') for f in extra) or 'none'))
wine.prepare_env()
def one(e):
    unit, src, flags = e
    d = out / unit.replace('/', '_'); d.mkdir(parents=True, exist_ok=True)
    o = d / 'x.obj'
    ok, log, to = compile_object(Path('.'), Path(src).resolve(), o.resolve(), flags.split() + extra, 300)
    if not ok: return unit, None, log[-300:]
    base = Path(f'build/objdiff/base/{unit}.obj').read_bytes()
    a = [(f['name'], f['size']) for f in object_functions(base)]
    b = [(f['name'], f['size']) for f in object_functions(o.read_bytes())]
    same_bytes = base[20:] == o.read_bytes()[20:]
    return unit, (a == b, same_bytes, [n for n, _ in b]), ''
res = {}
with concurrent.futures.ThreadPoolExecutor(max_workers=max(1, os.cpu_count() // 2)) as pool:
    for unit, r, log in pool.map(one, edges):
        res[unit] = r
        if r is None: print('FAIL', unit, log)
        elif not r[0]: print('ORDER', unit, ' '.join(n[:40] for n in r[2]))
print(len(edges), 'units;', sum(1 for r in res.values() if r and r[0]), 'same order;', sum(1 for r in res.values() if r and r[1]), 'byte-identical after header')
