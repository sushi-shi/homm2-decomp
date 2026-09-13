"""C02: all native bytes/sections/ordered targets with two declared type renames.

Run after building the typed cursor owners, using build/c02-owner-before.
Only the two 96-pointer global symbol spellings may change. This is a native
equivalence check; independent retail checks run after source-model refresh.
"""
import importlib.util
from pathlib import Path
from homm2.core.coff import CoffObject

spec=importlib.util.spec_from_file_location('comparison','docs/reconstruction/compare-objects.py')
mod=importlib.util.module_from_spec(spec);spec.loader.exec_module(mod)
before=Path('build/c02-owner-before');after=Path('build/objdiff/base')
renames={'?cColorBits@@3PAPAXA':'?cColorBits@@3PAPAEA',
         '?cAndBits@@3PAPAXA':'?cAndBits@@3PAPAEA'}
def renamed(value):
    if isinstance(value,str):return renames.get(value,value)
    if isinstance(value,dict):return {renamed(k):renamed(v) for k,v in value.items()}
    if isinstance(value,tuple):return tuple(renamed(v) for v in value)
    if isinstance(value,list):return [renamed(v) for v in value]
    return value
old=CoffObject((before/'BASE/MOUSEMGR.obj').read_bytes())
new=CoffObject((after/'BASE/MOUSEMGR.obj').read_bytes())
for old_name,new_name in renames.items():
    a,=[s for s in old.symbols.values() if s.name==old_name and s.section>0]
    b,=[s for s in new.symbols.values() if s.name==new_name and s.section>0]
    assert (a.section,a.value,a.storage_class)==(b.section,b.value,b.storage_class)
    assert old.section_bytes(old.symbol_section(a))[a.value:a.value+384]==new.section_bytes(new.symbol_section(b))[b.value:b.value+384]
paths=sorted(p.relative_to(before) for p in before.rglob('*.obj'))
assert len(paths)==98 and set(paths)=={p.relative_to(after) for p in after.rglob('*.obj')}
count=0
for path in paths:
    a=mod.census(before/path);b=mod.census(after/path)
    assert renamed(a)==b,path
    count+=len(a[0]);print('PASS',path,len(a[0]),'functions; all allocated sections and ordered relocations')
print('PASS all98 native objects;',count,'emitted functions; only two declared data type renames')
