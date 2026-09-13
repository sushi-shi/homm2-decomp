import unittest
from unittest import mock

from homm2 import cli


class BuildCommandTest(unittest.TestCase):
    def setUp(self):
        verifier = mock.patch('homm2.core.retail.verify_retail')
        self.verify_retail = verifier.start()
        self.addCleanup(verifier.stop)

    def test_ordinary_build_bypasses_matching_and_forwards_locale(self):
        for locale in ('--en', '--ru'):
            with self.subTest(locale=locale), mock.patch.object(cli, 'sh', return_value=0) as run:
                self.assertEqual(cli.main(['build', '--no-match', locale, '-j', '4']), 0)
                run.assert_called_once_with('python3', '-m', 'homm2.build.ordinary',
                                            locale, '-j', '4')
        self.verify_retail.assert_not_called()

    def test_english_matching_and_conflicting_locales_rejected_before_build(self):
        for args in (['--en'], ['--en', '--ru'], ['--no-match', '--ru', '--en']):
            with self.subTest(args=args), mock.patch.object(cli, 'sh') as run:
                self.assertEqual(cli.main(['build', *args]), 1)
                run.assert_not_called()
        self.verify_retail.assert_not_called()

    def test_build_help_does_not_require_retail_image(self):
        with mock.patch.object(cli, 'sh') as run:
            self.assertEqual(cli.main(['build', '--help']), 0)
            run.assert_not_called()
        self.verify_retail.assert_not_called()

    def test_matching_rejects_invalid_retail_before_generating_inputs(self):
        self.verify_retail.side_effect = ValueError('invalid retail image')
        with mock.patch.object(cli, 'sh') as run:
            self.assertEqual(cli.main(['build', '--ru']), 1)
            run.assert_not_called()
        self.verify_retail.assert_called_once_with(cli.REPO / 'build/orig/HMM2PL.exe')

    def test_explicit_russian_retains_matching_workflow(self):
        with mock.patch.object(cli, 'sh', return_value=0) as run, mock.patch(
            'homm2.match.status.load_report', return_value={'units': []}
        ), mock.patch('homm2.match.status.main', return_value=0):
            self.assertEqual(cli.main(['build', '--ru', '-j', '4']), 0)
            self.assertIn(mock.call('ninja', '-j', '4'), run.call_args_list)
        self.verify_retail.assert_called_once_with(cli.REPO / 'build/orig/HMM2PL.exe')

    def test_clean_build_generates_report_before_relocation_field_audit(self):
        report_ready = False

        def load_report():
            nonlocal report_ready
            report_ready = True
            return {"units": []}

        def run(*command):
            if command[-2:] == ("homm2.build.assert_relocs", "--fields"):
                self.assertTrue(report_ready)
            return 0

        with mock.patch.object(cli, "sh", side_effect=run), mock.patch(
            "homm2.match.status.load_report", side_effect=load_report
        ), mock.patch("homm2.match.status.main", return_value=0):
            self.assertEqual(cli.main(["build"]), 0)


class LinkCommandTest(unittest.TestCase):
    def run_link(self, *arguments):
        commands = []

        def run(*command):
            commands.append(command)
            return 0

        with mock.patch.object(cli, "sh", side_effect=run):
            self.assertEqual(cli.main(["link", *arguments]), 0)
        return commands

    def test_generic_builds_only_manifest_backed_inputs(self):
        self.assertEqual(
            self.run_link(),
            [
                ("python3", "configure.py"),
                ("ninja", "link-generic-inputs"),
                ("python3", "-m", "homm2.build.exact_link.plain"),
            ],
        )

    def test_resource_mode_adds_only_the_resource_edge(self):
        self.assertEqual(
            self.run_link("--rsrc"),
            [
                ("python3", "configure.py"),
                ("ninja", "link-generic-inputs", "link-resources"),
                ("python3", "-m", "homm2.build.exact_link.plain", "--rsrc"),
            ],
        )

    def test_transform_uses_the_retail_exact_graph(self):
        self.assertEqual(
            self.run_link("--transform"),
            [("python3", "configure.py"), ("ninja", "link")],
        )


if __name__ == "__main__":
    unittest.main()
