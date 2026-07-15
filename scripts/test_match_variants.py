import unittest
from unittest.mock import patch

import match_variants


class MatchVariantsFrontendTests(unittest.TestCase):
    def test_frontend_supplies_its_program_name_and_help(self):
        with patch("match_variants.generate_main", return_value=0) as generate_main:
            self.assertEqual(match_variants.main(["--help"]), 0)
        generate_main.assert_called_once_with(
            ["--help"], prog="match_variants.py", description=match_variants.__doc__
        )


if __name__ == "__main__":
    unittest.main()
