#!/usr/bin/env python3

from pathlib import Path
import shutil
import struct
import subprocess
import sys
import tempfile
import unittest


SCRIPT = Path(__file__).with_name("game_data.py")


def make_agg(path, name):
    """A one-entry resource archive, as agg_manifest reads it."""
    payload = name.encode("ascii")
    entry = struct.pack("<III", 0x1234, 2 + 12, len(payload))
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(struct.pack("<H", 1) + entry + payload
                     + payload + b"\0" * (15 - len(payload)))


class GameDataTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="homm2 game data ")
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.out = self.root / "out"

    def create(self, path, data=b"fixture"):
        result = self.root / path
        result.parent.mkdir(parents=True, exist_ok=True)
        result.write_bytes(data)
        return result

    def run_import(self, game, program=None):
        command = [sys.executable, str(SCRIPT), "--game", str(game), "--out", str(self.out)]
        if program is not None:
            command += ["--program", str(program)]
        return subprocess.run(command, capture_output=True, text=True)

    def disc(self):
        """The Buka disc's unpacked files: the game one folder down."""
        make_agg(self.root / "disc/Data/Data/heroes2.agg", "FONT.ICN")
        make_agg(self.root / "disc/Data/Data/heroes2x.agg", "X.ICN")
        self.create("disc/Data/Maps/Arrax.mx2")
        self.create("disc/Data/Games/________.GM1")
        self.create("disc/Program_Executable_Files/HMM2PL.exe", b"program")
        self.create("disc/Program_DLLs/SMACKW32.DLL")
        return self.root / "disc"

    def test_disc_layout_is_found_and_upper_cased(self):
        result = self.run_import(self.disc(), self.root / "program")
        self.assertEqual(result.returncode, 0, result.stderr)
        game = self.out / "game"
        self.assertEqual(sorted(p.name for p in game.iterdir()), ["DATA", "GAMES", "MAPS"])
        self.assertTrue((game / "DATA/heroes2.agg").is_file())
        self.assertTrue((game / "MAPS/Arrax.mx2").is_file())
        self.assertEqual((self.root / "program/HMM2PL.EXE").read_bytes(), b"program")
        self.assertIn("not one of the tested archives", result.stderr)

    def test_installation_files_and_music(self):
        make_agg(self.root / "gog/DATA/HEROES2.AGG", "FONT.ICN")
        make_agg(self.root / "gog/DATA/HEROES2X.AGG", "X.ICN")
        self.create("gog/MUSIC/Track02.ogg")
        self.create("gog/heroes2/anim/intro.smk")
        self.create("gog/h2camp.txt")
        self.create("gog/HEROES2W.EXE")
        self.create("gog/goggame.dll")
        result = self.run_import(self.root / "gog")
        self.assertEqual(result.returncode, 0, result.stderr)
        game = self.out / "game"
        self.assertEqual(sorted(p.name for p in game.iterdir()),
                         ["DATA", "H2CAMP.TXT", "HEROES2", "MUSIC"])
        self.assertTrue((game / "HEROES2/anim/intro.smk").is_file())
        self.assertIn("no MAPS folder", result.stderr)

    def test_missing_expansion_archive_is_refused(self):
        make_agg(self.root / "game/DATA/HEROES2.AGG", "FONT.ICN")
        result = self.run_import(self.root / "game")
        self.assertEqual(result.returncode, 1)
        self.assertIn("missing DATA/HEROES2X.AGG", result.stderr)
        self.assertFalse(self.out.exists())

    def test_damaged_archive_is_refused(self):
        make_agg(self.root / "game/DATA/HEROES2.AGG", "FONT.ICN")
        self.create("game/DATA/HEROES2X.AGG", b"\x05\x00 not an archive")
        result = self.run_import(self.root / "game")
        self.assertEqual(result.returncode, 1)
        self.assertIn("DATA/HEROES2X.AGG is not a Heroes II archive", result.stderr)

    def test_folder_without_the_game_is_refused(self):
        self.create("empty/readme.txt")
        result = self.run_import(self.root / "empty")
        self.assertEqual(result.returncode, 1)
        self.assertIn("no DATA/HEROES2.AGG", result.stderr)

    @unittest.skipUnless(shutil.which("7z"), "7z is not installed")
    def test_archive_of_the_disc(self):
        disc = self.disc()
        archive = self.root / "packed/heroes2.zip"
        archive.parent.mkdir()
        subprocess.run(["7z", "a", str(archive), "."], cwd=disc, check=True,
                       stdout=subprocess.DEVNULL)
        result = self.run_import(archive.parent, self.root / "program")
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertTrue((self.out / "game/DATA/heroes2x.agg").is_file())
        self.assertTrue((self.root / "program/HMM2PL.EXE").is_file())


if __name__ == "__main__":
    unittest.main()
