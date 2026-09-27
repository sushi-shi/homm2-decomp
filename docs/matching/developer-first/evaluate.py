from pathlib import Path
import ast,hashlib,json,struct,shutil,sys
from homm2.build.cc_wrap import run_compile
from homm2.core.coff import CoffObject,MEM_EXECUTE
from homm2.core.manifest import load,units,unit_flags
from homm2.core import wine
from homm2.build import native_link
root=Path.cwd();label=sys.argv[1] if len(sys.argv)>1 else 'draft'
draft=root/'docs/matching/developer-first'/label;base=root/'build/developer-first-draft';run=base/('evaluation' if label=='draft' else 'evaluation-release-order');run.mkdir(parents=True, exist_ok=True)
manifest=load();entries={u['unit']:u for u in units(manifest)}
frozen=json.loads((draft.parent/('frozen-source.json' if label=='draft' else 'frozen-release-order.json')).read_text())['files']
for name,digest in frozen.items():assert hashlib.sha256((draft/name).read_bytes()).hexdigest()==digest,name
# Reuse the reviewed read-only function inspector, not its executable matrix.
tree=ast.parse((root/'docs/matching/developer-first/coff_inspector.py').read_text());defs=[n for n in tree.body if isinstance(n,ast.FunctionDef) and n.name=='functions'];exec(compile(ast.Module(body=defs,type_ignores=[]),'inspector','exec'))
objects={};replacements={};baselines={}
for unit in ['BASE/AudiereEffects','BASE/DIMMER','BASE/WINDOW','SOURCE/REQUEST']:
 name=unit.split('/')[-1];src=draft/('src/'+unit+'.cpp')
 if not src.exists():src=root/entries[unit]['source']
 original=root/('build/objdiff/base/'+unit+'.obj');baselines[unit]=functions(CoffObject(original.read_bytes()));shutil.copy2(original,run/(name+'-baseline.obj'))
 out=run/(name+'.obj');flags=unit_flags(entries[unit],manifest)+['/I'+wine.winepath_w(draft/'include')]
 rc,log,timed=run_compile(src,out,flags,depfile=False,cl_timeout=60);(run/(name+'.log')).write_text(log)
 if rc or timed:raise RuntimeError((unit,rc,timed,log))
 c=CoffObject(out.read_bytes());objects[unit]=functions(c);replacements[str(original.relative_to(root))]=str(out.relative_to(root))
 (run/(name+'-sections.json')).write_text(json.dumps([dict(index=s.index,name=s.name,flags=s.characteristics,size=s.raw_size,bytes=c.section_bytes(s).hex()) for s in c.sections],indent=2)+'\n')
 print('compiled',unit,flush=True)
(run/'objects.json').write_text(json.dumps(objects,indent=2)+'\n');(run/'baseline.json').write_text(json.dumps(baselines,indent=2)+'\n')
configured=native_link.final_inputs(native_link.ninja_link_args(),include_resources=True);ninja=(root/'build.ninja').read_text().replace('$\n',' ');inputs=[]
for item in configured:
 if item not in ['build/link/BASE-prefix.lib','build/link/BASE-suffix.lib']:inputs.append(replacements.get(item,item));continue
 line=next(x for x in ninja.splitlines() if x.startswith('build '+item+': archive '));members=line.split(': archive ',1)[1].split(' |',1)[0].split();members=[replacements.get(m,m) for m in members]
 lib=run/Path(item).name;rsp=lib.with_suffix('.rsp');rsp.write_text(' '.join(['/NOLOGO','/OUT:'+str(lib.relative_to(root)),*members])+'\n');wine.run(root/'build/toolchain/msvc/bin/LIB.EXE','@'+str(rsp.relative_to(root)),cwd=root,log=lib.with_suffix('.log'));inputs.append(str(lib.relative_to(root)))
out=run/'HMM2PL.exe';mp=out.with_suffix('.map');rsp=out.with_suffix('.rsp');rsp.write_text(' '.join(native_link.link_prefix(out,mp,str((run/'HMM2PL.pdb').relative_to(root)))+inputs)+'\n');wine.run(native_link.LINK_EXE,'@'+str(rsp.relative_to(root)),cwd=root,log=run/'link.log')
assert out.exists()
result={}
for unit,rows in baselines.items():
 target={r['name']:r for r in objects[unit]};diff=[]
 for row in rows:
  other=target.get(row['name']);entry={'name':row['name'],'old_size':row['size'],'new_size':other['size'] if other else None}
  if other is None:entry['status']='not emitted by this owner'
  elif row['masked']!=other['masked']:entry['status']='body changed'
  elif [(r['site'],r['type'],r['target'],r['addend']) for r in row['relocations']]!=[(r['site'],r['type'],r['target'],r['addend']) for r in other['relocations']]:entry['status']='relocation records changed'
  else:continue
  diff.append(entry)
 result[unit]=diff
(run/'comparison.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps(result,indent=2),flush=True)
for name,digest in frozen.items():assert hashlib.sha256((draft/name).read_bytes()).hexdigest()==digest,name
print('Full native link completed; frozen source unchanged.',flush=True)
