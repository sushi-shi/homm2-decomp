"""Before/after census. Requires build/xcamp-owner-before copied from parent objects.
Allows only the two O08 physical-owner/addend rewrites; everything else is strict.
"""
import importlib.util
from pathlib import Path
spec=importlib.util.spec_from_file_location('comparison','docs/reconstruction/compare-objects.py')
mod=importlib.util.module_from_spec(spec);spec.loader.exec_module(mod)
before=Path('build/xcamp-owner-before');after=Path('build/objdiff/base')
paths=sorted(p.relative_to(before) for p in before.rglob('*.obj'))
assert len(paths)==98 and set(paths)=={p.relative_to(after) for p in after.rglob('*.obj')}
count=0
for path in paths:
 a=mod.census(before/path);b=mod.census(after/path)
 if str(path)=='SOURCE/X_CAMPGN.obj':
  obj=mod.CoffObject(mod.canonicalize_coff((before/path).read_bytes()).data)
  for name,owner,oldadd in [('JosephName','xJosephName',0),('IvanName','xUncleIvanName',24)]:
   symbol,=[s for s in obj.symbols.values() if s.name=='?'+name+'@ExpCampaign@@QAEPADXZ']
   oldowner='?xStableText@@3PAPADA';newowner='?'+owner+'@@3PAPADA'
   def corrected(data,relocs,offset):
    assert data[offset:offset+4]==oldadd.to_bytes(4,'little')
    assert (offset,6,oldowner) in relocs
    data=data[:offset]+bytes.fromhex('f0ffffff')+data[offset+4:]
    relocs=[(site,typ,newowner if site==offset else target) for site,typ,target in relocs]
    return data,relocs
   a[0][symbol.name]=corrected(*a[0][symbol.name],0x20)
   sect=a[1][symbol.section]
   data,relocs=corrected(sect[2],sect[3],symbol.value+0x20)
   a[1][symbol.section]=sect[:2]+(data,relocs)
 assert a==b,path
 count+=len(a[0])
 print('PASS',path,len(a[0]),'functions and all allocated sections/ordered relocations')
print('PASS all',len(paths),'native objects;',count,'emitted function instances')
