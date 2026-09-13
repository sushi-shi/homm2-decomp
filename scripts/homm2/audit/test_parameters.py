import copy
import csv
import tempfile
import unittest
from pathlib import Path

import clang.cindex as ci

from homm2.audit.parameters import (REVIEW_FIELDS, analyze, assemble, check_review,
                                    domains)
from homm2.build.annotated_data import configure_libclang


class ParameterTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.repo = Path(self.temp.name)
        (self.repo / 'src').mkdir()
        (self.repo / 'include').mkdir()
        configure_libclang()

    def scan(self, text, name='test.cpp'):
        path = self.repo / 'src' / name
        path.write_text(text)
        tu = ci.Index.create().parse(str(path), args=['-x', 'c++', '-std=c++20'])
        self.assertEqual([str(d) for d in tu.diagnostics if d.severity >= ci.Diagnostic.Error], [])
        rows, unowned = analyze(tu, self.repo)
        self.assertEqual(unowned, [])
        return {'source': 'src/' + name, 'parameters': rows,
                'unowned': [], 'files': ['src/' + name], 'errors': [], 'sdk_errors': []}

    def test_overloads_methods_and_declarations_keep_identity(self):
        scan = self.scan('''
struct Hero {};
void visit(Hero *hero); void visit(Hero *target) {}
void visit(int count) {}
struct A { void visit(Hero *hero) {} };
struct B { void visit(Hero *hero) {} };
''')
        report = assemble([scan], self.repo)
        self.assertEqual(len(report['groups']), 4)
        mismatches = [g for g in report['groups'] if g['spelling_difference']]
        self.assertEqual(len(mismatches), 1)
        self.assertEqual(mismatches[0]['names'], ['hero', 'target'])

    def test_pointer_reference_array_callback_and_storage_domains(self):
        scan = self.scan('''
enum class Kind { A };
struct Hero {};
template<class E, class S> class H2EnumStorage {};
void f(Hero *hero, const Hero &other, Hero heroes[],
       Kind (*callback)(Hero *target), H2EnumStorage<Kind, int> kind) {}
''')
        outer = [p for p in scan['parameters'] if p['function'] == 'f']
        self.assertEqual(len(outer), 5)
        self.assertEqual([d['name'] for d in outer[3]['domains']], ['Kind', 'Hero'])
        self.assertEqual(outer[4]['domains'][0]['name'], 'Kind')
        self.assertEqual(outer[4]['domains'][0]['route'], 'value/storage')
        self.assertTrue(any(p['callback'] and p['name'] == 'target' for p in scan['parameters']))

    def test_callback_typedef_and_dependent_template_are_counted(self):
        scan = self.scan('''
struct Hero {};
typedef int (*Handler)(Hero *hero);
template<class T> void f(T value) {}
''')
        self.assertEqual(len(scan['parameters']), 2)
        self.assertEqual(scan['parameters'][0]['function'], 'Handler')
        self.assertFalse(scan['parameters'][1]['domains'])

    def test_macro_sites_are_not_silently_lost(self):
        scan = self.scan('''
enum class Kind { A };
#define OPS(T) void f(T a) {} void f(T a, int b) {}
OPS(Kind)
''')
        report = assemble([scan], self.repo)
        self.assertEqual(len(report['parameters']), 3)
        self.assertEqual(len(report['groups']), 3)

    def test_internal_functions_do_not_merge_across_units(self):
        scans = [self.scan('struct Hero {}; static void f(Hero *hero) {}', name)
                 for name in ('a.cpp', 'b.cpp')]
        report = assemble(scans, self.repo)
        self.assertEqual(len(report['groups']), 2)

    def test_repeated_header_observations_deduplicate(self):
        scan = self.scan('struct Hero {}; void f(Hero *hero);')
        other = copy.deepcopy(scan)
        other['source'] = 'src/other.cpp'
        report = assemble([scan, other], self.repo)
        self.assertEqual(len(report['parameters']), 1)
        self.assertEqual(report['parameters'][0]['units'], ['src/test.cpp', 'src/other.cpp'])

    def test_missing_name_is_distinct_from_conflicting_name(self):
        report = assemble([self.scan('struct Hero {}; void f(Hero *); void f(Hero *hero) {}')], self.repo)
        self.assertFalse(report['groups'][0]['spelling_difference'])
        self.assertTrue(report['groups'][0]['unnamed_sites'])

    def test_unseen_headers_and_sources_are_reported(self):
        (self.repo / 'include/unused.h').write_text('')
        (self.repo / 'src/unused.cpp').write_text('')
        report = assemble([self.scan('void f(int value) {}')], self.repo)
        self.assertEqual(report['coverage']['unseen_headers'], ['include/unused.h'])
        self.assertEqual(report['coverage']['unselected_sources'], ['src/unused.cpp'])

    def test_review_rejects_missing_duplicate_stale_and_empty_reasons(self):
        report = assemble([self.scan('struct Hero {}; void f(Hero *hero) {}')], self.repo)
        row = report['parameters'][0]
        path = self.repo / 'review.tsv'
        values = [row[k] for k in REVIEW_FIELDS[:4]]
        with path.open('w') as stream:
            writer = csv.writer(stream, delimiter='\t')
            writer.writerow(REVIEW_FIELDS)
            writer.writerow(values + ['retain', 'Target hero in this interface.'])
        self.assertEqual(check_review(report, path), [])
        with path.open('a') as stream:
            csv.writer(stream, delimiter='\t').writerow(values + ['retain', ''])
        errors = check_review(report, path)
        self.assertTrue(any('duplicate' in e for e in errors))
        self.assertTrue(any('invalid disposition' in e for e in errors))
        row['name'] = 'target'
        errors = check_review(report, path)
        self.assertTrue(any('stale' in e for e in errors))
        self.assertTrue(any('missing' in e for e in errors))

    def test_invalid_review_schema_fails(self):
        path = self.repo / 'review.tsv'
        path.write_text('name\treason\n')
        with self.assertRaisesRegex(ValueError, 'columns'):
            check_review({'parameters': []}, path)
