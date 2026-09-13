"""Readable authored text, exact compiler bytes, and real Clang format checking."""
import ast
import json
from pathlib import Path
import tempfile
import unittest
from unittest import mock

from homm2.build import localization as loc


class LocalizationTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        (self.root / 'locales').mkdir()
        (self.root / 'src').mkdir()
        (self.root / 'include').mkdir()
        self.write_catalog()

    def write_catalog(self, english='Gold: %s %d', russian='Золото: %s %d'):
        (self.root / 'locales/messages.def').write_text(
            f'HOMM2_MESSAGE("resource.gold", {loc.quoted(english)})\n', encoding='utf-8')
        (self.root / 'locales/ru.po').write_text(
            'msgid ""\nmsgstr ""\n"Language: ru\\n"\n'
            '"Content-Type: text/plain; charset=UTF-8\\n"\n\n'
            f'msgctxt "resource.gold"\nmsgid {loc.quoted(english)}\n'
            f'msgstr {loc.quoted(russian)}\n', encoding='utf-8')

    def test_cp1251_bytes_and_escape_boundaries(self):
        text = 'ЁёЯя A9"\\\n\t\0'
        encoded = loc.literal(text)
        self.assertTrue(encoded.isascii())
        self.assertEqual(ast.literal_eval(encoded).encode('latin1'), text.encode('cp1251'))

    def test_unrepresentable_translation_is_fatal(self):
        self.write_catalog('Gold', 'Золото 🪙')
        with self.assertRaises(UnicodeEncodeError):
            loc.Catalog.load(self.root)

    def test_unknown_id_is_fatal(self):
        with self.assertRaisesRegex(ValueError, 'unknown localization ID'):
            loc.Catalog.load(self.root).render('localization::Tr("resource.unknown")')

    def test_dynamic_id_is_fatal(self):
        for value in ('id', '"resource.gold", count', '"resource." "gold"'):
            with self.subTest(value=value), self.assertRaisesRegex(ValueError, 'one literal'):
                loc.Catalog.load(self.root).render('localization::Tr(' + value + ')')

    def test_comments_and_quoted_code_are_not_calls(self):
        text = '// localization::Tr(id)\n/* localization::Tr(id) */\n"localization::Tr(id)"'
        self.assertEqual(loc.Catalog.load(self.root).render(text), text)

    def test_generated_view_preserves_utf8_offsets_and_lines(self):
        catalog = loc.Catalog.load(self.root)
        text = '// Русский комментарий\na=localization :: Tr(\n "resource.gold"\n);\nDATA(123) int n;'
        generated = catalog.render(text)
        self.assertEqual(len(text.encode()), len(generated.encode()))
        self.assertEqual([i for i, b in enumerate(text.encode()) if b == 10],
                         [i for i, b in enumerate(generated.encode()) if b == 10])
        self.assertEqual(text.encode().index(b'DATA'), generated.encode().index(b'DATA'))
        self.assertIn(catalog.macro('resource.gold'), generated)

    def test_expansion_preserves_packed_storage_nuls(self):
        self.write_catalog('Gold', 'Золото')
        text = 'localization::Tr("resource.gold") "\\0\\0" localization::Tr("resource.gold")'
        rendered = loc.Catalog.load(self.root).render(text, expanded=True)
        value = ''.join(ast.literal_eval(t.group()) for t in loc.tokens(rendered))
        self.assertEqual(value.encode('latin1'), 'Золото\0\0Золото'.encode('cp1251'))

    def test_stale_po_source_is_fatal(self):
        path = self.root / 'locales/ru.po'
        path.write_text(path.read_text().replace('Gold: %s %d', 'Changed: %s %d'))
        with self.assertRaisesRegex(ValueError, 'stale'):
            loc.Catalog.load(self.root)

    def test_catalog_cannot_hide_unicode_in_escapes(self):
        path = self.root / 'locales/ru.po'
        path.write_text(path.read_text().replace('З', r'\u0417'))
        with self.assertRaisesRegex(ValueError, 'numeric text escape'):
            loc.Catalog.load(self.root)

    def test_header_localization_reaches_both_compilers(self):
        source = self.root / 'src/test.cpp'
        source.write_text('#include <outer.h>\n')
        (self.root / 'include/outer.h').write_text('#include "inner.h"\n')
        (self.root / 'include/inner.h').write_text('char* text = localization::Tr("resource.gold");\n')
        compiled, header, overlay, _ = loc.prepare(self.root, source)
        self.assertNotEqual(compiled, source)
        self.assertIn('H2L', (header.parent / 'include/inner.h').read_text())
        self.assertTrue((header.parent / 'include/outer.h').is_file())
        self.assertTrue(any(e['name'].endswith('/inner.h') for e in json.loads(overlay.read_text())['roots']))

    def test_missing_translation_is_fatal(self):
        self.write_catalog(russian='')
        with self.assertRaisesRegex(ValueError, 'missing Russian translation'):
            loc.Catalog.load(self.root)

    def test_placeholder_mismatch_is_fatal(self):
        for ru in ('Золото: %s', 'Золото: %d %s', 'Золото: %s %ld', 'Золото: %s %d %d'):
            with self.subTest(ru=ru):
                self.write_catalog(russian=ru)
                with self.assertRaisesRegex(ValueError, 'placeholders differ'):
                    loc.Catalog.load(self.root)

    def test_stars_and_length_modifiers(self):
        self.assertEqual(loc.format_signature('%*.*s %ld %%'), ['*', '*', 's', 'ld'])
        self.assertEqual(loc.format_signature('50% protection and 10% of the cost'), [])

    def test_duplicate_ids_and_fuzzy_translations_are_fatal(self):
        path = self.root / 'locales/messages.def'
        path.write_text(path.read_text() * 2)
        with self.assertRaisesRegex(ValueError, 'duplicate'):
            loc.Catalog.load(self.root)
        self.write_catalog()
        po = self.root / 'locales/ru.po'
        po.write_text(po.read_text().replace('msgctxt', '#, fuzzy\nmsgctxt'))
        with self.assertRaisesRegex(ValueError, 'fuzzy'):
            loc.Catalog.load(self.root)

    def test_all_hidden_text_spellings_are_rejected(self):
        for source in (r'"\xcf\xf0"', r'"\317\360"', r'"\u041f"',
                       r'"\U0000041f"', r'"\x41"', r'"\x0a"', r"'\xcf'", '"Привет"'):
            with self.subTest(source=source):
                self.assertTrue(loc.hidden_text_errors(source))

    def test_controls_comments_paths_and_literal_backslashes_are_allowed(self):
        for source in (r'"\n\r\t\b\0"', r'"C:\\Users\\xfile"',
                       r'"\\xCF"', r'// "\xcf"', r'/* "Привет" */', r"'\0'"):
            with self.subTest(source=source):
                self.assertEqual(loc.hidden_text_errors(source), [])

    def test_tree_gate_checks_headers_and_character_literals(self):
        (self.root / 'include/bad.h').write_text(r"char bad = '\317';")
        errors, _ = loc.check_tree(self.root)
        self.assertTrue(any('bad.h:1' in e for e in errors))

    def test_prepare_tracks_catalog_and_source_edits(self):
        source = self.root / 'src/test.cpp'
        source.write_text('char* text = localization::Tr("resource.gold");\n')
        compiled, header, overlay, deps = loc.prepare(self.root, source)
        self.assertEqual(compiled.name, source.name)
        self.assertNotEqual(compiled, source)
        self.assertEqual(len(compiled.read_bytes()), len(source.read_bytes()))
        self.assertEqual(json.loads(overlay.read_text())['roots'][0]['name'], str(source))
        self.assertIn(self.root / 'locales/ru.po', deps)
        before = header.read_bytes()
        self.write_catalog(russian='Монеты: %s %d')
        _, changed, _, _ = loc.prepare(self.root, source)
        self.assertNotEqual(changed.read_bytes(), before)

    def test_effective_hash_input_changes_with_translation(self):
        text = 'localization::Tr("resource.gold")'
        before = loc.Catalog.load(self.root).render(text, expanded=True)
        self.write_catalog(russian='Монеты: %s %d')
        after = loc.Catalog.load(self.root).render(text, expanded=True)
        self.assertNotEqual(before, after)

    def test_english_correction_preserves_compiler_view(self):
        source = 'char* text = localization::Tr("resource.gold");'
        before = loc.Catalog.load(self.root)
        self.write_catalog(english='Original gold: %s %d')
        after = loc.Catalog.load(self.root)
        self.assertNotEqual(before.english, after.english)
        self.assertEqual(before.russian, after.russian)
        self.assertEqual(before.header(), after.header())
        for expanded in (False, True):
            self.assertEqual(before.render(source, expanded=expanded),
                             after.render(source, expanded=expanded))

    def test_original_english_wording_is_retained(self):
        # Pinned source-pol-2.0 provenance is in docs/localization-english-provenance.md.
        english = loc.Catalog.load(Path(__file__).resolve().parents[3]).english
        originals = {
            'combat.luck.bad.buka': 'Bad luck descends on the %s',
            'combat.luck.good.buka': 'Good luck shines on the %s',
            'combat.necromancy.multiple.buka':
                "Practicing the dark arts of necromancy, you are able to raise %d of the enemy's dead to return under your service as Skeletons.",
            'combat.necromancy.one.buka':
                "Practicing the dark arts of necromancy, you are able to raise one of the enemy's dead to return under your service as a Skeleton.",
            'combat.siege.strength_bonus.buka':
                '\n\nThe %s fires with the strength of %d Archers, each with a +%d bonus to their attack skill.',
            'save.filename.new_game': 'NEWGAME',
            'save.filename.player_exit': 'PLYREXIT',
            'town.army.split.buka': 'Move how many %s troops from %s to %s?',
        }
        for key, text in originals.items():
            with self.subTest(key=key):
                self.assertEqual(english[key], text)

    def test_original_english_fits_buka_fragment_arguments(self):
        english = loc.Catalog.load(Path(__file__).resolve().parents[3]).english
        resist = english['combat.spell.resist.buka']
        self.assertEqual(resist % ('', 'Dwarf'), 'The Dwarf resists the spell!')
        self.assertEqual(resist % (english['combat.spell.resist.troop_prefix'], 'Dwarves'),
                         'The troop of Dwarves resists the spell!')
        self.assertEqual('%s %s %s %d %s.' % (
            english['combat.fragment.attack'], 'Goblins',
            english['combat.fragment.does_damage'], 12,
            english['combat.fragment.damage_points']), 'The Goblins do 12 damage.')
        self.assertEqual('%d %s %s.' % (1, 'creature', english['combat.fragment.dies']),
                         '1 creature perishes.')
        self.assertEqual('%d %s %s.' % (2, 'creatures', english['combat.fragment.killed']),
                         '2 creatures perish.')
        # Buka provides both resource names before either quantity. Keep that ABI.
        self.assertEqual(english['trading.bargain.buka'] % (
            'Marketplace', 'wood', 'ore', 2, 'units', 3, 'units'),
            '{Marketplace}\n\nwood for ore: I can offer you 2 units for 3 units.')
        self.assertEqual(english['town.recruit.new_hero'], 'Recruit Hero')
        self.assertEqual(loc.format_signature(english['town.recruit.new_hero']), [])

    def test_original_map_messages_follow_branch_not_legacy_id(self):
        english = loc.Catalog.load(Path(__file__).resolve().parents[3]).english
        # REQUEST.cpp's legacy ID suffixes describe the Russian text, not player count.
        self.assertEqual(english['requester.map.size_mismatch.multiple.buka'] % 1,
                         'No maps exist for 1 human player at that size.')
        self.assertEqual(english['requester.map.size_mismatch.one.buka'] % 2,
                         'No maps exist for 2 human players at that size.')

    def test_retained_function_hash_tracks_its_translation(self):
        from homm2.match import source_hashes as hashes
        source = self.root / 'src/test.cpp'
        source.write_text('VA(0x00400100, 0x10)\nvoid f() { use(localization::Tr("resource.gold")); }\n')
        symbols = self.root / 'build/gen/symbol_names.csv'
        symbols.parent.mkdir(parents=True)
        symbols.write_text('rva,name,unit,size,kind,provenance\n0x100,f,test,0x10,func,test\n')
        with mock.patch.object(hashes, 'REPO', self.root):
            before = hashes.source_hashes()
            self.write_catalog(russian='Монеты: %s %d')
            after = hashes.source_hashes()
        self.assertNotEqual(before['test', 'f'], after['test', 'f'])

    def test_real_tree_has_no_hidden_text(self):
        root = Path(__file__).resolve().parents[3]
        errors, used = loc.check_tree(root)
        self.assertEqual(errors, [])
        self.assertGreater(len(used), 2400)


