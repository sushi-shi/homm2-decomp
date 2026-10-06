import json
from pathlib import Path
m=json.loads(Path('build/probe/audiere-pch.json').read_text())
eff=m["files"]["AudiereEffects.cpp"]
tail=eff[eff.index('VA(0x004ccc90'):eff.index('@@effdef@@')]
m["axes"][0]["options"]=[o for o in m["axes"][0]["options"] if o["name"]=="hdr_inline"]
m["axes"][1]["options"]=[o for o in m["axes"][1]["options"] if o["name"]=="pch_sb_sep"]
m["axes"].append({"name":"tail","options":[{"name":"keep"},{"name":"cut","edits":[{"file":"AudiereEffects.cpp","find":tail,"replace":""}]}]})
m["axes"].append({"name":"size","options":[{"name":"size_keep"},{"name":"size_cut","edits":[{"file":"AudiereEffects.cpp","find":"SIZE(AudiereSampleNode, 0xc);\n","replace":""}]}]})
play=eff[eff.index('VA(0x004cc8f0'):eff.index('VA(0x004ccc90')]
m["axes"].append({"name":"play","options":[{"name":"play_keep"},{"name":"play_cut","edits":[{"file":"AudiereEffects.cpp","find":play,"replace":""}]}]})
Path('build/probe/audiere-pch2.json').write_text(json.dumps(m,indent=1))
