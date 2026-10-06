import json
from pathlib import Path
m=json.loads(Path('build/probe/post-init3.json').read_text())
m["files"]["node.h"]="@@va@@@@sample@@@@lib@@"+m["files"]["node.h"].split("@@lib@@",1)[1]
m["files"]["eff.cpp"]=m["files"]["eff.cpp"].replace('#include "node.h"\n','#include "node.h"\n@@after@@')
m["defaults"]={"va":"","sample":"","after":""}
m["axes"]=[{"name":"lib","options":[o for o in m["axes"][0]["options"] if o["name"]=="audiere"]},
 {"name":"dt","options":[o for o in m["axes"][1]["options"] if o["name"]=="hdr_inline"]},
 {"name":"va","options":[{"name":"no_va"},{"name":"va","slots":{"va":"#include <va.h>\n"}}]},
 {"name":"sample","options":[{"name":"no_sample"},{"name":"sample","slots":{"sample":"#include <BASE/sample.h>\n"}}]},
 {"name":"after","options":[{"name":"none"},{"name":"kb","slots":{"after":"#include <BASE/soundManager.h>\n#include <SOURCE/KB.h>\n"}},{"name":"smgr","slots":{"after":"#include <BASE/soundManager.h>\n"}}]}]
Path('build/probe/post-init4.json').write_text(json.dumps(m,indent=1))
