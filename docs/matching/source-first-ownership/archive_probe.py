"""Reproduce the measured controls in header-first-use.cpp.

Run from the repository root inside nix develop .#build. Outputs are disposable
and live under build/holistic-source-first; production source is never edited.
Run header_probe.py before archive_probe.py.
"""

from pathlib import Path
import json,re,shutil
from homm2.core import wine
from homm2.core.coff import CoffObject,MEM_EXECUTE
from homm2.core.manifest import load,units,unit_flags
from homm2.build.cc_wrap import run_compile
root=Path.cwd();base=root/'build/holistic-source-first/archive-probe';base.mkdir(parents=True, exist_ok=True)
src=root/'build/holistic-source-first/header-probe/implicit-header-Ob1'
for name in ['Node.h','Effects.cpp','Music.cpp']:
 s=(src/name).read_text().replace('int main() { return 0; }\n','');(base/name).write_text(s)
manifest=load();entry=next(u for u in units(manifest) if u['unit']=='BASE/AudiereEffects');flags=unit_flags(entry,manifest)
for order in [('Effects','Music'),('Music','Effects')]:
 name='Driver-'+'-'.join(order)
 s='''#include "Node.h"
void PurgeEffects();
void PurgeMusic(Node*);
int main() {
'''
 s+='\n'.join('    PurgeEffects();' if x=='Effects' else '    PurgeMusic(NULL);' for x in order)+'\n    return 0;\n}\n';(base/(name+'.cpp')).write_text(s)
for unit in ['Effects','Music','Driver-Effects-Music','Driver-Music-Effects']:
 rc,log,timed=run_compile(base/(unit+'.cpp'),base/(unit+'.obj'),flags,depfile=False,cl_timeout=60);(base/(unit+'.log')).write_text(log)
 if rc or timed:raise RuntimeError((unit,rc,timed))
libs={'Effects':['Effects'],'Music':['Music'],'Both-Effects-Music':['Effects','Music'],'Both-Music-Effects':['Music','Effects']}
for name,members in libs.items():
 rsp=base/(name+'.rsp');rsp.write_text(' '.join(['/NOLOGO','/OUT:'+wine.winepath_w(base/(name+'.lib')),*[wine.winepath_w(base/(m+'.obj')) for m in members]])+'\n')
 wine.run(root/'build/toolchain/msvc/bin/LIB.EXE','@'+wine.winepath_w(rsp),cwd=root,log=base/(name+'-lib.log'))
rows=[]
for driver in ['Driver-Effects-Music','Driver-Music-Effects']:
 for libraries in [('Both-Effects-Music',),('Both-Music-Effects',),('Effects','Music'),('Music','Effects')]:
  name=driver+'--'+'-'.join(libraries);out=base/(name+'.exe');mp=out.with_suffix('.map');rsp=out.with_suffix('.rsp')
  args=['/NOLOGO','/SUBSYSTEM:CONSOLE','/MACHINE:IX86','/INCREMENTAL:NO','/OPT:NOREF','/OUT:'+wine.winepath_w(out),'/MAP:'+wine.winepath_w(mp),'/LIBPATH:'+wine.winepath_w(root/'build/toolchain/msvc/lib'),wine.winepath_w(base/(driver+'.obj')),*[wine.winepath_w(base/(lib+'.lib')) for lib in libraries],'MSVCPRT.LIB','LIBCMT.LIB','KERNEL32.LIB']
  rsp.write_text(' '.join(args)+'\n');wine.run(root/'build/toolchain/msvc/bin/LINK.EXE','@'+wine.winepath_w(rsp),cwd=root,log=out.with_suffix('.log'))
  selected={}
  for line in mp.read_text(errors='replace').splitlines():
   for role,frag in [('Node','??1Node@@'),('Effects','?PurgeEffects@@'),('Music','?PurgeMusic@@')]:
    if frag in line:selected[role]=line.strip()
  row=dict(driver=driver,libraries=libraries,symbols=selected);rows.append(row);print(row,flush=True)
(base/'results.json').write_text(json.dumps(rows,indent=2)+'\n')
