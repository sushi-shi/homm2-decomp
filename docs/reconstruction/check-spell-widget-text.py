"""B59: fixed retail spell-name catalog and complete UpdateSpellWidgets text bounds.

Run from the worktree with PYTHONPATH=scripts. This checks all 65 source
names against the actual retail pointer table; valid spell IDs are still
a caller contract. The cost bound covers every signed i32 result, not just
the default cost table. No source, object, or executable is modified.
"""
import ast
import re
from pathlib import Path
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

control = Path('build/orig/HMM2PL.exe')
verify_retail(control)
base, _, read_u32, read_bytes = _pe_layout(control)
source = Path('src/SOURCE/KB.cpp').read_text()
anchor = 'DATA(0x004fec04) H2_CONST char* gSpellNames[KB_SPELL_TEXT_COUNT] = {'
assert source.count(anchor) == 1
body = source.split(anchor, 1)[1].split('\n};', 1)[0]
body = re.sub(r'/\*.*?\*/', '', body, flags=re.S)
tokens = re.findall(r'"(?:\\.|[^"\\])*"', body)
names = [ast.literal_eval(token).encode('latin1') for token in tokens]
assert len(names) == 65
for i, name in enumerate(names):
    address = read_u32(0xfec04 + 4*i)
    assert read_bytes(address-base, len(name)+1) == name+b'\0', (i, address)
print('PASS all65 names and ordered retail pointer destinations')
longest = max(map(len, names))
print('Longest CP1251 name:', longest, 'bytes',
      [n.decode('cp1251') for n in names if len(n) == longest])
assert longest == 20
header = Path('include/SOURCE/X_GLOBAL.h').read_text()
assert re.search(r'GLOBAL_TEXT_BUFFER_SIZE\s*=\s*768\b', header)
assert 'DATA(0x00524ad0) char gText[GLOBAL_TEXT_BUFFER_SIZE];' in source
game = Path('src/SOURCE/GAME.cpp').read_text()
widget_body = game.split('void game::UpdateSpellWidgets(void) {', 1)[1].split('\nVA(', 1)[0]
assert widget_body.count('"%s\\n[%d]"') == 1
assert widget_body.count('"%s [%d]"') == 1
assert widget_body.count('gSpellNames[IDX(spell1)]') == 3
cost_width = len(str(-(1 << 31)))
assert cost_width == 11
for separator in (b'\n', b' '):
    # "%s\n[%d]" and "%s [%d]": name, separator, brackets, number, NUL.
    bound = longest + len(separator) + 2 + cost_width + 1
    assert bound == 35 and bound <= 768
    print('PASS complete name/cost label including NUL <=', bound, '/768')
# Signed integer division/remainder preserves magnitude below10. Even a
# negative remainder therefore formats as at most two characters plus NUL.
assert max(len(str(n))+1 for n in range(-9, 10)) == 3
print('PASS each decimal mana digit including NUL <=3/768')
