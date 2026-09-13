import struct
import unittest

from homm2.audit.symbols import census


def pe():
    data = bytearray(512)
    data[:2] = b'MZ'
    struct.pack_into('<I', data, 0x3c, 0x80)
    data[0x80:0x84] = b'PE\0\0'
    struct.pack_into('<H', data, 0x86, 1)
    struct.pack_into('<H', data, 0x94, 224)
    struct.pack_into('<H', data, 0x98, 0x10b)
    struct.pack_into('<I', data, 0x98 + 28, 0x400000)
    struct.pack_into('<I', data, 0x98 + 92, 16)
    section = 0x98 + 224
    data[section:section + 5] = b'.text'
    struct.pack_into('<IIII', data, section + 8, 0x1000, 0x1000, 0x100, 0x1c0)
    return data


def nb09():
    data = pe()
    base = len(data)
    record_body = struct.pack('<IHHB', 0x20, 1, 0, 4) + b'?f@@'
    record = struct.pack('<HH', len(record_body) + 2, 0x203) + record_body
    public = struct.pack('<HHIII', 0, 0, len(record), 0, 0) + record
    types = struct.pack('<II', 0, 0)
    directory = struct.pack('<HHIII', 16, 12, 2, 0, 0)
    directory += struct.pack('<HHII', 0x12a, 0xffff, 48, len(public))
    directory += struct.pack('<HHII', 0x12b, 0xffff, 48 + len(public), len(types))
    data += b'NB09' + struct.pack('<I', 8) + directory + public + types
    data += b'NB09' + struct.pack('<I', len(data) + 8 - base)
    return data


class SymbolTests(unittest.TestCase):
    def test_stripped_image_has_no_invented_symbols(self):
        result = census(pe())
        self.assertIsNone(result['codeview'])
        self.assertEqual(result['directories']['debug'], {'rva': 0, 'size': 0})

    def test_public_name_address_and_zero_type_index(self):
        result = census(nb09())['codeview']
        self.assertEqual(result['records'], {'S_PUB32': 1})
        self.assertEqual(result['global_type_count'], 0)
        self.assertEqual(result['publics'][0]['rva'], 0x1020)
        self.assertEqual(result['publics'][0]['type_index'], 0)
        self.assertEqual(result['publics'][0]['name'], '?f@@')
        self.assertNotIn('length', result['publics'][0])

    def test_bad_back_pointer_rejected(self):
        data = nb09()
        struct.pack_into('<I', data, len(data) - 4, len(data) + 1)
        with self.assertRaises(ValueError):
            census(data)

    def test_truncated_public_record_rejected(self):
        data = nb09()
        struct.pack_into('<H', data, 512 + 48 + 16, 0xffff)
        with self.assertRaisesRegex(ValueError, 'truncated CodeView'):
            census(data)

    def test_embedded_string_is_not_a_debug_stream(self):
        data = pe() + b'NB09hello world'
        self.assertIsNone(census(data)['codeview'])

    def test_nb10_is_a_pdb_reference_not_embedded_publics(self):
        data = pe()
        struct.pack_into('<II', data, 0x98 + 96 + 6 * 8, 0x1000, 28)
        payload = b'NB10' + struct.pack('<III', 0, 123, 4) + b'C:\\game.pdb\0'
        struct.pack_into('<IIHHIIII', data, 0x1c0, 0, 0, 0, 0, 2, len(payload), 0, len(data))
        data += payload
        result = census(data)
        self.assertIsNone(result['codeview'])
        self.assertEqual(result['debug_records'][0]['pdb_path'], 'C:\\game.pdb')
        self.assertEqual(result['debug_records'][0]['age'], 4)

    def test_non_pe_and_truncated_pe_fail(self):
        for data in (b'hello', b'MZ', pe()[:100]):
            with self.subTest(size=len(data)), self.assertRaises(ValueError):
                census(data)