class ClangLocalizationTests(unittest.TestCase):
    setUp = LocalizationTests.setUp
    write_catalog = LocalizationTests.write_catalog
    def clang_fixture(self):
        root = Path(__file__).resolve().parents[3]
        msvc = root / 'build/toolchain/msvc/include'
        if not msvc.is_dir():
            self.skipTest('real VC6 headers are not installed')
        target = self.root / 'build/toolchain/msvc/include'
        target.parent.mkdir(parents=True)
        target.symlink_to(msvc, target_is_directory=True)
        source = self.root / 'src/check.cpp'
        return source

    def test_clang_checks_actual_crt_format_arguments(self):
        source = self.clang_fixture()
        cases = {
            '"hero", 7': False,
            '7, 7': True,
            '"hero"': True,
            '"hero", 7, 8': True,
        }
        for arguments, must_fail in cases.items():
            source.write_text('#include <stdio.h>\nvoid check() { char out[100]; '
                              'sprintf(out, localization::Tr("resource.gold"), '
                              + arguments + '); }\n')
            errors = loc.check_formats(self.root, source)
            with self.subTest(arguments=arguments):
                self.assertEqual(bool(errors), must_fail, errors)

    def test_clang_keeps_annotation_offsets_and_array_size(self):
        from homm2.build.annotated_data import definitions_for_file
        source = self.clang_fixture()
        self.write_catalog('Gold', 'Золото')
        source.write_text('#define DATA(x)\nDATA(0x00400100) char text[] = '
                          'localization::Tr("resource.gold");\n'
                          'DATA(0x00400200) int value = 7;\n')
        rows = definitions_for_file(source, self.root / 'src', self.root)
        self.assertEqual([(r.name, r.size) for r in rows], [('text', 7), ('value', 4)])

    def test_wrapper_stops_before_vc6_on_bad_format_arguments(self):
        from homm2.build import cc_wrap
        source = self.clang_fixture()
        source.write_text('#include <stdio.h>\nvoid check() { char out[100]; '
                          'sprintf(out, localization::Tr("resource.gold"), 7, 7); }\n')
        with (mock.patch.object(cc_wrap, 'HOMM2_DIR', self.root),
              mock.patch.object(cc_wrap, '_prepare_compile_env', return_value=Path('CL.EXE')),
              mock.patch.object(cc_wrap, '_run_cl') as compiler):
            rc, log, timed_out = cc_wrap.run_compile(source, self.root / 'bad.obj', [])
        self.assertNotEqual(rc, 0)
        self.assertIn('format', log)
        self.assertFalse(timed_out)
        compiler.assert_not_called()


if __name__ == '__main__':
    unittest.main()
