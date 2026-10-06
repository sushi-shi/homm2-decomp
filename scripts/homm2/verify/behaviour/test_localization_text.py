"""The shipped text catalog renders the game's sentences."""
import unittest

from homm2.graph import localization as loc
from homm2.core.paths import REPO


class LocalizationTextTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.english = loc.Catalog.load(REPO).english

    def test_original_english_wording_is_retained(self):
        # Pinned source-pol-2.0 provenance is in docs/localization-english-provenance.md.
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
                self.assertEqual(self.english[key], text)

    def test_english_fragments_render_complete_sentences(self):
        english = self.english
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

    def test_map_messages_follow_branch_not_legacy_id(self):
        # REQUEST.cpp's legacy ID suffixes describe the Russian text, not player count.
        self.assertEqual(self.english['requester.map.size_mismatch.multiple.buka'] % 1,
                         'No maps exist for 1 human player at that size.')
        self.assertEqual(self.english['requester.map.size_mismatch.one.buka'] % 2,
                         'No maps exist for 2 human players at that size.')

    def test_tree_has_no_text_outside_the_catalog(self):
        errors, used = loc.check_tree(REPO)
        self.assertEqual(errors, [])
        self.assertGreater(len(used), 2400)


if __name__ == "__main__":
    unittest.main()
