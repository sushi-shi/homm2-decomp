"""Measured source products for whole-module-follow-through.cpp.

Run from a worktree with source AND raw objects at commit 23515eb2, inside
nix develop .#build. Writes disposable build artifacts only. See the dossier
for scope, completeness, startup corrections, and interpretation limits.
"""

from pathlib import Path
import json,re,hashlib,struct,time
from homm2.build.cc_wrap import run_compile
from homm2.core.manifest import load,units,unit_flags
from homm2.core.coff import CoffObject,MEM_EXECUTE
from homm2.core import wine
from homm2.build import native_link
root=Path.cwd();base=root/'build/holistic-modules'
manifest=load();entries={u['unit']:u for u in units(manifest)}
original={p:(root/p).read_text() for p in ['include/BASE/AudiereEffects.h','include/BASE/soundBackends.h','src/BASE/AudiereEffects.cpp','src/BASE/AudiereMusic.cpp','src/BASE/soundmgr.cpp','src/BASE/MilesSound.cpp']}
ctor='''    AudiereSampleNode(sample* resource, AudiereSampleNode* nextNode) {
        stream = NULL;
        sampleResource = resource;
        next = nextNode;
    }'''
allocation='''    gAudiereEffects.sampleList =
        new AudiereSampleNode(sampleResource, gAudiereEffects.sampleList);'''
api=original['include/BASE/soundBackends.h'].split('void PlayAudiereSample(')[1].split('void StopAudiereMusic(')[0]
api='void PlayAudiereSample('+api
names=re.findall(r'\b(\w+Audiere\w*|Audiere\w+)\s*\(',api)+['PurgeFinishedAudiereSamples','FindAudiereSample']
assert len(set(names))==12, names
variants=[]
# Write all source products before inspecting compiler or linker results.
for ownership in ['free-separate','class-separate','free-unified','class-unified']:
 for construction in ['body','members','default']:
  name=ownership+'-'+construction;d=base/name;files=dict(original)
  header=files['include/BASE/AudiereEffects.h'];effects=files['src/BASE/AudiereEffects.cpp']
  if construction=='members':header=header.replace(ctor,'''    AudiereSampleNode(sample* resource, AudiereSampleNode* nextNode)
        : stream(NULL), sampleResource(resource), next(nextNode) {}''')
  elif construction=='default':
   header=header.replace(ctor,'')
   effects=effects.replace(allocation,'''    AudiereSampleNode* newNode = new AudiereSampleNode;
    newNode->sampleResource = sampleResource;
    newNode->next = gAudiereEffects.sampleList;
    gAudiereEffects.sampleList = newNode;''')
  if ownership.startswith('class'):
   classdef='\nclass AudiereEffects {\npublic:\n'+''.join('    static '+s+';\n' for s in api.strip().split(';') if s.strip())+'private:\n    static void PurgeFinishedAudiereSamples();\n    static AudiereSampleNode* FindAudiereSample(sample* sampleResource);\n    static AudiereEffectsState state;\n};\n'
   header=header.replace('\n#endif',classdef+'\n#endif')
   files['include/BASE/soundBackends.h']=files['include/BASE/soundBackends.h'].replace(api,'').replace('#include <audiere.h>','#include <audiere.h>\n#include "AudiereEffects.h"')
   effects=effects.replace('static AudiereEffectsState gAudiereEffects','AudiereEffectsState AudiereEffects::state').replace('gAudiereEffects','AudiereEffects::state')
   for fn in names:
    effects=re.sub(r'\b'+fn+r'\b','AudiereEffects::'+fn,effects)
    files['src/BASE/soundmgr.cpp']=re.sub(r'\b'+fn+r'\b','AudiereEffects::'+fn,files['src/BASE/soundmgr.cpp'])
  files['include/BASE/AudiereEffects.h']=header
  if ownership.endswith('unified'):
   music=files['src/BASE/AudiereMusic.cpp']
   includes=list(dict.fromkeys(re.findall(r'^#include[^\n]*',effects+'\n'+music,re.M)))
   effects='\n'.join(includes)+'\n'+re.sub(r'^#include[^\n]*\n','',effects,flags=re.M)+'\n'+re.sub(r'^#include[^\n]*\n','',music,flags=re.M)
  files['src/BASE/AudiereEffects.cpp']=effects
  for p,s in files.items():
   q=d/p;q.parent.mkdir(parents=True,exist_ok=True)
   if p.startswith('src/'):
    s=s.replace('#include <BASE/soundBackends.h>', '#include "../../include/BASE/soundBackends.h"').replace('#include <BASE/AudiereEffects.h>', '#include "../../include/BASE/AudiereEffects.h"')
   q.write_text(s)
  variants.append(dict(name=name,ownership=ownership,construction=construction))
