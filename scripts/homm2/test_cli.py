import unittest
from unittest import mock

from homm2 import cli


class BuildCommandTest(unittest.TestCase):
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
