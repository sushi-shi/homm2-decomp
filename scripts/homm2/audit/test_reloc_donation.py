import unittest

from homm2.audit.reloc_donation import is_donatable_owner


class DonatableOwnerTests(unittest.TestCase):
    def test_durable_storage_names_are_accepted(self):
        self.assertTrue(is_donatable_owner("?globalValue@@3HA"))
        self.assertTrue(is_donatable_owner(
            "??_B?1???id@?$ctype@G@std@@$D@@9@51"))

    def test_all_dollar_prefixed_compiler_labels_are_rejected(self):
        self.assertFalse(is_donatable_owner("$continue_route$60490"))
        self.assertFalse(is_donatable_owner("$L123"))
        self.assertFalse(is_donatable_owner("$SG456"))

    def test_literal_and_exception_artifacts_are_rejected(self):
        for name in (
                "__real@3f800000", "??_C@_00@A@", "__ehhandler$foo",
                "__unwindfunclet$foo", "__catch$foo"):
            with self.subTest(name=name):
                self.assertFalse(is_donatable_owner(name))
        self.assertFalse(is_donatable_owner(None))


if __name__ == "__main__":
    unittest.main()