(base/'manifest.json').write_text(json.dumps(variants,indent=2)+'\n')
def audit(path):
 c=CoffObject(path.read_bytes());rows=[]
 for sec in c.sections:
  if not sec.characteristics & MEM_EXECUTE:continue
  for s in c.symbols.values():
   if s.section!=sec.index or s.typ!=32:continue
   end=min([t.value for t in c.symbols.values() if t.section==sec.index and t.typ==32 and t.value>s.value]+[sec.raw_size])
   raw=c.section_bytes(sec)[s.value:end];masked=bytearray(raw);rel=[]
   for r in c.relocations:
    if r.section!=sec.index or not s.value<=r.site<end:continue
    target=c.symbols[r.symbol_index];offset=r.site-s.value;addend=int.from_bytes(raw[offset:offset+4],'little');masked[offset:offset+4]=b'\0'*4
    rel.append(dict(site=offset,type=r.typ,target=target.name,addend=addend))
   rows.append(dict(name=s.name,section=sec.index,offset=s.value,size=len(raw),bytes=raw.hex(),masked=bytes(masked).hex(),relocations=rel))
 return rows
baseline={u:audit(root/f'build/objdiff/base/BASE/{u}.obj') for u in ['AudiereEffects','AudiereMusic','soundmgr','MilesSound']}
(base/'baseline.json').write_text(json.dumps(baseline,indent=2)+'\n')
def sections(data):
 pe=struct.unpack_from('<I',data,60)[0];n=struct.unpack_from('<H',data,pe+6)[0];opt=struct.unpack_from('<H',data,pe+20)[0];rows={}
 for i in range(n):
  p=pe+24+opt+i*40;name=data[p:p+8].split(b'\0')[0].decode();sz,off=struct.unpack_from('<II',data,p+16);rows[name]=data[off:off+sz]
 return rows
retail=sections((root/'build/orig/HMM2PL.exe').read_bytes())
configured=native_link.final_inputs(native_link.ninja_link_args(),include_resources=True)
ninja=(root/'build.ninja').read_text().replace('$\n',' ')
rows=[]
for v in variants:
 d=base/v['name'];start=time.monotonic();row=dict(v);replacements={};objects={};compiled=['AudiereEffects','soundmgr','MilesSound']+([] if v['ownership'].endswith('unified') else ['AudiereMusic'])
 for unit in compiled:
  out=d/(unit+'.obj');flags=unit_flags(entries['BASE/'+unit],manifest)+['/I'+wine.winepath_w(d/'include')]
  rc,log,timeout=run_compile(d/f'src/BASE/{unit}.cpp',out,flags,depfile=False,cl_timeout=60);(d/(unit+'.log')).write_text(log)
  if rc or timeout:raise RuntimeError((v['name'],unit,rc,timeout,log))
  replacements[f'build/objdiff/base/BASE/{unit}.obj']=str(out.relative_to(root));objects[unit]=audit(out)
 (d/'objects.json').write_text(json.dumps(objects,indent=2)+'\n')
 if v['ownership'].endswith('unified'):replacements['build/objdiff/base/BASE/AudiereMusic.obj']=None
 inputs=[]
 for item in configured:
  if item not in ['build/link/BASE-prefix.lib','build/link/BASE-suffix.lib']:inputs.append(item);continue
  line=next(x for x in ninja.splitlines() if x.startswith('build '+item+': archive '));members=line.split(': archive ',1)[1].split(' |',1)[0].split()
  members=[replacements.get(m,m) for m in members];members=[m for m in members if m is not None]
  lib=d/Path(item).name;rsp=lib.with_suffix('.rsp');rsp.write_text(' '.join(['/NOLOGO','/OUT:'+str(lib.relative_to(root)),*members])+'\n');wine.run(root/'build/toolchain/msvc/bin/LIB.EXE','@'+str(rsp.relative_to(root)),cwd=root,log=lib.with_suffix('.log'));inputs.append(str(lib.relative_to(root)))
 out=d/'HMM2PL.exe';mp=out.with_suffix('.map');rsp=out.with_suffix('.rsp');pdb=str((d/'HMM2PL.pdb').relative_to(root))
 rsp.write_text(' '.join(native_link.link_prefix(out,mp,pdb)+inputs)+'\n');wine.run(native_link.LINK_EXE,'@'+str(rsp.relative_to(root)),cwd=root,log=d/'link.log')
 blob=out.read_bytes();sec=sections(blob);row['sha256']=hashlib.sha256(blob).hexdigest();row['section_differences']={k:sum(a!=b for a,b in zip(s,sec.get(k,b'')))+abs(len(s)-len(sec.get(k,b''))) for k,s in retail.items()}
 row['seconds']=round(time.monotonic()-start,2);row['functions']={u:len(r) for u,r in objects.items()};rows.append(row);(base/'results.json').write_text(json.dumps(rows,indent=2)+'\n');print(row,flush=True)
