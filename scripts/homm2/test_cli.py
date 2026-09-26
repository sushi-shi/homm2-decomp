import unittest
from unittest import mock

from homm2 import cli


class BuildCommandTest(unittest.TestCase):
    def test_ordinary_build_bypasses_matching_and_forwards_locale(self):
        for locale in ('--en', '--ru'):
            with self.subTest(locale=locale), mock.patch.object(cli, 'sh', return_value=0) as run:
                self.assertEqual(cli.main(['build', '--no-match', locale, '-j', '4']), 0)
                run.assert_called_once_with('python3', '-m', 'homm2.build.ordinary',
                                            locale, '-j', '4')

    def test_english_matching_and_conflicting_locales_rejected_before_build(self):
        for args in (['--en'], ['--en', '--ru'], ['--no-match', '--ru', '--en']):
            with self.subTest(args=args), mock.patch.object(cli, 'sh') as run:
                self.assertEqual(cli.main(['build', *args]), 1)
                run.assert_not_called()

    def test_explicit_russian_retains_matching_workflow(self):
        with mock.patch.object(cli, 'sh', return_value=0) as run, mock.patch(
            'homm2.match.status.load_report', return_value={'units': []}
        ), mock.patch('homm2.match.status.main', return_value=0):
            self.assertEqual(cli.main(['build', '--ru', '-j', '4']), 0)
            self.assertIn(mock.call('ninja', '-j', '4'), run.call_args_list)

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

    def test_generic_uses_native_ninja_link(self):
        self.assertEqual(self.run_link(),
                         [("python3", "configure.py"), ("ninja", "link")])

    def test_resource_mode_uses_native_resource_link(self):
        self.assertEqual(self.run_link("--rsrc"),
                         [("python3", "configure.py"), ("ninja", "link-rsrc")])

    def test_historical_mode_uses_native_history_link(self):
        self.assertEqual(self.run_link("--historical"),
                         [("python3", "configure.py"), ("ninja", "link-historical")])

    def test_removed_transform_and_unknown_options_are_rejected_before_build(self):
        for args in (["--transform"], ["--rsrc", "--transform"], ["--unknown"]):
            with self.subTest(args=args), mock.patch.object(cli, "sh") as run:
                self.assertEqual(cli.main(["link", *args]), 1)
                run.assert_not_called()

    def test_link_help_does_not_build(self):
        with mock.patch.object(cli, "sh") as run:
            self.assertEqual(cli.main(["link", "--help"]), 0)
            run.assert_not_called()


if __name__ == "__main__":
    unittest.main()
