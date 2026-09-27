"""O02 diagnostic: compare true scratch owners, full bytes and ordered targets."""
from pathlib import Path
import json
from homm2.core.coff import CoffObject, FUNCTION_TYPE, MEM_EXECUTE
from homm2.build.candidate_data_manifest import _symbol_inventory, _pe_layout
from homm2.core.retail import verify_retail

verify_retail(Path('build/orig/HMM2PL.exe'))
FIELDS = [
  {
    "name": "_s_hasTarget",
    "old_offset": 0,
    "va": 5455264,
    "size": 4
  },
  {
    "name": "_s_hasAdjacentMonster",
    "old_offset": 4,
    "va": 5455268,
    "size": 4
  },
  {
    "name": "_s_possibleDirections",
    "old_offset": 8,
    "va": 5455272,
    "size": 8
  },
  {
    "name": "_s_neighborX",
    "old_offset": 16,
    "va": 5455280,
    "size": 4
  },
  {
    "name": "_s_remainingMobility",
    "old_offset": 20,
    "va": 5455284,
    "size": 4
  },
  {
    "name": "_s_targetWater",
    "old_offset": 28,
    "va": 5455292,
    "size": 4
  },
  {
    "name": "_s_adjacentCost",
    "old_offset": 32,
    "va": 5455296,
    "size": 4
  },
  {
    "name": "_s_currentWater",
    "old_offset": 36,
    "va": 5455300,
    "size": 4
  },
  {
    "name": "_s_adjacentX",
    "old_offset": 40,
    "va": 5455304,
    "size": 4
  },
  {
    "name": "_s_currentNode",
    "old_offset": 48,
    "va": 5455312,
    "size": 9
  },
  {
    "name": "_s_mapY",
    "old_offset": 60,
    "va": 5455324,
    "size": 4
  },
  {
    "name": "_s_directionBlocked",
    "old_offset": 64,
    "va": 5455328,
    "size": 4
  },
  {
    "name": "_s_neighborY",
    "old_offset": 68,
    "va": 5455332,
    "size": 4
  },
  {
    "name": "_s_targetCell",
    "old_offset": 72,
    "va": 5455336,
    "size": 4
  },
  {
    "name": "_s_directionCosts",
    "old_offset": 76,
    "va": 5455340,
    "size": 8
  },
  {
    "name": "_s_currentCost",
    "old_offset": 84,
    "va": 5455348,
    "size": 4
  },
  {
    "name": "_s_neighborCell",
    "old_offset": 88,
    "va": 5455352,
    "size": 4
  },
  {
    "name": "_s_candidateY",
    "old_offset": 92,
    "va": 5455356,
    "size": 4
  },
  {
    "name": "_s_mapX",
    "old_offset": 96,
    "va": 5455360,
    "size": 4
  },
  {
    "name": "_s_triggerType",
    "old_offset": 100,
    "va": 5455364,
    "size": 4
  },
  {
    "name": "_s_adjacentMonsterX",
    "old_offset": 104,
    "va": 5455368,
    "size": 4
  },
  {
    "name": "_s_targetStepCost",
    "old_offset": 108,
    "va": 5455372,
    "size": 4
  },
  {
    "name": "_s_direction",
    "old_offset": 112,
    "va": 5455376,
    "size": 4
  },
  {
    "name": "_s_processedPointCount",
    "old_offset": 116,
    "va": 5455380,
    "size": 4
  },
  {
    "name": "_s_neighborNode",
    "old_offset": 120,
    "va": 5455384,
    "size": 4
  },
  {
    "name": "_s_bestTargetCost",
    "old_offset": 124,
    "va": 5455388,
    "size": 4
  },
  {
    "name": "_s_adjacentY",
    "old_offset": 128,
    "va": 5455392,
    "size": 4
  },
  {
    "name": "_s_terrain",
    "old_offset": 132,
    "va": 5455396,
    "size": 4
  },
  {
    "name": "_s_currentHero",
    "old_offset": 136,
    "va": 5455400,
    "size": 4
  }
]
def census(path):
    obj = CoffObject(path.read_bytes())
    result = {}
    for s in obj.symbols.values():
        sec = obj.symbol_section(s)
        if not sec or not sec.characteristics & MEM_EXECUTE or s.typ != FUNCTION_TYPE:
            continue
        end = min([t.value for t in obj.symbols.values()
                   if t.section == s.section and t.typ == FUNCTION_TYPE
                   and t.value > s.value], default=sec.raw_size)
        raw = bytearray(obj.section_bytes(sec)[s.value:end])
        relocs = []
        for r in obj.relocations:
            if r.section != s.section or not s.value <= r.site < end:
                continue
            if r.typ not in (6, 20):
                raise ValueError(r)
            off = r.site-s.value
            target = obj.symbols[r.symbol_index]
            name = target.name
            addend = int.from_bytes(raw[off:off+4], 'little')
            if name == '_s_seedPositionState':
                matched = [f for f in FIELDS if f['old_offset'] <= addend < f['old_offset']+f['size']]
                if len(matched) != 1:
                    raise ValueError(('unowned field', hex(addend)))
                f = matched[0]
                name, addend = f['name'], addend-f['old_offset']
            relocs.append((off,r.typ,name,addend))
            raw[off:off+4] = b'\0'*4
        result[s.name] = (bytes(raw),relocs)
    return result

before = census(Path('build/reconstruction-SEARCH-before.obj'))
after = census(Path('build/objdiff/base/SOURCE/SEARCH.obj'))
comparisons=[]
for name in sorted(before.keys()|after.keys()):
    a,b = before.get(name),after.get(name)
    comparisons.append(dict(name=name,before_size=len(a[0]) if a else None,
      after_size=len(b[0]) if b else None,bytes_equal=a is not None and b is not None and a[0]==b[0],
      ordered_relocations_equal=a is not None and b is not None and a[1]==b[1],
      relocation_diffs=[] if a is None or b is None else [(x,y) for x,y in zip(a[1],b[1]) if x!=y]))
public,data,functions = _symbol_inventory(Path('build/gen/symbol_names.csv'))
public.update({f['name']: f['rva'] for f in functions['SOURCE/SEARCH']})
public.update({f['name']:f['va']-0x400000 for f in FIELDS})
base,sites,read_u32,read_bytes = _pe_layout(Path('build/orig/HMM2PL.exe'))
retail=[]
for f in functions['SOURCE/SEARCH']:
    if f['name'] not in after:
        continue
    raw,relocs=after[f['name']]
    target=bytearray(read_bytes(f['rva'],f['size']))
    mismatch=[]
    for off,typ,name,addend in relocs:
        actual=read_u32(f['rva']+off)
        actual=((actual-base) if typ==6 else (actual+f['rva']+off+4)) & 0xffffffff
        expected=public.get(name)
        if expected is not None:
            expected=(expected+addend)&0xffffffff
        if expected is None or expected!=actual:
            mismatch.append(dict(site=hex(base+f['rva']+off),name=name,addend=hex(addend),
              actual=hex(actual+base),expected=None if expected is None else hex(expected+base)))
        target[off:off+4]=b'\0'*4
    retail.append(dict(name=f['name'],bytes_equal=raw==bytes(target),
      absolute_sites_equal=[r[0] for r in relocs if r[1]==6]==[s-f['rva'] for s in sites if f['rva']<=s<f['rva']+f['size']],
      relocations=mismatch))
result=dict(before_after=comparisons,retail=retail)
Path('build/search-owner-verification.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result,indent=2))
