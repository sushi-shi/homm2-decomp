"""B65: full retail new-turn text bounds, conditional on valid name-table indices."""
import re
import shutil
import subprocess
from pathlib import Path
from homm2.core.retail import verify_retail
from homm2.build.candidate_data_manifest import _pe_layout

retail = Path('build/orig/HMM2PL.exe')
verify_retail(retail)
base, _, u32, read = _pe_layout(retail)
source = Path('src/SOURCE/GAME.cpp').read_text()
for name, value in [('NEW_TURN_LOWER_NAME_CAPACITY', 52),
                    ('NEW_TURN_MUSIC_FILENAME_CAPACITY', 16)]:
    assert re.search(r'\b' + name + r'\s*=\s*' + str(value) + r'\b', source)
assert re.search(r'GLOBAL_TEXT_BUFFER_SIZE\s*=\s*768', Path('include/SOURCE/X_GLOBAL.h').read_text())

def table(va, count):
    result = []
    for index in range(count):
        raw = read(u32(va - base + 4 * index) - base, 4096)
        assert b'\0' in raw
        result.append(raw.split(b'\0', 1)[0])
    return result

plural = table(0x4fded0, 66)
months = table(0x4fe4c0, 10)
weeks = table(0x4fe4e8, 15)
formats = table(0x4fe6d0, 7)
for label, values in [('plural names', plural), ('month names', months), ('week names', weeks)]:
    print(label, 'all lengths without NUL', list(map(len, values)))

def formatted(fmt, *widths):
    tokens = list(re.finditer(rb'%[sd]', fmt))
    assert len(tokens) == len(widths)
    assert b'%' not in re.sub(rb'%[sd]', b'', fmt)
    return len(fmt) + 1 + sum(width - len(token[0]) for token, width in zip(tokens, widths))

name = max(map(len, plural))
# Each player name occupies21 bytes including its required terminator.
# daysLeft is signed i8 and formatted only when nonnegative, at most127.
bounds = [formatted(formats[0], 20, 3), formatted(formats[1], 20),
          formatted(formats[2], max(map(len, months))), formatted(formats[3], name, name),
          formatted(formats[4]), formatted(formats[5], max(map(len, weeks))),
          formatted(formats[6], name, name)]
assert name + 1 <= 52 and max(bounds) <= 768
assert max(len(b'newmonth.82m'), len(b'newweek.82m')) + 1 <= 16
print('PASS lower name including NUL:', name + 1, '/52; music filename12/16')
print('PASS all seven gText format bounds including NUL:', bounds, '; maximum', max(bounds), '/768')
print('The one-byte CP1251 fold preserves length; table indices, player-name termination and live owners remain contracts.')

# Compile the actual recovered helper, not a separately maintained copy.
constants = source.split('H2_ENUM_BEGIN(Cp1251Constant)', 1)[1].split('H2_ENUM_END(Cp1251Constant)', 1)[0]
helper = source.split('    inline char ToLowerCp1251(u8 letter) {', 1)[1].split('\n    }', 1)[0]
expected = list(range(256))
for text in ['ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ']:
    for letter in text:
        expected[letter.encode('cp1251')[0]] = letter.lower().encode('cp1251')[0]
probe = ('typedef unsigned char u8;\nenum {' + constants + '};\n'
         'inline char ToLowerCp1251(u8 letter) {' + helper + '\n}\n'
         'int main() {\nconst unsigned char expected[256] = {' + ','.join(map(str, expected)) + '};\n'
         'for (int i = 0; i < 256; ++i) {\n'
         'char text = static_cast<char>(i);\n'
         'if (static_cast<unsigned char>(ToLowerCp1251(text)) != expected[i]) return 1;\n'
         'if (static_cast<unsigned char>(ToLowerCp1251(static_cast<u8>(i))) != expected[i]) return 2;\n'
         '} return 0; }\n')
compiler = shutil.which('c++')
assert compiler
for dialect in ['c++98', 'c++20']:
    output = Path('build/c43-cp1251-' + dialect)
    subprocess.run([compiler, '-x', 'c++', '-std=' + dialect, '-fsigned-char', '-', '-o', str(output)],
                   input=probe, text=True, check=True)
    subprocess.run([str(output.resolve())], check=True)
    print('PASS actual CP1251 helper:', dialect, 'all256 bytes through both signed-text and unsigned-byte inputs')
