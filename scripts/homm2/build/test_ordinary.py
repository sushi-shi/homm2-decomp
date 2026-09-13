"""Ordinary locale builds must never enter the retail/matching pipeline."""
import unittest
from unittest import mock

from homm2.build import cc_wrap, ordinary
from homm2.core.manifest import load


class OrdinaryBuildTests(unittest.TestCase):
    def test_all_edges_are_isolated_and_source_backed(self):
        for locale in ('ru', 'en'):
            graph = ordinary.graph(load(), locale).replace('$\n', '')
            edges = [line for line in graph.splitlines() if line.startswith('build ')]
            self.assertEqual(len(edges), 107)
            for edge in edges:
                self.assertTrue(edge.startswith(f'build build/ordinary/{locale}/'), edge)
                for forbidden in ('build/orig/', 'build/gen/', 'build/objdiff/',
                                  'build/delink/', 'build/link/', 'normalize', 'link_exe'):
                    self.assertNotIn(forbidden, edge)
            self.assertIn(f'default build/ordinary/{locale}/HMM2PL.exe', graph)
            self.assertIn('ordinary_link', graph)
            self.assertIn('/NODEFAULTLIB:LIBCMT', graph)
            self.assertIn('ml_omf src/BASE/BITS.asm', graph)

    def test_english_is_an_explicit_compile_flag(self):
        self.assertIn('--locale en -- $flags',
                      ' '.join(ordinary.graph(load(), 'en').replace('$\n', '').split()))
        self.assertNotIn('--locale en', ordinary.graph(load(), 'ru'))
        with self.assertRaises(ValueError):
            ordinary.graph(load(), 'pl')

    def test_cannot_direct_english_compiler_into_matching_outputs(self):
        with mock.patch.object(cc_wrap, '_prepare_compile_env') as prepare:
            with self.assertRaisesRegex(ValueError, 'matching objects'):
                cc_wrap.run_compile('src/SOURCE/GAME.cpp',
                                    cc_wrap.HOMM2_DIR / 'build/objdiff/base/SOURCE/GAME.obj',
                                    [], locale='en')
            prepare.assert_not_called()

    def test_ordinary_cli_rejects_arbitrary_ninja_graphs_and_targets(self):
        for args in (['-f', 'build.ninja'], ['link'], ['--en', '--ru'], ['-j', '-1']):
            with self.subTest(args=args), mock.patch.object(ordinary.subprocess, 'run') as run:
                with self.assertRaises(SystemExit):
                    ordinary.main(args)
                run.assert_not_called()
